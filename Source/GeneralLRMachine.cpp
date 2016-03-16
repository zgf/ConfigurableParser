#include "Include/stdafx.h"
#include "Include\GeneralLRMachine.h"
#include "Include\SymbolManager.h"
#include "Include\GeneralPushDownAutoMachine.h"
#include "Include\GeneralPushDownMachineData.h"
#include "Include\ParserSymbol.h"
namespace ztl
{
	namespace general_parser
	{
		GeneralLRMachine::GeneralLRMachine(const shared_ptr<PushDownAutoMachine>& _machine) :machine(_machine)
		{
		}
		ParserSymbol*  GeneralLRMachine::GetProductHeadRuleSymbolByPosition(const ProductPosition * product) const
		{
			auto ruleIndex = product->GetRuleIndex();
			auto&& ruleName = GetRuleNameByIndex(ruleIndex);
			auto ruleSymbol = GetSymbolManager()->GetRuleSymbolByName(ruleName);
			return ruleSymbol;
		}
		PDANode * GeneralLRMachine::GetStartNodeByProduction(const ProductPosition * product) const
		{
			auto ruleIndex = product->GetRuleIndex();
			auto&& ruleName = GetRuleNameByIndex(ruleIndex);
			auto startNode = GetRuleStart(ruleName);
			return startNode;
		}

		const unordered_map<ParserSymbol*, unordered_set<ParserSymbol*>>& GeneralLRMachine::GetFirstSet() const
		{
			return firstSet;
		}
	
		SymbolManager*				GeneralLRMachine::GetSymbolManager()const
		{
			return machine->GetSymbolManager();
		}
		GeneralTableDefine*			GeneralLRMachine::GetTable()const
		{
			return machine->GetTable();
		}
		const unordered_map<wstring, pair<PDANode*, PDANode*>>& GeneralLRMachine::GetPDAMap()const
		{
			return machine->GetPDAMap();
		}

		PushDownAutoMachine & GeneralLRMachine::GetMachine() const
		{
			return *machine;
			// TODO: insert return statement here
		}

		PDAEdge* GeneralLRMachine::IsCreatePosition(PDANode*position) const
		{
			for(auto&&edgeIter : position->GetNexts())
			{
				assert(!edgeIter->GetActions().empty());
				if(edgeIter->GetActions()[0].GetActionType() == ActionType::Create)
				{
					return edgeIter;
				}
			}
			return nullptr;
		}
		LRNode * GeneralLRMachine::GetLRMachineStartNode() const
		{
			assert(!nodePool.empty());
			return nodePool[0].get();
		}
		void AddGoToMapElement(std::unordered_map<ParserSymbol*, vector<PDAEdge*>>& gotoMap, ParserSymbol*first, PDAEdge*second)
		{
			if(gotoMap.find(first) == gotoMap.end())
			{
				gotoMap[first];
			}
			gotoMap[first].emplace_back(second);
		}

