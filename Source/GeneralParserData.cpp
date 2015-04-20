#include "Include/stdafx.h"
#include "Include/GeneralParserData.h"
#include "Include/GeneralPushDownMachineData.h"
#include "Include/CreatedNodeResolve.h"
namespace ztl
{
	namespace general_parser
	{
		ParserState::ParserState(int _tokenIndex, int _currentNodeIndex, const wstring& initRule, const EdgeInfo& _edgeInfo, CreatedNodeResolve* _resolve) :
			tokenIndex(_tokenIndex), currentNodeIndex(_currentNodeIndex), edgeInfo(_edgeInfo), resolve(_resolve)
		{
			currentRule = initRule;
			rulePathStack.emplace_back(initRule);
			stateStack.emplace_back(resolve->GetRuleDFA(initRule));
		}
		ParserState::ParserState(int _tokenIndex, int _currentNodeIndex, const wstring& initRule, CreatedNodeResolve* _resolve)
			:tokenIndex(_tokenIndex), currentNodeIndex(_currentNodeIndex), resolve(_resolve)
		{
			currentRule = initRule;
			stateStack.emplace_back(resolve->GetRuleDFA(initRule));
			rulePathStack.emplace_back(initRule);

		}
		CreateNode::CreateNode(const ActionType& _type, int _nodeIndex, int _tokenIndex, ParserSymbol* _symbol)
			:type(_type), nodeIndex(_nodeIndex), symbol(_symbol), tokenIndex(_tokenIndex)
		{
		}
		ActionType CreateNode::GetType()const
		{
			return type;
		}
		int CreateNode::GetTokenIndex()const
		{
			assert(type == ActionType::Terminate || type == ActionType::Assign || type == ActionType::Setter);
			return tokenIndex;
		}
		int CreateNode::GetTermIndex()const
		{
			assert(type == ActionType::Assign || type == ActionType::Create);
			return nodeIndex;
		}
		void CreateNode::SetNodeIndex(int index)
		{
			assert(type == ActionType::Assign);
			nodeIndex = index;
		}
		void CreateNode::SetTokenIndex(int index)
		{
			assert(type == ActionType::Assign || type == ActionType::Setter);
			tokenIndex = index;
		}
		ParserSymbol * CreateNode::GetSymbol() const
		{
			return symbol;
		}
	}
}