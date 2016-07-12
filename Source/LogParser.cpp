#include "Include/stdafx.h"
#include "Include/GeneralGrammarBuilderData.h"
#include "Include/GeneralGrammarBuilder.h"
#include "Include/GeneralLRBuilder.h"
#include "Include/ParserSymbol.h"
namespace ztl
{
	namespace general_parser
	{
		wstring GrammarBuilder::LogPDANode(const wstring & ruleName, PDANode * node) const
		{
			auto findIter = PDAMap.find(ruleName);
			assert(findIter != PDAMap.end());
			auto ruleStart = findIter->second.first;
			auto path = FindThePath(ruleStart, node);
			wstring result;
			for(auto&&iter : path)
			{
				result += iter->GetActions()[0].GetName() + L" ";
			}
			result += L"O";
			return result;
		}
		wstring LRNode::LogProductList(const GeneralLRBuilder & LRMachine) const
		{
			wstring result;
			for(auto&&iter : this->items)
			{
				result += L"		" + iter.LogProduct(LRMachine) + L"\n";
			}
			return result;
		}
		wstring LRNode::LogNextsList() const
		{
			wstring result;
			for(auto&&iter : nexts)
			{
				wstring symbol = iter.first->GetName();
				result += L"		" + to_wstring((int) iter.first->GetType()) + symbol + L" " + to_wstring(iter.second->GetNumber()) + L"\n";
			}
			return result;
		}
		wstring ztl::general_parser::LRNode::LogLRNode(const GeneralLRBuilder & LRMachine) const
		{
			wstring templateString = LR"(
				NodeNumber: $<Number>
				$<ProductList>
				next:
					$<nextList>
				)";
			generator::MarcoGenerator generator(templateString, { L"$<Number>",L"$<ProductList>",L"$<nextList>" });
			auto&& productList = LogProductList(LRMachine);
			auto&& nextLsit = LogNextsList();
			return generator.GenerateText({ to_wstring(number),productList,nextLsit }).GetMacroResult();
		}

		wstring  ActionTypeToWString(ActionType type)
		{
			wstring result;
			switch(type)
			{
				case ztl::general_parser::ActionType::Using:
					result = L"Using";
					break;
				case ztl::general_parser::ActionType::Shift:
					result = L"Shift";

					break;
				case ztl::general_parser::ActionType::Reduce:
					result = L"Reduce";

					break;
				case ztl::general_parser::ActionType::Terminate:
					result = L"Terminate";

					break;
				case ztl::general_parser::ActionType::NonTerminate:
					result = L"NonTerminate";

					break;
				case ztl::general_parser::ActionType::Create:
					result = L"Create";

					break;
				case ztl::general_parser::ActionType::Assign:
					result = L"Assign";

					break;
				case ztl::general_parser::ActionType::Setter:
					result = L"Setter";
					break;
				case ztl::general_parser::ActionType::Epsilon:
					result = L"Epsilon";
					break;
				case ztl::general_parser::ActionType::GrammarBegin:
					result = L"GrammarBegin";
					break;
				default:
					assert(false);
					break;
			}
			return result;
		}
		wstring ztl::general_parser::ProductPosition::LogProduct(const GeneralLRBuilder& LRMachine) const
		{
			wstring templateString = LR"(
				ruleName: $<RuleName>
				product: $<ProductBody>
				followToken: $<FollowTokens>
			    endAction: $<EndActions>)";
			ztl::generator::MarcoGenerator generator(templateString, { L"$<RuleName>",L"$<ProductBody>",L"$<FollowTokens>",L"$<EndActions>" });
			auto&& ruleName = LRMachine.GetRuleNameByIndex(GetRuleIndex());
			auto&& productBody = LRMachine.LogPDANode(ruleName, GetPosition());
			auto&& followTokens = LogFollowToken();
			auto&& endActions = LogEndAction();
			return generator.GenerateText({ ruleName ,productBody ,followTokens ,endActions  }).GetMacroResult();
		}
		wstring ztl::general_parser::ProductPosition::LogFollowToken() const
		{
			wstring result;
			for(auto&&iter : *followToken)
			{
				result += iter->GetName() + L" ";
			}
			return result;
		}
		//Log
		void GeneralLRBuilder::LogItems(const wstring& fileName) const
		{
			wofstream output(fileName);
			wstring result;
			for(auto&&iter : nodePool)
			{
				result += iter->LogLRNode(*this);
			}
			output.write(result.c_str(), result.size());
		}
		wstring GeneralLRBuilder::LogPDANode(const wstring & ruleName, PDANode * node) const
		{
			return machine->LogPDANode(ruleName, node);
		}
	
		wstring ProductPosition::LogEndAction() const
		{
			wstring result;
			if(endAction != nullptr)
			{
				for(auto&&iter : *endAction)
				{
					result += ActionTypeToWString(iter.GetActionType()) + L" " + iter.GetName() + L" ";
				}
			}

			return result;
		}
	

		using lambdaType = wstring(*)(const ActionWrap&);
		unordered_map<ActionType, lambdaType> InitActionTypeAndGrammarLogMap()
		{
			unordered_map<ActionType, lambdaType> actionMap;
			auto functor = [](const ActionWrap& wrap)->wstring
			{
				return ActionTypeToWString(wrap.GetActionType()) + L" : " + wrap.GetName() + L":" + wrap.GetValue();
			};
			vector<ActionType> ActionTypeList = {
				ActionType::Assign,
				ActionType::Create,
				ActionType::Epsilon,
				ActionType::NonTerminate,
				ActionType::Reduce,
				ActionType::Setter,
				ActionType::Shift,
				ActionType::Terminate,
				ActionType::Using,
				ActionType::GrammarBegin,
			};
			for(auto&& iter : ActionTypeList)
			{
				actionMap.insert(make_pair(iter, functor));
			}
			return actionMap;
		}
		vector<wstring> ActionWrapStringList(const vector<ActionWrap>& wrapList, unordered_map<ActionType, lambdaType>& actionMap)
		{
			vector<wstring> result;
			std::for_each(wrapList.begin(), wrapList.end(), [&result, &actionMap](const ActionWrap& element)
			{
				result.emplace_back(L"action:" + actionMap[element.GetActionType()](element) + L";");
			});
			return result;
		}
		void LogGraphInfo(const wstring& fileName, GrammarBuilder& machine)
		{
			unordered_set<PDANode*>sign;
			deque<PDANode*> queue;
			wofstream output(fileName);
			auto actionMap = InitActionTypeAndGrammarLogMap();
			for(auto&&iter : machine.GetPDAMap())
			{
				queue.emplace_back(iter.second.first);
				while(!queue.empty())
				{
					PDANode* front = queue.front();
					queue.pop_front();
					if(sign.find(front) == sign.end())
					{
						sign.insert(front);
						auto edges = front->GetNexts();
						output << L"currenIndex: " << front->GetNumber() << endl;
						for(auto&& colsIter : edges)
						{
							output << L" targetIndex: " << to_wstring(colsIter->GetTarget()->GetNumber()) << endl;

							auto&& actionWrapList = colsIter->GetActions();
							auto&& actionWrapStringList = ActionWrapStringList(actionWrapList, actionMap);
							for(auto&& actionIter : actionWrapStringList)
							{
								output << L"			" << actionIter << endl;
							}
						}
						for(auto&& edgeIter : front->GetNexts())
						{
							queue.emplace_back(edgeIter->GetTarget());
						}
					}
				}
			}

		}
	}
}