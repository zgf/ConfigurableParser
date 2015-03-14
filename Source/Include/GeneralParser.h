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
		class GeneralParser
		{
		public:
			GeneralParser()  = default;
			~GeneralParser() noexcept = default;
			GeneralParser(GeneralParser&&)  = default;
			GeneralParser(const GeneralParser&)  = default;
			GeneralParser& operator=(GeneralParser&&)  = default;
			GeneralParser& operator=(const GeneralParser&)   = default;
		private:
			vector<shared_ptr<GeneralTreeNode>>  nodePool;
			vector<TokenInfo>					 tokenPool;
			vector<GeneralTreeNode*>			 createdNodeStack;
			vector<wstring>						 rulePathStack;
			GeneralTreeNode*					 treeRoot;


		};
		vector<TokenInfo> ParseToken(const wstring& fileName);

	}
}