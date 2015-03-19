#pragma once
#include "Include/stdafx.h"
#include "Include/GeneralTreeNode.h"
#include "Include/GeneralParser.h"
namespace ztl
{
	namespace general_parser
	{
		GeneralTreeNode* ztl::general_parser::GeneralParser::MakeTreeNode(const wstring & nodeName)
		{
			static unordered_map<wstring, GeneralTreeNode> signMap = 
			{
				/*Code Generate*/
			};

			auto&& findIter = signMap.find(nodeName);
			if (findIter == signMap.end())
			{
				throw ztl_exception(L"error!"+nodeName+L" isn't a vaild TreeNode's name");
			}
			this->nodePool.emplace_back(make_shared<GeneralTreeNode>(findIter->second));
			nodePool.back()->SetNumber(nodePool.size() - 1);
			return nodePool.back().get();
		}
	}
}