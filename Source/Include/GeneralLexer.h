#pragma once
#include "stdafx.h"
namespace ztl
{
	namespace general_parser
	{
		struct GeneralTokenDefine;
		class GeneralDFAMachine;
		class GeneralNFAMachine;
		class GeneralRegexExpression;
		class GeneralCharSetTable;
		struct CharRange
		{
			int low;
			int high;
		};
		enum class RegexCharType:int
		{
			Dot=65537,
			LeftBracket,
			RightBracket,
			NegativeBracket,
			Greedy,
			Positive,
			Keleen,
			LeftSquare,
			RightSquare,
			Kern,
			W,
			w,
			D,
			d,
			S,
			s,
		};
		class NFATable;
		class RegexLexme
		{
		public:
			RegexLexme() = default;
			~RegexLexme() = default;
			RegexLexme(RegexLexme&&) = default;
			RegexLexme(const RegexLexme&) = default;
			RegexLexme& operator=(RegexLexme&&) = default;
			RegexLexme& operator=(const RegexLexme&) = default;
		public:
			RegexLexme(const wstring& regex);
			const vector<int>& GetRegex()const;
			void AdjustSymbolPriority();
			vector<CharRange> GetCharRanges();
			unique_ptr<NFATable> BuildNFATable(GeneralCharSetTable& charSetTable);
		private:
			vector<int> regex;
			static const std::unordered_map<wchar_t, std::function<int(int&, int)>> actions;
		};
		class DFATable;
		class NFATable
		{
		public:
			NFATable() = default;
			~NFATable() = default;
			NFATable(NFATable&&) = default;
			NFATable(const NFATable&) = default;
			NFATable& operator=(NFATable&&) = default;
			NFATable& operator=(const NFATable&) = default;
		public:
			unique_ptr<DFATable> BuildDFATable(GeneralCharSetTable& charSetTable);
			static unique_ptr<NFATable> MergeNFATable(vector<unique_ptr<NFATable>>& nfas,GeneralCharSetTable& charSetTable);
		private:
			vector<vector<vector<int>>> table;
		};
		class DFATable
		{
			public:
			vector<vector<int>> table;
		};
		class GeneralCharSetTable
		{
		public:
			GeneralCharSetTable()=delete;
			~GeneralCharSetTable()=default;
			GeneralCharSetTable(GeneralCharSetTable&&)=default;
			GeneralCharSetTable(const GeneralCharSetTable&)=default;
			GeneralCharSetTable& operator=(GeneralCharSetTable&&)=default;
			GeneralCharSetTable& operator=(const GeneralCharSetTable&)=default;
		public:
			GeneralCharSetTable(int number);
		public:
			void BuildCharSetTable(const vector<CharRange>& ranges);
			int WriteRange(const CharRange& range, int nextCount);
			void AdjustTableIndex();
			const vector<int>& GetCharSetTable() const;
		private:
			vector<int> table;
			int charNumber;
		};
		struct Lexme
		{
			Lexme() = delete;
			~Lexme() = default;
			Lexme(Lexme&&) = default;
			Lexme(const Lexme&) = default;
			Lexme& operator=(Lexme&&) = default;
			Lexme& operator=(const Lexme&) = default;

			Lexme(const wstring& _name, const wstring& _regex);

			wstring name;
			RegexLexme lexme;
		};
	
		class GeneralLexer
		{
		public:
			GeneralLexer() = delete;
			GeneralLexer(const vector<shared_ptr<GeneralTokenDefine>> &tokens);
			~GeneralLexer() = default;
			GeneralLexer(GeneralLexer&&) = default;
			GeneralLexer(const GeneralLexer&) = default;
			GeneralLexer& operator=(GeneralLexer&&) = default;
			GeneralLexer& operator=(const GeneralLexer&) = default;
		public:
			void BuildDFATable();
			void BuildCharSetTable();
		private:
			unique_ptr<DFATable> table;
			vector<Lexme> lexmes;
			GeneralCharSetTable charSetTable;
		};
	}
}