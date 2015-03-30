#include "Include\stdafx.h"
#include "Include\GeneralParser.h"
#include "Include\SymbolManager.h"
#include "Include\GeneralPushDownAutoMachine.h"
#include "Include\GeneralJumpTable.h"
#include "Include\GeneralFile.h"
#include "Include\GeneralParser.h"
#include "Include\GeneralTreeNode.h"
#include "Include\ParserSymbol.h"
#include "..\Lib\ZTL\ztl_hash.hpp"
namespace ztl
{
	namespace general_parser
	{
		GeneralParser::GeneralParser(const vector<shared_ptr<TokenInfo>>& tokens, const shared_ptr<GeneralTableDefine>& _tableDefine)
			: tokenPool(tokens),
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
			//CreatReflectionFile(manager.get());
			HelpLogJumpTable(L"LogJumpTable_MergeNoTermGraphTable.txt", *jumpTable);
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
			if(states.empty())
			{
				throw ztl_exception(L"Can't find vail Parser Tree");
			}
		}
		void PrintRuntimeInfo(const vector<ParserState>& parserStates, const vector<shared_ptr<TokenInfo>>& tokenPool)
		{
			wcout << L"nodeIndex:" << parserStates.front().currentNodeIndex << endl;
			wcout << L"tokenIndex:" << parserStates.front().tokenIndex << L" token: " << tokenPool[parserStates.front().tokenIndex]->tag << endl;
			wcout << L"currentStack:" << accumulate(parserStates.front().rulePathStack.begin(), parserStates.front().rulePathStack.end(), wstring(), [](const wstring& sum, const wstring& value)
			{
				return sum + L" " + value;
			}) << endl;
		}
		void GeneralParser::GenerateIsomorphismParserTree()
		{
			assert(!this->tokenPool.empty());
			auto rootRule = machine->GetRootRuleName();
			assert(!rootRule.empty());
			this->parserStates.emplace_back(0, jumpTable->GetRootNumber(), rootRule, MakeEmptyTreeNode());
			while(!parserStates.empty())
			{
				while(parserStates.front().tokenIndex != (int) tokenPool.size())
				{
					//PrintRuntimeInfo(parserStates);
					auto edges = EdgeResolve(parserStates.front());
					SaveEdge(parserStates, edges);
					HandleRightRecursionEdge(parserStates.front());
					ExecuteEdgeActions(parserStates.front());
					++parserStates.front().tokenIndex;
					parserStates.front().currentNodeIndex = parserStates.front().GetEdge()->GetTarget()->GetNumber();
				}
				if(!parserStates.empty())
				{
					assert(parserStates.front().createdNodeStack.size() == 1);
					treeRoots.emplace_back(parserStates.front().createdNodeStack.front());
					parserStates.pop_front();
				}
			}
		}

