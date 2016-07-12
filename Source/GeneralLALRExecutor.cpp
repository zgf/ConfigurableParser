#include "Include\stdafx.h"
#include "Include\GeneralLALRExecutor.h"
#include "Include\SymbolManager.h"
#include "Include\GeneralGrammarBuilder.h"
#include "Include\GeneralGrammarBuilderData.h"
#include "Include\GeneralTreeNode.h"
#include "Include\ParserSymbol.h"
#include "Include\GeneralLRBuilder.h"
namespace ztl
{
	namespace general_parser
	{
	
		const ProductPosition* GetReducableProductPosition(ParserSymbol* currentSymbol, LRNode* currentLRNode)
		{
			vector<const ProductPosition*> result;
			auto&& items = currentLRNode->GetItems();
			for (auto&&item : items)
			{
				if (item.IsProductEndPosition()&& 
					item.GetFollowTokens().find(currentSymbol) != item.GetFollowTokens().end())
				{
					result.push_back(&item);
				}
			}
			if (result.size() != 1)
			{
				int a = 0;
			}
			return  result[0];
		}
		pair<PDAEdge*, int> GetPositionPreEdge(PDANode* position,const pair<const vector<PDAEdge*>*, int>& edgesPair)
		{
			auto&& edges = edgesPair.first;
			auto&&tokenIndex = edgesPair.second;
			vector<pair<PDAEdge*, int> >result;
			for (auto&&edge : *edges)
			{
				if (edge->GetTarget() == position)
				{
					result.emplace_back(make_pair(edge, tokenIndex));
				}
			}
			assert(result.size() <= 1);
			if (result.empty())
			{
				return{nullptr,0};
			}
			else
			{
				return result[0];
			}
		}
	
		vector<pair<PDAEdge*, int>> CollectWaitExecuteEdges(PDANode*startNode, PDANode* endPosition,const vector<pair<const vector<PDAEdge*>*, int>>& edgeStack)
		{
			vector<pair<PDAEdge*, int>> result;
			auto nextPosition = endPosition;
			for (int i = edgeStack.size() - 1; i >= 0; i--)
			{
				auto&& edgesIter = edgeStack[i];
				auto edgePair = GetPositionPreEdge(nextPosition, edgesIter);
				if (edgePair.first != nullptr)
				{
					result.emplace_back(edgePair);
					nextPosition = edgePair.first->GetSource();

				}
				else
				{
					return result;
				}
			}
			if (nextPosition == startNode)
			{
				return result;
			}
			assert(false);
			return{};
		}
		
		void GeneralLALRExecutor::ExecutableEdgeAddctionAction(const vector<pair<PDAEdge*, int>>& edges,GeneralTreeNode* node)
		{
			vector<pair<PDAEdge*, int>> execute;
			
			for (auto&& edgeIter : edges)
			{
				if (edgeIter.first->GetActions().size() == 2&&edgeIter.first->HasNonTermActionType())
				{
					execute.emplace_back(edgeIter);
				}
				
			}
			auto count = execute.size();
			auto rbegin = treeNodeStack.rbegin();
			auto rend = rbegin+count;
			std::reverse(rbegin, rend);
			for (int i = edges.size() - 1; i >= 0; i--)
			{
				auto&&edgeIter = edges[i];
				ExcuteEdgeAdditionAction(edgeIter, node);
			}
		
		}
		void GeneralLALRExecutor::ExcuteEdgeAdditionAction(const pair<PDAEdge*, int>& grammar, GeneralTreeNode* node)
		{
			auto&& edge = grammar.first;
			auto&& tokenIndex = grammar.second;
			auto&& actions = edge->GetActions();
			if (actions.size() == 2)
			{
				assert(actions[0].GetActionType() == ActionType::NonTerminate || actions[0].GetActionType() == ActionType::Terminate);
				auto&& actionIter = actions[1];
				assert(actions[1].GetActionType() == ActionType::Assign);
				if (actions[0].GetActionType() == ActionType::NonTerminate)
				{
					node->SetFieldMap(actionIter.GetName(), treeNodeStack.back());
					treeNodeStack.pop_back();
				}
				else if (actions[0].GetActionType() == ActionType::Terminate)
				{
					node->SetTermMap(actionIter.GetName(), tokenIndex);
				}
			}
		}
		GeneralTreeNode* GeneralLALRExecutor::ExcuteEndAction(const vector<ActionWrap>& acionts)
		{
			assert(!acionts.empty());
			assert(acionts[0].GetActionType() == ActionType::Create);
			auto treeNode = MakeTreeNode(acionts[0].GetParserSymbol());
			if (acionts.size() > 1)
			{
				for (size_t i = 1; i < acionts.size(); i++)
				{
					assert(acionts[i].GetActionType() == ActionType::Setter);
					auto tokenIndex = GetPools().GetTokenPool().size();
					SetTokenPool(make_shared<TokenInfo>(acionts[i].GetValue(), L"Setter", -1, -1));
					treeNode->SetTermMap(acionts[i].GetName(), tokenIndex);
				}
			}
			return treeNode;
		}

