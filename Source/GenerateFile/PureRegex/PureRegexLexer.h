#pragma once
#include "..\..\Include\stdafx.h"
namespace ztl
{
	namespace pure_regex
	{
		
		//PureRegexASTBuilder.��������,�����ӦPureRegex�﷨��
		//CharsetTable.�����﷨��,�����ַ�����ӳ���
		//PureRegexASTToNFA. ����regex�﷨��,����NFA.
		//NFAToDFA NFAתDFA������ͣ��״̬�ϴ��ϱ��.
		//DFAToDFATable  DFA������ת��
		//RunTimeBuilder. ����ʱ�����,����Token.
		vector<shared_ptr<ztl::general_parser::TokenInfo>> PureRegexParseToken(const wstring& pattern);

	}
}