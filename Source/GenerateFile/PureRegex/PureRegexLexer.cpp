#include "Include/stdafx.h"
#include "..\..\Include\GeneralTableDefine.h"

namespace ztl
{
	namespace pure_regex
	{
		vector<shared_ptr<ztl::general_parser::TokenInfo>> PureRegexParseToken(const wstring& pattern)
		{
			using lambda_type = shared_ptr<ztl::general_parser::TokenInfo>(*)(const wstring&, size_t&);
			static unordered_map<wstring, lambda_type> actionMap =
			{
				{
					L"[",
					[](const wstring& pattern,size_t& index)
					{
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,1),L"CHASETBEGIN",index,1);
					}
				},
				{
					L"[^",
					[](const wstring& pattern,size_t& index)
					{
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,2),L"CHASETREVERSEBEGIN",index,2);
					}
				},
				{
					L"]",
					[](const wstring& pattern,size_t& index)
					{
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,1),L"CHASETEND",index,1);
					}
				},
				{
					L"(",
					[](const wstring& pattern,size_t& index)
					{
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,1),L"PARENTOPEN",index,1);
					}
				},
				{
					L")",
					[](const wstring& pattern,size_t& index)
					{
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,1),L"PARENTCLOSE",index,1);
					}
				},
				{
					L"*",
					[](const wstring& pattern,size_t& index)
					{
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,1),L"KLEENEGREEDY",index,1);
					}
				},
				{
					L"+",
					[](const wstring& pattern,size_t& index)
					{
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,1),L"POSITIVEKLEENEGREEDY",index,1);
					}
				},
				{
					L"?",
					[](const wstring& pattern,size_t& index)
					{
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,1),L"CHOCIEKLEENEGREEDY",index,1);
					}
				},
				{
					L"|",
					[](const wstring& pattern,size_t& index)
					{
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,1),L"CHOCIE",index,1);
					}
				},
				{
					L"-",
					[](const wstring& pattern,size_t& index)
					{
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,1),L"CONPONENT",index,1);
					}
				},
				{
					L"\\",
					[](const wstring& pattern,size_t& index)
					{
						++index;
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,1),L"NORMAL",index,1);
					}
				},
				{
					L".",
					[](const wstring& pattern,size_t& index)
					{
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,1),L"MatchAll",index,1);
					}
				},
				{
					L"*?",
					[](const wstring& pattern,size_t& index)
					{
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,2),L"KLEENE",index,2);
					}
				},
				{
					L"+?",
					[](const wstring& pattern,size_t& index)
					{
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,2),L"POSITIVEKLEENE",index,2);
					}
				},
				{
					L"??",
					[](const wstring& pattern,size_t& index)
					{
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,2),L"CHOCIEKLEENE",index,2);
					}
				},
				{
					L"\\W",
					[](const wstring& pattern,size_t& index)
					{
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,2),L"TROPEW",index,2);
					}
				},
				{
					L"\\w",
					[](const wstring& pattern,size_t& index)
					{
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,2),L"TROPEw",index,2);
					}
				},
				{
					L"\\S",
					[](const wstring& pattern,size_t& index)
					{
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,2),L"TROPES",index,2);
					}
				},
				{
					L"\\s",
					[](const wstring& pattern,size_t& index)
					{
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,2),L"TROPEs",index,2);
					}
				},
				{
					L"\\D",
					[](const wstring& pattern,size_t& index)
					{
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,2),L"TROPED",index,2);
					}
				},
				{
					L"\\d",
					[](const wstring& pattern,size_t& index)
					{
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,2),L"TROPEd",index,2);
					}
				},
				{
					L"",
					[](const wstring& pattern,size_t& index)
					{
						return make_shared<ztl::general_parser::TokenInfo>(pattern.substr(index,1),L"NORMAL",index,1);
					}
				},
			};
			vector<shared_ptr<ztl::general_parser::TokenInfo>> result;
			for(size_t i = 0; i < pattern.size();)
			{
				for(auto k = 2; k >= 1; k--)
				{
					auto key = pattern.substr(i, k);
					auto findIter = actionMap.find(key);
					if(findIter != actionMap.end())
					{
						result.emplace_back(findIter->second(pattern, i));
						i += k;
						break;
					}
					else if(k == 1 && findIter == actionMap.end())
					{
						result.emplace_back(actionMap[L""](pattern, i));
						++i;
					}
				}
			}
			return result;
		}

	}
}