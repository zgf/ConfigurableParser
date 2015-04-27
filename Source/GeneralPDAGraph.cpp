#include "Include/stdafx.h"
#include "Include/GeneralPushDownAutoMachine.h"
#include "Include/GeneralPushDownMachineData.h"
#include "Include/GeneralTableDefine.h"
#include "Include/SymbolManager.h"
#include "Include/ParserSymbol.h"
#include "../Lib/ZTL/ztl_pair_builder.hpp"
namespace ztl
{
	namespace general_parser
	{
		class CreateEpsilonPDAVisitor: public GeneralGrammarTypeDefine::IVisitor
		{
		public:
			CreateEpsilonPDAVisitor() = default;
			~CreateEpsilonPDAVisitor() noexcept = default;
			CreateEpsilonPDAVisitor(CreateEpsilonPDAVisitor&&) noexcept = default;
			CreateEpsilonPDAVisitor(const CreateEpsilonPDAVisitor&) = default;
			CreateEpsilonPDAVisitor& operator=(CreateEpsilonPDAVisitor&&) noexcept = default;
			CreateEpsilonPDAVisitor& operator=(const CreateEpsilonPDAVisitor&) = default;
			CreateEpsilonPDAVisitor(PushDownAutoMachine* _machine)noexcept
				:machine(_machine)
			{
			}
		public:
			auto GetResult()const
			{
				return result;
			}

