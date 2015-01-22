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

		unordered_map<GeneralRuleDefine*, vector<pair<PDANode*,PDANode*>>>& PushDownAutoMachine::GetPDAMap() 
		{
			return PDAMap;
		}

		void PushDownAutoMachine::AddEdge(PDANode * source, PDANode * target, const ActionWrap& wrap)
		{
			auto edge = NewEdge(source, target, wrap);
			source->nexts->emplace_back(edge);
			target->fronts->emplace_back(edge);
		}

		void PushDownAutoMachine::AddGeneratePDA(GeneralRuleDefine * rule, PDANode * start,PDANode* end)
		{
			auto findIter = PDAMap.find(rule);
			if(findIter == PDAMap.end())
			{
				PDAMap[rule];
			}
			PDAMap[rule].emplace_back(make_pair( start ,end));
		
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

			for(auto&& iter : *right->nexts)
			{
				iter->source = left;
				left->nexts->emplace_back(iter);
			}
			right->nexts->clear();
			for(auto&& iter : *right->fronts)
			{
				iter->target = left;
				left->fronts->emplace_back(iter);
			}
			right->fronts->clear();
			left->SetMergeFlag();
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

		PDAEdge* PushDownAutoMachine::NewEdge(PDANode* source, PDANode* target, const ActionWrap& wrap)
		{
			edges.emplace_back(make_shared<PDAEdge>(wrap, source,target));
			return edges.back().get();
		}
	
	}
}