		std::unordered_map<ParserSymbol*, vector<PDAEdge*>> GeneralLRMachine::GetGotoInfo(LRNode* node) const
		{
			std::unordered_map<ParserSymbol*, vector<PDAEdge*>> result;
			for(auto&&iter : node->GetItems())
			{
				auto&& currentNode = iter.GetPosition();
				for(auto&& edgeIter : currentNode->GetNexts())
				{
					assert(!edgeIter->GetActions().empty());
					auto&& firstActionWrap = *edgeIter->GetActions().begin();
					if(firstActionWrap.GetActionType() != ActionType::Create)
					{
						assert(firstActionWrap.GetActionType() == ActionType::NonTerminate || firstActionWrap.GetActionType() == ActionType::Terminate);
						auto symbol = firstActionWrap.GetParserSymbol();
						AddGoToMapElement(result, symbol, edgeIter);
					}
				}
			}
			return result;
		}
		int GeneralLRMachine::GetRuleIndex(PDANode * node) const
		{
			return machine->GetRuleIndexByNode(node);
		}
		int GeneralLRMachine::GetRuleIndexByName(const wstring& name)const
		{
			return GetSymbolManager()->GetRuleIndexByName(name);
		}
		const wstring& GeneralLRMachine::GetRuleNameByIndex(int index) const
		{
			return GetSymbolManager()->GetRuleNameByIndex(index);
		}
		vector<ProductPosition*> LRNode::GetProductPositionChildren(ProductPosition* parent)
		{
			vector<ProductPosition*> result;
			deque<ProductPosition*> queue;
			queue.emplace_back(parent);
			while (queue.empty())
			{
				auto element = queue.back();
				queue.pop_back();
				auto&&findIter = generateItemMap.find(element->GetPosition());
				if (findIter != generateItemMap.end())
				{
					for (auto&& childIter : findIter->second)
					{
						auto&& product = this->GetMutableProductByPDANode(childIter);
						result.push_back(&product);
						queue.emplace_back(&product);
					}
				}
			}
			return result;
		}
		void GeneralLRMachine::PropagateFollowInfoToNextPosition(LRNode* node,ProductPosition* product,deque<pair<LRNode*,ProductPosition*>>& queue)
		{
			auto&& nextEdges = product->GetPosition()->GetNexts();
			for (auto&&edge : nextEdges)
			{
				auto&& action = edge->GetActions()[0];
				assert(action.GetActionType() == ActionType::NonTerminate || action.GetActionType() == ActionType::Terminate || action.GetActionType() == ActionType::Create);
				if (action.GetActionType() == ActionType::NonTerminate || action.GetActionType() == ActionType::Terminate)
				{
				
					auto nextLRNode = node->GetNextLRNode(action.GetParserSymbol());
					assert(nextLRNode != nullptr);
					auto&& nextProduct = nextLRNode->GetMutableProductByPDANode(edge->GetTarget());
					
					if (nextProduct.GetFollowTokens() != product->GetFollowTokens())
					{
						nextProduct.AddFollowToken(product->GetFollowTokens());
						//Ҫ����nextProduct���ɵķǺ����������
						nextLRNode->UpdateGenerateItemFollowSet(nextProduct.GetPosition());
						queue.push_back(make_pair(nextLRNode, &nextProduct));
					}
				}
			}
		}
		void GeneralLRMachine::PropagateFollowInfoToNextPosition( deque<pair<LRNode*, ProductPosition*>>& products)
		{
			while (!products.empty())
			{
				
				auto productPair = products.back();
				auto node = productPair.first;
				auto product = productPair.second;
				if (node->GetNumber() == 66 && node->GetItemIndex(product->GetPosition()) == 0)
				{
					int a = 0;
				}
				products.pop_back();
				PropagateFollowInfoToNextPosition(node,product, products);
			}
		}
		deque<pair<LRNode*, ProductPosition*>> CreatePropagateDeque(const vector<ProductPosition*> init,LRNode* node)
		{
			deque<pair<LRNode*, ProductPosition*>> result;
			for (auto&&iter : init)
			{
				result.push_back(make_pair(node, iter));
			}
			return result;
		}
		void LRNode::AddFollowInfoToExsitLRNode(const vector<PDANode*>& coreItems, unordered_map<PDANode*, const unordered_set<ParserSymbol*>*>&coreItemToFollowSetMap)
		{
			for (size_t i = 0; i < this->GetCoreNumber(); i++)
			{
				auto&&product = this->GetMutableProductByIndex(i);
				auto follow = *coreItemToFollowSetMap.find(coreItems[i])->second;
				product.AddFollowToken(follow);
				//��һ���Ѿ����ڵ����ĳһ������ʽ���������µ�followset.
				//0.����Ҫ��followset����������������
				//1.����Ҫ���µ�followset�����������Ե������������ʽ�ڵ���.
				this->UpdateGenerateItemFollowSet(product.GetPosition());
				auto result = GetProductPositionChildren(&product);
				result.emplace_back(&product);
				auto queue = CreatePropagateDeque(result,this);
				//this->PropagateFollowInfoToNextPosition(queue);
			}
		}

