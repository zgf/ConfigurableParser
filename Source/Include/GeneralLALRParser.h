#pragma once
#include "GeneralParserBase.h"
namespace ztl
{
	namespace general_parser
	{

		 class GeneralLALRParser: public GeneralParserBase
		{
		public:
			GeneralLALRParser() = delete;
			GeneralLALRParser(const shared_ptr<GeneralTableDefine>& _tableDefine);

			~GeneralLALRParser() noexcept = default;
			GeneralLALRParser(GeneralLALRParser&&) = default;
			GeneralLALRParser(const GeneralLALRParser&) = default;
			GeneralLALRParser& operator=(GeneralLALRParser&&) = default;
			GeneralLALRParser& operator=(const GeneralLALRParser&) = default;
		public:
			virtual void GenerateIsomorphismParserTree()override;
			virtual void ClearEnvironment()override;
			
			
		private:
			ParserSymbol* GetTokenSymbol(size_t tokenIndex)const;
			pair<bool,PDAEdge*> NeedMove(PDANode*node, LRNode* LRNode, ParserSymbol* symbol);
			void ExcuteEdgeAdditionAction(const pair<PDAEdge*, int>& grammar, GeneralTreeNode* node);
			GeneralTreeNode* ExcuteEndAction(const vector<ActionWrap>& acionts);
			ParserSymbol* ExceteReduceAction(PDANode* currentPDANode, GeneralTreeNode* node, size_t nodeIndex);
			ParserSymbol* ExceteReduceWithoutEndAction(PDANode* currentPDANode);
			const vector<PDAEdge*>& FindTheNodePathEdges(PDANode* start, PDANode* end);
			void ExecutableEdgeAddctionAction(const vector<pair<PDAEdge*, int>>&edges, GeneralTreeNode* node);
			bool IsParserFinish(ParserSymbol* node,ParserSymbol*tokenSymbol)const;
		private:
			vector<int>							 treeNodeStack;
			vector<pair<PDAEdge*,int>>			 grammarStack;
			vector<LRNode*>						 LRNodeStack;
			vector<PDANode*>					 PDANodeStack;
			unordered_map<pair<PDANode*, PDANode*>,vector<PDAEdge*>,ztl_hash<pair<PDANode*, PDANode*>>> findPathCacheMap;
		};
	}
}