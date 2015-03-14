#pragma once
#include "Include/stdafx.h"
#include "Include/GeneralTableDefine.h"
#include "../Lib/Regex/ztl_regex_interpretor.h"

namespace ztl
{
	namespace general_parser
	{
		vector<TokenInfo> ParseToken(const wstring& fileName)
		{
			unordered_map<wstring, GeneralTokenDefine> infos;
			infos.insert({ L"CLASS",GeneralTokenDefine(L"CLASS", LR"(class)", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"STRUCT",GeneralTokenDefine(L"STRUCT", LR"(struct)", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"ENUM",GeneralTokenDefine(L"ENUM", LR"(enum)", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"TOKEN",GeneralTokenDefine(L"TOKEN", LR"(token)", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"DISCARDTOKEN",GeneralTokenDefine(L"DISCARDTOKEN", LR"(ignoretoken)", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"RULE",GeneralTokenDefine(L"RULE", LR"(rule)", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"AS",GeneralTokenDefine(L"AS", LR"(as)", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"WITH",GeneralTokenDefine(L"WITH", LR"(with)", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"OPEN",GeneralTokenDefine(L"OPEN", LR"(\{)", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"CLOSE",GeneralTokenDefine(L"CLOSE", LR"(\})", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"SEMICOLON",GeneralTokenDefine(L"SEMICOLON", LR"(;)", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"COLON",GeneralTokenDefine(L"COLON", LR"(:)", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"COMMA",GeneralTokenDefine(L"COMMA", LR"(,)", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"DOT",GeneralTokenDefine(L"DOT", LR"(\.)", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"ASSIGN",GeneralTokenDefine(L"ASSIGN", LR"(=)", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"USING",GeneralTokenDefine(L"USING", LR"(!)", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"OR",GeneralTokenDefine(L"OR", LR"(\|)", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"OPTOPEN",GeneralTokenDefine(L"OPTOPEN", LR"(\[)", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"OPTCLOSE",GeneralTokenDefine(L"OPTCLOSE", LR"(\])", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"PREOPEN",GeneralTokenDefine(L"PREOPEN", LR"(\()", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"PRECLOSE",GeneralTokenDefine(L"PRECLOSE", LR"(\))", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"NAME",GeneralTokenDefine(L"NAME", LR"([a-zA-Z_]\w*)", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"STRING",GeneralTokenDefine(L"STRING",  LR"("(\\"|[^"])*")", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"FINISH",GeneralTokenDefine(L"FINISH", LR"(<\$>)", GeneralTokenDefine::TokenOptional::False) });
			infos.insert({ L"SPACE",GeneralTokenDefine(L"SPACE", LR"(\s+)", GeneralTokenDefine::TokenOptional::True) });
			infos.insert({ L"LINENOTE",GeneralTokenDefine(L"LINENOTE",LR"(\"(\\"|[^"])*")", GeneralTokenDefine::TokenOptional::True) });
			infos.insert({ L"BLOCKNOTE",GeneralTokenDefine(L"BLOCKNOTE", LR"(/*.*?*/ //)", GeneralTokenDefine::TokenOptional::True) });
			wifstream input(fileName);
			if(!input.is_open())
			{
				throw ztl_exception(L"error:file" + fileName + L"not open!");
			}
			vector<TokenInfo> stream;
			std::wstring content((std::istreambuf_iterator<wchar_t>(input)),
				std::istreambuf_iterator<wchar_t>());
			auto pattern = accumulate(infos.begin(), infos.end(), wstring(), [](const wstring& sum, const pair<const wstring, GeneralTokenDefine>& token)
			{
				return sum + L"(<" + token.second.name + L">" + token.second.regex + L")|";
			});
			pattern.pop_back();
			RegexInterpretor interpretor(pattern);
			auto result = interpretor.Matches(content);
			for(auto&& iter : result)
			{
				assert(iter.group.size() == 1);
				auto groupIter = *iter.group.begin();
				auto name = groupIter.first;
				assert(infos.find(name) != infos.end());
				if(infos[name].ignore != GeneralTokenDefine::TokenOptional::True)
				{
					stream.emplace_back(name, groupIter.second.content, groupIter.second.position, groupIter.second.length);
				}
			}
			return stream;
		}
	}
}