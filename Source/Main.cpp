// ConfigurableParser.cpp : Defines the entry point for the console application.
//

#include "Include\stdafx.h"
#include "Include\SymbolManager.h"
#include "Include\GeneralPushDownAutoMachine.h"
#include "Include\GeneralPushDownMachineData.h"
#include "Include\GeneralLALRParser.h"
#include "Include\GeneralParserFile.h"
#include "GenerateFile\Json\JsonParser.h"
#include "GenerateFile\Xml\XmlParser.h"
#include "GenerateFile\PureRegex\PureRegexParser.h"
#include "Include\GeneralLRMachine.h"
#include <windows.h>
#include "..\UnitTest\ParserTest.h"
#include "..\UnitTest\GenerateFileTest.h"
#include "..\UnitTest\UnitTest.h"

int main()
{
	//ztl::general_parser::unit_test::UintTestTestAll();
	//releaseһ��������200msȫ��ִ������
	//GenerateFile();
	//����10W��Json 0.5s
	//����100W��Json 6s 3G���ڴ濪��
	//�����O(n)��Ч��
	//����10W��XML 0.9s
	//����100W��XML 3G�ڴ�������- -.
	//���������ܻ�����
	//MINISHARP

	ztl::general_parser::generate_file_test::GenerateTestAssignParserFile();
	ztl::general_parser::generate_file_test::GeneratePureRegexParserFile();
	ztl::general_parser::generate_file_test::GenerateXmlParserFile();

	ztl::general_parser::generate_file_test::GenerateMiniSharpParserFile();
	ztl::general_parser::generate_file_test::GenerateGeneralParserFile();

	ztl::general_parser::generate_file_test::GenerateJsonParserFile();

	//ztl::general_parser::parser_test::TestMiniSharp();
	//ztl::general_parser::parser_test::TestJson();
	//ztl::general_parser::parser_test::TestXml();
	//ztl::general_parser::parser_test::TestPureRegex();

	//ztl::general_parser::parser_test::TestTestAssign();
	//ztl::general_parser::unit_test::TestFirstSet();

	//ztl::general_parser::generate_file_test::GeneratePureRegexParserFile();
	//ztl::general_parser::generate_file_test::GenerateGeneralParserFile();
	//ztl::general_parser::generate_file_test::GenerateJsonParserFile();
	//ztl::general_parser::generate_file_test::GenerateXmlParserFile();
	return 0;
}