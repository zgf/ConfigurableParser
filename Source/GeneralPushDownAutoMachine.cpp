#include "Include/stdafx.h"
#include "Include/GeneralTableDefine.h"
#include "Include/GeneralPushDownAutoMachine.h"
#include "Include/SymbolManager.h"
#include "Include/ParserSymbol.h"
#include "Include/GeneralPushDownMachineData.h"
namespace ztl
{
	namespace general_parser
	{
	
		PushDownAutoMachine::PushDownAutoMachine() :manager(nullptr)
		{
		}
		PushDownAutoMachine::PushDownAutoMachine(SymbolManager* _manager)
			: manager(_manager)
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

		unordered_map<wstring, pair<PDANode*, PDANode*>>& PushDownAutoMachine::GetPDAMap()
		{
			return PDAMap;
		}

		void PushDownAutoMachine::AddEdge(PDANode * source, PDANode * target, const ActionWrap & wrap)
		{
			auto edge = NewEdge(source, target, wrap);
			source->nexts->push_back(edge);
			target->fronts->push_back(edge);
		}

		void PushDownAutoMachine::AddEdge(PDANode* source, PDANode* target, const vector<ActionWrap>& wrapList)
		{
			auto edge = NewEdge(source, target);
			edge->actions = wrapList;
			source->nexts->push_back(edge);
			target->fronts->push_back(edge);
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

		void PushDownAutoMachine::AddGeneratePDA(wstring ruleName, const pair<PDANode *, PDANode*>& pairNode)
		{
			auto findIter = PDAMap.find(ruleName);
			assert(findIter == PDAMap.end());
			PDAMap.insert(make_pair(ruleName, pairNode));
		}

		pair<PDANode*, PDANode*> PushDownAutoMachine::NewNodePair()
		{
			return{ NewNode(), NewNode() };
		}

		PDANode * PushDownAutoMachine::NewNode()
		{
			static int count = 0;
			++count;
			nodes.emplace_back(make_shared<PDANode>());
			nodes.back()->number = count;
			return nodes.back().get();
		}

		PDANode * PushDownAutoMachine::GetRoot() const
		{
			assert(!PDAMap.empty());
			auto findIter = PDAMap.find(this->GetRootRuleName());
			assert(findIter != PDAMap.end());
			return findIter->second.first;
		}

		pair<PDANode*, PDANode*> PushDownAutoMachine::AddSequenceLinkNode(pair<PDANode*, PDANode*>& left, pair<PDANode*, PDANode*>& right)
		{
			auto mergeNode = MergeIndependentNodes(left.second, right.first);
			if(right.first == right.second)
			{
				return pair<PDANode*, PDANode*>(left.first, mergeNode);
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
			MergeIndependentNodes(left.first, right.first);
			MergeIndependentNodes(left.second, right.second);
			return{ left.first,left.second };
		}

		pair<PDANode*, PDANode*> PushDownAutoMachine::AddOptionalLinkNode(PDANode* optionalStart, PDANode* optionalEnd)
		{
			auto mergeNode = MergeIndependentNodes(optionalStart, optionalEnd);

			return pair<PDANode*, PDANode*>(mergeNode, mergeNode);
		}

		void PushDownAutoMachine::FrontEdgesAdditionBackAction(PDANode* targetNode, const ActionWrap& wrap)
		{
			for(auto&& edgeIter : *targetNode->fronts)
			{
				this->BackInsertAction(edgeIter, wrap);
			}
		}
		void PushDownAutoMachine::FrontEdgesAdditionSetterAction(PDANode* targetNode, const ActionWrap& wrap)
		{
			FrontEdgesAdditionBackAction(targetNode, wrap);
			for(auto&& edgeIter : *targetNode->fronts)
			{
				swap(edgeIter->actions.back(), edgeIter->actions[edgeIter->actions.size() - 2]);
				assert(edgeIter->actions.back().GetActionType() == ActionType::Create);
			}
		}

		PDANode * PushDownAutoMachine::MergeIndependentNodes(PDANode * left, PDANode * right)
		{
			if(left != right)
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
			}
			return left;
		}

		void PushDownAutoMachine::BackInsertAction(PDAEdge* edge, const ActionWrap& wrap)
		{
			edge->actions.emplace_back(wrap);
		}

		void PushDownAutoMachine::FrontInsertAction(PDAEdge * edge, const ActionWrap & wrap)
		{
			edge->actions.emplace(edge->actions.begin(), wrap);
		}

		void PushDownAutoMachine::AddNodeMapElement(PDANode * node, int ruleIndex)
		{
			assert(nodeMap.find(node) == nodeMap.end());
			nodeMap.insert({ node,ruleIndex });
		}

		int PushDownAutoMachine::GetRuleIndexByNode(PDANode * node) const
		{
			auto findIter = nodeMap.find(node);
			assert(findIter != nodeMap.end());
			return findIter->second;
		}

		void PushDownAutoMachine::AddNodeEdgeMapElement(PDANode * node, PDAEdge * edge)
		{
			auto&& findIter = nodeEdgeMap.find(node);
			if (findIter == nodeEdgeMap.end())
			{
				nodeEdgeMap[node];
			}
			nodeEdgeMap[node].emplace_back(edge);
		}

		const vector<PDAEdge*>& PushDownAutoMachine::GetEdgesByNode(PDANode * node) const
		{
			auto findIter = nodeEdgeMap.find(node);
			assert(findIter != nodeEdgeMap.end());
			return findIter->second;
		}

		

		PDAEdge* PushDownAutoMachine::NewEdge(PDANode* source, PDANode* target, const ActionWrap& wrap)
		{
			edges.emplace_back(make_shared<PDAEdge>(wrap, source, target, (int) edges.size()));
			return edges.back().get();
		}

		PDAEdge * PushDownAutoMachine::NewEdge(PDANode * source, PDANode * target)
		{
			edges.emplace_back(make_shared<PDAEdge>(source, target, (int) edges.size()));
			return edges.back().get();
		}

		wstring	 PushDownAutoMachine::GetRootRuleName()const
		{
			assert(manager->GetRootSymbol()!=nullptr);
			return manager->GetRootSymbol()->GetName();
		}


	
	}
}