		public:
			void								Visit(GeneralGrammarTextTypeDefine* node)
			{
				result = machine->NewNodePair();
				auto tokenSymbol = machine->GetSymbolManager()->GetTokenSymbolByName(node->text);
				assert(tokenSymbol != nullptr&&tokenSymbol->IsTokenDef() && !tokenSymbol->IsIgnore());
				ActionWrap wrap(ActionType::Terminate, tokenSymbol);
				machine->AddEdge(result.first, result.second, move(wrap));
			}
			void								Visit(GeneralGrammarNormalTypeDefine* node)
			{
				result = machine->NewNodePair();
				auto ruleSymbol = machine->GetSymbolManager()->GetRuleSymbolByName(node->name);
				if(ruleSymbol)
				{
					ActionWrap wrap(ActionType::NonTerminate, ruleSymbol);
					machine->AddEdge(result.first, result.second, move(wrap));
				}
				else
				{
					auto tokenSymbol = machine->GetSymbolManager()->GetTokenSymbolByName(node->name);
					assert(tokenSymbol != nullptr&&tokenSymbol->IsTokenDef() && !tokenSymbol->IsIgnore());

					ActionWrap wrap(ActionType::Terminate, tokenSymbol);
					machine->AddEdge(result.first, result.second, move(wrap));
				}
			}
			void								Visit(GeneralGrammarSequenceTypeDefine* node)
			{
				node->first->Accept(this);
				auto firstResult = this->GetResult();
				node->second->Accept(this);
				auto secondResult = this->GetResult();
				result = machine->AddSequenceLinkNode(firstResult, secondResult);
			}
			void								Visit(GeneralGrammarLoopTypeDefine* node)
			{
				node->grammar->Accept(this);
				result = machine->AddLoopLinkNode(result.first, result.second);
			}
			void								Visit(GeneralGrammarOptionalTypeDefine* node)
			{
				node->grammar->Accept(this);
				result = machine->AddOptionalLinkNode(result.first, result.second);
			}
			void								Visit(GeneralGrammarSetterTypeDefine* node)
			{
				node->grammar->Accept(this);
				//Setter前一条边是Create
				assert(find_if(result.second->GetFronts().begin(), result.second->GetFronts().end(), [](PDAEdge*edge)
				{
					return edge->GetActions().back().GetActionType() != ActionType::Create;
				}) == result.second->GetFronts().end());
				assert(createTypeSymbol != nullptr);
				auto filedSymbol = createTypeSymbol->GetSubSymbolByName(node->name);
				assert(filedSymbol != nullptr&&filedSymbol->IsFieldDef());
				ActionWrap wrap(ActionType::Setter, filedSymbol, node->value);
				machine->FrontEdgesAdditionBackAction(result.second, move(wrap));
			}
			void								Visit(GeneralGrammarUsingTypeDefine* node)
			{
				node->grammar->Accept(this);
				assert(result.second->GetFronts().size() == 1);
				auto ruleSymbol = machine->GetSymbolManager()->GetCacheUsingGrammarToRuleDefSymbol(node->grammar.get());
				assert(ruleSymbol != nullptr&&ruleSymbol->IsRuleDef());
				ActionWrap wrap(ActionType::Using, ruleSymbol);
				machine->FrontEdgesAdditionBackAction(result.second, move(wrap));
			}
			void								Visit(GeneralGrammarCreateTypeDefine* node)
			{
				auto manager = machine->GetSymbolManager();
				createTypeSymbol = FindType(manager, manager->GetGlobalSymbol(), node->type.get());
				node->grammar->Accept(this);
				ActionWrap wrap(ActionType::Create, createTypeSymbol);
				//合并的节点可能是循环.直接Addition这样的话会导致create在循环内出现多次
				auto newNode = machine->NewNode();
				machine->AddEdge(result.second, newNode, move(wrap));
				result.second = newNode;
			}
			void								Visit(GeneralGrammarAlternationTypeDefine* node)
			{
				node->left->Accept(this);
				auto firstResult = this->GetResult();
				node->right->Accept(this);
				auto secondResult = this->GetResult();
				result = machine->AddAlterationLinkNode(firstResult, secondResult);
			}
			void								Visit(GeneralGrammarAssignTypeDefine* node)
			{
				auto ruleSymbol = machine->GetSymbolManager()->GetCacheNormalGrammarToRuleDefSymbol(node->grammar.get());
				ActionWrap wrap;
				if(ruleSymbol->IsTokenDef())
				{
					auto fieldSymbol = createTypeSymbol->SearchClassFieldSymbol(node->name);
					assert(fieldSymbol != nullptr&&fieldSymbol->IsFieldDef());
					wrap = ActionWrap(ActionType::Assign, fieldSymbol);
				}
				else
				{
					auto fieldSymbol = createTypeSymbol->SearchClassFieldSymbol(node->name);
					assert(fieldSymbol != nullptr&&fieldSymbol->IsFieldDef());
					auto fieldTypeSymbol = fieldSymbol->GetDescriptorSymbol();
					if(fieldTypeSymbol->IsArrayType())
					{
						fieldTypeSymbol = fieldTypeSymbol->GetDescriptorSymbol();
					}
					assert(fieldTypeSymbol->IsType() && !fieldTypeSymbol->IsArrayType());
					wrap = ActionWrap(ActionType::Assign, fieldSymbol);
				}
				node->grammar->Accept(this);
				machine->FrontEdgesAdditionBackAction(result.second, move(wrap));
			}
		private:
			PushDownAutoMachine* machine;
			pair<PDANode*, PDANode*> result;
			ParserSymbol* createTypeSymbol = nullptr;
		};
		void AddGeneratePDA(unordered_map<wstring, vector<pair<PDANode*, PDANode*>>>&  PDAMap, wstring ruleName, PDANode * start, PDANode* end)
		{
			auto findIter = PDAMap.find(ruleName);
			if(findIter == PDAMap.end())
			{
				PDAMap[ruleName];
			}
			PDAMap[ruleName].emplace_back(make_pair(start, end));
		}
		unordered_map<wstring, vector<pair<PDANode*, PDANode*>>> CreateEpsilonPDA(PushDownAutoMachine& machine)
		{
			unordered_map<wstring, vector<pair<PDANode*, PDANode*>>>  PDAMap;

			for(auto&& ruleIter : machine.GetTable()->rules)
			{
				auto rulePointer = ruleIter.get();
				for(auto&& grammarIter : ruleIter->grammars)
				{
					CreateEpsilonPDAVisitor visitor(&machine);
					grammarIter->Accept(&visitor);
					assert(visitor.GetResult().first != visitor.GetResult().second);

					AddGeneratePDA(PDAMap, rulePointer->name, visitor.GetResult().first, visitor.GetResult().second);
				}
			}
			//HelpLogJumpTable(L"LogJumpTable_RawTable.txt", machine);
			return PDAMap;
		}
		void AddPDAToPDAMachine(PushDownAutoMachine& machine, unordered_map<wstring, vector<pair<PDANode*, PDANode*>>>&  PDAMap)
		{
			for(auto&& ruleIter : PDAMap)
			{
				assert(ruleIter.second.size() > 0);
				machine.AddGeneratePDA(ruleIter.first, *ruleIter.second.begin());
			}
		}

