#include "Include/stdafx.h"
#include "PureRegexAutoMachineData.h"
#include "../../Include/GeneralLALRParser.h"
#include "PureRegexWriter.hpp"
#include "PureRegexParser.h"
#include "CharsetMapTable.h"
#include "PureRegexLexer.h"
namespace ztl
{
	namespace pure_regex
	{
		//////////////////////////////////////////////////////////////////////////
		//Token To Ast
		//////////////////////////////////////////////////////////////////////////
		//输入tokens,解析出一堆AST

		void ParsingAllTokensToAst(vector<TokenPacket>& tokens)
		{
			ztl::general_parser::GeneralLALRParser LALRParser(ztl::pure_regex::BootStrapDefineTable());
			LALRParser.BuildParser();
			for(auto&&tokenIter : tokens)
			{
				LALRParser.SetTokenPool(ztl::pure_regex::PureRegexParseToken(tokenIter.regex));
				LALRParser.GenerateIsomorphismParserTree();
				tokenIter.root = static_pointer_cast<Alternate>(ztl::pure_regex::GeneralHeterogeneousParserTree(LALRParser));
				LALRParser.ClearEnvironment();
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//获取字符映射表
		//////////////////////////////////////////////////////////////////////////
		//userdefine替换掉
		class BuildCharsetVisitor: public Factor::IVisitor
		{
		public:
			BuildCharsetVisitor() noexcept = default;
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

		shared_ptr<CharsetMapTable> BuildCharsetMapTable(const vector<TokenPacket>& tokens)
		{
			BuildCharsetVisitor visitor;
			for(auto&&tokenIter : tokens)
			{
				tokenIter.root->Accept(&visitor);
			}
			return visitor.GetResult();
		}
		///////////////////////////////////////////////////////////////////////////
		//AST to NFA
		//////////////////////////////////////////////////////////////////////////

	}
}