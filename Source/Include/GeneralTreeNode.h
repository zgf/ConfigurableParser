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
		private:
			wstring name;
			unordered_map<wstring, int> fieldMap;
			unordered_map<wstring, int> termMap;
		};
		
	}
}