		//可合并节点含义

		//同一个起点,包含同样信息的边,到达不同的节点,那么这个不同节点可以合并成同一个节点
		void MergeNodeByEdge(const vector<PDAEdge*>& edges, vector<PDANode*>& nodeList, PushDownAutoMachine& machine)
		{
			assert(!edges.empty());
			assert(is_sorted(edges.begin(), edges.end(), [](const PDAEdge* left, const PDAEdge* right)
			{
				return left->GetActions().size() == right->GetActions().size() ?
					left->GetActions() < right->GetActions() :
					left->GetActions().size() < right->GetActions().size();
			}));

			auto startIter = edges.begin();
			while(startIter != edges.end())
			{
				auto& actions = (*startIter)->GetActions();
				auto endIter = std::find_if_not(startIter, edges.end(), [&actions](const PDAEdge* element)
				{
					return element->GetActions() == actions;
				});
				if(distance(startIter, endIter) > 1)
				{
					auto&& save = (*startIter)->GetTarget();
					for_each(startIter + 1, endIter, [&save, &nodeList, &machine](PDAEdge* element)
					{
						machine.MergeIndependentNodes(save, element->GetTarget());
						machine.DeleteEdge(element);
						if(save->GetNexts().size() > 1)
						{
							nodeList.emplace_back(save);
						}
					});
				}
				startIter = endIter;
			}
		}
		void MergeCommonNode(PDANode* nodeIter, vector<PDANode*>& nodeList, PushDownAutoMachine& machine)
		{
			vector<PDAEdge*> edges(nodeIter->GetNexts().begin(), nodeIter->GetNexts().end());

			sort(edges.begin(), edges.end(), [](const PDAEdge* left, const PDAEdge* right)
			{
				return left->GetActions().size() == right->GetActions().size() ?
					left->GetActions() < right->GetActions() :
					left->GetActions().size() < right->GetActions().size();
			});

			if(!edges.empty())
			{
				MergeNodeByEdge(edges, nodeList, machine);
			}
		}
		template<typename prediction_type>
		vector<PDANode*> CollectGraphNode(const vector<PDANode*>& nodeList, const prediction_type& pred)
		{
			unordered_set<PDANode*>sign;
			deque<PDANode*> queue;
			vector<PDANode*> result;
			for(auto&&iter : nodeList)
			{
				queue.emplace_back(iter);
				while(!queue.empty())
				{
					PDANode* front = queue.front();
					queue.pop_front();
					if(sign.find(front) == sign.end())
					{
						sign.insert(front);
						if(pred(front))
						{
							result.emplace_back(front);
						}
						for(auto&& edgeIter : front->GetNexts())
						{
							queue.emplace_back(edgeIter->GetTarget());
						}
					}
				}
			}
			return result;
		}
		template<typename prediction_type>
		void TravelGraphNode(const vector<PDANode*>& nodeList, const prediction_type& pred)
		{
			unordered_set<PDANode*>sign;
			deque<PDANode*> queue;
			for(auto&&iter : nodeList)
			{
				queue.emplace_back(iter);
				while(!queue.empty())
				{
					PDANode* front = queue.front();
					queue.pop_front();
					if(sign.find(front) == sign.end())
					{
						sign.insert(front);
						pred(front);
						for(auto&& edgeIter : front->GetNexts())
						{
							queue.emplace_back(edgeIter->GetTarget());
						}
					}
				}
			}
		}

