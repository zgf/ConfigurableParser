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
				#include "GeneralTableDefine.h"
				#include "GeneralTableWriter.h"
				#include "GeneralTreeNode.h"
				#include "GeneralParser.h"
				)";
		}
		
		wstring GeneralParserFile::GenerateImpModuleInclude(const wstring& filename, const vector<wstring>& includes)
		{
			const wstring endInclude =
				LR"(#include ")" + GetFileLeafName(filename) + LR"(.h")";
			return GenerateModuleInclude(endInclude, includes);
		}
		wstring GeneralParserFile::GenerateModuleInclude(const wstring& endInclude, const vector<wstring>& includes)
		{
			const wstring templateString = L"#include \"$<FileName>\"\n";
			return accumulate(includes.begin(), includes.end(), wstring(), [&templateString](const wstring& sum, const wstring& value)
			{
				generator::MarcoGenerator generator(templateString, { L"$<FileName>" });
				return sum + generator.GenerateText({ value }).GetMacroResult() + L"\n";
			})+endInclude;
		}
		wstring GeneralParserFile::GenerateHeadModuleInclude()
{
			return 	GetPreDefineInclude();
		}
		wstring GeneralParserFile::GenerateHeadModuleContent(GeneralTableDefine* table, SymbolManager*manager,const vector<wstring>& namespaces)
		{
			auto headModule = GetGenerateHeadModuleBody(table, manager);
			headModule = GenerateModulesWithNamespace(headModule, namespaces);
			auto headInclude = GenerateHeadModuleInclude();
			return headInclude + headModule;
		}
		wstring GeneralParserFile::GenerateImpModuleContent(GeneralTableDefine* table, SymbolManager*manager, const vector<wstring>& includes, const wstring filename, const vector<wstring>& namespaces)
		{
			auto impModule = GetGenerateImpModuleBody(table, manager);
			impModule = GenerateModulesWithNamespace(impModule, namespaces);
			auto impInclude = GenerateImpModuleInclude(filename, includes);
			return impInclude + impModule;
		}
		wstring GeneralParserFile::GetGenerateHeadModuleBody(GeneralTableDefine* table, SymbolManager*manager)
		{
			auto nodeDefineModule = GetNodeDefineModule(table, manager);
			auto reflectModuleHead = GetReflectionModuleHead();
			auto serialCoreModuleHead = SerializeEBNFCoreModuleHead();
			return nodeDefineModule+serialCoreModuleHead+reflectModuleHead;
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

			auto headFilename = filename + L".h";
			auto implFilename = filename + L".cpp";
			
			CreateFile(headFilename, headContent);
			CreateFile(implFilename, implContent);
		}
		wstring GeneralParserFile::GetGenerateFileName(SymbolManager* manager)
		{
			assert(generalParser != nullptr);
			auto result = manager->GetCacheValueByProperty(L"filename");
			assert(result.size() <= 1);
			return result.empty() ? wstring(L"GenerateParserFile") : result[0];
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
		wstring GeneralParserFile::GetFileLeafName(const wstring & fileName)
		{
			int backslashResult =  fileName.rfind(L"/");
			int slashResult = fileName.rfind(L"\\");
			assert(backslashResult != slashResult);
			auto slashPosition = std::max(backslashResult, slashResult);
			auto leaf = fileName.substr(slashPosition + 1);
			return leaf;
		}
	}
}