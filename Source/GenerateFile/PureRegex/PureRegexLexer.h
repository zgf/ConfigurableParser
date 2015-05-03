#pragma once
#include "..\..\Include\stdafx.h"
namespace ztl
{
	namespace pure_regex
	{
		
		//PureRegexASTBuilder.接受输入,构造对应PureRegex语法树
		//CharsetTable.接受语法树,构造字符集合映射表
		//PureRegexASTToNFA. 接受regex语法树,构造NFA.
		//NFAToDFA NFA转DFA并且在停机状态上打上标记.
		//DFAToDFATable  DFA生成跳转表
		//RunTimeBuilder. 运行时环境搭建,返回Token.
		vector<shared_ptr<ztl::general_parser::TokenInfo>> PureRegexParseToken(const wstring& pattern);

	}
}