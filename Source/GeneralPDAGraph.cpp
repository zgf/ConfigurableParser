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
				ActionWrap wrap( node, ActionType::Terminate, node->text, L"");
				machine->AddEdge(result.first, result.second, move(wrap));
			}
			void								Visit(GeneralGrammarNormalTypeDefine* node)
			{
				result = machine->NewNodePair();
				if (machine->GetSymbolManager()->GetCacheRuleNameToSymbol(node->name))
				{
					ActionWrap wrap(node, ActionType::NonTerminate, node->name, L"");

					machine->AddEdge(result.first, result.second, move(wrap));
				}
				else
				{
					ActionWrap wrap(node, ActionType::Terminate, node->name, L"");
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
				ActionWrap wrap(node, ActionType::Setter, node->name, node->value);
				machine->EdgeAdditionAction(result.second, move(wrap));
			}
			void								Visit(GeneralGrammarUsingTypeDefine* node)
			{
				node->grammar->Accept(this);
				assert(result.second->GetFronts().size() == 1);
				
				ActionWrap wrap(node, ActionType::Using, machine->GetSymbolManager()->GetCacheUsingGrammarToRuleDefSymbol(node)->GetName(), L"");
				machine->EdgeAdditionAction(result.second, move(wrap));

			}
			void								Visit(GeneralGrammarCreateTypeDefine* node)
			{
				node->grammar->Accept(this);
				auto manager = machine->GetSymbolManager();
				ActionWrap wrap(node, ActionType::Create, FindType(manager, manager->GetGlobalSymbol(), node->type.get())->GetName(), L"");


				//合并的节点可能是因为循环.直接Addition这样的话会导致create在循环内出现多次
				if (result.second->IsMergeNode())
				{
					auto newNode = machine->NewNode();
					machine->AddEdge(result.second, newNode, move(wrap));
					result.second = newNode;
				}
				else
				{
					machine->EdgeAdditionAction(result.second, move(wrap));
				}
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
				ActionWrap wrap(node, ActionType::Assign, node->name, ruleSymbol->GetName());

				node->grammar->Accept(this);
				machine->EdgeAdditionAction(result.second, move(wrap));
			
			}
		private:
			PushDownAutoMachine* machine;
			GeneralRuleDefine* rule;
			pair<PDANode*, PDANode*> result;
		};

		void CreatePDAGraph(PushDownAutoMachine& machine)
		{
			for (auto&& ruleIter: machine.GetTable()->rules)
			{
				auto rulePointer = ruleIter.get();
				
				for(auto&& grammarIter : ruleIter->grammars)
				{
					CreateEpsilonPDAVisitor visitor(&machine, rulePointer);
					grammarIter->Accept(&visitor);
					assert(visitor.GetResult().first != visitor.GetResult().second);
					machine.AddGeneratePDA(rulePointer,visitor.GetResult().first,visitor.GetResult().second);
				}
			}
		}
		using lambdaType = wstring(*)(const ActionWrap&);

		unordered_map<ActionType, lambdaType> InitActionTypeAndGrammarLogMap()
		{
			unordered_map<ActionType, lambdaType> actionMap;

			actionMap.insert({ ActionType::Assign, [](const ActionWrap& wrap)->wstring
			{
				return ActionTypeToWString(wrap.GetActionType()) + L" : " + wrap.GetName();
			} });
			actionMap.insert({ ActionType::Using, [](const ActionWrap& wrap)->wstring
			{
				return ActionTypeToWString(wrap.GetActionType()) + L" : " + wrap.GetName();
			} });
			actionMap.insert({ ActionType::Create, [](const ActionWrap& wrap)->wstring
			{
				return ActionTypeToWString(wrap.GetActionType()) + L" : " + wrap.GetName();
			} });
			actionMap.insert({ ActionType::NonTerminate, [](const ActionWrap& wrap)->wstring
			{
				return ActionTypeToWString(wrap.GetActionType()) + L" : " + wrap.GetName();
			} });
			actionMap.insert({ ActionType::Terminate, [](const ActionWrap& wrap)->wstring
			{
				return ActionTypeToWString(wrap.GetActionType()) + L" : " + wrap.GetName();
			} });
			actionMap.insert({ ActionType::Setter, [](const ActionWrap& wrap)->wstring
			{
				return ActionTypeToWString(wrap.GetActionType()) + L" : " + wrap.GetName() + L" = " + wrap.GetValue();
			} });
			return actionMap;
		}
		vector<wstring> LogPDAGraphNode(unordered_map<ActionType, lambdaType>& actionMap,PDANode* node,const wstring& front,unordered_set<PDAEdge*>& sign)
		{
			vector<wstring> result;
			for(auto&& edgeIter : node->GetNexts())
			{
				if (sign.find(edgeIter) == sign.end())
				{
					sign.insert(edgeIter);

					auto content = std::accumulate(edgeIter->GetActions().begin(), edgeIter->GetActions().end(), front, [&actionMap](auto&& content, auto&& iterVal)
					{
						return content + L"    "+actionMap[iterVal.GetActionType()](iterVal);
					});
					auto resultList = LogPDAGraphNode(actionMap, edgeIter->GetTarget(), content, sign);
					if(resultList.empty())
					{
						result.emplace_back(content);
					}
					else
					{
						for (auto&&iter:resultList)
						{
							result.emplace_back(iter);
						}
					}
				}
			}
			return result;
		}
		
		vector<wstring> PDAGraphToString(PushDownAutoMachine& machine)
		{
			vector<wstring> contentList;
			for (auto&& ruleIter: machine.GetPDAMap())
			{
				auto ruleName = ruleIter.first->name;
				wstring front = L"RuleName: " + ruleName + L" ::= ";
				auto actionMap = InitActionTypeAndGrammarLogMap();
				for(auto&& grammarIter : ruleIter.second)
				{
					unordered_set<PDAEdge*> sign;
					auto resultList = LogPDAGraphNode(actionMap, grammarIter.first, front, sign);
					if (!resultList.empty())
					{
						for(auto&& iter : resultList)
						{
							contentList.emplace_back(iter);
						}
					} 
					else
					{
						assert(false);
					}
					
				}
			}
			return contentList;
		}
		void LogPDAGraph(const wstring& fileName, PushDownAutoMachine& machine)
		{
			auto result = PDAGraphToString(machine);
			wofstream output(fileName);
			for (auto&& iter: result)
			{
				iter += L'\n';
				output.write(iter.c_str(), iter.size());
			}
		}
		//提取左公因子
		class ActionSet
		{
		public:
			set<ActionWrap> actions;
			PDANode*		current;
		};

		void MergeGrammarCommonFactor(PushDownAutoMachine& machine, deque<ActionSet>& setList, unordered_set<PDAEdge*>& sign,bool gotoNext);
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
					auto front = setList.front();
					setList.erase(setList.begin(), target);
					//合并
					setList.push_front(front);
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
		void MergeGrammarCommonFactor(PushDownAutoMachine& machine)
		{
			for (auto&& ruleIter:machine.GetPDAMap())
			{
				auto ruleDef = ruleIter.first;
				if (ruleDef->name == L"Grammar")
				{
					int a = 0;
				}
				deque<ActionSet> actionSetList;
				unordered_set<PDAEdge*> sign;
				for (auto&& nodeIter : ruleIter.second)
				{
					actionSetList.emplace_back(ActionSet());
					actionSetList.back().current = nodeIter.first;
				}
				MergeGrammarCommonFactor(machine, actionSetList, sign,true);
				sign.clear();
				actionSetList.clear();

				/*for(auto&& nodeIter : ruleIter.second)
				{
					actionSetList.emplace_back(ActionSet());
					actionSetList.back().current = nodeIter.second;
				}
				MergeGrammarCommonFactor(machine, actionSetList, sign, false);*/

				assert(ruleIter.second.size() != 0);
				if (ruleIter.second.size()!=1)
				{
					ruleIter.second.erase(ruleIter.second.begin()+1,ruleIter.second.end());
				}
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
				default:
					assert(false);
					break;
			}
			return result;
		}
	
		


	}
}