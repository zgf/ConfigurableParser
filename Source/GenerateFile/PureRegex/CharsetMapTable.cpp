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
				(value == 0 && table[1] == table[0]) ||
				(value == 65535 && table[65535 - 1] == table[65535]) ||
				(table[value + 1] == table[value] || table[value - 1] == table[value]))
			{
				table[value] = count;
				++count;
			}
		}
		unsigned short CharsetMapTable::GetCharSetMap(wchar_t value) const
		{
			return table[value];
		}

		unsigned short CharsetMapTable::GetNullEdge() 
		{
			return 65535;
		}

		int CharsetMapTable::GetCount() const
		{
			return count;
		}

		void CharsetMapTable::AddEnd()
		{
			unsigned short number = 1;
			for(size_t i = 0; i < table.size();)
			{
				if (table[i]!=0)
				{
					auto current = table[i];
					auto findIter = find_if(table.begin() + i, table.end(), [current](unsigned short val)
					{
						return val != 0 && current != val;
					});
					replace_if(table.begin() + i, findIter, [](unsigned short val)
					{
						return val != 0;
					}, number);
					++number;
					i = findIter - table.begin();
				}
				else
				{
					++i;
				}
			}
			count = number;
		}
	}
}