		shared_ptr<void> GeneralParser::GeneralParserTree()
		{
			return shared_ptr<void>();
			//return GeneralHeterogeneousParserTree();
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
				/*throw ztl_exception(L"error!can't find match token,tokenInfo.\n" + GetParserInfo(state));*/
				return{};
			}
			return RuleResolve(edges, state);
		}
		template<typename iterator_type>
		auto FindRightRecursionPosition(iterator_type begin, iterator_type end)
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
		auto FindRightRecursionPosition(const vector<wstring>&ruleInfos)
		{
			return FindRightRecursionPosition(ruleInfos.begin(), ruleInfos.end());
		}
		bool IsRightRecursionGrammar(const vector<wstring>&ruleInfos)
		{
			auto end = FindRightRecursionPosition(ruleInfos);
			return end != ruleInfos.end();
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
				auto&& iter = edges->operator[](i);
				auto&& ruleRequire = jumpTable->GetRuleRequires(iter);

				if(rulePathStack.size() >= ruleRequire.size())
				{
					auto rbegin = make_reverse_iterator(rulePathStack.end());
					auto rend = rbegin + ruleRequire.size();
					auto miss = std::mismatch(ruleRequire.begin(), ruleRequire.end(), rbegin, rend);
					if(miss.first == ruleRequire.end() && miss.second == rend)
					{
						result.emplace_back(iter, false);
					}
					else
					{
						auto position = FindRightRecursionPosition(ruleRequire);
						if(position <= miss.first)
						{
							candicate.emplace_back(iter);
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
				/*throw ztl_exception(L"error!can't find match token'rule.\n" + GetParserInfo(state));*/
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
				auto&& ruleRequire = jumpTable->GetRuleRequires(edge);
				for(auto ruleRequireBegin = ruleRequire.begin(); ruleRequireBegin < ruleRequire.end();)
				{
					auto end = FindRightRecursionPosition(ruleRequireBegin, ruleRequire.end());
					if(end != ruleRequire.end())
					{
						auto first = find_if(ruleRequireBegin, end, [&end](const wstring& value)
						{
							return value == *end;
						});
						ruleRequireBegin = end;
						if(first != end)
						{
							auto offset = first - ruleRequire.begin();
							auto pathOffset = rulePathStack.size() - 1 - offset;
							assert(rulePathStack[pathOffset] == ruleRequire[offset]);
							auto number = end - first;
							createdNodeStack.insert(createdNodeStack.begin() + pathOffset, number, MakeEmptyTreeNode());
							rulePathStack.insert(rulePathStack.begin() + pathOffset + 1, make_reverse_iterator(end), make_reverse_iterator(first));
						}
					}
					else
					{
						break;
					}
				}
			}
		}
		pair<bool, EdgeInfo> GeneralParser::CreateNodeResolve(const
			EdgeInfo& iter, const vector<GeneralTreeNode*>& createdNodeStack)
		{
			static unordered_map<wstring, GeneralTreeNode> signMap = InitTreeNodeMap();
			static auto chioceFiledMap = InintChoiceFiledMap();
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
			return{ true, iter };
		TestNextEdge:
			return{ false,{} };
		}

		vector<EdgeInfo> GeneralParser::CreateNodeResolve(const vector<EdgeInfo>& edges, ParserState& state)
		{
			static unordered_map<wstring, GeneralTreeNode> signMap = InitTreeNodeMap();
			static auto chioceFiledMap = InintChoiceFiledMap();

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
			/*	if(result.empty())
				{
					throw ztl_exception(L"error!can't find match token'rule.\n" + GetParserInfo(state));
				}*/
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
							createdNodeStack.back()->SetTermMap(actionIter.GetName(), (int) terminatePool.size() - 1);
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
						createdNodeStack.back()->SetTermMap(actionIter.GetName(), (int) terminatePool.size() - 1);
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
			nodePool.back()->SetNumber((int) nodePool.size() - 1);
			return nodePool.back().get();
		}
		GeneralTreeNode*	GeneralParser::MakeEmptyTreeNode()
		{
			this->nodePool.emplace_back(new GeneralTreeNode());
			nodePool.back()->SetNumber((int) nodePool.size() - 1);
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
		GeneralTreeNode* GeneralParser::GetNonTermNodeByIndex(int index) const
		{
			return this->nodePool[index].get();
		}
		TokenInfo* GeneralParser::GetTermNodeByIndex(int index) const
		{
			return terminatePool[index].get();
		}
		void ReflectionBuidler(const wstring& className, const wstring& fieldName, void* classObject, void* valueObject)
		{
			using builderType = void(*)(void*, void*);
			static unordered_map<wstring, unordered_map<wstring, builderType>> builderMap =
			{
				{
					L"GeneralTableDefine",
					{
						{
							L"heads",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralTableDefine*>(classObject)->heads.emplace_back(static_cast<GeneralHeadInfoDefine*>(valueObject));
								return;
							}
						},{
							L"types",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralTableDefine*>(classObject)->types.emplace_back(static_cast<GeneralTypeDefine*>(valueObject));
								return;
							}
						},{
							L"tokens",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralTableDefine*>(classObject)->tokens.emplace_back(static_cast<GeneralTokenDefine*>(valueObject));
								return;
							}
						},{
							L"rules",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralTableDefine*>(classObject)->rules.emplace_back(static_cast<GeneralRuleDefine*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralClassMemberTypeDefine",
					{
						{
							L"type",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralClassMemberTypeDefine*>(classObject)->type.reset(static_cast<GeneralTypeObject*>(valueObject));
								return;
							}
						},{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralClassMemberTypeDefine*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralGrammarSetterTypeDefine",
					{
						{
							L"grammar",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarSetterTypeDefine*>(classObject)->grammar.reset(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarSetterTypeDefine*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},{
							L"value",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarSetterTypeDefine*>(classObject)->value = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralHeadInfoDefine",
					{
						{
							L"property",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralHeadInfoDefine*>(classObject)->property = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},{
							L"value",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralHeadInfoDefine*>(classObject)->value = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralGrammarLoopTypeDefine",
					{
						{
							L"grammar",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarLoopTypeDefine*>(classObject)->grammar.reset(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralNormalTypeObject",
					{
						{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralNormalTypeObject*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralGrammarCreateTypeDefine",
					{
						{
							L"type",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarCreateTypeDefine*>(classObject)->type.reset(static_cast<GeneralTypeObject*>(valueObject));
								return;
							}
						},{
							L"grammar",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarCreateTypeDefine*>(classObject)->grammar.reset(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralEnumMemberTypeDefine",
					{
						{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralEnumMemberTypeDefine*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralTokenDefine",
					{
						{
							L"regex",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralTokenDefine*>(classObject)->regex = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralTokenDefine*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},{
							L"ignore",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralTokenDefine*>(classObject)->ignore = *static_cast<GeneralTokenDefine::TokenOptional*>(valueObject);
								return;
							}
						},
					}
				},{
					L"GeneralGrammarSequenceTypeDefine",
					{
						{
							L"first",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarSequenceTypeDefine*>(classObject)->first.reset(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},{
							L"second",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarSequenceTypeDefine*>(classObject)->second.reset(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralTypeDefine",
					{
					}
				},{
					L"GeneralTypeObject",
					{
					}
				},{
					L"GeneralArrayTypeObject",
					{
						{
							L"element",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralArrayTypeObject*>(classObject)->element.reset(static_cast<GeneralTypeObject*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralStringTypeObject",
					{
					}
				},{
					L"GeneralSubTypeObject",
					{
						{
							L"parent",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralSubTypeObject*>(classObject)->parent.reset(static_cast<GeneralTypeObject*>(valueObject));
								return;
							}
						},{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralSubTypeObject*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralClassTypeDefine",
					{
						{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralClassTypeDefine*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},{
							L"subTypes",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralClassTypeDefine*>(classObject)->subTypes.emplace_back(static_cast<GeneralTypeDefine*>(valueObject));
								return;
							}
						},{
							L"parent",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralClassTypeDefine*>(classObject)->parent.reset(static_cast<GeneralTypeObject*>(valueObject));
								return;
							}
						},{
							L"members",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralClassTypeDefine*>(classObject)->members.emplace_back(static_cast<GeneralClassMemberTypeDefine*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralEnumTypeDefine",
					{
						{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralEnumTypeDefine*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},{
							L"members",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralEnumTypeDefine*>(classObject)->members.emplace_back(static_cast<GeneralEnumMemberTypeDefine*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralGrammarTypeDefine",
					{
					}
				},{
					L"GeneralGrammarTextTypeDefine",
					{
						{
							L"text",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarTextTypeDefine*>(classObject)->text = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralGrammarNormalTypeDefine",
					{
						{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarNormalTypeDefine*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralGrammarOptionalTypeDefine",
					{
						{
							L"grammar",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarOptionalTypeDefine*>(classObject)->grammar.reset(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralGrammarAssignTypeDefine",
					{
						{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarAssignTypeDefine*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},{
							L"grammar",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarAssignTypeDefine*>(classObject)->grammar.reset(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralGrammarUsingTypeDefine",
					{
						{
							L"grammar",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarUsingTypeDefine*>(classObject)->grammar.reset(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralGrammarAlternationTypeDefine",
					{
						{
							L"left",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarAlternationTypeDefine*>(classObject)->left.reset(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},{
							L"right",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarAlternationTypeDefine*>(classObject)->right.reset(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralRuleDefine",
					{
						{
							L"type",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralRuleDefine*>(classObject)->type.reset(static_cast<GeneralTypeObject*>(valueObject));
								return;
							}
						},{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralRuleDefine*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},{
							L"grammars",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralRuleDefine*>(classObject)->grammars.emplace_back(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},
					}
				},
			};
			auto findIter = builderMap.find(className);
			assert(findIter != builderMap.end());
			assert(findIter->second.find(fieldName) != findIter->second.end());
			builderMap[className][fieldName](classObject, valueObject);
		}
		shared_ptr<void> ReflecteObjectByName(const wstring& name)
		{
			using reflectObjectType = shared_ptr<void>(*)();
			static unordered_map < wstring, reflectObjectType> objectMap =
			{
				{
					L"GeneralTableDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralTableDefine>();
					}
				},
				{
					L"GeneralClassMemberTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralClassMemberTypeDefine>();
					}
				},
				{
					L"GeneralGrammarSetterTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarSetterTypeDefine>();
					}
				},
				{
					L"GeneralHeadInfoDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralHeadInfoDefine>();
					}
				},
				{
					L"GeneralGrammarLoopTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarLoopTypeDefine>();
					}
				},
				{
					L"GeneralNormalTypeObject",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralNormalTypeObject>();
					}
				},
				{
					L"GeneralGrammarCreateTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarCreateTypeDefine>();
					}
				},
				{
					L"GeneralEnumMemberTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralEnumMemberTypeDefine>();
					}
				},
				{
					L"GeneralTokenDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralTokenDefine>();
					}
				},
				{
					L"GeneralGrammarSequenceTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarSequenceTypeDefine>();
					}
				},
				{
					L"GeneralTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralTypeDefine>();
					}
				},
				{
					L"GeneralTypeObject",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralTypeObject>();
					}
				},
				{
					L"GeneralArrayTypeObject",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralArrayTypeObject>();
					}
				},
				{
					L"GeneralStringTypeObject",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralStringTypeObject>();
					}
				},
				{
					L"GeneralSubTypeObject",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralSubTypeObject>();
					}
				},
				{
					L"GeneralClassTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralClassTypeDefine>();
					}
				},
				{
					L"GeneralEnumTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralEnumTypeDefine>();
					}
				},
				{
					L"GeneralGrammarTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarTypeDefine>();
					}
				},
				{
					L"GeneralGrammarTextTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarTextTypeDefine>();
					}
				},
				{
					L"GeneralGrammarNormalTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarNormalTypeDefine>();
					}
				},
				{
					L"GeneralGrammarOptionalTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarOptionalTypeDefine>();
					}
				},
				{
					L"GeneralGrammarAssignTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarAssignTypeDefine>();
					}
				},
				{
					L"GeneralGrammarUsingTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarUsingTypeDefine>();
					}
				},
				{
					L"GeneralGrammarAlternationTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarAlternationTypeDefine>();
					}
				},
				{
					L"GeneralRuleDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralRuleDefine>();
					}
				},
			};
			assert(objectMap.find(name) != objectMap.end());
			return objectMap[name]();
		}
		void GeneralParser::HeterogeneousParse(GeneralTreeNode* classNode, shared_ptr<void>& classObject)
		{
			assert(classObject != nullptr);
			assert(classNode != nullptr);
			auto className = classNode->GetName();
			for(auto&& iter : classNode->GetFieldMap())
			{
				auto fieldName = iter.first;
				for(auto&& nodeIter : iter.second)
				{
					auto fieldNode = GetNonTermNodeByIndex(nodeIter);
					auto fieldObject = ReflecteObjectByName(fieldNode->GetName());
					heterogeneousNodePool.emplace_back(fieldObject);
					ReflectionBuidler(className, fieldName, classObject.get(), fieldObject.get());
					HeterogeneousParse(fieldNode, fieldObject);
				}
			}
			for(auto&&iter : classNode->GetTermMap())
			{
				auto fieldName = iter.first;
				for (auto&& nodeIter:iter.second)
				{
					auto termNode = GetTermNodeByIndex(nodeIter);
					ReflectionBuidler(className, fieldName, classObject.get(), &termNode);
				}
			}
		}
		shared_ptr<void> GeneralParser::HeterogeneousParse(GeneralTreeNode* root)
		{
			auto rootObject = ReflecteObjectByName(root->GetName());
			heterogeneousNodePool.emplace_back(rootObject);
			HeterogeneousParse(root, rootObject);
			return rootObject;
		}
		int WstringToEnumItem(const wstring& value)
		{
			static unordered_map<wstring, int> signMap =
			{
				{
					L"",
					1
				},
			};
			assert(signMap.find(value)!=signMap.end());
			return signMap[value];
		}
	}
}