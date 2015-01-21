#pragma once
#include "stdafx.h"
namespace ztl
{
	namespace general_parser
	{
		struct GeneralRuleDefine;
		struct GeneralGrammarTypeDefine;
		class SymbolManager;
		class PushDownAutoMachine;
		struct GeneralTableDefine;
		class PDANode;
		class PDAEdge;
		enum class ActionType
		{
			Using,
			Shfit,
			Reduce,
			Terminate,
			NonTerminate,
			Create,
			Assign,
			Setter,
			//Epsilon,
		};
		class PDAEdge
		{
			friend PushDownAutoMachine;
		public:
			PDAEdge() noexcept = default;
			~PDAEdge() noexcept = default;
			PDAEdge(PDAEdge&&) noexcept = default;
			PDAEdge(const PDAEdge&) noexcept = default;
			PDAEdge& operator=(PDAEdge&&) noexcept = default;
			PDAEdge& operator=(const PDAEdge&) noexcept = default;
			PDAEdge(GeneralGrammarTypeDefine* _grammar, ActionType _action, PDANode* _source,PDANode* _target)noexcept
				:grammar(_grammar),target(_target),source(_source),actions(make_unique<vector<ActionType>>()),mainActionIndex(0)
			{
				actions->emplace_back(_action);
			}
		private:
			GeneralGrammarTypeDefine* grammar;
			PDANode* source;
			PDANode* target;
			int		 mainActionIndex;
			unique_ptr<deque<ActionType>> actions;
		};
		class PDANode
		{
			friend PushDownAutoMachine;
		public:
			PDANode():nexts(make_unique<vector<PDAEdge*>>()),fronts(make_unique<vector<PDAEdge*>>())
			{

			}
			~PDANode() noexcept = default;
			PDANode(PDANode&&) noexcept = default;
			PDANode(const PDANode&) noexcept = default;
			PDANode& operator=(PDANode&&) noexcept = default;
			PDANode& operator=(const PDANode&) noexcept = default;
		private:
			unique_ptr<vector<PDAEdge*>> nexts;
			unique_ptr<vector<PDAEdge*>> fronts;

		public:
	/*		decltype(auto) begin()
			{
				return nexts->begin();
			}
			decltype(auto) end()
			{
				return nexts->end();
			}
			decltype(auto) size()
			{
				return nexts->size();
			}
			decltype(auto) clear()
			{
				return nexts->clear();
			}*/
		};
		class PushDownAutoMachine
		{
			
		public:
			PushDownAutoMachine();
			PushDownAutoMachine(SymbolManager* _manager);
			~PushDownAutoMachine()  = default;
			PushDownAutoMachine(PushDownAutoMachine&&)  = default;
			PushDownAutoMachine(const PushDownAutoMachine&)  = default;
			PushDownAutoMachine& operator=(PushDownAutoMachine&&)  = default;
			PushDownAutoMachine& operator=(const PushDownAutoMachine&)   = default;
		public:
			SymbolManager*				GetSymbolManager()const;
			GeneralTableDefine*			GetTable()const;
			unordered_map<GeneralRuleDefine*, vector<PDANode*>>& GetPDAMap();
			pair<PDANode*, PDANode*>	NewNodePair();
			PDANode*					NewNode();

			void						AddEdge(PDANode* source, PDANode* target, ActionType action, GeneralGrammarTypeDefine* grammar);
			void						AddGeneratePDA(GeneralRuleDefine* rule, PDANode* node);
			pair<PDANode*, PDANode*>	AddSequenceLinkNode(pair<PDANode*,PDANode*>& left,pair<PDANode*,PDANode*>&right);
			pair<PDANode*, PDANode*>	AddLoopLinkNode(PDANode* loopStart, PDANode* loopEnd);
			pair<PDANode*, PDANode*>	AddAlterationLinkNode(pair<PDANode*, PDANode*>& left, pair<PDANode*, PDANode*>&right);
			pair<PDANode*, PDANode*>	AddOptionalLinkNode(PDANode* optionalStart, PDANode* optionalEnd);
			//保留Left节点合并left right
			PDANode*					MergeIndependentNodes(PDANode* left, PDANode* right);
			
			void BackInsertAction(PDAEdge* edge, ActionType type);
			void FrontInsertAction(PDAEdge* edge, ActionType type);
		private:
			PDAEdge* NewEdge(PDANode* source, PDANode* target, ActionType action, GeneralGrammarTypeDefine* grammar);
		private:
			vector<shared_ptr<PDAEdge>> edges;
			vector<shared_ptr<PDANode>> nodes;
			SymbolManager* manager;
			unordered_map<GeneralRuleDefine*, vector<PDANode*>> PDAMap;
		};
		
		void CreateEpsilonPDAGraph(PushDownAutoMachine& machine);
		void LogEpsilonPDAGraph(const wstring& fileName, PushDownAutoMachine& machine);

	}
}