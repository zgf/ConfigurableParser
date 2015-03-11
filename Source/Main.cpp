// ConfigurableParser.cpp : Defines the entry point for the console application.
//

#include "Include\stdafx.h"
#include "Include\SymbolManager.h"
#include "Include\GeneralPushDownAutoMachine.h"
#include "Include\GeneralJumpTable.h"
#include "Include\GeneralFile.h"
#include "Include\GeneralLexer.hpp"
int main()
{
	auto table = ztl::general_parser::BootStrapDefineTable();
	ztl::general_parser::SymbolManager manger(table);
	ztl::general_parser::ValidateGeneratorCoreSemantic(&manger);
	ztl::general_parser::PushDownAutoMachine machine(&manger);
	ztl::general_parser::CreateDPDAGraph(machine);
	ztl::general_parser::GeneralJumpTable jumpTable(&machine);
	ztl::general_parser::CreateJumpTable(jumpTable);
//	HelpLogJumpTable(L"LogJumpTable_MergeNoTermGraphTable.txt", jumpTable);
//	CreateLexerFile(L"testLex.hpp", &manger);
//	CreateNodeDefineFile(L"Test.cpp", &manger);
	ztl::general_parser::Parse(L"ParserDefine.txt");
	return 0;
}

