#include "Include/stdafx.h"
#include "CharsetMapTable.h"
namespace ztl
{
	namespace pure_regex
	{
		void CharsetMapTable::AddCharRange(wchar_t first, wchar_t second)
		{
			assert(first <= second);
			for(auto i = first; i <= second;++i)
			{
				if (table[i]==0)
				{
					table[i] = count;
				}
			}
			++count;
		}
		void CharsetMapTable::AddChar(wchar_t value)
		{
			if((table[value] == 0)||
				(value == 0 && table[value + 1] == table[value]) ||
				(value == 65535 && table[value - 1] == table[value]) ||
				(table[value + 1] == table[value] || table[value - 1] == table[value]))
			{
				value = count;
				++count;
			}
		}
		int CharsetMapTable::GetCharSetMap(wchar_t value) const
		{
			return table[value];
		}
	}
}