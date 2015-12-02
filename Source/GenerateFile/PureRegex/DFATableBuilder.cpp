#include "Include\stdafx.h"
#include "PureRegexAutoMachineData.h"
#include "DFATableBuilder.h"
#include "CharsetMapTable.h"
namespace ztl
{
	namespace pure_regex
	{



		DFATableBuilder::DFATableBuilder(vector<shared_ptr<Alternate>>* _roots, TokenList* _tokens, CharsetMapTable* _charTable):tokens(_tokens),charTable(_charTable),roots(_roots)
		{
			assert(_roots != nullptr);
			assert(_tokens != nullptr);
			assert(_charTable != nullptr);
			BuildNFA(this);
			this->NFAToDFATable();
		}

		NFANode* DFATableBuilder::GetNewNode()
		{
			auto index = nfaNodePool.size();
			this->nfaNodePool.emplace_back(make_shared<NFANode>((int)index));
			return nfaNodePool.back().get();
		}

		void DFATableBuilder::AddNullLink(NFANode* left, NFANode* right)
		{
			left->AddOneNFANodeMap(CharsetMapTable::GetNullEdge(), right);
		}

		void DFATableBuilder::AddLink(NFANode* left, NFANode* right, wchar_t begin, wchar_t end)
		{
			auto first = charTable->GetCharSetMap(begin);
			auto second = charTable->GetCharSetMap(end);
			assert(first <= second);
			for(auto i = first; i <= second; ++i)
			{
				left->AddOneNFANodeMap(i, right);
			}
		}

		void DFATableBuilder::AddLink(NFANode* left, NFANode* right, wchar_t val)
		{
			left->AddOneNFANodeMap(charTable->GetCharSetMap(val),right);
		}

		TokenList* DFATableBuilder::GetTokens() const
		{
			return tokens;
		}

		vector<shared_ptr<Alternate>>* DFATableBuilder::GetRoots() const
		{
			return roots;
		}

		void DFATableBuilder::AddStopStates(int nfaNumber, int tokenIndex)
		{
			assert(stopStates.find(nfaNumber) == stopStates.end());
			stopStates.insert({ nfaNumber,tokenIndex });
		}

		void DFATableBuilder::SetNFA(NFANode* _start, NFANode* _end)
		{
			nfa.first = _start;
			nfa.second = _end;
		}
		//获得当前集合的null转换集合
		unordered_set<NFANode*> GetNullChangeSet(const unordered_set<NFANode*>& current)
		{
			unordered_set<NFANode*> result;
			auto nullChar = CharsetMapTable::GetNullEdge();
			for (auto&&iter:current)
			{
				result.insert(iter);
				if(iter->HaveCharIndex(nullChar))
				{
					auto temp = iter->GetNextNFASet(nullChar);
					result.insert(temp.begin(),temp.end());
				}
			}
			return result;
		}
		unordered_map<unsigned short, unordered_set<NFANode*>> GetCharMap(const unordered_set<NFANode*>& nodes)
		{
			unordered_map<unsigned short, unordered_set<NFANode*>> result;
			for (auto&&iter:nodes)
			{
				for (auto&& charIter:iter->GetNextMap())
				{
					if (charIter.first!=CharsetMapTable::GetNullEdge())
					{
						result[charIter.first].insert(charIter.second.begin(), charIter.second.end());
					}
				}
			}
			return result;
		}
		int DFATableBuilder::GetDFAStopState(const unordered_set<NFANode*>& nodes) const
		{
			assert(!nodes.empty());
			auto min = INT_MAX;
			for (auto&& iter:nodes)
			{
				auto findIter = stopStates.find(iter->GetNumber());
				if (findIter != stopStates.end())
				{
					min = std::min(findIter->second, min);
				}
			}
			return min;
		}
		
		void DFATableBuilder::NFAToDFATable()
		{
			
			//获得null转换集合.
			//对整个集合求特定char的映射.
			//映射到的集合是新的DFA节点
			unordered_map<int, int> dfaStopStates;
			unordered_map<unordered_set<NFANode*>, unsigned short, ztl_hash<unordered_set<NFANode*>>> nfaToDfa;
			deque<unordered_set<NFANode*>> queue;
			unordered_set<unordered_set<NFANode*>, ztl_hash<unordered_set<NFANode*>>> nfaSign;
			auto count = 0;
			auto SetDFAStopState = [this, &nfaToDfa, &dfaStopStates](const unordered_set<NFANode*>& front)
			{
				auto index = GetDFAStopState(front);
				if(index != INT_MAX)
				{
					dfaStopStates.insert({ nfaToDfa[front],index });
				}
			};
			auto SetNewDFAAndMark = [this,&queue,&nfaToDfa,&count]()
			{
				nfaToDfa.insert({ queue.back(),count });
				++count;
				dfaTable.emplace_back(vector<unsigned short>(charTable->GetCount(), std::numeric_limits<unsigned short>::max()));
			};
			queue.emplace_back();
			queue.back().insert(nfa.first);
			queue.back() = GetNullChangeSet(queue.back());
			nfaSign.insert(queue.back());
			SetNewDFAAndMark();
			while(!queue.empty())
			{
				auto&& front = queue.front();

				//设置DFAStopState
				SetDFAStopState(front);

				auto charMap = GetCharMap(front);
				assert(nfaToDfa.find(front) != nfaToDfa.end());
				for (auto&& charIter:charMap)
				{
					assert(charIter.first != CharsetMapTable::GetNullEdge());
					if (nfaSign.find(charIter.second)==nfaSign.end())
					{
						nfaSign.insert(charIter.second);
						queue.emplace_back(charIter.second);
						if (nfaToDfa.find(charIter.second) == nfaToDfa.end())
						{
							//未出现过的dfa
							SetNewDFAAndMark();
						}
						dfaTable[nfaToDfa[front]][charIter.first] = nfaToDfa[charIter.second];
					}
				}
				queue.pop_front();
			}
			stopStates = dfaStopStates;
		}
	}
}