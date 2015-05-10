#pragma once
#include "../Source/GenerateFile/PureRegex/PureRegexLexer.h"
#include "../Source/GenerateFile/PureRegex/GeneralLexer.h"
#include "Include\GeneralTableWriter.h"

namespace ztl
{
	namespace general_parser
	{
		namespace test_general_lexer
		{
			void TestCharMapTable()
			{
				auto writer = ztl::general_parser::GeneralTokenWriter().Token(L"CLASS", LR"(class)").Token(L"PRECLOSE", LR"(\))");
				auto result = ztl::pure_regex::ParsingAllTokensToAst(writer.tokens);
				auto table = ztl::pure_regex::BuildCharsetMapTable(result);
				int  a = 0;
			}
			void TestGeneralLexer()
			{
				auto table = ztl::general_parser::BootStrapDefineTable();
				ztl::pure_regex::GeneralLexer lexer(table->tokens);

			}
		}
	}
}




