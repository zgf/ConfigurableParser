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
#include "Include\GeneralLRMachine.h"
#include <windows.h>     

void TestJson()
{
	ztl::general_parser::GeneralLALRParser LALRParser( ztl::json::BootStrapDefineTable());
	LALRParser.BuildParser(L"TestJson.txt");
	LARGE_INTEGER BegainTime;
	LARGE_INTEGER EndTime;
	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&BegainTime);

	LALRParser.GenerateIsomorphismParserTree();

	auto tree = ztl::json::GeneralHeterogeneousParserTree(LALRParser);
	QueryPerformanceCounter(&EndTime);

	//输出运行时间（单位：s）    
	wcout << (double) (EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart << endl;
}
void TestXml()
{
	ztl::general_parser::GeneralLALRParser LALRParser( ztl::xml::BootStrapDefineTable());
	LALRParser.BuildParser(L"TestXml.txt");
	LARGE_INTEGER BegainTime;
	LARGE_INTEGER EndTime;
	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&BegainTime);

	LALRParser.GenerateIsomorphismParserTree();

	auto tree = ztl::xml::GeneralHeterogeneousParserTree(LALRParser);
	QueryPerformanceCounter(&EndTime);

	//输出运行时间（单位：s）    
	wcout << (double) (EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart << endl;
}
void TestParseGeneralDefine()
{
	ztl::general_parser::GeneralLALRParser LALRParser( ztl::general_parser::BootStrapDefineTable());
	LALRParser.BuildParser(L"ParserDefine.txt");
	LALRParser.GenerateIsomorphismParserTree();
	auto tree = ztl::general_parser::GeneralHeterogeneousParserTree(LALRParser);
}

void TestParseXMLDefine()
{
	ztl::general_parser::GeneralLALRParser LALRParser( ztl::general_parser::BootStrapDefineTable());
	LALRParser.BuildParser(L"Xml.ParserDefine.txt");
	LALRParser.GenerateIsomorphismParserTree();
	auto tree = ztl::general_parser::GeneralHeterogeneousParserTree(LALRParser);
}

void TestParseJsonDefine()
{
	ztl::general_parser::GeneralLALRParser LALRParser(ztl::general_parser::BootStrapDefineTable());
	LALRParser.BuildParser(L"Json.ParserDefine.txt");
	LALRParser.GenerateIsomorphismParserTree();
	auto tree = ztl::general_parser::GeneralHeterogeneousParserTree(LALRParser);
}
void GenerateGeneralParserFile()
{
	ztl::general_parser::GeneralParserFile parserGen(L"ParserDefine.txt", make_shared<ztl::general_parser::GeneralLALRParser>( ztl::general_parser::BootStrapDefineTable()));
	parserGen.GenerateSpecialParserFile();
}
void GenerateXmlParserFile()
{
	ztl::general_parser::GeneralParserFile parserGen(L"Xml.ParserDefine.txt", make_shared<ztl::general_parser::GeneralLALRParser>(ztl::general_parser::BootStrapDefineTable()));
	parserGen.GenerateSpecialParserFile();
}
void GenerateJsonParserFile()
{
	ztl::general_parser::GeneralParserFile parserGen(L"Json.ParserDefine.txt", make_shared<ztl::general_parser::GeneralLALRParser>(ztl::general_parser::BootStrapDefineTable()));
	parserGen.GenerateSpecialParserFile();
}
void GeneratePureRegexParserFile()
{

	ztl::general_parser::GeneralParserFile parserGen(L"PureRegex.ParserDefine.txt", make_shared<ztl::general_parser::GeneralLALRParser>(ztl::general_parser::BootStrapDefineTable()));
	parserGen.GenerateSpecialParserFile();
}
void GenerateFile()
{
	LARGE_INTEGER BegainTime;
	LARGE_INTEGER EndTime;
	LARGE_INTEGER Frequency;
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&BegainTime);
	GenerateGeneralParserFile();
	GenerateXmlParserFile();
	GenerateJsonParserFile();
	QueryPerformanceCounter(&EndTime);

	//输出运行时间（单位：s）    
	wcout << (double) (EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart << endl;
}
int main()
{
	//GenerateFile();
	GeneratePureRegexParserFile();
	//内部调用的regex库 debug模式下拆解token太慢- -debug下性能分析占了90%+的时间- -
	//release一启动渣本200ms全部执行完了
	//GenerateFile();
	//解析10W行Json 0.5s
	//解析100W行Json 6s 3G的内存开销
	//差不多是O(n)的效率
	//解析10W行XML 0.9s
	//解析100W行XML 3G内存用完了- -.
	//解析的性能还不错
	//TestJson();
	//TestXml();
	return 0;
}