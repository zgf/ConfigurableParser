#include "Include/stdafx.h"

#include "GenerateParser.h"
					#include "..\..\Include\GeneralTableWriter.h"

				
							namespace ztl
							{
								
							namespace general_parser
							{
								
			int WstringToEnumItem(const wstring& value)
			{
				static unordered_map<wstring, int> signMap =
				{
					
				{
					L"GeneralTokenDefine::TokenOptional::False",
					static_cast<int>(GeneralTokenDefine::TokenOptional::False)
				},
				{
					L"GeneralTokenDefine::TokenOptional::True",
					static_cast<int>(GeneralTokenDefine::TokenOptional::True)
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
								L"GeneralTableDefine",
								{
									{
										L"heads",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralTableDefine>(classObject)->heads.emplace_back(std::static_pointer_cast<GeneralHeadInfoDefine>(valueObject));
											return;
										}
									},{
										L"types",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralTableDefine>(classObject)->types.emplace_back(std::static_pointer_cast<GeneralTypeDefine>(valueObject));
											return;
										}
									},{
										L"tokens",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralTableDefine>(classObject)->tokens.emplace_back(std::static_pointer_cast<GeneralTokenDefine>(valueObject));
											return;
										}
									},{
										L"rules",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralTableDefine>(classObject)->rules.emplace_back(std::static_pointer_cast<GeneralRuleDefine>(valueObject));
											return;
										}
									},
								}
							},{
								L"GeneralClassMemberTypeDefine",
								{
									{
										L"type",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralClassMemberTypeDefine>(classObject)->type = std::static_pointer_cast<GeneralTypeObject>(valueObject);
											return;
										}
									},{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralClassMemberTypeDefine>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"GeneralGrammarSetterTypeDefine",
								{
									{
										L"grammar",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralGrammarSetterTypeDefine>(classObject)->grammar = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
											return;
										}
									},{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralGrammarSetterTypeDefine>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},{
										L"value",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralGrammarSetterTypeDefine>(classObject)->value=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"GeneralHeadInfoDefine",
								{
									{
										L"property",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralHeadInfoDefine>(classObject)->property=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},{
										L"value",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralHeadInfoDefine>(classObject)->value=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"GeneralGrammarLoopTypeDefine",
								{
									{
										L"grammar",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralGrammarLoopTypeDefine>(classObject)->grammar = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
											return;
										}
									},
								}
							},{
								L"GeneralNormalTypeObject",
								{
									{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralNormalTypeObject>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"GeneralTokenTypeObject",
								{
									
								}
							},{
								L"GeneralGrammarCreateTypeDefine",
								{
									{
										L"type",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralGrammarCreateTypeDefine>(classObject)->type = std::static_pointer_cast<GeneralTypeObject>(valueObject);
											return;
										}
									},{
										L"grammar",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralGrammarCreateTypeDefine>(classObject)->grammar = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
											return;
										}
									},
								}
							},{
								L"GeneralEnumMemberTypeDefine",
								{
									{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralEnumMemberTypeDefine>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"GeneralTokenDefine",
								{
									{
										L"regex",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralTokenDefine>(classObject)->regex=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralTokenDefine>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},{
										L"ignore",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralTokenDefine>(classObject)->ignore=static_cast<GeneralTokenDefine::TokenOptional>(WstringToEnumItem(L"GeneralTokenDefine::TokenOptional::" + std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content));
											return;
										}
									},
								}
							},{
								L"GeneralGrammarSequenceTypeDefine",
								{
									{
										L"first",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralGrammarSequenceTypeDefine>(classObject)->first = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
											return;
										}
									},{
										L"second",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralGrammarSequenceTypeDefine>(classObject)->second = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
											return;
										}
									},
								}
							},{
								L"GeneralTypeDefine",
								{
									
								}
							},{
								L"GeneralTypeObject",
								{
									
								}
							},{
								L"GeneralArrayTypeObject",
								{
									{
										L"element",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralArrayTypeObject>(classObject)->element = std::static_pointer_cast<GeneralTypeObject>(valueObject);
											return;
										}
									},
								}
							},{
								L"GeneralSubTypeObject",
								{
									{
										L"parent",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralSubTypeObject>(classObject)->parent = std::static_pointer_cast<GeneralTypeObject>(valueObject);
											return;
										}
									},{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralSubTypeObject>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"GeneralClassTypeDefine",
								{
									{
										L"subTypes",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralClassTypeDefine>(classObject)->subTypes.emplace_back(std::static_pointer_cast<GeneralTypeDefine>(valueObject));
											return;
										}
									},{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralClassTypeDefine>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},{
										L"members",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralClassTypeDefine>(classObject)->members.emplace_back(std::static_pointer_cast<GeneralClassMemberTypeDefine>(valueObject));
											return;
										}
									},{
										L"parent",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralClassTypeDefine>(classObject)->parent = std::static_pointer_cast<GeneralTypeObject>(valueObject);
											return;
										}
									},
								}
							},{
								L"GeneralEnumTypeDefine",
								{
									{
										L"members",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralEnumTypeDefine>(classObject)->members.emplace_back(std::static_pointer_cast<GeneralEnumMemberTypeDefine>(valueObject));
											return;
										}
									},{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralEnumTypeDefine>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"GeneralGrammarTypeDefine",
								{
									
								}
							},{
								L"GeneralGrammarTextTypeDefine",
								{
									{
										L"text",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralGrammarTextTypeDefine>(classObject)->text=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"GeneralGrammarNormalTypeDefine",
								{
									{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralGrammarNormalTypeDefine>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"GeneralGrammarOptionalTypeDefine",
								{
									{
										L"grammar",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralGrammarOptionalTypeDefine>(classObject)->grammar = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
											return;
										}
									},
								}
							},{
								L"GeneralGrammarAssignTypeDefine",
								{
									{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralGrammarAssignTypeDefine>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},{
										L"grammar",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralGrammarAssignTypeDefine>(classObject)->grammar = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
											return;
										}
									},
								}
							},{
								L"GeneralGrammarUsingTypeDefine",
								{
									{
										L"grammar",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralGrammarUsingTypeDefine>(classObject)->grammar = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
											return;
										}
									},
								}
							},{
								L"GeneralGrammarAlternationTypeDefine",
								{
									{
										L"left",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralGrammarAlternationTypeDefine>(classObject)->left = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
											return;
										}
									},{
										L"right",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralGrammarAlternationTypeDefine>(classObject)->right = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
											return;
										}
									},
								}
							},{
								L"GeneralRuleDefine",
								{
									{
										L"type",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralRuleDefine>(classObject)->type = std::static_pointer_cast<GeneralTypeObject>(valueObject);
											return;
										}
									},{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralRuleDefine>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},{
										L"grammars",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<GeneralRuleDefine>(classObject)->grammars.emplace_back(std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject));
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
					L"GeneralTableDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralTableDefine>();
					}
				},
				{
					L"GeneralClassMemberTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralClassMemberTypeDefine>();
					}
				},
				{
					L"GeneralGrammarSetterTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarSetterTypeDefine>();
					}
				},
				{
					L"GeneralHeadInfoDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralHeadInfoDefine>();
					}
				},
				{
					L"GeneralGrammarLoopTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarLoopTypeDefine>();
					}
				},
				{
					L"GeneralNormalTypeObject",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralNormalTypeObject>();
					}
				},
				{
					L"GeneralTokenTypeObject",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralTokenTypeObject>();
					}
				},
				{
					L"GeneralGrammarCreateTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarCreateTypeDefine>();
					}
				},
				{
					L"GeneralEnumMemberTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralEnumMemberTypeDefine>();
					}
				},
				{
					L"GeneralTokenDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralTokenDefine>();
					}
				},
				{
					L"GeneralGrammarSequenceTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarSequenceTypeDefine>();
					}
				},
				{
					L"GeneralTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralTypeDefine>();
					}
				},
				{
					L"GeneralTypeObject",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralTypeObject>();
					}
				},
				{
					L"GeneralArrayTypeObject",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralArrayTypeObject>();
					}
				},
				{
					L"GeneralSubTypeObject",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralSubTypeObject>();
					}
				},
				{
					L"GeneralClassTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralClassTypeDefine>();
					}
				},
				{
					L"GeneralEnumTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralEnumTypeDefine>();
					}
				},
				{
					L"GeneralGrammarTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarTypeDefine>();
					}
				},
				{
					L"GeneralGrammarTextTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarTextTypeDefine>();
					}
				},
				{
					L"GeneralGrammarNormalTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarNormalTypeDefine>();
					}
				},
				{
					L"GeneralGrammarOptionalTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarOptionalTypeDefine>();
					}
				},
				{
					L"GeneralGrammarAssignTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarAssignTypeDefine>();
					}
				},
				{
					L"GeneralGrammarUsingTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarUsingTypeDefine>();
					}
				},
				{
					L"GeneralGrammarAlternationTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarAlternationTypeDefine>();
					}
				},
				{
					L"GeneralRuleDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralRuleDefine>();
					}
				},
				};
				assert(objectMap.find(name) != objectMap.end());
				return objectMap[name]();
			}
				void GeneralHeterogeneousParserTree(ztl::general_parser::GeneralParserBase& parser, ztl::general_parser::GeneralTreeNode* classNode, shared_ptr<void>& classObject)
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
							ztl::general_parser::GeneralHeterogeneousParserTree(parser,fieldNode, fieldObject);
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
	
				shared_ptr<void> GeneralHeterogeneousParserTree(ztl::general_parser::GeneralParserBase& parser,ztl::general_parser::GeneralTreeNode* root)
				{
					assert(root != nullptr);
					auto rootObject = ReflecteObjectByName(root->GetName());
					parser.SaveHeterogeneousNode(rootObject);
					ztl::general_parser::GeneralHeterogeneousParserTree(parser,root, rootObject);
					return rootObject;
				}
				shared_ptr<void>	GeneralHeterogeneousParserTree(ztl::general_parser::GeneralParserBase& parser)
				{
					return ztl::general_parser::GeneralHeterogeneousParserTree(parser, parser.GetGeneralTreeRoot());
				}
				
				shared_ptr<ztl::general_parser::GeneralTableDefine> BootStrapDefineTable()
				{
					ztl::general_parser::GeneralTableWriter writer;
					writer.Head
						(
							ztl::general_parser::GeneralHeadInfoWriter()
							
					.Info(LR"(filename)",LR"(GenerateParser)")
				
					.Info(LR"(include)",LR"(Include/stdafx.h)")
				
					.Info(LR"(namespace)",LR"(ztl)")
				
					.Info(LR"(namespace)",LR"(general_parser)")
				
					.Info(LR"(dirname)",LR"(General)")
				
						)
						.Token
						(
							ztl::general_parser::GeneralTokenWriter()
													.Token(L"FINISH",L"<\\$>")
			.Token(L"CLASS",LR"(class)")
.Token(L"STRUCT",LR"(struct)")
.Token(L"ENUM",LR"(enum)")
.Token(L"TOKEN",LR"(token)")
.Token(L"DISCARDTOKEN",LR"(ignoretoken)")
.Token(L"RULE",LR"(rule)")
.Token(L"AS",LR"(as)")
.Token(L"WITH",LR"(with)")
.Token(L"OPEN",LR"(\{)")
.Token(L"CLOSE",LR"(\})")
.Token(L"SEMICOLON",LR"(;)")
.Token(L"COLON",LR"(:)")
.Token(L"COMMA",LR"(,)")
.Token(L"DOT",LR"(\.)")
.Token(L"ASSIGN",LR"(=)")
.Token(L"USING",LR"(!)")
.Token(L"OR",LR"(\|)")
.Token(L"OPTOPEN",LR"(\[)")
.Token(L"OPTCLOSE",LR"(\])")
.Token(L"PREOPEN",LR"(\()")
.Token(L"PRECLOSE",LR"(\))")
.Token(L"NAME",LR"([a-zA-Z_]\w*)")
.Token(L"STRING",LR"("(\\\\|\\"|[^"])*")")
.IgnoreToken(L"SPACE",LR"(\s+)")
.IgnoreToken(L"LINENOTE",LR"(//[^\n]*\n)")
.IgnoreToken(L"BLOCKNOTE",LR"(/\*.*?\*/)")

						)
						.Type
						(
							
					ztl::general_parser::GeneralTypeListWriter()
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralHeadInfoDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"property"))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"value"))
				
							
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralTokenDefine")
							
							.SubType(
						
					ztl::general_parser::GeneralTypeListWriter()
					
						.Enum
						(
							ztl::general_parser::GeneralEnumTypeWriter()
							.Name(L"TokenOptional")
							
						.Member(ztl::general_parser::EnumMember(L"False"))
					
						.Member(ztl::general_parser::EnumMember(L"True"))
					
						)
					
				
									)
						
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name"))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"regex"))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"TokenOptional"), L"ignore"))
				
							
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralTypeObject")
							
							
							
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralArrayTypeObject")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralTypeObject"), L"element"))
				
							.ParentType(ztl::general_parser::Normal(L"GeneralTypeObject"))
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralTokenTypeObject")
							
							
							.ParentType(ztl::general_parser::Normal(L"GeneralTypeObject"))
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralNormalTypeObject")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name"))
				
							.ParentType(ztl::general_parser::Normal(L"GeneralTypeObject"))
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralSubTypeObject")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralTypeObject"), L"parent"))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name"))
				
							.ParentType(ztl::general_parser::Normal(L"GeneralTypeObject"))
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralTypeDefine")
							
							
							
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralClassMemberTypeDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralTypeObject"), L"type"))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name"))
				
							.ParentType(ztl::general_parser::Normal(L"GeneralTypeDefine"))
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralEnumMemberTypeDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name"))
				
							.ParentType(ztl::general_parser::Normal(L"GeneralTypeDefine"))
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralClassTypeDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"GeneralTypeDefine")), L"subTypes"))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"GeneralClassMemberTypeDefine")), L"members"))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralTypeObject"), L"parent"))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name"))
				
							.ParentType(ztl::general_parser::Normal(L"GeneralTypeDefine"))
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralEnumTypeDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"GeneralEnumMemberTypeDefine")), L"members"))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name"))
				
							.ParentType(ztl::general_parser::Normal(L"GeneralTypeDefine"))
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralGrammarTypeDefine")
							
							
							
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralGrammarTextTypeDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"text"))
				
							.ParentType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralGrammarNormalTypeDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name"))
				
							.ParentType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralGrammarSequenceTypeDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"), L"first"))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"), L"second"))
				
							.ParentType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralGrammarLoopTypeDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"), L"grammar"))
				
							.ParentType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralGrammarOptionalTypeDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"), L"grammar"))
				
							.ParentType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralGrammarAssignTypeDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name"))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"), L"grammar"))
				
							.ParentType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralGrammarSetterTypeDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"), L"grammar"))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name"))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"value"))
				
							.ParentType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralGrammarUsingTypeDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"), L"grammar"))
				
							.ParentType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralGrammarCreateTypeDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralTypeObject"), L"type"))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"), L"grammar"))
				
							.ParentType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralGrammarAlternationTypeDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"), L"left"))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"), L"right"))
				
							.ParentType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralRuleDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralTypeObject"), L"type"))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name"))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine")), L"grammars"))
				
							
						)
					
						.Class
						(
							ztl::general_parser::GeneralClassTypeWriter()
							.Name(L"GeneralTableDefine")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"GeneralHeadInfoDefine")), L"heads"))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"GeneralTypeDefine")), L"types"))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"GeneralTokenDefine")), L"tokens"))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"GeneralRuleDefine")), L"rules"))
				
							
						)
					
				
						)
						.Rules
						(
							ztl::general_parser::GeneralRuleListWriter()
							
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"TypeDecl")
							.ReturnType(ztl::general_parser::Normal(L"GeneralTypeDefine"))
							
						|
						( !(ztl::general_parser::GrammarSymbol(L"Enum")) | !(ztl::general_parser::GrammarSymbol(L"Class")) )
					
				
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"Type")
							.ReturnType(ztl::general_parser::Normal(L"GeneralTypeObject"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					
					)
					.Create(ztl::general_parser::Normal(L"GeneralNormalTypeObject"))
					
				
						|
					(
						ztl::general_parser::GrammarSymbol(L"TOKEN")
					)
					.Create(ztl::general_parser::Normal(L"GeneralTokenTypeObject"))
					
				
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"Type")[L"parent"]
					+ztl::general_parser::GrammarSymbol(L"DOT")+
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					
					)
					.Create(ztl::general_parser::Normal(L"GeneralSubTypeObject"))
					
				
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"Type")[L"element"]
					+ztl::general_parser::GrammarSymbol(L"OPTOPEN")+ztl::general_parser::GrammarSymbol(L"OPTCLOSE")
					)
					.Create(ztl::general_parser::Normal(L"GeneralArrayTypeObject"))
					
				
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"EnumMember")
							.ReturnType(ztl::general_parser::Normal(L"GeneralEnumMemberTypeDefine"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					+ztl::general_parser::GrammarSymbol(L"COMMA")
					)
					.Create(ztl::general_parser::Normal(L"GeneralEnumMemberTypeDefine"))
					
				
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"Enum")
							.ReturnType(ztl::general_parser::Normal(L"GeneralEnumTypeDefine"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"ENUM")+
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					+ztl::general_parser::GrammarSymbol(L"OPEN")+*(
						ztl::general_parser::GrammarSymbol(L"EnumMember")[L"members"]
					)+ztl::general_parser::GrammarSymbol(L"CLOSE")+ztl::general_parser::GrammarSymbol(L"SEMICOLON")
					)
					.Create(ztl::general_parser::Normal(L"GeneralEnumTypeDefine"))
					
				
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"ClassMember")
							.ReturnType(ztl::general_parser::Normal(L"GeneralClassMemberTypeDefine"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"Type")[L"type"]
					+
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					+ztl::general_parser::GrammarSymbol(L"SEMICOLON")
					)
					.Create(ztl::general_parser::Normal(L"GeneralClassMemberTypeDefine"))
					
				
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"Class")
							.ReturnType(ztl::general_parser::Normal(L"GeneralClassTypeDefine"))
							
						|
					(
						
						( ztl::general_parser::GrammarSymbol(L"STRUCT") | ztl::general_parser::GrammarSymbol(L"CLASS") )
					+
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					+~(ztl::general_parser::GrammarSymbol(L"COLON")+
						ztl::general_parser::GrammarSymbol(L"Type")[L"parent"]
					)+ztl::general_parser::GrammarSymbol(L"OPEN")+*(
						( 
						ztl::general_parser::GrammarSymbol(L"ClassMember")[L"members"]
					 | 
						ztl::general_parser::GrammarSymbol(L"TypeDecl")[L"subTypes"]
					 )
					)+ztl::general_parser::GrammarSymbol(L"CLOSE")+ztl::general_parser::GrammarSymbol(L"SEMICOLON")
					)
					.Create(ztl::general_parser::Normal(L"GeneralClassTypeDefine"))
					
				
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"SequenceGrammar")
							.ReturnType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
							
						|!(ztl::general_parser::GrammarSymbol(L"PrimitiveGrammar"))
				
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"PrimitiveGrammar")[L"first"]
					+
						ztl::general_parser::GrammarSymbol(L"SequenceGrammar")[L"second"]
					
					)
					.Create(ztl::general_parser::Normal(L"GeneralGrammarSequenceTypeDefine"))
					
				
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"AlternativeGrammar")
							.ReturnType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
							
						|!(ztl::general_parser::GrammarSymbol(L"SequenceGrammar"))
				
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"SequenceGrammar")[L"left"]
					+ztl::general_parser::GrammarSymbol(L"OR")+
						ztl::general_parser::GrammarSymbol(L"AlternativeGrammar")[L"right"]
					
					)
					.Create(ztl::general_parser::Normal(L"GeneralGrammarAlternationTypeDefine"))
					
				
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"Grammar")
							.ReturnType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
							
						|!(ztl::general_parser::GrammarSymbol(L"AlternativeGrammar"))
				
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"Grammar")[L"grammar"]
					+ztl::general_parser::GrammarSymbol(L"AS")+
						ztl::general_parser::GrammarSymbol(L"Type")[L"type"]
					
					)
					.Create(ztl::general_parser::Normal(L"GeneralGrammarCreateTypeDefine"))
					
				
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"Grammar")[L"grammar"]
					+ztl::general_parser::GrammarSymbol(L"WITH")+ztl::general_parser::GrammarSymbol(L"OPEN")+
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					+ztl::general_parser::GrammarSymbol(L"ASSIGN")+
						ztl::general_parser::GrammarSymbol(L"STRING")[L"value"]
					+ztl::general_parser::GrammarSymbol(L"CLOSE")
					)
					.Create(ztl::general_parser::Normal(L"GeneralGrammarSetterTypeDefine"))
					
				
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"PrimitiveGrammar")
							.ReturnType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					
					)
					.Create(ztl::general_parser::Normal(L"GeneralGrammarNormalTypeDefine"))
					
				
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"STRING")[L"text"]
					
					)
					.Create(ztl::general_parser::Normal(L"GeneralGrammarTextTypeDefine"))
					
				
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"PrimitiveGrammar")[L"grammar"]
					+ztl::general_parser::GrammarSymbol(L"COLON")+
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					
					)
					.Create(ztl::general_parser::Normal(L"GeneralGrammarAssignTypeDefine"))
					
				
						|
					(
						ztl::general_parser::GrammarSymbol(L"USING")+
						ztl::general_parser::GrammarSymbol(L"PrimitiveGrammar")[L"grammar"]
					
					)
					.Create(ztl::general_parser::Normal(L"GeneralGrammarUsingTypeDefine"))
					
				
						|
					(
						ztl::general_parser::GrammarSymbol(L"OPTOPEN")+
						ztl::general_parser::GrammarSymbol(L"Grammar")[L"grammar"]
					+ztl::general_parser::GrammarSymbol(L"OPTCLOSE")
					)
					.Create(ztl::general_parser::Normal(L"GeneralGrammarOptionalTypeDefine"))
					
				
						|
					(
						ztl::general_parser::GrammarSymbol(L"OPEN")+
						ztl::general_parser::GrammarSymbol(L"Grammar")[L"grammar"]
					+ztl::general_parser::GrammarSymbol(L"CLOSE")
					)
					.Create(ztl::general_parser::Normal(L"GeneralGrammarLoopTypeDefine"))
					
				
						|ztl::general_parser::GrammarSymbol(L"PREOPEN")+!(ztl::general_parser::GrammarSymbol(L"Grammar"))+ztl::general_parser::GrammarSymbol(L"PRECLOSE")
				
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"TokenDecl")
							.ReturnType(ztl::general_parser::Normal(L"GeneralTokenDefine"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"TOKEN")+
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					+ztl::general_parser::GrammarSymbol(L"ASSIGN")+
						ztl::general_parser::GrammarSymbol(L"STRING")[L"regex"]
					+ztl::general_parser::GrammarSymbol(L"SEMICOLON")
					)
					.Create(ztl::general_parser::Normal(L"GeneralTokenDefine"))
					
						.Setter(L"ignore", L"False")
					
				
						|
					(
						ztl::general_parser::GrammarSymbol(L"DISCARDTOKEN")+
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					+ztl::general_parser::GrammarSymbol(L"ASSIGN")+
						ztl::general_parser::GrammarSymbol(L"STRING")[L"regex"]
					+ztl::general_parser::GrammarSymbol(L"SEMICOLON")
					)
					.Create(ztl::general_parser::Normal(L"GeneralTokenDefine"))
					
						.Setter(L"ignore", L"True")
					
				
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"HeadDecl")
							.ReturnType(ztl::general_parser::Normal(L"GeneralHeadInfoDefine"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"NAME")[L"property"]
					+ztl::general_parser::GrammarSymbol(L"COLON")+
						ztl::general_parser::GrammarSymbol(L"STRING")[L"value"]
					+ztl::general_parser::GrammarSymbol(L"SEMICOLON")
					)
					.Create(ztl::general_parser::Normal(L"GeneralHeadInfoDefine"))
					
				
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"RuleDecl")
							.ReturnType(ztl::general_parser::Normal(L"GeneralRuleDefine"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"RULE")+
						ztl::general_parser::GrammarSymbol(L"Type")[L"type"]
					+
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					+*(ztl::general_parser::GrammarSymbol(L"ASSIGN")+
						ztl::general_parser::GrammarSymbol(L"Grammar")[L"grammars"]
					)+ztl::general_parser::GrammarSymbol(L"SEMICOLON")
					)
					.Create(ztl::general_parser::Normal(L"GeneralRuleDefine"))
					
				
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"ParserDecl")
							.ReturnType(ztl::general_parser::Normal(L"GeneralTableDefine"))
							
						|
					(
						
						( *(
						ztl::general_parser::GrammarSymbol(L"HeadDecl")[L"heads"]
					) | 
						( *(
						ztl::general_parser::GrammarSymbol(L"TypeDecl")[L"types"]
					) | 
						( *(
						ztl::general_parser::GrammarSymbol(L"TokenDecl")[L"tokens"]
					) | *(
						ztl::general_parser::GrammarSymbol(L"RuleDecl")[L"rules"]
					) )
					 )
					 )
					
					)
					.Create(ztl::general_parser::Normal(L"GeneralTableDefine"))
					
				
						)
				
						);
					return writer.table;;
				}
			
							}
						 
							}
						 