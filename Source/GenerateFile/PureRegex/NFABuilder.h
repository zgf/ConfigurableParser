#pragma once
#include "..\..\Include\stdafx.h"
namespace ztl
{
	namespace pure_regex
	{
		struct Alternate;
		class CharsetMapTable;
		class NFANode;
		class NFABuilder
		{
			NFABuilder() = delete;
			NFABuilder(vector<shared_ptr<Alternate>>* _roots,TokenList*	 _tokens, CharsetMapTable*	_charTable);
			~NFABuilder() = default;
			NFABuilder(const NFABuilder&) = default;
			NFABuilder(NFABuilder&&) = default;
			NFABuilder& operator = (NFABuilder&&) = default;
			NFABuilder& operator=(const NFABuilder&) = default;
		public:
			void AddNullLink(NFANode* left, NFANode* right);
			void AddLink(NFANode* left, NFANode* right,wchar_t val);
			void AddLink(NFANode* left, NFANode* right, wchar_t begin,wchar_t end);
			NFANode* GetNewNode();
			TokenList* GetTokens()const;
			vector<shared_ptr<Alternate>>* GetRoots()const;
			void AddStopStates(int nfaNumber, int tokenIndex);
			void SetNFA(NFANode* _start, NFANode* _end);
		private:
			void BuildNFASet(NFANode* _start);
		private:
			TokenList*					tokens;
			CharsetMapTable*			charTable;
			vector<shared_ptr<Alternate>>* roots;

			vector<shared_ptr<NFANode>> nfaNodePool;
			pair<NFANode*,NFANode*>		nfa;
			unordered_map<int, int>		nfaStopStates;
		};
		shared_ptr<CharsetMapTable> BuildCharsetMapTable(const vector<shared_ptr<Alternate>>& roots);
		vector<shared_ptr<Alternate>> ParsingAllTokensToAst(const vector<TokenPacket>& tokens);
	}
}