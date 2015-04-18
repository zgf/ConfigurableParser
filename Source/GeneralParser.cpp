#include "Include\stdafx.h"
#include "Include\GeneralParser.h"
#include "Include\SymbolManager.h"
#include "Include\GeneralPushDownAutoMachine.h"
#include "Include\GeneralJumpInfo.h"
#include "Include\GeneralTreeNode.h"
#include "Include\ParserSymbol.h"
#include "Include\CreatedNodeResolve.h"
namespace ztl
{
	namespace general_parser
	{
		CreateNode::CreateNode(const ActionType& _type, int _nodeIndex, int _tokenIndex, ParserSymbol* _symbol)
			:type(_type), nodeIndex(_nodeIndex), symbol(_symbol), tokenIndex(_tokenIndex)
		{
		}
		ActionType CreateNode::GetType()const
		{
			return type;
		}
		int CreateNode::GetTokenIndex()const
		{
			assert(type == ActionType::Terminate || type == ActionType::Assign || type == ActionType::Setter);
			return tokenIndex;
		}
		int CreateNode::GetTermIndex()const
		{
			assert(type == ActionType::Assign || type == ActionType::Create);
			return nodeIndex;
		}
		void CreateNode::SetNodeIndex(int index)
		{
			assert(type == ActionType::Assign);
			nodeIndex = index;
		}
		void CreateNode::SetTokenIndex(int index)
		{
			assert(type == ActionType::Assign || type == ActionType::Setter);
			tokenIndex = index;
		}
		ParserSymbol * CreateNode::GetSymbol() const
		{
			return symbol;
		}
		GeneralParser::GeneralParser(const wstring& fileName, const shared_ptr<GeneralTableDefine>& _tableDefine) :
			manager(make_shared<SymbolManager>(_tableDefine)),
			jumpInfos(nullptr),
			machine(nullptr)
		{
			pools.SetTokenPool(move(ParseToken(fileName)));
		}

		void GeneralParser::BuildParser()
		{
			ValidateGeneratorCoreSemantic(manager.get());
			machine = make_shared<PushDownAutoMachine >(manager.get());
			CreateDPDAGraph(*machine.get());
			jumpInfos = make_shared<GeneralJumpInfo>(machine.get());
			CreateJumpInfo(*jumpInfos.get());
			//	HelpLogJumpTable(L"LogJumpTable_MergeNoTermGraphTable.txt", *jumpTable);
		}
		SymbolManager* GeneralParser::GetManager() const
		{
			return manager.get();
		}

		void	GeneralParser::SaveEdge(deque<ParserState>& states, const vector<EdgeInfo>& edges)
		{
			//当前edge/tokenIndex,没有被加入过队列就继续
			static unordered_set<pair<PDAEdge*, int>, ztl::hasher::ztl_hash<pair<PDAEdge*, int>>> sign;
			if(!edges.empty())
			{
				if(sign.find(make_pair(edges.front().edge, states.front().tokenIndex)) != sign.end())
				{
					states.pop_front();
				}
				else
				{
					states.front().SaveEdgeInfo(edges.front());
				}

				for(size_t i = 1; i < edges.size(); ++i)
				{
					if (sign.find(make_pair(edges[i].edge, states.front().tokenIndex)) == sign.end())
					{
						sign.insert(make_pair(edges[i].edge, states.front().tokenIndex));
						states.emplace_back(states.front());
						states.back().SaveEdgeInfo(edges[i]);
						states.back().fieldsList = states.front().fieldsList;
					}
					else if(sign.find(make_pair(edges[i].edge, states.front().tokenIndex)) != sign.end())
					{
						auto findIter = std::find_if(states.begin() + 1, states.end(), [&edge = edges[i],&states](const ParserState& state)
						{
							if (states.front().currentNodeIndex == state.currentNodeIndex&&
								state.rulePathStack == states.front().rulePathStack&&
								states.front().fieldsList== state.fieldsList&&
								0 == edge.Compare(state.edgeInfo) &&
								state.tokenIndex ==states.front().tokenIndex
								)
							{
								return true;
							}
							else
							{
								return false;
							}
						});
						if (findIter == states.end())
						{
							sign.insert(make_pair(edges[i].edge, states.front().tokenIndex));
							states.emplace_back(states.front());
							states.back().SaveEdgeInfo(edges[i]);
							states.back().fieldsList = states.front().fieldsList;
						}
					}
				}
			}
			else
			{
				states.pop_front();
			}
		}

