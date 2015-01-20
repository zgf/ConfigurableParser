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
		public:
			TokenPosition() = default;
			TokenPosition(int _index, int _length) :index(_index), length(_length)
			{

			}
		};
		struct TokenInfo
		{
			int				tag;
			TokenPosition	position;
			wstring			lex;
		public:
			TokenInfo() = default;
			TokenInfo(int _tag, int _index, int _length, wstring _lex) :tag(_tag), lex(_lex), position(_index, _length)
			{

			}
		};
		struct FileInfo
		{
			wstring name;
			wstring content;
		};
		struct LexTokenDefine
		{
			wstring name;
			wstring regex;
			bool    ignore;
			LexTokenDefine() = default;
			LexTokenDefine(const wstring& _name, const wstring& _regex, const bool _ignore)
				: name(_name), regex(_regex), ignore(_ignore)
			{
			}
		};

	}
}