#pragma once
#include "stdafx.h"
namespace ztl
{
	namespace general_parser
	{
		class GeneralParser;
		struct GeneralTableDefine;
		class SymbolManager;
		//�ڵ㶨���ļ�������,�ڵ㷴�亯��������.
		//Ҫ���ɵ�,�ڵ㶨��,�ڵ㷴�� �ڵ����л�����ģ��.�ŵ�ͬһ��.hpp�ļ�����ȥ
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
			wstring GetFileLeafName(const wstring& fileName);
		private:
			wstring GetGenerateImpModuleBody(GeneralTableDefine* table, SymbolManager*manager);
			wstring GetGenerateHeadModuleBody(GeneralTableDefine* table, SymbolManager*manager);
			wstring GetGenerateFileName(SymbolManager* manager);
			vector<wstring> GetGenerateNameSapce(SymbolManager* manager);
			wstring GetGenerateClassPrefix(SymbolManager* manager);
			wstring GetGenerateDirname(SymbolManager* manager);
			wstring GetGeneratePath(SymbolManager* manager);
			wstring GetGenerateUniqueProperty(SymbolManager* manager, const wstring& property, const wstring& default = wstring());
			vector<wstring> GetGenerateArrayProperty(SymbolManager* manager, const wstring& property);
			wstring			GetIncludeRelativePath(SymbolManager*manager);
			vector<wstring> GetGenerateInclude(SymbolManager* manager);
			shared_ptr<GeneralTableDefine>	GetGenerateParserTableDefine();
			wstring GenerateModulesWithNamespace(const wstring& content,const vector<wstring>& namespaces);
			wstring GenerateImpModuleInclude(const wstring& filename, const vector<wstring>& includes, SymbolManager* manager);
			wstring GenerateModuleInclude(const wstring& endInclude, const vector<wstring>& includes);
			wstring GenerateHeadModuleInclude(SymbolManager* manager);
			wstring GenerateHeadModuleContent(GeneralTableDefine* table, SymbolManager*manager, const vector<wstring>& namespaces);
			wstring GenerateImpModuleContent(GeneralTableDefine* table, SymbolManager*manager, const vector<wstring>& includes, const wstring filename, const vector<wstring>& namespaces);

		private:
			shared_ptr<GeneralParser>	generalParser;
			

		};
		wstring GetNodeDefineFileInclude(SymbolManager* manager);
		wstring GetReflectionModuleImp(SymbolManager* manager);
		wstring SerializeEBNFCoreModuleHead();
		wstring GetReflectionModuleHead();
		wstring GetNodeDefineModule(GeneralTableDefine* table, SymbolManager*manager);
		wstring SerializeEBNFCoreModuleImp(void* tableDefine);
	}
}