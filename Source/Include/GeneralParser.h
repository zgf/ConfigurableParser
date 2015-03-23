#pragma once
#include "stdafx.h"
namespace ztl
{
	namespace general_parser
	{
		class PDAEdge;
		class GeneralTreeNode;
		struct TokenInfo;
		class SymbolManager;
		class ActionWrap;
		class GeneralJumpTable;
		struct GeneralTableDefine;
		class PushDownAutoMachine;
		struct CreateInfo;
		class GeneralParser
		{
		public:
			GeneralParser() = delete;
			GeneralParser(const vector<shared_ptr<TokenInfo>>& tokens,const shared_ptr<GeneralTableDefine>& _tableDefine);
			~GeneralParser() noexcept = default;
			GeneralParser(GeneralParser&&)  = default;
			GeneralParser(const GeneralParser&)  = default;
			GeneralParser& operator=(GeneralParser&&)  = default;
			GeneralParser& operator=(const GeneralParser&)   = default;
		public:
			void				BuildParser();
			void				GenerateIsomorphismParserTree();
			shared_ptr<void>	GeneralHeterogeneousParserTree();
			shared_ptr<void>	GeneralParserTree();
		private:
			PDAEdge*			EdgeResolve(int number, int tokenIndex);
			PDAEdge *			TerminateResolve(int number, int tokenIndex);
			PDAEdge *			RuleResolve(vector<PDAEdge*>* edges, int tokenIndex);
			PDAEdge*			CreateNodeResolve(const vector<PDAEdge*>& edges, int tokenIndex);
			void				ExecuteEdgeActions(PDAEdge* edge, int tokenIndex);
			wstring				GetRulePathInfo()const;
			wstring				GetCreatNodeStackInfo()const;
			pair<bool, int>		CheckCreateNodeRequire(int createStackIndex, const GeneralTreeNode& node,vector<wstring>* exclude);

			bool				CheckCreateNodeRequires(const vector<CreateInfo>& requires,vector<wstring>* exclude);

			GeneralTreeNode*	MakeTreeNode(const wstring& nodeName);
			GeneralTreeNode*	MakeEmptyTreeNode();
			wstring				GetParserInfo(int tokenIndex)const;
			unordered_map<wstring, GeneralTreeNode> InitTreeNodeMap();
			unordered_map<wstring, wstring> InitDeriveToBaseMap();
			bool				IsDeriveRelateion(const wstring& base, const wstring& derive);
		private:
			vector<shared_ptr<GeneralTreeNode>>  nodePool;
			vector<shared_ptr<TokenInfo>>		 tokenPool;
			//Setter的value.assign的终结符号,Setter的TokenInfo tag==Setter -1,-1,
			vector<shared_ptr<TokenInfo>>		 terminatePool;
			vector<GeneralTreeNode*>			 createdNodeStack;
			vector<wstring>						 rulePathStack;
			GeneralTreeNode*					 treeRoot;

			shared_ptr<PushDownAutoMachine>		 machine;
			shared_ptr<GeneralJumpTable>		 jumpTable;
			shared_ptr<SymbolManager>			 manager;
			shared_ptr<GeneralTableDefine>		 tableDefine;

		};
		vector<shared_ptr<TokenInfo>>	 ParseToken(const wstring& fileName);
	}
}