		vector<ParserSymbol*> GetGotoTravelOrder(const std::unordered_map<ParserSymbol*, vector<PDAEdge*>>& gotoMap)
		{
			vector<ParserSymbol*> orderList;
			for (auto&&iter : gotoMap)
			{
				orderList.emplace_back(iter.first);
			}
			sort(orderList.begin(), orderList.end(), [](ParserSymbol*left, ParserSymbol*right) {return left->GetNumber() < right->GetNumber();});
			return orderList;
		}
		void GeneralLRMachine::BuildLRItems()
		{
			deque<LRNode*> queue;
			auto init = this->GetInitLRNode();
			queue.emplace_back(init);
			
			while(!queue.empty())
			{
				
				auto current = queue.front();
				if (current->GetNumber() == 38)
				{
					int a = 0;
				}
				queue.pop_front();
				auto&& gotoMap = GetGotoInfo(current);
				auto&& orderList = GetGotoTravelOrder(gotoMap);
			
				for(auto&& orderIter: orderList)
				{
					auto&&gotoEdges = gotoMap[orderIter];
					auto symbol = orderIter;
					vector<PDANode*> coreItems;
					//unordered_map<PDANode*, const unordered_set<ParserSymbol*>*>coreItemToFollowSetMap;
					auto&&edges = gotoEdges;
					for (auto&&edge : edges)
					{
						auto node = edge->GetSource();
						//auto&& product = current->GetProductByPDANode(node);
						coreItems.emplace_back(edge->GetTarget());
						/*coreItemToFollowSetMap.insert(make_pair(edge->GetTarget(), &product.GetFollowTokens()));
						coreItemToFollowSetMap.insert(make_pair(edge->GetTarget(), &product.GetFollowTokens()));*/
					}
					sort(coreItems.begin(), coreItems.end(), [](PDANode*left, PDANode*right) {return left->GetNumber() < right->GetNumber();});
					auto result = HasTheSameCoreItem(coreItems);
					if(result == nullptr)
					{
						result = AddLRItem(coreItems);
						queue.emplace_back(result);
						
					}
					/*else//follow�Ĵ�����һ�����������,���ﲻӦ��������,��Ҫר�ŵĴ����׶�ȥ����,������ΪLR��û������,��������
					{
						if (result->GetNumber() == 19)
						{
							int a = 0;
						}
						result->AddFollowInfoToExsitLRNode(coreItems, coreItemToFollowSetMap);
					}*/
					current->AddNexts(symbol, result);
				}
				current->AddGotoMap(move(gotoMap));
			}
		}
		LRNode * GeneralLRMachine::GetInitLRNode()
		{
			assert(machine != nullptr);
			auto init = NewEmptyLRNode();
			ProductPosition position(GetRuleIndex(GetRootRuleStart()), GetRootRuleStart(), *this, GetSymbolManager()->GetFinishTokenSymbol());
			init->AddCoreItem(move(position));
			init->AddCoreEndAndComputeAllItems(*this);
			return init;
		}

		LRNode * GeneralLRMachine::NewEmptyLRNode()
		{
			if (nodePool.size() == 19)
			{
				int a = 0;
			}
			nodePool.emplace_back(make_shared<LRNode>((int) nodePool.size()));
			return 	nodePool.back().get();
		}

		PDANode * GeneralLRMachine::GetRootRuleStart()
		{
			return GetRuleStart(machine->GetRootRuleName());
		}

		PDANode* GeneralLRMachine::GetRuleStart(const wstring& name) const
		{
			auto findIter = GetPDAMap().find(name);
			assert(findIter != GetPDAMap().end());
			return findIter->second.first;
		}

		LRNode* GeneralLRMachine::HasTheSameCoreItem(const vector<PDANode*>& expect) const
		{
			auto findIter = coreItemMap.find(expect);
			return  (findIter != coreItemMap.end()) ? nodePool[findIter->second].get() : nullptr;
		}
		void GeneralLRMachine::AddCoreItemMap(LRNode* init)
		{
			vector<PDANode*> hashKey;
			std::transform(init->GetItems().begin(), init->GetItems().end(), back_inserter(hashKey), [](const ProductPosition& value)
			{
				return value.GetPosition();
			});
			sort(hashKey.begin(), hashKey.end(), [](PDANode*left, PDANode*right) {return left->GetNumber() < right->GetNumber();});
			coreItemMap.insert({ hashKey,init->GetNumber() });
		}
		LRNode* GeneralLRMachine::AddLRItem(const vector<PDANode*>& coreItem, const unordered_map<PDANode*, const unordered_set<ParserSymbol*>*>&coreItemToFollowSetMap)
		{
			auto init = NewEmptyLRNode();
			assert(coreItem.size() == coreItemToFollowSetMap.size());
			for(size_t i = 0; i < coreItem.size(); i++)
			{
				auto&& iter = coreItem[i];
				auto followSet = *coreItemToFollowSetMap.find(iter)->second;
				ProductPosition position(GetRuleIndex(iter), iter, *this, followSet);
				init->AddCoreItem(move(position));
			}
			AddCoreItemMap(init);
			if (init->GetNumber() == 37)
			{
				int a = 0;
			}
			init->AddCoreEndAndComputeAllItems(*this);
			return init;
		}

