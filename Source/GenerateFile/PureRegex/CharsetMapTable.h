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
			CharsetMapTable() : table({0}),count(1)
			{
			}
			~CharsetMapTable() noexcept = default;
			CharsetMapTable(CharsetMapTable&&)  = default;
			CharsetMapTable(const CharsetMapTable&)  = default;
			CharsetMapTable& operator=(CharsetMapTable&&)  = default;
			CharsetMapTable& operator=(const CharsetMapTable&)   = default;
			
		public:
			void AddCharRange(wchar_t first, wchar_t second);
			void AddChar(wchar_t value);
			void AddEnd();//ÖØÅÅĞò.
			unsigned short GetCharSetMap(wchar_t value)const;
			static unsigned short GetNullEdge();
			int GetCount()const;
		private:
			std::array<unsigned short, 65536> table;
			unsigned short count;
		};
	}
}