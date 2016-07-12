#pragma once
#include "GeneralLRExecutor.h"
namespace ztl
{
	namespace general_parser
	{

		 class GeneralLALRExecutor: public GeneralLRExecutor
		{
		public:
			GeneralLALRExecutor() = delete;
			GeneralLALRExecutor(const shared_ptr<GeneralTableDefine>& _tableDefine);

			virtual ~GeneralLALRExecutor()
			{

			}
			GeneralLALRExecutor(GeneralLALRExecutor&&) = default;
			GeneralLALRExecutor(const GeneralLALRExecutor&) = default;
			GeneralLALRExecutor& operator=(GeneralLALRExecutor&&) = default;
			GeneralLALRExecutor& operator=(const GeneralLALRExecutor&) = default;
		public:
			virtual void GenerateIsomorphismParserTree()override;
			virtual void ClearEnvironment()override;
		private:
			ParserSymbol* GetTokenSymbol(size_t tokenIndex)const;
			void ExcuteEdgeAdditionAction(const pair<PDAEdge*, int>& grammar, GeneralTreeNode* node);
			GeneralTreeNode* ExcuteEndAction(const vector<ActionWrap>& acionts);
			void ExecutableEdgeAddctionAction(const vector<pair<PDAEdge*, int>>&edges, GeneralTreeNode* node);
			bool IsParserFinish(ParserSymbol* node,ParserSymbol*tokenSymbol)const;
		private:
			vector<int>							 treeNodeStack;
			vector<LRNode*>						 LRNodeStack;
		};
	}
}