		LRNode * GeneralLRMachine::AddLRItem(const vector<PDANode*>& coreItem)
		{
			auto init = NewEmptyLRNode();
			for (size_t i = 0; i < coreItem.size(); i++)
			{
				auto&& iter = coreItem[i];
				ProductPosition position(GetRuleIndex(iter), iter, *this);
				init->AddCoreItem(move(position));
			}
			AddCoreItemMap(init);
			init->AddCoreEndAndComputeAllItems(*this);
			return init;
		}

		void GeneralLRMachine::BuildFirstTable()
		{
			deque<PDANode*> queue;
			unordered_set<PDAEdge*>edgeSign;
			unordered_set<PDANode*> sign;
			for(auto&&iter : GetPDAMap())
			{
				auto start = iter.second.first;
				queue.emplace_back(start);
				sign.insert(start);
				while(!queue.empty())
				{
					auto front = queue.front();
					queue.pop_front();
					FindFirst(front, edgeSign);
					for(auto&&edgeIter : front->GetNexts())
					{
						if(sign.find(edgeIter->GetTarget()) == sign.end())
						{
							sign.insert(edgeIter->GetTarget());
							queue.emplace_back(edgeIter->GetTarget());
						}
					}
					edgeSign.clear();
				}
			}
			return;
		}
		unordered_set<ParserSymbol*>& GeneralLRMachine::FindFirst(PDAEdge*edge, unordered_set<PDAEdge*>& sign)
		{
			auto findIter = firstTable.find(edge);
			if(findIter == firstTable.end())
			{
				//û�й�����
				assert(firstTable.find(edge) == firstTable.end());
				firstTable[edge];
				if(edge->GetActions()[0].GetActionType() == ActionType::Terminate)
				{
					firstTable[edge].emplace(edge->GetActions()[0].GetParserSymbol());
				}
				else if(edge->GetActions()[0].GetActionType() == ActionType::NonTerminate)
				{
					auto start = GetRuleStart(edge->GetActions()[0].GetName());
					auto&& findList = FindFirst(start, sign);
					firstNodeTable[start].insert(findList.begin(), findList.end());
					firstTable[edge].insert(findList.begin(), findList.end());
					if(firstTable[edge].empty())
					{
						auto target = edge->GetTarget();
						while(!target->GetNexts().empty() && firstTable[edge].empty())
						{
							auto&& targetList = FindFirst(target, sign);
							firstTable[edge].insert(targetList.begin(), targetList.end());
						}
					}
				}
				else
				{
					assert(edge->GetActions()[0].GetActionType() == ActionType::Create);
				}
			}
			return firstTable[edge];
		}

		unordered_set<ParserSymbol*>& GeneralLRMachine::FindFirst(PDANode * node, unordered_set<PDAEdge*>& sign)
		{
			if(firstNodeTable.find(node) == firstNodeTable.end())
			{
				for(auto&&iter : node->GetNexts())
				{
					if(iter->GetActions()[0].GetActionType() == ActionType::Terminate)
					{
						auto&& iterList = FindFirst(iter, sign);
						firstNodeTable[node].insert(iterList.begin(), iterList.end());
					}
					else if(iter->GetActions()[0].GetActionType() == ActionType::NonTerminate)
					{
						if(sign.find(iter) == sign.end())
						{
							sign.insert(iter);
							auto&& iterList = FindFirst(iter, sign);
							firstNodeTable[node].insert(iterList.begin(), iterList.end());
						}
					}
				}
			}

			return firstNodeTable[node];
		}
		const vector<PDAEdge*>& GeneralLRMachine::GetEdgesByNode(PDANode* node)const
		{
			return machine->GetEdgesByNode(node);
		}
		void CollectionNodeEdgeSymbol(PDANode* node, unordered_set<ParserSymbol*>& termSet, unordered_set<ParserSymbol*>& nonTermSet)
		{
			
			auto&&nexts = node->GetNexts();
			for (auto&&iter : nexts)
			{
				auto&& actions = iter->GetActions();
				assert(!actions.empty());
				auto&& action = actions[0];
				auto symbol = action.GetParserSymbol();
				assert(action.GetActionType() == ActionType::NonTerminate || action.GetActionType() == ActionType::Terminate|| action.GetActionType() == ActionType::Create);
				if (action.GetActionType() == ActionType::NonTerminate)
				{
					nonTermSet.emplace(symbol);
				}
				else if(action.GetActionType() == ActionType::Terminate)
				{
					termSet.emplace(symbol);
				}
			}

		}

