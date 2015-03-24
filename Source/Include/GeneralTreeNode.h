#pragma once
#include "stdafx.h"
namespace ztl
{
	namespace general_parser
	{
		class GeneralTokenInfo;
		class GeneralTreeNode
		{
		public:
			GeneralTreeNode() = default;
			~GeneralTreeNode() noexcept = default;
			GeneralTreeNode(GeneralTreeNode&&) = default;
			GeneralTreeNode(const GeneralTreeNode&) = default;
			GeneralTreeNode& operator=(GeneralTreeNode&&) = default;
			GeneralTreeNode& operator=(const GeneralTreeNode&) = default;
			GeneralTreeNode(const int number,const wstring& name);
			GeneralTreeNode(const wstring& _name);
			void SetField(const wstring& fieldName, const int nodeIndex);
			void SetFieldMap(const wstring& fieldName, const int nodeIndex);

			void SetTermMap(const wstring& fieldName, const int nodeIndex);

			void InitTermMap(const wstring& fieldName);
			void InitTermMap(const vector<wstring>& fieldName);
			void InitFieldMap(const wstring& fieldName);
			void InitFieldMap(const vector<wstring>&  fieldName);
			bool HaveThisField(const wstring& fieldName);
			const wstring& GetName()const;
			void	  SetName(const wstring& _name);
			const int GetNumber()const;
			void	  SetNumber(int number);
			wstring GetNodeInfo() const;
			bool	IsEmpty()const;
			bool IsMayDeriveType(const GeneralTreeNode& node)const;
			bool IsMayDeriveType(const GeneralTreeNode & node, const vector<wstring>& exclude) const;
			bool IsTheSameType(const GeneralTreeNode&node)const;
			bool IsTheSameType(const GeneralTreeNode&node, const vector<wstring>& exclude) const;

		private:
			int nodeNumber;//nodeÔÚnodePoolµÄindex
			wstring name;
			unordered_map<wstring, vector<int>> fieldMap;
			unordered_map<wstring, vector<int>> termMap;
		};
	}
}