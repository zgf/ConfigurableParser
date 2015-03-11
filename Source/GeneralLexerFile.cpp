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
		wstring CreateInitialStatements(SymbolManager* manager)
		{
			auto infos = manager->GetTable()->tokens;
			auto initialTemplate = L"infos.insert({L\"$<Name>\",GeneralTokenDefine(L\"$<Name>\", LR\"($<Regex>)\", $<Ignore>)});\n";
			return accumulate(infos.begin(), infos.end(), wstring(), [&initialTemplate](const wstring&sum,const shared_ptr<GeneralTokenDefine>& iter)
			{
				generator::MarcoGenerator generator(initialTemplate, { L"$<Name>",L"$<Regex>",L"$<Ignore>" });
				auto text = generator
					.GenerateText({ iter->name,iter->regex,
					iter->ignore== GeneralTokenDefine::TokenOptional::True ?
					L"GeneralTokenDefine::TokenOptional::True" : 
					L"GeneralTokenDefine::TokenOptional::False" })
					.GetMacroResult();
				return sum + text;
			});
		}
		
		wstring CreateLexerFileBody(SymbolManager* manager)
		{
			wstring templateString = LR"(
			vector<TokenInfo> Parse(const wstring& fileName)
			{
				unordered_map<wstring, GeneralTokenDefine> infos;
				$<InitialBody>
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
					if(infos[name].ignore != GeneralTokenDefine::TokenOptional::True)
					{
						stream.emplace_back(name, groupIter.second.content, groupIter.second.position, groupIter.second.length);
					}
				}
				return stream;
			}
			)";
			auto initialBody = CreateInitialStatements(manager);
			generator::MarcoGenerator generator(templateString, { L"$<InitialBody>" });
			return generator.GenerateText({ initialBody }).GetMacroResult();
		}
		void CreateLexerFile(const wstring& fileName, SymbolManager* manager)
		{
			auto body = CreateLexerFileBody(manager);
			NodeDefineFileAddNamespace(body, manager);
			auto include = GetNodeDefineFileInclude(manager);
			body = include + body;
			CreateFile(fileName, body);
		}


	
		wstring CreateRegexString(const unordered_map<wstring, GeneralTokenDefine>& infos)
		{
			auto result = accumulate(infos.begin(), infos.end(), wstring(), 
				[](const wstring& sum, const pair<const wstring, GeneralTokenDefine>& token)
			{
				return sum + L"(?<" + token.second.name + L">" + token.second.regex + L")|";
			});
			result.pop_back();
			return result;
		}

	



		//vector<TokenInfo> Parse(const wstring& fileName)
		//{
		//	unordered_map<wstring, GeneralTokenDefine> infos/*=<initialLexer>*/;
		//	wifstream input(fileName);
		//	if(!input.is_open())
		//	{
		//		throw ztl_exception(L"error:file" + fileName + L"not open!");
		//	}
		//	vector<TokenInfo> stream;
		//	std::wstring content((std::istreambuf_iterator<wchar_t>(input)),
		//		std::istreambuf_iterator<wchar_t>());
		//	auto pattern = accumulate(infos.begin(), infos.end(), wstring(), [](const wstring& sum, const pair<const wstring, GeneralTokenDefine>& token)
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
		//		if(infos[name].ignore != GeneralTokenDefine::TokenOptional::True)
		//		{
		//			stream.emplace_back(name, groupIter.second.content, groupIter.second.position, groupIter.second.length);
		//		}
		//	}
		//	return stream;
		//}
	}
}