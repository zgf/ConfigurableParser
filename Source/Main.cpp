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


int main()
{
	//ztl::general_parser::parser_test::TestPureRegex();
	//内部调用的regex库 debug模式下拆解token太慢- -debug下性能分析占了90%+的时间- -
	//release一启动渣本200ms全部执行完了
	//GenerateFile();
	//解析10W行Json 0.5s
	//解析100W行Json 6s 3G的内存开销
	//差不多是O(n)的效率
	//解析10W行XML 0.9s
	//解析100W行XML 3G内存用完了- -.
	//解析的性能还不错
	{
		ztl::general_parser::generate_file_test::GeneratePureRegexParserFile();
		ztl::general_parser::generate_file_test::GenerateFile();
		ztl::general_parser::parser_test::TestPureRegex();
	}
	//ztl::general_parser::parser_test::TestXml();
	return 0;
}