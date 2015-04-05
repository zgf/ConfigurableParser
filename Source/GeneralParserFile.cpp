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
			LogGraphInfo(L"LogParserDefineInfo.txt", GetParser().GetMachine());

			auto result = generalParser->GenerateIsomorphismParserTree();
			assert(result.size() == 1);
			auto parserResult = GeneralHeterogeneousParserTree(*generalParser);
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
				#pragma once
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
				#include "$<RelativePath>GeneralTableDefine.h"
				#include "$<RelativePath>GeneralTreeNode.h"
				#include "$<RelativePath>GeneralParser.h"
				)";
		}

		wstring GeneralParserFile::GenerateImpModuleInclude(const wstring& filename, const vector<wstring>& includes, SymbolManager* manager)
		{
			const wstring endIncludeTemplate =
				LR"(#include ")" + filename + LR"(.h")" +
				LR"(
					#include "$<RelativePath>GeneralTableWriter.h"

				)";
			generator::MarcoGenerator generator(endIncludeTemplate, { L"$<RelativePath>" });
			auto endInclude = generator.GenerateText({ GetIncludeRelativePath(manager) }).GetMacroResult();
			return GenerateModuleInclude(endInclude, includes);
		}
		wstring GeneralParserFile::GenerateModuleInclude(const wstring& endInclude, const vector<wstring>& includes)
		{
			const wstring templateString = L"#include \"$<FileName>\"\n";
			return accumulate(includes.begin(), includes.end(), wstring(), [&templateString](const wstring& sum, const wstring& value)
			{
				generator::MarcoGenerator generator(templateString, { L"$<FileName>" });
				return sum + generator.GenerateText({ value }).GetMacroResult() + L"\n";
			}) + endInclude;
		}
		wstring	GeneralParserFile::GetIncludeRelativePath(SymbolManager*manager)
		{
			auto dirname = GetGenerateDirname(manager);
			if(dirname.empty())
			{
				return  LR"(..\Include\)";
			}
			else
			{
				return LR"(..\..\Include\)" ;
			}
		}
		wstring GeneralParserFile::GenerateHeadModuleInclude(SymbolManager* manager)
		{
			auto headTemplateString = GetPreDefineInclude();
			generator::MarcoGenerator generator(headTemplateString, { L"$<RelativePath>" });
			return generator.GenerateText({ GetIncludeRelativePath(manager) }).GetMacroResult();
		}
		wstring GeneralParserFile::GenerateHeadModuleContent(GeneralTableDefine* table, SymbolManager*manager, const vector<wstring>& namespaces)
		{
			auto headModule = GetGenerateHeadModuleBody(table, manager);
			headModule = GenerateModulesWithNamespace(headModule, namespaces);
			auto headInclude = GenerateHeadModuleInclude(manager);
			return headInclude + headModule;
		}
		wstring GeneralParserFile::GenerateImpModuleContent(GeneralTableDefine* table, SymbolManager*manager, const vector<wstring>& includes, const wstring filename, const vector<wstring>& namespaces)
		{
			auto impModule = GetGenerateImpModuleBody(table, manager);
			impModule = GenerateModulesWithNamespace(impModule, namespaces);
			auto impInclude = GenerateImpModuleInclude(filename, includes, manager);
			return impInclude + impModule;
		}
		wstring GeneralParserFile::GetGenerateHeadModuleBody(GeneralTableDefine* table, SymbolManager*manager)
		{
			auto nodeDefineModule = GetNodeDefineModule(table, manager);
			auto reflectModuleHead = GetReflectionModuleHead();
			auto serialCoreModuleHead = SerializeEBNFCoreModuleHead();
			return nodeDefineModule + serialCoreModuleHead + reflectModuleHead;
		}
		wstring GeneralParserFile::GetGenerateImpModuleBody(GeneralTableDefine* table, SymbolManager*manager)
		{
			auto reflectModule = GetReflectionModuleImp(manager);
			auto serialzeCoreModule = SerializeEBNFCoreModuleImp(table);
			return reflectModule + serialzeCoreModule;
		}

		void GeneralParserFile::GenerateSpecialParserFile()
		{
			auto table = GetGenerateParserTableDefine();
			SymbolManager manager(table);
			ValidateGeneratorCoreSemantic(&manager);

			auto namespaces = GetGenerateNameSapce(&manager);
			auto includes = GetGenerateInclude(&manager);
			auto filename = GetGenerateFileName(&manager);

			auto implContent = GenerateImpModuleContent(table.get(), &manager, includes, filename, namespaces);
			auto headContent = GenerateHeadModuleContent(table.get(), &manager, namespaces);
			auto path = GetGeneratePath(&manager);
			auto headFilename = path + filename + L".h";
			auto implFilename = path + filename + L".cpp";

			CreateFile(headFilename, headContent);
			CreateFile(implFilename, implContent);
		}

		wstring GeneralParserFile::GetGenerateFileName(SymbolManager* manager)
		{
			return GetGenerateUniqueProperty(manager, L"filename", L"GenerateParserFile");
		}

		wstring GeneralParserFile::GetGenerateClassPrefix(SymbolManager* manager)
		{
			return GetGenerateUniqueProperty(manager, L"classprefix");
		}
		wstring GeneralParserFile::GetGenerateDirname(SymbolManager * manager)
		{
			auto dirname = GetGenerateUniqueProperty(manager, L"dirname");
			if (dirname.empty())
			{
				return dirname;
			}
			else
			{
				return dirname + LR"(\)";
			}
		}
		wstring GeneralParserFile::GetGeneratePath(SymbolManager * manager)
		{
			wstring path =  LR"(Source\GenerateFile\)";
			return  path + GetGenerateDirname(manager);
		}
		vector<wstring> GeneralParserFile::GetGenerateNameSapce(SymbolManager* manager)
		{
			return GetGenerateArrayProperty(manager, L"namespace");
		}
		vector<wstring> GeneralParserFile::GetGenerateInclude(SymbolManager* manager)
		{
			return GetGenerateArrayProperty(manager, L"include");
		}
		wstring GeneralParserFile::GetGenerateUniqueProperty(SymbolManager * manager, const wstring & property, const wstring & default)
		{
			assert(generalParser != nullptr);
			auto result = manager->GetCacheValueByProperty(property);
			assert(result.size() <= 1);
			return result.empty() ? default: result[0];
		}

		vector<wstring> GeneralParserFile::GetGenerateArrayProperty(SymbolManager * manager, const wstring & property)
		{
			assert(generalParser != nullptr);
			auto result = manager->GetCacheValueByProperty(property);
			return result;
		}

		void GeneralParserFile::CreateFile(const wstring& fileName, const wstring& content)
		{
			wofstream output(fileName);
			output.write(content.c_str(), content.size());
		}
		wstring GeneralParserFile::GetFileLeafName(const wstring & fileName)
		{
			int backslashResult = (int)fileName.rfind(L"/");
			int slashResult = (int)fileName.rfind(L"\\");
			assert(backslashResult != slashResult || ((backslashResult == slashResult) && (backslashResult == -1)));
			int slashPosition = std::max(backslashResult, slashResult);
			auto leaf = fileName.substr(slashPosition + 1);
			return leaf;
		}
		GeneralParser & ztl::general_parser::GeneralParserFile::GetParser()
		{
			// TODO: insert return statement here
			return *generalParser;
		}
	}
}