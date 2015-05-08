#pragma once
#include "..\..\Include\stdafx.h"
namespace ztl
{
	namespace pure_regex
	{
		struct Alternate;
		class CharsetMapTable;
		class NFANode;
		class DFATableBuilder
		{
			DFATableBuilder() = delete;
			DFATableBuilder(vector<shared_ptr<Alternate>>* _roots,TokenList*	 _tokens, CharsetMapTable*	_charTable);
			~DFATableBuilder() = default;
			DFATableBuilder(const DFATableBuilder&) = default;
			DFATableBuilder(DFATableBuilder&&) = default;
			DFATableBuilder& operator = (DFATableBuilder&&) = default;
			DFATableBuilder& operator=(const DFATableBuilder&) = default;
		public:
			void AddNullLink(NFANode* left, NFANode* right);
			void AddLink(NFANode* left, NFANode* right,wchar_t val);
			void AddLink(NFANode* left, NFANode* right, wchar_t begin,wchar_t end);
			NFANode* GetNewNode();
			TokenList* GetTokens()const;
			vector<shared_ptr<Alternate>>* GetRoots()const;
			void AddStopStates(int nfaNumber, int tokenIndex);
			void SetNFA(NFANode* _start, NFANode* _end);
			void NFAToDFATable();
		private:
			int GetDFAStopState(const unordered_set<NFANode*>& nodes)const;
		private:
			TokenList*					tokens;
			CharsetMapTable*			charTable;
			vector<shared_ptr<Alternate>>* roots;
			vector<shared_ptr<NFANode>> nfaNodePool;
			pair<NFANode*,NFANode*>		nfa;
			unordered_map<int, int>		stopStates;
			vector<vector<unsigned short>> dfaTable;
		};
		shared_ptr<CharsetMapTable> BuildCharsetMapTable(const vector<shared_ptr<Alternate>>& roots);
		vector<shared_ptr<Alternate>> ParsingAllTokensToAst(const vector<TokenPacket>& tokens);
	}
}