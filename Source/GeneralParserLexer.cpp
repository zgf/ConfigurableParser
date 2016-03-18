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

		wstring GetFileContent(wstring fileName)
		{
			wifstream input(fileName);
			if (!input.is_open())
			{
				throw ztl_exception(L"error:file" + fileName + L"not open!");
			}
			
			std::wstring content((std::istreambuf_iterator<wchar_t>(input)),
				std::istreambuf_iterator<wchar_t>());
			return content;
		}
		wstring ReplaceCaptureToNonCapture(wstring& regex)
		{
			wstring temp;
			temp = ztl::algorithm::replace_all_distinct<wstring>(regex, L"(", L"(?:");
			wstring old = L"\\(?:";
			temp = ztl::algorithm::replace_all_distinct<wstring>(temp, old, L"\\(");
			return temp;
		}
		wstring AccumulateTokensToPattern(vector<shared_ptr<GeneralTokenDefine>> &tokens)
		{
			auto&& wpattern = accumulate(tokens.begin(), tokens.end(),
				wstring(), [](const wstring& sum, const shared_ptr<GeneralTokenDefine>& token)
			{
				if (token->name == L"STRING")
				{
					int a = 0;
				}
				auto regex = DealWithWord(token->regex);
				return sum + L"(" + ReplaceCaptureToNonCapture(regex) + L")|";
			});
			wpattern.pop_back();
			return wpattern;
		}

		vector<shared_ptr<TokenInfo>> TransformSourceToTokens(const wstring& wpattern, const wstring &content,
			const vector<shared_ptr<GeneralTokenDefine>> &tokens,const unordered_map<wstring, GeneralTokenDefine::TokenOptional>& infos)
		{
			/*{
				wstring wpattern = LR"((\+\+)|(\+)|(a))";
				wstring text = LR"(++a+a++++)";
				auto&& regex = boost::xpressive::wsregex::compile(wpattern, boost::xpressive::regex_constants::syntax_option_type::optimize);
				boost::xpressive::wsmatch what;
				boost::xpressive::regex_search(text, what, regex);
				auto a = what[0];
				auto b = what[1];
				auto c = what[2];
				auto d = what[3];
				for (auto&&iter = what.begin();iter != what.end();++iter)
				{

				}
			}*/
			vector<shared_ptr<TokenInfo>> stream;
			auto&& regex = boost::xpressive::wsregex::compile(wpattern, boost::xpressive::regex_constants::syntax_option_type::optimize);
			auto textBegin = content.begin();
			while (textBegin < content.end())
			{
				boost::xpressive::wsmatch match;
				boost::xpressive::regex_search(textBegin, content.end(), match, regex);
				/*for (auto&&iter : match)
				{
					wcout << iter;
				}*/
				auto&& findIter = std::find_if(match.begin() + 1, match.end(), [](auto&& iter) {return iter.matched;});
				auto&& index = findIter - match.begin() - 1;
				auto&&name = tokens[index]->name;
				auto&& matched = *findIter;
				auto&& str = findIter->str();

				if (infos.at(name) != GeneralTokenDefine::TokenOptional::True)
				{
					stream.emplace_back(make_shared<TokenInfo>(str, name, matched.first - content.begin(), matched.length()));
				}
				textBegin = matched.second;
			}

		/*	for (boost::xpressive::wsregex_iterator current(content.begin(), content.end(), regex), end;current != end;++current)
			{
				auto&& match = *current;
				auto&& findIter = std::find_if(match.begin()+1, match.end(), [](auto&& iter) {return iter.matched;});
				auto findIndex = findIter - match.begin();
				auto&& index = findIndex - 1;
				auto&&name = tokens[index]->name;
				auto&& matched = *findIter;
				auto&& str = findIter->str();

				if (infos.at(name) != GeneralTokenDefine::TokenOptional::True)
				{
					stream.emplace_back(make_shared<TokenInfo>(str, name, matched.first - content.begin(), matched.length()));
				}
			}*/
			stream.emplace_back(make_shared<TokenInfo>(L"<$>", L"FINISH", content.size(), 4));
			return stream;
		}
		unordered_map<wstring, GeneralTokenDefine::TokenOptional> CacheNameToIgnoreMap(const vector<shared_ptr<GeneralTokenDefine>> &tokens)
		{
			unordered_map<wstring, GeneralTokenDefine::TokenOptional> infos;
			for_each(tokens.begin(), tokens.end(), [&infos](const shared_ptr<GeneralTokenDefine>& token)
			{
				infos.insert({ token->name, token->ignore });
			});
			return infos;
		}
		vector<int> GetLineNumberMap(const wstring& content)
		{
			vector<int> result;
			auto beginIter = content.cbegin();
			auto endIter = content.cend();
			for (auto startIter = content.cbegin();startIter < endIter;)
			{
				startIter = find(startIter, content.cend(), L'\n');
				auto index = startIter - beginIter;
				result.emplace_back(index);
				if (startIter < endIter)
				{
					++startIter;
				}
			}
			result.emplace_back(std::numeric_limits<int>::max());
			return result;
		}

		int CalculateIndexLineNumber(int index, const vector<int>& lineMap)
		{
			assert(!lineMap.empty());
			assert(index >= 0);
			auto&& result = ztl::algorithm::binary_search(lineMap, index);
			return result.second;
		}

		void AddLineNumberOnStream(const wstring& content,vector<shared_ptr<TokenInfo>>& stream)
		{
			auto&& lineMap = GetLineNumberMap(content);
			for (auto&& iter : stream)
			{
				auto&& lineNumber = CalculateIndexLineNumber(iter->position, lineMap);
				iter->SetLineNumber(lineNumber);
			}
		}

		vector<shared_ptr<TokenInfo>> GeneralParserBase::ParseToken(const wstring& fileName)
		{
			auto&& table = manager->GetTable();
			auto&& tokens = table->tokens;
			assert(table != nullptr);
			auto&& infos = CacheNameToIgnoreMap(tokens);
			auto&& content = GetFileContent(fileName);
			auto&& wpattern = AccumulateTokensToPattern(tokens);
			auto stream = TransformSourceToTokens(wpattern, content, tokens, infos);
			AddLineNumberOnStream(content, stream);
			return stream;
		}
	}
}