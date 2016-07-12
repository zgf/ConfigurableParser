#include "Include/stdafx.h"
#include "Include/GeneralTableDefine.h"
#include "Include/GeneralGrammarBuilder.h"
#include "Include/SymbolManager.h"
#include "Include/ParserSymbol.h"
#include "Include/GeneralGrammarBuilderData.h"
namespace ztl
{
	namespace general_parser
	{
		const wstring& ActionWrap::GetName()const
		{
			return data->GetName();
		}
		GrammarBuilder::GrammarBuilder() :manager(nullptr)
		{
		}
		GrammarBuilder::GrammarBuilder(SymbolManager* _manager)
			: manager(_manager)
		{
		}

		SymbolManager * GrammarBuilder::GetSymbolManager() const
		{
			return this->manager;
		}

		GeneralTableDefine * GrammarBuilder::GetTable() const
		{
			return GetSymbolManager()->GetTable();
		}

		unordered_map<wstring, pair<PDANode*, PDANode*>>& GrammarBuilder::GetPDAMap()
		{
			return PDAMap;
		}

		void GrammarBuilder::AddEdge(PDANode * source, PDANode * target, const ActionWrap & wrap)
		{
			auto edge = NewEdge(source, target, wrap);
			source->nexts->push_back(edge);
			target->fronts->push_back(edge);
		}

		void GrammarBuilder::AddEdge(PDANode* source, PDANode* target, const vector<ActionWrap>& wrapList)
		{
			auto edge = NewEdge(source, target);
			edge->actions = wrapList;
			source->nexts->push_back(edge);
			target->fronts->push_back(edge);
		}

		void GrammarBuilder::DeleteEdge(PDAEdge* target)
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

		void GrammarBuilder::AddGeneratePDA(wstring ruleName, const pair<PDANode *, PDANode*>& pairNode)
		{
			auto findIter = PDAMap.find(ruleName);
			assert(findIter == PDAMap.end());
			PDAMap.insert(make_pair(ruleName, pairNode));
		}

		pair<PDANode*, PDANode*> GrammarBuilder::NewNodePair()
		{
			return{ NewNode(), NewNode() };
		}

		PDANode * GrammarBuilder::NewNode()
		{
			static int count = 0;
			++count;
			nodes.emplace_back(make_shared<PDANode>());
			nodes.back()->number = count;
			return nodes.back().get();
		}

		PDANode * GrammarBuilder::GetRoot() const
		{
			assert(!PDAMap.empty());
			auto findIter = PDAMap.find(this->GetRootRuleName());
			assert(findIter != PDAMap.end());
			return findIter->second.first;
		}

		pair<PDANode*, PDANode*> GrammarBuilder::AddSequenceLinkNode(pair<PDANode*, PDANode*>& left, pair<PDANode*, PDANode*>& right)
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
		
		pair<PDANode*, PDANode*> GrammarBuilder::AddLoopLinkNode(PDANode * loopStart, PDANode * loopEnd)
		{
			auto mergeNode = MergeIndependentNodes(loopEnd, loopStart);
			return pair<PDANode*, PDANode*>(mergeNode, mergeNode);
		}

		pair<PDANode*, PDANode*> GrammarBuilder::AddAlterationLinkNode(pair<PDANode*, PDANode*>& left, pair<PDANode*, PDANode*>& right)
		{
			MergeIndependentNodes(left.first, right.first);
			MergeIndependentNodes(left.second, right.second);
			return{ left.first,left.second };
		}

		pair<PDANode*, PDANode*> GrammarBuilder::AddOptionalLinkNode(PDANode* optionalStart, PDANode* optionalEnd)
		{
			auto mergeNode = MergeIndependentNodes(optionalStart, optionalEnd);

			return pair<PDANode*, PDANode*>(mergeNode, mergeNode);
		}

		void GrammarBuilder::FrontEdgesAdditionBackAction(PDANode* targetNode, const ActionWrap& wrap)
		{
			for(auto&& edgeIter : *targetNode->fronts)
			{
				this->BackInsertAction(edgeIter, wrap);
			}
		}
		void GrammarBuilder::FrontEdgesAdditionSetterAction(PDANode* targetNode, const ActionWrap& wrap)
		{
			FrontEdgesAdditionBackAction(targetNode, wrap);
			for(auto&& edgeIter : *targetNode->fronts)
			{
				swap(edgeIter->actions.back(), edgeIter->actions[edgeIter->actions.size() - 2]);
				assert(edgeIter->actions.back().GetActionType() == ActionType::Create);
			}
		}

		PDANode * GrammarBuilder::MergeIndependentNodes(PDANode * left, PDANode * right)
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

		void GrammarBuilder::BackInsertAction(PDAEdge* edge, const ActionWrap& wrap)
		{
			edge->actions.emplace_back(wrap);
		}

		void GrammarBuilder::FrontInsertAction(PDAEdge * edge, const ActionWrap & wrap)
		{
			edge->actions.emplace(edge->actions.begin(), wrap);
		}

		void GrammarBuilder::AddNodeMapElement(PDANode * node, int ruleIndex)
		{
			assert(nodeMap.find(node) == nodeMap.end());
			nodeMap.insert({ node,ruleIndex });
		}

		int GrammarBuilder::GetRuleIndexByNode(PDANode * node) const
		{
			auto findIter = nodeMap.find(node);
			assert(findIter != nodeMap.end());
			return findIter->second;
		}

		void GrammarBuilder::AddNodeEdgeMapElement(PDANode * node, PDAEdge * edge)
		{
			auto&& findIter = nodeEdgeMap.find(node);
			if (findIter == nodeEdgeMap.end())
			{
				nodeEdgeMap[node];
			}
			nodeEdgeMap[node].emplace_back(edge);
		}

		const vector<PDAEdge*>& GrammarBuilder::GetEdgesByNode(PDANode * node) const
		{
			auto findIter = nodeEdgeMap.find(node);
			assert(findIter != nodeEdgeMap.end());
			return findIter->second;
		}

		

		PDAEdge* GrammarBuilder::NewEdge(PDANode* source, PDANode* target, const ActionWrap& wrap)
		{
			edges.emplace_back(make_shared<PDAEdge>(wrap, source, target, (int) edges.size()));
			return edges.back().get();
		}

		PDAEdge * GrammarBuilder::NewEdge(PDANode * source, PDANode * target)
		{
			edges.emplace_back(make_shared<PDAEdge>(source, target, (int) edges.size()));
			return edges.back().get();
		}

		wstring	 GrammarBuilder::GetRootRuleName()const
		{
			assert(manager->GetRootSymbol()!=nullptr);
			return manager->GetRootSymbol()->GetName();
		}


	
	}
}