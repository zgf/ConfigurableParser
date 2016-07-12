#include "Include/stdafx.h"

#include "MiniSharpParser.h"
					#include "..\..\Include\GeneralTableWriter.h"

				
							namespace ztl
							{
								
							namespace miniSharp
							{
								
			int WstringToEnumItem(const wstring& value)
			{
				static unordered_map<wstring, int> signMap =
				{
					
				{
					L"PrefixPropertyValue::Private",
					static_cast<int>(PrefixPropertyValue::Private)
				},
				{
					L"PrefixPropertyValue::Constants",
					static_cast<int>(PrefixPropertyValue::Constants)
				},
				{
					L"PrefixPropertyValue::ReadOnly",
					static_cast<int>(PrefixPropertyValue::ReadOnly)
				},
				{
					L"PrefixPropertyValue::Static",
					static_cast<int>(PrefixPropertyValue::Static)
				},
				{
					L"PrefixPropertyValue::Virtual",
					static_cast<int>(PrefixPropertyValue::Virtual)
				},
				{
					L"PrefixPropertyValue::Public",
					static_cast<int>(PrefixPropertyValue::Public)
				},
				};
				assert(signMap.find(value)!=signMap.end());
				return signMap[value];
			}
			void ReflectionBuidler(const wstring& className,const wstring& fieldName,const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				using builderType = void(*)(const shared_ptr<void>&,const shared_ptr<void>&);
				static unordered_map<wstring, unordered_map<wstring, builderType>> builderMap =
				{
					{
								L"NewExpression",
								{
									{
										L"type",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<NewExpression>(classObject)->type = std::static_pointer_cast<IdentifierObject>(valueObject);
											return;
										}
									},{
										L"args",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<NewExpression>(classObject)->args.emplace_back(std::static_pointer_cast<AstExpression>(valueObject));
											return;
										}
									},
								}
							},{
								L"AttachSuffixExpression",
								{
									{
										L"op",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AttachSuffixExpression>(classObject)->op=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},{
										L"expression",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AttachSuffixExpression>(classObject)->expression = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},
								}
							},{
								L"IdentifierObject",
								{
									
								}
							},{
								L"CharExpression",
								{
									{
										L"value",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<CharExpression>(classObject)->value=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"DeclarationExpression",
								{
									{
										L"type",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<DeclarationExpression>(classObject)->type = std::static_pointer_cast<IdentifierObject>(valueObject);
											return;
										}
									},{
										L"expressions",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<DeclarationExpression>(classObject)->expressions.emplace_back(std::static_pointer_cast<AstExpression>(valueObject));
											return;
										}
									},
								}
							},{
								L"SubIdentifierObject",
								{
									{
										L"scope",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<SubIdentifierObject>(classObject)->scope = std::static_pointer_cast<IdentifierObject>(valueObject);
											return;
										}
									},{
										L"right",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<SubIdentifierObject>(classObject)->right = std::static_pointer_cast<IdentifierObject>(valueObject);
											return;
										}
									},
								}
							},{
								L"PrimaryIdentifierObject",
								{
									{
										L"primary",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<PrimaryIdentifierObject>(classObject)->primary=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"AstClassTypeDefine",
								{
									{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstClassTypeDefine>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},{
										L"parent",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstClassTypeDefine>(classObject)->parent = std::static_pointer_cast<IdentifierObject>(valueObject);
											return;
										}
									},{
										L"parentInterfaces",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstClassTypeDefine>(classObject)->parentInterfaces.emplace_back(std::static_pointer_cast<IdentifierObject>(valueObject));
											return;
										}
									},{
										L"members",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstClassTypeDefine>(classObject)->members.emplace_back(std::static_pointer_cast<AstClassMember>(valueObject));
											return;
										}
									},
								}
							},{
								L"NormalIdentifierObject",
								{
									{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<NormalIdentifierObject>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"ThisIdentifierObject",
								{
									
								}
							},{
								L"IntegerExpression",
								{
									{
										L"value",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<IntegerExpression>(classObject)->value=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"VariableExpression",
								{
									{
										L"variable",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<VariableExpression>(classObject)->variable = std::static_pointer_cast<IdentifierObject>(valueObject);
											return;
										}
									},
								}
							},{
								L"LogicExpression",
								{
									
								}
							},{
								L"BoolExpression",
								{
									{
										L"value",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<BoolExpression>(classObject)->value=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"AstExpression",
								{
									
								}
							},{
								L"CastExpression",
								{
									{
										L"type",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<CastExpression>(classObject)->type = std::static_pointer_cast<IdentifierObject>(valueObject);
											return;
										}
									},{
										L"expression",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<CastExpression>(classObject)->expression = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},
								}
							},{
								L"ArrayIdentifierObject",
								{
									{
										L"element",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<ArrayIdentifierObject>(classObject)->element = std::static_pointer_cast<IdentifierObject>(valueObject);
											return;
										}
									},{
										L"numbers",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<ArrayIdentifierObject>(classObject)->numbers.emplace_back(std::static_pointer_cast<AstExpression>(valueObject));
											return;
										}
									},
								}
							},{
								L"LogicAndExpression",
								{
									{
										L"left",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<LogicAndExpression>(classObject)->left = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},{
										L"right",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<LogicAndExpression>(classObject)->right = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},
								}
							},{
								L"PrefixProperty",
								{
									{
										L"value",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<PrefixProperty>(classObject)->value=static_cast<PrefixPropertyValue>(WstringToEnumItem(L"PrefixPropertyValue::" + std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content));
											return;
										}
									},
								}
							},{
								L"LiteralExpression",
								{
									
								}
							},{
								L"AstFieldTypeDefine",
								{
									{
										L"type",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstFieldTypeDefine>(classObject)->type = std::static_pointer_cast<IdentifierObject>(valueObject);
											return;
										}
									},{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstFieldTypeDefine>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"DoubleExpression",
								{
									{
										L"value",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<DoubleExpression>(classObject)->value=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"NullExpression",
								{
									
								}
							},{
								L"SwitchStatement",
								{
									{
										L"conditional",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<SwitchStatement>(classObject)->conditional = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},{
										L"cases",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<SwitchStatement>(classObject)->cases.emplace_back(std::static_pointer_cast<CaseStatement>(valueObject));
											return;
										}
									},{
										L"default",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<SwitchStatement>(classObject)->default = std::static_pointer_cast<AstStatement>(valueObject);
											return;
										}
									},
								}
							},{
								L"ParserASTRoot",
								{
									{
										L"usings",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<ParserASTRoot>(classObject)->usings.emplace_back(std::static_pointer_cast<AstUsingDeclExpression>(valueObject));
											return;
										}
									},{
										L"members",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<ParserASTRoot>(classObject)->members.emplace_back(std::static_pointer_cast<NamespaceMember>(valueObject));
											return;
										}
									},
								}
							},{
								L"StringExpression",
								{
									{
										L"value",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<StringExpression>(classObject)->value=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"ArithmeticExpression",
								{
									
								}
							},{
								L"FunctionCallExpression",
								{
									{
										L"args",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<FunctionCallExpression>(classObject)->args.emplace_back(std::static_pointer_cast<AstExpression>(valueObject));
											return;
										}
									},{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<FunctionCallExpression>(classObject)->name = std::static_pointer_cast<IdentifierObject>(valueObject);
											return;
										}
									},
								}
							},{
								L"NamespaceMember",
								{
									{
										L"properties",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<NamespaceMember>(classObject)->properties.emplace_back(std::static_pointer_cast<PrefixProperty>(valueObject));
											return;
										}
									},{
										L"define",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<NamespaceMember>(classObject)->define = std::static_pointer_cast<AstTypeDefine>(valueObject);
											return;
										}
									},
								}
							},{
								L"AstTypeDefine",
								{
									
								}
							},{
								L"AssignExpression",
								{
									{
										L"variable",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AssignExpression>(classObject)->variable = std::static_pointer_cast<IdentifierObject>(valueObject);
											return;
										}
									},{
										L"right",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AssignExpression>(classObject)->right = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},
								}
							},{
								L"ContinueStatement",
								{
									
								}
							},{
								L"HighArithmeticExpression",
								{
									{
										L"op",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<HighArithmeticExpression>(classObject)->op=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},{
										L"left",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<HighArithmeticExpression>(classObject)->left = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},{
										L"right",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<HighArithmeticExpression>(classObject)->right = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},
								}
							},{
								L"LowArithmeticExpression",
								{
									{
										L"op",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<LowArithmeticExpression>(classObject)->op=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},{
										L"left",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<LowArithmeticExpression>(classObject)->left = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},{
										L"right",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<LowArithmeticExpression>(classObject)->right = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},
								}
							},{
								L"InterfaceMember",
								{
									{
										L"properties",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<InterfaceMember>(classObject)->properties.emplace_back(std::static_pointer_cast<PrefixProperty>(valueObject));
											return;
										}
									},{
										L"sign",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<InterfaceMember>(classObject)->sign = std::static_pointer_cast<FunctionSignature>(valueObject);
											return;
										}
									},
								}
							},{
								L"CompareExpression",
								{
									{
										L"op",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<CompareExpression>(classObject)->op=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},{
										L"left",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<CompareExpression>(classObject)->left = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},{
										L"right",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<CompareExpression>(classObject)->right = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},
								}
							},{
								L"LogicOrExpression",
								{
									{
										L"left",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<LogicOrExpression>(classObject)->left = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},{
										L"right",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<LogicOrExpression>(classObject)->right = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},
								}
							},{
								L"AttachPrefixExpression",
								{
									{
										L"op",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AttachPrefixExpression>(classObject)->op=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},{
										L"expression",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AttachPrefixExpression>(classObject)->expression = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},
								}
							},{
								L"AstStatement",
								{
									
								}
							},{
								L"ExpressionStatement",
								{
									{
										L"properties",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<ExpressionStatement>(classObject)->properties.emplace_back(std::static_pointer_cast<PrefixProperty>(valueObject));
											return;
										}
									},{
										L"expression",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<ExpressionStatement>(classObject)->expression = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},
								}
							},{
								L"ScopeStatement",
								{
									{
										L"statements",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<ScopeStatement>(classObject)->statements.emplace_back(std::static_pointer_cast<AstStatement>(valueObject));
											return;
										}
									},
								}
							},{
								L"ReturnStatement",
								{
									{
										L"expression",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<ReturnStatement>(classObject)->expression = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},
								}
							},{
								L"IfStatement",
								{
									{
										L"conditional",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<IfStatement>(classObject)->conditional = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},{
										L"truePath",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<IfStatement>(classObject)->truePath = std::static_pointer_cast<ScopeStatement>(valueObject);
											return;
										}
									},{
										L"falsePath",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<IfStatement>(classObject)->falsePath = std::static_pointer_cast<AstStatement>(valueObject);
											return;
										}
									},
								}
							},{
								L"WhileStatement",
								{
									{
										L"conditional",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<WhileStatement>(classObject)->conditional = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},{
										L"body",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<WhileStatement>(classObject)->body = std::static_pointer_cast<ScopeStatement>(valueObject);
											return;
										}
									},
								}
							},{
								L"AstInterfaceTypeDefine",
								{
									{
										L"parents",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstInterfaceTypeDefine>(classObject)->parents.emplace_back(std::static_pointer_cast<IdentifierObject>(valueObject));
											return;
										}
									},{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstInterfaceTypeDefine>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},{
										L"members",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstInterfaceTypeDefine>(classObject)->members.emplace_back(std::static_pointer_cast<InterfaceMember>(valueObject));
											return;
										}
									},
								}
							},{
								L"ForStatement",
								{
									{
										L"body",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<ForStatement>(classObject)->body = std::static_pointer_cast<ScopeStatement>(valueObject);
											return;
										}
									},{
										L"initExpression",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<ForStatement>(classObject)->initExpression = std::static_pointer_cast<ExpressionStatement>(valueObject);
											return;
										}
									},{
										L"conditional",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<ForStatement>(classObject)->conditional = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},{
										L"stepExpressions",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<ForStatement>(classObject)->stepExpressions.emplace_back(std::static_pointer_cast<AstExpression>(valueObject));
											return;
										}
									},
								}
							},{
								L"AstFunctionTypeDefine",
								{
									{
										L"sign",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstFunctionTypeDefine>(classObject)->sign = std::static_pointer_cast<FunctionSignature>(valueObject);
											return;
										}
									},{
										L"body",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstFunctionTypeDefine>(classObject)->body = std::static_pointer_cast<ScopeStatement>(valueObject);
											return;
										}
									},
								}
							},{
								L"CaseStatement",
								{
									{
										L"value",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<CaseStatement>(classObject)->value = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},{
										L"statements",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<CaseStatement>(classObject)->statements.emplace_back(std::static_pointer_cast<AstStatement>(valueObject));
											return;
										}
									},
								}
							},{
								L"DefaultStatement",
								{
									{
										L"statements",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<DefaultStatement>(classObject)->statements.emplace_back(std::static_pointer_cast<AstStatement>(valueObject));
											return;
										}
									},
								}
							},{
								L"BreakStatement",
								{
									
								}
							},{
								L"UsingStatement",
								{
									{
										L"source",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<UsingStatement>(classObject)->source = std::static_pointer_cast<AstExpression>(valueObject);
											return;
										}
									},{
										L"body",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<UsingStatement>(classObject)->body = std::static_pointer_cast<ScopeStatement>(valueObject);
											return;
										}
									},
								}
							},{
								L"AstFunctionParameter",
								{
									{
										L"type",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstFunctionParameter>(classObject)->type = std::static_pointer_cast<IdentifierObject>(valueObject);
											return;
										}
									},{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstFunctionParameter>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"FunctionSignature",
								{
									{
										L"parameters",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<FunctionSignature>(classObject)->parameters.emplace_back(std::static_pointer_cast<AstFunctionParameter>(valueObject));
											return;
										}
									},{
										L"returnType",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<FunctionSignature>(classObject)->returnType = std::static_pointer_cast<IdentifierObject>(valueObject);
											return;
										}
									},{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<FunctionSignature>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"EnumFieldTypeDefine",
								{
									{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<EnumFieldTypeDefine>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},{
										L"value",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<EnumFieldTypeDefine>(classObject)->value=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"AstEnumTypeDefine",
								{
									{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstEnumTypeDefine>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},{
										L"base",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstEnumTypeDefine>(classObject)->base=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},{
										L"members",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstEnumTypeDefine>(classObject)->members.emplace_back(std::static_pointer_cast<EnumFieldTypeDefine>(valueObject));
											return;
										}
									},
								}
							},{
								L"AstClassMember",
								{
									{
										L"properties",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstClassMember>(classObject)->properties.emplace_back(std::static_pointer_cast<PrefixProperty>(valueObject));
											return;
										}
									},{
										L"define",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstClassMember>(classObject)->define = std::static_pointer_cast<AstTypeDefine>(valueObject);
											return;
										}
									},
								}
							},{
								L"AstNamesapceDefine",
								{
									{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstNamesapceDefine>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},{
										L"members",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstNamesapceDefine>(classObject)->members.emplace_back(std::static_pointer_cast<NamespaceMember>(valueObject));
											return;
										}
									},
								}
							},{
								L"AstUsingDeclExpression",
								{
									{
										L"module",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstUsingDeclExpression>(classObject)->module = std::static_pointer_cast<IdentifierObject>(valueObject);
											return;
										}
									},{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<AstUsingDeclExpression>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},
				};
				auto findIter = builderMap.find(className);
				assert(findIter!=builderMap.end());
				assert(findIter->second.find(fieldName)!=findIter->second.end());
				builderMap[className][fieldName](classObject,valueObject);
			}
			shared_ptr<void> ReflecteObjectByName(const wstring& name)
			{
				using reflectObjectType = shared_ptr<void>(*)();
				static unordered_map < wstring, reflectObjectType> objectMap =
				{
					
				{
					L"NewExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<NewExpression>();
					}
				},
				{
					L"AttachSuffixExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<AttachSuffixExpression>();
					}
				},
				{
					L"IdentifierObject",
					[]()->shared_ptr<void>
					{
						return make_shared<IdentifierObject>();
					}
				},
				{
					L"CharExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<CharExpression>();
					}
				},
				{
					L"DeclarationExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<DeclarationExpression>();
					}
				},
				{
					L"SubIdentifierObject",
					[]()->shared_ptr<void>
					{
						return make_shared<SubIdentifierObject>();
					}
				},
				{
					L"PrimaryIdentifierObject",
					[]()->shared_ptr<void>
					{
						return make_shared<PrimaryIdentifierObject>();
					}
				},
				{
					L"AstClassTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<AstClassTypeDefine>();
					}
				},
				{
					L"NormalIdentifierObject",
					[]()->shared_ptr<void>
					{
						return make_shared<NormalIdentifierObject>();
					}
				},
				{
					L"ThisIdentifierObject",
					[]()->shared_ptr<void>
					{
						return make_shared<ThisIdentifierObject>();
					}
				},
				{
					L"IntegerExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<IntegerExpression>();
					}
				},
				{
					L"VariableExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<VariableExpression>();
					}
				},
				{
					L"LogicExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<LogicExpression>();
					}
				},
				{
					L"BoolExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<BoolExpression>();
					}
				},
				{
					L"AstExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<AstExpression>();
					}
				},
				{
					L"CastExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<CastExpression>();
					}
				},
				{
					L"ArrayIdentifierObject",
					[]()->shared_ptr<void>
					{
						return make_shared<ArrayIdentifierObject>();
					}
				},
				{
					L"LogicAndExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<LogicAndExpression>();
					}
				},
				{
					L"PrefixProperty",
					[]()->shared_ptr<void>
					{
						return make_shared<PrefixProperty>();
					}
				},
				{
					L"LiteralExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<LiteralExpression>();
					}
				},
				{
					L"AstFieldTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<AstFieldTypeDefine>();
					}
				},
				{
					L"DoubleExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<DoubleExpression>();
					}
				},
				{
					L"NullExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<NullExpression>();
					}
				},
				{
					L"SwitchStatement",
					[]()->shared_ptr<void>
					{
						return make_shared<SwitchStatement>();
					}
				},
				{
					L"ParserASTRoot",
					[]()->shared_ptr<void>
					{
						return make_shared<ParserASTRoot>();
					}
				},
				{
					L"StringExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<StringExpression>();
					}
				},
				{
					L"ArithmeticExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<ArithmeticExpression>();
					}
				},
				{
					L"FunctionCallExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<FunctionCallExpression>();
					}
				},
				{
					L"NamespaceMember",
					[]()->shared_ptr<void>
					{
						return make_shared<NamespaceMember>();
					}
				},
				{
					L"AstTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<AstTypeDefine>();
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
					L"ContinueStatement",
					[]()->shared_ptr<void>
					{
						return make_shared<ContinueStatement>();
					}
				},
				{
					L"HighArithmeticExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<HighArithmeticExpression>();
					}
				},
				{
					L"LowArithmeticExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<LowArithmeticExpression>();
					}
				},
				{
					L"InterfaceMember",
					[]()->shared_ptr<void>
					{
						return make_shared<InterfaceMember>();
					}
				},
				{
					L"CompareExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<CompareExpression>();
					}
				},
				{
					L"LogicOrExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<LogicOrExpression>();
					}
				},
				{
					L"AttachPrefixExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<AttachPrefixExpression>();
					}
				},
				{
					L"AstStatement",
					[]()->shared_ptr<void>
					{
						return make_shared<AstStatement>();
					}
				},
				{
					L"ExpressionStatement",
					[]()->shared_ptr<void>
					{
						return make_shared<ExpressionStatement>();
					}
				},
				{
					L"ScopeStatement",
					[]()->shared_ptr<void>
					{
						return make_shared<ScopeStatement>();
					}
				},
				{
					L"ReturnStatement",
					[]()->shared_ptr<void>
					{
						return make_shared<ReturnStatement>();
					}
				},
				{
					L"IfStatement",
					[]()->shared_ptr<void>
					{
						return make_shared<IfStatement>();
					}
				},
				{
					L"WhileStatement",
					[]()->shared_ptr<void>
					{
						return make_shared<WhileStatement>();
					}
				},
				{
					L"AstInterfaceTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<AstInterfaceTypeDefine>();
					}
				},
				{
					L"ForStatement",
					[]()->shared_ptr<void>
					{
						return make_shared<ForStatement>();
					}
				},
				{
					L"AstFunctionTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<AstFunctionTypeDefine>();
					}
				},
				{
					L"CaseStatement",
					[]()->shared_ptr<void>
					{
						return make_shared<CaseStatement>();
					}
				},
				{
					L"DefaultStatement",
					[]()->shared_ptr<void>
					{
						return make_shared<DefaultStatement>();
					}
				},
				{
					L"BreakStatement",
					[]()->shared_ptr<void>
					{
						return make_shared<BreakStatement>();
					}
				},
				{
					L"UsingStatement",
					[]()->shared_ptr<void>
					{
						return make_shared<UsingStatement>();
					}
				},
				{
					L"AstFunctionParameter",
					[]()->shared_ptr<void>
					{
						return make_shared<AstFunctionParameter>();
					}
				},
				{
					L"FunctionSignature",
					[]()->shared_ptr<void>
					{
						return make_shared<FunctionSignature>();
					}
				},
				{
					L"EnumFieldTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<EnumFieldTypeDefine>();
					}
				},
				{
					L"AstEnumTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<AstEnumTypeDefine>();
					}
				},
				{
					L"AstClassMember",
					[]()->shared_ptr<void>
					{
						return make_shared<AstClassMember>();
					}
				},
				{
					L"AstNamesapceDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<AstNamesapceDefine>();
					}
				},
				{
					L"AstUsingDeclExpression",
					[]()->shared_ptr<void>
					{
						return make_shared<AstUsingDeclExpression>();
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
					for(auto&& iter : classNode->GetFieldMap())
					{
						auto fieldName = iter.first;
						for(auto&& nodeIter : iter.second)
						{
							auto fieldNode = parser.GetNonTermNodeByIndex(nodeIter);
							auto fieldObject = ReflecteObjectByName(fieldNode->GetName());
							parser.SaveHeterogeneousNode(fieldObject);
							ReflectionBuidler(className, fieldName, classObject, fieldObject);
							ztl::miniSharp::GenerateHeterogeneousParserTree(parser,fieldNode, fieldObject);
						}
					}
					for(auto&&iter : classNode->GetTermMap())
					{
						auto fieldName = iter.first;
						for(auto&& nodeIter : iter.second)
						{
							ReflectionBuidler(className, fieldName, classObject, parser.GetTermNodeByIndex(nodeIter));
						}
					}
				}
	
				shared_ptr<void> GenerateHeterogeneousParserTree(ztl::general_parser::GeneralLRExecutor& parser,ztl::general_parser::GeneralTreeNode* root)
				{
					assert(root != nullptr);
					auto rootObject = ReflecteObjectByName(root->GetName());
					parser.SaveHeterogeneousNode(rootObject);
					ztl::miniSharp::GenerateHeterogeneousParserTree(parser,root, rootObject);
					return rootObject;
				}
				shared_ptr<void>	GenerateHeterogeneousParserTree(ztl::general_parser::GeneralLRExecutor& parser)
				{
					return ztl::miniSharp::GenerateHeterogeneousParserTree(parser, parser.GetGeneralTreeRoot());
				}
				
				shared_ptr<ztl::general_parser::GeneralTableDefine> BootStrapDefineTable()
				{
					ztl::general_parser::GeneralTableWriter writer;
					writer.Head
						(
							ztl::general_parser::GeneralHeadInfoWriter()
							
					.Info(LR"(filename)",LR"(MiniSharpParser,
				ztl::general_parser::GeneralAttributeWriter()
				
				)")
				
					.Info(LR"(include)",LR"(Include/stdafx.h,
				ztl::general_parser::GeneralAttributeWriter()
				
				)")
				
					.Info(LR"(namespace)",LR"(ztl,
				ztl::general_parser::GeneralAttributeWriter()
				
				)")
				
					.Info(LR"(namespace)",LR"(miniSharp,
				ztl::general_parser::GeneralAttributeWriter()
				
				)")
				
					.Info(LR"(dirname)",LR"(miniSharp,
				ztl::general_parser::GeneralAttributeWriter()
				
				)")
				
						)
						.Token
						(
							ztl::general_parser::GeneralTokenWriter()
													.Token(L"FINISH",L"<\\$>")
			.Token(L"INTERFACE",LR"(interface)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"CLASS",LR"(class)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"ENUM",LR"(enum)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"IF",LR"(if)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"ELSE",LR"(else)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"SWTICH",LR"(switch)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"CASE",LR"(case)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"DEFAULT",LR"(default)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"WHILE",LR"(while)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"FOR",LR"(for)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"CAST",LR"(cast)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"BREAK",LR"(break)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"CONTINUE",LR"(continue)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"RETURN",LR"(return)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"STATIC",LR"(static)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"READONLY",LR"(readonly)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"THIS",LR"(this)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"CONST",LR"(const)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"PUBLIC",LR"(public)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"PRIVATE",LR"(private)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"VIRTUAL",LR"(virtual)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"TRUEVALUE",LR"(true)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"FALSEVALUE",LR"(false)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"INT",LR"(int)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"BOOL",LR"(bool)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"DOUBLE",LR"(double)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"VOID",LR"(void)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"CHAR",LR"(char)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"STRINGTYPE",LR"(string)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"NULL",LR"(null)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"USING",LR"(using)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"NAMESPACE",LR"(namespace)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"INCREASESELF",LR"(\+\+)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"ADD",LR"(\+)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"SUB",LR"(-)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"MUL",LR"(\*)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"DIV",LR"(\\)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"EQUAL",LR"(==)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"NOTEQUAL",LR"(!=)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"LESSEQUAL",LR"(<=)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"GREATEQUAL",LR"(>=)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"GREAT",LR"(>)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"LESS",LR"(<)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"NEGATION",LR"(!)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"DECREASESELF",LR"(--)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"NEW",LR"(new)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"OPEN",LR"(\{)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"CLOSE",LR"(\})",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"SEMICOLON",LR"(;)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"COLON",LR"(:)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"COMMA",LR"(,)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"ASSIGN",LR"(=)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"OR",LR"(\|\|)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"AND",LR"(&&)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"OPTOPEN",LR"(\[)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"OPTCLOSE",LR"(\])",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"PREOPEN",LR"(\()",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"PRECLOSE",LR"(\))",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"QUOT",LR"(")",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"CHARVALUE",LR"('.')",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"DOUBLEVALUE",LR"((\d|([1-9]\d+))\.\d+)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"INTVALUE",LR"(\d|([1-9]\d+))",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"DOT",LR"(\.)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"NAME",LR"([a-zA-Z_]\w*)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"STRING",LR"("(\\\\|\\"|[^"])*")",
				ztl::general_parser::GeneralAttributeWriter()
				
					.Attribute(L"Color",
					ztl::general_parser::GeneralAttributeParamsWriter()
					.Param(L"String")
				
					)
				
				)
.IgnoreToken(L"SPACE",LR"(\s+)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.IgnoreToken(L"LINENOTE",LR"(//[^\n]*\n)",
				ztl::general_parser::GeneralAttributeWriter()
				
					.Attribute(L"Color",
					ztl::general_parser::GeneralAttributeParamsWriter()
					.Param(L"Note")
				
					)
				
				)
.IgnoreToken(L"BLOCKNOTE",LR"(/\*.*?\*/)",
				ztl::general_parser::GeneralAttributeWriter()
				
					.Attribute(L"Color",
					ztl::general_parser::GeneralAttributeParamsWriter()
					.Param(L"Note")
				
					)
				
				)

						)
						.Type
						(
							
					ztl::general_parser::GeneralTypeListWriter()
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"IdentifierObject")
							
							
							
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"PrimaryIdentifierObject")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"primary",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"IdentifierObject"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"NormalIdentifierObject")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"IdentifierObject"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"SubIdentifierObject")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"IdentifierObject"), L"scope",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"IdentifierObject"), L"right",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"IdentifierObject"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"ThisIdentifierObject")
							
							
							.ParentType(ztl::general_parser::Normal(L"IdentifierObject"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"AstExpression")
							
							
							
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"ArrayIdentifierObject")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"IdentifierObject"), L"element",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"AstExpression")), L"numbers",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"IdentifierObject"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"LiteralExpression")
							
							
							.ParentType(ztl::general_parser::Normal(L"AstExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"VariableExpression")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"IdentifierObject"), L"variable",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"LiteralExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"IntegerExpression")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"value",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"LiteralExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"DoubleExpression")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"value",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"LiteralExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"BoolExpression")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"value",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"LiteralExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"NullExpression")
							
							
							.ParentType(ztl::general_parser::Normal(L"LiteralExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"StringExpression")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"value",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"LiteralExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"CharExpression")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"value",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"LiteralExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Enum
						(
							ztl::general_parser::GeneralEnumTypeWriter()
							.Name(L"PrefixPropertyValue")
							
						.Member(ztl::general_parser::EnumMember(L"Constants",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
					
						.Member(ztl::general_parser::EnumMember(L"ReadOnly",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
					
						.Member(ztl::general_parser::EnumMember(L"Static",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
					
						.Member(ztl::general_parser::EnumMember(L"Public",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
					
						.Member(ztl::general_parser::EnumMember(L"Private",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
					
						.Member(ztl::general_parser::EnumMember(L"Virtual",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
					
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"PrefixProperty")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"PrefixPropertyValue"), L"value",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"DeclarationExpression")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"IdentifierObject"), L"type",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"AstExpression")), L"expressions",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"FunctionCallExpression")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"IdentifierObject"), L"name",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"AstExpression")), L"args",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"NewExpression")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"IdentifierObject"), L"type",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"AstExpression")), L"args",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"AssignExpression")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"IdentifierObject"), L"variable",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"right",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"ArithmeticExpression")
							
							
							.ParentType(ztl::general_parser::Normal(L"AstExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"HighArithmeticExpression")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"op",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"left",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"right",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"ArithmeticExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"LowArithmeticExpression")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"op",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"left",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"right",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"ArithmeticExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"LogicExpression")
							
							
							.ParentType(ztl::general_parser::Normal(L"AstExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"CompareExpression")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"op",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"left",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"right",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"LogicExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"LogicOrExpression")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"left",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"right",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"LogicExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"LogicAndExpression")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"left",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"right",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"LogicExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"AttachPrefixExpression")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"op",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"expression",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"AttachSuffixExpression")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"op",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"expression",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"CastExpression")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"IdentifierObject"), L"type",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"expression",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"AstStatement")
							
							
							
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"ExpressionStatement")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"PrefixProperty")), L"properties",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"expression",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstStatement"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"ScopeStatement")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"AstStatement")), L"statements",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstStatement"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"IfStatement")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"conditional",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"ScopeStatement"), L"truePath",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstStatement"), L"falsePath",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstStatement"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"WhileStatement")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"conditional",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"ScopeStatement"), L"body",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstStatement"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"ForStatement")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"ExpressionStatement"), L"initExpression",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"conditional",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"AstExpression")), L"stepExpressions",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"ScopeStatement"), L"body",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstStatement"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"CaseStatement")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"value",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"AstStatement")), L"statements",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstStatement"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"DefaultStatement")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"AstStatement")), L"statements",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstStatement"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"SwitchStatement")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"conditional",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"CaseStatement")), L"cases",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstStatement"), L"default",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstStatement"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"ReturnStatement")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"expression",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstStatement"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"BreakStatement")
							
							
							.ParentType(ztl::general_parser::Normal(L"AstStatement"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"ContinueStatement")
							
							
							.ParentType(ztl::general_parser::Normal(L"AstStatement"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"UsingStatement")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstExpression"), L"source",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"ScopeStatement"), L"body",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstStatement"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"AstTypeDefine")
							
							
							
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"AstFieldTypeDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"IdentifierObject"), L"type",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstTypeDefine"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"AstFunctionParameter")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"IdentifierObject"), L"type",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
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
							.Name(L"FunctionSignature")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"IdentifierObject"), L"returnType",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"AstFunctionParameter")), L"parameters",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
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
							.Name(L"AstFunctionTypeDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"FunctionSignature"), L"sign",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"ScopeStatement"), L"body",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstTypeDefine"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"InterfaceMember")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"PrefixProperty")), L"properties",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"FunctionSignature"), L"sign",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"AstInterfaceTypeDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"IdentifierObject")), L"parents",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"InterfaceMember")), L"members",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstTypeDefine"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"EnumFieldTypeDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"value",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"AstEnumTypeDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"base",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"EnumFieldTypeDefine")), L"members",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstTypeDefine"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"AstClassMember")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"PrefixProperty")), L"properties",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstTypeDefine"), L"define",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"AstClassTypeDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"IdentifierObject"), L"parent",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"IdentifierObject")), L"parentInterfaces",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"AstClassMember")), L"members",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstTypeDefine"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"NamespaceMember")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"PrefixProperty")), L"properties",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"AstTypeDefine"), L"define",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"AstNamesapceDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"NamespaceMember")), L"members",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstTypeDefine"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"AstUsingDeclExpression")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"IdentifierObject"), L"module",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							.ParentType(ztl::general_parser::Normal(L"AstExpression"))
							.Attributes(
				ztl::general_parser::GeneralAttributeWriter()
				
				)
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"ParserASTRoot")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"AstUsingDeclExpression")), L"usings",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"NamespaceMember")), L"members",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
							
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
							.Name(L"IdentifierObjectUnitRule")
							.ReturnType(ztl::general_parser::Normal(L"IdentifierObject"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					
					)
					.Create(ztl::general_parser::Normal(L"NormalIdentifierObject"))
					
				
						|
					(
						ztl::general_parser::GrammarSymbol(L"THIS")
					)
					.Create(ztl::general_parser::Normal(L"ThisIdentifierObject"))
					
				
						|
					(
						
						( 
						ztl::general_parser::GrammarSymbol(L"INT")[L"primary"]
					 | 
						( 
						ztl::general_parser::GrammarSymbol(L"DOUBLE")[L"primary"]
					 | 
						( 
						ztl::general_parser::GrammarSymbol(L"CHAR")[L"primary"]
					 | 
						( 
						ztl::general_parser::GrammarSymbol(L"STRINGTYPE")[L"primary"]
					 | 
						( 
						ztl::general_parser::GrammarSymbol(L"BOOL")[L"primary"]
					 | 
						ztl::general_parser::GrammarSymbol(L"VOID")[L"primary"]
					 )
					 )
					 )
					 )
					 )
					
					)
					.Create(ztl::general_parser::Normal(L"PrimaryIdentifierObject"))
					
				
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"IdentifierObjectUnitRule")[L"element"]
					+ztl::general_parser::GrammarSymbol(L"OPTOPEN")+
						ztl::general_parser::GrammarSymbol(L"ExpressionRule")[L"numbers"]
					+ztl::general_parser::GrammarSymbol(L"OPTCLOSE")+*(ztl::general_parser::GrammarSymbol(L"OPTOPEN")+
						ztl::general_parser::GrammarSymbol(L"ExpressionRule")[L"numbers"]
					+ztl::general_parser::GrammarSymbol(L"OPTCLOSE"))
					)
					.Create(ztl::general_parser::Normal(L"ArrayIdentifierObject"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"IdentifierObjectRule")
							.ReturnType(ztl::general_parser::Normal(L"IdentifierObject"))
							
						|!(ztl::general_parser::GrammarSymbol(L"IdentifierObjectUnitRule"))
				
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"IdentifierObjectUnitRule")[L"scope"]
					+ztl::general_parser::GrammarSymbol(L"DOT")+
						ztl::general_parser::GrammarSymbol(L"IdentifierObjectRule")[L"right"]
					
					)
					.Create(ztl::general_parser::Normal(L"SubIdentifierObject"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"PrefixPropertyRule")
							.ReturnType(ztl::general_parser::Normal(L"PrefixProperty"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"CONST")
					)
					.Create(ztl::general_parser::Normal(L"PrefixProperty"))
					
						.Setter(L"value", L"Constants")
					
				
						|
					(
						ztl::general_parser::GrammarSymbol(L"READONLY")
					)
					.Create(ztl::general_parser::Normal(L"PrefixProperty"))
					
						.Setter(L"value", L"ReadOnly")
					
				
						|
					(
						ztl::general_parser::GrammarSymbol(L"PRIVATE")
					)
					.Create(ztl::general_parser::Normal(L"PrefixProperty"))
					
						.Setter(L"value", L"Private")
					
				
						|
					(
						ztl::general_parser::GrammarSymbol(L"STATIC")
					)
					.Create(ztl::general_parser::Normal(L"PrefixProperty"))
					
						.Setter(L"value", L"Static")
					
				
						|
					(
						ztl::general_parser::GrammarSymbol(L"VIRTUAL")
					)
					.Create(ztl::general_parser::Normal(L"PrefixProperty"))
					
						.Setter(L"value", L"Virtual")
					
				
						|
					(
						ztl::general_parser::GrammarSymbol(L"PUBLIC")
					)
					.Create(ztl::general_parser::Normal(L"PrefixProperty"))
					
						.Setter(L"value", L"Public")
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"VariableExpressionRule")
							.ReturnType(ztl::general_parser::Normal(L"AstExpression"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"IdentifierObjectRule")[L"variable"]
					
					)
					.Create(ztl::general_parser::Normal(L"VariableExpression"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"LiteralExpressionRule")
							.ReturnType(ztl::general_parser::Normal(L"AstExpression"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"INTVALUE")[L"value"]
					
					)
					.Create(ztl::general_parser::Normal(L"IntegerExpression"))
					
				
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"DOUBLEVALUE")[L"value"]
					
					)
					.Create(ztl::general_parser::Normal(L"DoubleExpression"))
					
				
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"STRING")[L"value"]
					
					)
					.Create(ztl::general_parser::Normal(L"StringExpression"))
					
				
						|
					(
						
						( 
						ztl::general_parser::GrammarSymbol(L"TRUEVALUE")[L"value"]
					 | 
						ztl::general_parser::GrammarSymbol(L"FALSEVALUE")[L"value"]
					 )
					
					)
					.Create(ztl::general_parser::Normal(L"BoolExpression"))
					
				
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"CHARVALUE")[L"value"]
					
					)
					.Create(ztl::general_parser::Normal(L"CharExpression"))
					
				
						|
					(
						ztl::general_parser::GrammarSymbol(L"NULL")
					)
					.Create(ztl::general_parser::Normal(L"NullExpression"))
					
				
						|!(ztl::general_parser::GrammarSymbol(L"VariableExpressionRule"))
				
						|ztl::general_parser::GrammarSymbol(L"PREOPEN")+!(ztl::general_parser::GrammarSymbol(L"ExpressionRule"))+ztl::general_parser::GrammarSymbol(L"PRECLOSE")
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"FunctionCallExpressionRule")
							.ReturnType(ztl::general_parser::Normal(L"AstExpression"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"IdentifierObjectRule")[L"name"]
					+ztl::general_parser::GrammarSymbol(L"PREOPEN")+~(
						ztl::general_parser::GrammarSymbol(L"LogicOrExpressionRule")[L"args"]
					)+*(ztl::general_parser::GrammarSymbol(L"COMMA")+
						ztl::general_parser::GrammarSymbol(L"LogicOrExpressionRule")[L"args"]
					)+ztl::general_parser::GrammarSymbol(L"PRECLOSE")
					)
					.Create(ztl::general_parser::Normal(L"FunctionCallExpression"))
					
				
						|
					(
						ztl::general_parser::GrammarSymbol(L"NEW")+
						ztl::general_parser::GrammarSymbol(L"IdentifierObjectRule")[L"type"]
					+ztl::general_parser::GrammarSymbol(L"PREOPEN")+~(
						ztl::general_parser::GrammarSymbol(L"LogicOrExpressionRule")[L"args"]
					)+*(ztl::general_parser::GrammarSymbol(L"COMMA")+
						ztl::general_parser::GrammarSymbol(L"LogicOrExpressionRule")[L"args"]
					)+ztl::general_parser::GrammarSymbol(L"PRECLOSE")
					)
					.Create(ztl::general_parser::Normal(L"NewExpression"))
					
				
						|!(ztl::general_parser::GrammarSymbol(L"LiteralExpressionRule"))
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"AttachSuffixExpressionRule")
							.ReturnType(ztl::general_parser::Normal(L"AstExpression"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"FunctionCallExpressionRule")[L"expression"]
					+
						( 
						ztl::general_parser::GrammarSymbol(L"INCREASESELF")[L"op"]
					 | 
						ztl::general_parser::GrammarSymbol(L"DECREASESELF")[L"op"]
					 )
					
					)
					.Create(ztl::general_parser::Normal(L"AttachSuffixExpression"))
					
				
						|!(ztl::general_parser::GrammarSymbol(L"FunctionCallExpressionRule"))
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"AttachPrefixExpressionRule")
							.ReturnType(ztl::general_parser::Normal(L"AstExpression"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"NEGATION")[L"op"]
					+
						ztl::general_parser::GrammarSymbol(L"AttachSuffixExpressionRule")[L"expression"]
					
					)
					.Create(ztl::general_parser::Normal(L"AttachPrefixExpression"))
					
				
						|!(ztl::general_parser::GrammarSymbol(L"AttachSuffixExpressionRule"))
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"CastExpressionRule")
							.ReturnType(ztl::general_parser::Normal(L"AstExpression"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"CAST")+ztl::general_parser::GrammarSymbol(L"PREOPEN")+
						ztl::general_parser::GrammarSymbol(L"IdentifierObjectRule")[L"type"]
					+ztl::general_parser::GrammarSymbol(L"PRECLOSE")+
						ztl::general_parser::GrammarSymbol(L"CastExpressionRule")[L"expression"]
					
					)
					.Create(ztl::general_parser::Normal(L"CastExpression"))
					
				
						|!(ztl::general_parser::GrammarSymbol(L"AttachPrefixExpressionRule"))
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"HighArithmeticExpressionRule")
							.ReturnType(ztl::general_parser::Normal(L"AstExpression"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"HighArithmeticExpressionRule")[L"left"]
					+
						( 
						ztl::general_parser::GrammarSymbol(L"MUL")[L"op"]
					 | 
						ztl::general_parser::GrammarSymbol(L"DIV")[L"op"]
					 )
					+
						ztl::general_parser::GrammarSymbol(L"CastExpressionRule")[L"right"]
					
					)
					.Create(ztl::general_parser::Normal(L"HighArithmeticExpression"))
					
				
						|!(ztl::general_parser::GrammarSymbol(L"CastExpressionRule"))
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"LowArithmeticExpressionRule")
							.ReturnType(ztl::general_parser::Normal(L"AstExpression"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"LowArithmeticExpressionRule")[L"left"]
					+
						( 
						ztl::general_parser::GrammarSymbol(L"ADD")[L"op"]
					 | 
						ztl::general_parser::GrammarSymbol(L"SUB")[L"op"]
					 )
					+
						ztl::general_parser::GrammarSymbol(L"HighArithmeticExpressionRule")[L"right"]
					
					)
					.Create(ztl::general_parser::Normal(L"LowArithmeticExpression"))
					
				
						|!(ztl::general_parser::GrammarSymbol(L"HighArithmeticExpressionRule"))
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"CompareExpressionRule")
							.ReturnType(ztl::general_parser::Normal(L"AstExpression"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"CompareExpressionRule")[L"left"]
					+
						( 
						ztl::general_parser::GrammarSymbol(L"EQUAL")[L"op"]
					 | 
						( 
						ztl::general_parser::GrammarSymbol(L"NOTEQUAL")[L"op"]
					 | 
						( 
						ztl::general_parser::GrammarSymbol(L"LESSEQUAL")[L"op"]
					 | 
						( 
						ztl::general_parser::GrammarSymbol(L"GREATEQUAL")[L"op"]
					 | 
						( 
						ztl::general_parser::GrammarSymbol(L"GREAT")[L"op"]
					 | 
						ztl::general_parser::GrammarSymbol(L"LESS")[L"op"]
					 )
					 )
					 )
					 )
					 )
					+
						ztl::general_parser::GrammarSymbol(L"LowArithmeticExpressionRule")[L"right"]
					
					)
					.Create(ztl::general_parser::Normal(L"CompareExpression"))
					
				
						|!(ztl::general_parser::GrammarSymbol(L"LowArithmeticExpressionRule"))
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"LogicAndExpressionRule")
							.ReturnType(ztl::general_parser::Normal(L"AstExpression"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"LogicAndExpressionRule")[L"left"]
					+ztl::general_parser::GrammarSymbol(L"AND")+
						ztl::general_parser::GrammarSymbol(L"CompareExpressionRule")[L"right"]
					
					)
					.Create(ztl::general_parser::Normal(L"LogicAndExpression"))
					
				
						|!(ztl::general_parser::GrammarSymbol(L"CompareExpressionRule"))
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"LogicOrExpressionRule")
							.ReturnType(ztl::general_parser::Normal(L"AstExpression"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"LogicOrExpressionRule")[L"left"]
					+ztl::general_parser::GrammarSymbol(L"OR")+
						ztl::general_parser::GrammarSymbol(L"LogicAndExpressionRule")[L"right"]
					
					)
					.Create(ztl::general_parser::Normal(L"LogicOrExpression"))
					
				
						|!(ztl::general_parser::GrammarSymbol(L"LogicAndExpressionRule"))
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"AssignExpressionRule")
							.ReturnType(ztl::general_parser::Normal(L"AstExpression"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"IdentifierObjectRule")[L"variable"]
					+ztl::general_parser::GrammarSymbol(L"ASSIGN")+
						ztl::general_parser::GrammarSymbol(L"AssignExpressionRule")[L"right"]
					
					)
					.Create(ztl::general_parser::Normal(L"AssignExpression"))
					
				
						|!(ztl::general_parser::GrammarSymbol(L"LogicOrExpressionRule"))
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"DeclarationExpressionRule")
							.ReturnType(ztl::general_parser::Normal(L"AstExpression"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"IdentifierObjectRule")[L"type"]
					+
						ztl::general_parser::GrammarSymbol(L"AssignExpressionRule")[L"expressions"]
					+*(ztl::general_parser::GrammarSymbol(L"COMMA")+
						ztl::general_parser::GrammarSymbol(L"AssignExpressionRule")[L"expressions"]
					)
					)
					.Create(ztl::general_parser::Normal(L"DeclarationExpression"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"ExpressionRule")
							.ReturnType(ztl::general_parser::Normal(L"AstExpression"))
							
						|!(ztl::general_parser::GrammarSymbol(L"DeclarationExpressionRule"))
				
						|!(ztl::general_parser::GrammarSymbol(L"AssignExpressionRule"))
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"ExpressionStatementRule")
							.ReturnType(ztl::general_parser::Normal(L"ExpressionStatement"))
							
						|
					(
						*(
						ztl::general_parser::GrammarSymbol(L"PrefixPropertyRule")[L"properties"]
					)+
						ztl::general_parser::GrammarSymbol(L"ExpressionRule")[L"expression"]
					+ztl::general_parser::GrammarSymbol(L"SEMICOLON")
					)
					.Create(ztl::general_parser::Normal(L"ExpressionStatement"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"IfStatementRule")
							.ReturnType(ztl::general_parser::Normal(L"IfStatement"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"IF")+ztl::general_parser::GrammarSymbol(L"PREOPEN")+
						ztl::general_parser::GrammarSymbol(L"LogicOrExpressionRule")[L"conditional"]
					+ztl::general_parser::GrammarSymbol(L"PRECLOSE")+
						ztl::general_parser::GrammarSymbol(L"ScopeStatementRule")[L"truePath"]
					+~(ztl::general_parser::GrammarSymbol(L"ELSE")+
						( 
						ztl::general_parser::GrammarSymbol(L"IfStatementRule")[L"falsePath"]
					 | 
						ztl::general_parser::GrammarSymbol(L"ScopeStatementRule")[L"falsePath"]
					 )
					)
					)
					.Create(ztl::general_parser::Normal(L"IfStatement"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"UsingStatementRule")
							.ReturnType(ztl::general_parser::Normal(L"UsingStatement"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"USING")+ztl::general_parser::GrammarSymbol(L"PREOPEN")+
						ztl::general_parser::GrammarSymbol(L"DeclarationExpressionRule")[L"source"]
					+ztl::general_parser::GrammarSymbol(L"PRECLOSE")+
						ztl::general_parser::GrammarSymbol(L"ScopeStatementRule")[L"body"]
					
					)
					.Create(ztl::general_parser::Normal(L"UsingStatement"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"WhileStatementRule")
							.ReturnType(ztl::general_parser::Normal(L"WhileStatement"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"WHILE")+ztl::general_parser::GrammarSymbol(L"PREOPEN")+
						ztl::general_parser::GrammarSymbol(L"LogicOrExpressionRule")[L"conditional"]
					+ztl::general_parser::GrammarSymbol(L"PRECLOSE")+
						ztl::general_parser::GrammarSymbol(L"ScopeStatementRule")[L"body"]
					
					)
					.Create(ztl::general_parser::Normal(L"WhileStatement"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"ForStatementRule")
							.ReturnType(ztl::general_parser::Normal(L"ForStatement"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"FOR")+ztl::general_parser::GrammarSymbol(L"PREOPEN")+
						ztl::general_parser::GrammarSymbol(L"ExpressionStatementRule")[L"initExpression"]
					+
						ztl::general_parser::GrammarSymbol(L"LogicOrExpressionRule")[L"conditional"]
					+ztl::general_parser::GrammarSymbol(L"SEMICOLON")+~(
						ztl::general_parser::GrammarSymbol(L"AssignExpressionRule")[L"stepExpressions"]
					)+*(ztl::general_parser::GrammarSymbol(L"COMMA")+
						ztl::general_parser::GrammarSymbol(L"AssignExpressionRule")[L"stepExpressions"]
					)+ztl::general_parser::GrammarSymbol(L"PRECLOSE")+
						ztl::general_parser::GrammarSymbol(L"ScopeStatementRule")[L"body"]
					
					)
					.Create(ztl::general_parser::Normal(L"ForStatement"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"CaseStatementRule")
							.ReturnType(ztl::general_parser::Normal(L"CaseStatement"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"CASE")+
						ztl::general_parser::GrammarSymbol(L"LiteralExpressionRule")[L"value"]
					+ztl::general_parser::GrammarSymbol(L"COLON")+*(
						( 
						ztl::general_parser::GrammarSymbol(L"ExpressionStatementRule")[L"statements"]
					 | 
						ztl::general_parser::GrammarSymbol(L"ScopeStatementRule")[L"statements"]
					 )
					)+ztl::general_parser::GrammarSymbol(L"BREAK")+ztl::general_parser::GrammarSymbol(L"SEMICOLON")
					)
					.Create(ztl::general_parser::Normal(L"CaseStatement"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"DefaultStatementRule")
							.ReturnType(ztl::general_parser::Normal(L"DefaultStatement"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"DEFAULT")+ztl::general_parser::GrammarSymbol(L"COLON")+*(
						( 
						ztl::general_parser::GrammarSymbol(L"ExpressionStatementRule")[L"statements"]
					 | 
						ztl::general_parser::GrammarSymbol(L"ScopeStatementRule")[L"statements"]
					 )
					)+ztl::general_parser::GrammarSymbol(L"BREAK")+ztl::general_parser::GrammarSymbol(L"SEMICOLON")
					)
					.Create(ztl::general_parser::Normal(L"DefaultStatement"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"SwitchStatementRule")
							.ReturnType(ztl::general_parser::Normal(L"SwitchStatement"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"SWTICH")+ztl::general_parser::GrammarSymbol(L"PREOPEN")+
						ztl::general_parser::GrammarSymbol(L"VariableExpressionRule")[L"conditional"]
					+ztl::general_parser::GrammarSymbol(L"PRECLOSE")+ztl::general_parser::GrammarSymbol(L"OPEN")+*(
						ztl::general_parser::GrammarSymbol(L"CaseStatementRule")[L"cases"]
					)+
						ztl::general_parser::GrammarSymbol(L"DefaultStatementRule")[L"default"]
					+ztl::general_parser::GrammarSymbol(L"CLOSE")
					)
					.Create(ztl::general_parser::Normal(L"SwitchStatement"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"BreakStatementRule")
							.ReturnType(ztl::general_parser::Normal(L"BreakStatement"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"BREAK")+ztl::general_parser::GrammarSymbol(L"SEMICOLON")
					)
					.Create(ztl::general_parser::Normal(L"BreakStatement"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"ContinueStatementRule")
							.ReturnType(ztl::general_parser::Normal(L"ContinueStatement"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"CONTINUE")+ztl::general_parser::GrammarSymbol(L"SEMICOLON")
					)
					.Create(ztl::general_parser::Normal(L"ContinueStatement"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"ReturnStatementRule")
							.ReturnType(ztl::general_parser::Normal(L"ReturnStatement"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"RETURN")+
						ztl::general_parser::GrammarSymbol(L"LogicOrExpressionRule")[L"expression"]
					+ztl::general_parser::GrammarSymbol(L"SEMICOLON")
					)
					.Create(ztl::general_parser::Normal(L"ReturnStatement"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"ScopeStatementRule")
							.ReturnType(ztl::general_parser::Normal(L"ScopeStatement"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"OPEN")+*(
						( 
						ztl::general_parser::GrammarSymbol(L"IfStatementRule")[L"statements"]
					 | 
						( 
						ztl::general_parser::GrammarSymbol(L"UsingStatementRule")[L"statements"]
					 | 
						( 
						ztl::general_parser::GrammarSymbol(L"WhileStatementRule")[L"statements"]
					 | 
						( 
						ztl::general_parser::GrammarSymbol(L"ForStatementRule")[L"statements"]
					 | 
						( 
						ztl::general_parser::GrammarSymbol(L"SwitchStatementRule")[L"statements"]
					 | 
						( 
						ztl::general_parser::GrammarSymbol(L"ScopeStatementRule")[L"statements"]
					 | 
						( 
						ztl::general_parser::GrammarSymbol(L"ExpressionStatementRule")[L"statements"]
					 | 
						( 
						ztl::general_parser::GrammarSymbol(L"BreakStatementRule")[L"statements"]
					 | 
						( 
						ztl::general_parser::GrammarSymbol(L"ContinueStatementRule")[L"statements"]
					 | 
						ztl::general_parser::GrammarSymbol(L"ReturnStatementRule")[L"statements"]
					 )
					 )
					 )
					 )
					 )
					 )
					 )
					 )
					 )
					)+ztl::general_parser::GrammarSymbol(L"CLOSE")
					)
					.Create(ztl::general_parser::Normal(L"ScopeStatement"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"FunctionParameterRule")
							.ReturnType(ztl::general_parser::Normal(L"AstFunctionParameter"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"IdentifierObjectRule")[L"type"]
					+
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					
					)
					.Create(ztl::general_parser::Normal(L"AstFunctionParameter"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"FunctionSignatureRule")
							.ReturnType(ztl::general_parser::Normal(L"FunctionSignature"))
							
						|
					(
						~(
						ztl::general_parser::GrammarSymbol(L"IdentifierObjectRule")[L"returnType"]
					)+
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					+ztl::general_parser::GrammarSymbol(L"PREOPEN")+~(
						ztl::general_parser::GrammarSymbol(L"FunctionParameterRule")[L"parameters"]
					)+*(ztl::general_parser::GrammarSymbol(L"COMMA")+
						ztl::general_parser::GrammarSymbol(L"FunctionParameterRule")[L"parameters"]
					)+ztl::general_parser::GrammarSymbol(L"PRECLOSE")
					)
					.Create(ztl::general_parser::Normal(L"FunctionSignature"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"FunctionTypeRule")
							.ReturnType(ztl::general_parser::Normal(L"AstFunctionTypeDefine"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"FunctionSignatureRule")[L"sign"]
					+
						ztl::general_parser::GrammarSymbol(L"ScopeStatementRule")[L"body"]
					
					)
					.Create(ztl::general_parser::Normal(L"AstFunctionTypeDefine"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"FieldTypeRule")
							.ReturnType(ztl::general_parser::Normal(L"AstFieldTypeDefine"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"IdentifierObjectRule")[L"type"]
					+
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					+ztl::general_parser::GrammarSymbol(L"SEMICOLON")
					)
					.Create(ztl::general_parser::Normal(L"AstFieldTypeDefine"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"InterfaceMemberRule")
							.ReturnType(ztl::general_parser::Normal(L"InterfaceMember"))
							
						|
					(
						*(
						ztl::general_parser::GrammarSymbol(L"PrefixPropertyRule")[L"properties"]
					)+
						ztl::general_parser::GrammarSymbol(L"FunctionSignatureRule")[L"sign"]
					
					)
					.Create(ztl::general_parser::Normal(L"InterfaceMember"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"InterfaceTypeRule")
							.ReturnType(ztl::general_parser::Normal(L"AstInterfaceTypeDefine"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"INTERFACE")+
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					+~(ztl::general_parser::GrammarSymbol(L"COLON")+
						ztl::general_parser::GrammarSymbol(L"IdentifierObjectRule")[L"parents"]
					)+*(ztl::general_parser::GrammarSymbol(L"COMMA")+
						ztl::general_parser::GrammarSymbol(L"IdentifierObjectRule")[L"parents"]
					)+ztl::general_parser::GrammarSymbol(L"OPEN")+*(
						ztl::general_parser::GrammarSymbol(L"InterfaceMemberRule")[L"members"]
					+ztl::general_parser::GrammarSymbol(L"SEMICOLON"))+ztl::general_parser::GrammarSymbol(L"CLOSE")
					)
					.Create(ztl::general_parser::Normal(L"AstInterfaceTypeDefine"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"EnumFieldTypeDefineRule")
							.ReturnType(ztl::general_parser::Normal(L"EnumFieldTypeDefine"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					+~(ztl::general_parser::GrammarSymbol(L"ASSIGN")+
						( 
						ztl::general_parser::GrammarSymbol(L"CHARVALUE")[L"value"]
					 | 
						ztl::general_parser::GrammarSymbol(L"INTVALUE")[L"value"]
					 )
					)
					)
					.Create(ztl::general_parser::Normal(L"EnumFieldTypeDefine"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"AstEnumTypeDefineRule")
							.ReturnType(ztl::general_parser::Normal(L"AstEnumTypeDefine"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"ENUM")+
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					+~(ztl::general_parser::GrammarSymbol(L"COLON")+
						( 
						ztl::general_parser::GrammarSymbol(L"INT")[L"base"]
					 | 
						ztl::general_parser::GrammarSymbol(L"CHAR")[L"base"]
					 )
					)+ztl::general_parser::GrammarSymbol(L"OPEN")+*(
						ztl::general_parser::GrammarSymbol(L"EnumFieldTypeDefineRule")[L"members"]
					+ztl::general_parser::GrammarSymbol(L"COMMA"))+ztl::general_parser::GrammarSymbol(L"CLOSE")
					)
					.Create(ztl::general_parser::Normal(L"AstEnumTypeDefine"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"AstClassMemberDefineRule")
							.ReturnType(ztl::general_parser::Normal(L"AstClassMember"))
							
						|
					(
						*(
						ztl::general_parser::GrammarSymbol(L"PrefixPropertyRule")[L"properties"]
					)+
						( 
						ztl::general_parser::GrammarSymbol(L"InterfaceTypeRule")[L"define"]
					 | 
						( 
						ztl::general_parser::GrammarSymbol(L"FieldTypeRule")[L"define"]
					 | 
						( 
						ztl::general_parser::GrammarSymbol(L"FunctionTypeRule")[L"define"]
					 | 
						( 
						ztl::general_parser::GrammarSymbol(L"ClassTypeDefineRule")[L"define"]
					 | 
						ztl::general_parser::GrammarSymbol(L"AstEnumTypeDefineRule")[L"define"]
					 )
					 )
					 )
					 )
					
					)
					.Create(ztl::general_parser::Normal(L"AstClassMember"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"ClassTypeDefineRule")
							.ReturnType(ztl::general_parser::Normal(L"AstClassTypeDefine"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"CLASS")+
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					+~(ztl::general_parser::GrammarSymbol(L"COLON")+
						ztl::general_parser::GrammarSymbol(L"IdentifierObjectRule")[L"parent"]
					)+*(ztl::general_parser::GrammarSymbol(L"COMMA")+
						ztl::general_parser::GrammarSymbol(L"IdentifierObjectRule")[L"parentInterfaces"]
					)+ztl::general_parser::GrammarSymbol(L"OPEN")+*(
						ztl::general_parser::GrammarSymbol(L"AstClassMemberDefineRule")[L"members"]
					)+ztl::general_parser::GrammarSymbol(L"CLOSE")
					)
					.Create(ztl::general_parser::Normal(L"AstClassTypeDefine"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"TypeDefineRule")
							.ReturnType(ztl::general_parser::Normal(L"AstTypeDefine"))
							
						|!(ztl::general_parser::GrammarSymbol(L"AstEnumTypeDefineRule"))
				
						|!(ztl::general_parser::GrammarSymbol(L"ClassTypeDefineRule"))
				
						|!(ztl::general_parser::GrammarSymbol(L"InterfaceTypeRule"))
				
						|!(ztl::general_parser::GrammarSymbol(L"FunctionTypeRule"))
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"NamespaceMemberRule")
							.ReturnType(ztl::general_parser::Normal(L"NamespaceMember"))
							
						|
					(
						*(
						ztl::general_parser::GrammarSymbol(L"PrefixPropertyRule")[L"properties"]
					)+
						( 
						ztl::general_parser::GrammarSymbol(L"TypeDefineRule")[L"define"]
					 | 
						ztl::general_parser::GrammarSymbol(L"NamesapceDefineRule")[L"define"]
					 )
					
					)
					.Create(ztl::general_parser::Normal(L"NamespaceMember"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"NamesapceDefineRule")
							.ReturnType(ztl::general_parser::Normal(L"AstNamesapceDefine"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"NAMESPACE")+
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					+ztl::general_parser::GrammarSymbol(L"OPEN")+*(
						ztl::general_parser::GrammarSymbol(L"NamespaceMemberRule")[L"members"]
					)+ztl::general_parser::GrammarSymbol(L"CLOSE")
					)
					.Create(ztl::general_parser::Normal(L"AstNamesapceDefine"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"UsingDeclStatementRule")
							.ReturnType(ztl::general_parser::Normal(L"AstUsingDeclExpression"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"USING")+~(
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					+ztl::general_parser::GrammarSymbol(L"ASSIGN"))+
						ztl::general_parser::GrammarSymbol(L"IdentifierObjectRule")[L"module"]
					+ztl::general_parser::GrammarSymbol(L"SEMICOLON")
					)
					.Create(ztl::general_parser::Normal(L"AstUsingDeclExpression"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"ParserDecl")
							.ReturnType(ztl::general_parser::Normal(L"ParserASTRoot"))
							
						|
					(
						*(
						ztl::general_parser::GrammarSymbol(L"UsingDeclStatementRule")[L"usings"]
					)+*(
						ztl::general_parser::GrammarSymbol(L"NamespaceMemberRule")[L"members"]
					)
					)
					.Create(ztl::general_parser::Normal(L"ParserASTRoot"))
					
				
							
						)
				
						);
					return writer.table;;
				}
			
							}
						 
							}
						 