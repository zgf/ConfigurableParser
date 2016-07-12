#pragma once
#include "stdafx.h"


namespace ztl
{
	namespace general_parser
	{
		class ParserSymbol;
		struct GeneralRuleDefine;
		struct GeneralGrammarTypeDefine;
		class SymbolManager;
		class GrammarBuilder;
		struct GeneralTableDefine;
		class PDANode;
		class PDAEdge;
		class CreatedNodeResolve;
		class ActionWrap;
		class GrammarBuilder
		{
		public:
			GrammarBuilder();
			GrammarBuilder(SymbolManager* _manager);
			~GrammarBuilder() = default;
			GrammarBuilder(GrammarBuilder&&) = default;
			GrammarBuilder(const GrammarBuilder&) = default;
			GrammarBuilder& operator=(GrammarBuilder&&) = default;
			GrammarBuilder& operator=(const GrammarBuilder&) = default;
		public:
			SymbolManager*				GetSymbolManager()const;
			GeneralTableDefine*			GetTable()const;
			unordered_map<wstring, pair<PDANode*, PDANode*>>& GetPDAMap();
			pair<PDANode*, PDANode*>	NewNodePair();
			PDANode*					NewNode();
			PDANode*					GetRoot() const;
			void						AddEdge(PDANode* source, PDANode* target, const ActionWrap& wrap);
			void						AddEdge(PDANode* source, PDANode* target, const vector<ActionWrap>& wrapList);
			void						DeleteEdge(PDAEdge* target);

			void						AddGeneratePDA(wstring ruleName, const pair<PDANode *, PDANode*>& pairNode);
			pair<PDANode*, PDANode*>	AddSequenceLinkNode(pair<PDANode*, PDANode*>& left, pair<PDANode*, PDANode*>&right);
			pair<PDANode*, PDANode*>	AddLoopLinkNode(PDANode* loopStart, PDANode* loopEnd);
			pair<PDANode*, PDANode*>	AddAlterationLinkNode(pair<PDANode*, PDANode*>& left, pair<PDANode*, PDANode*>&right);
			pair<PDANode*, PDANode*>	AddOptionalLinkNode(PDANode* optionalStart, PDANode* optionalEnd);
			void						FrontEdgesAdditionBackAction(PDANode* targetNode, const ActionWrap& wrap);
			void						FrontEdgesAdditionSetterAction(PDANode* targetNode, const ActionWrap& wrap);
			wstring						GetRootRuleName()const;
			//保留Left节点合并left right
			PDANode*					MergeIndependentNodes(PDANode* left, PDANode* right);
			void BackInsertAction(PDAEdge* edge, const ActionWrap& wrap);
			void FrontInsertAction(PDAEdge* edge, const ActionWrap& wrap);
			void AddNodeMapElement(PDANode* node, int ruleIndex);
			int GetRuleIndexByNode(PDANode* node)const;
			void AddNodeEdgeMapElement(PDANode* node, PDAEdge* edge);
			const vector<PDAEdge*>& GetEdgesByNode(PDANode* node)const;
			wstring LogPDANode(const wstring& ruleName,PDANode* node)const;
		private:
			PDAEdge* NewEdge(PDANode* source, PDANode* target, const ActionWrap& wrap);
			PDAEdge* NewEdge(PDANode* source, PDANode* target);

		private:
			vector<shared_ptr<PDAEdge>>											 edges;
			vector<shared_ptr<PDANode>>											 nodes;
			SymbolManager*														 manager;
			unordered_map<wstring, pair<PDANode*, PDANode*>>					 PDAMap;
			unordered_map<PDANode*, int>										 nodeMap;
			unordered_map<PDANode*, vector<PDAEdge*>>							 nodeEdgeMap;
		public:
		};
		vector<PDAEdge*> FindThePath(PDANode* start, PDANode* end);
		void CreateDPDAGraph(GrammarBuilder& machine);
		void MergeGrammarCommonFactor(GrammarBuilder& machine);
		void MergeStartAndEndNode(GrammarBuilder& machine, unordered_map<wstring, vector<pair<PDANode*, PDANode*>>>& PDAMap);
		extern void LogGraphInfo(const wstring& fileName, GrammarBuilder& machine);
	}
}