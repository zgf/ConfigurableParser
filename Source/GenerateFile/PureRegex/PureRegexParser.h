#pragma once
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <assert.h>
using std::pair;
using std::wstring;
using std::shared_ptr;
using std::vector;
using std::unordered_map;
using std::wifstream;
using std::make_shared;
#include "..\..\Include\GeneralTableDefine.h"
#include "..\..\Include\GeneralTreeNode.h"
#include "..\..\Include\GeneralLALRParser.h"

namespace ztl
{
	namespace pure_regex
	{
		struct TokenUnit;
		struct CharSet;
		struct BinaryFactor;
		struct UserDefineFactor;
		struct NormalFactor;
		struct Repeat;
		struct Alternate;
		struct Sequence;
		struct Factor;
		struct Define
		{
			class IVisitor
			{
			public:
				virtual void		Visit(TokenUnit* node) = 0;
				virtual void		Visit(CharSet* node) = 0;
				virtual void		Visit(BinaryFactor* node) = 0;
				virtual void		Visit(UserDefineFactor* node) = 0;
				virtual void		Visit(NormalFactor* node) = 0;
				virtual void		Visit(Repeat* node) = 0;
				virtual void		Visit(Alternate* node) = 0;
				virtual void		Visit(Sequence* node) = 0;
				virtual void		Visit(Factor* node) = 0;
			};
			virtual void									Accept(IVisitor*)
			{
			}
		};

		struct Sequence: public Define
		{
			shared_ptr<Define>    first;
			shared_ptr<Define>    second;

			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};

		struct Alternate: public Define
		{
			shared_ptr<Define>    left;
			shared_ptr<Define>    right;

			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};

		struct Repeat: public Define
		{
			enum class RepeatType
			{
				KLEENEGREEDY,
				KLEENE,
				POSITIVEKLEENEGREEDY,
				POSITIVEKLEENE,
				CHOCIEKLEENEGREEDY,
				CHOCIEKLEENE,
			};

			RepeatType    type;

			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};

		struct BinaryFactor;
		struct UserDefineFactor;
		struct NormalFactor;

		struct Factor: public Define
		{
			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}

			class IVisitor
			{
			public:
				virtual void		Visit(BinaryFactor* node) = 0;
				virtual void		Visit(UserDefineFactor* node) = 0;
				virtual void		Visit(NormalFactor* node) = 0;
			};
			virtual void									Accept(IVisitor*)
			{
			}
		};

		struct NormalFactor: public Factor
		{
			wstring    name;

			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};

		struct UserDefineFactor: public Factor
		{
			enum class UserDefineType
			{
				TROPEW,
				TROPEw,
				TROPES,
				TROPEs,
				TROPED,
				TROPEd,
				MatchAll,
			};

			UserDefineType    type;

			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};

		struct BinaryFactor: public Factor
		{
			shared_ptr<NormalFactor>    begin;
			shared_ptr<NormalFactor>    end;

			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};

		struct CharSet: public Define
		{
			enum class CharSetOptional
			{
				Positve,
				Reverse,
			};

			CharSetOptional    type;
			vector<shared_ptr<Factor>>    tokens;

			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};

		struct TokenUnit: public Define
		{
			shared_ptr<Define>    charset;
			shared_ptr<Repeat>    repeat;

			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};

		shared_ptr<ztl::general_parser::GeneralTableDefine> BootStrapDefineTable();

		shared_ptr<void> GeneralHeterogeneousParserTree(ztl::general_parser::GeneralParserBase& parser, ztl::general_parser::GeneralTreeNode* root);
		shared_ptr<void>	GeneralHeterogeneousParserTree(ztl::general_parser::GeneralParserBase& parser);
		vector<shared_ptr<ztl::general_parser::TokenInfo>> PureRegexParseToken(const wstring& pattern);

	}
}
