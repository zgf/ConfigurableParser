#pragma once
#include "stdafx.h"
namespace ztl
{
	namespace general_parser
	{
		class PDANode;
		class PDAEdge;
		class ActionWrap;
		class GrammarBuilder;
		class SymbolManager;
		struct GeneralTableDefine;
		class GeneralLRBuilder;
		class ParserSymbol;

		class ProductPosition
		{
			PDANode* position;
			int ruleIndex;
			bool isProductEndPosition;
			unique_ptr<vector<ActionWrap>> endAction = nullptr;
			unique_ptr<unordered_set<ParserSymbol*>> followToken = nullptr;
		public:
			ProductPosition()noexcept = default;
			ProductPosition(int _ruleIndex, PDANode* _position, GeneralLRBuilder& machine) noexcept:ruleIndex(_ruleIndex), position(_position), followToken(make_unique<unordered_set<ParserSymbol*>>())
			{
				AddIsProductEndPosition(machine);
			}
			ProductPosition(int _ruleIndex, PDANode* _position, GeneralLRBuilder& machine,ParserSymbol* initSymbol) noexcept:ruleIndex(_ruleIndex), position(_position), followToken(make_unique<unordered_set<ParserSymbol*>>())
			{
				followToken->emplace(initSymbol);
				AddIsProductEndPosition(machine);
			}
			ProductPosition(int _ruleIndex, PDANode* _position,GeneralLRBuilder& machine,const unordered_set<ParserSymbol*>& initSymbolList) noexcept : ruleIndex(_ruleIndex), position(_position), followToken(make_unique<unordered_set<ParserSymbol*>>())
			{
				followToken->insert(initSymbolList.begin(), initSymbolList.end());
				AddIsProductEndPosition(machine);
			}
			ProductPosition(ProductPosition&&) noexcept = default;
			ProductPosition(const ProductPosition&) noexcept = default;
			ProductPosition& operator=(ProductPosition&&) noexcept = default;
			ProductPosition& operator=(const ProductPosition&) noexcept = default;
			~ProductPosition() = default;
			PDANode* GetPosition()const;
			bool IsProductEndPosition()const;
			int GetRuleIndex()const;
			bool HasEndAction()const;
			const vector<ActionWrap>& GetEndAction()const;
			void AddFollowToken(ParserSymbol* symbol);
			void AddFollowToken(const unordered_set<ParserSymbol*>&symbolList);
			const unordered_set<ParserSymbol*>& GetFollowTokens()const;
			wstring LogProduct(const GeneralLRBuilder& LRMachine)const;
			wstring LogFollowToken()const;
			wstring LogEndAction()const;
			
		private:
			void AddEndAction(GeneralLRBuilder& LRMachine);
			void AddIsProductEndPosition(GeneralLRBuilder& LRMachine);
		};

		class LRNode
		{
			int number;
			vector<ProductPosition> items;
			unordered_map<ParserSymbol*, LRNode*> nexts;
			int coreItemEnd;
			unordered_map<PDANode*, int> itemsMap;
			unordered_map<ParserSymbol*, vector<PDAEdge*>> gotoMap;
			unordered_map<PDANode*, unordered_set<PDANode*>> generateItemMap;
			
		public:
			LRNode() = delete;
			LRNode(int _number);
		public:
			
			const vector<PDAEdge*>* GetGotoEdges(ParserSymbol*symbol)const;
			LRNode* GetNextLRNode(ParserSymbol*symbol)const;
			void AddGotoMap(unordered_map<ParserSymbol*, vector<PDAEdge*>>&& target);
			void AddCoreItem(ProductPosition&& item);
			void AddItem(ProductPosition&& item);
			void AddCoreEndAndComputeAllItems(GeneralLRBuilder & LRMachine);
			const ProductPosition& GetProductByPDANode(PDANode*)const;
			wstring LogProductList(const GeneralLRBuilder & LRMachine)const;
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
			ProductPosition& GetMutableProductByPDANode(PDANode*);
			ProductPosition& GetMutableProductByIndex(size_t);
			wstring LogLRNode(const GeneralLRBuilder& LRMachine)const;

