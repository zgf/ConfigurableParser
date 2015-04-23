#pragma once
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
		struct GeneralTableDefine;
		class PushDownAutoMachine;
		struct GeneralTokenDefine;
		class ParserSymbol;
		class GeneralLRMachine;
		enum class ActionType:int;
		class LRNode;
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
			
			const vector<shared_ptr<TokenInfo>>& GetTokenPool()const
			{
				return tokenPool;
			}
			
			const vector<shared_ptr<TokenInfo>>& GetTerminatePool()const
			{
				return terminatePool;
			}
			void SetTerminatePool(const shared_ptr<TokenInfo>& infos)
			{
				terminatePool.emplace_back(infos);
			}
			void SetTokenPool(const vector<shared_ptr<TokenInfo>>& tokens)
			{
				tokenPool = tokens;
			}
			void SetTokenPool(vector<shared_ptr<TokenInfo>>&& tokens)
			{
				tokenPool = move(tokens);
			}
			void SetGeneralNodePool(const shared_ptr<GeneralTreeNode>& node)
			{
				generalNodePool.emplace_back(node);
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
		class GeneralParserBase
		{
		public:
			GeneralParserBase(const wstring& fileName, const shared_ptr<GeneralTableDefine>& _tableDefine);

			GeneralParserBase()  = default;
			~GeneralParserBase() noexcept = default;
			GeneralParserBase(GeneralParserBase&&)  = default;
			GeneralParserBase(const GeneralParserBase&)  = default;
			GeneralParserBase& operator=(GeneralParserBase&&)  = default;
			GeneralParserBase& operator=(const GeneralParserBase&)   = default;
		public:
			void SetTerminatePool(const shared_ptr<TokenInfo>& infos);
			void SetTokenPool(const vector<shared_ptr<TokenInfo>>& tokens);
			void SetTokenPool(vector<shared_ptr<TokenInfo>>&& tokens);
			void SetGeneralNodePool(const shared_ptr<GeneralTreeNode>& node);
			void SetHeterogeneousPool(const shared_ptr<void>& element);
			void					BuildParser();
			virtual void GenerateIsomorphismParserTree();
			GeneralTreeNode* GetGeneralTreeRoot() const;
			SymbolManager* GetManager() const;
			void SaveHeterogeneousNode(const shared_ptr<void>& node);
			GeneralTreeNode* GetNonTermNodeByIndex(int index)const;
			shared_ptr<TokenInfo> GetTermNodeByIndex(int index)const;
			GeneralLRMachine & GeneralParserBase::GetMachine() const;
			LRNode*  GetLRMachineStartNode()const;
			const GeneralNodePools& GetPools()const;
			ParserSymbol* GetRootRuleSymbol()const;
		protected:
			GeneralTreeNode*	MakeTreeNode(ParserSymbol* symbol);
			GeneralTreeNode*	CopyTreeNode(GeneralTreeNode*);

			vector<shared_ptr<TokenInfo>> ParseToken(const wstring& fileName);
			
		protected:
			shared_ptr<SymbolManager>			 manager;
			GeneralNodePools					 pools;
			GeneralTreeNode*					 generalTreeRoot;
			shared_ptr<GeneralLRMachine>		 LRMachine;

		};

		shared_ptr<void>	GeneralHeterogeneousParserTree(ztl::general_parser::GeneralParserBase& parser, ztl::general_parser::GeneralTreeNode* root);
		shared_ptr<void>	GeneralHeterogeneousParserTree(ztl::general_parser::GeneralParserBase& parser);
	}
}