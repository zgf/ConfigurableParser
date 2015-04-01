
				#include <memory>
				#include <string>
				#include <vector>
				#include <unordered_map>
				#include <iostream>
				using std::pair;
				using std::wstring;
				using std::shared_ptr;
				using std::vector;
				using std::unordered_map;
				using std::wifstream;
				#include "Source/Include/stdafx.h"


							namespace XML
							{
								
							namespace test
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
					virtual void		Visit(Object* node) =0;
virtual void		Visit(ObjectField* node) =0;
virtual void		Visit(Array* node) =0;
virtual void		Visit(Number* node) =0;
virtual void		Visit(String* node) =0;
virtual void		Visit(Literal* node) =0;

				};
				virtual void									Accept(IVisitor* visitor)=0;
				
			};
			


			
			struct Literal : public Node
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
			


			
			struct String : public Node
			{
				
				wstring    content;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct Number : public Node
			{
				
				wstring    content;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct Array : public Node
			{
				
				vector<shared_ptr<Node>>    items;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct ObjectField : public Node
			{
				
				wstring    name;
shared_ptr<Node>    value;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct Object : public Node
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
				assert(signMap.find(value)!=signMap.end());
				return signMap[value];
			}
			void ReflectionBuidler(const wstring& className,const wstring& fieldName,void* classObject,void* valueObject)
			{
				using builderType = void(*)(void*,void*);
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
											static_cast<Number*>(classObject)->content=static_cast<TokenInfo*>(valueObject)->content;
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
											static_cast<Literal*>(classObject)->value=static_cast<Literal::Value>(WstringToEnumItem(L"Literal::Value::" + static_cast<TokenInfo*>(valueObject)->content));
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
											static_cast<String*>(classObject)->content=static_cast<TokenInfo*>(valueObject)->content;
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
											static_cast<ObjectField*>(classObject)->name=static_cast<TokenInfo*>(valueObject)->content;
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
				shared_ptr<GeneralTableDefine> BootStrapDefineTable()
				{
					GeneralTableWriter writer;
					writer.Head
						(
							GeneralHeadInfoWriter()
							
					.Info(L"filename",L"JsonParser")
					
						)
						.Token
						(
							GeneralTokenWriter()
							.Token(L"TRUEVALUE",LR"(true)")
.Token(L"FALSEVALUE",LR"(false)")
.Token(L"NULLVALUE",LR"(null)")
.Token(L"OBJOPEN",LR"(\{)")
.Token(L"OBJCLOSE",LR"(\})")
.Token(L"ARROPEN",LR"(\[)")
.Token(L"ARRCLOSE",LR"(\])")
.Token(L"COMMA",LR"(,)")
.Token(L"COLON",LR"(:)")
.Token(L"NUMBER",LR"([\-]?\d+(.\d+)?([eE][+\-]?\d+)?)")
.Token(L"STRING",LR"("(\\"|[^"])*")")
.IgnoreToken(L"SPACE",LR"(\s+)")

						)
						.Type
						(
							
					GeneralTypeListWriter()
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"Node")
							
							
							
						)
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"Literal")
							
							.SubType(
						
					GeneralTypeListWriter()
					
						.Enum
						(
							GeneralEnumTypeWriter()
							.Name(L"Value")
							
						.Member(EnumMember(L"True"))
					
						.Member(EnumMember(L"False"))
					
						.Member(EnumMember(L"Null"))
					
						)
					
				
									)
						
							
					.Member(ClassMember(Normal(L"Value"), L"value"))
				
							.ParentType(Normal(L"Node"))
						)
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"String")
							
							
					.Member(ClassMember(String(), L"content"))
				
							.ParentType(Normal(L"Node"))
						)
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"Number")
							
							
					.Member(ClassMember(String(), L"content"))
				
							.ParentType(Normal(L"Node"))
						)
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"Array")
							
							
					.Member(ClassMember(Array(Normal(L"Node")), L"items"))
				
							.ParentType(Normal(L"Node"))
						)
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"ObjectField")
							
							
					.Member(ClassMember(String(), L"name"))
				
					.Member(ClassMember(Normal(L"Node"), L"value"))
				
							.ParentType(Normal(L"Node"))
						)
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"Object")
							
							
					.Member(ClassMember(Array(Normal(L"ObjectField")), L"fields"))
				
							.ParentType(Normal(L"Node"))
						)
					
				
						)
						.Rules
						(
							GeneralRuleListWriter()
							
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"JLiteral")
							.ReturnType(Normal(L"Node"))
							
						|
						GrammarSymbol(L"STRING")[L"content"]
					
					.Create(Normal(L"String"))
					
				
						|
						GrammarSymbol(L"NUMBER")[L"content"]
					
					.Create(Normal(L"Number"))
					
				
						|Text(L"TRUEVALUE")
					.Create(Normal(L"Literal"))
					
						.Setter(L"value", L"True")
					
				
						|Text(L"FALSEVALUE")
					.Create(Normal(L"Literal"))
					
						.Setter(L"value", L"False")
					
				
						|Text(L"NULLVALUE")
					.Create(Normal(L"Literal"))
					
						.Setter(L"value", L"Null")
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"JField")
							.ReturnType(Normal(L"ObjectField"))
							
						|
						GrammarSymbol(L"STRING")[L"name"]
					+Text(L"COLON")+
						GrammarSymbol(L"JValue")[L"value"]
					
					.Create(Normal(L"ObjectField"))
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"JObject")
							.ReturnType(Normal(L"Object"))
							
						|Text(L"OBJOPEN")+
						GrammarSymbol(L"JField")[L"fields"]
					+Text(L"COMMA")+~(*(
						GrammarSymbol(L"JField")[L"fields"]
					))+Text(L"OBJCLOSE")
					.Create(Normal(L"Object"))
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"JArray")
							.ReturnType(Normal(L"Array"))
							
						|Text(L"ARROPEN")+
						GrammarSymbol(L"JValue")[L"items"]
					+Text(L"COMMA")+~(*(
						GrammarSymbol(L"JValue")[L"items"]
					))+Text(L"ARRCLOSE")
					.Create(Normal(L"Array"))
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"JValue")
							.ReturnType(Normal(L"Node"))
							
						|!(GrammarSymbol(L"JLiteral"))
				
						|!(GrammarSymbol(L"JObject"))
				
						|!(GrammarSymbol(L"JArray"))
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"JRoot")
							.ReturnType(Normal(L"Node"))
							
						|!(GrammarSymbol(L"JObject"))
				
						|!(GrammarSymbol(L"JArray"))
				
						)
				
						);
					return writer.table;;
				}
			
							}
						 
							}
						 