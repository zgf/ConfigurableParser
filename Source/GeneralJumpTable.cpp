#include "Include/stdafx.h"
#include "Include/GeneralJumpTable.h"
#include "Include/GeneralPushDownAutoMachine.h"
namespace ztl
{
	namespace general_parser
	{
		GeneralJumpTable::GeneralJumpTable(PushDownAutoMachine * _machine) :machine(_machine)
		{
		}
		SymbolManager* GeneralJumpTable::GetSymbolManager()const
		{
			return machine->GetSymbolManager();
		}
		PDANode* GeneralJumpTable::GetRoot()const
		{
			return machine->GetRoot();
		}
		int GeneralJumpTable::GetNodeIndex(PDANode * target)
		{
			assert(nodeIndexMap.find(target) != nodeIndexMap.end());
			return nodeIndexMap[target];
		}
		void GeneralJumpTable::CreateJumpTable()
		{
			unordered_set<PDAEdge*> sign;
			deque<PDANode*> queue;
			for(auto&& ruleIter : machine->GetPDAMap())
			{
				auto&& grammarIter = ruleIter.second;
				queue.emplace_back(grammarIter.first);
				while(!queue.empty())
				{
					auto front = queue.front();
					queue.pop_front();
					jumpTable.insert(make_pair(GetNodeIndex(front), CreateJumpItem(front, sign, queue)));
				}
			}
		}
		vector<JumpItem> GeneralJumpTable::CreateJumpItem(PDANode* source, unordered_set<PDAEdge*>& sign, deque<PDANode*>& queue)
		{
			vector<JumpItem > result;
			for(PDAEdge* edgeIter : source->GetNexts())
			{
				if(sign.find(edgeIter) == sign.end())
				{
					sign.insert(edgeIter);
					assert(edgeIter->GetActions().size() >= 1);
					result.emplace_back(GetNodeIndex(edgeIter->GetTarget()), edgeIter->GetActions());
					queue.emplace_back(edgeIter->GetTarget());
				}
			}
			return result;
		}
		void GeneralJumpTable::InitNodeIndexMap()
		{
			unordered_set<PDANode*>sign;
			deque<PDANode*> queue;
			int count = 0;

			for(auto&&ruleIter : machine->GetPDAMap())
			{
				auto&& grammarIter = ruleIter.second;
				queue.emplace_back(grammarIter.first);
				while(!queue.empty())
				{
					PDANode* front = queue.front();
					queue.pop_front();
					if(sign.find(front) == sign.end())
					{
						sign.insert(front);
						nodeIndexMap.insert(make_pair(front, count));
						++count;
						for(auto&& edgeIter : front->GetNexts())
						{
							queue.emplace_back(edgeIter->GetTarget());
						}
					}
				}
			}
			InitNodeIndexToRuleNameMap();
		}

		void GeneralJumpTable::ClearNodeIndexMap()
		{
			nodeIndexMap.clear();
			ClearNodeIndexToRuleNameMap();
		}

		void GeneralJumpTable::ClearNodeIndexToRuleNameMap()
		{
			nodeIndexToRuleNameMap.clear();
		}

		void GeneralJumpTable::ClearJumpTable()
		{
			jumpTable.clear();
		}

		const unordered_map<int, vector<JumpItem>>& GeneralJumpTable::GetJumpTable() const
		{
			return jumpTable;
		}

		void GeneralJumpTable::InitNodeIndexToRuleNameMap()
		{
			for(auto&& ruleIter : machine->GetPDAMap())
			{
				auto&& ruleName = ruleIter.first;
				auto&& grammarIter = ruleIter.second;
				nodeIndexToRuleNameMap.insert(make_pair(GetNodeIndex(grammarIter.first), ruleName));
			}
		}

		wstring GeneralJumpTable::GetRuleNameOrEmptyByNodeIndex(int index)
		{
			//assert(nodeIndexToRuleNameMap.find(index) == nodeIndexToRuleNameMap.end());
			return nodeIndexToRuleNameMap.find(index) == nodeIndexToRuleNameMap.end() ? wstring() : nodeIndexToRuleNameMap[index];
		}
		using lambdaType = wstring(*)(const ActionWrap&);
		unordered_map<ActionType, lambdaType> InitActionTypeAndGrammarLogMap()
		{
			unordered_map<ActionType, lambdaType> actionMap;
			auto functor = [](const ActionWrap& wrap)->wstring
			{
				return ActionTypeToWString(wrap.GetActionType()) + L" : " + wrap.GetName();
			};
			vector<ActionType> ActionTypeList = { 
				ActionType::Assign,
				ActionType::Create,
				ActionType::Epsilon,
				ActionType::NonTerminate,
				ActionType::Reduce,
				ActionType::Setter,
				ActionType::Shift,
				ActionType::Terminate,
				ActionType::Using 
			};
			for(auto&& iter : ActionTypeList)
			{
				actionMap.insert(make_pair(iter, functor));
			}
			return actionMap;
		}
		vector<wstring> ActionWrapStringList(const vector<ActionWrap>& wrapList, unordered_map<ActionType, lambdaType>& actionMap)
		{
			vector<wstring> result;
			std::for_each(wrapList.begin(), wrapList.end(), [&result, &actionMap](auto&& element)
			{
				result.emplace_back(L"action:" + actionMap[element.GetActionType()](element) + L";");
			});
			return result;
		}
		void HelpLogJumpTable(wstring fileName, GeneralJumpTable& jumpTable)
		{
			jumpTable.InitNodeIndexMap();
			jumpTable.CreateJumpTable();
			LogJumpTable(fileName, jumpTable);
			jumpTable.ClearJumpTable();
			jumpTable.ClearNodeIndexMap();
		}

		void LogJumpTable(wstring fileName, GeneralJumpTable& jumpTable)
		{
			auto actionMap = InitActionTypeAndGrammarLogMap();
			auto&& table = jumpTable.GetJumpTable();
			auto&& manager = jumpTable.GetSymbolManager();
			wofstream output(fileName);
			for(auto&& rowsIter : table)
			{
				auto&& nodeIndex = rowsIter.first;
				auto ruleName = jumpTable.GetRuleNameOrEmptyByNodeIndex(nodeIndex);
				if(!ruleName.empty())
				{
					output << L"RuleName: " << ruleName << endl;
				}
				output << L"NodeIndex:" + to_wstring(nodeIndex) << endl;
				for(auto&& colsIter : rowsIter.second)
				{
					auto&& targetNodeIndex = colsIter.targetIndex;
					output << L" targetIndex: " << to_wstring(targetNodeIndex) << endl;

					auto&& actionWrapList = colsIter.actions;
					auto&& actionWrapStringList = ActionWrapStringList(actionWrapList, actionMap);
					for(auto&& iter : actionWrapStringList)
					{
						output << L"			" << iter << endl;
					}
				}
			}
		}
	}
}