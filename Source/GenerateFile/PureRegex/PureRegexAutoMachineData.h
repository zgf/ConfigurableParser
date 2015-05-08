#pragma once
#include "..\..\Include\stdafx.h"
#include "..\..\Include\GeneralTableDefine.h"
namespace ztl
{
	namespace pure_regex
	{
		
		struct Alternate;
		struct TokenInfo;
		using TokenPacket = ztl::general_parser::GeneralTokenDefine;
		class TokenList
		{
			vector<TokenPacket*> tokens;
			unordered_map<wstring, int> tokenNameIndexMap;
		public:
			TokenList() = delete;
			TokenList(const vector<shared_ptr<TokenPacket>>& infos)
			{
				auto count = 0;
				for (auto&&iter:infos)
				{
					tokens.emplace_back(iter.get());
					tokenNameIndexMap.insert({iter->name,count });
					++count;
				}
			}
			~TokenList() = default;
			auto begin()
			{
				return tokens.begin();
			}
			auto end()
			{
				return tokens.end();
			}
			bool HasThisToken(const wstring& name)const
			{
				auto findIter = tokenNameIndexMap.find(name);
				return findIter != tokenNameIndexMap.end();
			}
			TokenPacket* GetToken(const wstring& name)const
			{
				auto findIter = tokenNameIndexMap.find(name);
				return (findIter != tokenNameIndexMap.end())?tokens[findIter->second]:nullptr;
			}
			TokenPacket* GetToken(int index)const
			{
				assert(index >= 0 && index < (int)tokens.size());
				return tokens [index]? tokens[index] : nullptr;
			}
		};
		class DFATableBuilder;
		class NFANode
		{
			friend DFATableBuilder;
			int number;
			unordered_map<unsigned short, unordered_set<NFANode*>> next;
		public:
			NFANode()  = default;
			~NFANode() noexcept = default;
			NFANode(NFANode&&)  = default;
			NFANode(const NFANode&)  = default;
			NFANode& operator=(NFANode&&)  = default;
			NFANode& operator=(const NFANode&)  = default;
			NFANode(int _number) :number(_number)
			{

			}
		public:
			const unordered_map<unsigned short, unordered_set<NFANode*>>& GetNextMap()const
			{
				return next;
			}
			int GetNumber()const
			{
				return number;
			}
			bool HaveCharIndex(unsigned short index)const
			{
				auto findIter = next.find(index);
				return findIter != next.end();
			}
			const unordered_set<NFANode*>& GetNextNFASet(unsigned short index)const
			{
				assert(HaveCharIndex(index));
				auto findIter = next.find(index);
				return findIter->second;
			}
		private:
			void AddOneNFANodeMap(unsigned short index, NFANode* node)
			{
				if (!HaveCharIndex(index))
				{
					next[index];
				}
				next[index].insert(node);
			}
		};
		class DFANode
		{
			int number;
			unordered_map<unsigned short, DFANode*> next;
		};
		class DFATable
		{
			vector<vector<unsigned short>> table;
		};
		
	}
}