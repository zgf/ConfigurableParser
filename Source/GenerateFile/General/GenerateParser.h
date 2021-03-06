
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
				#include "..\..\Include\GeneralLALRExecutor.h"
				
							namespace ztl
							{
								
							namespace general_parser
							{
								


			struct GeneralTypeDefine;
struct GeneralEnumTypeDefine;
struct GeneralClassTypeDefine;
struct GeneralEnumMemberTypeDefine;
struct GeneralClassMemberTypeDefine;

			struct GeneralTypeDefine 
			{
				
				
				
				
				class IVisitor
				{
				public:
					virtual void		Visit(GeneralTypeDefine* node) =0;
virtual void		Visit(GeneralEnumTypeDefine* node) =0;
virtual void		Visit(GeneralClassTypeDefine* node) =0;
virtual void		Visit(GeneralEnumMemberTypeDefine* node) =0;
virtual void		Visit(GeneralClassMemberTypeDefine* node) =0;

				};
				virtual void									Accept(IVisitor* )
				{

				}
				
			};
			


			
			struct GeneralAttributeArgumentDefine 
			{
				
				wstring    name;

				
				
			};
			


			
			struct GeneralAttributeDefine 
			{
				
				wstring    name;
vector<shared_ptr<GeneralAttributeArgumentDefine>>    arguments;

				
				
			};
			


			
			struct GeneralHeadInfoDefine 
			{
				
				wstring    property;
wstring    value;
vector<shared_ptr<GeneralAttributeDefine>>    attributes;

				
				
			};
			


			
			struct GeneralTokenDefine 
			{
				
					enum class TokenOptional
					{
						False,
True,

					};
					
				wstring    name;
wstring    regex;
TokenOptional    ignore;
vector<shared_ptr<GeneralAttributeDefine>>    attributes;

				
				
			};
			


			struct GeneralTypeObject;
struct GeneralSubTypeObject;
struct GeneralNormalTypeObject;
struct GeneralTokenTypeObject;
struct GeneralArrayTypeObject;

			struct GeneralTypeObject 
			{
				
				
				
				
				class IVisitor
				{
				public:
					virtual void		Visit(GeneralTypeObject* node) =0;
virtual void		Visit(GeneralSubTypeObject* node) =0;
virtual void		Visit(GeneralNormalTypeObject* node) =0;
virtual void		Visit(GeneralTokenTypeObject* node) =0;
virtual void		Visit(GeneralArrayTypeObject* node) =0;

				};
				virtual void									Accept(IVisitor* )
				{

				}
				
			};
			


			
			struct GeneralArrayTypeObject : public GeneralTypeObject
			{
				
				shared_ptr<GeneralTypeObject>    element;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct GeneralTokenTypeObject : public GeneralTypeObject
			{
				
				
				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct GeneralNormalTypeObject : public GeneralTypeObject
			{
				
				wstring    name;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct GeneralSubTypeObject : public GeneralTypeObject
			{
				
				shared_ptr<GeneralTypeObject>    parent;
wstring    name;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct GeneralClassMemberTypeDefine : public GeneralTypeDefine
			{
				
				shared_ptr<GeneralTypeObject>    type;
wstring    name;
vector<shared_ptr<GeneralAttributeDefine>>    attributes;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct GeneralEnumMemberTypeDefine : public GeneralTypeDefine
			{
				
				wstring    name;
vector<shared_ptr<GeneralAttributeDefine>>    attributes;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct GeneralClassTypeDefine : public GeneralTypeDefine
			{
				
				vector<shared_ptr<GeneralTypeDefine>>    subTypes;
vector<shared_ptr<GeneralClassMemberTypeDefine>>    members;
shared_ptr<GeneralTypeObject>    parent;
wstring    name;
vector<shared_ptr<GeneralAttributeDefine>>    attributes;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct GeneralEnumTypeDefine : public GeneralTypeDefine
			{
				
				vector<shared_ptr<GeneralEnumMemberTypeDefine>>    members;
wstring    name;
vector<shared_ptr<GeneralAttributeDefine>>    attributes;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			struct GeneralGrammarTypeDefine;
struct GeneralGrammarAlternationTypeDefine;
struct GeneralGrammarCreateTypeDefine;
struct GeneralGrammarUsingTypeDefine;
struct GeneralGrammarSetterTypeDefine;
struct GeneralGrammarAssignTypeDefine;
struct GeneralGrammarOptionalTypeDefine;
struct GeneralGrammarLoopTypeDefine;
struct GeneralGrammarSequenceTypeDefine;
struct GeneralGrammarNormalTypeDefine;
struct GeneralGrammarTextTypeDefine;

			struct GeneralGrammarTypeDefine 
			{
				
				
				
				
				class IVisitor
				{
				public:
					virtual void		Visit(GeneralGrammarTypeDefine* node) =0;
virtual void		Visit(GeneralGrammarAlternationTypeDefine* node) =0;
virtual void		Visit(GeneralGrammarCreateTypeDefine* node) =0;
virtual void		Visit(GeneralGrammarUsingTypeDefine* node) =0;
virtual void		Visit(GeneralGrammarSetterTypeDefine* node) =0;
virtual void		Visit(GeneralGrammarAssignTypeDefine* node) =0;
virtual void		Visit(GeneralGrammarOptionalTypeDefine* node) =0;
virtual void		Visit(GeneralGrammarLoopTypeDefine* node) =0;
virtual void		Visit(GeneralGrammarSequenceTypeDefine* node) =0;
virtual void		Visit(GeneralGrammarNormalTypeDefine* node) =0;
virtual void		Visit(GeneralGrammarTextTypeDefine* node) =0;

				};
				virtual void									Accept(IVisitor* )
				{

				}
				
			};
			


			
			struct GeneralGrammarTextTypeDefine : public GeneralGrammarTypeDefine
			{
				
				wstring    text;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct GeneralGrammarNormalTypeDefine : public GeneralGrammarTypeDefine
			{
				
				wstring    name;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct GeneralGrammarSequenceTypeDefine : public GeneralGrammarTypeDefine
			{
				
				shared_ptr<GeneralGrammarTypeDefine>    first;
shared_ptr<GeneralGrammarTypeDefine>    second;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct GeneralGrammarLoopTypeDefine : public GeneralGrammarTypeDefine
			{
				
				shared_ptr<GeneralGrammarTypeDefine>    grammar;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct GeneralGrammarOptionalTypeDefine : public GeneralGrammarTypeDefine
			{
				
				shared_ptr<GeneralGrammarTypeDefine>    grammar;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct GeneralGrammarAssignTypeDefine : public GeneralGrammarTypeDefine
			{
				
				wstring    name;
shared_ptr<GeneralGrammarTypeDefine>    grammar;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct GeneralGrammarSetterTypeDefine : public GeneralGrammarTypeDefine
			{
				
				shared_ptr<GeneralGrammarTypeDefine>    grammar;
wstring    name;
wstring    value;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct GeneralGrammarUsingTypeDefine : public GeneralGrammarTypeDefine
			{
				
				shared_ptr<GeneralGrammarTypeDefine>    grammar;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct GeneralGrammarCreateTypeDefine : public GeneralGrammarTypeDefine
			{
				
				shared_ptr<GeneralTypeObject>    type;
shared_ptr<GeneralGrammarTypeDefine>    grammar;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct GeneralGrammarAlternationTypeDefine : public GeneralGrammarTypeDefine
			{
				
				shared_ptr<GeneralGrammarTypeDefine>    left;
shared_ptr<GeneralGrammarTypeDefine>    right;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct GeneralRuleDefine 
			{
				
				shared_ptr<GeneralTypeObject>    type;
wstring    name;
vector<shared_ptr<GeneralGrammarTypeDefine>>    grammars;
vector<shared_ptr<GeneralAttributeDefine>>    attributes;

				
				
			};
			


			
			struct GeneralTableDefine 
			{
				
				vector<shared_ptr<GeneralHeadInfoDefine>>    heads;
vector<shared_ptr<GeneralTypeDefine>>    types;
vector<shared_ptr<GeneralTokenDefine>>    tokens;
vector<shared_ptr<GeneralRuleDefine>>    rules;

				
				
			};
			
				shared_ptr<ztl::general_parser::GeneralTableDefine> BootStrapDefineTable();
				
			shared_ptr<void> GenerateHeterogeneousParserTree(ztl::general_parser::GeneralLRExecutor& parser,ztl::general_parser::GeneralTreeNode* root);
			shared_ptr<void>	GenerateHeterogeneousParserTree(ztl::general_parser::GeneralLRExecutor& parser);
			
							}
						 
							}
						 