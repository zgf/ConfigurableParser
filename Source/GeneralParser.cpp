#include "Include\stdafx.h"
#include "Include\GeneralParser.h"
#include "Include\SymbolManager.h"
#include "Include\GeneralPushDownAutoMachine.h"
#include "Include\GeneralJumpTable.h"
#include "Include\GeneralFile.h"
#include "Include\GeneralParser.h"
#include "Include\GeneralTreeNode.h"
#include "Include\ParserSymbol.h"
namespace ztl
{
	namespace general_parser
	{
		GeneralParser::GeneralParser(const vector<shared_ptr<TokenInfo>>& tokens, const shared_ptr<GeneralTableDefine>& _tableDefine)
			: tokenPool(tokens),
			treeRoot(nullptr),
			tableDefine(_tableDefine),
			jumpTable(nullptr),
			machine(nullptr),
			manager(nullptr)

		{
		}

		void GeneralParser::BuildParser()
		{
			manager = make_shared<SymbolManager>(this->tableDefine);
			ValidateGeneratorCoreSemantic(manager.get());
			machine = make_shared<PushDownAutoMachine >(manager.get());
			CreateDPDAGraph(*machine.get());
			jumpTable = make_shared<GeneralJumpTable>(machine.get());
			CreateJumpTable(*jumpTable.get());
			//HelpLogJumpTable(L"LogJumpTable_MergeNoTermGraphTable.txt", *jumpTable);
		}

