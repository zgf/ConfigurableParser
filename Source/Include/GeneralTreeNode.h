#pragma once
#include "stdafx.h"
namespace ztl
{
	namespace general_parser
	{
		class GeneralTokenInfo;
		class ParserSymbol;
		class GeneralTreeNode
		{
		public:
			GeneralTreeNode() = default;
			~GeneralTreeNode() noexcept = default;
			GeneralTreeNode(GeneralTreeNode&&) = default;
			GeneralTreeNode(const GeneralTreeNode&) = default;
			GeneralTreeNode& operator=(GeneralTreeNode&&) = default;
			GeneralTreeNode& operator=(const GeneralTreeNode&) = default;
			GeneralTreeNode(const int number,ParserSymbol* symbol);
			void SetField(const wstring& fieldName, const int nodeIndex);
			void SetFieldMap(const wstring& fieldName, const int nodeIndex);

			void SetTermMap(const wstring& fieldName, const int nodeIndex);

			void InitTermMap(const wstring& fieldName);
			void InitTermMap(const vector<wstring>& fieldName);
			void InitFieldMap(const wstring& fieldName);
			void InitFieldMap(const vector<wstring>&  fieldName);
			bool HaveThisField(const wstring& fieldName);
			const wstring& GetName()const;
			void SetParserSymbol(ParserSymbol* symbol);
			const int GetNumber()const;
			void	  SetNumber(int number);
			wstring GetNodeInfo() const;
			const unordered_map<wstring, vector<int>> GetFieldMap()const;
			const unordered_map<wstring, vector<int>> GetTermMap()const;
		private:
			int nodeNumber;//nodeÔÚnodePoolµÄindex
			ParserSymbol* symbol;//classDef
			unordered_map<wstring, vector<int>> fieldMap;
			unordered_map<wstring, vector<int>> termMap;
		};
	}
}