		void GeneralLRMachine::CalculateFirstSet(ParserSymbol* ruleSymbol)
		{
			unordered_set<ParserSymbol*> result;
			auto&& ruleName = ruleSymbol->GetName();
			auto&& startNode = GetRuleStart(ruleName);
			unordered_set<ParserSymbol*> termSet;
			unordered_set<ParserSymbol*> nonTermSet;
			CollectionNodeEdgeSymbol(startNode, termSet, nonTermSet);
			if (nonTermSet.find(ruleSymbol) != nonTermSet.end())
			{
				nonTermSet.erase(ruleSymbol);
			}
			for (auto&&iter : nonTermSet)
			{
				if (firstSet.find(iter) == firstSet.end())
				{
					CalculateFirstSet(iter);
					assert(!firstSet[iter].empty());
					
				}
				result.insert(firstSet[iter].begin(), firstSet[iter].end());
			}
			result.insert(termSet.begin(), termSet.end());
			assert(firstSet.find(ruleSymbol) == firstSet.end());
			firstSet[ruleSymbol] = move(result);

		}
		void GeneralLRMachine::CalculateFirstSet()
		{
			for (auto&&iter : GetPDAMap())
			{
				auto ruleName = iter.first;
				
				auto ruleSymbol = GetSymbolManager()->GetRuleSymbolByName(ruleName);
				assert(ruleSymbol != nullptr);
				if (firstSet.find(ruleSymbol) == firstSet.end())
				{
					this->CalculateFirstSet(ruleSymbol);
				}
			}
		}
		void GeneralLRMachine::PropagateLRItemFollowSymbol()
		{
			for (auto&&iter : this->nodePool)
			{
				auto&&node = iter.get();
				deque<pair<LRNode*, ProductPosition*>>queue;
				for (size_t i = 0; i < node->GetItems().size(); i++)
				{
					auto&&iter = node->GetMutableProductByIndex(i);
					queue.push_back(make_pair(node, &iter));
				}
				PropagateFollowInfoToNextPosition(queue);
			}
			/*auto start = GetLRMachineStartNode();
			 deque<pair<LRNode*, ProductPosition*>>queue;
			 for (size_t i = 0; i < start->GetItems().size(); i++)
			 {
				 auto&&iter = start->GetMutableProductByIndex(i);
				 queue.push_back(make_pair(start, &iter));
			 }
			 PropagateFollowInfoToNextPosition(queue);*/
		}
		const unordered_set<ParserSymbol*>& GeneralLRMachine::GetFirstSetByNode(PDANode * node) 
		{
			/*auto findIter = firstNodeTable.find(node);
			assert(findIter != firstNodeTable.end());
			return findIter->second;*/
			auto findIter = firstNodeSet.find(node);
			if (findIter == firstNodeSet.end())
			{
				firstNodeSet[node];
				unordered_set<ParserSymbol*> termSet;
				unordered_set<ParserSymbol*> nonTermSet;
				CollectionNodeEdgeSymbol(node, termSet, nonTermSet);
				for (auto&&iter : nonTermSet)
				{
					auto&& temp = firstSet[iter];
					firstNodeSet[node].insert(temp.begin(), temp.end());
				}
				firstNodeSet[node].insert(termSet.begin(), termSet.end());

			}
			assert(firstNodeTable[node] == firstNodeSet[node]);
			return firstNodeSet[node];
		}
		bool GeneralLRMachine::CheckShiftAndReduceConfilct() const
		{
			for(auto&&iter : nodePool)
			{
				for(auto&& productIter : iter->GetItems())
				{
					if(productIter.HasEndAction())
					{
						//end�ڵ��ķ�ĩβ�ڵ�
						//��ԼԪ�ؼ�
						auto followSet = productIter.GetFollowTokens();
						for(auto&&followIter : productIter.GetFollowTokens())
						{
							auto findIter = iter->GetGotoMap().find(followIter);
							if(findIter != iter->GetGotoMap().end())
							{
								//next���ϳ��������Ԫ��
								for(auto&& edgeIter : findIter->second)
								{
									//�鿴�ǲ��ǵ�ǰ�����ķ����ֵ�.
									if(edgeIter->GetSource() == productIter.GetPosition())
									{
										return true;
									}
								}
							}
						}
					}
				}
			}
			return false;
		}
		void ztl::general_parser::GeneralLRMachine::LALRConfilctDetection()
		{
			
			for (auto&&node : this->nodePool)
			{
				unordered_set<ParserSymbol*> endSet;
				unordered_set<ParserSymbol*>moveSet;
				unordered_set<ParserSymbol*>testSet;
				for (auto&&item : node->GetItems())
				{
					
					if (item.HasEndAction())
					{
						auto set = item.GetFollowTokens();
						endSet.insert(set.begin(), set.end());
					}
				}
				
				for (auto&&mapIter : node->GetGotoMap())
				{
					auto symbol = mapIter.first;
					moveSet.insert(symbol);
				}
				for (auto&&mapIter : node->GetNexts())
				{
					auto symbol = mapIter.first;
					testSet.insert(symbol);
				}
				assert(moveSet == testSet);
				for (auto&&iter : endSet)
				{
					assert(moveSet.find(iter) == moveSet.end());
				}
			}
		}
		unique_ptr<vector<ActionWrap>> GeneralLRMachine::GetEndAction(PDAEdge* edge) const
		{
			auto result = make_unique<vector<ActionWrap>>();
			auto& actions = edge->GetActions();
			auto current = edge->GetTarget();
			result->insert(result->end(), actions.begin(), actions.end());
			auto& currentNext = current->GetNexts();
			while(!currentNext.empty())
			{
				assert(currentNext.empty());
				auto&& next = currentNext[0];
				auto& nextActions = next->GetActions();
				result->insert(result->end(), nextActions.begin(), nextActions.end());
				current = next->GetTarget();
			}
			return result;
		}

