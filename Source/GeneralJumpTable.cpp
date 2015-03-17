#include "Include/stdafx.h"
#include "Include/GeneralJumpTable.h"
#include "Include/GeneralPushDownAutoMachine.h"
namespace ztl
{
	namespace general_parser
	{
		GeneralJumpTable::GeneralJumpTable(PushDownAutoMachine * _machine)
			:machine(_machine),
			createdNodeRequiresMap(make_shared<unordered_map<PDAEdge*, vector<CreateInfo>>>()),
			jumpTable(make_shared<unordered_map<int, vector<JumpItem>>>()),
			ruleRequiresMap(make_shared<unordered_map<PDAEdge*, vector<wstring>>>()),
			terminateMap(make_shared<unordered_map<PDAEdge*, wstring>>()),
			enterRuleMap(make_shared<unordered_map<PDAEdge*, wstring>>())
		{
		}
		SymbolManager* GeneralJumpTable::GetSymbolManager()const
		{
			return machine->GetSymbolManager();
		}
		PDANode* GeneralJumpTable::GetRoot()const
		{
			return machine->GetRoot();
		}

		void GeneralJumpTable::CreateJumpTable()
		{
			unordered_set<PDAEdge*> sign;
			deque<PDANode*> queue;
			queue.emplace_back(GetRoot());
			while(!queue.empty())
			{
				auto front = queue.front();
				queue.pop_front();
				jumpTable->insert(make_pair(front->GetNumber(), CreateJumpItem(front, sign, queue)));
			}
		}
		vector<JumpItem> GeneralJumpTable::CreateJumpItem(PDANode* source, unordered_set<PDAEdge*>& sign, deque<PDANode*>& queue)
		{
			vector<JumpItem > result;
			for(PDAEdge* edgeIter : source->GetNexts())
			{
				if(sign.find(edgeIter) == sign.end())
				{
					sign.insert(edgeIter);
					assert(edgeIter->GetActions().size() >= 1);
					result.emplace_back(edgeIter->GetTarget()->GetNumber(), edgeIter);
					CacheEdgeInfo(edgeIter);
					queue.emplace_back(edgeIter->GetTarget());
				}
			}
			return result;
		}
		void GeneralJumpTable::CacheCreatedNodeRequiresMap(PDAEdge * edge,const vector<ActionWrap>& nodeStack, vector<CreateInfo>& createInfos)
		{
			find_if(nodeStack.begin(), nodeStack.end(), [&createInfos](const ActionWrap& wrap)
			{
				if(wrap.GetActionType() == ActionType::Create)
				{
					return true;
				}
				else
				{
					createInfos.emplace_back(wrap.GetValue(), wrap.GetName());
					return false;
				}
			});
			this->createdNodeRequiresMap->insert(make_pair(edge, createInfos));
		}
		void GeneralJumpTable::CacheRuleRequiresMap(PDAEdge* edge, const vector< ActionWrap>& ruleStack, vector<wstring>&ruleInfos)
		{
			find_if(ruleStack.begin(), ruleStack.end(), [flag = true, &ruleInfos](const ActionWrap& wrap)mutable
			{
				if(wrap.GetActionType() == ActionType::Shift)
				{
					return true;
				}
				else if(flag == false)
				{
					flag = true;
					ruleInfos.emplace_back(wrap.GetName());
				}
				else
				{
					ruleInfos.emplace_back(wrap.GetValue());
				}
				return false;
			});
			this->ruleRequiresMap->insert({ edge,ruleInfos });
		}
		void GeneralJumpTable::CacheEnterRuleMap(PDAEdge* edge)
		{
			assert(!edge->GetActions().empty());
			auto actions = edge->GetActions();
			auto first = actions.front();
			assert(first.GetActionType() != ActionType::Setter);
			assert(first.GetActionType() != ActionType::Assign);

			ActionType type = first.GetActionType();
			wstring name;
			switch(type)
			{
				
				case ztl::general_parser::ActionType::Shift:
					name = first.GetName();
					break;
				case ztl::general_parser::ActionType::Reduce:
					name = first.GetName();

					break;
				case ztl::general_parser::ActionType::Terminate:
					name = first.GetValue();
					break;
				case ztl::general_parser::ActionType::Create:
					name = first.GetValue();
					break;
				case ztl::general_parser::ActionType::Assign:
				case ztl::general_parser::ActionType::NonTerminate:
				case ztl::general_parser::ActionType::Epsilon:
				case ztl::general_parser::ActionType::Setter:
				case ztl::general_parser::ActionType::Using:
				default:
					assert(false);
					break;
			}
			this->enterRuleMap->insert({ edge,name });

		}
		void GeneralJumpTable::CacheEdgeInfo(PDAEdge * edge)
		{
			auto&& actions = edge->GetActions();

			assert(!actions.empty());
			assert(accumulate(actions.begin(), actions.end(), 0, [](int sum, const ActionWrap& wrap)
			{
				return sum + (wrap.GetActionType() == ActionType::Terminate);
			}) == 1);
			assert(find_if(actions.begin(), actions.end(), [](const ActionWrap& wrap)
			{
				return wrap.GetActionType() == ActionType::Epsilon ||
					wrap.GetActionType() == ActionType::NonTerminate ||
					wrap.GetActionType() == ActionType::Using;
			}) == actions.end());
			//assgin 前面必定是reduce或者terminate
			assert(find_if_not(actions.begin() + 1, actions.end(), [last = actions.begin()](const ActionWrap& wrap)mutable
			{
				bool result = true;
				if(wrap.GetActionType() == ActionType::Assign)
				{
					result = last->GetActionType() == ActionType::Terminate ||
								  last->GetActionType() == ActionType::Reduce;
				}
				++last;
				return result;

			}) == actions.end());

			vector<CreateInfo> createInfos;
			vector<wstring> ruleInfos;
			vector<ActionWrap> nodeStack;
			vector<ActionWrap> ruleStack;
			for(size_t i = 0; i < actions.size(); ++i)
			{
				auto&&iter = actions[i];
				switch(iter.GetActionType())
				{
					case ztl::general_parser::ActionType::Epsilon:
					case ztl::general_parser::ActionType::Using:
					case ztl::general_parser::ActionType::NonTerminate:
						assert(false);
						break;

					case ztl::general_parser::ActionType::Shift:
						ruleStack.emplace_back(iter);
						break;
					case ztl::general_parser::ActionType::Reduce:
						if(!ruleStack.empty() && ruleStack.back().GetActionType() == ActionType::Shift)
						{
							ruleStack.pop_back();
						}
						else
						{
							ruleStack.emplace_back(iter);
						}
						break;

					case ztl::general_parser::ActionType::Terminate:
						this->terminateMap->insert({ edge,iter.GetName() });
						break;

					case ztl::general_parser::ActionType::Create:
						nodeStack.emplace_back(iter);
						break;
					case ztl::general_parser::ActionType::Assign:
						if(!nodeStack.empty() &&
							actions[i - 1].GetActionType() != ActionType::Terminate&&
							nodeStack.back().GetActionType() == ActionType::Create)
						{
							nodeStack.pop_back();
						}
						else if(actions[i - 1].GetActionType() != ActionType::Terminate)
						{
							nodeStack.emplace_back(iter);
						}
						break;

					case ztl::general_parser::ActionType::Setter:
					default:
						break;
				}
			}
			CacheCreatedNodeRequiresMap(edge, nodeStack, createInfos);
			CacheRuleRequiresMap(edge, ruleStack, ruleInfos);
			CacheEnterRuleMap(edge);
		}

