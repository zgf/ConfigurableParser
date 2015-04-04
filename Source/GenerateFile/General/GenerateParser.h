
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
				#include "..\..\Include\GeneralParser.h"
				
							namespace ztl
							{
								
							namespace general_parser
							{
								


			
			struct GeneralHeadInfoDefine 
			{
				
				wstring    property;
wstring    value;

				
				
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

				
				
			};
			


			struct GeneralSubTypeObject;
struct GeneralNormalTypeObject;
struct GeneralTokenTypeObject;
struct GeneralArrayTypeObject;

			struct GeneralTypeObject 
			{
				
				
				
				
				class IVisitor
				{
				public:
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
			


			struct GeneralEnumTypeDefine;
struct GeneralClassTypeDefine;
struct GeneralEnumMemberTypeDefine;
struct GeneralClassMemberTypeDefine;

			struct GeneralTypeDefine 
			{
				
				
				
				
				class IVisitor
				{
				public:
					virtual void		Visit(GeneralEnumTypeDefine* node) =0;
virtual void		Visit(GeneralClassTypeDefine* node) =0;
virtual void		Visit(GeneralEnumMemberTypeDefine* node) =0;
virtual void		Visit(GeneralClassMemberTypeDefine* node) =0;

				};
				virtual void									Accept(IVisitor* )
				{

				}
				
			};
			


			
			struct GeneralClassMemberTypeDefine : public GeneralTypeDefine
			{
				
				shared_ptr<GeneralTypeObject>    type;
wstring    name;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct GeneralEnumMemberTypeDefine : public GeneralTypeDefine
			{
				
				wstring    name;

				
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

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct GeneralEnumTypeDefine : public GeneralTypeDefine
			{
				
				vector<shared_ptr<GeneralEnumMemberTypeDefine>>    members;
wstring    name;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


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

				
				
			};
			


			
			struct GeneralTableDefine 
			{
				
				vector<shared_ptr<GeneralHeadInfoDefine>>    heads;
vector<shared_ptr<GeneralTypeDefine>>    types;
vector<shared_ptr<GeneralTokenDefine>>    tokens;
vector<shared_ptr<GeneralRuleDefine>>    rules;

				
				
			};
			
				shared_ptr<ztl::general_parser::GeneralTableDefine> BootStrapDefineTable();
				
			shared_ptr<void> GeneralHeterogeneousParserTree(ztl::general_parser::GeneralParser& parser,ztl::general_parser::GeneralTreeNode* root);
			shared_ptr<void>	GeneralHeterogeneousParserTree(ztl::general_parser::GeneralParser& parser);
			
							}
						 
							}
						 