		int LRNode::GetItemIndex(PDANode * node) const
		{
			auto findIter = itemsMap.find(node);
			assert(findIter != itemsMap.end());
			return findIter->second;
		}
		int LRNode::GetCoreNumber()const
		{
			return coreItemEnd;
		}

		const ProductPosition & LRNode::GetProductByPDANode(PDANode *node) const
		{
			auto index = GetItemIndex(node);
			assert(index >= 0 && index < (int) items.size());
			return items[index];
		}
		ProductPosition & LRNode::GetMutableProductByPDANode(PDANode * node)
		{
			auto index = GetItemIndex(node);
			assert(index >= 0 && index < (int)items.size());
			return items[index];
		}
		ProductPosition & LRNode::GetMutableProductByIndex(size_t index)
		{
			assert( index < items.size());
			return items[index];
		}
		int LRNode::GetNumber()const
		{
			return number;
		}

		void LRNode::AddCoreItem(ProductPosition&& item)
		{
			this->items.emplace_back(std::forward<ProductPosition&&>(item));
			this->coreItemEnd++;
		}

		void LRNode::AddItem(ProductPosition && item)
		{
			this->items.emplace_back(std::forward<ProductPosition&&>(item));
		}

		void LRNode::AddCoreEndAndComputeAllItems(GeneralLRMachine & LRMachine)
		{
			sort(items.begin(), items.end(), [](const ProductPosition&left, const ProductPosition& right)
			{
				return left.GetPosition() < right.GetPosition();
			});
			BuildCoreItemsMap();
			ComputeAllItems(LRMachine);
			BuildItemsMap();
		}

