#include "Include/stdafx.h"

#include "XmlParser.h"
					#include "..\..\Include\GeneralTableWriter.h"

				
							namespace ztl
							{
								
							namespace xml
							{
								
			int WstringToEnumItem(const wstring& value)
			{
				static unordered_map<wstring, int> signMap =
				{
					
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
								L"Instruction",
								{
									{
										L"attributes",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<Instruction>(classObject)->attributes.emplace_back(std::static_pointer_cast<Attribute>(valueObject));
											return;
										}
									},{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<Instruction>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"Node",
								{
									
								}
							},{
								L"Text",
								{
									{
										L"content",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<Text>(classObject)->content=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"Comment",
								{
									{
										L"content",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<Comment>(classObject)->content=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"CData",
								{
									{
										L"content",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<CData>(classObject)->content=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"Attribute",
								{
									{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<Attribute>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},{
										L"value",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<Attribute>(classObject)->value=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"Element",
								{
									{
										L"subNodes",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<Element>(classObject)->subNodes.emplace_back(std::static_pointer_cast<Node>(valueObject));
											return;
										}
									},{
										L"attributes",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<Element>(classObject)->attributes.emplace_back(std::static_pointer_cast<Attribute>(valueObject));
											return;
										}
									},{
										L"name",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<Element>(classObject)->name=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},{
										L"closingName",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<Element>(classObject)->closingName=std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"Document",
								{
									{
										L"prologs",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<Document>(classObject)->prologs.emplace_back(std::static_pointer_cast<Node>(valueObject));
											return;
										}
									},{
										L"rootElement",
										[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
										{
											std::static_pointer_cast<Document>(classObject)->rootElement = std::static_pointer_cast<Element>(valueObject);
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
					L"Instruction",
					[]()->shared_ptr<void>
					{
						return make_shared<Instruction>();
					}
				},
				{
					L"Node",
					[]()->shared_ptr<void>
					{
						return make_shared<Node>();
					}
				},
				{
					L"Text",
					[]()->shared_ptr<void>
					{
						return make_shared<Text>();
					}
				},
				{
					L"Comment",
					[]()->shared_ptr<void>
					{
						return make_shared<Comment>();
					}
				},
				{
					L"CData",
					[]()->shared_ptr<void>
					{
						return make_shared<CData>();
					}
				},
				{
					L"Attribute",
					[]()->shared_ptr<void>
					{
						return make_shared<Attribute>();
					}
				},
				{
					L"Element",
					[]()->shared_ptr<void>
					{
						return make_shared<Element>();
					}
				},
				{
					L"Document",
					[]()->shared_ptr<void>
					{
						return make_shared<Document>();
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
							ztl::xml::GenerateHeterogeneousParserTree(parser,fieldNode, fieldObject);
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
					ztl::xml::GenerateHeterogeneousParserTree(parser,root, rootObject);
					return rootObject;
				}
				shared_ptr<void>	GenerateHeterogeneousParserTree(ztl::general_parser::GeneralLRExecutor& parser)
				{
					return ztl::xml::GenerateHeterogeneousParserTree(parser, parser.GetGeneralTreeRoot());
				}
				
				shared_ptr<ztl::general_parser::GeneralTableDefine> BootStrapDefineTable()
				{
					ztl::general_parser::GeneralTableWriter writer;
					writer.Head
						(
							ztl::general_parser::GeneralHeadInfoWriter()
							
					.Info(LR"(filename)",LR"(XmlParser,
				ztl::general_parser::GeneralAttributeWriter()
				
				)")
				
					.Info(LR"(include)",LR"(Include/stdafx.h,
				ztl::general_parser::GeneralAttributeWriter()
				
				)")
				
					.Info(LR"(namespace)",LR"(ztl,
				ztl::general_parser::GeneralAttributeWriter()
				
				)")
				
					.Info(LR"(namespace)",LR"(xml,
				ztl::general_parser::GeneralAttributeWriter()
				
				)")
				
					.Info(LR"(dirname)",LR"(xml,
				ztl::general_parser::GeneralAttributeWriter()
				
				)")
				
						)
						.Token
						(
							ztl::general_parser::GeneralTokenWriter()
													.Token(L"FINISH",L"<\\$>")
			.Token(L"INSTRUCTIONOPEN",LR"(<\?)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"INSTRUCTIONCLOSE",LR"(\?>)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"COMPLEXELEMENTOPEN",LR"(</)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"SINGLEELEMENTCLOSE",LR"(/>)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"ELEMENTOPEN",LR"(<)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"ELEMENTCLOSE",LR"(>)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"EQUAL",LR"(=)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"NAME",LR"([a-zA-Z0-9:._\-]+)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"ATTVALUE",LR"("[^<>"]*"|'[^<>']*')",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"COMMENT",LR"(<!--([^\->]|-[^\->]|--[^>])*-->)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"CDATA",LR"(<!\[CDATA\[([^\]]|\][^\]]|\]\][^>])*\]\]>)",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.Token(L"TEXT",LR"(([^<>="' \r\n\ta-zA-Z0-9:._\-])+|"|')",
				ztl::general_parser::GeneralAttributeWriter()
				
				)
.IgnoreToken(L"SPACE",LR"(\s+)",
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
							.Name(L"Text")
							
							
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
							.Name(L"CData")
							
							
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
							.Name(L"Attribute")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"value",
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
							.Name(L"Comment")
							
							
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
							.Name(L"Element")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"closingName",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"Attribute")), L"attributes",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"Node")), L"subNodes",
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
							.Name(L"Instruction")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"Attribute")), L"attributes",
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
							.Name(L"Document")
							
							
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"Node")), L"prologs",
				ztl::general_parser::GeneralAttributeWriter()
				
				))
				
					.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"Element"), L"rootElement",
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
							.Name(L"XAttribute")
							.ReturnType(ztl::general_parser::Normal(L"Attribute"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					+ztl::general_parser::GrammarSymbol(L"EQUAL")+
						ztl::general_parser::GrammarSymbol(L"ATTVALUE")[L"value"]
					
					)
					.Create(ztl::general_parser::Normal(L"Attribute"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"XText")
							.ReturnType(ztl::general_parser::Normal(L"Text"))
							
						|
					(
						
						( 
						ztl::general_parser::GrammarSymbol(L"NAME")[L"content"]
					 | 
						( 
						ztl::general_parser::GrammarSymbol(L"EQUAL")[L"content"]
					 | 
						( 
						ztl::general_parser::GrammarSymbol(L"ATTVALUE")[L"content"]
					 | 
						ztl::general_parser::GrammarSymbol(L"TEXT")[L"content"]
					 )
					 )
					 )
					
					)
					.Create(ztl::general_parser::Normal(L"Text"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"XCData")
							.ReturnType(ztl::general_parser::Normal(L"CData"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"CDATA")[L"content"]
					
					)
					.Create(ztl::general_parser::Normal(L"CData"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"XComment")
							.ReturnType(ztl::general_parser::Normal(L"Comment"))
							
						|
					(
						
						ztl::general_parser::GrammarSymbol(L"COMMENT")[L"content"]
					
					)
					.Create(ztl::general_parser::Normal(L"Comment"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"XElement")
							.ReturnType(ztl::general_parser::Normal(L"Element"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"ELEMENTOPEN")+
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					+*(
						ztl::general_parser::GrammarSymbol(L"XAttribute")[L"attributes"]
					)+ztl::general_parser::GrammarSymbol(L"SINGLEELEMENTCLOSE")
					)
					.Create(ztl::general_parser::Normal(L"Element"))
					
				
						|
					(
						ztl::general_parser::GrammarSymbol(L"ELEMENTOPEN")+
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					+*(
						ztl::general_parser::GrammarSymbol(L"XAttribute")[L"attributes"]
					)+ztl::general_parser::GrammarSymbol(L"ELEMENTCLOSE")+*(
						ztl::general_parser::GrammarSymbol(L"XSubNode")[L"subNodes"]
					)+ztl::general_parser::GrammarSymbol(L"COMPLEXELEMENTOPEN")+
						ztl::general_parser::GrammarSymbol(L"NAME")[L"closingName"]
					+ztl::general_parser::GrammarSymbol(L"ELEMENTCLOSE")
					)
					.Create(ztl::general_parser::Normal(L"Element"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"XSubNode")
							.ReturnType(ztl::general_parser::Normal(L"Node"))
							
						|
						( !(ztl::general_parser::GrammarSymbol(L"XText")) | 
						( !(ztl::general_parser::GrammarSymbol(L"XCData")) | 
						( !(ztl::general_parser::GrammarSymbol(L"XComment")) | !(ztl::general_parser::GrammarSymbol(L"XElement")) )
					 )
					 )
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"XInstruction")
							.ReturnType(ztl::general_parser::Normal(L"Instruction"))
							
						|
					(
						ztl::general_parser::GrammarSymbol(L"INSTRUCTIONOPEN")+
						ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
					+*(
						ztl::general_parser::GrammarSymbol(L"XAttribute")[L"attributes"]
					)+ztl::general_parser::GrammarSymbol(L"INSTRUCTIONCLOSE")
					)
					.Create(ztl::general_parser::Normal(L"Instruction"))
					
				
							
						)
				
						.Rule
						(
							ztl::general_parser::GeneralRuleWriter()
							.Name(L"XDocument")
							.ReturnType(ztl::general_parser::Normal(L"Document"))
							
						|
					(
						*(
						( 
						ztl::general_parser::GrammarSymbol(L"XInstruction")[L"prologs"]
					 | 
						ztl::general_parser::GrammarSymbol(L"XComment")[L"prologs"]
					 )
					)+
						ztl::general_parser::GrammarSymbol(L"XElement")[L"rootElement"]
					
					)
					.Create(ztl::general_parser::Normal(L"Document"))
					
				
							
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
								class IRCodeGen<Node> : public Node::IVisitor
								{
								public:
									virtual void		Visit(Node* node) override
						{
				
						}
						virtual void		Visit(Document* node) override
						{
				
						}
						virtual void		Visit(Instruction* node) override
						{
				
						}
						virtual void		Visit(Element* node) override
						{
				
						}
						virtual void		Visit(Comment* node) override
						{
				
						}
						virtual void		Visit(Attribute* node) override
						{
				
						}
						virtual void		Visit(CData* node) override
						{
				
						}
						virtual void		Visit(Text* node) override
						{
				
						}
							
									IRCodeGenContext* context;
								};

						














							}
						 
							}
						 