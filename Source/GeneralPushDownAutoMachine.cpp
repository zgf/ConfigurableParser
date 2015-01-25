#include "Include/stdafx.h"
#include "Include/GeneralTableDefine.h"
#include "Include/GeneralPushDownAutoMachine.h"
#include "Include/SymbolManager.h"
namespace ztl
{
	namespace general_parser
	{
		
		PushDownAutoMachine::PushDownAutoMachine() :manager(nullptr)
		{
		}
		PushDownAutoMachine::PushDownAutoMachine(SymbolManager* _manager)
			:manager(_manager)
		{
			
		}

		SymbolManager * PushDownAutoMachine::GetSymbolManager() const
		{
			return this->manager;
		}

		GeneralTableDefine * PushDownAutoMachine::GetTable() const
		{
			return GetSymbolManager()->GetTable();
		}

		unordered_map<wstring, vector<pair<PDANode*,PDANode*>>>& PushDownAutoMachine::GetPDAMap()
{
			return PDAMap;
		}

		void PushDownAutoMachine::AddEdge(PDANode * source, PDANode * target, const ActionWrap& wrap)
		{
			auto edge = NewEdge(source, target, wrap);
			source->nexts->emplace_back(edge);
			target->fronts->emplace_back(edge);
		}

		void PushDownAutoMachine::AddEdge(PDANode * source, PDANode * target, const deque<ActionWrap>& wrapList)
		{
			auto edge = NewEdge(source, target);
			*edge->actions = wrapList;
			source->nexts->emplace_back(edge);
			target->fronts->emplace_back(edge);
		}

		void PushDownAutoMachine::DeleteEdge(PDAEdge* target)
		{
			auto frontNode = target->GetSource();
			auto nextNode = target->GetTarget();
			auto frontIer = std::find(frontNode->GetNexts().begin(), frontNode->GetNexts().end(), target);
			assert(frontIer != frontNode->GetNexts().end());
			auto nextIter = std::find(nextNode->GetFronts().begin(), nextNode->GetFronts().end(), target);
			assert(nextIter != nextNode->GetFronts().end());
			target->source = nullptr;
			target->target = nullptr;
			frontNode->nexts->erase(frontIer);
			nextNode->fronts->erase(nextIter);
		}

		void PushDownAutoMachine::AddGeneratePDA(wstring ruleName, PDANode * start,PDANode* end)
		{
			auto findIter = PDAMap.find(ruleName);
			if(findIter == PDAMap.end())
			{
				PDAMap[ruleName];
			}
			PDAMap[ruleName].emplace_back(make_pair( start ,end));
		
		}

		pair<PDANode*, PDANode*> PushDownAutoMachine::NewNodePair()
		{
			return{ NewNode(), NewNode() };
		}

		PDANode * PushDownAutoMachine::NewNode()
		{
			nodes.emplace_back(make_shared<PDANode>());
			return nodes.back().get();
		}

		pair<PDANode*, PDANode*> PushDownAutoMachine::AddSequenceLinkNode(pair<PDANode*, PDANode*>& left, pair<PDANode*, PDANode*>& right)
		{
			auto mergeNode = MergeIndependentNodes(left.second, right.first);
			if (right.first == right.second)
			{
				return pair<PDANode*, PDANode*>(mergeNode, mergeNode);
			}
			else
			{
				return pair<PDANode*, PDANode*>(left.first, right.second);
			}
		}

		pair<PDANode*, PDANode*> PushDownAutoMachine::AddLoopLinkNode(PDANode * loopStart, PDANode * loopEnd)
		{
			auto mergeNode = MergeIndependentNodes(loopEnd, loopStart);
			return pair<PDANode*, PDANode*>(mergeNode, mergeNode);
		}

		pair<PDANode*, PDANode*> PushDownAutoMachine::AddAlterationLinkNode(pair<PDANode*, PDANode*>& left, pair<PDANode*, PDANode*>& right)
		{
			auto newNodes = NewNodePair();
			MergeIndependentNodes(newNodes.first, left.first);
			MergeIndependentNodes(newNodes.first, right.first);
			MergeIndependentNodes(newNodes.second, left.second);
			MergeIndependentNodes(newNodes.second, left.second);

			return newNodes;
		}

		pair<PDANode*, PDANode*> PushDownAutoMachine::AddOptionalLinkNode(PDANode* optionalStart, PDANode* optionalEnd)
		{
			auto mergeNode = MergeIndependentNodes(optionalStart, optionalEnd);

			return pair<PDANode*, PDANode*>(mergeNode, mergeNode);
		}

		void PushDownAutoMachine::EdgeAdditionAction(PDANode* targetNode, const ActionWrap& wrap)
		{
			for (auto&& edgeIter: *targetNode->fronts)
			{
				this->BackInsertAction(edgeIter, wrap);
			}
		}


