#pragma once
#include "stdafx.h"
#include "../../Lib/ZTL/ztl_pair_builder.hpp"

namespace ztl
{
	namespace general_parser
	{
		class ParserSymbol;
		struct GeneralRuleDefine;
		struct GeneralGrammarTypeDefine;
		class SymbolManager;
		class PushDownAutoMachine;
		struct GeneralTableDefine;
		class PDANode;
		class PDAEdge;
		class CreatedNodeResolve;
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
			GrammarBegin,
		};
		
		class ActionWrap
		{
			ActionType action;
			//	wstring name;//setter key using ruleNmae create className nonterm ruleName term tokenName assgin fieldName
			//Assgin ParserSymbol FiledDef,
			//Setter  ParserSymbol FiledDef,
			//Noterminate ParserSymbol RuleDef
			//Terminate ParserSymbol TokenDef
			//Create ParserSymbol ClassDef
			//Shift ParserSymbol From RuleDef
			//Using ParserSymbol From RuleDef
			ParserSymbol* data;
			wstring value;//setter value assgin ruleName
			
		public:
			ActionWrap() = default;
			~ActionWrap() noexcept = default;
			ActionWrap(ActionWrap&&)  = default;
			ActionWrap(const ActionWrap&)  = default;
			ActionWrap& operator=(ActionWrap&&)  = default;
			ActionWrap& operator=(const ActionWrap&)  = default;
			ActionWrap(ActionType _action, ParserSymbol* _data, const wstring& _value)
				: action(_action), data(_data), value(_value)
			{
			}
			ActionWrap(ActionType _action, ParserSymbol* _data)
				: action(_action), data(_data)
			{
			}
			ActionType GetActionType()const
			{
				return action;
			}
			ParserSymbol*const& GetParserSymbol()const
			{
				return data;
			}
			const wstring& GetName()const;
			const wstring& GetValue()const
			{
				return value;
			}
			
			bool operator==(const ActionWrap& target)const
			{
				return target.action == action&&
					target.GetName() == GetName() &&
					target.value == value;
			}
			friend bool operator!=(const ActionWrap& left, const ActionWrap& target)
			{
				return !(left == target);
			}
			friend bool operator<(const ActionWrap& left, const ActionWrap& right)
			{
				if(left.action == right.action)
				{
					if(left.GetName() == right.GetName())
					{
						return left.GetValue() < right.GetValue();
					}
					else
					{
						return left.GetName() < right.GetName();
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
			PDAEdge(PDAEdge&&) = default;
			PDAEdge(const PDAEdge&) = default;
			PDAEdge& operator=(PDAEdge&&) = default;
			PDAEdge& operator=(const PDAEdge&) = default;
			PDAEdge(const ActionWrap& wrap, PDANode* _source, PDANode* _target, int _number)
				:target(_target), source(_source), number(_number)
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
			PDANode() :nexts(make_unique<vector<PDAEdge*>>()), fronts(make_unique<vector<PDAEdge*>>())
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

			int GetNumber() const
			{
				return number;
			}
		private:
			int number;

			unique_ptr<vector<PDAEdge*>> nexts;
			unique_ptr<vector<PDAEdge*>> fronts;
		};
		PAIR_BUILDER(CreateInfo, wstring, createType, vector<wstring>, fieldNames);
		
	}
}