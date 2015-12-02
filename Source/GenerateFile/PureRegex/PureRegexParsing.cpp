#include "Include/stdafx.h"
#include "PureRegexAutoMachineData.h"
#include "../../Include/GeneralLALRParser.h"
#include "PureRegexWriter.hpp"
#include "PureRegexParser.h"
#include "CharsetMapTable.h"
#include "PureRegexLexer.h"
#include "DFATableBuilder.h"
namespace ztl
{
	namespace pure_regex
	{
		//////////////////////////////////////////////////////////////////////////
		//Token To Ast
		//////////////////////////////////////////////////////////////////////////
		//输入tokens,解析出一堆AST

		vector<shared_ptr<Alternate>> ParsingAllTokensToAst(const vector<shared_ptr<TokenPacket>>& tokens)
		{
			vector<shared_ptr<Alternate>> result;
			ztl::general_parser::GeneralLALRParser LALRParser(ztl::pure_regex::BootStrapDefineTable());
			LALRParser.BuildParser();
			for(auto&&tokenIter : tokens)
			{
				LALRParser.SetTokenPool(ztl::pure_regex::PureRegexParseToken(tokenIter->regex));
				LALRParser.GenerateIsomorphismParserTree();
				result.emplace_back(static_pointer_cast<Alternate>(ztl::pure_regex::GeneralHeterogeneousParserTree(LALRParser)));
				LALRParser.ClearEnvironment();
			}
			return result;
		}

		//////////////////////////////////////////////////////////////////////////
		//获取字符映射表
		//////////////////////////////////////////////////////////////////////////
		//userdefine替换掉
		class BuildCharsetVisitor: public Factor::IVisitor
		{
		public:
			BuildCharsetVisitor():table(make_shared<CharsetMapTable>())
			{

			}
			~BuildCharsetVisitor() noexcept = default;
			BuildCharsetVisitor(BuildCharsetVisitor&&) noexcept = default;
			BuildCharsetVisitor(const BuildCharsetVisitor&) noexcept = default;
			BuildCharsetVisitor& operator=(BuildCharsetVisitor&&) noexcept = default;
			BuildCharsetVisitor& operator=(const BuildCharsetVisitor&)noexcept = default;
		public:
			void		Visit(Define*) override
			{
			}
			void		Visit(TokenUnit* node) override
			{
				node->charset->Accept(this);
			}
			void		Visit(CharSet* node) override
			{
				for(auto&&iter : node->tokens)
				{
					iter->Accept(this);
				}
			}
			void		Visit(BinaryFactor* node)override
			{
				table->AddCharRange(node->begin->name[0], node->end->name[0]);
			}
			void		Visit(UserDefineFactor* node) override
			{
				//调用之前调整AST,把userdefine的全部换掉.;
				node->factor = MakeUseDefinChaSet(node->type);
				node->factor->Accept(this);
			}
			void		Visit(NormalFactor* node)override
			{
				table->AddChar(node->name[0]);
			}
			void		Visit(Repeat*) override
			{
			}
			void		Visit(Alternate* node)override
			{
				node->left->Accept(this);
				node->right->Accept(this);
			}
			void		Visit(Sequence* node)override
			{
				node->first->Accept(this);
				node->second->Accept(this);
			}
			void		Visit(Factor*)override
			{
			}
		public:
			shared_ptr<CharsetMapTable> GetResult()const
			{
				return table;
			}
		private:
			shared_ptr<CharsetMapTable> table;
		};

