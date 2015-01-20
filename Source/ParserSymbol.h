#pragma once
#include "stdafx.h"
namespace ztl
{
	namespace general_parser
	{
		struct TokenPosition
		{
			int index;
			int length;
		};
		struct TokenInfo
		{
			int		tag;
			TokenPosition	position;
			wstring			lex;
		};
		struct FileInfo
		{
			wstring name;
			wstring content;
		};
	}
}