		void PrintRuntimeInfo(const deque<ParserState>& parserStates, const vector<shared_ptr<TokenInfo>>& tokenPool)
		{
			wcout << L"nodeIndex:" << parserStates.front().currentNodeIndex << endl;
			wcout << L"tokenIndex:" << parserStates.front().tokenIndex << L" token: " << tokenPool[parserStates.front().tokenIndex]->GetTokenInfo() << endl;
			wcout << L"currentStack:" << accumulate(parserStates.front().rulePathStack.begin(), parserStates.front().rulePathStack.end(), wstring(), [](const wstring& sum, const wstring& value)
			{
				return sum + L" " + value;
			}) << endl;
		}
		void GeneralParser::CheckParserResultConvergence()
		{
			if(parserStates.front().fieldsList.size() != 2)
			{
				throw ztl_exception(L"Parser Result Can't convergence!" + GetParserInfo(parserStates.front()));
			}
		}
		PushDownAutoMachine&  GeneralParser::GetMachine()const
		{
			return *machine;
		}
		void GeneralParser::SaveHeterogeneousNode(const shared_ptr<void>& node)
		{
			pools.SetHeterogeneousPool(node);
		}
		vector<GeneralTreeNode*> GeneralParser::GenerateIsomorphismParserTree()
		{
			assert(!this->pools.GetTokenPool().empty());
			auto rootRule = machine->GetRootRuleName();
			assert(!rootRule.empty());
			this->parserStates.emplace_back(0, jumpInfos->GetRootNumber(), rootRule);
			vector<GeneralTreeNode*> ambiguityRoots;
			while(!parserStates.empty())
			{
				while(parserStates.front().tokenIndex != (int) pools.GetTokenPool().size())
				{
					if(parserStates.front().tokenIndex == 468)
					{
						int a = 0;
					}
					auto edges = EdgeResolve(parserStates.front());
					SaveEdge(parserStates, edges);
					HandleRightRecursionEdge(parserStates.front());
					ExecuteEdgeActions(parserStates.front());
					++parserStates.front().tokenIndex;
					parserStates.front().currentNodeIndex = parserStates.front().GetEdge()->GetTarget()->GetNumber();
				}
				if(!parserStates.empty())
				{
					CheckParserResultConvergence();
					ambiguityRoots.emplace_back(this->pools.GetGeneralNodePool()[parserStates.front().fieldsList.front().GetTermIndex()].get());
					parserStates.pop_front();
				}
			}
			if(!ambiguityRoots.empty())
			{
				generalTreeRoot = ambiguityRoots.front();
			}
			return ambiguityRoots;
		}
		GeneralTreeNode* GeneralParser::GetGeneralTreeRoot() const
		{
			return generalTreeRoot;
		}

		vector<EdgeInfo> GeneralParser::EdgeResolve(ParserState& state)
		{
			return TerminateResolve(state);
		}
		vector<EdgeInfo> GeneralParser::TerminateResolve(ParserState& state)
		{
			auto& tokenIndex = state.tokenIndex;
			auto& number = state.currentNodeIndex;
			auto&& token = this->pools.GetTokenPool()[tokenIndex];

			auto edges = jumpInfos->GetPDAEdgeByTerminate(number, token->tag);
			if(edges == nullptr || edges->empty())
			{
				return{};
			}
			return RuleResolve(edges, state);
		}

