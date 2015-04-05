#pragma once
#include "stdafx.h"
#include "..\..\Lib\ZTL\ztl_pair_builder.hpp"
namespace ztl
{
	namespace general_parser
	{
		class PDAEdge;
		class GeneralTreeNode;
		struct TokenInfo;
		class SymbolManager;
		class ActionWrap;
		class GeneralJumpInfo;
		struct GeneralTableDefine;
		class PushDownAutoMachine;
		struct CreateInfo;
		struct GeneralTokenDefine;
		PAIR_BUILDER(EdgeInfo, PDAEdge*, edge, bool, rightRecursion);

		struct ParserState
		{
		public:
			ParserState() = default;
			~ParserState() noexcept = default;
			ParserState(ParserState&&) = default;
			ParserState(const ParserState&) = default;
			ParserState& operator=(ParserState&&) = default;
			ParserState& operator=(const ParserState&) = default;
			ParserState(int _tokenIndex, int _currentNodeIndex, const wstring& initRule, GeneralTreeNode* initCreateNode, const EdgeInfo& _edgeInfo) :
				tokenIndex(_tokenIndex), currentNodeIndex(_currentNodeIndex), edgeInfo(_edgeInfo)
			{
				createdNodeStack.emplace_back(initCreateNode);
				rulePathStack.emplace_back(initRule);
			}
			ParserState(int _tokenIndex, int _currentNodeIndex, const wstring& initRule, GeneralTreeNode* initCreateNode)
				:tokenIndex(_tokenIndex), currentNodeIndex(_currentNodeIndex)
			{
				createdNodeStack.emplace_back(initCreateNode);
				rulePathStack.emplace_back(initRule);
			}
			void SaveEdgeInfo(const EdgeInfo& _edgeInfo)
			{
				this->edgeInfo = _edgeInfo;
			}
			void SaveNodeStackInFo(const vector<GeneralTreeNode*>& info)
			{
				createdNodeStack = info;
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
			vector<GeneralTreeNode*>			 createdNodeStack;
			vector<wstring>						 rulePathStack;
			int									 tokenIndex;
			int									 currentNodeIndex;
			EdgeInfo							 edgeInfo;
		};
		class GeneralNodePools
		{
		public:
			GeneralNodePools() = default;
			~GeneralNodePools() noexcept = default;
			GeneralNodePools(GeneralNodePools&&) = default;
			GeneralNodePools(const GeneralNodePools&) = default;
			GeneralNodePools& operator=(GeneralNodePools&&) = default;
			GeneralNodePools& operator=(const GeneralNodePools&) = default;

		public:
			const vector<shared_ptr<GeneralTreeNode>>& GetGeneralNodePool()const
			{
				return generalNodePool;
			}
			void SetGeneralNodePool(const shared_ptr<GeneralTreeNode>& node)
			{
				generalNodePool.emplace_back(node);
			}
			const vector<shared_ptr<TokenInfo>>& GetTokenPool()const
			{
				return tokenPool;
			}
			void SetTokenPool(const vector<shared_ptr<TokenInfo>>& tokens)
			{
				tokenPool = tokens;
			}
			void SetTokenPool(vector<shared_ptr<TokenInfo>>&& tokens)
			{
				tokenPool = move(tokens);
			}
			const vector<shared_ptr<TokenInfo>>& GetTerminatePool()const
			{
				return terminatePool;
			}
			void SetTerminatePool(const shared_ptr<TokenInfo>& infos)
			{
				terminatePool.emplace_back(infos);
			}

			void SetHeterogeneousPool(const shared_ptr<void>& element)
			{
				heterogeneousPool.emplace_back(element);
			}
		private:
			vector<shared_ptr<GeneralTreeNode>>  generalNodePool;
			vector<shared_ptr<TokenInfo>>		 tokenPool;
			//Setter的value.assign的终结符号,Setter的TokenInfo tag==Setter -1,-1,
			vector<shared_ptr<TokenInfo>>		 terminatePool;
			vector<shared_ptr<void>>			 heterogeneousPool;
		};
		class GeneralParser
		{
		public:
			GeneralParser() = delete;
			GeneralParser(const wstring& fileName, const shared_ptr<GeneralTableDefine>& _tableDefine);
			~GeneralParser() noexcept = default;
			GeneralParser(GeneralParser&&) = default;
			GeneralParser(const GeneralParser&) = default;
			GeneralParser& operator=(GeneralParser&&) = default;
			GeneralParser& operator=(const GeneralParser&) = default;
		public:
			void				BuildParser();
			vector<GeneralTreeNode*> GenerateIsomorphismParserTree();
			bool IsCorrectNode(GeneralTreeNode& node, const wstring& value)const;

			GeneralTreeNode* GetGeneralTreeRoot() const;
			SymbolManager* GetManager() const;
			void SaveHeterogeneousNode(const shared_ptr<void>& node);
			GeneralTreeNode* GetNonTermNodeByIndex(int index)const;
			shared_ptr<TokenInfo> GetTermNodeByIndex(int index)const;
			PushDownAutoMachine&  GetMachine()const;

		private:
			vector<EdgeInfo> EdgeResolve(ParserState& state);
			vector<EdgeInfo> TerminateResolve(ParserState& state);
			vector<EdgeInfo>RuleResolve(vector<PDAEdge*>* edges, ParserState& state);
			void HandleRightRecursionEdge(ParserState& state);
			void HandleRightRecursionEdge(PDAEdge* edge, vector<wstring>& rulePathStack, vector<GeneralTreeNode*>& createdNodeStack, bool isRightRecursionEdge);
			vector<EdgeInfo> CreateNodeResolve(const vector<EdgeInfo>& edges, ParserState& state);
			pair<bool, EdgeInfo> CreateNodeResolve(const EdgeInfo& iter, const vector<GeneralTreeNode*>& createdNodeStack);

			void ExecuteEdgeActions(ParserState& state);
			wstring GetRulePathInfo(ParserState& state)const;
			wstring GetCreatNodeStackInfo(ParserState& state)const;
			GeneralTreeNode*	MakeTreeNode(const wstring& nodeName);
			GeneralTreeNode*	MakeEmptyTreeNode();
			GeneralTreeNode*	CopyTreeNode(GeneralTreeNode*);
			void	SaveEdge(deque<ParserState>& states, const vector<EdgeInfo>& edges);
			wstring GetParserInfo(ParserState& state)const;
			unordered_map<wstring, shared_ptr<GeneralTreeNode>> InitTreeNodeMap();
			unordered_map<wstring, vector<wstring>> InintChoiceFiledMap();
			vector<GeneralTreeNode*> SaveCurrentStack(const vector<GeneralTreeNode*>& current);

			vector<shared_ptr<TokenInfo>> ParseToken(const wstring& fileName);
			void CheckParserResultConvergence();
		private:
			GeneralNodePools					 pools;
			GeneralTreeNode*					 generalTreeRoot;
			deque<ParserState>					 parserStates;

			shared_ptr<PushDownAutoMachine>		 machine;
			shared_ptr<GeneralJumpInfo>			jumpInfos;
			shared_ptr<SymbolManager>			 manager;
			unordered_map<wstring, shared_ptr<GeneralTreeNode>> wstringToTreeNodeMap;
		};
		shared_ptr<void>	GeneralHeterogeneousParserTree(ztl::general_parser::GeneralParser& parser, ztl::general_parser::GeneralTreeNode* root);
		shared_ptr<void>	GeneralHeterogeneousParserTree(ztl::general_parser::GeneralParser& parser);
	}
}