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
		};
		class ActionWrap
		{
			ActionType action;
			GeneralGrammarTypeDefine* grammar;
			wstring name;//setter key using ruleNmae create className nonterm ruleName term tokenName assgin fieldName
			wstring value;//setter value assgin ruleName
		public:
			ActionWrap() = default;
			ActionWrap(GeneralGrammarTypeDefine* _grammar, ActionType _action, const wstring& _name, const wstring& _value)
				:grammar(_grammar), action(_action), name(_name), value(_value)
			{
				grammar = nullptr;
			}
			~ActionWrap() = default;
			/*GeneralGrammarTypeDefine*const GetGrammar()const
			{
				return grammar;
			}*/
			ActionType GetActionType()const
			{
				return action;
			}
			wstring GetName()const
			{
				return name;
			}
			wstring GetValue()const
			{
				return value;
			}
			bool operator==(const ActionWrap& target)const
			{
				return target.action == action&& 
					target.name == name&&
					target.value == value;
			}
			bool operator!=(const ActionWrap& target)const
			{
				return !operator==(target);
			}
			friend bool operator<(const ActionWrap& left,const ActionWrap& right)
			{
				if (left.action == right.action)
				{
					if (left.name == right.name)
					{
						return left.value < right.value;
					}
					else
					{
						return left.name < right.name;
					}
				}
				else
				{
					return left.action < right.action;
				}
			}
		};
	
		class PDAEdge
		{
			friend PushDownAutoMachine;
		public:
			
		public:
			PDAEdge() noexcept = default;
			~PDAEdge() noexcept = default;
			PDAEdge(PDAEdge&&) noexcept = default;
			PDAEdge(const PDAEdge&) noexcept = default;
			PDAEdge& operator=(PDAEdge&&) noexcept = default;
			PDAEdge& operator=(const PDAEdge&) noexcept = default;
			PDAEdge(const ActionWrap& wrap, PDANode* _source,PDANode* _target)noexcept
				:target(_target),source(_source),actions(make_unique<deque<ActionWrap>>()),mainActionIndex(0)
			{
				actions->emplace_back(wrap);
			}
			PDAEdge(PDANode* _source, PDANode* _target)noexcept
				: target(_target), source(_source), actions(make_unique<deque<ActionWrap>>()), mainActionIndex(0)
			{
			}
			const deque<ActionWrap>& GetActions()const
			{
				return *actions;
			}
			PDANode* GetTarget()const
			{
				return target;
			}
			PDANode* GetSource()const
			{
				return source;
			}
			bool IsNonTerminateSymbol()const
			{
				return actions->at(mainActionIndex).GetActionType() == ActionType::NonTerminate;
			}
		private:
			PDANode* source;
			PDANode* target;
			int		 mainActionIndex;
			unique_ptr<deque<ActionWrap>> actions;
			
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
		public:
			const vector<PDAEdge*>& GetNexts()const
			{
				return *nexts;
			}
			const vector<PDAEdge*>& GetFronts()const
			{
				return *fronts;
			}
			
			void SetMergeFlag()
			{
				isMerge = true;
			}
			void ClearMergeFlag()
			{
				isMerge = false;
			}
			bool IsMergeNode()const
			{
				return isMerge;
			}
		private:
			unique_ptr<vector<PDAEdge*>> nexts;
			unique_ptr<vector<PDAEdge*>> fronts;
			bool						 isMerge=false;
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
			unordered_map<wstring, vector<pair<PDANode*,PDANode*>>>& GetPDAMap();
			pair<PDANode*, PDANode*>	NewNodePair();
			PDANode*					NewNode();

			void						AddEdge(PDANode* source, PDANode* target, const ActionWrap& wrap);
			void						AddEdge(PDANode* source, PDANode* target, const deque<ActionWrap>& wrapList);
			void DeleteEdge(PDAEdge* target);

			void						AddGeneratePDA(wstring ruleName, PDANode * start, PDANode* end);
			pair<PDANode*, PDANode*>	AddSequenceLinkNode(pair<PDANode*, PDANode*>& left, pair<PDANode*, PDANode*>&right);
			pair<PDANode*, PDANode*>	AddLoopLinkNode(PDANode* loopStart, PDANode* loopEnd);
			pair<PDANode*, PDANode*>	AddAlterationLinkNode(pair<PDANode*, PDANode*>& left, pair<PDANode*, PDANode*>&right);
			pair<PDANode*, PDANode*>	AddOptionalLinkNode(PDANode* optionalStart, PDANode* optionalEnd);
			void						EdgeAdditionAction(PDANode* targetNode, const ActionWrap& wrap);

			//保留Left节点合并left right
			PDANode*					MergeIndependentNodes(PDANode* left, PDANode* right);
			
			void BackInsertAction(PDAEdge* edge, const ActionWrap& wrap);
			void FrontInsertAction(PDAEdge* edge, const ActionWrap& wrap);
		private:
			PDAEdge* NewEdge(PDANode* source, PDANode* target, const ActionWrap& wrap);
			PDAEdge* NewEdge(PDANode* source, PDANode* target);

		private:
			vector<shared_ptr<PDAEdge>> edges;
			vector<shared_ptr<PDANode>> nodes;
			SymbolManager* manager;
			unordered_map<wstring, vector<pair<PDANode*,PDANode*>>> PDAMap;
		};
		
		void CreatePDAGraph(PushDownAutoMachine& machine);
		void LogPDAGraph(const wstring& fileName, PushDownAutoMachine& machine);
		wstring ActionTypeToWString(ActionType type);
		void MergeGrammarCommonFactor(PushDownAutoMachine& machine);
		void MergeGraph(PushDownAutoMachine& machine);

	}
}