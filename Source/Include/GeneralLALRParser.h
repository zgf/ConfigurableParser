#pragma once
#include "GeneralParserBase.h"
namespace ztl
{
	namespace general_parser
	{
		class GeneralLALRParser:public GeneralParserBase
		{
		public:
			GeneralLALRParser() = delete;
			GeneralLALRParser(const wstring& fileName, const shared_ptr<GeneralTableDefine>& _tableDefine);

			~GeneralLALRParser() noexcept = default;
			GeneralLALRParser(GeneralLALRParser&&) = default;
			GeneralLALRParser(const GeneralLALRParser&) = default;
			GeneralLALRParser& operator=(GeneralLALRParser&&) = default;
			GeneralLALRParser& operator=(const GeneralLALRParser&) = default;
		public:
			virtual void GenerateIsomorphismParserTree()override;
		private:
			ParserSymbol* GetTokenSymbol(int tokenIndex)const;
			pair<bool,PDAEdge*> NeedMove(PDANode*node, LRNode* LRNode, ParserSymbol* symbol);
			void ExcuteEdgeAdditionAction(const pair<PDAEdge*, int>& grammar, GeneralTreeNode* node);
			GeneralTreeNode* ExcuteEndAction(const vector<ActionWrap>& acionts);
			ParserSymbol* ExceteReduceAction(PDANode* currentPDANode, GeneralTreeNode* node, int nodeIndex);
			ParserSymbol* ExceteReduceWithoutEndAction(PDANode* currentPDANode);

			bool IsParserFinish(size_t tokenIndex,ParserSymbol* node)const;
		private:
			vector<int>							 treeNodeStack;
			vector<pair<PDAEdge*,int>>			 grammarStack;
			vector<LRNode*>						 LRNodeStack;
			vector<PDANode*>					 PDANodeStack;
		};
	}
}