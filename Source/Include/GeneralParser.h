#pragma once
#include "stdafx.h"
namespace ztl
{
	namespace general_parser
	{
		class PDAEdge;
		class GeneralTreeNode;
		struct GeneralTokenInfo;
		class TokenInfo;
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
			vector<shared_ptr<GeneralTokenInfo>> tokenPool;
			vector<GeneralTreeNode*>			 createdNodeStack;
			vector<wstring>						 rulePathStack;
			//ĳ�߹�ȥ�����ǰ���ս��ַ�������LALR(2);ÿ���ߵ����һ��action�����ս��ַ�.
			unordered_map<PDAEdge*, vector<ActionWrap>> lookaheadMap;
			GeneralTreeNode* treeRoot;
		};
		vector<TokenInfo> Parse(const wstring& fileName, SymbolManager* manager);

	}
}