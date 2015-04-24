#include "Include\stdafx.h"
#include "Include\GeneralParserBase.h"
#include "Include\SymbolManager.h"
#include "Include\GeneralPushDownAutoMachine.h"
#include "Include\GeneralPushDownMachineData.h"
#include "Include\GeneralTreeNode.h"
#include "Include\ParserSymbol.h"
#include "Include\GeneralLRMachine.h"
namespace ztl
{
	namespace general_parser
	{
		GeneralParserBase::GeneralParserBase(const wstring& _fileName, const shared_ptr<GeneralTableDefine>& _tableDefine) :
			manager(make_shared<SymbolManager>(_tableDefine)),fileName(_fileName)
		{
			
		}

		void GeneralParserBase::BuildParser()
		{
			pools.SetTokenPool(move(ParseToken(fileName)));
			ValidateGeneratorCoreSemantic(manager.get());
			auto machine = make_shared<PushDownAutoMachine>(manager.get());
			CreateDPDAGraph(*machine.get());
			LRMachine = make_shared<GeneralLRMachine>(machine);
			LRMachine->BuildFirstTable();
			LRMachine->BuildLRItems();
			//LRMachine->LogItems(L"LogGeneralLRMchine.txt");
		}
		SymbolManager* GeneralParserBase::GetManager() const
		{
			return manager.get();
		}
		
		
		void GeneralParserBase::SaveHeterogeneousNode(const shared_ptr<void>& node)
		{
			pools.SetHeterogeneousPool(node);
		}
		LRNode*  GeneralParserBase::GetLRMachineStartNode()const
		{
			return LRMachine->GetLRMachineStartNode();
		}

		GeneralLRMachine & GeneralParserBase::GetMachine() const
		{
			// TODO: insert return statement here
			return *LRMachine;
		}
		GeneralTreeNode* GeneralParserBase::GetGeneralTreeRoot() const
		{
			return generalTreeRoot;
		}
		void GeneralParserBase::GenerateIsomorphismParserTree()
		{
			return;
		}
		GeneralTreeNode* GeneralParserBase::MakeTreeNode(ParserSymbol* symbol)
		{
			pools.SetGeneralNodePool(make_shared<GeneralTreeNode>((int) pools.GetGeneralNodePool().size(), symbol));
			return  pools.GetGeneralNodePool().back().get();
		}

		GeneralTreeNode*	GeneralParserBase::CopyTreeNode(GeneralTreeNode* target)
		{
			this->pools.SetGeneralNodePool(make_shared<GeneralTreeNode>());
			*pools.GetGeneralNodePool().back() = *target;
			return pools.GetGeneralNodePool().back().get();
		}

		const GeneralNodePools& GeneralParserBase::GetPools() const
{
			return pools;
			// TODO: insert return statement here
		}
		ParserSymbol * GeneralParserBase::GetRootRuleSymbol() const
		{
			return manager->GetRuleSymbolByName(LRMachine->GetMachine().GetRootRuleName());
		}
		void GeneralParserBase::SetTerminatePool(const shared_ptr<TokenInfo>& infos)
		{
			pools.SetTerminatePool(infos);
		}
		void GeneralParserBase::SetTokenPool(const vector<shared_ptr<TokenInfo>>& tokens)
		{
			pools.SetTokenPool(tokens);
		}
		void GeneralParserBase::SetGeneralNodePool(const shared_ptr<GeneralTreeNode>& node)
		{
			pools.SetGeneralNodePool(node);
		}
		void GeneralParserBase::SetTokenPool(vector<shared_ptr<TokenInfo>>&& tokens)
		{
			pools.SetTokenPool(std::forward<vector<shared_ptr<TokenInfo>>&&>(tokens));
		}
		void GeneralParserBase::SetHeterogeneousPool(const shared_ptr<void>& element)
		{
			pools.SetHeterogeneousPool(element);
		}
		GeneralTreeNode* GeneralParserBase::GetNonTermNodeByIndex(int index) const
		{
			return pools.GetGeneralNodePool()[index].get();
		}
		shared_ptr<TokenInfo> GeneralParserBase::GetTermNodeByIndex(int index) const
		{
			return pools.GetTerminatePool()[index];
		}
		
	
	}
}