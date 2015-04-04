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
#include "GeneralTableDefine.h"
#include "GeneralTreeNode.h"
#include "GeneralParser.h"

namespace ztl
{
	namespace json
	{
		struct Object;
		struct ObjectField;
		struct Array;
		struct Number;
		struct String;
		struct Literal;

		struct Node
		{
			class IVisitor
			{
			public:
				virtual void		Visit(Object* node) = 0;
				virtual void		Visit(ObjectField* node) = 0;
				virtual void		Visit(Array* node) = 0;
				virtual void		Visit(Number* node) = 0;
				virtual void		Visit(String* node) = 0;
				virtual void		Visit(Literal* node) = 0;
			};
			virtual void									Accept(IVisitor*)
			{
			}
		};

		struct Literal: public Node
		{
			enum class Value
			{
				True,
				False,
				Null,
			};

			Value    value;

			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};

		struct String: public Node
		{
			wstring    content;

			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};

		struct Number: public Node
		{
			wstring    content;

			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};

		struct Array: public Node
		{
			vector<shared_ptr<Node>>    items;

			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};

		struct ObjectField: public Node
		{
			wstring    name;
			shared_ptr<Node>    value;

			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};

		struct Object: public Node
		{
			vector<shared_ptr<ObjectField>>    fields;

			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};

		shared_ptr<ztl::general_parser::GeneralTableDefine> BootStrapDefineTable();

		shared_ptr<void> GeneralHeterogeneousParserTree(ztl::general_parser::GeneralParser& parser, ztl::general_parser::GeneralTreeNode* root);
		shared_ptr<void>	GeneralHeterogeneousParserTree(ztl::general_parser::GeneralParser& parser);
	}
}
