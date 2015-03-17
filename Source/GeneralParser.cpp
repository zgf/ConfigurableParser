#include "Include\stdafx.h"
#include "Include\GeneralParser.h"
#include "Include\SymbolManager.h"
#include "Include\GeneralPushDownAutoMachine.h"
#include "Include\GeneralJumpTable.h"
#include "Include\GeneralFile.h"
#include "Include\GeneralParser.h"
namespace ztl
{
	namespace general_parser
	{
		GeneralParser::GeneralParser(const vector<TokenInfo>& tokens, const shared_ptr<GeneralTableDefine>& _tableDefine)
			: tokenPool(tokens), treeRoot(nullptr),tableDefine(_tableDefine), table(make_shared<GeneralJumpTable>(nullptr))
		{
		}
		
		void GeneralParser::BuildParser()
		{
			SymbolManager manger(this->tableDefine);
			ValidateGeneratorCoreSemantic(&manger);
			PushDownAutoMachine machine(&manger);
			CreateDPDAGraph(machine);
			GeneralJumpTable jumpTable(&machine);
			CreateJumpTable(jumpTable);
			*this->table = move(jumpTable);
		}
		GeneralTreeNode* GeneralParser::GenerateIsomorphismParserTree()
		{

			return NULL;
		}
	}
}
