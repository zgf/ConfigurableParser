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
void TestJson()
{
	ztl::general_parser::GeneralLALRParser LALRParser(L"TestJson.txt", ztl::json::BootStrapDefineTable());
	LALRParser.BuildParser();
	LALRParser.GenerateIsomorphismParserTree();
	auto tree = ztl::json::GeneralHeterogeneousParserTree(LALRParser);
}
void TestXml()
{
	ztl::general_parser::GeneralLALRParser LALRParser(L"TestXml.txt", ztl::xml::BootStrapDefineTable());
	LALRParser.BuildParser();
	LALRParser.GenerateIsomorphismParserTree();
	auto tree = ztl::xml::GeneralHeterogeneousParserTree(LALRParser);
}
void TestParseGeneralDefine()
{
	ztl::general_parser::GeneralLALRParser LALRParser(L"ParserDefine.txt", ztl::general_parser::BootStrapDefineTable());
	LALRParser.BuildParser();
	LALRParser.GenerateIsomorphismParserTree();
	auto tree = ztl::general_parser::GeneralHeterogeneousParserTree(LALRParser);
}

void TestParseXMLDefine()
{
	ztl::general_parser::GeneralLALRParser LALRParser(L"Xml.ParserDefine.txt", ztl::general_parser::BootStrapDefineTable());
	LALRParser.BuildParser();
	LALRParser.GenerateIsomorphismParserTree();
	auto tree = ztl::general_parser::GeneralHeterogeneousParserTree(LALRParser);
}

void TestParseJsonDefine()
{
	ztl::general_parser::GeneralLALRParser LALRParser(L"Json.ParserDefine.txt", ztl::general_parser::BootStrapDefineTable());
	LALRParser.BuildParser();
	LALRParser.GenerateIsomorphismParserTree();
	auto tree = ztl::general_parser::GeneralHeterogeneousParserTree(LALRParser);
}
void GenerateGeneralParserFile()
{
	ztl::general_parser::GeneralParserFile parserGen(make_shared<ztl::general_parser::GeneralLALRParser>(L"ParserDefine.txt", ztl::general_parser::BootStrapDefineTable()));
	parserGen.GenerateSpecialParserFile();
}
void GenerateXmlParserFile()
{
	ztl::general_parser::GeneralParserFile parserGen(make_shared<ztl::general_parser::GeneralLALRParser>(L"Xml.ParserDefine.txt", ztl::general_parser::BootStrapDefineTable()));
	parserGen.GenerateSpecialParserFile();
}
void GenerateJsonParserFile()
{
	ztl::general_parser::GeneralParserFile parserGen(make_shared<ztl::general_parser::GeneralLALRParser>(L"Json.ParserDefine.txt", ztl::general_parser::BootStrapDefineTable()));
	parserGen.GenerateSpecialParserFile();
}
int main()
{
//	GenerateGeneralParserFile();
//	GenerateXmlParserFile();
//	GenerateJsonParserFile();
	TestJson();
	TestXml();
	//TestParseXMLDefine();
	//TestParseJsonDefine();
	//TestParseGeneralDefine();
	return 0;
}