// ConfigurableParser.cpp : Defines the entry point for the console application.
//

#include "Include\stdafx.h"
#include "Include\SymbolManager.h"
#include "Include\GeneralPushDownAutoMachine.h"
#include "Include\GeneralJumpTable.h"
#include "Include\GeneralFile.h"
#include "Include\GeneralParser.h"
int main()
{
	
	ztl::general_parser::GeneralParser parser(L"ParserDefine.txt"
		,
		ztl::general_parser::BootStrapDefineTable());
	parser.BuildParser();
	parser.GeneralParserTree();
	auto parserResult = parser.GetParserTree();
	ztl::general_parser::CreatReflectionFile(parser.GetManager());
	ztl::general_parser::CreateFile(L"testEBNFCore.cpp",parser.SerializeEBNFCore(parserResult.front().get()));
	return 0;
}