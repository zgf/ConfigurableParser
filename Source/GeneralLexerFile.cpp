#pragma once
#include "Include/stdafx.h"
#include "Include/GeneralTableDefine.h"
#include "Include/SymbolManager.h"
#include "../Lib/Regex/ztl_regex_interpretor.h"
#include "../Lib/ZTL/ztl_generator.hpp"
#include "Include/GeneralFile.h"
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
			GeneralTokenInfo(const wstring& _name, const wstring& _regex, GeneralTokenDefine::TokenOptional _ignore)
				: name(_name), regex(_regex)
			{
				ignore = (_ignore == GeneralTokenDefine::TokenOptional::True) ? true : false;
			}
		};
	
		wstring CreateInitialStatements(unordered_map<wstring, GeneralTokenInfo>& infos)
		{
			auto initialTemplate = 
				LR"(infos.insert({L"$<Name>",GeneralTokenInfo(L"$<Name>", L"$<Regex>", $<Ignore>)});
				)";
			return accumulate(infos.begin(), infos.end(), wstring(), [&initialTemplate](const wstring&sum, const pair<const wstring, GeneralTokenInfo>& iter)
			{
				generator::MarcoGenerator generator(initialTemplate, { L"$<Name>",L"$<Regex>",L"$<Ignore>" });
				auto text = generator.GenerateText({ iter.second.name,iter.second.regex,iter.second.ignore==true?L"true":L"false" }).GetMacroResult();
				return sum + text;
			});
		}
		unordered_map<wstring, GeneralTokenInfo> GetTokenInfoList(SymbolManager* manager)
		{
			unordered_map<wstring, GeneralTokenInfo>tokenInfos;
			auto table = manager->GetTable();
			for(auto&& iter : table->tokens)
			{
				tokenInfos.insert({ iter->name,GeneralTokenInfo(iter->name, iter->regex,iter->ignore) });
			}
			return tokenInfos;
		}
		wstring CreateLexerFileBody(SymbolManager* manager)
		{
			auto infos = GetTokenInfoList(manager);
			wstring templateString = LR"(
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
				$<InitialBody>
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
			)";
			auto initialBody = CreateInitialStatements(infos);
			generator::MarcoGenerator generator(templateString, { L"$<InitialBody>" });
			return generator.GenerateText({ initialBody }).GetMacroResult();
		}
		void CreateLexerFile(const wstring& fileName, SymbolManager* manager)
		{
			auto body = CreateLexerFileBody(manager);
			NodeDefineFileAddNamespace(body, manager);
			CreateFile(fileName, body);
		}


	
		wstring CreateRegexString(const unordered_map<wstring, GeneralTokenInfo>& infos)
		{
			auto result = accumulate(infos.begin(), infos.end(), wstring(), 
				[](const wstring& sum, const pair<const wstring, GeneralTokenInfo>& token)
			{
				return sum + L"(?<" + token.second.name + L">" + token.second.regex + L")|";
			});
			result.pop_back();
			return result;
		}

	



		//vector<TokenInfo> Parse(const wstring& fileName)
		//{
		//	struct GeneralTokenInfo
		//	{
		//		wstring name;
		//		wstring regex;
		//		bool    ignore;
		//		GeneralTokenInfo() = default;
		//		GeneralTokenInfo(const wstring& _name, const wstring& _regex, const bool _ignore)
		//			: name(_name), regex(_regex), ignore(_ignore)
		//		{
		//		}
		//	};
		//	unordered_map<wstring, GeneralTokenInfo> infos/*=<initialLexer>*/;
		//	wifstream input(fileName);
		//	if(!input.is_open())
		//	{
		//		throw ztl_exception(L"error:file" + fileName + L"not open!");
		//	}
		//	vector<TokenInfo> stream;
		//	std::wstring content((std::istreambuf_iterator<wchar_t>(input)),
		//		std::istreambuf_iterator<wchar_t>());
		//	auto pattern = accumulate(infos.begin(), infos.end(), wstring(), [](const wstring& sum, const pair<const wstring, GeneralTokenInfo>& token)
		//	{
		//		return sum + L"(?<" + token.second.name + L">" + token.second.regex + L")|";
		//	});
		//	pattern.pop_back();
		//	RegexInterpretor interpretor(pattern);
		//	auto result = interpretor.Matches(content);
		//	for(auto&& iter : result)
		//	{
		//		assert(iter.group.size() == 1);
		//		auto groupIter = *iter.group.begin();
		//		auto name = groupIter.first;
		//		assert(infos.find(name) != infos.end());
		//		if(!infos[name].ignore)
		//		{
		//			stream.emplace_back(name, groupIter.second.content, groupIter.second.position, groupIter.second.length);
		//		}
		//	}
		//	return stream;
		//}
	}
}