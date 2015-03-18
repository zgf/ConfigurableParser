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
			GeneralTreeNode* node = nullptr;
			while(tokenIndex != tokenPool.size())
			{
				auto&& currentToken = this->tokenPool[tokenIndex];
				auto edge = EdgeResolve(currentNodeIndex, currentToken);
				node = ExecuteEdgeActions(edge, node);
				++tokenIndex;
				currentNodeIndex = edge->GetTarget()->GetNumber();
			}
			return node;
		}
		PDAEdge* GeneralParser::EdgeResolve(int number, const TokenInfo& token)
		{
			return TerminateResolve(number,token);
		}
		PDAEdge * GeneralParser::TerminateResolve(int number, const TokenInfo& token)
		{
			auto edges = jumpTable->GetCachePDAEdgeByTerminate(number, token.tag);
			return RuleResolve(edges);
		}
		PDAEdge * GeneralParser::RuleResolve(vector<PDAEdge*>* edges)
		{
			vector<PDAEdge*> result;
			
			
		
			return CreateNodeResolve(result);
		}
		PDAEdge * GeneralParser::CreateNodeResolve(const vector<PDAEdge*>& edges)
		{

			return nullptr;
		}
		GeneralTreeNode* GeneralParser::ExecuteEdgeActions(PDAEdge* edge, GeneralTreeNode* node)
		{
			return nullptr;
		}
	}
}