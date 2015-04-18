#include "Include\stdafx.h"
#include "Include\CreatedNodeResolve.h"
#include "Include\GeneralPushDownAutoMachine.h"

namespace ztl
{
	namespace general_parser
	{
		unordered_map<PDANode*, unordered_map<ParserSymbol*, PDANode*>>& CreatedNodeResolve::GetCreatedDFAMap()
		{
			// TODO: insert return statement here
			return createdDFAMap;
		}

		unordered_map<wstring, PDANode*>& CreatedNodeResolve::GetCreatedDFA()
		{
			return createdDFA;
		}
		unordered_map<PDANode*, unordered_map<ParserSymbol*, PDANode*>>& CreatedNodeResolve::GetRuleDFAMap()
		{
			return ruleDFAMap;
			// TODO: insert return statement here
		}
		unordered_map<wstring, PDANode*>& CreatedNodeResolve::GetRuleDFA()
		{
			return ruleDFA;
		}
		void CreatedNodeResolve::CacheRightRecursionArea(wstring number, const vector<ActionWrap> & wrapList)
		{
			auto findIter = nullRightRecursionAreas.find(number);
			assert(findIter == nullRightRecursionAreas.end());
			nullRightRecursionAreas.insert({ number,wrapList });
		}
		const vector<ActionWrap>& CreatedNodeResolve::GetNullRightRecursionAreas(const wstring& number) const
		{
			auto findIter = nullRightRecursionAreas.find(number);
			assert(findIter != nullRightRecursionAreas.end());
			return findIter->second;
		}

		bool CreatedNodeResolve::IsNullRightRecursionAreas(const wstring& number) const
		{
			return nullRightRecursionAreas.find(number) != nullRightRecursionAreas.end();
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
		int FindNodeActionType(const vector<ActionWrap>& actions)
		{
			for(int i = 0; i < (int) actions.size(); ++i)
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
				if(edgeIter->GetActions().size() > 1)
				{
					auto source = edgeIter->GetSource();
					auto target = edgeIter->GetTarget();
					auto actions = edgeIter->GetActions();
					machine.DeleteEdge(edgeIter);
					//assert(actions.size() > 1);
					assert(actions.size() == 2);
					if(actions[0].GetActionType() == ActionType::Terminate&&actions[1].GetActionType() == ActionType::Assign)
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
	
		vector<PDAEdge*> GetAllCreateTagNode(PDANode* dfaRoot)
		{
			return CollcetGraphEdgeByRoot(dfaRoot, [](PDAEdge* edge)
			{
				assert(edge->GetActions().size() == 1);
				return edge->GetActions().begin()->GetActionType() == ActionType::Create;
			});
		}
		void CreateDFAJumpInfoMap(PushDownAutoMachine& machine,PDANode* root)
		{
			auto& createdDfaMap = machine.GetCreateNodeResolve()->GetCreatedDFAMap();
			auto& ruleDfaMap = machine.GetCreateNodeResolve()->GetRuleDFAMap();
			//从Root表达式开始
			ReverseTravelGraphEdgeByRoot(root, [&ruleDfaMap,&createdDfaMap](PDAEdge* edgeIter)
			{
				auto target = edgeIter->GetTarget();
				auto source = edgeIter->GetSource();
				auto createdFindIter = createdDfaMap.find(target);
				if(createdFindIter == createdDfaMap.end())
				{
					createdDfaMap.insert({ target,unordered_map<ParserSymbol*,PDANode*>() });
				}
				auto ruleFindIter = ruleDfaMap.find(source);
				if(ruleFindIter == ruleDfaMap.end())
				{
					ruleDfaMap.insert({ source,unordered_map<ParserSymbol*,PDANode*>() });
				}
				assert(edgeIter->GetActions().size() == 1);
				auto symbol = edgeIter->GetActions()[0].GetParserSymbol();

				assert(createdDfaMap[target].find(symbol) == createdDfaMap[target].end() || createdDfaMap[target][symbol] == source);
				assert(createdDfaMap[source].find(symbol) == createdDfaMap[source].end() || createdDfaMap[source][symbol] == target);

				createdDfaMap[target].insert({ symbol,source });
				ruleDfaMap[source].insert({ symbol,target });
			});
		}
		pair<PDANode*,vector<PDAEdge*>> CreateResolveDFA(PushDownAutoMachine& machine, PDANode* root)
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
					if(sign.find(edgeIter) == sign.end())
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
			return{ dfaRoot,GetAllCreateTagNode(dfaRoot) };
		}
		void CreateResolveDFA(PushDownAutoMachine& machine)
		{
			for(auto&&iter : machine.GetPDAMap())
			{
				auto root = iter.second.first;
				auto&& solve = *machine.GetCreateNodeResolve();
				auto result = CreateResolveDFA(machine, root);
				solve.GetRuleDFA().insert({ iter.first, result.first });
				for(auto&&edge : result.second)
				{
					assert(edge->GetActions().size() == 1);
					auto value = edge->GetActions().begin()->GetValue();
					assert(edge->GetActions().begin()->GetActionType() == ActionType::Create);
					assert(solve.GetCreatedDFA().find(value) == solve.GetCreatedDFA().end());
					solve.GetCreatedDFA().emplace(value, edge->GetSource());
					MergeCreatedDFA(edge->GetSource(), machine);
					CreateDFAJumpInfoMap(machine, edge->GetSource());
				}
			}
		}
		vector<ActionWrap> CacheNullRightRecursionAreas(PDANode* start, PDANode* end)
		{
			//当前路径上遇到的边
			vector<PDAEdge*> path;
			//当前路径上遇到的边的hash
			unordered_set<PDAEdge*> sign;
			bool found = false;
			function<void(PDANode*)> DFS = [&found, &DFS, &sign, &path, end](PDANode* node)
			{
				for(size_t i = 0; i < node->GetNexts().size(); ++i)
				{
					auto&& edgeIter = node->GetNexts()[i];
					auto target = edgeIter->GetTarget();
					if(sign.find(edgeIter) == sign.end())
					{
						path.emplace_back(edgeIter);
						sign.insert(edgeIter);
						if(end == target)
						{
							found = true;
							//遇到需要的节点了,当前路径可以终止搜索了.
							return;
						}
						else if(edgeIter->HasTermActionType())
						{
							found = false;
							sign.erase(edgeIter);
							path.pop_back();
							return;
						}
						else
						{
							DFS(target);
						}
						if(found)
						{
							return;
						}
						assert(found == false);
						sign.erase(edgeIter);
						assert(path.back() == edgeIter);

						path.pop_back();
					}
				}
			};
			DFS(start);
			vector<ActionWrap> result;
			for(auto&&iter : path)
			{
				result.insert(result.end(), iter->GetActions().begin(), iter->GetActions().end());
			}
			return result;
		}
		void CacheNullRightRecursionAreas(PushDownAutoMachine& machine, PDANode* start, PDANode* target, const vector<ActionWrap>& startActions)
		{
			auto restActions = CacheNullRightRecursionAreas(start, target);
			if(!restActions.empty())
			{
				restActions.insert(restActions.begin(), startActions.begin(), startActions.end());
				assert(restActions.back().GetActionType() == ActionType::Create);
				auto number = restActions.back().GetValue();
				restActions.pop_back();
				reverse(restActions.begin(), restActions.end());
				machine.GetCreateNodeResolve()->CacheRightRecursionArea(number, restActions);
			}
		}
	}
}