		vector<EdgeInfo> GeneralParser::RuleResolve(vector<PDAEdge*>* edges, ParserState& state)
		{
			auto& rulePathStack = state.rulePathStack;
			auto& tokenIndex = state.tokenIndex;
			assert(edges != nullptr && !edges->empty());
			vector<EdgeInfo> result;
			vector<PDAEdge*> candicate;

			for(size_t i = 0; i < edges->size(); ++i)
			{
				PDAEdge* iter = (*edges)[i];
				const vector<wstring>& ruleRequire = jumpInfos->GetRuleRequires(iter);
				if(rulePathStack.size() >= ruleRequire.size())
				{
					if(std::equal(rulePathStack.rbegin(), rulePathStack.rbegin() + ruleRequire.size(), ruleRequire.begin(), ruleRequire.end()))
					{
						if(jumpInfos->IsRightRecursionEdge(iter))
						{
							candicate.emplace_back(iter);
						}
						else
						{
							result.emplace_back(iter, false);
						}
					}
				}
			}
			if(result.empty())
			{
				for(auto&& iter : candicate)
				{
					result.emplace_back(iter, true);
				}
			}
			if(result.empty())
			{
				return{};
			}
			return CreateNodeResolve(result, state);
		}
		void GeneralParser::HandleRightRecursionEdge(ParserState& state)
		{
			auto& edge = state.edgeInfo.edge;
			auto& rulePathStack = state.rulePathStack;
			auto& isRightRecursionEdge = state.edgeInfo.rightRecursion;
			HandleRightRecursionEdge(edge, rulePathStack, isRightRecursionEdge);
		}
		void GeneralParser::HandleRightRecursionEdge(PDAEdge* edge, vector<wstring>& rulePathStack, bool isRightRecursionEdge)
		{
			if(isRightRecursionEdge)
			{
				auto&& ruleRequire = jumpInfos->GetRightRecursionRuleRequires(edge);
				auto rightRecursionAreas = FindRightRecursionArea(ruleRequire);
				for(auto&&iter : rightRecursionAreas)
				{
					if(iter.begin != iter.back)
					{
						auto offset = iter.begin;
						auto pathOffset = rulePathStack.size() - 1 - offset;
						assert(rulePathStack[pathOffset] == ruleRequire[offset]);
						auto number = iter.back - iter.begin;
						rulePathStack.insert(rulePathStack.begin() + pathOffset + 1,
							make_reverse_iterator(ruleRequire.begin() + iter.back),
							make_reverse_iterator(ruleRequire.begin() + iter.begin));
					}
				}
			}
		}