		shared_ptr<CharsetMapTable> BuildCharsetMapTable(const vector<shared_ptr<Alternate>>& roots)
		{
			assert(!roots.empty());
			BuildCharsetVisitor visitor;
			for(auto&&rootIter : roots)
			{
				rootIter->Accept(&visitor);
			}
			visitor.GetResult()->AddEnd();
			return visitor.GetResult();
		}
		///////////////////////////////////////////////////////////////////////////
		//AST to NFA
		//////////////////////////////////////////////////////////////////////////
		class BuildNFAVistor:public Define::IVisitor
		{
		public:
			BuildNFAVistor() = delete;
			BuildNFAVistor(DFATableBuilder* _builder) :builder(_builder), start(nullptr), end(nullptr)
			{
			}
			~BuildNFAVistor() noexcept = default;
			BuildNFAVistor(BuildNFAVistor&&) noexcept = default;
			BuildNFAVistor(const BuildNFAVistor&) noexcept = default;
			BuildNFAVistor& operator=(BuildNFAVistor&&) noexcept = default;
			BuildNFAVistor& operator=(const BuildNFAVistor&) noexcept = default;

		public:
			NFANode* GetEnd()const
			{
				assert(end != nullptr);
				return end;
			}
			NFANode* GetStart()const
			{
				assert(start != nullptr);
				return start;
			}
			void		Visit(Define*)
			{
			}
			void		Visit(TokenUnit* node)
			{
				node->charset->Accept(this);
				auto type = node->repeat->type;
				switch(type)
				{
					case ztl::pure_regex::Repeat::RepeatType::KLEENE:
						builder->AddNullLink(end, start);
						builder->AddNullLink(start, end);
						break;
					case ztl::pure_regex::Repeat::RepeatType::POSITIVEKLEENE:
						builder->AddNullLink(end, start);
						break;
					case ztl::pure_regex::Repeat::RepeatType::CHOCIEKLEENE:
						builder->AddNullLink(start, end);
						break;
					default:
						assert(false);
						break;
				}
			}
			void		Visit(CharSet* node)
			{
				for(auto&&iter : node->tokens)
				{
					iter->Accept(this);
				}
			}
			void		Visit(Factor*)
			{
			}
			void		Visit(UserDefineFactor* node)
			{
				assert(node->factor != nullptr);
				node->factor->Accept(this);
			}
			void		Visit(BinaryFactor* node)
			{
				start = builder->GetNewNode();
				end = builder->GetNewNode();
				assert(node->begin != nullptr);
				assert(node->end != nullptr);
				builder->AddLink(start, end, node->begin->name[0], node->end->name[0]);
			}
			void		Visit(NormalFactor* node)
			{
				start = builder->GetNewNode();
				end = builder->GetNewNode();
				builder->AddLink(start, end, node->name[0]);
			}
			void		Visit(Repeat*)
			{
			}
			void		Visit(Alternate* node)
			{
				node->left->Accept(this);
				BuildNFAVistor visitor(builder);
				node->left->Accept(&visitor);
				builder->AddNullLink(this->end, visitor.start);
				end = visitor.end;
			}
			void		Visit(Sequence* node)
			{
				node->first->Accept(this);
				BuildNFAVistor visitor(builder);
				node->second->Accept(&visitor);
				auto left = builder->GetNewNode();
				auto right = builder->GetNewNode();
				builder->AddNullLink(left, this->start);
				builder->AddNullLink(left, visitor.start);
				builder->AddNullLink(right, this->end);
				builder->AddNullLink(right, visitor.end);
				start = left;
				end = right;
			}

		private:
			NFANode* start;
			NFANode* end;
			DFATableBuilder* builder;
		};
		void BuildNFA(DFATableBuilder* builder)
		{
			vector<BuildNFAVistor>result;
			for(size_t i = 0; i < builder->GetRoots()->size(); ++i)
			{
				auto&&iter = (*builder->GetRoots())[i];
				result.emplace_back(builder);
				iter->Accept(&result.back());
				builder->AddStopStates(result[i].GetEnd()->GetNumber(), (int)i);
			}
			auto first = builder->GetNewNode();
			auto second = builder->GetNewNode();
			for (auto&&iter:result)
			{
				builder->AddNullLink(first, iter.GetStart());
				builder->AddNullLink(iter.GetEnd(), second);
			}
			builder->SetNFA(first, second);
		}

		//NFA to DFA
		
	}
}