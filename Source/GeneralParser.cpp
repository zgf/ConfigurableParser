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

		void GeneralParser::GenerateIsomorphismParserTree()
		{
			assert(!this->tokenPool.empty());
			auto rootRule = machine->GetRootRuleName();
			assert(!rootRule.empty());
			this->rulePathStack.emplace_back(rootRule);
			this->createdNodeStack.emplace_back(MakeEmptyTreeNode());
			auto currentNodeIndex = jumpTable->GetRootNumber();
			size_t tokenIndex = 0;
			while(tokenIndex != tokenPool.size())
			{
				bool isRightRecursionEdge = false;
				if (tokenIndex == 90)
				{
					int a = 0;
				}
				wcout << currentNodeIndex << endl;
				auto edge = EdgeResolve(currentNodeIndex, tokenIndex, isRightRecursionEdge);
				HandleRightRecursionEdge(edge, isRightRecursionEdge);
				ExecuteEdgeActions(edge, tokenIndex);
				++tokenIndex;
				currentNodeIndex = edge->GetTarget()->GetNumber();
			}
			assert(createdNodeStack.size() == 1);
			treeRoot = createdNodeStack.front();
		}

		shared_ptr<void> GeneralParser::GeneralParserTree()
		{
			GenerateIsomorphismParserTree();
			return GeneralHeterogeneousParserTree();
		}
		PDAEdge* GeneralParser::EdgeResolve(int number, int tokenIndex, bool& isRightRecursionEdge)
		{
			return TerminateResolve(number, tokenIndex, isRightRecursionEdge);
		}
		PDAEdge * GeneralParser::TerminateResolve(int number, int tokenIndex, bool& isRightRecursionEdge)
		{
			auto&& token = this->tokenPool[tokenIndex];

			auto edges = jumpTable->GetPDAEdgeByTerminate(number, token->tag);
			if(edges == nullptr || edges->empty())
			{
				throw ztl_exception(L"error!can't find match token,tokenInfo.\n" + GetParserInfo(tokenIndex));
			}
			return RuleResolve(edges, tokenIndex, isRightRecursionEdge);
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
		PDAEdge * GeneralParser::RuleResolve(vector<PDAEdge*>* edges, int tokenIndex, bool& isRightRecursionEdge)
		{
			assert(edges != nullptr && !edges->empty());
			vector<PDAEdge*> result;
			for(auto&& iter : *edges)
			{
				auto&& ruleRequire = jumpTable->GetRuleRequires(iter);
				auto rbegin = make_reverse_iterator(rulePathStack.end());
				auto rend = make_reverse_iterator(rulePathStack.end() - ruleRequire.size());
				if(std::equal(ruleRequire.begin(), ruleRequire.end(), rbegin, rend))
				{
					result.emplace_back(iter);
				}
			}
			if(result.empty())
			{
				for(auto&& iter : *edges)
				{
					auto&& ruleRequire = jumpTable->GetRuleRequires(iter);
					if (IsRightRecursionGrammar(ruleRequire))
					{
						isRightRecursionEdge = true;
						result.emplace_back(iter);
					}
				}
			}
			if(result.empty())
			{
				throw ztl_exception(L"error!can't find match token'rule.\n" + GetParserInfo(tokenIndex));
			}
			return CreateNodeResolve(result, tokenIndex);
		}
		void GeneralParser::HandleRightRecursionEdge(PDAEdge * edge, bool isRightRecursionEdge)
		{
			if (isRightRecursionEdge)
			{
				auto&& ruleRequire = jumpTable->GetRuleRequires(edge);
				auto rulePathIndex = rulePathStack.size() - 1;
				auto iter = find_if_not(ruleRequire.begin(),ruleRequire.end(),[this,&rulePathIndex](const wstring& value)
				{
					return value == rulePathStack[rulePathIndex--];
				});
				auto position = rulePathIndex + 2;
				rulePathStack.insert(rulePathStack.begin()+ position, *iter);
				createdNodeStack.insert(createdNodeStack.begin() + position, MakeEmptyTreeNode());
			}
		}
		PDAEdge* GeneralParser::CreateNodeResolve(const vector<PDAEdge*>& edges, int tokenIndex)
		{
			static unordered_map<wstring, GeneralTreeNode> signMap = InitTreeNodeMap();
			static auto chioceFiledMap = InintChoiceFiledMap();

			assert(!edges.empty());
			vector<PDAEdge*> result;
			
			for(auto&& iter : edges)
			{
				auto actions = iter->GetActions();

				GeneralTreeNode current = *createdNodeStack.back();
				auto currentIndex = createdNodeStack.size() - 1;
				for(size_t i = 0; i < actions.size()&&actions[i].GetActionType() != ActionType::Shift;++i)
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
							if (chioceFiledMap.find(actionIter.GetName()) == chioceFiledMap.end() )
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
							if (isTerminate)
							{
								current.SetTermMap(actionIter.GetName(), -1);
							}
							else
							{
								current.SetFieldMap(actionIter.GetName(), -1);
							}
							break;
						case ztl::general_parser::ActionType::Setter:
							current.SetTermMap(actionIter.GetName(),-1);
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
				throw ztl_exception(L"error!can't find match token'rule.\n" + GetParserInfo(tokenIndex));
			}
			if(result.size() > 1)
			{
				throw ztl_exception(L"error ambiguous parse! find more match edge.\n" + GetParserInfo(tokenIndex));
			}
			assert(result.size() == 1);
			return result[0];
		}
		void GeneralParser::ExecuteEdgeActions(PDAEdge* edge, int tokenIndex)
		{
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
							/*if (actionIter.GetFrom() != actionIter.GetTo())
							{*/
								//非右递归的assgin
							//	assert(actionIter.GetFrom() != actionIter.GetTo());
								assert(actionIter.GetFrom() == rulePathStack.back());
								assert(createdNodeStack.size() > 1);
								createdNodeStack[createdNodeStack.size() - 2]->SetFieldMap(actionIter.GetName(), createdNodeStack.back()->GetNumber());
								createdNodeStack.pop_back();
								rulePathStack.pop_back();
								assert(actionIter.GetTo() == rulePathStack.back());
							//}
							//else
							//{
							//	//右递归生成的assgin.也就是shift应该有一个左递归,但实际无左递归的shift,所以要"插入"一个节点.文法不变
							//	assert(actionIter.GetFrom() == actionIter.GetTo());
							//	assert(actionIter.GetFrom() == rulePathStack.back());
							//	createdNodeStack.emplace_back(MakeEmptyTreeNode());
							//	swap(createdNodeStack[createdNodeStack.size() - 2], createdNodeStack[createdNodeStack.size() - 1]);
							//	createdNodeStack[createdNodeStack.size() - 2]->SetFieldMap(actionIter.GetName(), createdNodeStack.back()->GetNumber());
							//	createdNodeStack.pop_back();
							//	assert(actionIter.GetTo() == rulePathStack.back());
							//}

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
		wstring GeneralParser::GetRulePathInfo() const
		{
			return accumulate(rulePathStack.begin(), rulePathStack.end(), wstring(), [](const wstring& sum, const wstring& value)
			{
				return sum + L" " + value;
			}) + L"\n";
		}
		wstring GeneralParser::GetCreatNodeStackInfo() const
		{
			return accumulate(createdNodeStack.begin(), createdNodeStack.end(), wstring(), [this](const wstring& sum, GeneralTreeNode* node)
			{
				return	sum + node->GetNodeInfo();
			});
		}
		
		pair<bool, int>	GeneralParser::CheckCreateNodeRequire(int createStackIndex, const GeneralTreeNode& node, vector<wstring>* exclude)
		{
			assert(createStackIndex >= 0);

			for(; createStackIndex >= 0; --createStackIndex)
			{
				auto current = createdNodeStack[createStackIndex];
				if(!current->IsEmpty())
				{
					if (
							(exclude == nullptr&&
							node.IsMayDeriveType(*current))||
							(exclude != nullptr&& 
							node.IsMayDeriveType(*current, *exclude))
						
						)
					{
						return{ true,createStackIndex };
					}
					return{ false, 0 };
				}
			}
			return{ false, 0 };
		}
		bool GeneralParser::CheckCreateNodeRequires(const vector<CreateInfo>& requires, vector<wstring>* exclude)
		{
		
				static unordered_map<wstring, GeneralTreeNode> signMap = InitTreeNodeMap();
				auto createStackIndex = createdNodeStack.size() - 1;
				for(int i = requires.size() - 1; i >= 0; --i, --createStackIndex)
				{
					auto&& current = requires[i];
					auto&& createTypeName = current.createType;
					auto&& fieldName = current.fieldName;
					assert(signMap.find(createTypeName) != signMap.end());
					auto&& result = CheckCreateNodeRequire(createStackIndex, signMap[createTypeName], exclude);
				
					if(result.first == false)
					{
						return false;
					}
					else
					{
						createStackIndex = result.second;
					}
				}
				return true;
			
		}

		wstring GeneralParser::GetParserInfo(int tokenIndex) const
		{
			return L" current rule Path" + GetRulePathInfo() +
				L"current token info" + tokenPool[tokenIndex]->GetTokenInfo() +
				L"current creatNodeStack: " + GetCreatNodeStackInfo();
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
						if (symbol->GetDescriptorSymbol()->IsTokenType()||
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
		unordered_map<wstring,vector<wstring>> GeneralParser::InintChoiceFiledMap()
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
				if (!temp.empty())
				{
					result.emplace(typeDefSymbol->GetName(), move(temp));
				}
			}
			return result;
		}
		unordered_map<wstring, wstring> GeneralParser::InitDeriveToBaseMap()
		{
			unordered_map<wstring, wstring> result;
			auto&& symbolMap = manager->GetbaseSymbolToDeriveMap();
			for(auto&&iter : symbolMap)
			{
				auto baseSymbol = iter.first;
				if(baseSymbol != nullptr)
				{
					//baseSymbol == nullptr是global
					auto&& deriveSymbols = iter.second;
					for(auto&& deriveIter : deriveSymbols)
					{
						if(result.find(deriveIter->GetName()) == result.end())
						{
							result.emplace(deriveIter->GetName(), baseSymbol->GetName());
						}
					}
				}
			}
			return result;
		}

		bool GeneralParser::IsDeriveRelateion(const wstring& base, const wstring& derive)
		{
			static unordered_map<wstring, wstring> deriveToBaseMap = InitDeriveToBaseMap();

			auto work = derive;
			while(work != base)
			{
				auto findIter = deriveToBaseMap.find(work);
				if(findIter == deriveToBaseMap.end())
				{
					return false;
				}
				work = findIter->second;
			}
			return true;
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
	}
}