#pragma once
#include "..\..\Include\stdafx.h"
namespace ztl
{
	namespace pure_regex
	{
		//Ñ¹Ëõ×Ö·û·¶Î§µÄ±í
		class CharsetMapTable
		{
		public:
			CharsetMapTable() :table({ 0 }), count(0)
			{

			}
			~CharsetMapTable() noexcept = default;
			CharsetMapTable(CharsetMapTable&&) noexcept = default;
			CharsetMapTable(const CharsetMapTable&) noexcept = default;
			CharsetMapTable& operator=(CharsetMapTable&&) noexcept = default;
			CharsetMapTable& operator=(const CharsetMapTable&) noexcept  = default;
			
		public:
			void AddCharRange(wchar_t first, wchar_t second);
			void AddChar(wchar_t value);
			int GetCharSetMap(wchar_t value)const;
		private:
			std::array<unsigned short, 65536> table;
			unsigned short count;
		};
	}
}