		bool LRNode::HasSameCoreItems(const vector<PDANode*>& expect) const
		{
			assert(is_sorted(expect.begin(), expect.end(), [](PDANode*left, PDANode*right) {return left->GetNumber() < right->GetNumber();}));
			assert(is_sorted(this->items.begin(), items.begin() + this->coreItemEnd, [](const ProductPosition&left, const ProductPosition& right)
			{
				return left.GetPosition() < right.GetPosition();
			}));
			return equal(this->items.begin(), items.begin() + this->coreItemEnd, expect.begin(), expect.end(), [](const ProductPosition& left, PDANode* right)
			{
				return left.GetPosition() == right;
			});
		}
		bool IsNonTerminateSymbol(PDAEdge* edge)
		{
			assert(!edge->GetActions().empty());
			return edge->GetActions()[0].GetActionType() == ActionType::NonTerminate;
		}
		void LRNode::UpdateGenerateItemFollowSet(PDANode* parentNode)
		{
			auto&& findIter = generateItemMap.find(parentNode);
			if (findIter != generateItemMap.end())
			{
				auto&& generates = findIter->second;
				for (auto&&iter : generates)
				{
					auto&& product = this->GetMutableProductByPDANode(iter);
					product.AddFollowToken(this->GetMutableProductByPDANode(parentNode).GetFollowTokens());
				}

				for (auto&&iter : generates)
				{
					UpdateGenerateItemFollowSet(iter);
				}
			}
		}

		void LRNode::UpdateGenerateItemRelation( PDANode* parent, PDANode* child)
		{
			if (generateItemMap.find(parent) == generateItemMap.end())
			{
				generateItemMap[parent];
			}
			generateItemMap[parent].insert(child);
		}

		void LRNode::ComputeAllItems(GeneralLRMachine & LRMachine)
		{
			if (number == 38)
			{
				int a = 0;
			}
			//����Ҫע�⵽һ��������Ǻ��������ɵ����followset����Ϊ�������followset���¶�����
			assert(!items.empty());
			assert(is_sorted(items.begin(), items.end(), [](const ProductPosition&left, const ProductPosition& right)
			{
				return left.GetPosition() < right.GetPosition();
			}));

			assert(this->coreItemEnd != 0);
			unordered_set<wstring> signMap;
			for(size_t i = 0; i < items.size(); i++)
			{
				
				for(auto&&edgeIter : items[i].GetPosition()->GetNexts())
				{
					if(IsNonTerminateSymbol(edgeIter))
					{
						auto&& ruleName = edgeIter->GetActions()[0].GetName();
						auto ruleIndex = LRMachine.GetRuleIndexByName(ruleName);
						auto start = LRMachine.GetRuleStart(ruleName);
						auto propagateToken = items[i].GetFollowTokens();
						auto spontaneousToken = LRMachine.GetFirstSetByNode(edgeIter->GetTarget());
						unordered_set<ParserSymbol*> followToken;
						followToken.insert(propagateToken.begin(), propagateToken.end());
						followToken.insert(spontaneousToken.begin(), spontaneousToken.end());
						if (signMap.find(ruleName) == signMap.end()&&itemsMap.find(start)==itemsMap.end())
						{
							signMap.insert(ruleName);
							ProductPosition product(ruleIndex, start, LRMachine, followToken);
							itemsMap[start] = items.size();
							AddItem(move(product));
							UpdateGenerateItemRelation(items[i].GetPosition(),product.GetPosition());
						}
						else
						{
							
							auto&& product = this->GetMutableProductByPDANode(start);
							assert(product.GetPosition() == start);
							if (product.GetFollowTokens() != followToken)
							{
								//���Ѿ����ڵ������followSet;
								product.AddFollowToken(followToken);
								//����product���ɵ����followset
								UpdateGenerateItemFollowSet(product.GetPosition());
							}
							
						}
					}
				}
			}
			return;
		}
		void ztl::general_parser::LRNode::BuildCoreItemsMap()
		{
			for(size_t i = 0; i < (size_t) coreItemEnd; i++)
			{
				itemsMap.insert({ items[i].GetPosition(),i });
			}
		}
		void LRNode::BuildItemsMap()
		{
			/*for(size_t i = this->coreItemEnd; i < items.size(); i++)
			{
				itemsMap.insert({ items[i].GetPosition(),i });
			}*/
		}
		
