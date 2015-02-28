#include "Include/stdafx.h"
#include "Include/GeneralPushDownAutoMachine.h"
#include "Include/GeneralTableDefine.h"
#include "Include/SymbolManager.h"
#include "Include/ParserSymbol.h"
#include "../Lib/ZTL/ztl_pair_builder.hpp"

namespace ztl
{
	namespace general_parser
	{
		class CreateEpsilonPDAVisitor: public GeneralGrammarTypeDefine::IVisitor
		{
		public:
			CreateEpsilonPDAVisitor() = default;
			~CreateEpsilonPDAVisitor() noexcept = default;
			CreateEpsilonPDAVisitor(CreateEpsilonPDAVisitor&&) noexcept = default;
			CreateEpsilonPDAVisitor(const CreateEpsilonPDAVisitor&) noexcept = default;
			CreateEpsilonPDAVisitor& operator=(CreateEpsilonPDAVisitor&&) noexcept = default;
			CreateEpsilonPDAVisitor& operator=(const CreateEpsilonPDAVisitor&) = default;
			CreateEpsilonPDAVisitor(PushDownAutoMachine* _machine, GeneralRuleDefine* _rule)noexcept
				:machine(_machine), rule(_rule)
			{
			}
		public:
			auto GetResult()const
			{
				return result;
			}