		int GeneralParser::RunDFA(vector<CreateNode>& fieldsList, const wstring& createIndex)
		{
			auto resolve = machine->GetCreateNodeResolve();
			assert(resolve->GetCreatedDFA().find(createIndex) != resolve->GetCreatedDFA().end());
			return RunDFA(fieldsList, resolve, resolve->GetCreatedDFA()[createIndex]);
		}
		int GeneralParser::RunDFA(vector<CreateNode>& fieldsList, CreatedNodeResolve* resolve, PDANode * dfaStart, int moveNumber)
		{
			assert(fieldsList.size() > moveNumber);
			auto currentNode = dfaStart;
			assert(resolve->GetDFAMap().find(currentNode) != resolve->GetDFAMap().end());
			int i = 0;
			for(; i < (int) fieldsList.size(); ++i)
			{
				auto current = fieldsList[fieldsList.size() - 1- moveNumber - i];
				auto currentSymbol = current.GetSymbol();
				if(currentNode->GetFronts().size() == 0)
				{
					return i;
				}
				assert(resolve->GetDFAMap().find(currentNode) != resolve->GetDFAMap().end());

				auto findIter = resolve->GetDFAMap()[currentNode].find(currentSymbol);
				if(findIter == resolve->GetDFAMap()[currentNode].end())
				{
					return 0;
				}
				currentNode = resolve->GetDFAMap()[currentNode][currentSymbol];
			}

			return i;
		}
		void CreateNodeActionTypeMapFunction(const CreateNode& current, GeneralTreeNode* node)
		{
			auto type = current.GetType();
			if(ActionType::Assign == type)
			{
				auto index = current.GetTermIndex();
				auto fieldName = current.GetSymbol()->GetName();
				if(index == -1)
				{
					//说明是终结符号池的索引
					node->SetTermMap(fieldName, current.GetTokenIndex());
				}
				else
				{
					//说明是Node池的索引
					node->SetFieldMap(fieldName, index);
				}
			}
			else
			{
				assert(ActionType::Setter == type);
				node->SetTermMap(current.GetSymbol()->GetName(), current.GetTokenIndex());
				return;
			}
		}
		PDANode* GeneralParser::MoveOnDFA(const vector<ActionWrap>& steps, PDANode* start)
		{
			auto resolve = machine->GetCreateNodeResolve();
			auto currentNode = start;
			assert(resolve->GetDFAMap().find(currentNode) != resolve->GetDFAMap().end());
			int i = 0;
			for(; i < (int) steps.size(); ++i)
			{
				auto current = steps[i];
				auto currentSymbol = current.GetParserSymbol();

				assert(resolve->GetDFAMap().find(currentNode) != resolve->GetDFAMap().end());

				auto findIter = resolve->GetDFAMap()[currentNode].find(currentSymbol);

				currentNode = resolve->GetDFAMap()[currentNode][currentSymbol];
			}
			return currentNode;
		}
		bool GeneralParser::CreateNodeCreateAction(int tokenIndex, vector<CreateNode>& fieldsList, const ActionWrap& wrap)
		{
			assert(wrap.GetActionType() == ActionType::Create);
			auto&& number = wrap.GetValue();
			auto count = RunDFA(fieldsList, number);
			//这里需要生成节点和判断是否路径正确.
			//然后设置create的NodeIndex
			//再清除掉前面的一串内容.
			if(count != 0)
			{
				while(count != 0)
				{
					count = RunDFA(fieldsList, number);
					if (count ==0)
					{
						break;
					}
					assert((int) fieldsList.size() >= count);
					//构建节点,清除前面的元素,挂在create上
					auto node = MakeTreeNode(wrap.GetParserSymbol());
					for(auto i = 0; i < count; ++i)
					{
						auto current = fieldsList[fieldsList.size() - 1 - i];
						if(current.GetType() != ActionType::Terminate)
						{
							assert(current.GetType() == ActionType::Setter || current.GetType() == ActionType::Assign);
							CreateNodeActionTypeMapFunction(current, node);
						}
					}
					fieldsList.erase(fieldsList.end() - count, fieldsList.end());
					fieldsList.emplace_back(ActionType::Create, node->GetNumber(), -1, wrap.GetParserSymbol());
					count = 0;
					if(machine->GetCreateNodeResolve()->IsNullRightRecursionAreas(number))
					{
						auto&& actions = machine->GetCreateNodeResolve()->GetNullRightRecursionAreas(number);
						assert(!actions.empty());
						auto resolve = machine->GetCreateNodeResolve();
						assert(resolve->GetCreatedDFA().find(number) != resolve->GetCreatedDFA().end());
						auto start = MoveOnDFA(actions, resolve->GetCreatedDFA()[number]);
						count = RunDFA(fieldsList, resolve, start,(int)actions.size());
						if (count !=0)
						{
							CreateNodeResolve(tokenIndex, actions, fieldsList);
						}
					}
				}
				return true;
			}
			else
			{
				return false;
			}
		}
		void GeneralParser::CreateNodeAssignAction(bool isTerminate, vector<CreateNode>& fieldsList, const ActionWrap& wrap)
		{
			assert(wrap.GetActionType() == ActionType::Assign);

			//清除最后的Create,获得index,挂在assgin上.
			if(isTerminate)
			{
				auto index = fieldsList.back().GetTokenIndex();
				//assgin的符号是在终结符号池的符号
				this->pools.SetTerminatePool(pools.GetTokenPool()[index]);
				index = (int) pools.GetTerminatePool().size() - 1;
				fieldsList.pop_back();
				fieldsList.emplace_back(ActionType::Assign, -1, index, wrap.GetParserSymbol());
			}
			else
			{
				auto index = fieldsList.back().GetTermIndex();
				fieldsList.pop_back();
				fieldsList.emplace_back(ActionType::Assign, index, -1, wrap.GetParserSymbol());
			}
		}
		void GeneralParser::CreateNodeSetterAction(vector<CreateNode>& fieldsList, const ActionWrap& wrap)
		{
			assert(wrap.GetActionType() == ActionType::Setter);

			pools.SetTerminatePool(make_shared<TokenInfo>(wrap.GetValue(), L"Setter", -1, -1));
			int index = (int) pools.GetTerminatePool().size() - 1;
			fieldsList.emplace_back(ActionType::Setter, -1, index, wrap.GetParserSymbol());
		}
		void GeneralParser::CreateNodeTerminateAction(int tokenIndex, vector<CreateNode>& fieldsList, const ActionWrap& wrap)
		{
			assert(wrap.GetActionType() == ActionType::Terminate);

			fieldsList.emplace_back(ActionType::Terminate, -1, tokenIndex, wrap.GetParserSymbol());
		}
		bool GeneralParser::CreateNodeResolve(int tokenIndex, const
			vector<ActionWrap>& actions, vector<CreateNode>& fieldsList)
		{
			bool isTerminate = false;
			for(size_t i = 0; i < actions.size() && actions[i].GetActionType() != ActionType::Shift; ++i)
			{
				auto&& actionIter = actions[i];
				ActionType type = actionIter.GetActionType();
				switch(type)
				{
					case ztl::general_parser::ActionType::Using:
						break;
					case ztl::general_parser::ActionType::Create:
						isTerminate = false;
						if(!CreateNodeCreateAction(tokenIndex, fieldsList, actionIter))
						{
							goto TestNextEdge;
						}

						break;
					case ztl::general_parser::ActionType::Assign:
						CreateNodeAssignAction(isTerminate, fieldsList, actionIter);
						break;
					case ztl::general_parser::ActionType::Setter:
						CreateNodeSetterAction(fieldsList, actionIter);

						break;
					case ztl::general_parser::ActionType::Terminate:
						CreateNodeTerminateAction(tokenIndex, fieldsList, actionIter);
						isTerminate = true;
						break;
					case ztl::general_parser::ActionType::GrammarBegin:
						break;
					case ztl::general_parser::ActionType::Shift:
					case ztl::general_parser::ActionType::NonTerminate:
					case ztl::general_parser::ActionType::Epsilon:
					case ztl::general_parser::ActionType::Reduce:
					default:
						assert(false);
						break;
				}
			}
			return true;
		TestNextEdge:
			return false;
		}

