#include "Include\stdafx.h"
#include "Include\GeneralParserBase.h"
#include "Include\SymbolManager.h"
#include "../Lib/Regex/ztl_regex_interpretor.h"
namespace ztl
{
	namespace general_parser
	{
		//'\\'n 重写成'\n'

		static wstring DealWithNewLine(const wstring& value)
		{
			auto result = value;
			ztl::algorithm::replace_all_distinct<wstring>(result, LR"(\n)", L"\n");
			return result;
		}
		//单词要加单词边界

		static wstring DealWithWord(const wstring& value)
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
		vector<shared_ptr<TokenInfo>> GeneralParserBase::ParseToken(const wstring& fileName)
		{
			assert(manager->GetTable() != nullptr);
			unordered_map<wstring, GeneralTokenDefine::TokenOptional> infos;
			for_each(manager->GetTable()->tokens.begin(), manager->GetTable()->tokens.end(), [&infos](const shared_ptr<GeneralTokenDefine>& token)
			{
				infos.insert({ token->name, token->ignore });
			});

			wifstream input(fileName);
			if(!input.is_open())
			{
				throw ztl_exception(L"error:file" + fileName + L"not open!");
			}
			vector<shared_ptr<TokenInfo>> stream;
			std::wstring content((std::istreambuf_iterator<wchar_t>(input)),
				std::istreambuf_iterator<wchar_t>());
			auto pattern = accumulate(manager->GetTable()->tokens.begin(), manager->GetTable()->tokens.end(),
				wstring(), [](const wstring& sum, const shared_ptr<GeneralTokenDefine>& token)
			{
				return sum + L"(<" + token->name + L">" + DealWithWord(token->regex) + L")|";
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
			stream.emplace_back(make_shared<TokenInfo>(L"<$>", L"FINISH", -1, 4));
			return stream;
		}
	}
}