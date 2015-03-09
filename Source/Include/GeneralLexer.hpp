#pragma once
#include "stdafx.h"
namespace ztl
{
	namespace general_parser
	{
		struct GeneralTokenInfo
		{
			wstring name;
			wstring regex;
			bool    ignore;
			GeneralTokenInfo() = default;
			GeneralTokenInfo(const wstring& _name, const wstring& _regex, const bool _ignore)
				: name(_name), regex(_regex), ignore(_ignore)
			{
			}
		};
		vector<TokenInfo> Parse(const wstring& fileName)
		{
		
			unordered_map<wstring, GeneralTokenInfo> infos;
			infos.insert({ L"CLOSE",GeneralTokenInfo(L"CLOSE", L"\}", false) });
			infos.insert({ L"RULE",GeneralTokenInfo(L"RULE", L"rule", false) });
			infos.insert({ L"CLASS",GeneralTokenInfo(L"CLASS", L"class", false) });
			infos.insert({ L"TOKEN",GeneralTokenInfo(L"TOKEN", L"token", false) });
			infos.insert({ L"ENUM",GeneralTokenInfo(L"ENUM", L"enum", false) });
			infos.insert({ L"DISCARDTOKEN",GeneralTokenInfo(L"DISCARDTOKEN", L"ignoretoken", false) });
			infos.insert({ L"WITH",GeneralTokenInfo(L"WITH", L"with", false) });
			infos.insert({ L"AS",GeneralTokenInfo(L"AS", L"as", false) });
			infos.insert({ L"SPACE",GeneralTokenInfo(L"SPACE", L"\s+", true) });
			infos.insert({ L"OPEN",GeneralTokenInfo(L"OPEN", L"\{", false) });
			infos.insert({ L"SEMICOLON",GeneralTokenInfo(L"SEMICOLON", L";", false) });
			infos.insert({ L"ASSIGN",GeneralTokenInfo(L"ASSIGN", L"=", false) });
			infos.insert({ L"COLON",GeneralTokenInfo(L"COLON", L":", false) });
			infos.insert({ L"COMMA",GeneralTokenInfo(L"COMMA", L",", false) });
			infos.insert({ L"DOT",GeneralTokenInfo(L"DOT", L"\.", false) });
			infos.insert({ L"USING",GeneralTokenInfo(L"USING", L"!", false) });
			infos.insert({ L"OR",GeneralTokenInfo(L"OR", L"\|", false) });
			infos.insert({ L"OPTOPEN",GeneralTokenInfo(L"OPTOPEN", L"\[", false) });
			infos.insert({ L"STRING",GeneralTokenInfo(L"STRING", L"\\\"(\\\\\"|[^\"])*\"", false) });
			infos.insert({ L"OPTCLOSE",GeneralTokenInfo(L"OPTCLOSE", L"\]", false) });
			infos.insert({ L"PREOPEN",GeneralTokenInfo(L"PREOPEN", L"\(", false) });
			infos.insert({ L"PRECLOSE",GeneralTokenInfo(L"PRECLOSE", L"\)", false) });
			infos.insert({ L"NAME",GeneralTokenInfo(L"NAME", L"[a-zA-Z_]\w*", false) });
			infos.insert({ L"FINISH",GeneralTokenInfo(L"FINISH", L"<\$>", false) });
			infos.insert({ L"LINENOTE",GeneralTokenInfo(L"LINENOTE", L"(//[^\n]*\n)", true) });
			infos.insert({ L"BLOCKNOTE",GeneralTokenInfo(L"BLOCKNOTE", L"/*.*?*/ //", true) });

			wifstream input(fileName);
			if(!input.is_open())
			{
				throw ztl_exception(L"error:file" + fileName + L"not open!");
			}
			vector<TokenInfo> stream;
			std::wstring content((std::istreambuf_iterator<wchar_t>(input)),
				std::istreambuf_iterator<wchar_t>());
			auto pattern = accumulate(infos.begin(), infos.end(), wstring(),
				[](const wstring& sum, const pair<const wstring, GeneralTokenInfo>& token)
			{
				return sum + L"(?<" + token.second.name + L">" + token.second.regex + L")|";
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
				if(!infos[name].ignore)
				{
					stream.emplace_back(name, groupIter.second.content, groupIter.second.position, groupIter.second.length);
				}
			}
			return stream;
		}
	}
	}
}