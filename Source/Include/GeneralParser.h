#pragma once
#include "stdafx.h"
namespace ztl
{
	namespace general_parser
	{
		class PDAEdge;
		class GeneralTreeNode;
		struct TokenInfo;
		class SymbolManager;
		class ActionWrap;
		class GeneralJumpTable;
		struct GeneralTableDefine;
		class GeneralParser
		{
		public:
			GeneralParser() = delete;
			GeneralParser(const vector<TokenInfo>& tokens,const shared_ptr<GeneralTableDefine>& _tableDefine);
			~GeneralParser() noexcept = default;
			GeneralParser(GeneralParser&&)  = default;
			GeneralParser(const GeneralParser&)  = default;
			GeneralParser& operator=(GeneralParser&&)  = default;
			GeneralParser& operator=(const GeneralParser&)   = default;
		public:
			void BuildParser();
			GeneralTreeNode* Run();
		private:
			vector<shared_ptr<GeneralTreeNode>>  nodePool;
			vector<TokenInfo>					 tokenPool;
			vector<GeneralTreeNode*>			 createdNodeStack;
			vector<wstring>						 rulePathStack;
			GeneralTreeNode*					 treeRoot;
			shared_ptr<GeneralJumpTable>		 table;
			shared_ptr<GeneralTableDefine>		 tableDefine;

		};
		vector<TokenInfo> ParseToken(const wstring& fileName);

	}
}