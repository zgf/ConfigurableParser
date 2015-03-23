#include "Include/stdafx.h"
#include "Include/GeneralJumpTable.h"
#include "Include/GeneralPushDownAutoMachine.h"

namespace ztl
{
	namespace general_parser
	{
		GeneralJumpTable::GeneralJumpTable(PushDownAutoMachine * _machine)
			:machine(_machine),
			createdNodeRequiresMap(make_shared<unordered_map<PDAEdge*, vector<CreateInfo>>>()),
			jumpTable(make_shared<unordered_map<int, vector<JumpItem>>>()),
			ruleRequiresMap(make_shared<unordered_map<PDAEdge*, unique_ptr<vector<wstring>>>>()),
			terminateMap(make_shared<TerminateMapType>()),
			rootNumber(-1)
		{
			assert(machine->GetRoot() != nullptr);
			rootNumber = machine->GetRoot()->GetNumber();
		}
		SymbolManager* GeneralJumpTable::GetSymbolManager()const
		{
			return machine->GetSymbolManager();
		}
		PDANode* GeneralJumpTable::GetRoot()const
		{
			return machine->GetRoot();
		}

		void GeneralJumpTable::CreateJumpTable()
		{
			unordered_set<PDAEdge*> sign;
			deque<PDANode*> queue;
			queue.emplace_back(GetRoot());
			while(!queue.empty())
			{
				auto front = queue.front();
				queue.pop_front();
				jumpTable->insert(make_pair(front->GetNumber(), CreateJumpItem(front, sign, queue)));
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
					result.emplace_back(edgeIter->GetTarget()->GetNumber(), edgeIter);
					CacheEdgeInfo(edgeIter);
					queue.emplace_back(edgeIter->GetTarget());
				}
			}
			return result;
		}
		void GeneralJumpTable::CacheCreatedNodeRequiresMap(PDAEdge * edge, const vector<ActionWrap>& nodeStack, vector<CreateInfo>& createInfos)
		{
			for_each(nodeStack.begin(), nodeStack.end(), [&createInfos](const ActionWrap& wrap)
			{
				createInfos.emplace_back(wrap.GetValue(), wrap.GetName());
			});
			if(!createInfos.empty())
			{
				this->createdNodeRequiresMap->insert(make_pair(edge, createInfos));
			}
		}
		void GeneralJumpTable::CacheRuleRequiresMap(PDAEdge* edge, const vector< ActionWrap>& ruleStack, vector<wstring>&ruleInfos)
		{
			for_each(ruleStack.begin(), ruleStack.end(), [flag = false, &ruleInfos](const ActionWrap& wrap)mutable
			{
				if(flag == false)
				{
					flag = true;
					ruleInfos.emplace_back(wrap.GetFrom());
					ruleInfos.emplace_back(wrap.GetTo());
				}
				else
				{
					ruleInfos.emplace_back(wrap.GetTo());
				}
			});
			if(!ruleInfos.empty())
			{
				this->ruleRequiresMap->insert({ edge,make_unique<vector<wstring>>(ruleInfos) });
			}
			else
			{
				CacheEnterRule(edge);
			}
		}
		void GeneralJumpTable::CacheEnterRule(PDAEdge* edge)
		{
			assert(!edge->GetActions().empty());
			auto actions = edge->GetActions();
			auto first = actions.begin();
			assert(first->GetActionType() != ActionType::Setter);

			ActionType type = first->GetActionType();
			wstring name;
			switch(type)
			{
				case ztl::general_parser::ActionType::Using:
				case ztl::general_parser::ActionType::Shift:
				case ztl::general_parser::ActionType::Terminate:
				case ztl::general_parser::ActionType::Create:
				case ztl::general_parser::ActionType::Assign:
					name = first->GetFrom();
					break;
				case ztl::general_parser::ActionType::Reduce:
				case ztl::general_parser::ActionType::NonTerminate:
				case ztl::general_parser::ActionType::Epsilon:
				case ztl::general_parser::ActionType::Setter:
				default:
					assert(false);
					break;
			}
			assert(!name.empty());
			assert(ruleRequiresMap->find(edge) == ruleRequiresMap->end());
			this->ruleRequiresMap->insert({ edge, make_unique<vector<wstring>>() });
			(*ruleRequiresMap)[edge]->emplace_back(name);
		}
		void GeneralJumpTable::CacheTerminateMap(PDAEdge* edge)
		{
			auto findTermIter = find_if(make_reverse_iterator(edge->GetActions().end()), make_reverse_iterator(edge->GetActions().begin()), [](const ActionWrap& wrap)
			{
				return wrap.GetActionType() == ActionType::Terminate;
			}).base();
			assert(findTermIter != edge->GetActions().begin());
			findTermIter -= 1;
			auto terminate = findTermIter->GetName();
			auto number = edge->GetSource()->GetNumber();
			auto findIter = terminateMap->find(number);
			if(findIter == terminateMap->end())
			{
				terminateMap->insert({ number,TerminateToEdgesMapType() });
			}
			else if(findIter->second.termnateToEdgesMap.find(terminate) == findIter->second.termnateToEdgesMap.end())
			{
				findIter->second.termnateToEdgesMap.insert({ terminate, vector<PDAEdge*>() });
			}
			(*terminateMap)[number].termnateToEdgesMap[terminate].emplace_back(edge);
		}
		vector<PDAEdge*>* GeneralJumpTable::GetPDAEdgeByTerminate(const int number, const wstring & terminate)const
		{
			auto findIter = terminateMap->find(number);
			assert(findIter != terminateMap->end());
			auto findTerminateIter = findIter->second.termnateToEdgesMap.find(terminate);
			return (findTerminateIter == findIter->second.termnateToEdgesMap.end()) ? nullptr : std::addressof(findTerminateIter->second);
		}

