#pragma once
#include "Include\stdafx.h"
#include "ztl_algorithm.h"
#include <codecvt>
#include <string>
#include <locale>
std::wstring ztl::algorithm::s2ws(const std::string & str)
{
	std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t> converterX;
	return converterX.from_bytes(str);
}

std::string ztl::algorithm::ws2s(const std::wstring & wstr)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;
	return converterX.to_bytes(wstr);
}
