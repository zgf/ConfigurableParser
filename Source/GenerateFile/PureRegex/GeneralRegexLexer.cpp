#pragma once
#include "Include\stdafx.h"
#include "GeneralRegexLexer.h"
#include "PureRegexAutoMachineData.h"
#include "DFATableBuilder.h"
namespace ztl
{
	namespace pure_regex
	{

			GeneralLexer::GeneralLexer(const vector<shared_ptr<TokenPacket>>& infos)
				:tokens(make_shared<TokenList>(infos)),
				roots(make_shared<vector<shared_ptr<Alternate>>>(ParsingAllTokensToAst(infos))),
				table(BuildCharsetMapTable(*roots)), 
				builder(make_shared<DFATableBuilder>(roots.get(),tokens.get(),table.get()))
			{
				
			}
	}
}
