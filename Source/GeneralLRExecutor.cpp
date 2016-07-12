#include "Include\stdafx.h"
#include "Include\GeneralLRExecutor.h"
#include "Include\SymbolManager.h"
#include "Include\GeneralGrammarBuilder.h"
#include "Include\GeneralGrammarBuilderData.h"
#include "Include\GeneralTreeNode.h"
#include "Include\ParserSymbol.h"
#include "Include\GeneralLRBuilder.h"
namespace ztl
{
	namespace general_parser
	{
		GeneralLRExecutor::GeneralLRExecutor(const shared_ptr<GeneralTableDefine>& _tableDefine) :
			manager(make_shared<SymbolManager>(_tableDefine))
		{
		}

		void GeneralLRExecutor::SetTokenPool(const wstring& fileName)
		{
			assert(!fileName.empty());
			pools.SetTokenPool(ParseToken(fileName));
		}
	
		void GeneralLRExecutor::BuildParser()
		{
			SemanticAnalysis(manager.get());
			auto machine = make_shared<GrammarBuilder>(manager.get());
			CreateDPDAGraph(*machine.get());
			LRMachine = make_shared<GeneralLRBuilder>(machine);
			LRMachine->CalculateFirstSet();
			LRMachine->BuildLRItems();
			LRMachine->PropagateLRItemFollowSymbol();
		}
		SymbolManager* GeneralLRExecutor::GetManager() const
		{
			return manager.get();
		}

		void GeneralLRExecutor::SaveHeterogeneousNode(const shared_ptr<void>& node)
		{
			pools.SetHeterogeneousPool(node);
		}
		LRNode*  GeneralLRExecutor::GetLRMachineStartNode()const
		{
			return LRMachine->GetLRMachineStartNode();
		}
		void GeneralLRExecutor::LALRConfilctDetection()
		{
			LRMachine->LALRConfilctDetection();
		}
		GeneralLRBuilder & GeneralLRExecutor::GetMachine() const
		{
			// TODO: insert return statement here
			return *LRMachine;
		}
		GeneralTreeNode* GeneralLRExecutor::GetGeneralTreeRoot() const
		{
			return generalTreeRoot;
		}
		void GeneralLRExecutor::GenerateIsomorphismParserTree()
		{
			return;
		}
		void GeneralLRExecutor::ClearEnvironment()
		{
			generalTreeRoot = nullptr;
			pools.ClearPools();
		}
		GeneralTreeNode* GeneralLRExecutor::MakeTreeNode(ParserSymbol* symbol)
		{
			pools.SetGeneralNodePool(make_shared<GeneralTreeNode>((int) pools.GetGeneralNodePool().size(), symbol));
			return  pools.GetGeneralNodePool().back().get();
		}

		GeneralTreeNode*	GeneralLRExecutor::CopyTreeNode(GeneralTreeNode* target)
		{
			this->pools.SetGeneralNodePool(make_shared<GeneralTreeNode>());
			*pools.GetGeneralNodePool().back() = *target;
			return pools.GetGeneralNodePool().back().get();
		}

		const GeneralNodePools& GeneralLRExecutor::GetPools() const
		{
			return pools;
			// TODO: insert return statement here
		}
		ParserSymbol * GeneralLRExecutor::GetRootRuleSymbol() const
		{
			return manager->GetRuleSymbolByName(LRMachine->GetMachine().GetRootRuleName());
		}

		void GeneralLRExecutor::SetTokenPool(const vector<shared_ptr<TokenInfo>>& tokens)
		{
			pools.SetTokenPool(tokens);
		}
		void GeneralLRExecutor::SetTokenPool(const shared_ptr<TokenInfo>& tokens)
		{
			pools.SetTokenPool(tokens);
		}
		void GeneralLRExecutor::SetGeneralNodePool(const shared_ptr<GeneralTreeNode>& node)
		{
			pools.SetGeneralNodePool(node);
		}
		void GeneralLRExecutor::SetTokenPool(vector<shared_ptr<TokenInfo>>&& tokens)
		{
			pools.SetTokenPool(std::forward<vector<shared_ptr<TokenInfo>>&&>(tokens));
		}

		void GeneralLRExecutor::SetHeterogeneousPool(const shared_ptr<void>& element)
		{
			pools.SetHeterogeneousPool(element);
		}
		GeneralTreeNode* GeneralLRExecutor::GetNonTermNodeByIndex(int index) const
		{
			return pools.GetGeneralNodePool()[index].get();
		}
		shared_ptr<TokenInfo> GeneralLRExecutor::GetTermNodeByIndex(int index) const
		{
			return pools.GetTokenPool()[index];
		}
	}
}