		vector<EdgeInfo> GeneralParser::CreateNodeResolve(const vector<EdgeInfo>& edges, ParserState& state)
		{
			assert(!edges.empty());
			vector<EdgeInfo> result;

			for(auto&& iter : edges)
			{
				bool resolve;
				auto rulePathStack = state.rulePathStack;
				auto fieldsList = state.fieldsList;
				auto tokenIndex = state.tokenIndex;
				if(iter.rightRecursion)
				{
					HandleRightRecursionEdge(iter.edge, rulePathStack, iter.rightRecursion);
					resolve = CreateNodeResolve(tokenIndex, iter.edge->GetActions(), fieldsList);
				}
				else
				{
					resolve = CreateNodeResolve(tokenIndex, iter.edge->GetActions(), fieldsList);
				}
				if(resolve)
				{
					result.emplace_back(iter);
				}
			}
			return result;
		}
		void GeneralParser::ExecuteEdgeActions(ParserState& state)
		{
			auto& edge = state.edgeInfo.edge;
			auto& rulePathStack = state.rulePathStack;
			auto& fieldsList = state.fieldsList;
			auto& tokenIndex = state.tokenIndex;
			auto actions = edge->GetActions();
			assert(!actions.empty());
			bool IsTerminate = false;
			for(auto&& actionIter : actions)
			{
				auto type = actionIter.GetActionType();
				switch(type)
				{
					case ztl::general_parser::ActionType::Shift:
						assert(rulePathStack.back() == actionIter.GetName());
						rulePathStack.emplace_back(actionIter.GetValue());
						break;
					case ztl::general_parser::ActionType::Using:
						assert(rulePathStack.size() > 1);
						assert(rulePathStack.back() == actionIter.GetName() &&
							rulePathStack[rulePathStack.size() - 2] == actionIter.GetValue());
						rulePathStack.pop_back();
						IsTerminate = false;
						break;
					case ztl::general_parser::ActionType::Terminate:
						assert(actionIter.GetFrom() == rulePathStack.back());
						assert(actionIter.GetName() == pools.GetTokenPool()[tokenIndex]->tag);
						IsTerminate = true;
						CreateNodeTerminateAction(tokenIndex, fieldsList, actionIter);
						break;

					case ztl::general_parser::ActionType::Create:
						assert(actionIter.GetFrom() == rulePathStack.back());
						CreateNodeCreateAction(tokenIndex, fieldsList, actionIter);
						break;
					case ztl::general_parser::ActionType::Assign:
						if(!IsTerminate)
						{
							assert(actionIter.GetFrom() == rulePathStack.back());

							rulePathStack.pop_back();
							assert(actionIter.GetTo() == rulePathStack.back());
						}
						CreateNodeAssignAction(IsTerminate, fieldsList, actionIter);
						break;
					case ztl::general_parser::ActionType::Setter:
						CreateNodeSetterAction(fieldsList, actionIter);
				
						break;
					case ztl::general_parser::ActionType::GrammarBegin:
						break;
					case ztl::general_parser::ActionType::Epsilon:
					case ztl::general_parser::ActionType::Reduce:
					case ztl::general_parser::ActionType::NonTerminate:
					default:
						assert(false);
						break;
				}
			}
		}
		wstring GeneralParser::GetRulePathInfo(ParserState& state) const
		{
			return accumulate(state.rulePathStack.begin(), state.rulePathStack.end(), wstring(), [](const wstring& sum, const wstring& value)
			{
				return sum + L" " + value;
			}) + L"\n";
		}
		wstring GeneralParser::GetFieldListInfo(ParserState&) const
		{
			//TODO
			return{};
		}