		void MergeGrammarCommonFactor(PushDownAutoMachine& machine)
		{
			vector<PDANode*>nodeList;
			//收集最左的边>2的节点
			for(auto&&iter : machine.GetPDAMap())
			{
				auto nodeIter = iter.second.first;
				if(nodeIter->GetNexts().size() > 1)
				{
					nodeList.emplace_back(nodeIter);
				}
			}

			for(size_t i = 0; i < nodeList.size(); ++i)
			{
				auto&& iter = nodeList[i];

				assert(iter->GetNexts().size()>1);
				MergeCommonNode(iter, nodeList, machine);
			}
		}
		
		void MergeStartAndEndNode(PushDownAutoMachine& machine, unordered_map<wstring, vector<pair<PDANode*, PDANode*>>>& PDAMap)
		{
			for(auto&& ruleIter : PDAMap)
			{
				auto& grammars = ruleIter.second;
				assert(!grammars.empty());
				for(size_t i = 0; i < grammars.size(); ++i)
				{
					machine.MergeIndependentNodes(grammars[0].first, grammars[i].first);
					machine.MergeIndependentNodes(grammars[0].second, grammars[i].second);
				}
				grammars.erase(grammars.begin() + 1, grammars.end());
			}
		}
		void CollectNodeAndEdgeMap(PushDownAutoMachine& machine)
		{
			unordered_set<PDANode*>sign;
			for(auto&&iter : machine.GetSymbolManager()->GetStartRuleList())
			{
				deque<PDANode*> queue;
				queue.emplace_back(machine.GetPDAMap()[iter].first);
				while(!queue.empty())
				{
					PDANode* front = queue.front();
					queue.pop_front();
					if(sign.find(front) == sign.end())
					{
						sign.insert(front);
						machine.AddNodeMapElement(front, machine.GetSymbolManager()->GetRuleIndexByName(iter));
						for(auto&& edgeIter : front->GetNexts())
						{
							machine.AddNodeEdgeMapElement(front, edgeIter);
							queue.emplace_back(edgeIter->GetTarget());
						}
					}
				}
			}
		}
		void FindThePath(PDANode* start, PDANode* end, unordered_set<PDAEdge*>& sign, vector<PDAEdge*>& result, bool& finish)
		{
			if(start != end)
			{
				for(auto&&edgeIter : start->GetNexts())
				{
					if(sign.find(edgeIter) == sign.end())
					{
						sign.insert(edgeIter);
						result.emplace_back(edgeIter);
						FindThePath(edgeIter->GetTarget(), end, sign, result, finish);
						if(finish == true)
						{
							return;
						}
						result.pop_back();
						sign.erase(edgeIter);

					}
				}
			}
			else
			{
				assert(start == end);
				finish = true;
			}
		}
		vector<PDAEdge*> FindThePath(PDANode* start, PDANode* end)
		{
			vector<PDAEdge*> result;
			unordered_set<PDAEdge*> sign;
			bool finish = false;
			FindThePath(start, end, sign, result, finish);
			return result;
		}
		void CreateDPDAGraph(PushDownAutoMachine& machine)
		{
			auto PDAMap = CreateEpsilonPDA(machine);
			MergeStartAndEndNode(machine, PDAMap);
			AddPDAToPDAMachine(machine, PDAMap);
			MergeGrammarCommonFactor(machine);
			CollectNodeAndEdgeMap(machine);
		//	LogGraphInfo(machine.GetSymbolManager()->GetGenerateUniqueProperty(L"filename") + L"NonMerge.txt", machine);
		}
	}
}