#include "Include/stdafx.h"

#include "JsonParser.h"
#include "..\..\Include\GeneralTableWriter.h"

namespace ztl
{
	namespace json
	{
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
		void ReflectionBuidler(const wstring& className, const wstring& fieldName, const shared_ptr<void>& classObject, const shared_ptr<void>& valueObject)
		{
			using builderType = void(*)(const shared_ptr<void>&, const shared_ptr<void>&);
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
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<Number>(classObject)->content = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
									}
								},
							}
						},{
							L"Literal",
							{
								{
									L"value",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<Literal>(classObject)->value = static_cast<Literal::Value>(WstringToEnumItem(L"Literal::Value::" + std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content));
									}
								},
							}
						},{
							L"Object",
							{
								{
									L"fields",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<Object>(classObject)->fields.emplace_back(std::static_pointer_cast<ObjectField>(valueObject));
									}
								},
							}
						},{
							L"String",
							{
								{
									L"content",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<String>(classObject)->content = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
									}
								},
							}
						},{
							L"Array",
							{
								{
									L"items",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<Array>(classObject)->items.emplace_back(std::static_pointer_cast<Node>(valueObject));
									}
								},
							}
						},{
							L"ObjectField",
							{
								{
									L"name",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<ObjectField>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
									}
								},{
									L"value",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<ObjectField>(classObject)->value = std::static_pointer_cast<Node>(valueObject);
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
		void GenerateHeterogeneousParserTree(ztl::general_parser::GeneralLRExecutor& parser, ztl::general_parser::GeneralTreeNode* classNode, shared_ptr<void>& classObject)
		{
			assert(classObject != nullptr);
			assert(classNode != nullptr);
			auto className = classNode->GetName();
			for (auto&& iter : classNode->GetFieldMap())
			{
				auto fieldName = iter.first;
				for (auto&& nodeIter : iter.second)
				{
					auto fieldNode = parser.GetNonTermNodeByIndex(nodeIter);
					auto fieldObject = ReflecteObjectByName(fieldNode->GetName());
					parser.SaveHeterogeneousNode(fieldObject);
					ReflectionBuidler(className, fieldName, classObject, fieldObject);
					ztl::json::GenerateHeterogeneousParserTree(parser, fieldNode, fieldObject);
				}
			}
			for (auto&&iter : classNode->GetTermMap())
			{
				auto fieldName = iter.first;
				for (auto&& nodeIter : iter.second)
				{
					ReflectionBuidler(className, fieldName, classObject, parser.GetTermNodeByIndex(nodeIter));
				}
			}
		}

		shared_ptr<void> GenerateHeterogeneousParserTree(ztl::general_parser::GeneralLRExecutor& parser, ztl::general_parser::GeneralTreeNode* root)
		{
			assert(root != nullptr);
			auto rootObject = ReflecteObjectByName(root->GetName());
			parser.SaveHeterogeneousNode(rootObject);
			ztl::json::GenerateHeterogeneousParserTree(parser, root, rootObject);
			return rootObject;
		}
		shared_ptr<void>	GenerateHeterogeneousParserTree(ztl::general_parser::GeneralLRExecutor& parser)
		{
			return ztl::json::GenerateHeterogeneousParserTree(parser, parser.GetGeneralTreeRoot());
		}

		shared_ptr<ztl::general_parser::GeneralTableDefine> BootStrapDefineTable()
		{
			ztl::general_parser::GeneralTableWriter writer;
			writer.Head
				(
					ztl::general_parser::GeneralHeadInfoWriter()

					.Info(LR"(filename)", LR"(JsonParser,
				ztl::general_parser::GeneralAttributeWriter()

				)")

					.Info(LR"(include)", LR"(Include/stdafx.h,
				ztl::general_parser::GeneralAttributeWriter()

				)")

					.Info(LR"(namespace)", LR"(ztl,
				ztl::general_parser::GeneralAttributeWriter()

				)")

					.Info(LR"(namespace)", LR"(json,
				ztl::general_parser::GeneralAttributeWriter()

				)")

					.Info(LR"(dirname)", LR"(json,
				ztl::general_parser::GeneralAttributeWriter()

				)")

					)
				.Token
				(
					ztl::general_parser::GeneralTokenWriter()
					.Token(L"FINISH", L"<\\$>")
					.Token(L"TRUEVALUE", LR"(true)",
						ztl::general_parser::GeneralAttributeWriter()

						)
					.Token(L"FALSEVALUE", LR"(false)",
						ztl::general_parser::GeneralAttributeWriter()

						)
					.Token(L"NULLVALUE", LR"(null)",
						ztl::general_parser::GeneralAttributeWriter()

						)
					.Token(L"OBJOPEN", LR"(\{)",
						ztl::general_parser::GeneralAttributeWriter()

						)
					.Token(L"OBJCLOSE", LR"(\})",
						ztl::general_parser::GeneralAttributeWriter()

						)
					.Token(L"ARROPEN", LR"(\[)",
						ztl::general_parser::GeneralAttributeWriter()

						)
					.Token(L"ARRCLOSE", LR"(\])",
						ztl::general_parser::GeneralAttributeWriter()

						)
					.Token(L"COMMA", LR"(,)",
						ztl::general_parser::GeneralAttributeWriter()

						)
					.Token(L"COLON", LR"(:)",
						ztl::general_parser::GeneralAttributeWriter()

						)
					.Token(L"NUMBER", LR"([\-]?\d+(.\d+)?([eE][+\-]?\d+)?)",
						ztl::general_parser::GeneralAttributeWriter()

						)
					.Token(L"STRING", LR"("(\\"|[^"])*")",
						ztl::general_parser::GeneralAttributeWriter()

						)
					.IgnoreToken(L"SPACE", LR"(\s+)",
						ztl::general_parser::GeneralAttributeWriter()

						)

					)
				.Type
				(

					ztl::general_parser::GeneralTypeListWriter()

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"Node")

						.Attributes(
							ztl::general_parser::GeneralAttributeWriter()

							)
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

								.Member(ztl::general_parser::EnumMember(L"True",
									ztl::general_parser::GeneralAttributeWriter()

									))

								.Member(ztl::general_parser::EnumMember(L"False",
									ztl::general_parser::GeneralAttributeWriter()

									))

								.Member(ztl::general_parser::EnumMember(L"Null",
									ztl::general_parser::GeneralAttributeWriter()

									))

								.Attributes(
									ztl::general_parser::GeneralAttributeWriter()

									)
								)

							)

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"Value"), L"value",
							ztl::general_parser::GeneralAttributeWriter()

							))

						.ParentType(ztl::general_parser::Normal(L"Node"))
						.Attributes(
							ztl::general_parser::GeneralAttributeWriter()

							)
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"String")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"content",
							ztl::general_parser::GeneralAttributeWriter()

							))

						.ParentType(ztl::general_parser::Normal(L"Node"))
						.Attributes(
							ztl::general_parser::GeneralAttributeWriter()

							)
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"Number")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"content",
							ztl::general_parser::GeneralAttributeWriter()

							))

						.ParentType(ztl::general_parser::Normal(L"Node"))
						.Attributes(
							ztl::general_parser::GeneralAttributeWriter()

							)
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"Array")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"Node")), L"items",
							ztl::general_parser::GeneralAttributeWriter()

							))

						.ParentType(ztl::general_parser::Normal(L"Node"))
						.Attributes(
							ztl::general_parser::GeneralAttributeWriter()

							)
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"ObjectField")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
							ztl::general_parser::GeneralAttributeWriter()

							))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"Node"), L"value",
							ztl::general_parser::GeneralAttributeWriter()

							))

						.ParentType(ztl::general_parser::Normal(L"Node"))
						.Attributes(
							ztl::general_parser::GeneralAttributeWriter()

							)
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"Object")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"ObjectField")), L"fields",
							ztl::general_parser::GeneralAttributeWriter()

							))

						.ParentType(ztl::general_parser::Normal(L"Node"))
						.Attributes(
							ztl::general_parser::GeneralAttributeWriter()

							)
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
						(

							ztl::general_parser::GrammarSymbol(L"STRING")[L"content"]

							)
						.Create(ztl::general_parser::Normal(L"String"))

						|
						(

							ztl::general_parser::GrammarSymbol(L"NUMBER")[L"content"]

							)
						.Create(ztl::general_parser::Normal(L"Number"))

						|
						(
							ztl::general_parser::GrammarSymbol(L"TRUEVALUE")
							)
						.Create(ztl::general_parser::Normal(L"Literal"))

						.Setter(L"value", L"True")

						|
						(
							ztl::general_parser::GrammarSymbol(L"FALSEVALUE")
							)
						.Create(ztl::general_parser::Normal(L"Literal"))

						.Setter(L"value", L"False")

						|
						(
							ztl::general_parser::GrammarSymbol(L"NULLVALUE")
							)
						.Create(ztl::general_parser::Normal(L"Literal"))

						.Setter(L"value", L"Null")

						)

					.Rule
					(
						ztl::general_parser::GeneralRuleWriter()
						.Name(L"JField")
						.ReturnType(ztl::general_parser::Normal(L"ObjectField"))

						|
						(

							ztl::general_parser::GrammarSymbol(L"STRING")[L"name"]
							+ ztl::general_parser::GrammarSymbol(L"COLON") +
							ztl::general_parser::GrammarSymbol(L"JValue")[L"value"]

							)
						.Create(ztl::general_parser::Normal(L"ObjectField"))

						)

					.Rule
					(
						ztl::general_parser::GeneralRuleWriter()
						.Name(L"JObject")
						.ReturnType(ztl::general_parser::Normal(L"Object"))

						|
						(
							ztl::general_parser::GrammarSymbol(L"OBJOPEN") + ~(
								ztl::general_parser::GrammarSymbol(L"JField")[L"fields"]
								+ *(ztl::general_parser::GrammarSymbol(L"COMMA") +
									ztl::general_parser::GrammarSymbol(L"JField")[L"fields"]
									)) + ztl::general_parser::GrammarSymbol(L"OBJCLOSE")
							)
						.Create(ztl::general_parser::Normal(L"Object"))

						)

					.Rule
					(
						ztl::general_parser::GeneralRuleWriter()
						.Name(L"JArray")
						.ReturnType(ztl::general_parser::Normal(L"Array"))

						|
						(
							ztl::general_parser::GrammarSymbol(L"ARROPEN") + ~(
								ztl::general_parser::GrammarSymbol(L"JValue")[L"items"]
								+ *(ztl::general_parser::GrammarSymbol(L"COMMA") +
									ztl::general_parser::GrammarSymbol(L"JValue")[L"items"]
									)) + ztl::general_parser::GrammarSymbol(L"ARRCLOSE")
							)
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