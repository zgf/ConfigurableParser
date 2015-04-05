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
			CreateEpsilonPDAVisitor(PushDownAutoMachine* _machine, GeneralRuleDefine* _rule, const wstring& _ruleName)noexcept
				:machine(_machine), rule(_rule), ruleName(_ruleName)
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
				ActionWrap wrap(ActionType::Terminate, node->text, ruleName, ruleName, L"");
				machine->AddEdge(result.first, result.second, move(wrap));
			}
			void								Visit(GeneralGrammarNormalTypeDefine* node)
			{
				result = machine->NewNodePair();
				if(machine->GetSymbolManager()->GetCacheRuleNameToSymbol(node->name))
				{
					ActionWrap wrap(ActionType::NonTerminate, node->name, L"", L"", L"");

					machine->AddEdge(result.first, result.second, move(wrap));
				}
				else
				{
					ActionWrap wrap(ActionType::Terminate, node->name, ruleName, ruleName, L"");
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
				ActionWrap wrap(ActionType::Setter, node->name, node->value, ruleName, L"");
				machine->FrontEdgesAdditionSetterAction(result.second, move(wrap));
			}
			void								Visit(GeneralGrammarUsingTypeDefine* node)
			{
				node->grammar->Accept(this);
				assert(result.second->GetFronts().size() == 1);
				auto name = machine->GetSymbolManager()->GetCacheUsingGrammarToRuleDefSymbol(node->grammar.get())->GetName();
				ActionWrap wrap(ActionType::Using, name, ruleName, name, ruleName);
				machine->FrontEdgesAdditionBackAction(result.second, move(wrap));
			}
			void								Visit(GeneralGrammarCreateTypeDefine* node)
			{
				auto manager = machine->GetSymbolManager();
				createTypeSymbol = FindType(manager, manager->GetGlobalSymbol(), node->type.get());
				node->grammar->Accept(this);
				ActionWrap wrap(ActionType::Create, createTypeSymbol->GetName(), ruleName, ruleName, L"");
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
					wrap = ActionWrap(ActionType::Assign, node->name, ruleSymbol->GetName(), ruleName, L"");
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
					wrap = ActionWrap(ActionType::Assign, node->name, fieldTypeSymbol->GetName(), ruleSymbol->GetName(), ruleName);
				}
				node->grammar->Accept(this);
				machine->FrontEdgesAdditionBackAction(result.second, move(wrap));
			}
		private:
			PushDownAutoMachine* machine;
			GeneralRuleDefine* rule;
			pair<PDANode*, PDANode*> result;
			wstring ruleName;
			ParserSymbol* createTypeSymbol = nullptr;
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
					CreateEpsilonPDAVisitor visitor(&machine, rulePointer, ruleIter->name);
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

		void DirectAddGrammarNumberOnPathEdge(PDAEdge* root, int count, unordered_set<PDAEdge*>& sign, unordered_map<int, PDAEdge*>& numberToRootMap)
		{
			if(root->GetNumber() == 31)
			{
				int a = 0;
			}
			sign.insert(root);
			root->SetGrammarNumber(count);
			for(auto&&edgeIter : root->GetTarget()->GetNexts())
			{
				if(root->GetNumber() == 31)
				{
					int a = 0;
				}
				if(sign.find(edgeIter) == sign.end())
				{
					DirectAddGrammarNumberOnPathEdge(edgeIter, count, sign, numberToRootMap);
				}
			}
		}
		void AddGrammarNumberOnPathEdge(PDAEdge* root, int count, unordered_set<PDAEdge*>& sign, unordered_map<int, PDAEdge*>& numberToRootMap)
		{
			sign.insert(root);
			if (root->GetNumber() == 31)
			{
				int a = 0;
			}
			if(root->GetGrammarNumber() == -1)
			{
				root->SetGrammarNumber(count);
				for(auto&&edgeIter : root->GetTarget()->GetNexts())
				{
					if(root->GetNumber() == 31)
					{
						int a = 0;
					}
					if(sign.find(edgeIter) == sign.end())
					{
						AddGrammarNumberOnPathEdge(edgeIter, count, sign, numberToRootMap);
					}
				}
			}
			else
			{
				assert(numberToRootMap.find(root->GetGrammarNumber()) != numberToRootMap.end());
				unordered_set<PDAEdge*> signSet;
				DirectAddGrammarNumberOnPathEdge(numberToRootMap[root->GetGrammarNumber()], count, signSet, numberToRootMap);
			}
		}
		void AddGrammarNumberToEdge(PushDownAutoMachine& machine)
		{
			auto count = 0;
			unordered_map<int, PDAEdge*> numberToRootMap;
			for(auto&& ruleIter : machine.GetPDAMap())
			{
				for(auto&& grammarIter : ruleIter.second.first->GetNexts())
				{
					unordered_set<PDAEdge*> sign;
					AddGrammarNumberOnPathEdge(grammarIter, count, sign, numberToRootMap);
					numberToRootMap.insert(make_pair(count, grammarIter));
					++count;
				}
			}
		}
		void AddGrammarNumberToAction(PDAEdge* root, unordered_set<PDAEdge*>& sign, PushDownAutoMachine& machine)
		{
			if(sign.find(root) == sign.end())
			{
				sign.insert(root);
				machine.SetEdgeGrammarNumberToAction(root);
			}

			for(auto&&edgeIter : root->GetTarget()->GetNexts())
			{
				if(sign.find(edgeIter) == sign.end())
				{
					AddGrammarNumberToAction(edgeIter, sign, machine);
				}
			}
		}
		void AddGrammarNumberToAction(PushDownAutoMachine& machine)
		{
			unordered_set<PDAEdge*> sign;
			for(auto&& ruleIter : machine.GetPDAMap())
			{
				for(auto&& grammarIter : ruleIter.second.first->GetNexts())
				{
					AddGrammarNumberToAction(grammarIter, sign, machine);
				}
			}
		}

		//可合并节点含义

		//同一个起点,包含同样信息的边,到达不同的节点,那么这个不同节点可以合并成同一个节点
		void MergeNodeByEdge(vector<PDAEdge*>& edges, vector<PDANode*>& nodeList, PushDownAutoMachine& machine)
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
		template<typename prediction_type>
		vector<PDAEdge*> CollcetGraphEdgeByRoot(PushDownAutoMachine& machine, prediction_type pred)
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
			unordered_set<PDANode*> sign;
			vector<PDAEdge*> edges;
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
					assert(edgeIter->GetActions()[0].GetActionType() == ActionType::NonTerminate);
					auto&& source = edgeIter->GetSource();
					auto&& target = edgeIter->GetTarget();
					auto actions = edgeIter->GetActions();
					auto&& nonTermiateIter = actions.begin();
					auto nonTerminateName = nonTermiateIter->GetName();
					auto number = edgeIter->GetGrammarNumber();
					machine.DeleteEdge(edgeIter);
					auto findIter = machine.GetPDAMap().find(nonTerminateName);
					assert(findIter != machine.GetPDAMap().end());
					assert(actions[0].GetActionType() == ActionType::NonTerminate);
					actions.erase(nonTermiateIter);
					machine.AddEdge(findIter->second.second, target, actions, number);
					machine.AddEdge(source, findIter->second.first, ActionWrap(ActionType::Shift, ruleName, nonTerminateName, ruleName, nonTerminateName, number), number);
				}
			}
		}
		void CheckAndDeleteReCursionRing(vector<ActionWrap>& newActions, ActionType type)
		{
			int lastIndex = find_if(make_reverse_iterator(newActions.end()), make_reverse_iterator(newActions.begin()), [type](const ActionWrap& wrap)
			{
				return wrap.GetActionType() == type;
			}).base() - newActions.begin();
			int firstIndex = find_if(newActions.begin(), newActions.end(), [type](const ActionWrap& wrap)
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
							//newActions.clear();
							//return;
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
		void CheckAndDeleteRightReCursionRing(vector<ActionWrap>& newActions)
		{
			CheckAndDeleteReCursionRing(newActions, ActionType::Reduce);
		}

		vector<ActionWrap> GetNewAction(vector<PDAEdge*>& save)
		{
			vector<ActionWrap> newActions;
			for_each(save.begin(), save.end(), [&newActions](PDAEdge* edge)
			{
				for(auto&& iter : edge->GetActions())
				{
					assert(iter.GetActionType() != ActionType::Epsilon&&iter.GetActionType() != ActionType::NonTerminate);
					newActions.emplace_back(iter);
					assert(iter.GetGrammarNumber() != -1);
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
			CollectGraphNode(machine, [&edgeCountMap](PDANode* element)
			{
				edgeCountMap.insert({ element,element->GetNexts().size() });
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

		void CreateDPDAGraph(PushDownAutoMachine& machine)
		{
			auto PDAMap = CreateEpsilonPDA(machine);
			MergeStartAndEndNode(machine, PDAMap);
			AddPDAToPDAMachine(machine, PDAMap);
			MergeGrammarCommonFactor(machine);
			//添加文法编号
			AddGrammarNumberToEdge(machine);
			//添加动作编号
			AddGrammarNumberToAction(machine);
			MergeEpsilonPDAGraph(machine);
			//添加结束节点.
			AddFinishNode(machine);

			MergePDAEpsilonSymbol(machine);
			//	MergeGrammarCommonFactor(machine);
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