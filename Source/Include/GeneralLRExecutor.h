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
		class GrammarBuilder;
		struct GeneralTokenDefine;
		class ParserSymbol;
		class GeneralLRBuilder;
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

			void SetTokenPool(const shared_ptr<TokenInfo>& token)
			{
				tokenPool.emplace_back(token);
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
			void ClearPools()
			{
				generalNodePool.clear();
				tokenPool.clear();
				heterogeneousPool.clear();
			}
		private:
			vector<shared_ptr<GeneralTreeNode>>  generalNodePool;
			vector<shared_ptr<TokenInfo>>		 tokenPool;
			//Setter��value.assign���ս����,Setter��TokenInfo tag==Setter -1,-1,
			vector<shared_ptr<void>>			 heterogeneousPool;
		};
		class GeneralLRExecutor
		{
		public:
			GeneralLRExecutor(const shared_ptr<GeneralTableDefine>& _tableDefine);
			GeneralLRExecutor()  = default;
			virtual ~GeneralLRExecutor()
			{

			}
			GeneralLRExecutor(GeneralLRExecutor&&)  = default;
			GeneralLRExecutor(const GeneralLRExecutor&)  = default;
			GeneralLRExecutor& operator=(GeneralLRExecutor&&)  = default;
			GeneralLRExecutor& operator=(const GeneralLRExecutor&)   = default;
		public:
			virtual void GenerateIsomorphismParserTree();
			virtual void ClearEnvironment();
			void SetTokenPool(const wstring& fileName);
			void SetTokenPool(const vector<shared_ptr<TokenInfo>>& tokens);
			void SetTokenPool(vector<shared_ptr<TokenInfo>>&& tokens);
			void SetTokenPool(const shared_ptr<TokenInfo>& token);
			void SetGeneralNodePool(const shared_ptr<GeneralTreeNode>& node);
			void SetHeterogeneousPool(const shared_ptr<void>& element);
			
			void LALRConfilctDetection();
		
			GeneralTreeNode* GetGeneralTreeRoot() const;
			SymbolManager* GetManager() const;
			void SaveHeterogeneousNode(const shared_ptr<void>& node);
			GeneralTreeNode* GetNonTermNodeByIndex(int index)const;
			shared_ptr<TokenInfo> GetTermNodeByIndex(int index)const;
			GeneralLRBuilder & GeneralLRExecutor::GetMachine() const;
			LRNode*  GetLRMachineStartNode()const;
			const GeneralNodePools& GetPools()const;
			ParserSymbol* GetRootRuleSymbol()const;
			void BuildParser();
		protected:
			GeneralTreeNode*	MakeTreeNode(ParserSymbol* symbol);
			GeneralTreeNode*	CopyTreeNode(GeneralTreeNode*);

			vector<shared_ptr<TokenInfo>> ParseToken(const wstring& fileName);

			


		protected:
			shared_ptr<SymbolManager>			 manager;
			GeneralNodePools					 pools;
			GeneralTreeNode*					 generalTreeRoot;
			shared_ptr<GeneralLRBuilder>		 LRMachine;

		};

		shared_ptr<void>	GenerateHeterogeneousParserTree(ztl::general_parser::GeneralLRExecutor& parser, ztl::general_parser::GeneralTreeNode* root);
		shared_ptr<void>	GenerateHeterogeneousParserTree(ztl::general_parser::GeneralLRExecutor& parser);
	}
}