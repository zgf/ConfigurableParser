#pragma once
#include <memory>

namespace ztl
{
	using std::shared_ptr;
	template<typename Type>
	using Ptr = shared_ptr < Type >;
}
