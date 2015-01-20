#pragma once
#include "stdafx.h"
#include "SymbolManager.h"
#include "../../Lib/Regex/ztl_regex_interpretor.h"
#include "../../Lib/ZTL/ztl_exception.h"
namespace ztl
{
	namespace general_parser
	{
		namespace lexer
		{
		
			using contract::ztl_exception;
			using contract::ThrowException;
			class GeneralLexer
			{
				unordered_map<wstring, LexTokenDefine>							lexDefineMap;
				shared_ptr<SymbolManager>										manager;
				vector<FileInfo>												fileInfoList;
				wstring															regex;
				unordered_map<wstring, vector<TokenInfo>>						reusltList;
			public:
				GeneralLexer() = delete;
				GeneralLexer(const vector<wstring>& fileNameList,const unordered_map<wstring, LexTokenDefine>& _lexDefineMap) :lexDefineMap(_lexDefineMap)
				{
					this->fileInfoList = SetFileInfoList(fileNameList);
					this->regex = GetRealRegexWString();
				}
				~GeneralLexer() = default;
			public:
				vector<FileInfo> SetFileInfoList(const vector<wstring>& fileNameList)
				{
					vector<FileInfo>	result;
					wifstream			inputHandle;
					for(auto& name : fileNameList)
					{
						inputHandle.open(name);
						ThrowException(inputHandle.is_open(), ztl_exception(L"Can't find File: " + name));

						FileInfo    info;
						info.name = name;
						info.content = GetFileContent(inputHandle);
						result.emplace_back(move(info));
					}
					return result;
				}

				wstring GetFileContent(wifstream& inputHandle)
				{
					istreambuf_iterator<wchar_t> beg(inputHandle);
					istreambuf_iterator<wchar_t> end;
					wstring						 content(beg, end);
					inputHandle.close();
					return content;
				}

				wstring GetRealRegexWString()
				{
					vector<wstring> regex_list;
					regex_list.reserve(lexDefineMap.size());

					for(auto& element : lexDefineMap)
					{
						//被忽视的不加入捕获组
						if (!element.second.ignore)
						{
							regex_list.emplace_back(L"(<" + element.first + L">" + element.second.regex + L")");
						}
					}

					ThrowException(!regex_list.empty(), ztl_exception(L"Can't find file!"));
					return accumulate(regex_list.begin() + 1, regex_list.end(), *regex_list.begin(), [](const wstring value, const wstring& first)
					{
						return value + L"|" + first;
					});
				}
			public:	/*公开的API*/
				
				vector<TokenInfo> Parse(const wstring& content, RegexInterpretor& interpretor)
				{
					vector<TokenInfo> tokens;

					auto  result = interpretor.Matches(content);

					for(auto&element : result)
					{
						assert(element.group.size() == 1);
						auto name = element.group.begin()->first;
						auto groupItem = element.group.begin()->second;
						TokenInfo info(manager->GetTagByString(name), groupItem.position, groupItem.length, groupItem.content);
						/*	if(!lexDefineMap[name].ignore)
							{*/
						//tokens.emplace_back(TokenInfo());
						//auto& info = tokens.back();
						
						//info.tag = ;//GeneralLexer::NameToEnumMap[name];
						//info.lex = groupItem.content;
						//info.position.index = groupItem.position;
						//info.position.length = groupItem.length;
						//}
					}
					return tokens;
				}

				void Parse()
				{
					RegexInterpretor interpretor(regex);
					for(auto& fileIter : this->fileInfoList)
					{
						auto parseResult = Parse(fileIter.content, interpretor);
						this->reusltList.insert({ fileIter.name, move(parseResult) });
					}
				}

				const unordered_map<wstring, vector<TokenInfo>>& GetReusltList() const
				{
					return reusltList;
				}
			};
		}
	}
}