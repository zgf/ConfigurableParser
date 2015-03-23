#include "Include\stdafx.h"
#include "Include\GeneralTreeNode.h"
namespace ztl
{
	namespace general_parser
	{
		GeneralTreeNode::GeneralTreeNode(const int number, const wstring & _name) :name(_name), nodeNumber(number)
		{
		}

		GeneralTreeNode::GeneralTreeNode(const wstring & _name) : name(_name)
		{
		}
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
		void GeneralTreeNode::SetField(const wstring& fieldName, const int nodeIndex)
		{
			auto findIter = fieldMap.find(fieldName);
			if(findIter == fieldMap.end())
			{
				findIter = termMap.find(fieldName);
				if(findIter == termMap.end())
				{
					throw ztl_exception(L"error! this Node haven't this field " + fieldName + L"\n");
				}
				termMap[fieldName].emplace_back(nodeIndex);
			}
			else
			{
				assert(termMap.find(fieldName) == termMap.end());
				fieldMap[fieldName].emplace_back(nodeIndex);
			}
		}

		void GeneralTreeNode::InitTermMap(const wstring & fieldName)
		{
			termMap.insert({ fieldName,vector<int>() });
		}

		void GeneralTreeNode::InitTermMap(const vector<wstring>& fileNames)
		{
			for(auto&&iter : fileNames)
			{
				InitTermMap(iter);
			}
		}

		void GeneralTreeNode::InitFieldMap(const wstring & fieldName)
		{
			fieldMap.insert({ fieldName,vector<int>() });
		}

		void GeneralTreeNode::InitFieldMap(const vector<wstring>& fileNames)
		{
			for(auto&&iter : fileNames)
			{
				InitFieldMap(iter);
			}
		}

		bool GeneralTreeNode::HaveThisField(const wstring & fieldName)
		{
			if(this->fieldMap.find(fieldName) != fieldMap.end() ||
				this->termMap.find(fieldName) != fieldMap.end())
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
			return name;
		}

		const int GeneralTreeNode::GetNumber() const
		{
			return nodeNumber;
		}
		void GeneralTreeNode::SetName(const wstring& _name)
		{
			assert(name.empty());
			name = _name;
		}
		void GeneralTreeNode::SetNumber(int number)
		{
			nodeNumber = number;
		}
		bool	GeneralTreeNode::IsEmpty()const
		{
			return name.empty() && this->fieldMap.empty() && this->termMap.empty();
		}
		bool HasSameField(const unordered_map<wstring, vector< int>>& left, const unordered_map<wstring, vector<int>>& right)
		{
			return find_if(left.begin(), left.end(), [&right](const pair<const wstring, vector< int>>& element)
			{
				return right.find(element.first) == right.end();
			}) == left.end();
		}
		bool HasSameField(const unordered_map<wstring, vector< int>>& left, const unordered_map<wstring, vector<int>>& right,const vector<wstring>& exclude)
		{
			for (auto&& element:left)
			{
				if(right.find(element.first) == right.end())
				{
					auto findResult = find(exclude.begin(), exclude.end(), element.first);
					if(findResult == exclude.end())
					{
						return false;
					}
				}
			}
			return true;
		}
		bool GeneralTreeNode::IsMayDeriveType(const GeneralTreeNode & node) const
		{
			return HasSameField(termMap, node.termMap) && HasSameField(fieldMap, node.fieldMap);
		}
		bool GeneralTreeNode::IsMayDeriveType(const GeneralTreeNode & node, const vector<wstring>& exclude) const
		{
			return HasSameField(termMap, node.termMap, exclude) && HasSameField(fieldMap, node.fieldMap);
		}
	}
}