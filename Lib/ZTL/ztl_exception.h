#pragma once
#include <string>
#include <iostream>
using std::wstring;
using std::wcout;
using std::endl;
namespace ztl
{
	namespace contract
	{
		class ztl_exception
		{
		public:
			ztl_exception() = delete;
			ztl_exception(const wstring& _message) :message(_message)
			{

			}
		public:
			void What()const
			{
				wcout << message << endl;
			}
			wstring Message()const
			{
				return message;
			}
		private:
			wstring message;
		};
		template<typename exception_type>
		void ThrowException(bool result, const exception_type exception)
		{
			if(!result)
			{
				throw exception;
			}
		}
		
		/*void ThrowZTLException(bool result, const wstring message)
		{
			if(!result)
			{
				throw ztl_exception(message);
			}
		}*/
	}

	
}