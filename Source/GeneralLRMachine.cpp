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
		void AddGoToMapElement(unordered_map<ParserSymbol*, vector<PDAEdge*>>& gotoMap, ParserSymbol*first, PDAEdge*second)
		{
			if(gotoMap.find(first) == gotoMap.end())
			{
				gotoMap[first];
			}
			gotoMap[first].emplace_back(second);
		}

		unordered_map<ParserSymbol*, vector<PDAEdge*>> GeneralLRMachine::GetGotoInfo(LRNode* node) const
		{
			unordered_map<ParserSymbol*, vector<PDAEdge*>> result;
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
		void GeneralLRMachine::BuildLRItems()
		{
			deque<LRNode*> queue;
			auto init = GetInitLRNode();
			queue.emplace_back(init);

			while(!queue.empty())
			{
				auto current = queue.front();
				queue.pop_front();
				auto&& gotoMap = GetGotoInfo(current);
				for(auto&&iter : gotoMap)
				{
					auto symbol = iter.first;
					vector<PDANode*> coreItems;
					vector<const unordered_set<ParserSymbol*>*> followSetList;
					transform(iter.second.begin(), iter.second.end(), back_inserter(coreItems), [&current, &followSetList](PDAEdge* edge)
					{
						auto node = edge->GetSource();
						auto&& product = current->GetProductByPDANode(node);
						followSetList.push_back(&product.GetFollowTokens());
						return edge->GetTarget();
					});
					sort(coreItems.begin(), coreItems.end());
					auto result = HasTheSameCoreItem(coreItems);
					if(result == nullptr)
					{
						result = AddLRItem(coreItems, followSetList);
						queue.emplace_back(result);
					}
					current->AddNexts(symbol, result);
				}
				current->AddGotoMap(move(gotoMap));
			}
		}

		LRNode * GeneralLRMachine::GetInitLRNode()
		{
			assert(machine != nullptr);
			auto init = NewEmptyLRNode();
			ProductPosition position(GetRuleIndex(GetRootRuleStart()), GetRootRuleStart(), *this, GetSymbolManager()->GetTokenTypeSymbol());
			init->AddCoreItem(move(position));
			init->AddCoreEnd(*this);
			return init;
		}

		LRNode * GeneralLRMachine::NewEmptyLRNode()
		{
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

		LRNode* GeneralLRMachine::AddLRItem(const vector<PDANode*>& coreItem, const vector<const unordered_set<ParserSymbol*>*>& followSetList)
		{
			auto init = NewEmptyLRNode();
			assert(coreItem.size() == followSetList.size());
			for(size_t i = 0; i < coreItem.size(); i++)
			{
				auto&& iter = coreItem[i];
				auto&& followSet = *followSetList[i];
				ProductPosition position(GetRuleIndex(iter), iter, *this, followSet);
				init->AddCoreItem(move(position));
			}
			vector<PDANode*> hashKey;
			std::transform(init->GetItems().begin(), init->GetItems().end(), back_inserter(hashKey), [](const ProductPosition& value)
			{
				return value.GetPosition();
			});
			coreItemMap.insert({ hashKey,init->GetNumber() });
			init->AddCoreEnd(*this);
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
		}
		unordered_set<ParserSymbol*>& GeneralLRMachine::FindFirst(PDAEdge*edge, unordered_set<PDAEdge*>& sign)
		{
			auto findIter = firstTable.find(edge);
			if(findIter == firstTable.end())
			{
				//没有构建过
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
		const unordered_set<ParserSymbol*>& GeneralLRMachine::GetFirstSetByNode(PDANode * node) const
		{
			auto findIter = firstNodeTable.find(node);
			assert(findIter != firstNodeTable.end());
			return findIter->second;
		}
		bool GeneralLRMachine::CheckShiftAndReduceConfilct() const
		{
			for(auto&&iter : nodePool)
			{
				for(auto&& productIter : iter->GetItems())
				{
					if(productIter.HasEndAction())
					{
						//end节点文法末尾节点
						//规约元素集
						auto followSet = productIter.GetFollowTokens();
						for(auto&&followIter : productIter.GetFollowTokens())
						{
							auto findIter = iter->GetGotoMap().find(followIter);
							if(findIter != iter->GetGotoMap().end())
							{
								//next集合出现了这个元素
								for(auto&& edgeIter : findIter->second)
								{
									//查看是不是当前所在文法出现的.
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
			// TODO: insert return statement here
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

		void LRNode::AddCoreEnd(GeneralLRMachine & LRMachine)
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
			assert(is_sorted(expect.begin(), expect.end()));
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
		void LRNode::ComputeAllItems(GeneralLRMachine & LRMachine)
		{
			assert(!items.empty());
			assert(is_sorted(items.begin(), items.end(), [](const ProductPosition&left, const ProductPosition& right)
			{
				return left.GetPosition() < right.GetPosition();
			}));

			assert(this->coreItemEnd != 0);
			unordered_set<wstring> signMap;

			for(size_t i = 0; i < items.size(); i++)
			{
				auto& current = items[i];
				for(auto&&edgeIter : current.GetPosition()->GetNexts())
				{
					if(IsNonTerminateSymbol(edgeIter))
					{
						assert(!edgeIter->GetActions().empty() && edgeIter->GetActions()[0].GetActionType() == ActionType::NonTerminate);
						auto&& ruleName = edgeIter->GetActions()[0].GetName();
						if(signMap.find(ruleName) == signMap.end())
						{
							signMap.insert(ruleName);
							auto ruleIndex = LRMachine.GetRuleIndexByName(ruleName);
							auto start = LRMachine.GetRuleStart(ruleName);
							auto&& followSet = LRMachine.GetFirstSetByNode(start);
							if(followSet.size() == 0)
							{
								ProductPosition product(ruleIndex, start, LRMachine, items[this->itemsMap[start]].GetFollowTokens());
								AddItem(move(product));
							}
							else
							{
								ProductPosition product(ruleIndex, start, LRMachine, followSet);
								AddItem(move(product));
							}
						}
					}
				}
			}
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
			for(size_t i = this->coreItemEnd; i < items.size(); i++)
			{
				itemsMap.insert({ items[i].GetPosition(),i });
			}
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
		PDANode * ProductPosition::GetPosition() const
		{
			return position;
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
		void ztl::general_parser::ProductPosition::AddFollowToken(const vector<ParserSymbol*>& symbolList)
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