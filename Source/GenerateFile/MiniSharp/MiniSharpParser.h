
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
								
							namespace miniSharp
							{
								


			struct IdentifierObject;
struct ArrayIdentifierObject;
struct ThisIdentifierObject;
struct SubIdentifierObject;
struct NormalIdentifierObject;
struct PrimaryIdentifierObject;

			struct IdentifierObject 
			{
				
				
				
				
				class IVisitor
				{
				public:
					virtual void		Visit(IdentifierObject* node) =0;
virtual void		Visit(ArrayIdentifierObject* node) =0;
virtual void		Visit(ThisIdentifierObject* node) =0;
virtual void		Visit(SubIdentifierObject* node) =0;
virtual void		Visit(NormalIdentifierObject* node) =0;
virtual void		Visit(PrimaryIdentifierObject* node) =0;

				};
				virtual void									Accept(IVisitor* )
				{

				}
				
				
			};
			


			
			struct PrimaryIdentifierObject : public IdentifierObject
			{
				
				wstring    primary;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct NormalIdentifierObject : public IdentifierObject
			{
				
				wstring    name;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct SubIdentifierObject : public IdentifierObject
			{
				
				shared_ptr<IdentifierObject>    scope;
shared_ptr<IdentifierObject>    right;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct ThisIdentifierObject : public IdentifierObject
			{
				
				
				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			struct AstExpression;
struct AstUsingDeclExpression;
struct CastExpression;
struct AttachSuffixExpression;
struct AttachPrefixExpression;
struct LogicExpression;
struct LogicAndExpression;
struct LogicOrExpression;
struct CompareExpression;
struct ArithmeticExpression;
struct LowArithmeticExpression;
struct HighArithmeticExpression;
struct AssignExpression;
struct NewExpression;
struct FunctionCallExpression;
struct DeclarationExpression;
struct LiteralExpression;
struct CharExpression;
struct StringExpression;
struct NullExpression;
struct BoolExpression;
struct DoubleExpression;
struct IntegerExpression;
struct VariableExpression;

			struct AstExpression 
			{
				
				
				
				
				class IVisitor
				{
				public:
					virtual void		Visit(AstExpression* node) =0;
virtual void		Visit(AstUsingDeclExpression* node) =0;
virtual void		Visit(CastExpression* node) =0;
virtual void		Visit(AttachSuffixExpression* node) =0;
virtual void		Visit(AttachPrefixExpression* node) =0;
virtual void		Visit(LogicExpression* node) =0;
virtual void		Visit(LogicAndExpression* node) =0;
virtual void		Visit(LogicOrExpression* node) =0;
virtual void		Visit(CompareExpression* node) =0;
virtual void		Visit(ArithmeticExpression* node) =0;
virtual void		Visit(LowArithmeticExpression* node) =0;
virtual void		Visit(HighArithmeticExpression* node) =0;
virtual void		Visit(AssignExpression* node) =0;
virtual void		Visit(NewExpression* node) =0;
virtual void		Visit(FunctionCallExpression* node) =0;
virtual void		Visit(DeclarationExpression* node) =0;
virtual void		Visit(LiteralExpression* node) =0;
virtual void		Visit(CharExpression* node) =0;
virtual void		Visit(StringExpression* node) =0;
virtual void		Visit(NullExpression* node) =0;
virtual void		Visit(BoolExpression* node) =0;
virtual void		Visit(DoubleExpression* node) =0;
virtual void		Visit(IntegerExpression* node) =0;
virtual void		Visit(VariableExpression* node) =0;

				};
				virtual void									Accept(IVisitor* )
				{

				}
				
				
			};
			


			
			struct ArrayIdentifierObject : public IdentifierObject
			{
				
				shared_ptr<IdentifierObject>    element;
vector<shared_ptr<AstExpression>>    numbers;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct LiteralExpression : public AstExpression
			{
				
				
				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct VariableExpression : public LiteralExpression
			{
				
				shared_ptr<IdentifierObject>    variable;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct IntegerExpression : public LiteralExpression
			{
				
				wstring    value;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct DoubleExpression : public LiteralExpression
			{
				
				wstring    value;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct BoolExpression : public LiteralExpression
			{
				
				wstring    value;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct NullExpression : public LiteralExpression
			{
				
				
				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct StringExpression : public LiteralExpression
			{
				
				wstring    value;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct CharExpression : public LiteralExpression
			{
				
				wstring    value;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


					enum class PrefixPropertyValue
					{
						Constants,
ReadOnly,
Static,
Public,
Private,
Virtual,

					};
					


			
			struct PrefixProperty 
			{
				
				PrefixPropertyValue    value;

				
				
				
			};
			


			
			struct DeclarationExpression : public AstExpression
			{
				
				shared_ptr<IdentifierObject>    type;
vector<shared_ptr<AstExpression>>    expressions;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct FunctionCallExpression : public AstExpression
			{
				
				shared_ptr<IdentifierObject>    name;
vector<shared_ptr<AstExpression>>    args;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct NewExpression : public AstExpression
			{
				
				shared_ptr<IdentifierObject>    type;
vector<shared_ptr<AstExpression>>    args;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct AssignExpression : public AstExpression
			{
				
				shared_ptr<IdentifierObject>    variable;
shared_ptr<AstExpression>    right;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct ArithmeticExpression : public AstExpression
			{
				
				
				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct HighArithmeticExpression : public ArithmeticExpression
			{
				
				wstring    op;
shared_ptr<AstExpression>    left;
shared_ptr<AstExpression>    right;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct LowArithmeticExpression : public ArithmeticExpression
			{
				
				wstring    op;
shared_ptr<AstExpression>    left;
shared_ptr<AstExpression>    right;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct LogicExpression : public AstExpression
			{
				
				
				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct CompareExpression : public LogicExpression
			{
				
				wstring    op;
shared_ptr<AstExpression>    left;
shared_ptr<AstExpression>    right;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct LogicOrExpression : public LogicExpression
			{
				
				shared_ptr<AstExpression>    left;
shared_ptr<AstExpression>    right;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct LogicAndExpression : public LogicExpression
			{
				
				shared_ptr<AstExpression>    left;
shared_ptr<AstExpression>    right;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct AttachPrefixExpression : public AstExpression
			{
				
				wstring    op;
shared_ptr<AstExpression>    expression;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct AttachSuffixExpression : public AstExpression
			{
				
				wstring    op;
shared_ptr<AstExpression>    expression;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct CastExpression : public AstExpression
			{
				
				shared_ptr<IdentifierObject>    type;
shared_ptr<AstExpression>    expression;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			struct AstStatement;
struct UsingStatement;
struct ContinueStatement;
struct BreakStatement;
struct ReturnStatement;
struct SwitchStatement;
struct DefaultStatement;
struct CaseStatement;
struct ForStatement;
struct WhileStatement;
struct IfStatement;
struct ScopeStatement;
struct ExpressionStatement;

			struct AstStatement 
			{
				
				
				
				
				class IVisitor
				{
				public:
					virtual void		Visit(AstStatement* node) =0;
virtual void		Visit(UsingStatement* node) =0;
virtual void		Visit(ContinueStatement* node) =0;
virtual void		Visit(BreakStatement* node) =0;
virtual void		Visit(ReturnStatement* node) =0;
virtual void		Visit(SwitchStatement* node) =0;
virtual void		Visit(DefaultStatement* node) =0;
virtual void		Visit(CaseStatement* node) =0;
virtual void		Visit(ForStatement* node) =0;
virtual void		Visit(WhileStatement* node) =0;
virtual void		Visit(IfStatement* node) =0;
virtual void		Visit(ScopeStatement* node) =0;
virtual void		Visit(ExpressionStatement* node) =0;

				};
				virtual void									Accept(IVisitor* )
				{

				}
				
				
			};
			


			
			struct ExpressionStatement : public AstStatement
			{
				
				vector<shared_ptr<PrefixProperty>>    properties;
shared_ptr<AstExpression>    expression;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct ScopeStatement : public AstStatement
			{
				
				vector<shared_ptr<AstStatement>>    statements;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct IfStatement : public AstStatement
			{
				
				shared_ptr<AstExpression>    conditional;
shared_ptr<ScopeStatement>    truePath;
shared_ptr<AstStatement>    falsePath;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct WhileStatement : public AstStatement
			{
				
				shared_ptr<AstExpression>    conditional;
shared_ptr<ScopeStatement>    body;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct ForStatement : public AstStatement
			{
				
				shared_ptr<ExpressionStatement>    initExpression;
shared_ptr<AstExpression>    conditional;
vector<shared_ptr<AstExpression>>    stepExpressions;
shared_ptr<ScopeStatement>    body;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct CaseStatement : public AstStatement
			{
				
				shared_ptr<AstExpression>    value;
vector<shared_ptr<AstStatement>>    statements;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct DefaultStatement : public AstStatement
			{
				
				vector<shared_ptr<AstStatement>>    statements;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct SwitchStatement : public AstStatement
			{
				
				shared_ptr<AstExpression>    conditional;
vector<shared_ptr<CaseStatement>>    cases;
shared_ptr<AstStatement>    default;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct ReturnStatement : public AstStatement
			{
				
				shared_ptr<AstExpression>    expression;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct BreakStatement : public AstStatement
			{
				
				
				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct ContinueStatement : public AstStatement
			{
				
				
				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct UsingStatement : public AstStatement
			{
				
				shared_ptr<AstExpression>    source;
shared_ptr<ScopeStatement>    body;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			struct AstTypeDefine;
struct AstNamesapceDefine;
struct AstClassTypeDefine;
struct AstEnumTypeDefine;
struct AstInterfaceTypeDefine;
struct AstFunctionTypeDefine;
struct AstFieldTypeDefine;

			struct AstTypeDefine 
			{
				
				
				
				
				class IVisitor
				{
				public:
					virtual void		Visit(AstTypeDefine* node) =0;
virtual void		Visit(AstNamesapceDefine* node) =0;
virtual void		Visit(AstClassTypeDefine* node) =0;
virtual void		Visit(AstEnumTypeDefine* node) =0;
virtual void		Visit(AstInterfaceTypeDefine* node) =0;
virtual void		Visit(AstFunctionTypeDefine* node) =0;
virtual void		Visit(AstFieldTypeDefine* node) =0;

				};
				virtual void									Accept(IVisitor* )
				{

				}
				
				
			};
			


			
			struct AstFieldTypeDefine : public AstTypeDefine
			{
				
				shared_ptr<IdentifierObject>    type;
wstring    name;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct AstFunctionParameter 
			{
				
				shared_ptr<IdentifierObject>    type;
wstring    name;

				
				
				
			};
			


			
			struct FunctionSignature 
			{
				
				shared_ptr<IdentifierObject>    returnType;
vector<shared_ptr<AstFunctionParameter>>    parameters;
wstring    name;

				
				
				
			};
			


			
			struct AstFunctionTypeDefine : public AstTypeDefine
			{
				
				shared_ptr<FunctionSignature>    sign;
shared_ptr<ScopeStatement>    body;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct InterfaceMember 
			{
				
				vector<shared_ptr<PrefixProperty>>    properties;
shared_ptr<FunctionSignature>    sign;

				
				
				
			};
			


			
			struct AstInterfaceTypeDefine : public AstTypeDefine
			{
				
				wstring    name;
vector<shared_ptr<IdentifierObject>>    parents;
vector<shared_ptr<InterfaceMember>>    members;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct EnumFieldTypeDefine 
			{
				
				wstring    name;
wstring    value;

				
				
				
			};
			


			
			struct AstEnumTypeDefine : public AstTypeDefine
			{
				
				wstring    name;
wstring    base;
vector<shared_ptr<EnumFieldTypeDefine>>    members;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct AstClassMember 
			{
				
				vector<shared_ptr<PrefixProperty>>    properties;
shared_ptr<AstTypeDefine>    define;

				
				
				
			};
			


			
			struct AstClassTypeDefine : public AstTypeDefine
			{
				
				wstring    name;
shared_ptr<IdentifierObject>    parent;
vector<shared_ptr<IdentifierObject>>    parentInterfaces;
vector<shared_ptr<AstClassMember>>    members;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct NamespaceMember 
			{
				
				vector<shared_ptr<PrefixProperty>>    properties;
shared_ptr<AstTypeDefine>    define;

				
				
				
			};
			


			
			struct AstNamesapceDefine : public AstTypeDefine
			{
				
				wstring    name;
vector<shared_ptr<NamespaceMember>>    members;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct AstUsingDeclExpression : public AstExpression
			{
				
				shared_ptr<IdentifierObject>    module;
wstring    name;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct ParserASTRoot 
			{
				
				vector<shared_ptr<AstUsingDeclExpression>>    usings;
vector<shared_ptr<NamespaceMember>>    members;

				
				
				
			};
			
				shared_ptr<ztl::general_parser::GeneralTableDefine> BootStrapDefineTable();
				
			shared_ptr<void> GenerateHeterogeneousParserTree(ztl::general_parser::GeneralLRExecutor& parser,ztl::general_parser::GeneralTreeNode* root);
			shared_ptr<void>	GenerateHeterogeneousParserTree(ztl::general_parser::GeneralLRExecutor& parser);
			
							}
						 
							}
						 