#pragma once
#include "..\..\Include\stdafx.h"
#include "..\..\Include\GeneralTableDefine.h"
namespace ztl
{
	namespace pure_regex
	{
		class CharsetMapTable;
		class DFATableBuilder;
		class TokenList;
		struct Alternate;
		using TokenPacket = ztl::general_parser::GeneralTokenDefine;

		class GeneralLexer
		{
		public:
			GeneralLexer() noexcept = default;
			GeneralLexer(const vector<shared_ptr<TokenPacket>>& infos);
			~GeneralLexer() noexcept = default;
			GeneralLexer(GeneralLexer&&) noexcept = default;
			GeneralLexer(const GeneralLexer&) noexcept = default;
			GeneralLexer& operator=(GeneralLexer&&) noexcept = default;
			GeneralLexer& operator=(const GeneralLexer&) noexcept  = default;
		public:

		private:

			shared_ptr<TokenList>		tokens;
			shared_ptr<vector<shared_ptr<Alternate>>> roots;
			shared_ptr<CharsetMapTable> table;
			shared_ptr<DFATableBuilder> builder;
		};
		vector<shared_ptr<Alternate>> ParsingAllTokensToAst(const vector<shared_ptr<TokenPacket>>& tokens);
		shared_ptr<CharsetMapTable> BuildCharsetMapTable(const vector<shared_ptr<Alternate>>& roots);

	}
}