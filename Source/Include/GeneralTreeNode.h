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
			void SetFieldMap(const wstring& fieldName, const int nodeIndex);

			void SetTermMap(const wstring& fieldName, const int nodeIndex);

			bool HaveThisField(const wstring& fieldName);
			const wstring& GetName()const;
			const int GetNumber()const;
			wstring GetNodeInfo() const;
			void AdjustFieldSequence();
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