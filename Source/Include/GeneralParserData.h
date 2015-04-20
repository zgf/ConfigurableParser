#pragma once
#include "stdafx.h"
#include "..\..\Lib\ZTL\ztl_pair_builder.hpp"

namespace ztl
{
	namespace general_parser
	{
		class PDAEdge;
		class PDANode;
		class GeneralTreeNode;
		struct TokenInfo;
		class SymbolManager;
		class ActionWrap;
		class GeneralJumpInfo;
		struct GeneralTableDefine;
		class PushDownAutoMachine;
		struct CreateInfo;
		struct GeneralTokenDefine;
		class ParserSymbol;
		class CreatedNodeResolve;
		enum class ActionType:int;

		PAIR_BUILDER(EdgeInfo, PDAEdge*, edge, bool, rightRecursion);

		class CreateNode
		{
		public:
			CreateNode(const ActionType& _type, int _nodeIndex, int _tokenIndex, ParserSymbol* _symbol);
			CreateNode() noexcept = default;
			~CreateNode() noexcept = default;
			CreateNode(CreateNode&&) noexcept = default;
			CreateNode(const CreateNode&) noexcept = default;
			CreateNode& operator=(CreateNode&&) noexcept = default;
			CreateNode& operator=(const CreateNode&) noexcept = default;
			ActionType CreateNode::GetType()const;
			int CreateNode::GetTokenIndex()const;
			int CreateNode::GetTermIndex()const;
			void CreateNode::SetNodeIndex(int index);
			void SetTokenIndex(int index);
			ParserSymbol* GetSymbol()const;
			friend bool operator==(const CreateNode& left, const CreateNode& right)
			{
				return left.type == right.type&&
					left.nodeIndex == right.nodeIndex &&
					left.tokenIndex == right.tokenIndex &&
					left.symbol == right.symbol;
			}
			friend bool operator!=(const CreateNode& left, const CreateNode& right)
			{
				return !(left == right);
			}
		private:
			ActionType type;//assgin setter create terminate
			int nodeIndex = -1;// generalNodeIndex
			int tokenIndex = -1;// tokenIndex
			ParserSymbol*	symbol = nullptr;//assgin setter create GetInfo
		};
		struct ParserState
		{
		public:
			ParserState() = default;
			~ParserState() noexcept = default;
			ParserState(ParserState&&) = default;
			ParserState(const ParserState&) = default;
			ParserState& operator=(ParserState&&) = default;
			ParserState& operator=(const ParserState&) = default;
			ParserState(int _tokenIndex, int _currentNodeIndex, const wstring& initRule, const EdgeInfo& _edgeInfo, CreatedNodeResolve* _resolve);
			ParserState(int _tokenIndex, int _currentNodeIndex, const wstring& initRule, CreatedNodeResolve* _resolve);
			void SaveEdgeInfo(const EdgeInfo& _edgeInfo)
			{
				this->edgeInfo = _edgeInfo;
			}

			PDAEdge* GetEdge()const
			{
				return edgeInfo.edge;
			}
			bool	IsRightRecursion()const
			{
				return edgeInfo.rightRecursion;
			}

		public:
			wstring								 currentRule;
			vector<PDANode*>					 stateStack;
			vector<wstring>						 rulePathStack;
			vector<CreateNode>					 fieldsList;
			int									 tokenIndex;
			int									 currentNodeIndex;
			EdgeInfo							 edgeInfo;
			CreatedNodeResolve*					 resolve;
		};
	}
}