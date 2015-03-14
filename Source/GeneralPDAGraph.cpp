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
			CreateEpsilonPDAVisitor(const CreateEpsilonPDAVisitor&)  = default;
			CreateEpsilonPDAVisitor& operator=(CreateEpsilonPDAVisitor&&) noexcept = default;
			CreateEpsilonPDAVisitor& operator=(const CreateEpsilonPDAVisitor&) = default;
			CreateEpsilonPDAVisitor(PushDownAutoMachine* _machine, GeneralRuleDefine* _rule,const wstring& _ruleName)noexcept
				:machine(_machine), rule(_rule),ruleName(_ruleName)
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

				ActionWrap wrap(ActionType::Using, machine->GetSymbolManager()->GetCacheUsingGrammarToRuleDefSymbol(node)->GetName(), ruleName);
				machine->FrontEdgesAdditionBackAction(result.second, move(wrap));
			}
			void								Visit(GeneralGrammarCreateTypeDefine* node)
			{
				node->grammar->Accept(this);
				auto manager = machine->GetSymbolManager();
				ActionWrap wrap(ActionType::Create, FindType(manager, manager->GetGlobalSymbol(), node->type.get())->GetName(), ruleName);
				//合并的节点可能是循环.直接Addition这样的话会导致create在循环内出现多次
				auto newNode = machine->NewNode();
				machine->AddEdge(newNode, result.first, move(wrap));
				result.first = newNode;
				/*	machine->AddEdge(result.second, newNode, move(wrap));
					result.second = newNode;*/
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
			wstring ruleName;
		};

		void AddGeneratePDA(unordered_map<wstring, vector<pair<PDANode*, PDANode*>>>&  PDAMap, wstring ruleName, PDANode * start, PDANode* end)
		{
			auto findIter = PDAMap.find(ruleName);
			if(findIter == PDAMap.end())
			{
				PDAMap[ruleName];
			}
			PDAMap[ruleName].emplace_back(make_pair(start, end));
		}
		unordered_map<wstring, vector<pair<PDANode*, PDANode*>>> CreateEpsilonPDA(PushDownAutoMachine& machine)
		{
			unordered_map<wstring, vector<pair<PDANode*, PDANode*>>>  PDAMap;

			for(auto&& ruleIter : machine.GetTable()->rules)
			{
				auto rulePointer = ruleIter.get();
				for(auto&& grammarIter : ruleIter->grammars)
				{
					CreateEpsilonPDAVisitor visitor(&machine, rulePointer,ruleIter->name);
					grammarIter->Accept(&visitor);
					assert(visitor.GetResult().first != visitor.GetResult().second);

					AddGeneratePDA(PDAMap, rulePointer->name, visitor.GetResult().first, visitor.GetResult().second);
				}
			}
			//HelpLogJumpTable(L"LogJumpTable_RawTable.txt", machine);
			return PDAMap;
		}
		void AddPDAToPDAMachine(PushDownAutoMachine& machine, unordered_map<wstring, vector<pair<PDANode*, PDANode*>>>&  PDAMap)
		{
			for(auto&& ruleIter : PDAMap)
			{
				assert(ruleIter.second.size() > 0);
				machine.AddGeneratePDA(ruleIter.first, *ruleIter.second.begin());
			}
			//HelpLogJumpTable(L"LogJumpTable_MergeFactorTable.txt", machine);
		}

		void AddFinishNode(PushDownAutoMachine& machine)
		{
			auto& pdaMap = machine.GetPDAMap();
			assert(!machine.GetSymbolManager()->GetStartRuleList().empty());
			auto rootRuleName = machine.GetRootRuleName();
			assert(machine.GetPDAMap().find(rootRuleName) != machine.GetPDAMap().end());
			machine.AddFinishNodeFollowTarget(pdaMap[rootRuleName].second);
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

		template<typename prediction_type>
		vector<PDANode*> CollectGraphNode(PushDownAutoMachine& machine, prediction_type pred)
		{
			unordered_set<PDANode*>sign;
			deque<PDANode*> queue;
			vector<PDANode*> result;
			for(auto&&ruleName : machine.GetSymbolManager()->GetStartRuleList())
			{
				auto&& grammarIter = machine.GetPDAMap()[ruleName];
				queue.emplace_back(grammarIter.first);
				while(!queue.empty())
				{
					PDANode* front = queue.front();
					queue.pop_front();
					if(sign.find(front) == sign.end())
					{
						sign.insert(front);
						if(pred(front))
						{
							result.emplace_back(front);
						}
						for(auto&& edgeIter : front->GetNexts())
						{
							queue.emplace_back(edgeIter->GetTarget());
						}
					}
				}
			}
			return result;
		}
		void MergeGrammarCommonFactor(PushDownAutoMachine& machine)
		{
			unordered_set<PDANode*> sign;
			vector<PDAEdge*> edges;
			auto&& nodeList = CollectGraphNode(machine, [](PDANode* element)
			{
				return element->GetNexts().size() > 1;
			});
			assert(std::accumulate(nodeList.begin(), nodeList.end(), 0, [](int val, const PDANode* element)
			{
				return val + element->GetNexts().size() <= 1;
			}) == 0);

			for(size_t i = 0; i < nodeList.size(); ++i)
			{
				auto&& iter = nodeList[i];
				if(!iter->GetNexts().empty())
				{
					MergeCommonNode(iter, machine);
				}
			}
		}
		unordered_map<wstring,vector<PDAEdge*>> CollectNontermiateEdge(PushDownAutoMachine& machine)
		{
			unordered_map<wstring, vector<PDAEdge*>> result;
			unordered_set<PDAEdge*> sign;
			//从Root表达式开始
			for(auto&& iter : machine.GetSymbolManager()->GetStartRuleList())
			{
				result[iter];
			}
			for(auto&& iter : machine.GetSymbolManager()->GetStartRuleList())
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
							if(edgeIter->HasNonTermActionType())
							{
								result[iter].emplace_back(edgeIter);
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
		void DeleteNoTermAndUsingAction(vector<ActionWrap>::iterator nonTermiateIter,vector<ActionWrap>& actions)
		{
			assert(actions[0].GetActionType() == ActionType::NonTerminate);
			if (actions.size() > 1&&actions[1].GetActionType() == ActionType::Using)
			{
				actions.erase(nonTermiateIter,nonTermiateIter + 2);
			}
			else
			{
				actions.erase(nonTermiateIter);
			}
			//nonTerm清除后,nonTerm位置替换为reduce,noterm边上附带的信息放到reduce后面

		}
		void MergeEpsilonPDAGraph(PushDownAutoMachine& machine)
		{
			auto&& edgeList = CollectNontermiateEdge(machine);

			for(auto&& iter : edgeList)
			{
				auto&& ruleName = iter.first;
				for (auto&& edgeIter: iter.second)
				{
					assert(edgeIter->GetActions()[0].GetActionType() == ActionType::NonTerminate);
					auto&& source = edgeIter->GetSource();
					auto&& target = edgeIter->GetTarget();
					auto actions = edgeIter->GetActions();
					auto&& nonTermiateIter = actions.begin();
					auto nonTerminateName = nonTermiateIter->GetName();

					machine.DeleteEdge(edgeIter);
					auto findIter = machine.GetPDAMap().find(nonTerminateName);
					assert(findIter != machine.GetPDAMap().end());
					DeleteNoTermAndUsingAction(nonTermiateIter, actions);
					actions.emplace(actions.begin(), ActionType::Reduce, nonTerminateName, ruleName);
					//清除掉using
					machine.AddEdge(findIter->second.second, target, actions);
					machine.AddEdge(source, findIter->second.first, ActionWrap(ActionType::Shift, ruleName , nonTerminateName));
				}
			}
		}
		void CheckAndDeleteReCursionRing(vector<ActionWrap>& newActions, ActionType type)
		{
			int lastIndex = newActions.size();
			int firstIndex = newActions.size();
			for(int i = newActions.size() - 1; i >= 0; --i)
			{
				if(newActions[i].GetActionType() == type)
				{
					lastIndex = i + 1;
					break;
				}
			}
			firstIndex = find_if(newActions.begin(), newActions.end(), [type](const ActionWrap& wrap)
			{
				return wrap.GetActionType() == type;
			}) - newActions.begin();
			if(firstIndex != lastIndex)
			{
			
				for(auto index = firstIndex; index != lastIndex; ++index)
				{
					auto end = index + 1;
					for(auto i = firstIndex; i < end;++i)
					{
						if (newActions[i].GetName() == newActions[index].GetValue())
						{
							if(i != end)
							{
								newActions.clear();
								return;
								//break;
							/*	newActions.erase(newActions.begin()+i, newActions.begin()+end);
								auto num = end - i;
								firstIndex = end - num;
								lastIndex = lastIndex - num;
								index = index - num;*/
							
							}
						}
					}
					
				}
			}
		}
		void CheckAndDeleteLeftReCursionRing(vector<ActionWrap>& newActions)
		{
			CheckAndDeleteReCursionRing(newActions, ActionType::Shift);
		}
		void CheckAndDeleteRightReCursionRing(vector<ActionWrap>& newActions)
		{
			CheckAndDeleteReCursionRing(newActions, ActionType::Reduce);
		}

		vector<ActionWrap> GetNewAction(vector<PDAEdge*>& save)
		{
			vector<ActionWrap> newActions;
			for_each(save.begin(), save.end(), [&newActions](PDAEdge* iter)
			{
				newActions.insert(newActions.end(), iter->GetActions().begin(), iter->GetActions().end());
			});
			CheckAndDeleteLeftReCursionRing(newActions);
			//不处理右递归,右递归会带上assign信息.走的时候查看语法堆栈和节点就可以知道能否走通了
			CheckAndDeleteRightReCursionRing(newActions);
			return newActions;
		}
		void MergePathSymbol(vector<PDAEdge*>& save, PushDownAutoMachine& machine, unordered_map<PDANode*, int>& edgeCountMap)
		{
			assert(!save.empty());
			auto source = save.front()->GetSource();
			auto target = save.back()->GetTarget();
			auto nexts = source->GetNexts();
			const vector<ActionWrap>* actionPointer = nullptr;
			auto CheckAndAddEdge = [&nexts, &source, &target, &actionPointer, &edgeCountMap, &machine]()
			{
				if(find_if(nexts.begin() + edgeCountMap[source], nexts.end(), [&target, &actionPointer](PDAEdge* edge)
				{
					return edge->GetTarget() == target&&
						edge->GetActions() == *actionPointer;
				}) == nexts.end())
				{
					machine.AddEdge(source, target, *actionPointer);
				}
			};
			if(save.size() == 1)
			{
				assert(edgeCountMap[source] != 0);
				actionPointer = &save[0]->GetActions();
				CheckAndAddEdge();
			}
			else
			{
				vector<ActionWrap> newActions = GetNewAction(save);
				if (!newActions.empty())
				{
					if(find_if(nexts.begin(), nexts.end(), [&newActions](PDAEdge* element)
					{
						return element->GetActions() == newActions;
					}) == nexts.end())
					{
						actionPointer = &newActions;

						CheckAndAddEdge();
					}
				}
			}
		}
		struct Path
		{
			vector<PDAEdge*> edges;
		};
		bool IsTermRingPath(vector<PDAEdge*>& path)
		{
			return find_if(path.begin(), path.end(), [](PDAEdge* iter)
			{
				return iter->HasTermActionType();
			}) != path.end();
		}
		bool IsLeftreCursionRingPath(vector<PDAEdge*>& path)
		{
			return accumulate(path.begin(), path.end(), (size_t) 0, [](int val, PDAEdge* iter)
			{
				auto actions = iter->GetActions();
				if(actions.size() == 1 &&
					actions.begin()->GetActionType() == ActionType::Shift)
				{
					return val + 1;
				}
				else
				{
					return val;
				}
			}) == path.size();
		}
		void RecordNewNode(PDANode* target, vector<PDANode*>& allNode, const vector<PDANode*>& noNeed)
		{
			if(find(noNeed.begin(), noNeed.end(), target) == noNeed.end())
			{
				allNode.emplace_back(target);
			}
		}
		void MergePDAEpsilonSymbol(PushDownAutoMachine& machine)
		{
			vector<PDAEdge*> path;
			//第i号路径已经完结集合
			unordered_set<int> save;
			vector<PDANode*> allNode;
			vector<PDANode*> noNeed;
			unordered_set<PDAEdge*> deleter;
			//路径上遇到的节点
			unordered_set<PDANode*> pathSign;
			unordered_map<PDANode*, int> edgeCountMap;
			vector<vector<PDAEdge*>> ringPathSaves;
			auto nodes = CollectGraphNode(machine, [&edgeCountMap](PDANode* element)
			{
				edgeCountMap.insert({ element,element->GetNexts().size() });
				return true;
			});
			unordered_multimap<PDANode*, vector<int>> nodeEdgesMap;
			auto root = machine.GetRoot();
			function<void(PDANode*)> DFS = [&pathSign, &noNeed, &deleter, &ringPathSaves, &allNode, &DFS, &path, &edgeCountMap, &machine](PDANode* node)
			{
				auto nexts = node->GetNexts();
				size_t length = edgeCountMap[node];
				for(size_t i = 0; i < length; ++i)
				{
					auto&& edgeIter = nexts[i];
					auto target = edgeIter->GetTarget();
					path.emplace_back(edgeIter);
					assert(path.size() == pathSign.size());
					
					if(pathSign.find(target) == pathSign.end() ||
						//对于包含Term的环,构造新环.
						IsTermRingPath(path))
					{
						if(edgeIter->HasTermActionType())
						{
							//遇到Term了,当前路径可以终止搜索了.
							
							MergePathSymbol(path, machine, edgeCountMap);
							//记录需要删除的边
							deleter.insert(path.begin(), path.end());
							assert(!path.empty());
							//记录待处理的节点
							assert(edgeIter->HasTermActionType());
							RecordNewNode(target, allNode, noNeed);
						}
						else
						{
							pathSign.insert(target);
							DFS(target);
							pathSign.erase(target);
						}
					}
					else if(IsLeftreCursionRingPath(path))
					{
						//遇到环了,对于全部由shift构造的环,也就是左递归,直接丢弃环就OK.
						deleter.insert(path.begin(), path.end());
						//deleter.insert(path.begin(), path.end());
					}
					else
					{
						//这里是无法遇到Term的环,也就是,走进了左递归,是不正确的路线,丢弃.
						deleter.insert(path.begin(), path.end());
					}
					path.pop_back();
				}
			};
			allNode.emplace_back(root);
			for(size_t i = 0; i < allNode.size(); ++i)
			{
				//这里用auto& auto&&会导致earse删除遇到0xddddddd,奇葩啊- -...
				auto nodeIter = allNode[i];
				pathSign.insert(nodeIter);
				DFS(nodeIter);
				noNeed.emplace_back(nodeIter);
				pathSign.erase(nodeIter);
				assert(pathSign.empty());
				assert(path.empty());
			}
			for(auto iter : deleter)
			{
				machine.DeleteEdge(iter);
			}
		}
		vector<PDAEdge*> CollectNontermiateEdgeByRoot(PushDownAutoMachine& machine)
		{
			vector<PDAEdge*> result;
			unordered_set<PDAEdge*> sign;
			//从Root表达式开始
			deque<PDANode*> queue;
			queue.emplace_back(machine.GetRoot());
			while(!queue.empty())
			{
				auto&& front = queue.front();
				for(auto&& edgeIter : front->GetNexts())
				{
					if(sign.find(edgeIter) == sign.end())
					{
						sign.insert(edgeIter);
						
						result.emplace_back(edgeIter);
						
						queue.emplace_back(edgeIter->GetTarget());
					}
				}
				queue.pop_front();
			}

			return result;
		}
		void DeleteNullPropertyEdge(PushDownAutoMachine& machine)
		{
			auto edges = CollectNontermiateEdgeByRoot(machine);
			for(auto&& iter : edges)
			{
				iter->DeleteNullPropertyAction();
			}
		}
		void CreateDPDAGraph(PushDownAutoMachine& machine)
		{
			auto PDAMap = CreateEpsilonPDA(machine);
			MergeStartAndEndNode(machine, PDAMap);
			AddPDAToPDAMachine(machine, PDAMap);
			machine.CreateRoot();
			MergeEpsilonPDAGraph(machine);
			MergeGrammarCommonFactor(machine);
			//添加结束节点.
		
			//应该直接在文法上添加finish fixed
		//	AddFinishNode(machine);
			//	HelpLogJumpTable(L"LogJumpTable_MergeGraphTable.txt", machine);
			MergePDAEpsilonSymbol(machine);
			MergeGrammarCommonFactor(machine);
			//DeleteNullPropertyEdge(machine);
			//MergeGrammarCommonFactor(machine);

			//HelpLogJumpTable(L"LogJumpTable_MergeNoTermGraphTable.txt", machine);
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