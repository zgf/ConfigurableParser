#include "Include/stdafx.h"
#include "Include/GeneralJumpInfo.h"
#include "Include/GeneralPushDownAutoMachine.h"

namespace ztl
{
	namespace general_parser
	{
		GeneralJumpInfo::GeneralJumpInfo(PushDownAutoMachine * _machine)
			:machine(_machine),
			ruleRequiresMap(make_shared<unordered_map<PDAEdge*, vector<wstring>>>()),
			terminateMap(make_shared<TerminateMapType>()),
			rightRecursionMap(make_shared<unordered_map<PDAEdge*, vector<wstring>>>()),
			rootNumber(-1)
		{
			assert(machine->GetRoot() != nullptr);
			rootNumber = machine->GetRoot()->GetNumber();
		}
		SymbolManager* GeneralJumpInfo::GetSymbolManager()const
		{
			return machine->GetSymbolManager();
		}
		PDANode* GeneralJumpInfo::GetRoot()const
		{
			return machine->GetRoot();
		}

		PushDownAutoMachine * GeneralJumpInfo::GetMachine() const
		{
			return machine;
		}

		void GeneralJumpInfo::CacheJumpInfo()
		{
			unordered_set<PDAEdge*> sign;
			deque<PDANode*> queue;
			queue.emplace_back(GetRoot());
			while(!queue.empty())
			{
				auto front = queue.front();
				queue.pop_front();
				for(PDAEdge* edgeIter : front->GetNexts())
				{
					if(sign.find(edgeIter) == sign.end())
					{
						sign.insert(edgeIter);
						assert(edgeIter->GetActions().size() >= 1);
						CacheEdgeInfo(edgeIter);
						queue.emplace_back(edgeIter->GetTarget());
					}
				}
			}
		}

		vector<wstring>::const_iterator FindRightRecursionPosition(vector<wstring>::const_iterator begin, vector<wstring>::const_iterator end)
		{
			unordered_set<wstring> sign;
			return find_if(begin, end, [&sign](const wstring& value)
			{
				if(sign.find(value) == sign.end())
				{
					sign.insert(value);
					return false;
				}
				else
				{
					return true;
				}
			});
		}
		vector<wstring>::const_iterator FindRightRecursionPosition(const vector<wstring>&ruleInfos)
		{
			return FindRightRecursionPosition(ruleInfos.begin(), ruleInfos.end());
		}
		//end是超尾

		RightRecursionInfo FindRightRecursionArea(vector<wstring>::const_iterator& begin, vector<wstring>::const_iterator end)
		{
			RightRecursionInfo result;
			auto back = FindRightRecursionPosition(begin, end);
			if(back == end)
			{
				result.isRecursion = false;
			}
			else
			{
				result.back = back - begin;
				result.isRecursion = true;
				auto first = find_if(begin, back, [&back](const wstring& value)
				{
					return value == *back;
				});
				assert(first != back);
				result.begin = first - begin;
			}
			return result;
		}
		vector<RightRecursionInfo> FindRightRecursionArea(const vector<wstring>&ruleInfos)
		{
			vector<RightRecursionInfo> result;
			for(auto ruleInfosBegin = ruleInfos.begin(); ruleInfosBegin < ruleInfos.end(); ++ruleInfosBegin)
			{
				auto back = FindRightRecursionPosition(ruleInfosBegin, ruleInfos.end());
				if(back != ruleInfos.end())
				{
					RightRecursionInfo temp;
					temp.back = back - ruleInfos.begin();
					temp.isRecursion = true;
					auto first = find_if(ruleInfos.begin(), back, [&back](const wstring& value)
					{
						return value == *back;
					});
					assert(first != back&&first != ruleInfos.end());
					temp.begin = first - ruleInfos.begin();
					result.emplace_back(move(temp));
					ruleInfosBegin = ruleInfos.begin() + result.back().back;
				}
			}

			return result;
		}
		bool IsRightRecursionGrammar(const vector<wstring>&ruleInfos)
		{
			auto end = FindRightRecursionPosition(ruleInfos);
			return end != ruleInfos.end();
		}
		//右递归的ruleRquire转成非右递归的形式用来匹配RuleStack.
		vector<wstring>	rightRecursionToNormalRules(const vector<RightRecursionInfo>&rightRecursionPositions, const vector<wstring>& ruleRequire)
		{
			vector<wstring> nonRightRecursionList;
			ptrdiff_t lastPosition = 0;
			for(auto&&rightIter : rightRecursionPositions)
			{
				for(auto j = lastPosition; j < rightIter.begin; ++j)
				{
					nonRightRecursionList.emplace_back(ruleRequire[j]);
				}
				lastPosition = rightIter.back;
			}
			for(; lastPosition < (ptrdiff_t) ruleRequire.size(); ++lastPosition)
			{
				nonRightRecursionList.emplace_back(ruleRequire[lastPosition]);
			}
			return nonRightRecursionList;
		}
		void GeneralJumpInfo::CacheRuleRequiresMap(PDAEdge* edge, const vector< ActionWrap>& ruleStack, vector<wstring>&ruleInfos)
		{
			if(!ruleStack.empty())
			{
				for(size_t i = 0; i < ruleStack.size(); ++i)
				{
					auto&& wrap = ruleStack[i];
					assert(wrap.GetActionType() == ActionType::Assign || wrap.GetActionType() == ActionType::Using);
					ruleInfos.emplace_back(wrap.GetFrom());
				}
				ruleInfos.emplace_back(ruleStack.back().GetTo());
			}
			if(!ruleInfos.empty())
			{
				auto areas = FindRightRecursionArea(ruleInfos);
				if(areas.size() != 0)
				{
					//有右递归
					this->rightRecursionMap->insert({ edge,ruleInfos });

					this->ruleRequiresMap->insert({ edge,rightRecursionToNormalRules(areas,ruleInfos) });
				}
				else
				{
					this->ruleRequiresMap->insert({ edge,ruleInfos });
				}
			}
			else
			{
				CacheEnterRule(edge);
			}
		}

