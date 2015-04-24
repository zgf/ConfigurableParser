#pragma once
#include "stdafx.h"
namespace ztl
{
	namespace general_parser
	{
		class PDANode;
		class PDAEdge;
		class ActionWrap;
		class PushDownAutoMachine;
		class SymbolManager;
		struct GeneralTableDefine;
		class GeneralLRMachine;
		class ParserSymbol;

		class ProductPosition
		{
			PDANode* position;
			int ruleIndex;
			unique_ptr<vector<ActionWrap>> endAction = nullptr;
			unique_ptr<unordered_set<ParserSymbol*>> followToken = nullptr;
		public:
			ProductPosition()noexcept = default;
			ProductPosition(int _ruleIndex, PDANode* _position, GeneralLRMachine& machine,ParserSymbol* initSymbol) noexcept:ruleIndex(_ruleIndex), position(_position), followToken(make_unique<unordered_set<ParserSymbol*>>())
			{
				followToken->emplace(initSymbol);
				AddEndAction(machine);
			}
			ProductPosition(int _ruleIndex, PDANode* _position,GeneralLRMachine& machine,const unordered_set<ParserSymbol*>& initSymbolList) noexcept : ruleIndex(_ruleIndex), position(_position), followToken(make_unique<unordered_set<ParserSymbol*>>())
			{
				followToken->insert(initSymbolList.begin(), initSymbolList.end());
				AddEndAction(machine);
			}
			ProductPosition(ProductPosition&&) noexcept = default;
			ProductPosition(const ProductPosition&) noexcept = default;
			ProductPosition& operator=(ProductPosition&&) noexcept = default;
			ProductPosition& operator=(const ProductPosition&) noexcept = default;
			~ProductPosition() = default;
			PDANode* GetPosition()const;
			int GetRuleIndex()const;
			bool HasEndAction()const;
			const vector<ActionWrap>& GetEndAction()const;
			void AddFollowToken(ParserSymbol* symbol);
			void AddFollowToken(const vector<ParserSymbol*>&symbolList);
			const unordered_set<ParserSymbol*>& GetFollowTokens()const;
			wstring LogProduct(const GeneralLRMachine& LRMachine)const;
			wstring LogFollowToken()const;
			wstring LogEndAction()const;
		private:
			void AddEndAction(GeneralLRMachine& LRMachine);
		};

		class LRNode
		{
			int number;
			vector<ProductPosition> items;
			unordered_map<ParserSymbol*, LRNode*> nexts;
			int coreItemEnd;
			unordered_map<PDANode*, int> itemsMap;
			unordered_map<ParserSymbol*, vector<PDAEdge*>> gotoMap;
		public:
			LRNode() = delete;
			LRNode(int _number);
		public:
			const vector<PDAEdge*>* GetGotoEdges(ParserSymbol*symbol)const;
			LRNode* GetNextLRNode(ParserSymbol*symbol)const;
			void AddGotoMap(unordered_map<ParserSymbol*, vector<PDAEdge*>>&& target);
			void AddCoreItem(ProductPosition&& item);
			void AddItem(ProductPosition&& item);
			void AddCoreEnd(GeneralLRMachine & LRMachine);
			const ProductPosition& GetProductByPDANode(PDANode*)const;
			wstring LogProductList(const GeneralLRMachine & LRMachine)const;
			wstring LogNextsList()const;
			int GetNumber()const;
			int GetCoreNumber()const;
			bool HasSameCoreItems(const vector<PDANode*>& expect)const;
			void AddNexts(ParserSymbol* symbol, LRNode* next);
			const vector<ProductPosition>& GetItems()const;
			const unordered_map<ParserSymbol*, LRNode*>& GetNexts()const;
			const unordered_map<ParserSymbol*, vector<PDAEdge*>>& GetGotoMap()const;
			const unordered_map<PDANode*, int>& GetItemsMap()const;
			const ProductPosition& GetFirstProductPosition()const;
			int GetItemIndex(PDANode* node)const;

			wstring LogLRNode(const GeneralLRMachine& LRMachine)const;
		private:
			void ComputeAllItems(GeneralLRMachine & LRMachine);
			void BuildCoreItemsMap();
			void BuildItemsMap();

		};
		struct coreItemHash
		{
			size_t operator()(const vector<PDANode*> & _Keyval) const;
		};
		class GeneralLRMachine
		{
		public:
			GeneralLRMachine() = delete;
			GeneralLRMachine(const shared_ptr<PushDownAutoMachine>& _machine);
			~GeneralLRMachine() noexcept = default;
			GeneralLRMachine(GeneralLRMachine&&) = default;
			GeneralLRMachine(const GeneralLRMachine&) = default;
			GeneralLRMachine& operator=(GeneralLRMachine&&) = default;
			GeneralLRMachine& operator=(const GeneralLRMachine&) = default;
		public:
			SymbolManager*				GetSymbolManager()const;
			GeneralTableDefine*			GetTable()const;
			const unordered_map<wstring, pair<PDANode*, PDANode*>>& GetPDAMap()const;
			PushDownAutoMachine&		GetMachine()const;
			PDAEdge* IsCreatePosition(PDANode*position)const;
			LRNode*  GetLRMachineStartNode()const;
		public:
			void BuildLRItems();
			LRNode* GetInitLRNode();
			LRNode* NewEmptyLRNode();
			unique_ptr<vector<ActionWrap>> GetEndAction(PDAEdge* edge)const;
			int GetRuleIndex(PDANode* node)const;
			int GetRuleIndexByName(const wstring& name)const;
			const wstring& GetRuleNameByIndex(int index)const;
			PDANode* GetRuleStart(const wstring& name)const;
			void BuildFirstTable();
			const unordered_set<ParserSymbol*>& GetFirstSetByNode(PDANode* node)const;
			bool CheckShiftAndReduceConfilct()const;
			void LogItems(const wstring& fileName)const;
			wstring LogPDANode(const wstring & ruleName, PDANode * node) const;
		private:
			unordered_map<ParserSymbol*, vector<PDAEdge*>> GetGotoInfo(LRNode* node)const;

			PDANode* GetRootRuleStart();
			LRNode* HasTheSameCoreItem(const vector<PDANode*>& expect)const;
			LRNode* AddLRItem(const vector<PDANode*>& coreItem, const vector<const unordered_set<ParserSymbol*>*>& followSetList);
			unordered_set<ParserSymbol*>& FindFirst(PDAEdge*edge, unordered_set<PDAEdge*>& sign);
			unordered_set<ParserSymbol*>& FindFirst(PDANode*node, unordered_set<PDAEdge*>& sign);
			const vector<PDAEdge*>& GetEdgesByNode(PDANode* node)const;

		private:
			shared_ptr<PushDownAutoMachine> machine;
			vector<shared_ptr<LRNode>> nodePool;
			unordered_map<vector<PDANode*>, int, coreItemHash> coreItemMap;
			unordered_map<PDAEdge*, unordered_set<ParserSymbol*>> firstTable;
			unordered_map<PDANode*, unordered_set<ParserSymbol*>> firstNodeTable;
		};
	}
}