		wstring GeneralParser::GetParserInfo(ParserState& state) const
		{
			wcout << L"nodeIndex:" << parserStates.front().currentNodeIndex << endl;
			wcout << L"tokenIndex:" << parserStates.front().tokenIndex << L" token: " << pools.GetTokenPool()[parserStates.front().tokenIndex]->GetTokenInfo() << endl;
			wcout << L"currentStack:" << accumulate(parserStates.front().rulePathStack.begin(), parserStates.front().rulePathStack.end(), wstring(), [](const wstring& sum, const wstring& value)
			{
				return sum + L" " + value;
			}) << endl;
			return L" current rule Path" + GetRulePathInfo(state) +
				L"current token info" + pools.GetTokenPool()[state.tokenIndex]->GetTokenInfo() +
				L"current creatNodeStack: " + GetFieldListInfo(state);
		}

		GeneralTreeNode* GeneralParser::MakeTreeNode(ParserSymbol* symbol)
		{
			pools.SetGeneralNodePool(make_shared<GeneralTreeNode>((int) pools.GetGeneralNodePool().size(), symbol));
			return  pools.GetGeneralNodePool().back().get();
		}

		GeneralTreeNode*	GeneralParser::CopyTreeNode(GeneralTreeNode* target)
		{
			this->pools.SetGeneralNodePool(make_shared<GeneralTreeNode>());
			*pools.GetGeneralNodePool().back() = *target;
			return pools.GetGeneralNodePool().back().get();
		}

		vector<GeneralTreeNode*> GeneralParser::SaveCurrentStack(const vector<GeneralTreeNode*>& current)
		{
			vector<GeneralTreeNode*> result;
			for(auto&& iter : current)
			{
				result.emplace_back(CopyTreeNode(iter));
			}
			return result;
		}
		GeneralTreeNode* GeneralParser::GetNonTermNodeByIndex(int index) const
		{
			return pools.GetGeneralNodePool()[index].get();
		}
		shared_ptr<TokenInfo> GeneralParser::GetTermNodeByIndex(int index) const
		{
			return pools.GetTerminatePool()[index];
		}
	}
}