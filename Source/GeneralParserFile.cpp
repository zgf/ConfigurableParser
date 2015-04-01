#include "Include\stdafx.h"
#include "Include\GeneralParser.h"
#include "Include\GeneralParserFile.h"
#include "Include\SymbolManager.h"
namespace ztl
{
	namespace general_parser
	{
		GeneralParserFile::GeneralParserFile(const wstring & fileName, const shared_ptr<GeneralTableDefine>& _tableDefine) :generalParser(make_shared<GeneralParser>(fileName, _tableDefine))
		{
		}
		shared_ptr<GeneralTableDefine> GeneralParserFile::GetGenerateParserTableDefine()
		{
			generalParser->BuildParser();
			generalParser->GenerateIsomorphismParserTree();
			auto parserResult = generalParser->GeneralHeterogeneousParserTree();
			return shared_ptr<GeneralTableDefine>(*(std::shared_ptr<GeneralTableDefine>*)(&parserResult));
		}
		wstring GeneralParserFile::GenerateModulesWithNamespace(const wstring& content, const vector<wstring>& namespaces)
		{
			auto result = accumulate(namespaces.rbegin(), namespaces.rend(), content, [](const wstring& sum, const wstring& value)
			{
				const wstring templateString =
					LR"(
							namespace $<Name>
							{
								$<Body>
							}
						 )";
				generator::MarcoGenerator generator(templateString, { L"$<Name>",L"$<Body>" });

				return generator.GenerateText({ value ,sum }).GetMacroResult();
			});
			return result;
		}
		wstring GetPreDefineInclude()
		{
			return LR"(
				#include <memory>
				#include <string>
				#include <vector>
				#include <unordered_map>
				#include <iostream>
				#include <assert.h>
				using std::pair;
				using std::wstring;
				using std::shared_ptr;
				using std::vector;
				using std::unordered_map;
				using std::wifstream;
				using std::make_shared;
				)";
		}
		wstring GeneralParserFile::GenerateModulesInclude(const vector<wstring>& includes)
		{
			const wstring templateString = L"#include \"$<FileName>\"\n";
			return accumulate(includes.begin(), includes.end(), GetPreDefineInclude(), [&templateString](const wstring& sum, const wstring& value)
			{
				generator::MarcoGenerator generator(templateString, { L"$<FileName>" });
				return sum + generator.GenerateText({ value }).GetMacroResult() + L"\n";
			});
		}
		wstring GeneralParserFile::GetGenerateModuleBody(GeneralTableDefine* table, SymbolManager*manager)
		{
			auto nodeDefineModule = GetNodeDefineModule(table, manager);
			auto reflectModule = GetReflectionModule(manager);
			auto serialzeCoreModule = SerializeEBNFCoreModule(table);
			return nodeDefineModule + reflectModule + serialzeCoreModule;
		}
		void GeneralParserFile::GenerateSpecialParserFile()
		{
			auto table = GetGenerateParserTableDefine();
			SymbolManager manager(table);
			ValidateGeneratorCoreSemantic(&manager);
			auto modules = GetGenerateModuleBody(table.get(), &manager);

			auto namespaces = GetGenerateNameSapce(&manager);
			auto body = GenerateModulesWithNamespace(modules, namespaces);

			auto includes = GetGenerateInclude(&manager);
			auto include = GenerateModulesInclude(includes);

			auto content = include + body;
			auto filename = GetGenerateFileName(&manager);
			CreateFile(filename, content);
		}

		wstring GeneralParserFile::GetGenerateFileName(SymbolManager* manager)
		{
			assert(generalParser != nullptr);
			auto result = manager->GetCacheValueByProperty(L"filename");
			assert(result.size() <= 1);
			return result.empty() ? wstring(L"GenerateParserFile.hpp") : result[0] + L".hpp";
		}
		vector<wstring> GeneralParserFile::GetGenerateNameSapce(SymbolManager* manager)
		{
			assert(generalParser != nullptr);
			auto result = manager->GetCacheValueByProperty(L"namespace");
			return result;
		}
		wstring GeneralParserFile::GetGenerateClassPrefix(SymbolManager* manager)
		{
			assert(generalParser != nullptr);
			auto result = manager->GetCacheValueByProperty(L"classprefix");
			assert(result.size() <= 1);
			return result.empty() ? wstring() : result[0];
		}
		vector<wstring> GeneralParserFile::GetGenerateInclude(SymbolManager* manager)
		{
			assert(generalParser != nullptr);
			auto result = manager->GetCacheValueByProperty(L"include");
			return result;
		}
		void GeneralParserFile::CreateFile(const wstring& fileName, const wstring& content)
		{
			wofstream output(fileName);
			output.write(content.c_str(), content.size());

		}
	}
}