#include "Include\stdafx.h"
#include "Include\GeneralLexer.h"
#include "Include\GeneralTableDefine.h"
const std::unordered_map<wchar_t, std::function<int(int&, int)>>  ztl::general_parser::RegexLexme::actions =
{
	{ L'(',[](int& next,int) {return ++next,static_cast<int>(RegexCharType::LeftBracket);} } ,
	{ L')',[](int& next,int) {return ++next, static_cast<int>(RegexCharType::RightBracket);} } ,
	{ L']',[](int& next,int) {return  ++next,static_cast<int>(RegexCharType::RightSquare);} } ,
	{ L'?',[](int& next,int) {return  ++next,static_cast<int>(RegexCharType::Greedy);} } ,
	{ L'*',[](int& next,int) {return  ++next,static_cast<int>(RegexCharType::Keleen);} } ,
	{ L'+',[](int& next,int) {return  ++next,static_cast<int>(RegexCharType::Positive);} } ,
	{ L'-',[](int& next,int) {return  ++next,static_cast<int>(RegexCharType::Kern);} } ,

	{ L'[',[](int& next,int nextChar) 
		{
			return nextChar == L'^' ?
				next+=2,static_cast<int>(RegexCharType::NegativeBracket) : 
				++next,static_cast<int>(RegexCharType::LeftSquare);
		} 
	} ,
	{ L'\\',[](int& next,int nextChar)
		{
			next += 2;
			return  nextChar == L's' ? static_cast<int>(RegexCharType::s) :
				nextChar == L'S' ? static_cast<int>(RegexCharType::S) :
				nextChar == L'D' ? static_cast<int>(RegexCharType::D) :
				nextChar == L'd' ? static_cast<int>(RegexCharType::d) :
				nextChar == L'W' ? static_cast<int>(RegexCharType::W) :
				nextChar == L'w' ? static_cast<int>(RegexCharType::w) : nextChar;
		}
	} ,
	{ 0,[](int& next,int currentChar)
		{
			return next++,currentChar;
		}
	} ,
};

ztl::general_parser::RegexLexme::RegexLexme(const wstring & regex)
{
	bool inCharSet=false;
	for (int index = 0;index != (int)regex.size();)
	{
		auto&& findIter = actions.find(regex[index]);
		int token = 0;
		if (findIter != actions.end())
		{
			token = findIter->second(index, regex[index + 1]);
		}
		else
		{
			token = actions.find(0)->second(index, regex[index]);
		}
		assert(token != 0);

		if (token == static_cast<int>(RegexCharType::LeftSquare) ||token == static_cast<int>(RegexCharType::NegativeBracket))
		{
			inCharSet = true;
		}
		else if(token == static_cast<int>(RegexCharType::RightSquare))
		{
			inCharSet = false;
		}
		else if(token == static_cast<int>(RegexCharType::Kern) && inCharSet == false)
		{
			token = L'-';
		}

		this->regex.emplace_back(token);
	}
	AdjustSymbolPriority();
}

const std::vector<int>& ztl::general_parser::RegexLexme::GetRegex() const
{
	return regex;
}

void ztl::general_parser::RegexLexme::AdjustSymbolPriority()
{
	//todo 逆波兰表达式
}

std::vector<ztl::general_parser::CharRange> ztl::general_parser::RegexLexme::GetCharRanges()
{
	//生成charrange
	return;
}


std::unique_ptr<ztl::general_parser::NFATable> ztl::general_parser::RegexLexme::BuildNFATable(GeneralCharSetTable & charSetTable)
{
	//托马斯算法
	return ;
}

ztl::general_parser::GeneralCharSetTable::GeneralCharSetTable(int number):charNumber(number),table(number,0)
{

}
void ztl::general_parser::GeneralCharSetTable::BuildCharSetTable(const vector<CharRange>& ranges)
{
	int count = 1;
	for (auto&& range : ranges)
	{
		count = WriteRange(range, count);
	}
}

int ztl::general_parser::GeneralCharSetTable::WriteRange(const CharRange & range, int nextCount)
{
	auto begin = range.low;
	auto end = range.high;
	auto old = nextCount;
	for (;begin <= end;++begin)
	{
		if (table[begin] != 0)
		{
			++nextCount;
		}
		table[begin] = nextCount;
	}
	return old == nextCount ? nextCount + 1 : nextCount;
}

void ztl::general_parser::GeneralCharSetTable::AdjustTableIndex()
{
	unordered_map<int,int> indexMap;
	auto&& count = 1;
	for (auto&& iter : this->table)
	{
		indexMap.insert({ iter ,count});
		++count;
	}

	for (size_t i = 0; i < table.size(); i++)
	{
		table[i] = indexMap[table[i]];
	}

}

const std::vector<int>& ztl::general_parser::GeneralCharSetTable::GetCharSetTable() const
{
	return table;
}

ztl::general_parser::GeneralLexer::GeneralLexer(const vector<shared_ptr<GeneralTokenDefine>>& tokens):charSetTable(65535)
{
	for (auto&& token : tokens)
	{
		Lexme lex(token->name,token->regex);
		this->lexmes.emplace_back(move(lex));
	}
}

void ztl::general_parser::GeneralLexer::BuildDFATable()
{
	vector<unique_ptr<NFATable>> nfaTables;
	for (auto&&lexme : this->lexmes)
	{
		nfaTables.emplace_back(lexme.lexme.BuildNFATable(charSetTable));
	}
	auto&& nfaTable = NFATable::MergeNFATable(nfaTables, charSetTable);
	this->table = move(nfaTable->BuildDFATable(charSetTable));
}

void ztl::general_parser::GeneralLexer::BuildCharSetTable()
{
	for (auto&&lexme : this->lexmes)
	{
		auto&& ranges = lexme.lexme.GetCharRanges();
		charSetTable.BuildCharSetTable(ranges);
	}
	charSetTable.AdjustTableIndex();
}

ztl::general_parser::Lexme::Lexme(const wstring & _name, const wstring & _regex):name(_name),lexme(_regex)
{
}

std::unique_ptr<ztl::general_parser::DFATable> ztl::general_parser::NFATable::BuildDFATable(GeneralCharSetTable & charSetTable)
{
	//构建DFA
	return ;
}

std::unique_ptr<ztl::general_parser::NFATable> ztl::general_parser::NFATable::MergeNFATable(vector<unique_ptr<NFATable>>& nfas, GeneralCharSetTable & charSetTable)
{
	//合并NFA
	return;
}
