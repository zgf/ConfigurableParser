#include "Include\stdafx.h"
#include "Include\GeneralParser.h"
#include "Include\SymbolManager.h"
#include "Include\GeneralPushDownAutoMachine.h"
#include "Include\GeneralJumpInfo.h"
#include "Include\GeneralTreeNode.h"
#include "Include\ParserSymbol.h"
namespace ztl
{
	namespace general_parser
	{
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
			wstringToTreeNodeMap = move(InitTreeNodeMap());
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
					if(sign.find(make_pair(edges[i].edge, states.front().tokenIndex)) == sign.end())
					{
						sign.insert(make_pair(edges[i].edge, states.front().tokenIndex));
						states.emplace_back(states.front());
						states.back().SaveEdgeInfo(edges[i]);
						states.back().createdNodeStack = SaveCurrentStack(states.front().createdNodeStack);
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
			if(parserStates.front().createdNodeStack.size() != 1)
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
			this->parserStates.emplace_back(0, jumpInfos->GetRootNumber(), rootRule, MakeEmptyTreeNode());
			vector<GeneralTreeNode*> ambiguityRoots;
			while(!parserStates.empty())
			{
				while(parserStates.front().tokenIndex != (int) pools.GetTokenPool().size())
				{
					
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
					ambiguityRoots.emplace_back(parserStates.front().createdNodeStack.front());
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
			auto& createdNodeStack = state.createdNodeStack;
			auto& isRightRecursionEdge = state.edgeInfo.rightRecursion;
			HandleRightRecursionEdge(edge, rulePathStack, createdNodeStack, isRightRecursionEdge);
		}
		void GeneralParser::HandleRightRecursionEdge(PDAEdge* edge, vector<wstring>& rulePathStack, vector<GeneralTreeNode*>& createdNodeStack, bool isRightRecursionEdge)
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
						createdNodeStack.insert(createdNodeStack.begin() + pathOffset, number, MakeEmptyTreeNode());
						rulePathStack.insert(rulePathStack.begin() + pathOffset + 1, 
							make_reverse_iterator(ruleRequire.begin() + iter.back), 
							make_reverse_iterator(ruleRequire.begin() + iter.begin));
					}
				}
			}
		}
		bool GeneralParser::IsCorrectNode(GeneralTreeNode& node, const wstring& value) const
		{
			auto& nodeMap = machine->GetCreatedNodeRequiresMap();
			assert(nodeMap.find(value) != nodeMap.end());
			auto& fields = nodeMap[value].fieldNames;
			if (nodeMap[value].createType != node.GetName())
			{
				return false;
			}
			for(auto&&fieldName : fields)
			{
				if(!node.HaveThisField(fieldName))
				{
					return false;
				}
			}
			return true;
		}
		pair<bool, EdgeInfo> GeneralParser::CreateNodeResolve(const
			EdgeInfo& iter, const vector<GeneralTreeNode*>& createdNodeStack)
		{
			auto& signMap = wstringToTreeNodeMap;
			auto actions = iter.edge->GetActions();

			GeneralTreeNode current = *createdNodeStack.back();
			auto currentIndex = createdNodeStack.size() - 1;
			for(size_t i = 0; i < actions.size() && actions[i].GetActionType() != ActionType::Shift; ++i)
			{
				auto&& actionIter = actions[i];
				ActionType type = actionIter.GetActionType();
				bool isTerminate = false;
				switch(type)
				{
					case ztl::general_parser::ActionType::Using:
						if(!createdNodeStack[currentIndex - 1]->IsEmpty())
						{
							goto TestNextEdge;
						}
						--currentIndex;
						break;
					case ztl::general_parser::ActionType::Create:
						isTerminate = false;
						current.SetName(actionIter.GetName());
						if (!IsCorrectNode(current, actionIter.GetValue()))
						{
							goto TestNextEdge;
						}
						break;
					case ztl::general_parser::ActionType::Assign:
						--currentIndex;
						current = *createdNodeStack[currentIndex];
						if(isTerminate)
						{
							current.SetTermMap(actionIter.GetName(), -1);
						}
						else
						{
							current.SetFieldMap(actionIter.GetName(), -1);
						}
						break;
					case ztl::general_parser::ActionType::Setter:
						current.SetTermMap(actionIter.GetName(), -1);
						break;
					case ztl::general_parser::ActionType::Terminate:
						isTerminate = true;
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
			return{ true, iter };
		TestNextEdge:
			return{ false,{} };
		}

		vector<EdgeInfo> GeneralParser::CreateNodeResolve(const vector<EdgeInfo>& edges, ParserState& state)
		{
			assert(!edges.empty());
			vector<EdgeInfo> result;

			for(auto&& iter : edges)
			{
				pair<bool, EdgeInfo> resolve;
				if(iter.rightRecursion)
				{
					auto createdNodeStack = state.createdNodeStack;
					auto tokenIndex = state.tokenIndex;
					auto rulePathStack = state.rulePathStack;
					HandleRightRecursionEdge(iter.edge, rulePathStack, createdNodeStack, iter.rightRecursion);
					resolve = CreateNodeResolve(iter, createdNodeStack);
				}
				else
				{
					resolve = CreateNodeResolve(iter, state.createdNodeStack);
				}
				if(resolve.first)
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
			auto& createdNodeStack = state.createdNodeStack;
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
						createdNodeStack.emplace_back(MakeEmptyTreeNode());
						break;
					case ztl::general_parser::ActionType::Using:
						assert(rulePathStack.size() > 1);
						assert(rulePathStack.back() == actionIter.GetName() &&
							rulePathStack[rulePathStack.size() - 2] == actionIter.GetValue());
						rulePathStack.pop_back();
						assert(createdNodeStack.size() > 1);
						assert(createdNodeStack[createdNodeStack.size() - 2]->IsEmpty());
						swap(createdNodeStack[createdNodeStack.size() - 2], createdNodeStack[createdNodeStack.size() - 1]);
						createdNodeStack.pop_back();
						IsTerminate = false;
						break;
					case ztl::general_parser::ActionType::Terminate:
						assert(actionIter.GetFrom() == rulePathStack.back());
						assert(actionIter.GetName() == pools.GetTokenPool()[tokenIndex]->tag);
						IsTerminate = true;
						break;

					case ztl::general_parser::ActionType::Create:
						assert(actionIter.GetFrom() == rulePathStack.back());
						createdNodeStack.back()->SetName(actionIter.GetName());

						break;
					case ztl::general_parser::ActionType::Assign:
						if(IsTerminate)
						{
							pools.SetTerminatePool(pools.GetTokenPool()[tokenIndex]);
							createdNodeStack.back()->SetTermMap(actionIter.GetName(), (int) pools.GetTerminatePool().size() - 1);
						}
						else
						{
							assert(actionIter.GetFrom() == rulePathStack.back());
							assert(createdNodeStack.size() > 1);
							createdNodeStack[createdNodeStack.size() - 2]->SetFieldMap(actionIter.GetName(), createdNodeStack.back()->GetNumber());
							createdNodeStack.pop_back();
							rulePathStack.pop_back();
							assert(actionIter.GetTo() == rulePathStack.back());
						}
						break;
					case ztl::general_parser::ActionType::Setter:
						pools.SetTerminatePool(make_shared<TokenInfo>(actionIter.GetValue(), L"Setter", -1, -1));
						createdNodeStack.back()->SetTermMap(actionIter.GetName(), (int) pools.GetTerminatePool().size() - 1);
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
		wstring GeneralParser::GetCreatNodeStackInfo(ParserState& state) const
		{
			return accumulate(state.createdNodeStack.begin(), state.createdNodeStack.end(), wstring(), [this](const wstring& sum, GeneralTreeNode* node)
			{
				return	sum + node->GetNodeInfo();
			});
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
				L"current creatNodeStack: " + GetCreatNodeStackInfo(state);
		}
		unordered_map<wstring, shared_ptr<GeneralTreeNode>> GeneralParser::InitTreeNodeMap()
		{
			assert(manager != nullptr);
			unordered_map<wstring, shared_ptr<GeneralTreeNode>> result;
			for(auto&& iter : manager->GetTypeDefSymbolMap())
			{
				ParserSymbol* typeDefSymbol = iter.second;

				result.emplace(typeDefSymbol->GetName(), make_shared<GeneralTreeNode>(typeDefSymbol->GetName()));

				if(typeDefSymbol->IsClassType())
				{
					auto fieldSymbol = typeDefSymbol->GetClassAllFieldDefSymbol();
					for(auto&& symbol : fieldSymbol)
					{
						auto&& name = symbol->GetName();
						if(symbol->GetDescriptorSymbol()->IsTokenType() ||
							symbol->GetDescriptorSymbol()->IsEnumType())
						{
							result[typeDefSymbol->GetName()]->InitTermMap(name);
						}
						else
						{
							result[typeDefSymbol->GetName()]->InitFieldMap(name);
						}
					}
				}
				assert(typeDefSymbol->IsClassType() || typeDefSymbol->IsEnumType());
			}
			return result;
		}

		GeneralTreeNode* GeneralParser::MakeTreeNode(const wstring & nodeName)
		{
			auto&& findIter = wstringToTreeNodeMap.find(nodeName);
			if(findIter == wstringToTreeNodeMap.end())
			{
				throw ztl_exception(L"error!" + nodeName + L" isn't a vaild TreeNode's name");
			}
			pools.SetGeneralNodePool(make_shared<GeneralTreeNode>(*findIter->second));
			pools.GetGeneralNodePool().back()->SetNumber((int) pools.GetGeneralNodePool().size() - 1);
			return  pools.GetGeneralNodePool().back().get();
		}
		GeneralTreeNode*	GeneralParser::MakeEmptyTreeNode()
		{
			this->pools.SetGeneralNodePool(make_shared<GeneralTreeNode>());
			pools.GetGeneralNodePool().back()->SetNumber((int) pools.GetGeneralNodePool().size() - 1);
			return pools.GetGeneralNodePool().back().get();
		}
		GeneralTreeNode*	GeneralParser::CopyTreeNode(GeneralTreeNode* target)
		{
			auto node = MakeEmptyTreeNode();
			*node = *target;
			return node;
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