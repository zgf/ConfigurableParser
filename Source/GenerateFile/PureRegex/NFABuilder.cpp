#include "Include\stdafx.h"
#include "PureRegexAutoMachineData.h"
#include "NFABuilder.h"
#include "CharsetMapTable.h"
namespace ztl
{
	namespace pure_regex
	{



		NFABuilder::NFABuilder(vector<shared_ptr<Alternate>>* _roots, TokenList* _tokens, CharsetMapTable* _charTable):tokens(_tokens),charTable(_charTable),roots(_roots)
		{

		}

		NFANode* NFABuilder::GetNewNode()
		{
			auto index = nfaNodePool.size();
			this->nfaNodePool.emplace_back(make_shared<NFANode>((int)index));
			return nfaNodePool.back().get();
		}

		void NFABuilder::AddNullLink(NFANode* left, NFANode* right)
		{
			left->AddOneNFANodeMap(CharsetMapTable::GetNullEdge(), right);
		}

		void NFABuilder::AddLink(NFANode* left, NFANode* right, wchar_t begin, wchar_t end)
		{
			auto first = charTable->GetCharSetMap(begin);
			auto second = charTable->GetCharSetMap(end);
			assert(first < second);
			for(auto i = first; i <= second; ++i)
			{
				left->AddOneNFANodeMap(i, right);
			}
		}

		void NFABuilder::AddLink(NFANode* left, NFANode* right, wchar_t val)
		{
			left->AddOneNFANodeMap(charTable->GetCharSetMap(val),right);
		}

		TokenList* NFABuilder::GetTokens() const
		{
			return tokens;
		}

		vector<shared_ptr<Alternate>>* NFABuilder::GetRoots() const
		{
			return roots;
		}

		void NFABuilder::AddStopStates(int nfaNumber, int tokenIndex)
		{
			assert(nfaStopStates.find(nfaNumber) == nfaStopStates.end());
			nfaStopStates.insert({ nfaNumber,tokenIndex });
		}

		void NFABuilder::SetNFA(NFANode* _start, NFANode* _end)
		{
			nfa.first = _start;
			nfa.second = _end;
		}

	}
}