		void ReduceGrammarInStack( vector<pair<const vector<PDAEdge*>*, int>>&edgeStack, vector<LRNode*>& LRNodeStack,int num)
		{
			edgeStack.erase(edgeStack.end() - num, edgeStack.end());
			LRNodeStack.erase(LRNodeStack.end() - num, LRNodeStack.end());
		}
		void GeneralLALRExecutor::GenerateIsomorphismParserTree()
		{
			//int ÊÇtokenIndex
			vector<pair<const vector<PDAEdge*>*,int>> edgeStack;
			auto currentLRNode = GetLRMachineStartNode();
			LRNodeStack.emplace_back(currentLRNode);
			size_t tokenIndex = 0;
			auto currentSymbol = GetTokenSymbol(tokenIndex);
			auto NeedMove = [](ParserSymbol* currentSymbol,LRNode* currentLRNode)
			{
				auto next = currentLRNode->GetNextLRNode(currentSymbol);
				auto edges = currentLRNode->GetGotoEdges(currentSymbol);
				return make_pair(next,edges);
			};
			while (!IsParserFinish(currentSymbol, GetTokenSymbol(tokenIndex)))
			{
				currentLRNode = LRNodeStack.back();
				LRNode* next;
				const vector<PDAEdge*>* edgesIter;
				std::tie(next, edgesIter)  = NeedMove(currentSymbol, currentLRNode);
				if (next==nullptr)
				{
					//REDUCE
					auto reducablePosition = GetReducableProductPosition(currentSymbol, currentLRNode);
					assert(reducablePosition != nullptr);
					auto startNode = GetMachine().GetStartNodeByProduction(reducablePosition);
					auto headRuleSymbol = GetMachine().GetProductHeadRuleSymbolByPosition(reducablePosition);
					assert(startNode != nullptr);
					auto executableEdges = CollectWaitExecuteEdges(startNode, reducablePosition->GetPosition(), edgeStack);
					if (reducablePosition->HasEndAction())
					{
						//create
						auto treeNode = ExcuteEndAction(reducablePosition->GetEndAction());
						auto nodeName = treeNode->GetName();
						auto nodeIndex = GetPools().GetGeneralNodePool().size() - 1;
						ExecutableEdgeAddctionAction(executableEdges, treeNode);
						treeNodeStack.emplace_back(nodeIndex);
					}
					ReduceGrammarInStack(edgeStack, LRNodeStack,executableEdges.size());
					currentSymbol = headRuleSymbol;
				}
				else
				{
					//MOVE
					assert(edgesIter != nullptr);
					LRNodeStack.emplace_back(next);
					edgeStack.emplace_back(edgesIter,tokenIndex);
					if (currentSymbol->IsTokenDef())
					{
						++tokenIndex;
					}
					currentSymbol = GetTokenSymbol(tokenIndex);
					assert(currentSymbol->IsTokenDef());
				}
			}
			assert(treeNodeStack.size() == 1);
			this->generalTreeRoot = GetPools().GetGeneralNodePool()[treeNodeStack[0]].get();
		}
		void GeneralLALRExecutor::ClearEnvironment()
		{
			GeneralLRExecutor::ClearEnvironment();
			this->treeNodeStack.clear();
			this->LRNodeStack.clear();
		}
		
		ParserSymbol* GeneralLALRExecutor::GetTokenSymbol(size_t tokenIndex) const
		{
			auto&& currentToken = GetPools().GetTokenPool()[tokenIndex].get();
			return GetManager()->GetTokenSymbolByName(currentToken->tag);
		}

	
	
		bool GeneralLALRExecutor::IsParserFinish(ParserSymbol* node, ParserSymbol*tokenSymbol) const
		{
			return  GetRootRuleSymbol() == node&&treeNodeStack.size() == 1&& tokenSymbol == manager->GetFinishTokenSymbol();
		}
		GeneralLALRExecutor::GeneralLALRExecutor(const shared_ptr<GeneralTableDefine>& _tableDefine) :GeneralLRExecutor(_tableDefine)
		{
		}
	}
}