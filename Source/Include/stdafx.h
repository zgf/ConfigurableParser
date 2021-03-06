
#include <exception>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <assert.h>
#include <algorithm>
#include <locale>
#include <codecvt>
#include <iostream>
#include <sstream>
#include <numeric>
#include <fstream>
#include <locale>
#include <deque>
#include <set>
#include <array>
#include <iterator>
#include <windows.h>
#include "../../Lib/ZTL/ztl_exception.h"
#include "../../Lib/ZTL/ztl_hash.hpp"
#include "../../Lib/ZTL/ztl_generator.hpp"
#include "../../Lib/ZTL/ztl_algorithm.h"

//Boost
#include <boost/xpressive/xpressive.hpp>
#include <boost/any.hpp>

//LLVM
#undef min
#undef max
#include <llvm\IR\IRBuilder.h>
namespace ztl
{
	//////////////////////////////////////////////////////////////////////////
	// ǰ������
	//////////////////////////////////////////////////////////////////////////
	using std::set;
	using std::make_unique;
	using std::vector;
	using std::shared_ptr;
	using std::make_shared;
	using std::wstring;
	using std::function;
	using std::unordered_map;
	using std::exception;
	using std::unordered_set;
	using std::unique_ptr;
	using std::to_wstring;
	using std::move;
	using std::wcout;
	using std::endl;
	using std::dynamic_pointer_cast;
	using std::wstringstream;
	using std::decay;
	using std::enable_shared_from_this;
	using std::equal;
	using std::reverse_iterator;
	using std::weak_ptr;
	using std::pair;
	using std::set_difference;
	using std::iota;
	using std::inserter;
	using std::fstream;
	using std::ofstream;
	using std::string;
	using std::wofstream;
	using std::hash;
	using std::deque;
	using std::unordered_multimap;
	using std::swap;
	using std::make_pair;
	using std::find_if_not;
	using std::find_if;
	using std::wfstream;
	using std::wifstream;
	using std::istreambuf_iterator;
	using std::accumulate;
	using std::ifstream;
	using std::back_inserter;
	using std::for_each;
	using std::array;
	using std::static_pointer_cast;
	using ztl::contract::ztl_exception;
	using ztl::generator::MarcoGenerator; 
	using ztl::hasher::ztl_hash;
}
