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
			wstring current;//当前文法
			wstring next;//下一个文法
		public:
			ActionWrap() = default;
			
			ActionWrap(ActionType _action, const wstring& _name, const wstring& _value, const wstring& _current, const wstring& _next)
				: action(_action), name(_name), value(_value), current(_current), next(_next)
			{
			}
			~ActionWrap() = default;
		
			ActionType GetActionType()const
			{
				return action;
			}
			const wstring& GetName()const
			{
				return name;
			}
			const wstring& GetValue()const
			{
				return value;
			}
			const wstring& GetFrom()const
			{
				return current;
			}
			const wstring& GetTo()const
			{
				return next;
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
			PDAEdge() = delete;
			~PDAEdge()noexcept = default;
			PDAEdge(PDAEdge&&)  = default;
			PDAEdge(const PDAEdge&)  = default;
			PDAEdge& operator=(PDAEdge&&)  = default;
			PDAEdge& operator=(const PDAEdge&)  = default;
			PDAEdge(const ActionWrap& wrap, PDANode* _source,PDANode* _target,int _number)
				:target(_target),source(_source),number(_number)
			{
				actions.emplace_back(wrap);
			}
			PDAEdge(PDANode* _source, PDANode* _target, int _number)
				: target(_target), source(_source), number(_number)
			{
			}
			const vector<ActionWrap>& GetActions()const
			{
				return actions;
			}
			PDANode* GetTarget()const
			{
				return target;
			}
			PDANode* GetSource()const
			{
				return source;
			}
			int GetNumber()const
			{
				return number;
			}
			bool HasNonTermActionType()const
			{
				return HasThisActionType(ActionType::NonTerminate);
			}
			bool HasTermActionType()const
			{
				return HasThisActionType(ActionType::Terminate);
			}
			bool IsNullPropertyAction(ActionType type)
			{
				assert(type != ActionType::NonTerminate);
				return
				type == ActionType::Reduce ||
					type == ActionType::Shift  ||
					type == ActionType::Using;
			}
			void DeleteNullPropertyAction()
			{
				for(size_t i = 0; i < actions.size();)
				{
					auto&& iter = actions[i];
					if (IsNullPropertyAction(iter.GetActionType()))
					{
						actions.erase(actions.begin() + i);
						
					}
					else
					{
						++i;
					}
				}
			}
		
		private:
			bool HasThisActionType(ActionType type)const
			{
				return find_if(actions.begin(), actions.end(), [&type](const ActionWrap& wrap)
				{
					return wrap.GetActionType() == type;
				}) != actions.end();
			}
		private:
			int number;
			PDANode* target;
			PDANode* source;
			vector<ActionWrap> actions;
			
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
			int GetNumber() const
			{
				return number;
			}
		private:
			int number;
			
			unique_ptr<vector<PDAEdge*>> nexts;
			unique_ptr<vector<PDAEdge*>> fronts;
			bool						 isMerge=false;
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
			unordered_map<wstring, pair<PDANode*,PDANode*>>& GetPDAMap();
			pair<PDANode*, PDANode*>	NewNodePair();
			PDANode*					NewNode();
			PDANode*					GetRoot() const;
			void						AddEdge(PDANode* source, PDANode* target, const ActionWrap& wrap);

			void						AddEdge(PDANode* source, PDANode* target, const vector<ActionWrap>& wrapList);

			void						DeleteEdge(PDAEdge* target);

			void						AddGeneratePDA(wstring ruleName, const pair<PDANode *, PDANode*>& pairNode);
			pair<PDANode*, PDANode*>	AddSequenceLinkNode(pair<PDANode*, PDANode*>& left, pair<PDANode*, PDANode*>&right);
			pair<PDANode*, PDANode*>	AddLoopLinkNode(PDANode* loopStart, PDANode* loopEnd);
			pair<PDANode*, PDANode*>	AddAlterationLinkNode(pair<PDANode*, PDANode*>& left, pair<PDANode*, PDANode*>&right);
			pair<PDANode*, PDANode*>	AddOptionalLinkNode(PDANode* optionalStart, PDANode* optionalEnd);
			void						AddFinishNodeFollowTarget(PDANode* target,const wstring& ruleName);
			void						FrontEdgesAdditionBackAction(PDANode* targetNode, const ActionWrap& wrap);
			void						FrontEdgesAdditionSetterAction(PDANode* targetNode, const ActionWrap& wrap);
			wstring						GetRootRuleName()const;
			//保留Left节点合并left right
			PDANode*					MergeIndependentNodes(PDANode* left, PDANode* right);
			void BackInsertAction(PDAEdge* edge, const ActionWrap& wrap);

		private:
			PDAEdge* NewEdge(PDANode* source, PDANode* target, const ActionWrap& wrap);
			PDAEdge* NewEdge(PDANode* source, PDANode* target);
			
		private:
			vector<shared_ptr<PDAEdge>>											 edges;
			vector<shared_ptr<PDANode>>											 nodes;
			SymbolManager*														 manager;
			unordered_map<wstring, pair<PDANode*,PDANode*>>						 PDAMap;

		public:

		};
		void CreateDPDAGraph(PushDownAutoMachine& machine);
		wstring ActionTypeToWString(ActionType type);
		void MergeGrammarCommonFactor(PushDownAutoMachine& machine);
		void MergeEpsilonPDAGraph(PushDownAutoMachine& machine);
		void MergePDAEpsilonSymbol(PushDownAutoMachine& machine);

		void MergeStartAndEndNode(PushDownAutoMachine& machine, unordered_map<wstring, vector<pair<PDANode*, PDANode*>>>& PDAMap);
		void AddFinishNode(PushDownAutoMachine& machine);
	}
}