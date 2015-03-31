#pragma once
#include "stdafx.h"
namespace ztl
{
	namespace general_parser
	{
		class SymbolManager;
		//�ڵ㶨���ļ�������,�ڵ㷴�亯��������.
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