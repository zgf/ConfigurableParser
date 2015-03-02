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

		unordered_map<wstring, pair<PDANode*,PDANode*>>& PushDownAutoMachine::GetPDAMap()
		{
			return PDAMap;
		}

		void PushDownAutoMachine::AddEdge(PDANode * source, PDANode * target, const ActionWrap& wrap)
		{
			auto edge = NewEdge(source, target, wrap);
			source->nexts->push_back(edge);
			target->fronts->push_back(edge);
		}

		void PushDownAutoMachine::AddEdge(PDANode * source, PDANode * target, const vector<ActionWrap>& wrapList)
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

		void PushDownAutoMachine::AddGeneratePDA(wstring ruleName,const pair<PDANode *,PDANode*>& pairNode)
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
			assert(root != nullptr);
			return root;
		}

		void PushDownAutoMachine::SetRoot(PDANode * val)
		{
			root = val;
		}

		pair<PDANode*, PDANode*> PushDownAutoMachine::AddSequenceLinkNode(pair<PDANode*, PDANode*>& left, pair<PDANode*, PDANode*>& right)
		{
			auto mergeNode = MergeIndependentNodes(left.second, right.first);
			if (right.first == right.second)
			{
				return pair<PDANode*, PDANode*>(left.first, mergeNode);
			}
			else
			{
				return pair<PDANode*, PDANode*>(left.first, right.second);
			}
		}
		void	PushDownAutoMachine::AddFinishNodeFollowTarget(PDANode* target)
		{
			for (auto&& frontIter:target->GetFronts())
			{
				if(find_if(frontIter->GetActions().begin(), frontIter->GetActions().end(), [](const ActionWrap& wrap)
				{
					return wrap.GetActionType() == ActionType::Create;
				}) != frontIter->GetActions().end())
				{
					BackInsertAction(frontIter, ActionWrap(ActionType::Terminate, L"<$>", L""));
				}
			}
		}
		pair<PDANode*, PDANode*> PushDownAutoMachine::AddLoopLinkNode(PDANode * loopStart, PDANode * loopEnd)
		{
			auto mergeNode = MergeIndependentNodes(loopEnd, loopStart);
			return pair<PDANode*, PDANode*>(mergeNode, mergeNode);
		}

		pair<PDANode*, PDANode*> PushDownAutoMachine::AddAlterationLinkNode(pair<PDANode*, PDANode*>& left, pair<PDANode*, PDANode*>& right)
		{
			MergeIndependentNodes(left.first,right.first );
			MergeIndependentNodes(left.second, right.second);
			return{left.first,left.second};
		}

		pair<PDANode*, PDANode*> PushDownAutoMachine::AddOptionalLinkNode(PDANode* optionalStart, PDANode* optionalEnd)
		{
			auto mergeNode = MergeIndependentNodes(optionalStart, optionalEnd);

			return pair<PDANode*, PDANode*>(mergeNode, mergeNode);
		}

		void PushDownAutoMachine::FrontEdgesAdditionBackAction(PDANode* targetNode, const ActionWrap& wrap)
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
			edge->actions.emplace_back(wrap);
		}



		void PushDownAutoMachine::CreateRoot()
		{
			assert(GetPDAMap().size() != 0);
			root = this->GetPDAMap()[this->GetRootRuleName()].first;
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

		wstring	 PushDownAutoMachine::GetRootRuleName()const
		{
			assert(!this->GetSymbolManager()->GetStartRuleList().empty());
			return this->GetSymbolManager()->GetStartRuleList()[0];
		}
	}

}