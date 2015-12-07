#include "Include\stdafx.h"
#include "Include\GeneralParserBase.h"
#include "Include\SymbolManager.h"
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
			static boost::xpressive::wsregex regex = boost::xpressive::wsregex::compile(LR"(^[a-zA-Z_]\w*$)");
			if (boost::xpressive::regex_match(value, regex))
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
			auto&& table = manager->GetTable();
			auto&& tokens = table->tokens;
			assert(table != nullptr);
			unordered_map<wstring, GeneralTokenDefine::TokenOptional> infos;
			for_each(tokens.begin(), tokens.end(), [&infos](const shared_ptr<GeneralTokenDefine>& token)
			{
				infos.insert({ token->name, token->ignore });
			});

			wifstream input(fileName);
			if (!input.is_open())
			{
				throw ztl_exception(L"error:file" + fileName + L"not open!");
			}
			vector<shared_ptr<TokenInfo>> stream;
			std::wstring content((std::istreambuf_iterator<wchar_t>(input)),
				std::istreambuf_iterator<wchar_t>());

			auto wpattern = accumulate(tokens.begin(), tokens.end(),
				wstring(), [](const wstring& sum, const shared_ptr<GeneralTokenDefine>& token)
			{
				return sum + L"(?P<" + token->name + L">" + DealWithWord(token->regex) + L")|";
			});
			wpattern.pop_back();
			auto regex = boost::xpressive::wsregex::compile(wpattern, boost::xpressive::regex_constants::syntax_option_type::optimize);
			boost::xpressive::wsregex_iterator end;
			for (boost::xpressive::wsregex_iterator current(content.begin(), content.end(), regex);current != end;++current)
			{
				auto&& match = *current;
				auto&& findIter = std::find_if(tokens.begin(), tokens.end(), [&match](auto&& iter) {return match[iter->name].matched;});
				assert(findIter != tokens.end());
				auto&& name = (*findIter)->name;
				auto&& matched = match[name];
				auto&& str = matched.str();

				if (infos[name] != GeneralTokenDefine::TokenOptional::True)
				{
					stream.emplace_back(make_shared<TokenInfo>(str, name, matched.first - content.begin(), matched.length()));
				}
			}

			stream.emplace_back(make_shared<TokenInfo>(L"<$>", L"FINISH", -1, 4));
			return stream;
		}
	}
}