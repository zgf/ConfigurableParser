// ConfigurableParser.cpp : Defines the entry point for the console application.
//

#include "Include\stdafx.h"
#include "Include\SymbolManager.h"
#include "Include\GeneralPushDownAutoMachine.h"
#include "Include\GeneralJumpInfo.h"
#include "Include\GeneralParser.h"
#include "Include\GeneralParserFile.h"
#include "GenerateFile\Json\JsonParser.h"
#include "GenerateFile\Xml\XmlParser.h"
void TestJson()
{
	ztl::general_parser::GeneralParser parser(L"TestJson.txt", ztl::json::BootStrapDefineTable());
	parser.BuildParser();
	LogGraphInfo(L"LogJsonInfo.txt", parser.GetMachine());
	parser.GenerateIsomorphismParserTree();

	auto tree = ztl::json::GeneralHeterogeneousParserTree(parser);
}
void TestXml()
{
	ztl::general_parser::GeneralParser parser(L"TestXml.txt", ztl::xml::BootStrapDefineTable());
	parser.BuildParser();
	LogGraphInfo(L"LogXmlInfo.txt", parser.GetMachine());
	parser.GenerateIsomorphismParserTree();

	auto tree = ztl::xml::GeneralHeterogeneousParserTree(parser);
}

void GenerateGeneralParserFile()
{
	ztl::general_parser::GeneralParserFile parserGen(L"ParserDefine.txt", ztl::general_parser::BootStrapDefineTable());

	parserGen.GenerateSpecialParserFile();
}
void GenerateXmlParserFile()
{
	ztl::general_parser::GeneralParserFile parserGen(L"Xml.ParserDefine.txt", ztl::general_parser::BootStrapDefineTable());
	parserGen.GenerateSpecialParserFile();
}
void GenerateJsonParserFile()
{
	ztl::general_parser::GeneralParserFile parserGen(L"Json.ParserDefine.txt", ztl::general_parser::BootStrapDefineTable());
	parserGen.GenerateSpecialParserFile();
}
int main()
{
	//GenerateGeneralParserFile();
	TestJson();
	TestXml();
	return 0;
}