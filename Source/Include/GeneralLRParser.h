#pragma once
#include "stdafx.h"
namespace ztl
{
	namespace general_parser
	{
		class LALRParser
		{
		public:
			LALRParser() noexcept = default;
			~LALRParser() noexcept = default;
			LALRParser(LALRParser&&) noexcept = default;
			LALRParser(const LALRParser&) noexcept = default;
			LALRParser& operator=(LALRParser&&) noexcept = default;
			LALRParser& operator=(const LALRParser&) noexcept = default;

		protected:

		private:

		};
	}
}