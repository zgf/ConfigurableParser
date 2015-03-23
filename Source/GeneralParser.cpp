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
			HelpLogJumpTable(L"LogJumpTable_MergeNoTermGraphTable.txt", *jumpTable);
		}

		void GeneralParser::GenerateIsomorphismParserTree()
		{
			assert(!this->tokenPool.empty());
			auto rootRule = machine->GetRootRuleName();
			assert(!rootRule.empty());
			this->rulePathStack.emplace_back(rootRule);
			auto currentNodeIndex = jumpTable->GetRootNumber();
			size_t tokenIndex = 0;
			while(tokenIndex != tokenPool.size())
			{
				wcout << currentNodeIndex<<endl;
				auto edge = EdgeResolve(currentNodeIndex, tokenIndex);
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
		PDAEdge* GeneralParser::EdgeResolve(int number, int tokenIndex)
		{
			return TerminateResolve(number,tokenIndex);
		}
		PDAEdge * GeneralParser::TerminateResolve(int number, int tokenIndex)
		{
			auto&& token = this->tokenPool[tokenIndex];

			auto edges = jumpTable->GetPDAEdgeByTerminate(number, token->tag);
			if(edges == nullptr || edges->empty())
			{
				throw ztl_exception(L"error!can't find match token,tokenInfo.\n" + GetParserInfo(tokenIndex));
			}
			return RuleResolve(edges, tokenIndex);
		}
		PDAEdge * GeneralParser::RuleResolve(vector<PDAEdge*>* edges, int tokenIndex)
		{
			assert(edges != nullptr && !edges->empty());
			vector<PDAEdge*> result;
			for(auto&& iter : *edges)
			{
				auto ruleRequire = jumpTable->GetRuleRequires(iter);
				auto rbegin = make_reverse_iterator(rulePathStack.end());
				auto rend = make_reverse_iterator(rulePathStack.end() - ruleRequire->size());
				if(std::equal(ruleRequire->begin(), ruleRequire->end(), rbegin, rend))
				{
					result.emplace_back(iter);
				}
			}
			if(result.empty())
			{
				throw ztl_exception(L"error!can't find match token'rule.\n" + GetParserInfo(tokenIndex));
			}
		
			return CreateNodeResolve(result, tokenIndex);
		}
		PDAEdge* GeneralParser::CreateNodeResolve(const vector<PDAEdge*>& edges, int tokenIndex)
		{
			
			assert(!edges.empty());
			vector<PDAEdge*> result;
			for (auto&& iter:edges)
			{
				auto requires = jumpTable->GetCreateNodeRequires(iter);
				assert(requires == nullptr||(requires != nullptr&&!requires->empty()));
				if (requires==nullptr)
				{
					result.emplace_back(iter);
				}
				else if(requires->size() <= createdNodeStack.size())
				{
					if (CheckCreateNodeRequires(*requires))
					{
						result.emplace_back(iter);
					}
				}
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
			for (auto&& actionIter : actions)
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
						assert(rulePathStack.back() == actionIter.GetName()&&
						rulePathStack[rulePathStack.size()-2] == actionIter.GetValue());
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
						if (IsTerminate)
						{
							terminatePool.emplace_back(tokenPool[tokenIndex]);
							createdNodeStack.back()->SetField(actionIter.GetName(), terminatePool.size() - 1);
						}
						else
						{
							assert(createdNodeStack.size() > 1);
							assert(createdNodeStack[createdNodeStack.size() - 2]->HaveThisField(actionIter.GetName()));
							createdNodeStack[createdNodeStack.size()-2]->SetField(actionIter.GetName(), createdNodeStack.back()->GetNumber());
							createdNodeStack.pop_back();
						}
						break;
					case ztl::general_parser::ActionType::Setter:
						terminatePool.emplace_back(make_shared<TokenInfo>(actionIter.GetValue(), L"Setter", -1, -1));
						createdNodeStack.back()->SetField(actionIter.GetName(), terminatePool.size() - 1);
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
			})+L"\n";
		}
		wstring GeneralParser::GetCreatNodeStackInfo() const
		{
			return accumulate(createdNodeStack.begin(), createdNodeStack.end(), wstring(), [this](const wstring& sum,GeneralTreeNode* node)
			{
				return	sum + node->GetNodeInfo();
			});
		}
		pair<bool,int> GeneralParser::CheckCreateNodeRequire(int createStackIndex,const GeneralTreeNode& node)
		{
			assert(createStackIndex >= 0);
			
			for(; createStackIndex >= 0;--createStackIndex)
			{
				
				auto current = createdNodeStack[createStackIndex];
				if (!current->IsEmpty())
				{
					if(!current->IsEqualType(node))
					{
						return {false, 0};
					}
					else
					{
						return{ true,createStackIndex };
					}
				}
			}

		}
		bool GeneralParser::CheckCreateNodeRequires(const vector<CreateInfo>& requires)
		{
			static unordered_map<wstring, GeneralTreeNode> signMap = InitTreeNodeMap();
			auto createStackIndex = createdNodeStack.size() - 1;
			for(int i = requires.size() - 1; i >= 0;--i,--createStackIndex)
			{
				auto&& current = requires[i];
				auto&& createTypeName = current.createType;
				auto&& fieldName = current.fieldName;
				assert(signMap.find(createTypeName) != signMap.end());
				
				auto result = CheckCreateNodeRequire(createStackIndex,signMap[createTypeName]);
				if (result.first == false)
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
			for (auto&& iter: manager->GetTypeDefSymbolMap())
			{
				ParserSymbol* typeDefSymbol = iter.second;
				
				result.emplace(typeDefSymbol->GetName(), GeneralTreeNode(typeDefSymbol->GetName()));
				
				if (typeDefSymbol->IsClassType())
				{
					auto fieldNames = typeDefSymbol->GetClassAllFieldName();
					for (auto&& name:fieldNames)
					{
						result[typeDefSymbol->GetName()].InitFieldMap(name);
					}
				}
				assert(typeDefSymbol->IsClassType() || typeDefSymbol->IsEnumType());
			}
			return result;
		}
		unordered_map<wstring, wstring> GeneralParser::InitDeriveToBaseMap()
		{
			unordered_map<wstring, wstring> result;
			auto&& symbolMap = manager->GetbaseSymbolToDeriveMap();
			for (auto&&iter:symbolMap)
			{
				auto baseSymbol = iter.first;
				if (baseSymbol!=nullptr)
				{
					//baseSymbol == nullptrÊÇglobal
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
			while(work!=base)
			{
				auto findIter = deriveToBaseMap.find(work);
				if (findIter == deriveToBaseMap.end())
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