		int GeneralJumpTable::GetRootNumber()const
		{
			assert(rootNumber != -1);
			return rootNumber;
		}

		vector<wstring>* GeneralJumpTable::GetRuleRequires(PDAEdge * edge) const
		{
			auto findIter = ruleRequiresMap->find(edge);
			return (findIter != ruleRequiresMap->end()) ? std::addressof(*findIter->second) : nullptr;
		}
		const vector<CreateInfo>* GeneralJumpTable::GetCreateNodeRequires(PDAEdge * edge) const
		{
			auto findIter = createdNodeRequiresMap->find(edge);
			return (findIter != createdNodeRequiresMap->end()) ? std::addressof(findIter->second) : nullptr;
		}
		void GeneralJumpTable::CacheEdgeInfo(PDAEdge * edge)
		{
			auto&& actions = edge->GetActions();

			assert(!actions.empty());
			assert(accumulate(actions.begin(), actions.end(), 0, [](int sum, const ActionWrap& wrap)
			{
				return sum + (wrap.GetActionType() == ActionType::Terminate);
			}) == 1);
			assert(find_if(actions.begin(), actions.end(), [](const ActionWrap& wrap)
			{
				return wrap.GetActionType() == ActionType::Epsilon ||
					wrap.GetActionType() == ActionType::NonTerminate ||
					wrap.GetActionType() == ActionType::Reduce;
			}) == actions.end());

			vector<CreateInfo> createInfos;
			vector<wstring> ruleInfos;
			vector<ActionWrap> nodeStack;
			vector<ActionWrap> ruleStack;

			for(size_t i = 0; i < actions.size() && actions[i].GetActionType() != ActionType::Shift; ++i)
			{
				auto&&iter = actions[i];
				switch(iter.GetActionType())
				{
					case ztl::general_parser::ActionType::Epsilon:
					case ztl::general_parser::ActionType::NonTerminate:
					case ztl::general_parser::ActionType::Reduce:
					case ztl::general_parser::ActionType::Shift:
						assert(false);
						break;
					case ztl::general_parser::ActionType::Create:
						nodeStack.emplace_back(iter);
						break;
					case ztl::general_parser::ActionType::Assign:
						ruleStack.emplace_back(iter);
						break;
					case ztl::general_parser::ActionType::Using:
						ruleStack.emplace_back(iter);
						break;
					case ztl::general_parser::ActionType::Setter:
					default:
						break;
				}
			}
			if(!nodeStack.empty())
			{
				CacheCreatedNodeRequiresMap(edge, nodeStack, createInfos);
			}
			CacheTerminateMap(edge);
			CacheRuleRequiresMap(edge, ruleStack, ruleInfos);
		}

		void GeneralJumpTable::ClearJumpTable()
		{
			jumpTable->clear();
		}

		const unordered_map<int, vector<JumpItem>>& GeneralJumpTable::GetJumpTable() const
		{
			assert(!jumpTable->empty());
			return *jumpTable;
		}
		using lambdaType = wstring(*)(const ActionWrap&);
		unordered_map<ActionType, lambdaType> InitActionTypeAndGrammarLogMap()
		{
			unordered_map<ActionType, lambdaType> actionMap;
			auto functor = [](const ActionWrap& wrap)->wstring
			{
				return ActionTypeToWString(wrap.GetActionType()) + L" : " + wrap.GetName() + L":" + wrap.GetValue() +
					L" : " + wrap.GetFrom() + L":" + wrap.GetTo();
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
		void LogJumpTable(wstring fileName, GeneralJumpTable& jumpTable)
		{
			auto actionMap = InitActionTypeAndGrammarLogMap();
			auto&& table = jumpTable.GetJumpTable();
			wofstream output(fileName);
			for(auto rowsIter : table)
			{
				auto&& nodeIndex = rowsIter.first;
				auto&&  edges = rowsIter.second;
				sort(edges.begin(), edges.end(), [](const JumpItem& left, const JumpItem& right)
				{
					return left.targetIndex < right.targetIndex;
				});
				output << L"NodeIndex:" + to_wstring(nodeIndex) << endl;

				for(auto&& colsIter : edges)
				{
					auto&& targetNodeIndex = colsIter.targetIndex;
					output << L" targetIndex: " << to_wstring(targetNodeIndex) << endl;

					auto&& actionWrapList = colsIter.edges->GetActions();
					auto&& actionWrapStringList = ActionWrapStringList(actionWrapList, actionMap);
					for(auto&& iter : actionWrapStringList)
					{
						output << L"			" << iter << endl;
					}
				}
			}
		}
		void CreateJumpTable(GeneralJumpTable & jumpTable)
		{
			jumpTable.CreateJumpTable();
		}
		void HelpLogJumpTable(wstring fileName, GeneralJumpTable& jumpTable)
		{
			LogJumpTable(fileName, jumpTable);
		}
	}
}