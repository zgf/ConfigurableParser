// ConfigurableParser.cpp : Defines the entry point for the console application.
//

#include "Include\stdafx.h"
#include "Include\SymbolManager.h"
#include "Include\GeneralPushDownAutoMachine.h"

int main()
{

	auto table = ztl::general_parser::BootStrapDefineTable();
	ztl::general_parser::SymbolManager manger(table);
	ztl::general_parser::ValidateGeneratorCoreSemantic(&manger);
	ztl::general_parser::PushDownAutoMachine machine(&manger);
	ztl::general_parser::CreatePDAGraph(machine);
	ztl::general_parser::LogPDAGraph(L"LogPDAGraph.txt",machine);
	ztl::general_parser::MergeGrammarCommonFactor(machine);
	ztl::general_parser::LogPDAGraph(L"LogMegreGraph.txt", machine);

	return 0;
}