		public:
			void								Visit(GeneralGrammarTextTypeDefine* node)
			{
				result = machine->NewNodePair();
				ActionWrap wrap(ActionType::Terminate, node->text, L"");
				machine->AddEdge(result.first, result.second, move(wrap));
			}
			void								Visit(GeneralGrammarNormalTypeDefine* node)
			{
				result = machine->NewNodePair();
				if(machine->GetSymbolManager()->GetCacheRuleNameToSymbol(node->name))
				{
					ActionWrap wrap(ActionType::NonTerminate, node->name, L"");

					machine->AddEdge(result.first, result.second, move(wrap));
				}
				else
				{
					ActionWrap wrap(ActionType::Terminate, node->name, L"");
					machine->AddEdge(result.first, result.second, move(wrap));
				}
			}
			void								Visit(GeneralGrammarSequenceTypeDefine* node)
			{
				node->first->Accept(this);
				auto firstResult = this->GetResult();
				node->second->Accept(this);
				auto secondResult = this->GetResult();
				result = machine->AddSequenceLinkNode(firstResult, secondResult);
			}
			void								Visit(GeneralGrammarLoopTypeDefine* node)
			{
				node->grammar->Accept(this);
				result = machine->AddLoopLinkNode(result.first, result.second);
			}
			void								Visit(GeneralGrammarOptionalTypeDefine* node)
			{
				node->grammar->Accept(this);
				result = machine->AddOptionalLinkNode(result.first, result.second);
			}
			void								Visit(GeneralGrammarSetterTypeDefine* node)
			{
				node->grammar->Accept(this);
				ActionWrap wrap(ActionType::Setter, node->name, node->value);
				machine->FrontEdgesAdditionBackAction(result.second, move(wrap));
			}
			void								Visit(GeneralGrammarUsingTypeDefine* node)
			{
				node->grammar->Accept(this);
				assert(result.second->GetFronts().size() == 1);

				ActionWrap wrap(ActionType::Using, machine->GetSymbolManager()->GetCacheUsingGrammarToRuleDefSymbol(node)->GetName(), L"");
				machine->FrontEdgesAdditionBackAction(result.second, move(wrap));
			}
			void								Visit(GeneralGrammarCreateTypeDefine* node)
			{
				node->grammar->Accept(this);
				auto manager = machine->GetSymbolManager();
				ActionWrap wrap(ActionType::Create, FindType(manager, manager->GetGlobalSymbol(), node->type.get())->GetName(), L"");
				//合并的节点可能是循环.直接Addition这样的话会导致create在循环内出现多次
				auto newNode = machine->NewNode();
				machine->AddEdge(newNode, result.first, move(wrap));
				result.first = newNode;
			}
			void								Visit(GeneralGrammarAlterationTypeDefine* node)
			{
				node->left->Accept(this);
				auto firstResult = this->GetResult();
				node->right->Accept(this);
				auto secondResult = this->GetResult();
				result = machine->AddAlterationLinkNode(firstResult, secondResult);
			}
			void								Visit(GeneralGrammarAssignTypeDefine* node)
			{
				auto ruleSymbol = machine->GetSymbolManager()->GetCacheNormalGrammarToRuleDefSymbol(node->grammar.get());
				ActionWrap wrap(ActionType::Assign, node->name, ruleSymbol->GetName());

				node->grammar->Accept(this);
				machine->FrontEdgesAdditionBackAction(result.second, move(wrap));
			}
		private:
			PushDownAutoMachine* machine;
			GeneralRuleDefine* rule;
			pair<PDANode*, PDANode*> result;
		};
		void HelpLogJumpTable(wstring fileName, PushDownAutoMachine& machine)
		{
			machine.CreateJumpTable();
			LogJumpTable(fileName, machine);
			machine.ClearJumpTable();
		}
		void AddGeneratePDA(unordered_map<wstring, vector<pair<PDANode*, PDANode*>>>&  PDAMap, wstring ruleName, PDANode * start, PDANode* end)
		{
			auto findIter = PDAMap.find(ruleName);
			if(findIter == PDAMap.end())
			{
				PDAMap[ruleName];
			}
			PDAMap[ruleName].emplace_back(make_pair(start, end));
		}
		void CreatePDAGraph(PushDownAutoMachine& machine)
		{
			unordered_map<wstring, vector<pair<PDANode*, PDANode*>>>  PDAMap;
			for(auto&& ruleIter : machine.GetTable()->rules)
			{
				auto rulePointer = ruleIter.get();
				for(auto&& grammarIter : ruleIter->grammars)
				{
					CreateEpsilonPDAVisitor visitor(&machine, rulePointer);
					grammarIter->Accept(&visitor);
					assert(visitor.GetResult().first != visitor.GetResult().second);

					AddGeneratePDA(PDAMap, rulePointer->name, visitor.GetResult().first, visitor.GetResult().second);
				}
			}
			//HelpLogJumpTable(L"LogJumpTable_RawTable.txt", machine);
			MergeStartAndEndNode(machine, PDAMap);

			for(auto&& ruleIter : PDAMap)
			{
				assert(ruleIter.second.size() > 0);
				machine.AddGeneratePDA(ruleIter.first, *ruleIter.second.begin());
			}
			//HelpLogJumpTable(L"LogJumpTable_MergeFactorTable.txt", machine);

			ztl::general_parser::MergeGraph(machine);
			MergeGrammarCommonFactor(machine);

			//添加结束节点.
			AddFinishNode(machine);

			machine.InitNodeIndexMap();
			HelpLogJumpTable(L"LogJumpTable_MergeGraphTable.txt", machine);
			//machine.CreateJumpTable();
		}
		using lambdaType = wstring(*)(const ActionWrap&);

