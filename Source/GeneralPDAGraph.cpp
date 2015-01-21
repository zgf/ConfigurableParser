#include "Include/stdafx.h"
#include "Include/GeneralPushDownAutoMachine.h"
#include "Include/GeneralTableDefine.h"
#include "Include/SymbolManager.h"
namespace ztl
{
	namespace general_parser
	{

		class CreateEpsilonPDAVisitor: public GeneralGrammarTypeDefine::IVisitor
		{
		public:
			CreateEpsilonPDAVisitor()  = default;
			~CreateEpsilonPDAVisitor() noexcept = default;
			CreateEpsilonPDAVisitor(CreateEpsilonPDAVisitor&&) noexcept = default;
			CreateEpsilonPDAVisitor(const CreateEpsilonPDAVisitor&) noexcept = default;
			CreateEpsilonPDAVisitor& operator=(CreateEpsilonPDAVisitor&&) noexcept = default;
			CreateEpsilonPDAVisitor& operator=(const CreateEpsilonPDAVisitor&)  = default;
			CreateEpsilonPDAVisitor(PushDownAutoMachine* _machine,GeneralRuleDefine* _rule)noexcept
				:machine(_machine),rule(_rule)
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
				machine->AddEdge(result.first, result.second, ActionType::Terminate,node);
			}
			void								Visit(GeneralGrammarNormalTypeDefine* node)
			{
				if (machine->GetSymbolManager()->GetCacheRuleNameToSymbol(node->name))
				{
					result = machine->NewNodePair();
					machine->AddEdge(result.first, result.second, ActionType::NonTerminate, node);
				}
				else
				{
					machine->GetSymbolManager()->GetCacheTokenNameToSymbol(node->name);
					result = machine->NewNodePair();
					machine->AddEdge(result.first, result.second, ActionType::Terminate, node);
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
				result = machine->AddLoopLinkNode(result.second, result.first);
			}
			void								Visit(GeneralGrammarOptionalTypeDefine* node)
			{
				node->grammar->Accept(this);
				result = machine->AddOptionalLinkNode(result.first, result.second);
			}
			void								Visit(GeneralGrammarSetterTypeDefine* node)
			{
				node->grammar->Accept(this);
				auto newNode = machine->NewNode();
				machine->AddEdge(result.second, newNode, ActionType::Setter, node);
				result.second = newNode;
			}
			void								Visit(GeneralGrammarUsingTypeDefine* node)
			{
				node->grammar->Accept(this);
				auto newNode = machine->NewNode();
				machine->AddEdge(result.second, newNode, ActionType::Using, node);
				result.second = newNode;
			}
			void								Visit(GeneralGrammarCreateTypeDefine* node)
			{
				node->grammar->Accept(this);
				auto newNode = machine->NewNode();
				machine->AddEdge(result.second, newNode, ActionType::Create, node);
				result.second = newNode;
			}
			void								Visit(GeneralGrammarAlterationTypeDefine* node)
			{
				node->left->Accept(this);
				auto firstResult = this->GetResult();
				node->right->Accept(this);
				auto secondResult = this->GetResult();
				result = machine->AddAlterationLinkNode(firstResult, secondResult);
			}
			void								Visit(GeneralGrammarAssignTypeDefine* node)
			{
				node->grammar->Accept(this);
				auto newNode = machine->NewNode();
				machine->AddEdge(result.second, newNode, ActionType::Assign, node);
				result.second = newNode;
			}
		private:
			PushDownAutoMachine* machine;
			GeneralRuleDefine* rule;
			pair<PDANode*, PDANode*> result;
		};

		void CreateEpsilonPDAGraph(PushDownAutoMachine& machine)
		{
			for (auto&& ruleIter: machine.GetTable()->rules)
			{
				auto rulePointer = ruleIter.get();
				
				for(auto&& grammarIter : ruleIter->grammars)
				{
					CreateEpsilonPDAVisitor visitor(&machine, rulePointer);
					grammarIter->Accept(&visitor);
					machine.AddGeneratePDA(rulePointer,visitor.GetResult().first);
				}
			}
		}
		void LogEpsilonPDAGraph(const wstring& fileName,PushDownAutoMachine& machine)
		{
			wofstream output(fileName);
			for (auto&& ruleIter: machine.GetPDAMap())
			{
				auto frontSize = 0;
				auto ruleName = ruleIter.first->name;
				ruleName += L" :";
				frontSize = (ruleName).size();
				output << ruleName<< endl;
				vector<wstring> grammarStrings;
				for (auto&& grammarIter:ruleIter.second)
				{
					
				}
				
			}
		}


	}
}