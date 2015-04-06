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
					ActionWrap wrap(ActionType::Terminate, node->name ,ruleName, ruleName, L"");
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
				ActionWrap wrap(ActionType::Create, createTypeSymbol->GetName(), to_wstring(uniqueId++), ruleName, L"");
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
					machine.DeleteEdge(edgeIter);
					auto findIter = machine.GetPDAMap().find(nonTerminateName);
					assert(findIter != machine.GetPDAMap().end());
					assert(actions[0].GetActionType() == ActionType::NonTerminate);
					actions.erase(nonTermiateIter);
					machine.AddEdge(findIter->second.second, target, actions);
					machine.AddEdge(source, findIter->second.first, ActionWrap(ActionType::Shift, ruleName, nonTerminateName, ruleName, nonTerminateName));
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
		void CollcetPathNodeFieldInfo(PushDownAutoMachine& machine, PDAEdge* edge,
			unordered_set<PDAEdge*>&sign, vector<PDAEdge*>& signList, vector<PDAEdge*>& edgeList, vector<wstring>& fields)
		{
			if(sign.find(edge) == sign.end())
			{
				sign.insert(edge);
				signList.emplace_back(edge);
				for(auto&&action : edge->GetActions())
				{
					if(action.GetActionType() == ActionType::Create)
					{
					
						auto findIter = machine.GetCreatedNodeRequiresMap().find(action.GetValue());
						if (findIter == machine.GetCreatedNodeRequiresMap().end())
						{
							CreateInfo info;
							info.createType = action.GetName();
							info.fieldNames = fields;
							std::sort(info.fieldNames.begin(), info.fieldNames.end());
							machine.GetCreatedNodeRequiresMap().insert({ action.GetValue(),info });
						}
						else
						{
							auto& origin = machine.GetCreatedNodeRequiresMap()[action.GetValue()].fieldNames;
							assert(is_sorted(origin.begin(),origin.end()));
							vector<wstring> fieldCopy(fields.size() + origin.size());
							fieldCopy = fields;
							sort(fieldCopy.begin(), fieldCopy.end());
							auto num = fieldCopy.size();
							
							std::set_intersection(fieldCopy.begin(), fieldCopy.end(), origin.begin(), origin.end(),back_inserter(fieldCopy) );
							fieldCopy.erase(fieldCopy.begin(), fieldCopy.begin() + num);
							sort(fieldCopy.begin(), fieldCopy.end());
							origin = move(fieldCopy);
						}
						assert(edge->GetTarget()->GetNexts().size() == 0);
					}
					else if(action.GetActionType() == ActionType::Assign || action.GetActionType() == ActionType::Setter)
					{
						fields.emplace_back(action.GetName());
						edgeList.emplace_back(edge);
					}
				}
				for(auto&&iter : edge->GetTarget()->GetNexts())
				{
					CollcetPathNodeFieldInfo(machine, iter, sign,signList, edgeList, fields);
				}
				sign.erase(signList.back());
				signList.pop_back();
				while(!edgeList.empty() && edgeList.back() == edge)
				{
					edgeList.pop_back();
					fields.pop_back();
				}
			}
			
		}
		void CollcetPathNodeFieldInfo(PushDownAutoMachine& machine)
		{
			unordered_set<PDAEdge*>sign;
			vector<PDAEdge*> edgeList;
			vector<PDAEdge*> signList;
			vector<wstring>	fields;
			for(auto&& ruleIter : machine.GetPDAMap())
			{
				for(auto&&edgeIter : ruleIter.second.first->GetNexts())
				{
					
					CollcetPathNodeFieldInfo(machine, edgeIter, sign,signList, edgeList, fields);
					assert(fields.empty());
					assert(edgeList.empty());
					assert(sign.empty());
					assert(signList.empty());
				}
			}
		}
		void CreateDPDAGraph(PushDownAutoMachine& machine)
		{
			auto PDAMap = CreateEpsilonPDA(machine);
			MergeStartAndEndNode(machine, PDAMap);
			AddPDAToPDAMachine(machine, PDAMap);
			MergeGrammarCommonFactor(machine);
			CollcetPathNodeFieldInfo(machine);
			MergeEpsilonPDAGraph(machine);
			//添加结束节点.
			AddFinishNode(machine);
			MergePDAEpsilonSymbol(machine);
		}
	}
}