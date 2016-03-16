#include "Include\stdafx.h"
#include "Include\GeneralLALRParser.h"
#include "Include\SymbolManager.h"
#include "Include\GeneralPushDownAutoMachine.h"
#include "Include\GeneralPushDownMachineData.h"
#include "Include\GeneralTreeNode.h"
#include "Include\ParserSymbol.h"
#include "Include\GeneralLRMachine.h"
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
				if (item.IsProductEndPosition()&& item.GetFollowTokens().find(currentSymbol) != item.GetFollowTokens().end())
				{
					result.push_back(&item);
				}
			}
			assert(result.size() == 1);
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
			/*	auto&& edgesIter = edgeStack[i];
				auto edgePair = GetPositionPreEdge(nextPosition, edgesIter);
				result.emplace_back(edgePair);
				nextPosition = edgePair.first->GetSource();
				if (edgePair.first->GetSource() == startNode)
				{
					return result;
				}*/
				
			}
			if (nextPosition == startNode)
			{
				return result;
			}
			assert(false);
			return{};
		}
		void GeneralLALRParser::ExecutableEdgeAddctionAction(const vector<pair<PDAEdge*, int>>& edges,GeneralTreeNode* node)
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
			/*for (auto&& edgeIter : edges)
			{
				ExcuteEdgeAdditionAction(edgeIter, node);
			}*/
		}
		void ReduceGrammarInStack( vector<pair<const vector<PDAEdge*>*, int>>&edgeStack, vector<LRNode*>& LRNodeStack,int num)
		{
			edgeStack.erase(edgeStack.end() - num, edgeStack.end());
			LRNodeStack.erase(LRNodeStack.end() - num, LRNodeStack.end());
		}
		void GeneralLALRParser::GenerateIsomorphismParserTree()
		{
			//int 是tokenIndex
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
				if (tokenIndex == 116)
				{
					int a = 0;
				}
				currentLRNode = LRNodeStack.back();
				auto resultPair = NeedMove(currentSymbol, currentLRNode);
				auto next = resultPair.first;
				auto edgesIter = resultPair.second;
				
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
					//else using
					//{
						
					//}
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
					auto oldSymbol = currentSymbol;
					currentSymbol = GetTokenSymbol(tokenIndex);
					assert(currentSymbol->IsTokenDef());
				}
			}

			/*auto currentLRNode = GetLRMachineStartNode();
			LRNodeStack.emplace_back(currentLRNode);*/
			//assert(currentLRNode->GetCoreNumber() == 1);
			//auto&& currentPDANode = currentLRNode->GetFirstProductPosition().GetPosition();
			//PDANodeStack.emplace_back(currentPDANode);
			//size_t tokenIndex = 0;
			//auto currentSymbol = GetTokenSymbol(tokenIndex);
			//this->LALRConfilctDetection();
			//while(!IsParserFinish(currentSymbol, GetTokenSymbol(tokenIndex)))
			//{
			//	if (tokenIndex == 5)
			//	{
			//		int a = 0;
			//	}
			//	currentLRNode = LRNodeStack.back();
			//	currentPDANode = PDANodeStack.back();
			//	assert(grammarStack.empty() || grammarStack.back().first->GetTarget() == currentPDANode);
			//	assert(LRNodeStack.back()->GetItemsMap().find(currentPDANode) != LRNodeStack.back()->GetItemsMap().end());
			//	auto findIter = NeedMove(currentPDANode, currentLRNode, currentSymbol);
			//	if(findIter.first == false)
			//	{
			//		//NeedReduce
			//		auto&& currentProduct = currentLRNode->GetProductByPDANode(currentPDANode);
			//		GeneralTreeNode* treeNode = nullptr;
			//		if(currentProduct.HasEndAction())
			//		{				
			//			treeNode = ExcuteEndAction(currentProduct.GetEndAction());
			//			assert(currentPDANode == grammarStack.back().first->GetTarget());
			//			auto nodeIndex = GetPools().GetGeneralNodePool().size() - 1;
			//			currentSymbol = ExceteReduceAction(currentPDANode, treeNode, nodeIndex);
			//			assert(currentSymbol->IsRuleDef());
			//		}
			//		else
			//		{
			//			//Using路径
			//			assert(currentPDANode == grammarStack.back().first->GetTarget());
			//			currentSymbol = ExceteReduceWithoutEndAction(currentPDANode);
			//			assert(currentSymbol->IsRuleDef());
			//		}
			//	}
			//	else
			//	{
			//		//Need Move;
			//		auto edge = findIter.second;
			//		grammarStack.emplace_back(edge, tokenIndex);
			//		currentPDANode = edge->GetTarget();
			//		PDANodeStack.emplace_back(currentPDANode);
			//		currentLRNode = currentLRNode->GetNextLRNode(currentSymbol);
			//		LRNodeStack.emplace_back(currentLRNode);
			//		if(currentSymbol->IsTokenDef())
			//		{
			//			++tokenIndex;
			//		}
			//		auto oldSymbol = currentSymbol;
			//		currentSymbol = GetTokenSymbol(tokenIndex);
			//		assert(currentSymbol->IsTokenDef());
			//	}
			//}
			assert(treeNodeStack.size() == 1);
		
			this->generalTreeRoot = GetPools().GetGeneralNodePool()[treeNodeStack[0]].get();
		}
		void GeneralLALRParser::ClearEnvironment()
		{
			GeneralParserBase::ClearEnvironment();
			this->grammarStack.clear();
			this->treeNodeStack.clear();
			this->LRNodeStack.clear();
			this->PDANodeStack.clear();
		}
		
		ParserSymbol* GeneralLALRParser::GetTokenSymbol(size_t tokenIndex) const
		{
			auto&& currentToken = GetPools().GetTokenPool()[tokenIndex].get();
			return GetManager()->GetTokenSymbolByName(currentToken->tag);
		}

		pair<bool, PDAEdge*> GeneralLALRParser::NeedMove(PDANode*node, LRNode* LRNode, ParserSymbol* symbol)
		{
			auto edges = LRNode->GetGotoEdges(symbol);
			if(edges == nullptr)
			{
				return{ false ,nullptr };
			}
			for(auto&& edgeIter : *edges)
			{
				auto&& index = LRNode->GetItemIndex(edgeIter->GetSource());
				if(index < LRNode->GetCoreNumber() && index == LRNode->GetItemIndex(node))
				{
					return{ true,edgeIter };
				}
				else if(index < LRNode->GetCoreNumber() && index != LRNode->GetItemIndex(node))
				{
					//修改左递归的位置
					auto newGrammarPosition = edgeIter->GetSource();
					auto&& product = LRNode->GetItems()[LRNode->GetItemIndex(newGrammarPosition)];
					auto&& newGrammarStart = LRMachine->GetRuleStart(LRMachine->GetRuleNameByIndex(product.GetRuleIndex()));
					auto&& oldProduct = LRNode->GetItems()[LRNode->GetItemIndex(node)];
					auto&& oldGrammarStart = LRMachine->GetRuleStart(LRMachine->GetRuleNameByIndex(oldProduct.GetRuleIndex()));
					auto&& oldGrammarPosition = node;
					auto&& newEdgeList = FindTheNodePathEdges(newGrammarStart, newGrammarPosition);
					assert(FindTheNodePathEdges(newGrammarStart, newGrammarPosition).size() <= FindTheNodePathEdges(oldGrammarStart, oldGrammarPosition).size());
					//这里可以优化
					if(FindTheNodePathEdges(newGrammarStart, newGrammarPosition).size() < FindTheNodePathEdges(oldGrammarStart, oldGrammarPosition).size())
					{
						//少了新的产生式的头结点如果不多插入一个节点,最后会多覆盖一个节点
						PDANodeStack.emplace_back(nullptr);
					}
					auto PDANodeStackIter = PDANodeStack.end() - newEdgeList.size() - 1;
					auto grammrIter = grammarStack.end() - newEdgeList.size();
					for(size_t i = 0; i < newEdgeList.size(); i++)
					{
						grammrIter->first = newEdgeList[i];
						assert(newEdgeList[i]->GetActions() == grammrIter->first->GetActions());
						*PDANodeStackIter = newEdgeList[i]->GetSource();
						++PDANodeStackIter;
						++grammrIter;
					}
					*PDANodeStackIter = newEdgeList.back()->GetTarget();
					return{ true,edgeIter };
				}
				else if(index >= LRNode->GetCoreNumber())
				{
					this->PDANodeStack.emplace_back(edgeIter->GetSource());
					return{ true,edgeIter };
				}
			}
			return{ false ,nullptr };
		}
		void GeneralLALRParser::ExcuteEdgeAdditionAction(const pair<PDAEdge*, int>& grammar, GeneralTreeNode* node)
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
		GeneralTreeNode* GeneralLALRParser::ExcuteEndAction(const vector<ActionWrap>& acionts)
		{
			assert(!acionts.empty());
			assert(acionts[0].GetActionType() == ActionType::Create);
			auto treeNode = MakeTreeNode(acionts[0].GetParserSymbol());
			if(acionts.size() > 1)
			{
				for(size_t i = 1; i < acionts.size(); i++)
				{
					assert(acionts[i].GetActionType() == ActionType::Setter);
					auto tokenIndex = GetPools().GetTokenPool().size();
					SetTokenPool(make_shared<TokenInfo>(acionts[i].GetValue(), L"Setter", -1, -1));
					treeNode->SetTermMap(acionts[i].GetName(), tokenIndex);
				}
			}
			return treeNode;
		}
		ParserSymbol* GeneralLALRParser::ExceteReduceAction(PDANode* currentPDANode, GeneralTreeNode* node, size_t nodeIndex)
		{
			assert(grammarStack.back().first->GetTarget() == currentPDANode);
			assert(LRNodeStack.back()->GetItemsMap().find(currentPDANode) != LRNodeStack.back()->GetItemsMap().end());
			assert(currentPDANode == grammarStack.back().first->GetTarget());
			auto backIndex = grammarStack.size() - 1;
			size_t i = 0;
			for(; i < backIndex + 1 && currentPDANode == grammarStack[backIndex - i].first->GetTarget(); i++)
			{
				auto&&current = grammarStack[backIndex - i];
				auto&& edge = current.first;
				assert(edge->GetActions().size() != 0);
				if(edge->GetActions().size() == 2)
				{
					ExcuteEdgeAdditionAction(current, node);
				}
				currentPDANode = current.first->GetSource();
			
			}
			this->grammarStack.erase(grammarStack.end() - i, grammarStack.end());
			this->LRNodeStack.erase(LRNodeStack.end() - i, LRNodeStack.end());
			this->PDANodeStack.erase(PDANodeStack.end() - (i), PDANodeStack.end());
			auto ruleIndex = LRNodeStack.back()->GetProductByPDANode(currentPDANode).GetRuleIndex();
			auto&& ruleName = GetManager()->GetRuleNameByIndex(ruleIndex);
			auto ruleSymbol = GetManager()->GetRuleSymbolByName(ruleName);
			assert(ruleSymbol->IsRuleDef());
			node->AdjustFieldSequence();
			treeNodeStack.emplace_back(nodeIndex);

			assert(grammarStack.empty() || grammarStack.back().first->GetTarget() == PDANodeStack.back());
			assert(PDANodeStack.empty() || LRNodeStack.back()->GetItemsMap().find(PDANodeStack.back()) != LRNodeStack.back()->GetItemsMap().end());
			return ruleSymbol;
		}
		ParserSymbol * GeneralLALRParser::ExceteReduceWithoutEndAction(PDANode * currentPDANode)
		{
			assert(grammarStack.back().first->GetTarget() == currentPDANode);
			assert(LRNodeStack.back()->GetItemsMap().find(currentPDANode) != LRNodeStack.back()->GetItemsMap().end());
			assert(currentPDANode == grammarStack.back().first->GetTarget());
			auto backIndex = grammarStack.size() - 1;
			size_t i = 0;
			for(; i < backIndex + 1 && currentPDANode == grammarStack[backIndex - i].first->GetTarget(); i++)
			{
				auto&&current = grammarStack[backIndex - i];
				auto&& edge = current.first;
				assert(edge->GetActions().size() != 0);
				currentPDANode = current.first->GetSource();
			}
			this->grammarStack.erase(grammarStack.end() - i, grammarStack.end());
			this->LRNodeStack.erase(LRNodeStack.end() - i, LRNodeStack.end());
			this->PDANodeStack.erase(PDANodeStack.end() - (i +1), PDANodeStack.end());
			auto ruleIndex = LRNodeStack.back()->GetProductByPDANode(currentPDANode).GetRuleIndex();
			auto&& ruleName = GetManager()->GetRuleNameByIndex(ruleIndex);
			auto ruleSymbol = GetManager()->GetRuleSymbolByName(ruleName);
			assert(ruleSymbol->IsRuleDef());
			assert(grammarStack.empty() || grammarStack.back().first->GetTarget() == PDANodeStack.back());
			assert(PDANodeStack.empty() || LRNodeStack.back()->GetItemsMap().find(PDANodeStack.back()) != LRNodeStack.back()->GetItemsMap().end());
			return ruleSymbol;
		}
	
		bool GeneralLALRParser::IsParserFinish(ParserSymbol* node, ParserSymbol*tokenSymbol) const
		{
			return  GetRootRuleSymbol() == node&&treeNodeStack.size() == 1&& tokenSymbol == manager->GetFinishTokenSymbol();
		}
		GeneralLALRParser::GeneralLALRParser(const shared_ptr<GeneralTableDefine>& _tableDefine) :GeneralParserBase(_tableDefine)
		{
		}

		const vector<PDAEdge*>& GeneralLALRParser::FindTheNodePathEdges(PDANode* start, PDANode* end)
		{
			auto&& key = make_pair(start, end);
			auto&& findIter = findPathCacheMap.find(key);
			if(findIter == findPathCacheMap.end())
			{
				auto&& edges = FindThePath(start, end);
				findPathCacheMap.insert({ make_pair(move(key),move(edges)) });
			}
			return findPathCacheMap[key];
		}
		
	}
}