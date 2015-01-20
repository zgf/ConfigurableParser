#pragma once
#include <string>
#include <assert.h>
#include <vector>
#include <algorithm>
#include "../../Lib/ZTL/ztl_algorithm.h"
namespace ztl
{
	using std::vector;
	using std::wstring;
	namespace generator
	{
		class MarcoGenerator
		{
		private:
			vector<wstring> params;
			vector<wstring> arguments;
			wstring text;
			wstring result;
		public:
			MarcoGenerator() = delete;
			MarcoGenerator(const wstring& _text,const vector<wstring>& paramList):text(_text),params(paramList.begin(),paramList.end())
			{

			}
		
			MarcoGenerator& GenerateText(const vector<wstring>& argumentList)
			{
				arguments.assign(argumentList.begin(), argumentList.end());
				GenerateText();
				return *this;
			}
			/*template<typename type>
			MarcoGenerator& GenerateText(const std::initializer_list<type>& argumentList)
			{
				arguments.assign(argumentList.begin(), argumentList.end());
				GenerateText();
				return *this;
			}*/
			void GenerateText()
			{
				this->result = text;
				assert(params.size() == arguments.size());
				assert(!arguments.empty());
				for(size_t i = 0; i < params.size();i++)
				{
					auto& param = params[i];
					auto& argument = arguments[i];
					ztl::algorithm::replace_all_distinct(result, param, argument);
				}
			}
			wstring GetMacroResult()const
			{ 
				return result;
			}
		};
	}
}