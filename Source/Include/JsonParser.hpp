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

#include "GeneralTableWriter.h"

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

		int WstringToEnumItem(const wstring& value)
		{
			static unordered_map<wstring, int> signMap =
			{
			{
				L"Literal::Value::True",
				static_cast<int>(Literal::Value::True)
			},
			{
				L"Literal::Value::False",
				static_cast<int>(Literal::Value::False)
			},
			{
				L"Literal::Value::Null",
				static_cast<int>(Literal::Value::Null)
			},
			};
			assert(signMap.find(value) != signMap.end());
			return signMap[value];
		}
		void ReflectionBuidler(const wstring& className, const wstring& fieldName, void* classObject, void* valueObject)
		{
			using builderType = void(*)(void*, void*);
			static unordered_map<wstring, unordered_map<wstring, builderType>> builderMap =
			{
				{
							L"Node",
							{
							}
						},{
							L"Number",
							{
								{
									L"content",
									[](void* classObject,void* valueObject)
									{
										static_cast<Number*>(classObject)->content = static_cast<ztl::general_parser::TokenInfo*>(valueObject)->content;
										return;
									}
								},
							}
						},{
							L"Literal",
							{
								{
									L"value",
									[](void* classObject,void* valueObject)
									{
										static_cast<Literal*>(classObject)->value = static_cast<Literal::Value>(WstringToEnumItem(L"Literal::Value::" + static_cast<ztl::general_parser::TokenInfo*>(valueObject)->content));
										return;
									}
								},
							}
						},{
							L"Object",
							{
								{
									L"fields",
									[](void* classObject,void* valueObject)
									{
										static_cast<Object*>(classObject)->fields.emplace_back(static_cast<ObjectField*>(valueObject));
										return;
									}
								},
							}
						},{
							L"String",
							{
								{
									L"content",
									[](void* classObject,void* valueObject)
									{
										static_cast<String*>(classObject)->content = static_cast<ztl::general_parser::TokenInfo*>(valueObject)->content;
										return;
									}
								},
							}
						},{
							L"Array",
							{
								{
									L"items",
									[](void* classObject,void* valueObject)
									{
										static_cast<Array*>(classObject)->items.emplace_back(static_cast<Node*>(valueObject));
										return;
									}
								},
							}
						},{
							L"ObjectField",
							{
								{
									L"name",
									[](void* classObject,void* valueObject)
									{
										static_cast<ObjectField*>(classObject)->name = static_cast<ztl::general_parser::TokenInfo*>(valueObject)->content;
										return;
									}
								},{
									L"value",
									[](void* classObject,void* valueObject)
									{
										static_cast<ObjectField*>(classObject)->value.reset(static_cast<Node*>(valueObject));
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
				L"Node",
				[]()->shared_ptr<void>
				{
					return make_shared<Node>();
				}
			},
			{
				L"Number",
				[]()->shared_ptr<void>
				{
					return make_shared<Number>();
				}
			},
			{
				L"Literal",
				[]()->shared_ptr<void>
				{
					return make_shared<Literal>();
				}
			},
			{
				L"Object",
				[]()->shared_ptr<void>
				{
					return make_shared<Object>();
				}
			},
			{
				L"String",
				[]()->shared_ptr<void>
				{
					return make_shared<String>();
				}
			},
			{
				L"Array",
				[]()->shared_ptr<void>
				{
					return make_shared<Array>();
				}
			},
			{
				L"ObjectField",
				[]()->shared_ptr<void>
				{
					return make_shared<ObjectField>();
				}
			},
			};
			assert(objectMap.find(name) != objectMap.end());
			return objectMap[name]();
		}
		shared_ptr<ztl::general_parser::GeneralTableDefine> BootStrapDefineTable()
		{
			ztl::general_parser::GeneralTableWriter writer;
			writer.Head
				(
				ztl::general_parser::GeneralHeadInfoWriter()

				.Info(LR"(filename)", LR"(Source\Include\JsonParser)")

				.Info(LR"(include)", LR"(GeneralTableDefine.h)")

				.Info(LR"(include)", LR"(GeneralTableWriter.h)")

				.Info(LR"(namespace)", LR"(ztl)")

				.Info(LR"(namespace)", LR"(json)")

				)
				.Token
				(
				ztl::general_parser::GeneralTokenWriter()
				.Token(L"TRUEVALUE", LR"(true)")
				.Token(L"FALSEVALUE", LR"(false)")
				.Token(L"NULLVALUE", LR"(null)")
				.Token(L"OBJOPEN", LR"(\{)")
				.Token(L"OBJCLOSE", LR"(\})")
				.Token(L"ARROPEN", LR"(\[)")
				.Token(L"ARRCLOSE", LR"(\])")
				.Token(L"COMMA", LR"(,)")
				.Token(L"COLON", LR"(:)")
				.Token(L"NUMBER", LR"([\-]?\d+(.\d+)?([eE][+\-]?\d+)?)")
				.Token(L"STRING", LR"("(\\"|[^"])*")")
				.IgnoreToken(L"SPACE", LR"(\s+)")

				)
				.Type
				(

				ztl::general_parser::GeneralTypeListWriter()

				.Class
				(
				ztl::general_parser::GeneralClassTypeWriter()
				.Name(L"Node")

				)

				.Class
				(
				ztl::general_parser::GeneralClassTypeWriter()
				.Name(L"Literal")

				.SubType(

				ztl::general_parser::GeneralTypeListWriter()

				.Enum
				(
				ztl::general_parser::GeneralEnumTypeWriter()
				.Name(L"Value")

				.Member(ztl::general_parser::EnumMember(L"True"))

				.Member(ztl::general_parser::EnumMember(L"False"))

				.Member(ztl::general_parser::EnumMember(L"Null"))

				)

				)

				.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"Value"), L"value"))

				.ParentType(ztl::general_parser::Normal(L"Node"))
				)

				.Class
				(
				ztl::general_parser::GeneralClassTypeWriter()
				.Name(L"String")

				.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"content"))

				.ParentType(ztl::general_parser::Normal(L"Node"))
				)

				.Class
				(
				ztl::general_parser::GeneralClassTypeWriter()
				.Name(L"Number")

				.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"content"))

				.ParentType(ztl::general_parser::Normal(L"Node"))
				)

				.Class
				(
				ztl::general_parser::GeneralClassTypeWriter()
				.Name(L"Array")

				.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"Node")), L"items"))

				.ParentType(ztl::general_parser::Normal(L"Node"))
				)

				.Class
				(
				ztl::general_parser::GeneralClassTypeWriter()
				.Name(L"ObjectField")

				.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name"))

				.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"Node"), L"value"))

				.ParentType(ztl::general_parser::Normal(L"Node"))
				)

				.Class
				(
				ztl::general_parser::GeneralClassTypeWriter()
				.Name(L"Object")

				.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"ObjectField")), L"fields"))

				.ParentType(ztl::general_parser::Normal(L"Node"))
				)

				)
				.Rules
				(
				ztl::general_parser::GeneralRuleListWriter()

				.Rule
				(
				ztl::general_parser::GeneralRuleWriter()
				.Name(L"JLiteral")
				.ReturnType(ztl::general_parser::Normal(L"Node"))

				|
				ztl::general_parser::GrammarSymbol(L"STRING")[L"content"]

				.Create(ztl::general_parser::Normal(L"String"))

				|
				ztl::general_parser::GrammarSymbol(L"NUMBER")[L"content"]

				.Create(ztl::general_parser::Normal(L"Number"))

				| ztl::general_parser::GrammarSymbol(L"TRUEVALUE")
				.Create(ztl::general_parser::Normal(L"Literal"))

				.Setter(L"value", L"True")

				| ztl::general_parser::GrammarSymbol(L"FALSEVALUE")
				.Create(ztl::general_parser::Normal(L"Literal"))

				.Setter(L"value", L"False")

				| ztl::general_parser::GrammarSymbol(L"NULLVALUE")
				.Create(ztl::general_parser::Normal(L"Literal"))

				.Setter(L"value", L"Null")

				)

				.Rule
				(
				ztl::general_parser::GeneralRuleWriter()
				.Name(L"JField")
				.ReturnType(ztl::general_parser::Normal(L"ObjectField"))

				|
				ztl::general_parser::GrammarSymbol(L"STRING")[L"name"]
				+ ztl::general_parser::GrammarSymbol(L"COLON") +
				ztl::general_parser::GrammarSymbol(L"JValue")[L"value"]

				.Create(ztl::general_parser::Normal(L"ObjectField"))

				)

				.Rule
				(
				ztl::general_parser::GeneralRuleWriter()
				.Name(L"JObject")
				.ReturnType(ztl::general_parser::Normal(L"Object"))

				| ztl::general_parser::GrammarSymbol(L"OBJOPEN") +
				ztl::general_parser::GrammarSymbol(L"JField")[L"fields"]
				+ ztl::general_parser::GrammarSymbol(L"COMMA") + ~(*(
				ztl::general_parser::GrammarSymbol(L"JField")[L"fields"]
				)) + ztl::general_parser::GrammarSymbol(L"OBJCLOSE")
				.Create(ztl::general_parser::Normal(L"Object"))

				)

				.Rule
				(
				ztl::general_parser::GeneralRuleWriter()
				.Name(L"JArray")
				.ReturnType(ztl::general_parser::Normal(L"Array"))

				| ztl::general_parser::GrammarSymbol(L"ARROPEN") +
				ztl::general_parser::GrammarSymbol(L"JValue")[L"items"]
				+ ztl::general_parser::GrammarSymbol(L"COMMA") + ~(*(
				ztl::general_parser::GrammarSymbol(L"JValue")[L"items"]
				)) + ztl::general_parser::GrammarSymbol(L"ARRCLOSE")
				.Create(ztl::general_parser::Normal(L"Array"))

				)

				.Rule
				(
				ztl::general_parser::GeneralRuleWriter()
				.Name(L"JValue")
				.ReturnType(ztl::general_parser::Normal(L"Node"))

				| !(ztl::general_parser::GrammarSymbol(L"JLiteral"))

				| !(ztl::general_parser::GrammarSymbol(L"JObject"))

				| !(ztl::general_parser::GrammarSymbol(L"JArray"))

				)

				.Rule
				(
				ztl::general_parser::GeneralRuleWriter()
				.Name(L"JRoot")
				.ReturnType(ztl::general_parser::Normal(L"Node"))

				| !(ztl::general_parser::GrammarSymbol(L"JObject"))

				| !(ztl::general_parser::GrammarSymbol(L"JArray"))

				)

				);
			return writer.table;;
		}
	}
}