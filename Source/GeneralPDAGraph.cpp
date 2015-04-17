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
			CreateEpsilonPDAVisitor(const CreateEpsilonPDAVisitor&) = default;
			CreateEpsilonPDAVisitor& operator=(CreateEpsilonPDAVisitor&&) noexcept = default;
			CreateEpsilonPDAVisitor& operator=(const CreateEpsilonPDAVisitor&) = default;
			CreateEpsilonPDAVisitor(PushDownAutoMachine* _machine, const wstring& _ruleName)noexcept
				:machine(_machine), ruleName(_ruleName)
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
				auto tokenSymbol = machine->GetSymbolManager()->GetTokenSymbolByName(node->text);
				assert(tokenSymbol != nullptr&&tokenSymbol->IsTokenDef() && !tokenSymbol->IsIgnore());
				ActionWrap wrap(ActionType::Terminate, tokenSymbol, ruleName, ruleName, L"");
				machine->AddEdge(result.first, result.second, move(wrap));
			}
			void								Visit(GeneralGrammarNormalTypeDefine* node)
			{
				result = machine->NewNodePair();
				auto ruleSymbol = machine->GetSymbolManager()->GetRuleSymbolByName(node->name);
				if(ruleSymbol)
				{
					ActionWrap wrap(ActionType::NonTerminate, ruleSymbol, L"", L"", L"");
					machine->AddEdge(result.first, result.second, move(wrap));
				}
				else
				{
					auto tokenSymbol = machine->GetSymbolManager()->GetTokenSymbolByName(node->name);
					assert(tokenSymbol != nullptr&&tokenSymbol->IsTokenDef() && !tokenSymbol->IsIgnore());

					ActionWrap wrap(ActionType::Terminate, tokenSymbol, ruleName, ruleName, L"");
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
				//Setter前一条边是Create
				assert(find_if(result.second->GetFronts().begin(), result.second->GetFronts().end(), [](PDAEdge*edge)
				{
					return edge->GetActions().back().GetActionType() != ActionType::Create;
				}) == result.second->GetFronts().end());
				assert(createTypeSymbol != nullptr);
				auto filedSymbol = createTypeSymbol->GetSubSymbolByName(node->name);
				assert(filedSymbol != nullptr&&filedSymbol->IsFieldDef());
				ActionWrap wrap(ActionType::Setter, filedSymbol, node->value, ruleName, L"");
				machine->FrontEdgesAdditionSetterAction(result.second, move(wrap));
			}
			void								Visit(GeneralGrammarUsingTypeDefine* node)
			{
				node->grammar->Accept(this);
				assert(result.second->GetFronts().size() == 1);
				auto ruleSymbol = machine->GetSymbolManager()->GetCacheUsingGrammarToRuleDefSymbol(node->grammar.get());
				assert(ruleSymbol != nullptr&&ruleSymbol->IsRuleDef());
				ActionWrap wrap(ActionType::Using, ruleSymbol, ruleName, ruleSymbol->GetName(), ruleName);
				machine->FrontEdgesAdditionBackAction(result.second, move(wrap));
			}
			void								Visit(GeneralGrammarCreateTypeDefine* node)
			{
				auto manager = machine->GetSymbolManager();
				createTypeSymbol = FindType(manager, manager->GetGlobalSymbol(), node->type.get());
				node->grammar->Accept(this);
				ActionWrap wrap(ActionType::Create, createTypeSymbol, to_wstring(uniqueId++), ruleName, L"");
				//合并的节点可能是循环.直接Addition这样的话会导致create在循环内出现多次
				auto newNode = machine->NewNode();
				machine->AddEdge(result.second, newNode, move(wrap));
				result.second = newNode;
			}
			void								Visit(GeneralGrammarAlternationTypeDefine* node)
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
				ActionWrap wrap;
				if(ruleSymbol->IsTokenDef())
				{
					auto fieldSymbol = createTypeSymbol->SearchClassFieldSymbol(node->name);
					assert(fieldSymbol != nullptr&&fieldSymbol->IsFieldDef());
					wrap = ActionWrap(ActionType::Assign, fieldSymbol, ruleSymbol->GetName(), ruleName, L"");
				}
				else
				{
					auto fieldSymbol = createTypeSymbol->SearchClassFieldSymbol(node->name);
					assert(fieldSymbol != nullptr&&fieldSymbol->IsFieldDef());
					auto fieldTypeSymbol = fieldSymbol->GetDescriptorSymbol();
					if(fieldTypeSymbol->IsArrayType())
					{
						fieldTypeSymbol = fieldTypeSymbol->GetDescriptorSymbol();
					}
					assert(fieldTypeSymbol->IsType() && !fieldTypeSymbol->IsArrayType());
					wrap = ActionWrap(ActionType::Assign, fieldSymbol, fieldTypeSymbol->GetName(), ruleSymbol->GetName(), ruleName);
				}
				node->grammar->Accept(this);
				machine->FrontEdgesAdditionBackAction(result.second, move(wrap));
			}
		private:
			PushDownAutoMachine* machine;
			pair<PDANode*, PDANode*> result;
			wstring ruleName;
			ParserSymbol* createTypeSymbol = nullptr;
			static int uniqueId;
		};
		int CreateEpsilonPDAVisitor::uniqueId = 0;
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
					CreateEpsilonPDAVisitor visitor(&machine, ruleIter->name);
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
		}

		void AddFinishNode(PushDownAutoMachine& machine)
		{
			auto& pdaMap = machine.GetPDAMap();
			assert(!machine.GetSymbolManager()->GetStartRuleList().empty());
			auto rootRuleName = machine.GetRootRuleName();
			assert(machine.GetPDAMap().find(rootRuleName) != machine.GetPDAMap().end());
			machine.AddFinishNodeFollowTarget(pdaMap[rootRuleName].second, rootRuleName);
		}

		//可合并节点含义

		//同一个起点,包含同样信息的边,到达不同的节点,那么这个不同节点可以合并成同一个节点
		void MergeNodeByEdge(const vector<PDAEdge*>& edges, vector<PDANode*>& nodeList, PushDownAutoMachine& machine)
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
					for_each(startIter + 1, endIter, [&save, &nodeList, &machine](PDAEdge* element)
					{
						machine.MergeIndependentNodes(save, element->GetTarget());
						machine.DeleteEdge(element);
						if(save->GetNexts().size() > 1)
						{
							nodeList.emplace_back(save);
						}
					});
				}
				startIter = endIter;
			}
		}
		void MergeCommonNode(PDANode* nodeIter, vector<PDANode*>& nodeList, PushDownAutoMachine& machine)
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
				MergeNodeByEdge(edges, nodeList, machine);
			}
		}
		template<typename prediction_type>
		vector<PDANode*> CollectGraphNode(const vector<PDANode*>& nodeList, const prediction_type& pred)
		{
			unordered_set<PDANode*>sign;
			deque<PDANode*> queue;
			vector<PDANode*> result;
			for(auto&&iter : nodeList)
			{
				queue.emplace_back(iter);
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
		template<typename prediction_type>
		vector<PDANode*> ReverseCollectGraphNode(const vector<PDANode*>& nodeList, const prediction_type& pred)
		{
			unordered_set<PDANode*>sign;
			deque<PDANode*> queue;
			vector<PDANode*> result;
			for(auto&&iter : nodeList)
			{
				queue.emplace_back(iter);
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
						for(auto&& edgeIter : front->GetFronts())
						{
							queue.emplace_back(edgeIter->GetSource());
						}
					}
				}
			}
			return result;
		}
		template<typename functor_type>
		void ReverseTravelGraphEdgeByRoot(PDANode*root, functor_type functor)
		{
			unordered_set<PDAEdge*> sign;
			//从Root表达式开始
			deque<PDANode*> queue;
			queue.emplace_back(root);
			while(!queue.empty())
			{
				auto&& front = queue.front();
				for(auto&& edgeIter : front->GetFronts())
				{
					if(sign.find(edgeIter) == sign.end())
					{
						sign.insert(edgeIter);
						functor(edgeIter);
						queue.emplace_back(edgeIter->GetSource());
					}
				}
				queue.pop_front();
			}
		}
		template<typename prediction_type>
		vector<PDAEdge*> CollcetGraphEdgeByRoot(PDANode*root, prediction_type pred)
		{
			vector<PDAEdge*> result;
			unordered_set<PDAEdge*> sign;
			//从Root表达式开始
			deque<PDANode*> queue;
			queue.emplace_back(root);
			while(!queue.empty())
			{
				auto&& front = queue.front();
				for(auto&& edgeIter : front->GetNexts())
				{
					if(sign.find(edgeIter) == sign.end())
					{
						sign.insert(edgeIter);
						if(pred(edgeIter))
						{
							result.emplace_back(edgeIter);
						}
						queue.emplace_back(edgeIter->GetTarget());
					}
				}
				queue.pop_front();
			}

			return result;
		}
		void MergeGrammarCommonFactor(PushDownAutoMachine& machine)
		{
			vector<PDANode*>nodeList;
			//收集最左的边>2的节点
			for(auto&&iter : machine.GetPDAMap())
			{
				auto nodeIter = iter.second.first;
				if(nodeIter->GetNexts().size() > 1)
				{
					nodeList.emplace_back(nodeIter);
				}
			}

			for(size_t i = 0; i < nodeList.size(); ++i)
			{
				auto&& iter = nodeList[i];

				assert(iter->GetNexts().size()>1);
				MergeCommonNode(iter, nodeList, machine);
			}
		}
		unordered_map<wstring, vector<PDAEdge*>> CollectNontermiateEdge(PushDownAutoMachine& machine)
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
		void MergeEpsilonPDAGraph(PushDownAutoMachine& machine)
		{
			auto&& edgeList = CollectNontermiateEdge(machine);

			for(auto&& iter : edgeList)
			{
				auto&& ruleName = iter.first;
				for(auto&& edgeIter : iter.second)
				{
					auto&& source = edgeIter->GetSource();
					auto&& target = edgeIter->GetTarget();
					auto actions = edgeIter->GetActions();

					auto&& nonTermiateIter = find_if(actions.begin(), actions.end(), [](const ActionWrap& wrap)
					{
						return wrap.GetActionType() == ActionType::NonTerminate;
					});

					assert(nonTermiateIter != actions.end());
					auto nonTerminateName = nonTermiateIter->GetName();

					vector<ActionWrap> leftAction;
					leftAction.insert(leftAction.end(), actions.begin(), nonTermiateIter);
					auto ruleSymbol = machine.GetSymbolManager()->GetRuleSymbolByName(ruleName);
					assert(ruleSymbol != nullptr&&ruleSymbol->IsRuleDef());
					leftAction.emplace_back(ActionWrap(ActionType::Shift, ruleSymbol, nonTerminateName, ruleName, nonTerminateName));

					vector<ActionWrap> rightAction;
					rightAction.insert(rightAction.end(), nonTermiateIter + 1, actions.end());
					machine.DeleteEdge(edgeIter);
					auto findIter = machine.GetPDAMap().find(nonTerminateName);
					assert(findIter != machine.GetPDAMap().end());
					actions.erase(nonTermiateIter);
					machine.AddEdge(findIter->second.second, target, rightAction);
					machine.AddEdge(source, findIter->second.first, leftAction);
				}
			}
		}
		void CheckAndDeleteReCursionRing(vector<ActionWrap>& newActions, ActionType type)
		{
			auto lastIndex = find_if(make_reverse_iterator(newActions.end()), make_reverse_iterator(newActions.begin()), [type](const ActionWrap& wrap)
			{
				return wrap.GetActionType() == type;
			}).base() - newActions.begin();
			auto firstIndex = find_if(newActions.begin(), newActions.end(), [type](const ActionWrap& wrap)
			{
				return wrap.GetActionType() == type;
			}) - newActions.begin();
			firstIndex = (firstIndex == (int) newActions.size()) ? 0 : firstIndex;
			vector<int> deleter;
			if(firstIndex != lastIndex)
			{
				for(auto index = firstIndex; index != lastIndex; ++index)
				{
					auto begin = newActions.begin() + firstIndex;
					auto end = newActions.begin() + index + 1;
					for(auto i = begin; i < end; ++i)
					{
						if(i->GetName() == newActions[index].GetValue() && i != end)
						{
							auto count = 0;
							for_each(i, end, [&count, type](const ActionWrap& wrap)
							{
								if(wrap.GetActionType() != type)
								{
									++count;
								}
							});
							if(count == 0)
							{
								auto number = end - i;
								assert(std::all_of(i,  end, [type](const ActionWrap& wrap)
								{
									return wrap.GetActionType() == type;
								}));
								newActions.erase(i, end);
								lastIndex = lastIndex - number;
								index = index - number;
								break;
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
		/*void CheckAndDeleteRightReCursionRing(vector<ActionWrap>& newActions)
		{
			CheckAndDeleteReCursionRing(newActions, ActionType::Reduce);
		}*/

		vector<ActionWrap> GetNewAction(vector<PDAEdge*>& save)
		{
			vector<ActionWrap> newActions;
			for_each(save.begin(), save.end(), [&newActions](PDAEdge* edge)
			{
				for(auto&& iter : edge->GetActions())
				{
					assert(iter.GetActionType() != ActionType::Epsilon&&iter.GetActionType() != ActionType::NonTerminate);
					newActions.emplace_back(iter);
				}
			});

			CheckAndDeleteLeftReCursionRing(newActions);
			//不处理右递归,右递归会带上assign信息.走的时候查看语法堆栈和节点就可以知道能否走通了
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
				if(!newActions.empty())
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

		void RecordNewNode(PDANode* target, vector<PDANode*>& allNode, unordered_set<PDANode*>& noNeed)
		{
			if(noNeed.find(target) == noNeed.end())
			{
				noNeed.insert(target);
				allNode.emplace_back(target);
			}
		}
		bool IsCorrectEdge(const vector<PDAEdge*>& path, const PDAEdge* edge)
		{
			if(path.empty())
			{
				return true;
			}
			else
			{
				assert(!path.empty());
				auto grammr = path.back()->GetActions().back().GetTo();
				if(grammr.empty())
				{
					grammr = path.back()->GetActions().back().GetFrom();
				}
				return grammr == edge->GetActions().front().GetFrom();
			}
		}

		void MergePDAEpsilonSymbol(PushDownAutoMachine& machine)
		{
			//当前路径上遇到的边
			vector<PDAEdge*> path;
			//当前路径上遇到的边的hash
			unordered_set<PDAEdge*> sign;
			//待处理的点队列
			vector<PDANode*> allNode;
			//已进队列或者处理结束的点
			unordered_set<PDANode*> noNeed;
			//待删除的边
			unordered_set<PDAEdge*> deleter;
			//每个节点上旧边的数量
			unordered_map<PDANode*, int> edgeCountMap;
			//edgeCountMap收集完毕
			auto& startList = machine.GetSymbolManager()->GetStartRuleList();
			vector<PDANode*>nodeList;
			std::transform(startList.begin(), startList.end(), back_inserter(nodeList), [&machine](const wstring& ruleName)
			{
				return machine.GetPDAMap()[ruleName].first;
			});
			CollectGraphNode(nodeList, [&edgeCountMap](PDANode* element)
			{
				edgeCountMap.insert({ element,(int) element->GetNexts().size() });
				return true;
			});
			auto root = machine.GetRoot();
			allNode.emplace_back(root);
			noNeed.insert(root);

			function<void(PDANode*)> DFS = [&noNeed, &deleter, &allNode, &DFS, &sign, &path, &edgeCountMap, &machine](PDANode* node)
			{
				auto nexts = node->GetNexts();
				size_t length = edgeCountMap[node];
				for(size_t i = 0; i < length; ++i)
				{
					auto&& edgeIter = nexts[i];
					auto target = edgeIter->GetTarget();
					if(sign.find(edgeIter) == sign.end() && IsCorrectEdge(path, edgeIter))
					{
						path.emplace_back(edgeIter);
						sign.insert(edgeIter);
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
							DFS(target);
						}
						sign.erase(edgeIter);
						assert(path.back() == edgeIter);
						path.pop_back();
					}
					else
					{
						//遇到环了
						deleter.insert(path.begin(), path.end());
					}
				}
			};
			for(size_t i = 0; i < allNode.size(); ++i)
			{
				auto nodeIter = allNode[i];
				DFS(nodeIter);
			}
			for(auto iter : deleter)
			{
				machine.DeleteEdge(iter);
			}
		}

		int FindNodeActionType(const vector<ActionWrap>& actions)
		{
			for(size_t i = 0; i < actions.size(); ++i)
			{
				auto&&iter = actions[i];
				if(iter.GetActionType() == ActionType::Assign ||
					iter.GetActionType() == ActionType::Create ||
					iter.GetActionType() == ActionType::Terminate ||
					iter.GetActionType() == ActionType::Setter)
				{
					return i;
				}
			}
			return -1;
		}

		PDANode* FindMapNode(PDANode* origin, unordered_map<PDANode*, PDANode*>& nodeMap, PushDownAutoMachine& machine)
		{
			auto findIter = nodeMap.find(origin);
			if(findIter == nodeMap.end())
			{
				auto node = machine.NewNode();
				nodeMap.insert({ origin,node });
				return node;
			}
			else
			{
				return findIter->second;
			}
		}

		void GenerateDFANode(const vector<PDAEdge*>& save, unordered_map<PDANode*, PDANode*>& nodeMap, PushDownAutoMachine& machine)
		{
			assert(!save.empty());
			auto source = save.front()->GetSource();
			auto target = save.back()->GetTarget();
			source = FindMapNode(source, nodeMap, machine);
			target = FindMapNode(target, nodeMap, machine);
			machine.AddEdge(source, target, save.back()->GetActions());
		}
		void SeparateActions(PDANode* root, PushDownAutoMachine& machine)
		{
			auto edges = CollcetGraphEdgeByRoot(root, [](PDAEdge* edge)
			{
				return edge->GetActions().size() > 1;
			});

			for(auto&&edgeIter : edges)
			{
				if (edgeIter->GetActions().size()>1)
				{
					auto source = edgeIter->GetSource();
					auto target = edgeIter->GetTarget();
					auto actions = edgeIter->GetActions();
					machine.DeleteEdge(edgeIter);
					//assert(actions.size() > 1);
					assert(actions.size() == 2);
					if (actions[0].GetActionType()== ActionType::Terminate&&actions[1].GetActionType()== ActionType::Assign)
					{
						swap(actions.front(), actions.back());
						actions.pop_back();
						machine.AddEdge(source, target, actions);
					}
					else if(actions[0].GetActionType() == ActionType::NonTerminate&&actions[1].GetActionType() == ActionType::Assign)
					{
						swap(actions.front(), actions.back());
						actions.pop_back();
						machine.AddEdge(source, target, actions);
					}
					else
					{
						vector<PDANode*> nodeList(actions.size() + 1);
						nodeList[0] = source;
						generate_n(nodeList.begin() + 1, actions.size() - 1, [&machine]()
						{
							return machine.NewNode();
						});
						nodeList[nodeList.size() - 1] = target;
						assert(nodeList.size() == actions.size() + 1);
						int sourceIter = 0;
						int targetIter = 1;
						for(auto&&actionIter : actions)
						{
							machine.AddEdge(nodeList[sourceIter], nodeList[targetIter], actionIter);
							++sourceIter;
							++targetIter;
						}
					}
				
				}
			}
		}
		void MergeNodeByFrontEdge(const vector<PDAEdge*>& edges, vector<PDANode*>& nodeList, PushDownAutoMachine& machine)
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
					auto&& save = (*startIter)->GetSource();
					for_each(startIter + 1, endIter, [&save, &nodeList, &machine](PDAEdge* element)
					{
						machine.MergeIndependentNodes(save, element->GetSource());
						machine.DeleteEdge(element);
						if(save->GetFronts().size() > 1)
						{
							nodeList.emplace_back(save);
						}
					});
				}
				startIter = endIter;
			}
		}
		void MergeCreatedDFA(vector<PDANode*>&queue, PDANode* root, unordered_set<PDANode*>&sign, PushDownAutoMachine& machine)
		{
			if(root->GetFronts().size() > 1)
			{
				assert(sign.find(root) == sign.end());
				auto nexts = root->GetFronts();
				sort(nexts.begin(), nexts.end(), [](const PDAEdge* left, const PDAEdge* right)
				{
					return left->GetActions().size() == right->GetActions().size() ?
						left->GetActions() < right->GetActions() :
						left->GetActions().size() < right->GetActions().size();
				});
				MergeNodeByFrontEdge(nexts, queue, machine);
				sign.insert(root);
			}
		}
		void MergeCreatedDFA(PDANode* root, PushDownAutoMachine& machine)
		{
			vector<PDANode*>queue;
			unordered_set<PDANode*>sign;
			sign.emplace(root);
			queue.emplace_back(root);
			queue = ReverseCollectGraphNode(queue, [](PDANode*node)
			{
				return node->GetFronts().size() > 1;
			});
			for(size_t i = 0; i < queue.size(); ++i)
			{
				auto&& iter = queue[i];
				if(sign.find(iter) == sign.end())
				{
					MergeCreatedDFA(queue, iter, sign, machine);
				}
			}
		}
		vector<PDAEdge*> GetAllCreateTagNode( PDANode* dfaRoot)
		{
			return CollcetGraphEdgeByRoot(dfaRoot, [](PDAEdge* edge)
			{
				assert(edge->GetActions().size() == 1);
				return edge->GetActions().begin()->GetActionType() == ActionType::Create;
			});
		}
		void CreateDFAJumpInfoMap(PushDownAutoMachine& machine, PDANode* root)
		{
			auto& dfaMap = machine.GetDFAMap();
			//从Root表达式开始
			ReverseTravelGraphEdgeByRoot(root, [&dfaMap](PDAEdge* edgeIter)
			{
				auto source = edgeIter->GetTarget();
				auto findIter = dfaMap.find(source);
				if(findIter == dfaMap.end())
				{
					dfaMap.insert({ source,unordered_map<ParserSymbol*,PDANode*>() });
				}
				assert(edgeIter->GetActions().size() == 1);
				auto symbol = edgeIter->GetActions()[0].GetParserSymbol();
				
				assert(dfaMap[source].find(symbol) == dfaMap[source].end() || dfaMap[source][symbol] == edgeIter->GetSource());

				dfaMap[source].insert({ symbol,edgeIter->GetSource() });
			});
		}
		vector<PDAEdge*> CreateResolveDFA(PushDownAutoMachine& machine,PDANode* root)
		{
			//当前路径上遇到的边
			vector<PDAEdge*> path;
			//当前路径上遇到的边的hash
			unordered_set<PDAEdge*> sign;
			//待处理的点队列
			vector<PDANode*> queue;
			//已进队列或者处理结束的点
			unordered_set<PDANode*> noNeed;
			queue.emplace_back(root);
			noNeed.insert(root);
			//DFA节点和原图节点的映射
			unordered_map<PDANode*, PDANode*> nodeMap;
			auto dfaRoot = FindMapNode(root, nodeMap, machine);
			function<void(PDANode*)> DFS = [&noNeed, &queue, &nodeMap, &DFS, &sign, &path, &machine](PDANode* node)
			{
				for(size_t i = 0; i < node->GetNexts().size(); ++i)
				{
					auto&& edgeIter = node->GetNexts()[i];
					auto target = edgeIter->GetTarget();
					if(sign.find(edgeIter) == sign.end() && IsCorrectEdge(path, edgeIter))
					{
						path.emplace_back(edgeIter);
						sign.insert(edgeIter);
						auto index = FindNodeActionType(edgeIter->GetActions());
						if(index != -1)
						{
							//遇到需要的节点了,当前路径可以终止搜索了.
							GenerateDFANode(path, nodeMap, machine);
							assert(!path.empty());
							//记录待处理的节点
							RecordNewNode(target, queue, noNeed);
						}
						else
						{
							DFS(target);
						}
						sign.erase(edgeIter);
						assert(path.back() == edgeIter);
						path.pop_back();
					}
				}
			};
			for(size_t i = 0; i < queue.size(); ++i)
			{
				auto nodeIter = queue[i];
				DFS(nodeIter);
			}
			SeparateActions(dfaRoot, machine);
			return  GetAllCreateTagNode(dfaRoot);
		}
		void CreateResolveDFA(PushDownAutoMachine& machine)
		{
			for (auto&&iter:machine.GetPDAMap())
			{
				auto root = iter.second.first;
				for (auto&&edge: CreateResolveDFA(machine, root))
				{
					assert(edge->GetActions().size() == 1);
					auto value = edge->GetActions().begin()->GetValue();
					assert(edge->GetActions().begin()->GetActionType() == ActionType::Create);
					assert(machine.GetCreateDFA().find(value) == machine.GetCreateDFA().end());
					machine.GetCreateDFA().emplace(value,edge->GetSource());
					MergeCreatedDFA(edge->GetSource(), machine);
					CreateDFAJumpInfoMap(machine, edge->GetSource());
				}
			}
		}
		
		void CreateDPDAGraph(PushDownAutoMachine& machine)
		{
			auto PDAMap = CreateEpsilonPDA(machine);
			MergeStartAndEndNode(machine, PDAMap);
			AddPDAToPDAMachine(machine, PDAMap);
			MergeGrammarCommonFactor(machine);
			CreateResolveDFA(machine);
			MergeEpsilonPDAGraph(machine);
			//添加结束节点.
			AddFinishNode(machine);
			LogGraphInfo(L"NonMergeParserDefine.txt",machine);
			MergePDAEpsilonSymbol(machine);
		}
	}
}