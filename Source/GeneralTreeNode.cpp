#include "Include\stdafx.h"
#include "Include\GeneralTreeNode.h"
#include "Include\ParserSymbol.h"
namespace ztl
{
	namespace general_parser
	{
		void GeneralTreeNode::SetFieldMap(const wstring& fieldName, const int nodeIndex)
		{
			auto findIter = fieldMap.find(fieldName);
			if(findIter == fieldMap.end())
			{
				fieldMap[fieldName];
			}
			fieldMap[fieldName].emplace_back(nodeIndex);
		}

		void GeneralTreeNode::SetTermMap(const wstring& fieldName, const int nodeIndex)
		{
			auto findIter = termMap.find(fieldName);
			if(findIter == termMap.end())
			{
				termMap[fieldName];
			}
			termMap[fieldName].emplace_back(nodeIndex);
		}
		GeneralTreeNode::GeneralTreeNode(const int number, ParserSymbol * _symbol) :symbol(_symbol), nodeNumber(number)
		{
		}

		bool GeneralTreeNode::HaveThisField(const wstring & fieldName)
		{
			if(this->fieldMap.find(fieldName) != fieldMap.end() ||
				this->termMap.find(fieldName) != termMap.end())
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		wstring GeneralTreeNode::GetNodeInfo() const
		{
			auto fieldMapString = accumulate(this->fieldMap.begin(), fieldMap.end(), wstring(L"fieldMap: "), [](wstring& sum, const pair<const wstring, vector<int>>& value)
			{
				auto fieldValueString = accumulate(value.second.begin(), value.second.end(), wstring(), [](const wstring& sum, int value)
				{
					return sum + to_wstring(value) + L", ";
				});

				return sum + L"fieldName:" + value.first + L"fieldValue" + fieldValueString + L"\n";
			});
			auto termMapString = accumulate(this->fieldMap.begin(), fieldMap.end(), wstring(L"termMap: "), [](wstring& sum, const pair<const wstring, vector<int>>& value)
			{
				auto termValueString = accumulate(value.second.begin(), value.second.end(), wstring(), [](const wstring& sum, int value)
				{
					return sum + to_wstring(value) + L", ";
				});

				return sum + L"fieldName:" + value.first + L"termValue" + termValueString + L"\n";
			});
			return L"node Name:" + GetName() + L"\n" +
				L"node Number" + to_wstring(GetNumber()) + L"\n" +
				fieldMapString + termMapString;
		}
		const wstring & GeneralTreeNode::GetName() const
		{
			return symbol->GetName();
		}

		const int GeneralTreeNode::GetNumber() const
		{
			return nodeNumber;
		}
	
		const unordered_map<wstring, vector<int>> GeneralTreeNode::GetFieldMap()const
		{
			return fieldMap;
		}
		const unordered_map<wstring, vector<int>> GeneralTreeNode::GetTermMap()const
		{
			return termMap;
		}
	}
}