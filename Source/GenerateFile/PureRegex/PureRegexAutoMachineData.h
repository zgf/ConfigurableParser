#pragma once
#include "..\..\Include\stdafx.h"
namespace ztl
{
	namespace pure_regex
	{
		
		struct Alternate;
		struct TokenPacket
		{
			wstring name;
			wstring regex;
			shared_ptr<Alternate> root;
		};
		class NFANode
		{
			int number;
			unordered_map<unsigned short, vector<NFANode*>> next;
		};
		class DFANode
		{
			int number;
			unordered_map<unsigned short, DFANode*> next;
		};
		class DFATable
		{
			vector<vector<unsigned short>> table;
		};
		
	}
}