		PDANode * PushDownAutoMachine::MergeIndependentNodes(PDANode * left, PDANode * right)
		{
			if (left!=right)
			{
				for(size_t i = 0; i < right->nexts->size(); i++)
				{
					auto iter = right->nexts->at(i);
					iter->source = left;
					left->nexts->emplace_back(iter);
				}

				right->nexts->clear();

				for(size_t i = 0; i < right->fronts->size(); i++)
				{
					auto iter = right->fronts->at(i);
					iter->target = left;
					left->fronts->emplace_back(iter);

				}
				right->fronts->clear();
				left->SetMergeFlag();
			}
			return left;
		}

		void PushDownAutoMachine::BackInsertAction(PDAEdge* edge,const ActionWrap& wrap)
		{
			edge->actions->emplace_back(wrap);
		}

		void PushDownAutoMachine::FrontInsertAction(PDAEdge* edge, const ActionWrap& wrap)
		{
			edge->actions->emplace_front(wrap);
			++edge->mainActionIndex;
		}

		void PushDownAutoMachine::InitNodeIndexMap()
		{
			unordered_set<PDANode*>sign;
			deque<PDANode*> queue;
			int count = 0;

			for(auto&&ruleIter : PDAMap)
			{
				for (auto&& grammarIter:ruleIter.second)
				{
					queue.emplace_back(grammarIter.first);
					while(!queue.empty())
					{
						PDANode* front = queue.front();
						queue.pop_front();
						if (sign.find(front) == sign.end())
						{
							sign.insert(front);
							nodeIndexMap.insert(make_pair(front, count));
							++count;
							for(auto&& edgeIter : front->GetNexts())
							{
								queue.emplace_back(edgeIter->GetTarget());
							}
						}
					}
				}
			}
			InitNodeIndexToRuleNameMap();
		}

		void PushDownAutoMachine::CreateJumpTable()
		{
			unordered_set<PDAEdge*> sign;
			deque<PDANode*> queue;
			for(auto&& ruleIter : GetPDAMap())
			{
				for (auto&& grammarIter:ruleIter.second)
				{
					queue.emplace_back(grammarIter.first);
					while(!queue.empty())
					{
						auto front = queue.front();
						queue.pop_front();
						jumpTable.insert(make_pair(GetNodeIndex(front), CreateJumpItem(front, sign, queue)));
					}
				}
			}
		}

		void PushDownAutoMachine::ClearJumpTable()
		{
			jumpTable.clear();
		}

		const unordered_map<int, unordered_map<int, pair<int,deque<ActionWrap>>>>& PushDownAutoMachine::GetJumpTable() const
{
			return jumpTable;
		}

		int PushDownAutoMachine::GetNodeIndex(PDANode * target)
		{
			assert(nodeIndexMap.find(target) != nodeIndexMap.end());
			return nodeIndexMap[target];
		}

		void PushDownAutoMachine::InitNodeIndexToRuleNameMap()
		{
			for (auto&& ruleIter:PDAMap)
			{
				auto&& ruleName = ruleIter.first;
				for (auto&& grammarIter:ruleIter.second)
				{
					nodeIndexToRuleNameMap.insert(make_pair(GetNodeIndex(grammarIter.first), ruleName));
				}
			}
		}

		unordered_map<int, pair<int,deque<ActionWrap>>> PushDownAutoMachine::CreateJumpItem(PDANode* source,unordered_set<PDAEdge*>& sign,deque<PDANode*>& queue)
		{
			unordered_map<int, pair<int,deque<ActionWrap>>> result;
			for(PDAEdge* edgeIter : source->GetNexts())
			{
				auto name = edgeIter->GetActions().begin()->GetName();
				if(sign.find(edgeIter) == sign.end())
				{
					sign.insert(edgeIter);
					result.insert(make_pair(manager->GetCacheTagByName(name),make_pair(GetNodeIndex(edgeIter->GetTarget()),*edgeIter->actions)));
					queue.emplace_back(edgeIter->GetTarget());
				}
			}
			return result;
		}

		wstring PushDownAutoMachine::GetRuleNameOrEmptyByNodeIndex(int index) 
		{
			//assert(nodeIndexToRuleNameMap.find(index) == nodeIndexToRuleNameMap.end());
			return nodeIndexToRuleNameMap.find(index) == nodeIndexToRuleNameMap.end() ? wstring() : nodeIndexToRuleNameMap[index];
		}


		PDAEdge* PushDownAutoMachine::NewEdge(PDANode* source, PDANode* target, const ActionWrap& wrap)
		{
			edges.emplace_back(make_shared<PDAEdge>(wrap, source,target));
			return edges.back().get();
		}

		PDAEdge * PushDownAutoMachine::NewEdge(PDANode * source, PDANode * target)
		{
			edges.emplace_back(make_shared<PDAEdge>(source, target));
			return edges.back().get();
		}
	
	}
}