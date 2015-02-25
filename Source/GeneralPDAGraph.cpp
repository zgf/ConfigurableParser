#include "Include/stdafx.h"
#include "Include/GeneralPushDownAutoMachine.h"
#include "Include/GeneralTableDefine.h"
#include "Include/SymbolManager.h"
#include "Include/ParserSymbol.h"
namespace ztl
{
	namespace general_parser
	{

		class CreateEpsilonPDAVisitor: public GeneralGrammarTypeDefine::IVisitor
		{
		public:
			CreateEpsilonPDAVisitor()  = default;
			~CreateEpsilonPDAVisitor() noexcept = default;
			CreateEpsilonPDAVisitor(CreateEpsilonPDAVisitor&&) noexcept = default;
			CreateEpsilonPDAVisitor(const CreateEpsilonPDAVisitor&) noexcept = default;
			CreateEpsilonPDAVisitor& operator=(CreateEpsilonPDAVisitor&&) noexcept = default;
			CreateEpsilonPDAVisitor& operator=(const CreateEpsilonPDAVisitor&)  = default;
			CreateEpsilonPDAVisitor(PushDownAutoMachine* _machine,GeneralRuleDefine* _rule)noexcept
				:machine(_machine),rule(_rule)
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
				if (machine->GetSymbolManager()->GetCacheRuleNameToSymbol(node->name))
				{
					ActionWrap wrap( ActionType::NonTerminate, node->name, L"");

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
				machine->AddEdge(newNode,result.first , move(wrap));
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
		void HelpLogJumpTable(wstring fileName,PushDownAutoMachine& machine)
		{
			machine.CreateJumpTable();
			LogJumpTable(fileName,machine);
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
			for (auto&& ruleIter: machine.GetTable()->rules)
			{
				auto rulePointer = ruleIter.get();
				
				for(auto&& grammarIter : ruleIter->grammars)
				{
					CreateEpsilonPDAVisitor visitor(&machine, rulePointer);
					grammarIter->Accept(&visitor);
					assert(visitor.GetResult().first != visitor.GetResult().second);

					AddGeneratePDA(PDAMap,rulePointer->name,visitor.GetResult().first,visitor.GetResult().second);
				}
			}
			//HelpLogJumpTable(L"LogJumpTable_RawTable.txt", machine);
			ztl::general_parser::MergeGrammarCommonFactor(machine, PDAMap);
			for (auto&& ruleIter:PDAMap)
			{
				assert(ruleIter.second.size() > 0);
				machine.AddGeneratePDA(ruleIter.first, *ruleIter.second.begin());
			}
			machine.InitNodeIndexMap();

			//HelpLogJumpTable(L"LogJumpTable_MergeFactorTable.txt", machine);
			ztl::general_parser::MergeGraph(machine);
			HelpLogJumpTable(L"LogJumpTable_MergeGraphTable.txt", machine);
			

		}
		using lambdaType = wstring(*)(const ActionWrap&);

		unordered_map<ActionType, lambdaType> InitActionTypeAndGrammarLogMap()
		{
			unordered_map<ActionType, lambdaType> actionMap;
			auto functor =  [](const ActionWrap& wrap)->wstring
			{
				return ActionTypeToWString(wrap.GetActionType()) + L" : " + wrap.GetName();
			};
			vector<ActionType> ActionTypeList = {ActionType::Assign,ActionType::Create,ActionType::Epsilon,ActionType::NonTerminate,ActionType::Reduce,ActionType::Setter,ActionType::Shfit,ActionType::Terminate,ActionType::Using};
			for(auto&& iter : ActionTypeList)
			{
				actionMap.insert(make_pair(iter, functor));
			}
			return actionMap;
		}
		vector<wstring> ActionWrapStringList(const deque<ActionWrap>& wrapList)
		{
			vector<wstring> result;
			auto actionMap = InitActionTypeAndGrammarLogMap();
			std::for_each(wrapList.begin(), wrapList.end(), [&result, &actionMap](auto&& element)
			{
				result.emplace_back(L"action:"+actionMap[element.GetActionType()](element) + L";");
			});
			return result;
		}
		void LogJumpTable(wstring fileName, PushDownAutoMachine& machine)
		{
			auto&& table = machine.GetJumpTable();
			auto&& manager = machine.GetSymbolManager();
			wofstream output(fileName);
			for (auto&& rowsIter : table)
			{
				auto&& nodeIndex = rowsIter.first;
				auto ruleName = machine.GetRuleNameOrEmptyByNodeIndex(nodeIndex);
				if (!ruleName.empty())
				{
					output << L"RuleName: " << ruleName<<endl;
				}
				output <<L"NodeIndex:" + to_wstring(nodeIndex)<<endl;
				for (auto&& colsIter:rowsIter.second)
				{
					auto&& tagName = manager->GetCacheNameByTag(colsIter.first);
					auto&& targetNodeIndex = colsIter.second.first;
					output << L"	tagName: " << tagName <<L" targetIndex: "<< to_wstring(targetNodeIndex)<<endl;

					auto&& actionWrapList = colsIter.second.second;
					auto&& actionWrapStringList = ActionWrapStringList(actionWrapList);
					for(auto&& iter : actionWrapStringList)
					{
						output << L"			"<<iter<<endl;
					}
				}
			}
		}
		
		
		//提取左公因子
		class ActionSet
		{
		public:
			set<ActionWrap> actions;
			PDANode*		current;
		};

		void GotoNextNodeFromFront(PushDownAutoMachine& machine, deque<ActionSet>& setList, unordered_set<PDAEdge*>& sign,bool gotoNext)
		{
			if (!setList.empty())
			{
				auto front = move(setList.front());
				setList.pop_front();
				for(auto&& iter : front.current->GetNexts())
				{
					if (sign.find(iter) == sign.end())
					{
						sign.insert(iter);
						setList.emplace_back(ActionSet());
						setList.back().current = iter->GetTarget();
						setList.back().actions = front.actions;
						setList.back().actions.insert(iter->GetActions().begin(), iter->GetActions().end());
					}
					
				}
				MergeGrammarCommonFactor(machine, setList,sign, gotoNext);
			}
		}
		void GotoFrontNodeFromFront(PushDownAutoMachine& machine, deque<ActionSet>& setList, unordered_set<PDAEdge*>& sign,bool gotoNext)
		{
			if(!setList.empty())
			{
				auto front = move(setList.front());
				setList.pop_front();
				for(auto&& iter : front.current->GetFronts())
				{
					if(sign.find(iter) == sign.end())
					{
						sign.insert(iter);
						setList.emplace_back(ActionSet());
						setList.back().current = iter->GetSource();
						setList.back().actions = front.actions;
						setList.back().actions.insert(iter->GetActions().begin(), iter->GetActions().end());
					}

				}
				MergeGrammarCommonFactor(machine, setList, sign, gotoNext);
			}
		}
		void MergeGrammarCommonFactor(PushDownAutoMachine& machine,deque<ActionSet>& setList, unordered_set<PDAEdge*>& sign,bool gotoNext)
		{
			assert(std::accumulate(setList.begin(), setList.end(),size_t(), [](auto&& val, auto&& iter)
			{
				return val + (iter.current == nullptr?0:1);
			})== setList.size());
			if (setList.size()>1)
			{
				sort(setList.begin(), setList.end(), [](auto&& left, auto&& right)
				{
					return (left.actions.size() == right.actions.size()) ?
						left.actions < right.actions :
						left.actions.size() < right.actions.size();
				});
				auto&& target = setList.begin();
				target = std::find_if_not(setList.begin()+1, setList.end(), [&target](auto&&element)
				{
					return element.actions == target->actions;
				});
				
				if (target != setList.begin())
				{
					for_each(setList.begin() + 1, target, [&machine,&setList](auto&&element)
					{
						machine.MergeIndependentNodes(setList.begin()->current, element.current);
					});
					setList.erase(setList.begin()+1, target);
					//合并
				}
			}
			if(gotoNext)
			{
				GotoNextNodeFromFront(machine, setList, sign, gotoNext);
			}
			else
			{
				GotoFrontNodeFromFront(machine, setList, sign, gotoNext);
			}
		}
		void MergeGrammarCommonFactor(PushDownAutoMachine& machine, unordered_map<wstring, vector<pair<PDANode*, PDANode*>>>& PDAMap)
		{
			assert(machine.GetPDAMap().size() == 0);
			for (auto&& ruleIter: PDAMap)
			{
				auto ruleName = ruleIter.first;
				deque<ActionSet> actionSetList;
				unordered_set<PDAEdge*> sign;
				for(auto&& nodeIter : ruleIter.second)
				{
					actionSetList.emplace_back(ActionSet());
					actionSetList.back().current = nodeIter.first;
				}
				MergeGrammarCommonFactor(machine, actionSetList, sign, true);
				
				sign.clear();
				actionSetList.clear();

				for(auto&& nodeIter : ruleIter.second)
				{
					actionSetList.emplace_back(ActionSet());
					actionSetList.back().current = nodeIter.second;
				}
				MergeGrammarCommonFactor(machine, actionSetList, sign, false);
				
				
				
				assert(ruleIter.second.size() != 0);
				if (ruleIter.second.size() != 1)
				{
					ruleIter.second.erase(ruleIter.second.begin() + 1,ruleIter.second.end());
				}
			}
		}

		vector<PDAEdge*> CollectNontermiateEdge(PushDownAutoMachine& machine)
		{
			vector<PDAEdge*> result;
			unordered_set<PDAEdge*> sign;
			for(auto&& ruleIter : machine.GetPDAMap())
			{
				deque<PDANode*> queue;
				queue.emplace_back(ruleIter.second.first);
				while(!queue.empty())
				{
					auto&& front = queue.front();
					for(auto&& edgeIter: front->GetNexts())
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
		void MergeGraph(PushDownAutoMachine& machine)
		{
			auto&& edgeList = CollectNontermiateEdge(machine);
			for(auto&& edgeIter : edgeList)
			{
				auto   index = edgeIter->GetNonTermSymbolIndex();
				assert(index != -1);
				auto&& source = edgeIter->GetSource();
				auto&& target = edgeIter->GetTarget();
				auto actions = edgeIter->GetActions();
				auto&& nonTermiateIter = actions.begin() + index;
				machine.DeleteEdge(edgeIter);
				auto ruleName = nonTermiateIter->GetName();
				auto findIter = machine.GetPDAMap().find(ruleName);
				assert(findIter != machine.GetPDAMap().end());
				actions.erase(nonTermiateIter);
				actions.emplace_back(ActionType::Reduce, L"", L"");
				machine.AddEdge(findIter->second.second, target, actions);
				machine.AddEdge(source, findIter->second.first,ActionWrap(ActionType::Shfit,L"",L""));
			}
		}

		wstring ActionTypeToWString(ActionType type)
		{
			wstring result;
			switch(type)
			{
				case ztl::general_parser::ActionType::Using:
					result = L"Using";
					break;
				case ztl::general_parser::ActionType::Shfit:
					result = L"Shfit";

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