		void	GeneralParser::SaveEdge(deque<ParserState>& states,const vector<EdgeInfo>& edges)
		{
			if(!edges.empty())
			{
				states.front().SaveEdgeInfo(edges.front());
				for(size_t i = 1; i < edges.size(); ++i)
				{
					states.emplace_back(states.front());
					states.back().SaveEdgeInfo(edges[i]);
					states.back().createdNodeStack = SaveCurrentStack(states.front().createdNodeStack);
				}
			}
			else
			{
				states.pop_front();
			}
		
		}
		void GeneralParser::GenerateIsomorphismParserTree()
		{
			assert(!this->tokenPool.empty());
			auto rootRule = machine->GetRootRuleName();
			assert(!rootRule.empty());
			this->parserStates.emplace_back(0, jumpTable->GetRootNumber(), rootRule, MakeEmptyTreeNode());
			while(parserStates.front().tokenIndex != (int)tokenPool.size())
			{
				if(parserStates.front().tokenIndex == 486)
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



			assert(parserStates.front().createdNodeStack.size() == 1);
			treeRoot = parserStates.front().createdNodeStack.front();
		}

		shared_ptr<void> GeneralParser::GeneralParserTree()
		{
			GenerateIsomorphismParserTree();
			return GeneralHeterogeneousParserTree();
		}
		vector<EdgeInfo> GeneralParser::EdgeResolve(ParserState& state)
		{
			return TerminateResolve(state);
		}
		vector<EdgeInfo> GeneralParser::TerminateResolve(ParserState& state)
		{
			auto& tokenIndex = state.tokenIndex;
			auto& number = state.currentNodeIndex;
			auto&& token = this->tokenPool[tokenIndex];

			auto edges = jumpTable->GetPDAEdgeByTerminate(number, token->tag);
			if(edges == nullptr || edges->empty())
			{
				throw ztl_exception(L"error!can't find match token,tokenInfo.\n" + GetParserInfo(state));
			}
			return RuleResolve(edges, state);
		}
		bool IsRightRecursionGrammar(const vector<wstring>&ruleInfos)
		{
			unordered_set<wstring> sign;
			auto end = find_if(ruleInfos.begin(), ruleInfos.end(), [&sign](const wstring& value)
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
			return end != ruleInfos.end();
		}
		vector<EdgeInfo> GeneralParser::RuleResolve(vector<PDAEdge*>* edges, ParserState& state)
		{
			auto& rulePathStack = state.rulePathStack;
			auto& tokenIndex = state.tokenIndex;
			assert(edges != nullptr && !edges->empty());
			vector<EdgeInfo> result;
			for(auto i = 0; i < edges->size();++i)
			{
				auto&& iter = edges->operator[](i);
				auto&& ruleRequire = jumpTable->GetRuleRequires(iter);
				auto rbegin = make_reverse_iterator(rulePathStack.end());
				auto rend = rbegin + ruleRequire.size();

				if(std::equal(ruleRequire.begin(), ruleRequire.end(), rbegin, rend))
				{
					result.emplace_back(iter,false);
				}
			}
			if(result.empty())
			{
				for(auto&& iter : *edges)
				{
					auto&& ruleRequire = jumpTable->GetRuleRequires(iter);
					if(IsRightRecursionGrammar(ruleRequire))
					{
						result.emplace_back(iter,true);
					}
				}
			}
			if(result.empty())
			{
				throw ztl_exception(L"error!can't find match token'rule.\n" + GetParserInfo(state));
			}
			return CreateNodeResolve(result, state);
		}
		void GeneralParser::HandleRightRecursionEdge(ParserState& state)
		{
			auto& edge = state.edgeInfo.edge;
			auto& rulePathStack = state.rulePathStack;
			auto& createdNodeStack = state.createdNodeStack;
			auto& isRightRecursionEdge = state.edgeInfo.rightRecursion;
			if(isRightRecursionEdge)
			{
				auto&& ruleRequire = jumpTable->GetRuleRequires(edge);
				auto rulePathIndex = rulePathStack.size() - 1;
				auto iter = find_if_not(ruleRequire.begin(), ruleRequire.end(), [&rulePathStack, &rulePathIndex](const wstring& value)
				{
					return value == rulePathStack[rulePathIndex--];
				});
				auto position = rulePathIndex + 2;
				rulePathStack.insert(rulePathStack.begin() + position, *iter);
				createdNodeStack.insert(createdNodeStack.begin() + position, MakeEmptyTreeNode());
			}
		}
		vector<EdgeInfo> GeneralParser::CreateNodeResolve(const vector<EdgeInfo>& edges, ParserState& state)
		{
			static unordered_map<wstring, GeneralTreeNode> signMap = InitTreeNodeMap();
			static auto chioceFiledMap = InintChoiceFiledMap();
			auto& createdNodeStack = state.createdNodeStack;
			auto& tokenIndex = state.tokenIndex;

			assert(!edges.empty());
			vector<EdgeInfo> result;

			for(auto&& iter : edges)
			{
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
							if(chioceFiledMap.find(actionIter.GetName()) == chioceFiledMap.end())
							{
								if(!current.IsTheSameType(signMap[actionIter.GetName()]))
								{
									goto TestNextEdge;
								}
							}
							else
							{
								if(!current.IsTheSameType(signMap[actionIter.GetName()], chioceFiledMap[actionIter.GetName()]))
								{
									goto TestNextEdge;
								}
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
				result.emplace_back(iter);

			TestNextEdge:;
			}
			if(result.empty())
			{
				throw ztl_exception(L"error!can't find match token'rule.\n" + GetParserInfo(state));
			}
		/*	if(result.size() > 1)
			{
				throw ztl_exception(L"error ambiguous parse! find more match edge.\n" + GetParserInfo(state));
			}
			assert(result.size() == 1);*/
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
						assert(actionIter.GetValue() == rulePathStack.back());
						assert(actionIter.GetName() == tokenPool[tokenIndex]->tag);
						IsTerminate = true;
						break;

					case ztl::general_parser::ActionType::Create:
						assert(actionIter.GetValue() == rulePathStack.back());
						createdNodeStack.back()->SetName(actionIter.GetName());

						break;
					case ztl::general_parser::ActionType::Assign:
						if(IsTerminate)
						{
							terminatePool.emplace_back(tokenPool[tokenIndex]);
							createdNodeStack.back()->SetTermMap(actionIter.GetName(), terminatePool.size() - 1);
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
						terminatePool.emplace_back(make_shared<TokenInfo>(actionIter.GetValue(), L"Setter", -1, -1));
						createdNodeStack.back()->SetTermMap(actionIter.GetName(), terminatePool.size() - 1);
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
			return L" current rule Path" + GetRulePathInfo(state) +
				L"current token info" + tokenPool[state.tokenIndex]->GetTokenInfo() +
				L"current creatNodeStack: " + GetCreatNodeStackInfo(state);
		}
		unordered_map<wstring, GeneralTreeNode> GeneralParser::InitTreeNodeMap()
		{
			unordered_map<wstring, GeneralTreeNode> result;
			for(auto&& iter : manager->GetTypeDefSymbolMap())
			{
				ParserSymbol* typeDefSymbol = iter.second;

				result.emplace(typeDefSymbol->GetName(), GeneralTreeNode(typeDefSymbol->GetName()));

				if(typeDefSymbol->IsClassType())
				{
					auto fieldSymbol = typeDefSymbol->GetClassAllFieldDefSymbol();
					for(auto&& symbol : fieldSymbol)
					{
						auto&& name = symbol->GetName();
						if(symbol->GetDescriptorSymbol()->IsTokenType() ||
							symbol->GetDescriptorSymbol()->IsEnumType())
						{
							result[typeDefSymbol->GetName()].InitTermMap(name);
						}
						else
						{
							result[typeDefSymbol->GetName()].InitFieldMap(name);
						}
					}
				}
				assert(typeDefSymbol->IsClassType() || typeDefSymbol->IsEnumType());
			}
			return result;
		}
		unordered_map<wstring, vector<wstring>> GeneralParser::InintChoiceFiledMap()
		{
			vector<wstring> temp;
			unordered_map<wstring, vector<wstring>> result;
			for(auto&& iter : manager->GetTypeDefSymbolMap())
			{
				ParserSymbol* typeDefSymbol = iter.second;

				if(typeDefSymbol->IsClassType())
				{
					auto fieldSymbol = typeDefSymbol->GetClassAllFieldDefSymbol();
					for(auto&& symbol : fieldSymbol)
					{
						if(symbol->IsChoiceFieldDef())
						{
							temp.emplace_back(symbol->GetName());
						}
					}
				}
				assert(typeDefSymbol->IsClassType() || typeDefSymbol->IsEnumType());
				if(!temp.empty())
				{
					result.emplace(typeDefSymbol->GetName(), move(temp));
				}
			}
			return result;
		}
		
		GeneralTreeNode* GeneralParser::MakeTreeNode(const wstring & nodeName)
		{
			static unordered_map<wstring, GeneralTreeNode> signMap = InitTreeNodeMap();

			auto&& findIter = signMap.find(nodeName);
			if(findIter == signMap.end())
			{
				throw ztl_exception(L"error!" + nodeName + L" isn't a vaild TreeNode's name");
			}
			this->nodePool.emplace_back(make_shared<GeneralTreeNode>(findIter->second));
			nodePool.back()->SetNumber(nodePool.size() - 1);
			return nodePool.back().get();
		}
		GeneralTreeNode*	GeneralParser::MakeEmptyTreeNode()
		{
			this->nodePool.emplace_back(new GeneralTreeNode());
			nodePool.back()->SetNumber(nodePool.size() - 1);
			return nodePool.back().get();
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
	}
	

}