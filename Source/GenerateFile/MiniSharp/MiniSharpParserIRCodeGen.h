
					#pragma once
					#include "MiniSharpParser.h"
				
							namespace ztl
							{
								
							namespace miniSharp
							{
								
					struct IRCodeGenContext
					{

					};
					template<typename BaseClassType>
					class IRCodeGen : public BaseClassType::IVisitor
					{
						IRCodeGenContext* context;
					};
				


								class IdentifierObject;
class ArrayIdentifierObject;
class ThisIdentifierObject;
class SubIdentifierObject;
class NormalIdentifierObject;
class PrimaryIdentifierObject;

								template<>
								class IRCodeGen<IdentifierObject> : public IdentifierObject::IVisitor
								{
								public:
									IRCodeGen(IRCodeGenContext* _context):context(_context)
									{

									}

									static IRCodeGen<AstExpression> CodeGen(AstExpression* node, IRCodeGenContext* context)
									{
										IRCodeGen<AstExpression> object(context);
										object.Visit(node);
										return object;
									}
								public:
									virtual void		Visit(IdentifierObject* ) override
														{
							
														}
														virtual void		Visit(ArrayIdentifierObject* ) override
														{
							
														}
														virtual void		Visit(ThisIdentifierObject* ) override
														{
							
														}
														virtual void		Visit(SubIdentifierObject* ) override
														{
							
														}
														virtual void		Visit(NormalIdentifierObject* ) override
														{
							
														}
														virtual void		Visit(PrimaryIdentifierObject* ) override
														{
							
														}
															
								public:
									Value* GetValue()
									{
										return value;
									}
									IRCodeGenContext* Context()
									{
										return context;
									}
								private:
									Value* value;
									IRCodeGenContext* context;
								};

						










								class AstExpression;
class AstUsingDeclExpression;
class CastExpression;
class AttachSuffixExpression;
class AttachPrefixExpression;
class LogicExpression;
class LogicAndExpression;
class LogicOrExpression;
class CompareExpression;
class ArithmeticExpression;
class LowArithmeticExpression;
class HighArithmeticExpression;
class AssignExpression;
class NewExpression;
class FunctionCallExpression;
class DeclarationExpression;
class LiteralExpression;
class CharExpression;
class StringExpression;
class NullExpression;
class BoolExpression;
class DoubleExpression;
class IntegerExpression;
class VariableExpression;

								template<>
								class IRCodeGen<AstExpression> : public AstExpression::IVisitor
								{
								public:
									IRCodeGen(IRCodeGenContext* _context):context(_context)
									{

									}

									static IRCodeGen<AstExpression> CodeGen(AstExpression* node, IRCodeGenContext* context)
									{
										IRCodeGen<AstExpression> object(context);
										object.Visit(node);
										return object;
									}
								public:
									virtual void		Visit(AstExpression* ) override
														{
							
														}
														virtual void		Visit(AstUsingDeclExpression* ) override
														{
							
														}
														virtual void		Visit(CastExpression* ) override
														{
							
														}
														virtual void		Visit(AttachSuffixExpression* ) override
														{
							
														}
														virtual void		Visit(AttachPrefixExpression* ) override
														{
							
														}
														virtual void		Visit(LogicExpression* ) override
														{
							
														}
														virtual void		Visit(LogicAndExpression* ) override
														{
							
														}
														virtual void		Visit(LogicOrExpression* ) override
														{
							
														}
														virtual void		Visit(CompareExpression* ) override
														{
							
														}
														virtual void		Visit(ArithmeticExpression* ) override
														{
							
														}
														virtual void		Visit(LowArithmeticExpression* ) override
														{
							
														}
														virtual void		Visit(HighArithmeticExpression* ) override
														{
							
														}
														virtual void		Visit(AssignExpression* ) override
														{
							
														}
														virtual void		Visit(NewExpression* ) override
														{
							
														}
														virtual void		Visit(FunctionCallExpression* ) override
														{
							
														}
														virtual void		Visit(DeclarationExpression* ) override
														{
							
														}
														virtual void		Visit(LiteralExpression* ) override
														{
							
														}
														virtual void		Visit(CharExpression* ) override
														{
							
														}
														virtual void		Visit(StringExpression* ) override
														{
							
														}
														virtual void		Visit(NullExpression* ) override
														{
							
														}
														virtual void		Visit(BoolExpression* ) override
														{
							
														}
														virtual void		Visit(DoubleExpression* ) override
														{
							
														}
														virtual void		Visit(IntegerExpression* ) override
														{
							
														}
														virtual void		Visit(VariableExpression* ) override
														{
							
														}
															
								public:
									Value* GetValue()
									{
										return value;
									}
									IRCodeGenContext* Context()
									{
										return context;
									}
								private:
									Value* value;
									IRCodeGenContext* context;
								};

						




















































								class AstStatement;
class UsingStatement;
class ContinueStatement;
class BreakStatement;
class ReturnStatement;
class SwitchStatement;
class DefaultStatement;
class CaseStatement;
class ForStatement;
class WhileStatement;
class IfStatement;
class ScopeStatement;
class ExpressionStatement;

								template<>
								class IRCodeGen<AstStatement> : public AstStatement::IVisitor
								{
								public:
									IRCodeGen(IRCodeGenContext* _context):context(_context)
									{

									}

									static IRCodeGen<AstExpression> CodeGen(AstExpression* node, IRCodeGenContext* context)
									{
										IRCodeGen<AstExpression> object(context);
										object.Visit(node);
										return object;
									}
								public:
									virtual void		Visit(AstStatement* ) override
														{
							
														}
														virtual void		Visit(UsingStatement* ) override
														{
							
														}
														virtual void		Visit(ContinueStatement* ) override
														{
							
														}
														virtual void		Visit(BreakStatement* ) override
														{
							
														}
														virtual void		Visit(ReturnStatement* ) override
														{
							
														}
														virtual void		Visit(SwitchStatement* ) override
														{
							
														}
														virtual void		Visit(DefaultStatement* ) override
														{
							
														}
														virtual void		Visit(CaseStatement* ) override
														{
							
														}
														virtual void		Visit(ForStatement* ) override
														{
							
														}
														virtual void		Visit(WhileStatement* ) override
														{
							
														}
														virtual void		Visit(IfStatement* ) override
														{
							
														}
														virtual void		Visit(ScopeStatement* ) override
														{
							
														}
														virtual void		Visit(ExpressionStatement* ) override
														{
							
														}
															
								public:
									Value* GetValue()
									{
										return value;
									}
									IRCodeGenContext* Context()
									{
										return context;
									}
								private:
									Value* value;
									IRCodeGenContext* context;
								};

						


























								class AstTypeDefine;
class AstNamesapceDefine;
class AstClassTypeDefine;
class AstEnumTypeDefine;
class AstInterfaceTypeDefine;
class AstFunctionTypeDefine;
class AstFieldTypeDefine;

								template<>
								class IRCodeGen<AstTypeDefine> : public AstTypeDefine::IVisitor
								{
								public:
									IRCodeGen(IRCodeGenContext* _context):context(_context)
									{

									}

									static IRCodeGen<AstExpression> CodeGen(AstExpression* node, IRCodeGenContext* context)
									{
										IRCodeGen<AstExpression> object(context);
										object.Visit(node);
										return object;
									}
								public:
									virtual void		Visit(AstTypeDefine* ) override
														{
							
														}
														virtual void		Visit(AstNamesapceDefine* ) override
														{
							
														}
														virtual void		Visit(AstClassTypeDefine* ) override
														{
							
														}
														virtual void		Visit(AstEnumTypeDefine* ) override
														{
							
														}
														virtual void		Visit(AstInterfaceTypeDefine* ) override
														{
							
														}
														virtual void		Visit(AstFunctionTypeDefine* ) override
														{
							
														}
														virtual void		Visit(AstFieldTypeDefine* ) override
														{
							
														}
															
								public:
									Value* GetValue()
									{
										return value;
									}
									IRCodeGenContext* Context()
									{
										return context;
									}
								private:
									Value* value;
									IRCodeGenContext* context;
								};

						




























							}
						 
							}
						 