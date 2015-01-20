// ConfigurableParser.cpp : Defines the entry point for the console application.
//

#include "Include\stdafx.h"
#include "Include\SymbolManager.h"


int main()
{

	auto table = ztl::general_parser::BootStrapDefineTable();
	ztl::general_parser::ValidateGeneratorCoreSemantic(table);

	return 0;
}

