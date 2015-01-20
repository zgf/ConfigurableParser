#pragma once
namespace ztl
{
	namespace algorithm
	{
		template<typename string_type>
		string_type&   replace_all(string_type&   str, const   string_type&   old_value, const   string_type&   new_value)
		{
			while(true)
			{
				string_type::size_type   pos(0);
				if((pos = str.find(old_value)) != string_type::npos)
					str.replace(pos, old_value.length(), new_value);
				else   break;
			}
			return   str;
		}
		template<typename string_type>
		string_type&   replace_all_distinct(string_type&   str, const   string_type&   old_value, const   string_type&   new_value)
		{
			for(string_type::size_type pos(0); pos != string_type::npos; pos += new_value.length())
			{
				if((pos = str.find(old_value, pos)) != string_type::npos)
					str.replace(pos, old_value.length(), new_value);
				else   break;
			}
			return   str;
		}
	}
}