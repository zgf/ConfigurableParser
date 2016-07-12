#include "Include/stdafx.h"

#include "TestAssignParser.h"
#include "..\..\Include\GeneralTableWriter.h"

namespace ztl
{
	namespace testAssign
	{
		int WstringToEnumItem(const wstring& value)
		{
			static unordered_map<wstring, int> signMap =
			{
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
							L"PrimaryExpression",
							{
								{
									L"name",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<PrimaryExpression>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
										return;
									}
								},
							}
						},{
							L"Variable",
							{
								{
									L"name",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<Variable>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
										return;
									}
								},
							}
						},{
							L"Expression",
							{
							}
						},{
							L"AssignExpression",
							{
								{
									L"left",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<AssignExpression>(classObject)->left = std::static_pointer_cast<Variable>(valueObject);
										return;
									}
								},{
									L"right",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<AssignExpression>(classObject)->right = std::static_pointer_cast<Expression>(valueObject);
										return;
									}
								},
							}
						},{
							L"BinaryExpression",
							{
								{
									L"left",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<BinaryExpression>(classObject)->left = std::static_pointer_cast<Expression>(valueObject);
										return;
									}
								},{
									L"right",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<BinaryExpression>(classObject)->right = std::static_pointer_cast<Expression>(valueObject);
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
				L"PrimaryExpression",
				[]()->shared_ptr<void>
				{
					return make_shared<PrimaryExpression>();
				}
			},
			{
				L"Variable",
				[]()->shared_ptr<void>
				{
					return make_shared<Variable>();
				}
			},
			{
				L"Expression",
				[]()->shared_ptr<void>
				{
					return make_shared<Expression>();
				}
			},
			{
				L"AssignExpression",
				[]()->shared_ptr<void>
				{
					return make_shared<AssignExpression>();
				}
			},
			{
				L"BinaryExpression",
				[]()->shared_ptr<void>
				{
					return make_shared<BinaryExpression>();
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
					ztl::testAssign::GenerateHeterogeneousParserTree(parser, fieldNode, fieldObject);
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
			ztl::testAssign::GenerateHeterogeneousParserTree(parser, root, rootObject);
			return rootObject;
		}
		shared_ptr<void>	GenerateHeterogeneousParserTree(ztl::general_parser::GeneralLRExecutor& parser)
		{
			return ztl::testAssign::GenerateHeterogeneousParserTree(parser, parser.GetGeneralTreeRoot());
		}

		shared_ptr<ztl::general_parser::GeneralTableDefine> BootStrapDefineTable()
		{
			ztl::general_parser::GeneralTableWriter writer;
			writer.Head
				(
					ztl::general_parser::GeneralHeadInfoWriter()

					.Info(LR"(filename)", LR"(TestAssignParser,
				ztl::general_parser::GeneralAttributeWriter()

				)")

					.Info(LR"(include)", LR"(Include/stdafx.h,
				ztl::general_parser::GeneralAttributeWriter()

				)")

					.Info(LR"(namespace)", LR"(ztl,
				ztl::general_parser::GeneralAttributeWriter()

				)")

					.Info(LR"(namespace)", LR"(testAssign,
				ztl::general_parser::GeneralAttributeWriter()

				)")

					.Info(LR"(dirname)", LR"(TestAssign,
				ztl::general_parser::GeneralAttributeWriter()

				)")

					)
				.Token
				(
					ztl::general_parser::GeneralTokenWriter()
					.Token(L"FINISH", L"<\\$>")
					.Token(L"NAME", LR"([a-zA-Z_]\w*)",
						ztl::general_parser::GeneralAttributeWriter()

						)
					.Token(L"ASSIGN", LR"(=)",
						ztl::general_parser::GeneralAttributeWriter()

						)
					.Token(L"ADD", LR"(\+)",
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
						.Name(L"Variable")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
							ztl::general_parser::GeneralAttributeWriter()

							))

						.Attributes(
							ztl::general_parser::GeneralAttributeWriter()

							)
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"Expression")

						.Attributes(
							ztl::general_parser::GeneralAttributeWriter()

							)
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"PrimaryExpression")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
							ztl::general_parser::GeneralAttributeWriter()

							))

						.ParentType(ztl::general_parser::Normal(L"Expression"))
						.Attributes(
							ztl::general_parser::GeneralAttributeWriter()

							)
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"AssignExpression")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"Variable"), L"left",
							ztl::general_parser::GeneralAttributeWriter()

							))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"Expression"), L"right",
							ztl::general_parser::GeneralAttributeWriter()

							))

						.ParentType(ztl::general_parser::Normal(L"Expression"))
						.Attributes(
							ztl::general_parser::GeneralAttributeWriter()

							)
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"BinaryExpression")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"Expression"), L"left",
							ztl::general_parser::GeneralAttributeWriter()

							))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"Expression"), L"right",
							ztl::general_parser::GeneralAttributeWriter()

							))

						.ParentType(ztl::general_parser::Normal(L"Expression"))
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
						.Name(L"PrimaryExpressionRule")
						.ReturnType(ztl::general_parser::Normal(L"Expression"))

						|
						(

							ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]

							)
						.Create(ztl::general_parser::Normal(L"PrimaryExpression"))

						)

					.Rule
					(
						ztl::general_parser::GeneralRuleWriter()
						.Name(L"BinaryExpressionRule")
						.ReturnType(ztl::general_parser::Normal(L"Expression"))

						|
						(

							ztl::general_parser::GrammarSymbol(L"BinaryExpressionRule")[L"right"]
							+ ztl::general_parser::GrammarSymbol(L"ADD") +
							ztl::general_parser::GrammarSymbol(L"PrimaryExpressionRule")[L"left"]

							)
						.Create(ztl::general_parser::Normal(L"BinaryExpression"))

						| !(ztl::general_parser::GrammarSymbol(L"PrimaryExpressionRule"))

						)

					);
			return writer.table;;
		}

		class IRCodeGenContext
		{
		};
		template<typename BaseClassType>
		class IRCodeGen : public BaseClassType::IVisitor
		{
			IRCodeGenContext* context;
		};

		
			template<>
		class IRCodeGen<Expression> : public Expression::IVisitor
		{
		public:
			virtual void		Visit(Expression*) override
			{
			}
			virtual void		Visit(BinaryExpression*) override
			{
			}
			virtual void		Visit(AssignExpression*) override
			{
			}
			virtual void		Visit(PrimaryExpression*) override
			{
			}

			IRCodeGenContext* context;
		};
	}
}