		bool  GeneralJumpInfo::IsRightRecursionEdge(PDAEdge* edge)const
		{
			auto findIter = rightRecursionMap->find(edge);
			return findIter != rightRecursionMap->end();
		}
		const  vector<wstring>&		GeneralJumpInfo::GetRightRecursionRuleRequires(PDAEdge* edge)const
		{
			auto findIter = rightRecursionMap->find(edge);
			assert(findIter != rightRecursionMap->end());
			return findIter->second;
		}
		void GeneralJumpInfo::CacheEnterRule(PDAEdge* edge)
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
				case ztl::general_parser::ActionType::Setter:
				case ztl::general_parser::ActionType::Assign:
					name = first->GetFrom();
					break;
				case ztl::general_parser::ActionType::Reduce:
				case ztl::general_parser::ActionType::NonTerminate:
				case ztl::general_parser::ActionType::Epsilon:
				default:
					assert(false);
					break;
			}
			assert(!name.empty());
			assert(ruleRequiresMap->find(edge) == ruleRequiresMap->end());
			this->ruleRequiresMap->insert({ edge,vector<wstring>() });
			(*ruleRequiresMap)[edge].emplace_back(name);
		}
		void GeneralJumpInfo::CacheTerminateMap(PDAEdge* edge)
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
		vector<PDAEdge*>* GeneralJumpInfo::GetPDAEdgeByTerminate(const int number, const wstring & terminate)const
		{
			auto findIter = terminateMap->find(number);
			assert(findIter != terminateMap->end());
			auto findTerminateIter = findIter->second.termnateToEdgesMap.find(terminate);
			return (findTerminateIter == findIter->second.termnateToEdgesMap.end()) ? nullptr : std::addressof(findTerminateIter->second);
		}

		int GeneralJumpInfo::GetRootNumber()const
		{
			assert(rootNumber != -1);
			return rootNumber;
		}

		const vector<wstring>& GeneralJumpInfo::GetRuleRequires(PDAEdge* edge) const
		{
			assert(ruleRequiresMap->find(edge) != ruleRequiresMap->end());
			return (*ruleRequiresMap)[edge];
		}

		void GeneralJumpInfo::CacheEdgeInfo(PDAEdge * edge)
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
					case ztl::general_parser::ActionType::Assign:
						if(!iter.GetTo().empty())
						{
							ruleStack.emplace_back(iter);
							nodeStack.emplace_back(iter);
						}
						break;
					case ztl::general_parser::ActionType::Using:
						ruleStack.emplace_back(iter);
						break;
					case ztl::general_parser::ActionType::Setter:
						break;
					case ztl::general_parser::ActionType::Create:
					default:
						break;
				}
			}
			CacheTerminateMap(edge);
			CacheRuleRequiresMap(edge, ruleStack, ruleInfos);
		}
		wstring ActionTypeToWString(ActionType type)
		{
			wstring result;
			switch(type)
			{
				case ztl::general_parser::ActionType::Using:
					result = L"Using";
					break;
				case ztl::general_parser::ActionType::Shift:
					result = L"Shift";

					break;
				case ztl::general_parser::ActionType::Reduce:
					result = L"Reduce";

					break;
				case ztl::general_parser::ActionType::Terminate:
					result = L"Terminate";

					break;
				case ztl::general_parser::ActionType::NonTerminate:
					result = L"NonTerminate";

					break;
				case ztl::general_parser::ActionType::Create:
					result = L"Create";

					break;
				case ztl::general_parser::ActionType::Assign:
					result = L"Assign";

					break;
				case ztl::general_parser::ActionType::Setter:
					result = L"Setter";
					break;
				case ztl::general_parser::ActionType::Epsilon:
					result = L"Epsilon";
					break;
				default:
					assert(false);
					break;
			}
			return result;
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
		void LogGraphInfo(const wstring& fileName,PushDownAutoMachine& machine)
		{
			unordered_set<PDANode*>sign;
			deque<PDANode*> queue;
			wofstream output(fileName);
			auto actionMap = InitActionTypeAndGrammarLogMap();
			queue.emplace_back(machine.GetRoot());
			while(!queue.empty())
			{
				PDANode* front = queue.front();
				queue.pop_front();
				if(sign.find(front) == sign.end())
				{
					sign.insert(front);
					auto edges = front->GetNexts();
					output << L"currenIndex: " << front->GetNumber()<<endl;
					for(auto&& colsIter : edges)
					{
						output << L" targetIndex: " << to_wstring(colsIter->GetTarget()->GetNumber()) << endl;

						auto&& actionWrapList = colsIter->GetActions();
						auto&& actionWrapStringList = ActionWrapStringList(actionWrapList, actionMap);
						for(auto&& iter : actionWrapStringList)
						{
							output << L"			" << iter << endl;
						}
					}
					for(auto&& edgeIter : front->GetNexts())
					{
						queue.emplace_back(edgeIter->GetTarget());
					}
				}
			}
		}
		void CreateJumpInfo(GeneralJumpInfo& jumpTable)
		{
			jumpTable.CacheJumpInfo();
		}
	}
}
/*

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

*/