#pragma once
#include "stdafx.h"
namespace ztl
{
	namespace general_parser
	{
		class SymbolManager;
		//节点定义文件的生成,节点反射函数的生成.
		//
		class GeneralParserFile
		{
		public:
			GeneralParserFile()  = default;
			~GeneralParserFile() noexcept = default;
			GeneralParserFile(GeneralParserFile&&)  = default;
			GeneralParserFile(const GeneralParserFile&)  = default;
			GeneralParserFile& operator=(GeneralParserFile&&)  = default;
			GeneralParserFile& operator=(const GeneralParserFile&)   = default;
		private:
			SymbolManager* manager;
		};
		void NodeDefineFileAddNamespace(wstring& content, SymbolManager* manager);
		void CreateFile(const wstring& fileName, const wstring& content);
		void CreateNodeDefineFile(const wstring& fileName, SymbolManager* manager);
		wstring GetNodeDefineFilePreDefineIncludeString();
		wstring GetNodeDefineFileInclude(SymbolManager* manager);
		void CreatReflectionFile(SymbolManager* manager);
	}
}