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
		void GeneralLALRParser::GenerateIsomorphismParserTree()
		{
			auto currentLRNode = GetLRMachineStartNode();
			LRNodeStack.emplace_back(currentLRNode);
			assert(currentLRNode->GetCoreNumber() == 1);
			auto&& currentPDANode = currentLRNode->GetFirstProductPosition().GetPosition();
			PDANodeStack.emplace_back(currentPDANode);
			size_t tokenIndex = 0;
			auto currentSymbol = GetTokenSymbol(tokenIndex);
			while(!IsParserFinish(tokenIndex, currentSymbol))
			{
				currentLRNode = LRNodeStack.back();
				currentPDANode = PDANodeStack.back();
				assert(grammarStack.empty() || grammarStack.back().first->GetTarget() == currentPDANode);
				assert(LRNodeStack.back()->GetItemsMap().find(currentPDANode) != LRNodeStack.back()->GetItemsMap().end());
				auto findIter = NeedMove(currentPDANode, currentLRNode, currentSymbol);
				if(findIter.first == false)
				{
					//NeedReduce
					auto&& currentProduct = currentLRNode->GetProductByPDANode(currentPDANode);
					GeneralTreeNode* treeNode = nullptr;
					if(currentProduct.HasEndAction())
					{
						treeNode = ExcuteEndAction(currentProduct.GetEndAction());
						assert(currentPDANode == grammarStack.back().first->GetTarget());
						auto nodeIndex = GetPools().GetGeneralNodePool().size() - 1;
						currentSymbol = ExceteReduceAction(currentPDANode, treeNode, nodeIndex);
						assert(currentSymbol->IsRuleDef());
					}
					else
					{
						//Using路径
						assert(currentPDANode == grammarStack.back().first->GetTarget());
						currentSymbol = ExceteReduceWithoutEndAction(currentPDANode);
						assert(currentSymbol->IsRuleDef());
					}
					//currentPDANode被修改.
				}
				else
				{
					//Need Move;
					auto edge = findIter.second;
					grammarStack.emplace_back(edge, tokenIndex);
					currentPDANode = edge->GetTarget();
					PDANodeStack.emplace_back(currentPDANode);
					currentLRNode = currentLRNode->GetNextLRNode(currentSymbol);
					LRNodeStack.emplace_back(currentLRNode);
					if(currentSymbol->IsTokenDef())
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

		ParserSymbol* GeneralLALRParser::GetTokenSymbol(int tokenIndex) const
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
					auto grammrIter = grammarStack.end() - newEdgeList.size();
					for(size_t i = 0; i < newEdgeList.size(); i++)
					{
						grammrIter->first = newEdgeList[i];
						assert(newEdgeList[i]->GetActions() == grammrIter->first->GetActions());
						++grammrIter;
					}
					//这里可以优化
					if(FindTheNodePathEdges(newGrammarStart, newGrammarPosition).size() < FindTheNodePathEdges(oldGrammarStart, oldGrammarPosition).size())
					{
						//少了新的产生式的头结点如果不多插入一个节点,最后会多覆盖一个节点
						PDANodeStack.emplace_back(nullptr);
					}
					auto&& newNodeList = FindTheNodePathNodes(newGrammarStart, newGrammarPosition);
					auto PDANodeStackIter = PDANodeStack.end() - newEdgeList.size() - 1;
					for(size_t i = 0; i < newNodeList.size(); i++)
					{
						*PDANodeStackIter = newNodeList[i];
						++PDANodeStackIter;
					}
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
			assert(actions.size() == 2);
			assert(actions[0].GetActionType() == ActionType::NonTerminate || actions[0].GetActionType() == ActionType::Terminate);
			auto&& actionIter = actions[1];
			assert(actions[1].GetActionType() == ActionType::Assign);
			if(actions[0].GetActionType() == ActionType::NonTerminate)
			{
				node->SetFieldMap(actionIter.GetName(), treeNodeStack.back());
				treeNodeStack.pop_back();
			}
			else if(actions[0].GetActionType() == ActionType::Terminate)
			{
				auto setterIndex = GetPools().GetTerminatePool().size();
				SetTerminatePool(GetPools().GetTokenPool()[tokenIndex]);
				node->SetTermMap(actionIter.GetName(), setterIndex);
			}
		}
		GeneralTreeNode* GeneralLALRParser::ExcuteEndAction(const vector<ActionWrap>& acionts)
		{
			assert(!acionts.empty());
			assert(acionts[0].GetActionType() == ActionType::Create);
			auto nodeIndex = GetPools().GetGeneralNodePool().size();
			auto treeNode = MakeTreeNode(acionts[0].GetParserSymbol());
			if(acionts.size() > 1)
			{
				for(size_t i = 1; i < acionts.size(); i++)
				{
					assert(acionts[i].GetActionType() == ActionType::Setter);
					auto setterIndex = GetPools().GetTerminatePool().size();
					SetTerminatePool(make_shared<TokenInfo>(acionts[i].GetValue(), L"Setter", -1, -1));
					treeNode->SetTermMap(acionts[i].GetName(), setterIndex);
				}
			}
			return treeNode;
		}
		ParserSymbol* GeneralLALRParser::ExceteReduceAction(PDANode* currentPDANode, GeneralTreeNode* node, int nodeIndex)
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
				auto&& tokenIndex = current.second;
				assert(edge->GetActions().size() != 0);
				if(edge->GetActions().size() == 2)
				{
					ExcuteEdgeAdditionAction(current, node);
				}

				currentPDANode = current.first->GetSource();
			
			}
			this->grammarStack.erase(grammarStack.end() - i, grammarStack.end());
			this->LRNodeStack.erase(LRNodeStack.end() - i, LRNodeStack.end());
			this->PDANodeStack.erase(PDANodeStack.end() - (i + 1), PDANodeStack.end());
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
				auto&& tokenIndex = current.second;
				assert(edge->GetActions().size() != 0);
				currentPDANode = current.first->GetSource();
				grammarStack.pop_back();
				LRNodeStack.pop_back();
				PDANodeStack.pop_back();
			}
			PDANodeStack.pop_back();
			auto ruleIndex = LRNodeStack.back()->GetProductByPDANode(currentPDANode).GetRuleIndex();
			auto&& ruleName = GetManager()->GetRuleNameByIndex(ruleIndex);
			auto ruleSymbol = GetManager()->GetRuleSymbolByName(ruleName);
			assert(ruleSymbol->IsRuleDef());
			assert(grammarStack.empty() || grammarStack.back().first->GetTarget() == PDANodeStack.back());
			assert(PDANodeStack.empty() || LRNodeStack.back()->GetItemsMap().find(PDANodeStack.back()) != LRNodeStack.back()->GetItemsMap().end());
			return ruleSymbol;
		}
		bool GeneralLALRParser::IsParserFinish(size_t tokenIndex, ParserSymbol* symbol) const
		{
			return  (tokenIndex == GetPools().GetTokenPool().size() - 1) && GetRootRuleSymbol() == symbol;
		}
		GeneralLALRParser::GeneralLALRParser(const wstring & fileName, const shared_ptr<GeneralTableDefine>& _tableDefine) :GeneralParserBase(fileName, _tableDefine)
		{
		}

		const pair<vector<PDAEdge*>, vector<PDANode*>>& GeneralLALRParser::FindTheNodePath(PDANode* start, PDANode* end)
		{
			auto&& key = make_pair(start, end);
			auto&& findIter = findPathCacheMap.find(key);
			if (findIter ==findPathCacheMap.end())
			{
				auto&& edges = FindThePath(start, end);
				vector<PDANode*> nodes;
				//findPathCacheMap.insert({key,make_pair()})
				for (auto&& edgeIter:edges)
				{
					nodes.emplace_back(edgeIter->GetSource());
				}
				nodes.emplace_back(edges.back()->GetTarget());
				findPathCacheMap.insert({make_pair(move(key),make_pair(move(edges),move(nodes)))});
			}
			return findPathCacheMap[key];
		}

		const vector<PDAEdge*>& GeneralLALRParser::FindTheNodePathEdges(PDANode* start, PDANode* end)
		{
			return FindTheNodePath(start, end).first;
		}

		const vector<PDANode*>& GeneralLALRParser::FindTheNodePathNodes(PDANode* start, PDANode* end)
		{
			return FindTheNodePath(start, end).second;
		}

	}
}