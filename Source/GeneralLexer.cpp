#pragma once
#include "Include/stdafx.h"
#include "Include/GeneralTableDefine.h"
#include "../Lib/Regex/ztl_regex_interpretor.h"

namespace ztl
{
	namespace general_parser
	{
		//单词要加单词边界
		wstring DealWithNewLine(const wstring& value)
		{
			auto result = value;
			ztl::algorithm::replace_all_distinct<wstring>(result, LR"(\n)", L"\n");
			return result;
		}
		wstring DealWithWord(const wstring& value)
		{
			static RegexInterpretor interepter(LR"(^[a-zA-Z_]\w*$)");
			if(interepter.IsMatch(value))
			{
				return LR"(\b)" + value + LR"(\b)";
			}
			else
			{
				return DealWithNewLine(value);
			}
		}
		//'\\'n 重写成'\n'
		
		vector<shared_ptr<TokenInfo>>	 ParseToken(const wstring& fileName)
		{
			unordered_map<wstring, GeneralTokenDefine::TokenOptional> infos;
			infos.insert({ L"CLASS",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"STRUCT",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"ENUM",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"TOKEN",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"DISCARDTOKEN",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"RULE",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"AS",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"WITH",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"OPEN",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"CLOSE",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"SEMICOLON",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"COLON",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"COMMA",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"DOT",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"ASSIGN",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"USING",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"OR",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"OPTOPEN",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"OPTCLOSE",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"PREOPEN",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"PRECLOSE",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"NAME",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"STRING",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"FINISH",GeneralTokenDefine::TokenOptional::False });
			infos.insert({ L"SPACE",GeneralTokenDefine::TokenOptional::True });
			infos.insert({ L"LINENOTE",GeneralTokenDefine::TokenOptional::True });
			infos.insert({ L"BLOCKNOTE",GeneralTokenDefine::TokenOptional::True });
			vector<GeneralTokenDefine> tokens;
			tokens.emplace_back(GeneralTokenDefine(L"CLASS", LR"(class)", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"STRUCT", LR"(struct)", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"ENUM", LR"(enum)", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"TOKEN", LR"(token)", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"DISCARDTOKEN", LR"(ignoretoken)", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"RULE", LR"(rule)", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"AS", LR"(as)", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"WITH", LR"(with)", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"OPEN", LR"(\{)", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"CLOSE", LR"(\})", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"SEMICOLON", LR"(;)", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"COLON", LR"(:)", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"COMMA", LR"(,)", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"DOT", LR"(\.)", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"ASSIGN", LR"(=)", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"USING", LR"(!)", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"OR", LR"(\|)", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"OPTOPEN", LR"(\[)", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"OPTCLOSE", LR"(\])", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"PREOPEN", LR"(\()", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"PRECLOSE", LR"(\))", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"NAME", LR"([a-zA-Z_]\w*)", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"STRING",  LR"("(\\"|[^"])*")", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"FINISH", LR"(<\$>)", GeneralTokenDefine::TokenOptional::False));
			tokens.emplace_back(GeneralTokenDefine(L"SPACE", LR"(\s+)", GeneralTokenDefine::TokenOptional::True));
			tokens.emplace_back(GeneralTokenDefine(L"LINENOTE", LR"(//[^\n]*\n)", GeneralTokenDefine::TokenOptional::True));
			tokens.emplace_back(GeneralTokenDefine(L"BLOCKNOTE", LR"(/\*.*?\*/)", GeneralTokenDefine::TokenOptional::True));
		
			wifstream input(fileName);
			if(!input.is_open())
			{
				throw ztl_exception(L"error:file" + fileName + L"not open!");
			}
			vector<shared_ptr<TokenInfo>>	 stream;
			std::wstring content((std::istreambuf_iterator<wchar_t>(input)),
				std::istreambuf_iterator<wchar_t>());
			auto pattern = accumulate(tokens.begin(), tokens.end(), wstring(), [](const wstring& sum, GeneralTokenDefine& token)
			{
				return sum + L"(<" + token.name + L">" + DealWithWord(token.regex) + L")|";
			});
			pattern.pop_back();
			RegexInterpretor interpretor(pattern);
			auto result = interpretor.Matches(content);
			for(auto&& iter : result)
			{
				assert(iter.group.size() == 1);
				auto groupIter = *iter.group.begin();
				auto tag = groupIter.first;
				assert(infos.find(tag) != infos.end());
				if(infos[tag] != GeneralTokenDefine::TokenOptional::True)
				{
					stream.emplace_back(make_shared<TokenInfo>(groupIter.second.content, tag, groupIter.second.position, groupIter.second.length));
				}
			}
			//添加end字符
			stream.emplace_back(make_shared<TokenInfo>(L"<$>",L"FINISH",-1,4));
			return stream;
		}
	}
}