		void LRNode::AddNexts(ParserSymbol * symbol, LRNode * next)
		{
			if(this->nexts.find(symbol) == nexts.end())
			{
				nexts.insert({ symbol,next });
			}
			else
			{
				assert(nexts[symbol] == next);
			}
		}
		const vector<ProductPosition>& LRNode::GetItems() const
		{
			return items;
		}
		const unordered_map<ParserSymbol*, LRNode*>& ztl::general_parser::LRNode::GetNexts() const
		{
			return nexts;
		}
		const unordered_map<ParserSymbol*, vector<PDAEdge*>>& ztl::general_parser::LRNode::GetGotoMap() const
		{
			return gotoMap;
		}
		const unordered_map<PDANode*, int>& ztl::general_parser::LRNode::GetItemsMap() const
		{
			return itemsMap;
		}

		const ProductPosition & ztl::general_parser::LRNode::GetFirstProductPosition() const
		{
			assert(!items.empty());
			return this->items[0];
			// TODO: insert return statement here
		}

		const vector<PDAEdge*>* ztl::general_parser::LRNode::GetGotoEdges(ParserSymbol*symbol) const
		{
			auto findIter = gotoMap.find(symbol);
			return findIter == gotoMap.end() ? nullptr : &findIter->second;
			// TODO: insert return statement here
		}

		LRNode * ztl::general_parser::LRNode::GetNextLRNode(ParserSymbol* symbol) const
		{
			auto findIter = nexts.find(symbol);
			return (findIter == nexts.end()) ? nullptr : findIter->second;
		}
	
		void ztl::general_parser::LRNode::AddGotoMap(unordered_map<ParserSymbol*, vector<PDAEdge*>>&& target)
		{
			gotoMap = move(std::forward<unordered_map<ParserSymbol*, vector<PDAEdge*>>&&>(target));
		}
		LRNode::LRNode(int _number) :number(_number), coreItemEnd(0)
		{
		}
		void ProductPosition::AddEndAction(GeneralLRMachine& LRMachine)
		{
			auto result = LRMachine.IsCreatePosition(position);

			if(result != nullptr)
			{
				auto actions = LRMachine.GetEndAction(result);
				auto findIter = find_if(actions->begin(), actions->end(), [&LRMachine](const ActionWrap& wrap)
				{
					return wrap.GetActionType() == ActionType::Terminate&&
						wrap.GetParserSymbol() == LRMachine.GetSymbolManager()->GetTokenSymbolByName(L"FINISH");
				});
				endAction = move(actions);
			}
		}
		void ProductPosition::AddIsProductEndPosition(GeneralLRMachine & LRMachine)
		{
			AddEndAction(LRMachine);
			if (HasEndAction())
			{
				isProductEndPosition = true;
			}
			else if (position->GetNexts().size() == 0)//�����ǲ�������������create��������β�� ���� "("!Grammar")"
			{
				isProductEndPosition = true;
			}
			else
			{
				isProductEndPosition = false;
			}
		}
		PDANode * ProductPosition::GetPosition() const
		{
			return position;
		}

		bool ProductPosition::IsProductEndPosition() const
		{
			return isProductEndPosition;
		}

		int ProductPosition::GetRuleIndex() const
		{
			return ruleIndex;
		}
		bool ProductPosition::HasEndAction() const
		{
			return endAction != nullptr;
		}
		const vector<ActionWrap>& ProductPosition::GetEndAction() const
		{
			assert(HasEndAction());
			return *endAction;
		}

		void ztl::general_parser::ProductPosition::AddFollowToken(ParserSymbol * symbol)
		{
			followToken->emplace(symbol);
		}
		void ztl::general_parser::ProductPosition::AddFollowToken(const unordered_set<ParserSymbol*>& symbolList)
		{
			followToken->insert(symbolList.begin(), symbolList.end());
		}
		const unordered_set<ParserSymbol*>& ztl::general_parser::ProductPosition::GetFollowTokens() const
		{
			return*followToken;
			// TODO: insert return statement here
		}

		size_t coreItemHash::operator()(const vector<PDANode*> & _Keyval) const
		{
			size_t sum = 0;
			for(auto&&iter : _Keyval)
			{
				sum += std::_Hash_seq((const unsigned char*) &iter, sizeof(PDANode*));
			}
			return sum;
		}
	}
}