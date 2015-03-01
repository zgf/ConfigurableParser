#pragma once
#include "stdafx.h"
#include "../Lib/ZTL/ztl_pair_builder.hpp"
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
			Epsilon,
			Using,
			Shift,
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
			wstring name;//setter key using ruleNmae create className nonterm ruleName term tokenName assgin fieldName
			wstring value;//setter value assgin ruleName
		public:
			ActionWrap() = default;
			ActionWrap(ActionType _action, const wstring& _name, const wstring& _value)
				: action(_action), name(_name), value(_value)
			{
			}
		
			~ActionWrap() = default;
		
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
			friend bool operator!=(const ActionWrap& left,const ActionWrap& target)
			{
				return !(left == target);
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
				:target(_target),source(_source),actions(make_unique<deque<ActionWrap>>())
			{
				actions->emplace_back(wrap);
			}
			PDAEdge(PDANode* _source, PDANode* _target)noexcept
				: target(_target), source(_source), actions(make_unique<deque<ActionWrap>>())
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
			int GetNonTermSymbolIndex()const
			{
				for(int i = 0; i < (int)actions->size();i++)
				{
					if (actions->at(i).GetActionType() == ActionType::NonTerminate)
					{
						return i;
					}
				}
				return -1;
			}
		private:
			PDANode* target;
			PDANode* source;
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
		};
	
		PAIR_BUILDER(JumpItem, int, targetIndex, deque<ActionWrap>, actions);
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
			unordered_map<wstring, pair<PDANode*,PDANode*>>& GetPDAMap();
			pair<PDANode*, PDANode*>	NewNodePair();
			PDANode*					NewNode();

			void						AddEdge(PDANode* source, PDANode* target, const ActionWrap& wrap);
			void						AddEdge(PDANode* source, PDANode* target, const deque<ActionWrap>& wrapList);
			void						DeleteEdge(PDAEdge* target);

			void						AddGeneratePDA(wstring ruleName, const pair<PDANode *, PDANode*>& pairNode);
			pair<PDANode*, PDANode*>	AddSequenceLinkNode(pair<PDANode*, PDANode*>& left, pair<PDANode*, PDANode*>&right);
			pair<PDANode*, PDANode*>	AddLoopLinkNode(PDANode* loopStart, PDANode* loopEnd);
			pair<PDANode*, PDANode*>	AddAlterationLinkNode(pair<PDANode*, PDANode*>& left, pair<PDANode*, PDANode*>&right);
			pair<PDANode*, PDANode*>	AddOptionalLinkNode(PDANode* optionalStart, PDANode* optionalEnd);
			PDANode*					AddFinishNodeFollowTarget(PDANode* target);
			void						FrontEdgesAdditionBackAction(PDANode* targetNode, const ActionWrap& wrap);
			void						NextEdgesAdditionFrontAction(PDANode* targetNode, const ActionWrap& wrap);

			wstring						GetRuleNameOrEmptyByNodeIndex(int index) ;
			wstring						GetRootRuleName()const;
			//保留Left节点合并left right
			PDANode*					MergeIndependentNodes(PDANode* left, PDANode* right);
			
			void BackInsertAction(PDAEdge* edge, const ActionWrap& wrap);
			void FrontInsertAction(PDAEdge* edge, const ActionWrap& wrap);
			void													CreateJumpTable();
			void													ClearJumpTable();
				const unordered_map<int, vector<JumpItem>>& GetJumpTable()const;
			vector<JumpItem> CreateJumpItem(PDANode* source, unordered_set<PDAEdge*>& sign, deque<PDANode*>& queue);

			void InitNodeIndexToRuleNameMap();
			void InitNodeIndexMap();
			void ClearNodeIndexMap();
			void ClearNodeIndexToRuleNameMap();
		private:
			PDAEdge* NewEdge(PDANode* source, PDANode* target, const ActionWrap& wrap);
			PDAEdge* NewEdge(PDANode* source, PDANode* target);
			int  GetNodeIndex(PDANode* target);
			
		private:
			vector<shared_ptr<PDAEdge>>											 edges;
			vector<shared_ptr<PDANode>>											 nodes;
			SymbolManager*														 manager;
			unordered_map<wstring, pair<PDANode*,PDANode*>>						 PDAMap;
			//int 1 nodeIndex int 2 tagIndex  int targetNodeIndex
			
			unordered_map<int, vector<JumpItem>>								 jumpTable;
			unordered_map<PDANode*, int>										 nodeIndexMap;
			unordered_map<int, wstring>											 nodeIndexToRuleNameMap;
			
		};
		void CreatePDAGraph(PushDownAutoMachine& machine);
		void LogPDAGraph(const wstring& fileName, PushDownAutoMachine& machine);
		wstring ActionTypeToWString(ActionType type);
		void MergeGrammarCommonFactor(PushDownAutoMachine& machine);
		void MergeGraph(PushDownAutoMachine& machine);
		void MergeNoTermSymbol(PushDownAutoMachine& machine);

		void LogJumpTable(wstring fileName,PushDownAutoMachine& machine);
		void MergeStartAndEndNode(PushDownAutoMachine& machine, unordered_map<wstring, vector<pair<PDANode*, PDANode*>>>& PDAMap);
		void AddFinishNode(PushDownAutoMachine& machine);
	}
}