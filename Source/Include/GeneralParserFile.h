#pragma once
#include "stdafx.h"
namespace ztl
{
	namespace general_parser
	{
		class GeneralParser;
		struct GeneralTableDefine;
		//节点定义文件的生成,节点反射函数的生成.
		//要生成的,节点定义,节点反射 节点序列化三个模块.放到同一个.hpp文件里面去
		//
		class GeneralParserFile
		{
		public:
			GeneralParserFile()  = default;
			GeneralParserFile(const wstring& fileName, const shared_ptr<GeneralTableDefine>& _tableDefine);
			~GeneralParserFile() noexcept = default;
			GeneralParserFile(GeneralParserFile&&)  = default;
			GeneralParserFile(const GeneralParserFile&)  = default;
			GeneralParserFile& operator=(GeneralParserFile&&)  = default;
			GeneralParserFile& operator=(const GeneralParserFile&)   = default;
		public:
			void				GenerateSpecialParserFile();
			void CreateFile(const wstring& fileName, const wstring& content);

		private:
			wstring				  GetGenerateModuleBody(GeneralTableDefine* table, SymbolManager*manager);
			wstring GetGenerateFileName(SymbolManager* manager);
			vector<wstring> GetGenerateNameSapce(SymbolManager* manager);
			wstring GetGenerateClassPrefix(SymbolManager* manager);
			vector<wstring> GetGenerateInclude(SymbolManager* manager);
			shared_ptr<GeneralTableDefine>	GetGenerateParserTableDefine();
			wstring GenerateModulesWithNamespace(const wstring& content,const vector<wstring>& namespaces);
			wstring GenerateModulesInclude(const vector<wstring>& includes);
		private:
			shared_ptr<GeneralParser>	generalParser;
			

		};
		wstring GetNodeDefineFileInclude(SymbolManager* manager);
		wstring GetReflectionModule(SymbolManager* manager);
		wstring GetNodeDefineModule(GeneralTableDefine* table, SymbolManager*manager);
		wstring SerializeEBNFCoreModule(void* tableDefine);
	}
}