		unordered_map<ActionType, lambdaType> InitActionTypeAndGrammarLogMap()
		{
			unordered_map<ActionType, lambdaType> actionMap;
			auto functor = [](const ActionWrap& wrap)->wstring
			{
				return ActionTypeToWString(wrap.GetActionType()) + L" : " + wrap.GetName();
			};
			vector<ActionType> ActionTypeList = { ActionType::Assign,ActionType::Create,ActionType::Epsilon,ActionType::NonTerminate,ActionType::Reduce,ActionType::Setter,ActionType::Shift,ActionType::Terminate,ActionType::Using };
			for(auto&& iter : ActionTypeList)
			{
				actionMap.insert(make_pair(iter, functor));
			}
			return actionMap;
		}
		void AddFinishNode(PushDownAutoMachine& machine)
		{
			auto& pdaMap = machine.GetPDAMap();
			assert(!machine.GetSymbolManager()->StartRuleList().empty());
			auto rootRuleName = machine.GetSymbolManager()->StartRuleList()[0];
			assert(machine.GetPDAMap().find(rootRuleName) != machine.GetPDAMap().end());
			pdaMap[rootRuleName].second = machine.AddFinishNodeFollowTarget(pdaMap[rootRuleName].second);
		}
		vector<wstring> ActionWrapStringList(const deque<ActionWrap>& wrapList)
		{
			vector<wstring> result;
			auto actionMap = InitActionTypeAndGrammarLogMap();
			std::for_each(wrapList.begin(), wrapList.end(), [&result, &actionMap](auto&& element)
			{
				result.emplace_back(L"action:" + actionMap[element.GetActionType()](element) + L";");
			});
			return result;
		}
		void LogJumpTable(wstring fileName, PushDownAutoMachine& machine)
		{
			auto&& table = machine.GetJumpTable();
			auto&& manager = machine.GetSymbolManager();
			wofstream output(fileName);
			for(auto&& rowsIter : table)
			{
				auto&& nodeIndex = rowsIter.first;
				auto ruleName = machine.GetRuleNameOrEmptyByNodeIndex(nodeIndex);
				if(!ruleName.empty())
				{
					output << L"RuleName: " << ruleName << endl;
				}
				output << L"NodeIndex:" + to_wstring(nodeIndex) << endl;
				for(auto&& colsIter : rowsIter.second)
				{
					auto&& targetNodeIndex = colsIter.targetIndex;
					output <<L" targetIndex: " << to_wstring(targetNodeIndex) << endl;

					auto&& actionWrapList = colsIter.actions;
					auto&& actionWrapStringList = ActionWrapStringList(actionWrapList);
					for(auto&& iter : actionWrapStringList)
					{
						output << L"			" << iter << endl;
					}
				}
			}
		}

		//可合并节点含义

		//同一个起点,包含同样信息的边,到达不同的节点,那么这个不同节点可以合并成同一个节点
		void MergeNodeByEdge(vector<PDAEdge*>& edges, PushDownAutoMachine& machine)
		{
			assert(!edges.empty());
			assert(is_sorted(edges.begin(), edges.end(), [](const PDAEdge* left, const PDAEdge* right)
			{
				return left->GetActions().size() == right->GetActions().size() ?
					left->GetActions() < right->GetActions() :
					left->GetActions().size() < right->GetActions().size();
			}));

			auto startIter = edges.begin();
			while(startIter != edges.end())
			{
				auto& actions = (*startIter)->GetActions();
				auto endIter = std::find_if_not(startIter, edges.end(), [&actions](const PDAEdge* element)
				{
					return element->GetActions() == actions;
				});
				if(distance(startIter, endIter) > 1)
				{
					auto&& save = (*startIter)->GetTarget();
					for_each(startIter + 1, endIter, [&save, &machine](PDAEdge* element)
					{
						machine.MergeIndependentNodes(save, element->GetTarget());
						machine.DeleteEdge(element);
					});
				}
				startIter = endIter;
			}
		}
		void MergeCommonNode(PDANode* nodeIter, PushDownAutoMachine& machine)
		{
			vector<PDAEdge*> edges(nodeIter->GetNexts().begin(), nodeIter->GetNexts().end());

			sort(edges.begin(), edges.end(), [](const PDAEdge* left, const PDAEdge* right)
			{
				return left->GetActions().size() == right->GetActions().size() ?
					left->GetActions() < right->GetActions() :
					left->GetActions().size() < right->GetActions().size();
			});

			if(!edges.empty())
			{
				MergeNodeByEdge(edges, machine);
			}
		}
		vector<PDANode*> CollectGraphNode(PushDownAutoMachine& machine)
		{
			vector<PDANode*> result;
			unordered_set<PDANode*> sign;
			for(auto&& ruleIter : machine.GetPDAMap())
			{
				deque<PDANode*> queue;
				queue.emplace_back(ruleIter.second.first);
				while(!queue.empty())
				{
					auto&& front = queue.front();
					sign.insert(front);
					if(front->GetNexts().size() > 1)
					{
						result.emplace_back(front);
					}
					for(auto&& edgeIter : front->GetNexts())
					{
						if(sign.find(edgeIter->GetTarget()) == sign.end())
						{
							queue.emplace_back(edgeIter->GetTarget());
						}
					}
					queue.pop_front();
				}
			}
			return result;
		}
		void MergeGrammarCommonFactor(PushDownAutoMachine& machine)
		{
			unordered_set<PDANode*> sign;
			vector<PDAEdge*> edges;
			auto&& nodeList = CollectGraphNode(machine);
			assert(std::accumulate(nodeList.begin(), nodeList.end(), 0, [](int val, const PDANode* element)
			{
				return val + element->GetNexts().size() <= 1;
			}) == 0);

			for(auto&& iter : nodeList)
			{
				if(!iter->GetNexts().empty())
				{
					MergeCommonNode(iter, machine);
				}
			}
		}

