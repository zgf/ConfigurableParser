#include "Include\stdafx.h"
#include "Include\GeneralLRExecutor.h"
#include "Include\GeneralParserFileGenerator.h"
#include "Include\SymbolManager.h"
namespace ztl
{
	namespace general_parser
	{
		GeneralParserFileGenerator::GeneralParserFileGenerator(const shared_ptr<GeneralLRExecutor>& _generalParser) :generalParser(_generalParser)
		{
		}
		GeneralParserFileGenerator::GeneralParserFileGenerator(const wstring & _fileName, const shared_ptr<GeneralLRExecutor>& _generalParser) : generalParser(_generalParser), fileName(_fileName)
		{
		}
		shared_ptr<GeneralTableDefine> GeneralParserFileGenerator::GetGenerateParserTableDefine()
		{
			generalParser->SetTokenPool(fileName);
			generalParser->BuildParser();
			generalParser->GenerateIsomorphismParserTree();
			auto parserResult = GenerateHeterogeneousParserTree(*generalParser);
			return shared_ptr<GeneralTableDefine>(*(std::shared_ptr<GeneralTableDefine>*)(&parserResult));
		}
		wstring GeneralParserFileGenerator::GenerateModulesWithNamespace(const wstring& content, const vector<wstring>& namespaces)
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
		wstring GetPreDefineHeadInclude()
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
				#include "$<RelativePath>GeneralLALRExecutor.h"
				)";
		}

		wstring GeneralParserFileGenerator::GenerateImpModuleInclude(const wstring& filename, const vector<wstring>& includes, SymbolManager* manager)
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
		wstring GeneralParserFileGenerator::GenerateModuleInclude(const wstring& endInclude, const vector<wstring>& includes)
		{
			const wstring templateString = L"#include \"$<FileName>\"\n";
			return accumulate(includes.begin(), includes.end(), wstring(), [&templateString](const wstring& sum, const wstring& value)
			{
				generator::MarcoGenerator generator(templateString, { L"$<FileName>" });
				return sum + generator.GenerateText({ value }).GetMacroResult() + L"\n";
			}) + endInclude;
		}
		wstring	GeneralParserFileGenerator::GetIncludeRelativePath(SymbolManager*manager)
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
		wstring GeneralParserFileGenerator::GenerateHeadModuleInclude(SymbolManager* manager)
		{
			auto headTemplateString = GetPreDefineHeadInclude();
			generator::MarcoGenerator generator(headTemplateString, { L"$<RelativePath>" });
			return generator.GenerateText({ GetIncludeRelativePath(manager) }).GetMacroResult();
		}
		wstring GeneralParserFileGenerator::GenerateHeadModuleContent(GeneralTableDefine* table, SymbolManager*manager, const vector<wstring>& namespaces)
		{
			auto headModule = GetGenerateHeadModuleBody(table, manager);
			headModule = GenerateModulesWithNamespace(headModule, namespaces);
			auto headInclude = GenerateHeadModuleInclude(manager);
			return headInclude + headModule;
		}
		wstring GetPredefineIRCodeGenHead(const wstring& filename)
		{
			wstring templateString = LR"(
					#pragma once
					#include "$<FileName>.h"
				)";
			generator::MarcoGenerator generator(templateString, { L"$<FileName>" });
			return generator.GenerateText({ filename }).GetMacroResult();
		}
		wstring GeneralParserFileGenerator::GeneratIRCodeGenModuleContent(const wstring& filename,GeneralTableDefine* table, SymbolManager*manager, const vector<wstring>& namespaces)
		{
			auto headModule = GetGenerateIRCodeGenoduleBody(table, manager);
			headModule = GenerateModulesWithNamespace(headModule, namespaces);
			return GetPredefineIRCodeGenHead(filename) + headModule;
		}
		wstring GeneralParserFileGenerator::GenerateImpModuleContent(GeneralTableDefine* table, SymbolManager*manager, const vector<wstring>& includes, const wstring filename, const vector<wstring>& namespaces)
		{
			auto impModule = GetGenerateImpModuleBody(table, manager);
			impModule = GenerateModulesWithNamespace(impModule, namespaces);
			auto impInclude = GenerateImpModuleInclude(filename, includes, manager);
			return impInclude + impModule;
		}
		wstring GeneralParserFileGenerator::GetGenerateHeadModuleBody(GeneralTableDefine* table, SymbolManager*manager)
		{
			auto nodeDefineModule = GetNodeDefineModule(table, manager);
			auto reflectModuleHead = GetReflectionModuleHead();
			auto serialCoreModuleHead = SerializeEBNFCoreModuleHead();
			return nodeDefineModule + serialCoreModuleHead + reflectModuleHead;
		}
		wstring GeneralParserFileGenerator::GetGenerateIRCodeGenoduleBody(GeneralTableDefine* table, SymbolManager*manager)
		{
			return GetIRCodeGenModule(table, manager);
		}
		wstring GeneralParserFileGenerator::GetGenerateImpModuleBody(GeneralTableDefine* table, SymbolManager*manager)
		{
			auto reflectModule = GetReflectionModuleImp(manager);
			auto serialzeCoreModule = SerializeEBNFCoreModuleImp(table);
			return reflectModule + serialzeCoreModule;
		}

		void GeneralParserFileGenerator::GenerateSpecialParserFile()
		{
			
			auto table = GetGenerateParserTableDefine();
			SymbolManager manager(table);
			SemanticAnalysis(&manager);

			auto namespaces = GetGenerateNameSapce(&manager);
			auto includes = GetGenerateInclude(&manager);
			auto filename = GetGenerateFileName(&manager);

			auto implContent = GenerateImpModuleContent(table.get(), &manager, includes, filename, namespaces);
			auto headContent = GenerateHeadModuleContent(table.get(), &manager, namespaces);
			auto path = GetGeneratePath(&manager);
			auto headFilename = path + filename + L".h";
			auto implFilename = path + filename + L".cpp";
			this->CreateFile(headFilename, headContent);
			this->CreateFile(implFilename, implContent);

			auto IRCodeGenFilename = path + filename + L"IRCodeGen.h";
			auto IRCodeGenContent = GeneratIRCodeGenModuleContent(filename,table.get(), &manager, namespaces);
			this->CreateFile(IRCodeGenFilename, IRCodeGenContent);
		}

		wstring GeneralParserFileGenerator::GetGenerateFileName(SymbolManager* manager)
		{
			return GetGenerateUniqueProperty(manager, L"filename", L"GenerateParserFile");
		}

		wstring GeneralParserFileGenerator::GetGenerateClassPrefix(SymbolManager* manager)
		{
			return GetGenerateUniqueProperty(manager, L"classprefix");
		}
		wstring GeneralParserFileGenerator::GetGenerateDirname(SymbolManager * manager)
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
		wstring GeneralParserFileGenerator::GetGeneratePath(SymbolManager * manager)
		{
			wstring path =  LR"(Source\GenerateFile\)";
			return  path + GetGenerateDirname(manager);
		}
		vector<wstring> GeneralParserFileGenerator::GetGenerateNameSapce(SymbolManager* manager)
		{
			return GetGenerateArrayProperty(manager, L"namespace");
		}
		vector<wstring> GeneralParserFileGenerator::GetGenerateInclude(SymbolManager* manager)
		{
			return GetGenerateArrayProperty(manager, L"include");
		}
		wstring GeneralParserFileGenerator::GetGenerateUniqueProperty(SymbolManager * manager, const wstring & property, const wstring & default)
		{
			assert(generalParser != nullptr);
			return manager->GetGenerateUniqueProperty(property,default);
		}

		vector<wstring> GeneralParserFileGenerator::GetGenerateArrayProperty(SymbolManager * manager, const wstring & property)
		{
			assert(generalParser != nullptr);
			return manager->GetGenerateArrayProperty(property);
		}

		void GeneralParserFileGenerator::CreateFile(const wstring& filePath, const wstring& content)
		{
			wofstream output(filePath);
			output.write(content.c_str(), content.size());
		}
		wstring GeneralParserFileGenerator::GetFileLeafName(const wstring & filePath)
		{
			int backslashResult = (int) filePath.rfind(L"/");
			int slashResult = (int) filePath.rfind(L"\\");
			assert(backslashResult != slashResult || ((backslashResult == slashResult) && (backslashResult == -1)));
			int slashPosition = std::max(backslashResult, slashResult);
			auto leaf = filePath.substr(slashPosition + 1);
			return leaf;
		}
		GeneralLRExecutor & ztl::general_parser::GeneralParserFileGenerator::GetParser()
		{
			// TODO: insert return statement here
			return *generalParser;
		}
	}
}