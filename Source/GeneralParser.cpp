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
			: tokenPool(tokens), 
			treeRoot(nullptr),
			tableDefine(_tableDefine),
			jumpTable(nullptr),
			machine(nullptr),
			manager(nullptr)

		{
		}
		
		void GeneralParser::BuildParser()
		{
			manager = make_shared<SymbolManager>(this->tableDefine);
			ValidateGeneratorCoreSemantic(manager.get());
			machine = make_shared<PushDownAutoMachine >(manager.get());
			CreateDPDAGraph(*machine.get());
			jumpTable = make_shared<GeneralJumpTable>(machine.get());
			CreateJumpTable(*jumpTable.get());
		}
		GeneralTreeNode* GeneralParser::GenerateIsomorphismParserTree()
		{
			assert(!this->tokenPool.empty());
			auto rootRule = machine->GetRootRuleName();
			assert(!rootRule.empty());
			this->rulePathStack.emplace_back(rootRule);
			auto currentNodeIndex = jumpTable->GetRootNumber();
			auto tokenIndex = 0;
			while(true)
			{
				auto& currentToken = this->tokenPool[tokenIndex];
				
				auto candidateEdges = jumpTable->GetCachePDAEdgeByTerminate(currentNodeIndex, currentToken.tag);
				if (candidateEdges == nullptr||candidateEdges->empty())
				{
					ztl_exception(L"run error!can't match "+currentToken.tag+L" lex is "+currentToken.content);
				}
				else if(candidateEdges->size()==1)
				{

				}
				else
				{
					assert(candidateEdges->size() > 1);
					vector<PDAEdge*> result;
					for (auto&&iter : *candidateEdges)
					{
						
					}
				}
			}
			return nullptr;
		}
	}
}