			void LRNode::UpdateGenerateItemFollowSet(PDANode* parent);
		private:
			void ComputeAllItems(GeneralLRBuilder & LRMachine);
			void BuildCoreItemsMap();
			void BuildItemsMap();
			void LRNode::UpdateGenerateItemRelation( PDANode* parent, PDANode* child);
			
			vector<ProductPosition*> GetProductPositionChildren(ProductPosition* parent);
			
		};
		struct coreItemHash
		{
			size_t operator()(const vector<PDANode*> & _Keyval) const;
		};
		class GeneralLRBuilder
		{
		public:
			GeneralLRBuilder() = delete;
			GeneralLRBuilder(const shared_ptr<GrammarBuilder>& _machine);
			~GeneralLRBuilder() noexcept = default;
			GeneralLRBuilder(GeneralLRBuilder&&) = default;
			GeneralLRBuilder(const GeneralLRBuilder&) = default;
			GeneralLRBuilder& operator=(GeneralLRBuilder&&) = default;
			GeneralLRBuilder& operator=(const GeneralLRBuilder&) = default;
		public:
			SymbolManager*				GetSymbolManager()const;
			GeneralTableDefine*			GetTable()const;
			const unordered_map<wstring, pair<PDANode*, PDANode*>>& GetPDAMap()const;
			GrammarBuilder&		GetMachine()const;
			PDAEdge* IsCreatePosition(PDANode*position)const;
			LRNode*  GetLRMachineStartNode()const;
			ParserSymbol * GetProductHeadRuleSymbolByPosition(const ProductPosition * product) const;

		public:
			void BuildLRItems();
			LRNode* GetInitLRNode();
			LRNode* NewEmptyLRNode();
			unique_ptr<vector<ActionWrap>> GetEndAction(PDAEdge* edge)const;
			int GetRuleIndex(PDANode* node)const;
			int GetRuleIndexByName(const wstring& name)const;
			const wstring& GetRuleNameByIndex(int index)const;
			PDANode* GetRuleStart(const wstring& name)const;
			const unordered_set<ParserSymbol*>& GetFirstSetByNode(PDANode* node);
			bool CheckShiftAndReduceConfilct()const;
			void LogItems(const wstring& fileName)const;
			wstring LogPDANode(const wstring & ruleName, PDANode * node) const;
			void LALRConfilctDetection();
			PDANode* GetStartNodeByProduction(const ProductPosition* product)const;
			const unordered_map<ParserSymbol*, unordered_set<ParserSymbol*>> &  GetFirstSet()const;
			void CalculateFirstSet();
			void PropagateLRItemFollowSymbol();
			void PropagateFollowInfoToNextPosition(deque<pair<LRNode*, ProductPosition*>>& products);
			void PropagateFollowInfoToNextPosition(LRNode* node, ProductPosition* product, deque<pair<LRNode*, ProductPosition*>>& queue);

		private:
			unordered_map<ParserSymbol*, vector<PDAEdge*>> GetGotoInfo(LRNode* node)const;
			void AddCoreItemMap(LRNode* init);
			PDANode* GetRootRuleStart();
			LRNode* HasTheSameCoreItem(const vector<PDANode*>& expect)const;
			LRNode* AddLRItem(const vector<PDANode*>& coreItem,const unordered_map<PDANode*, const unordered_set<ParserSymbol*>*>&coreItemToFollowSetMap);
			LRNode* GeneralLRBuilder::AddLRItem(const vector<PDANode*>& coreItem);
			
			const vector<PDAEdge*>& GetEdgesByNode(PDANode* node)const;

			void CalculateFirstSet(ParserSymbol* ruleSymbol);
			
		private:
			shared_ptr<GrammarBuilder> machine;
			vector<shared_ptr<LRNode>> nodePool;
			unordered_map<ParserSymbol*, unordered_set<ParserSymbol*>> firstSet;
			unordered_map<PDANode*, unordered_set<ParserSymbol*>> firstNodeSet;
			unordered_map<vector<PDANode*>, int, coreItemHash> coreItemMap;
		};
	}
}