		vector<PDAEdge*> CollectNontermiateEdge(PushDownAutoMachine& machine)
		{
			vector<PDAEdge*> result;
			unordered_set<PDAEdge*> sign;
			assert(!machine.GetSymbolManager()->StartRuleList().empty());
			//从Root表达式开始

			for(auto&& iter : machine.GetSymbolManager()->StartRuleList())
			{
				auto&& ruleIter = machine.GetPDAMap()[iter];
				deque<PDANode*> queue;
				queue.emplace_back(ruleIter.first);
				while(!queue.empty())
				{
					auto&& front = queue.front();
					for(auto&& edgeIter : front->GetNexts())
					{
						if(sign.find(edgeIter) == sign.end())
						{
							sign.insert(edgeIter);
							if(edgeIter->GetNonTermSymbolIndex() != -1)
							{
								result.emplace_back(edgeIter);
							}
							queue.emplace_back(edgeIter->GetTarget());
						}
					}
					queue.pop_front();
				}
			}
			return result;
		}
		void MergeStartAndEndNode(PushDownAutoMachine& machine, unordered_map<wstring, vector<pair<PDANode*, PDANode*>>>& PDAMap)
		{
			for(auto&& ruleIter : PDAMap)
			{
				auto& grammars = ruleIter.second;
				assert(!grammars.empty());
				for(size_t i = 0; i < grammars.size(); ++i)
				{
					machine.MergeIndependentNodes(grammars[0].first, grammars[i].first);
					machine.MergeIndependentNodes(grammars[0].second, grammars[i].second);
				}
				grammars.erase(grammars.begin() + 1, grammars.end());
			}
		}
		void MergeGraph(PushDownAutoMachine& machine)
		{
			auto&& edgeList = CollectNontermiateEdge(machine);
			for(auto&& edgeIter : edgeList)
			{
				auto   index = edgeIter->GetNonTermSymbolIndex();
				assert(index != -1);
				assert(index == 0);
				auto&& source = edgeIter->GetSource();
				auto&& target = edgeIter->GetTarget();
				auto actions = edgeIter->GetActions();
				auto&& nonTermiateIter = actions.begin() + index;
				auto nonTerminateName = nonTermiateIter->GetName();

				machine.DeleteEdge(edgeIter);
				auto ruleName = nonTermiateIter->GetName();
				auto findIter = machine.GetPDAMap().find(ruleName);
				assert(findIter != machine.GetPDAMap().end());
				actions.erase(nonTermiateIter);
				actions.emplace_back(ActionType::Reduce, nonTerminateName, L"");
				machine.AddEdge(findIter->second.second, target, actions);
				machine.AddEdge(source, findIter->second.first, ActionWrap(ActionType::Shift, nonTerminateName, L""));
			}
		}
		//合并非终结节点,每个边上都有一个终结节点.
		void MergeNoTermSymbol(PushDownAutoMachine& machine)
		{
		}
		wstring ActionTypeToWString(ActionType type)
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
				default:
					assert(false);
					break;
			}
			return result;
		}
	}
}