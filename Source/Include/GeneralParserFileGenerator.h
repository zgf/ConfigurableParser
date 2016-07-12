#pragma once
namespace ztl
{
	namespace general_parser
	{
		class GeneralLRExecutor;
		struct GeneralTableDefine;
		class GrammarBuilder;
		class SymbolManager;
		class GeneralLRExecutor;
		//节点定义文件的生成,节点反射函数的生成.
		//要生成的,节点定义,节点反射 节点序列化三个模块.放到同一个.hpp文件里面去
		//
		class GeneralParserFileGenerator
		{
		public:
			GeneralParserFileGenerator()  = default;
			GeneralParserFileGenerator(const shared_ptr<GeneralLRExecutor>& _generalParser);
			GeneralParserFileGenerator(const wstring& fileName,const shared_ptr<GeneralLRExecutor>& _generalParser);

			~GeneralParserFileGenerator() noexcept = default;
			GeneralParserFileGenerator(GeneralParserFileGenerator&&)  = default;
			GeneralParserFileGenerator(const GeneralParserFileGenerator&)  = default;
			GeneralParserFileGenerator& operator=(GeneralParserFileGenerator&&)  = default;
			GeneralParserFileGenerator& operator=(const GeneralParserFileGenerator&)   = default;
		public:
			void				GenerateSpecialParserFile();
			void CreateFile(const wstring& fileName, const wstring& content);
			wstring GetFileLeafName(const wstring& fileName);
			GeneralLRExecutor&	GetParser();
		private:
			wstring GetGenerateImpModuleBody(GeneralTableDefine* table, SymbolManager*manager);
			wstring GetGenerateHeadModuleBody(GeneralTableDefine* table, SymbolManager*manager);
			wstring GetGenerateIRCodeGenoduleBody(GeneralTableDefine* table, SymbolManager*manager);
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
			wstring GeneratIRCodeGenModuleContent(const wstring& filename,GeneralTableDefine* table, SymbolManager*manager, const vector<wstring>& namespaces);
			wstring GenerateImpModuleContent(GeneralTableDefine* table, SymbolManager*manager, const vector<wstring>& includes, const wstring filename, const vector<wstring>& namespaces);

		private:
			shared_ptr<GeneralLRExecutor>	generalParser;
			wstring fileName;

		};
		extern void LogGraphInfo(const wstring& fileName, GrammarBuilder& machine);
		wstring GetReflectionModuleImp(SymbolManager* manager);
		wstring SerializeEBNFCoreModuleHead();
		wstring GetReflectionModuleHead();
		wstring GetNodeDefineModule(GeneralTableDefine* table, SymbolManager*manager);
		wstring SerializeEBNFCoreModuleImp(void* tableDefine);
		wstring GetIRCodeGenModule(GeneralTableDefine* table, SymbolManager*manager);
	}
}