		void GeneralJumpTable::ClearJumpTable()
		{
			jumpTable->clear();
		}

		const unordered_map<int, vector<JumpItem>>& GeneralJumpTable::GetJumpTable() const
		{
			assert(!jumpTable->empty());
			return *jumpTable;
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
				ActionType::Using
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
			std::for_each(wrapList.begin(), wrapList.end(), [&result, &actionMap](auto&& element)
			{
				result.emplace_back(L"action:" + actionMap[element.GetActionType()](element) + L";");
			});
			return result;
		}
		void LogJumpTable(wstring fileName, GeneralJumpTable& jumpTable)
		{
			auto actionMap = InitActionTypeAndGrammarLogMap();
			auto&& table = jumpTable.GetJumpTable();
			wofstream output(fileName);
			for(auto rowsIter : table)
			{
				auto&& nodeIndex = rowsIter.first;
				auto&&  edges = rowsIter.second;
				sort(edges.begin(), edges.end(), [](const JumpItem& left, const JumpItem& right)
				{
					return left.targetIndex < right.targetIndex;
				});
				output << L"NodeIndex:" + to_wstring(nodeIndex) << endl;
				for(auto&& colsIter : edges)
				{
					auto&& targetNodeIndex = colsIter.targetIndex;
					output << L" targetIndex: " << to_wstring(targetNodeIndex) << endl;

					auto&& actionWrapList = colsIter.edges->GetActions();
					auto&& actionWrapStringList = ActionWrapStringList(actionWrapList, actionMap);
					for(auto&& iter : actionWrapStringList)
					{
						output << L"			" << iter << endl;
					}
				}
			}
		}
		void CreateJumpTable(GeneralJumpTable & jumpTable)
		{
			jumpTable.CreateJumpTable();
		}
		void HelpLogJumpTable(wstring fileName, GeneralJumpTable& jumpTable)
		{
			jumpTable.ClearJumpTable();
			CreateJumpTable(jumpTable);
			LogJumpTable(fileName, jumpTable);
			jumpTable.ClearJumpTable();
		}
	}
}