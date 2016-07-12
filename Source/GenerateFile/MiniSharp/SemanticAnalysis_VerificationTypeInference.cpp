#pragma once
#include"Include\stdafx.h"
#include "MiniSharpSymbolManager.h"
//收集定义
//对类型定义生成LLVM::Type*
//绑定标识符和定义
//对所有的表达式做类型推导
namespace ztl
{
	namespace miniSharp
	{
		struct  VerificationTypeInferenceVisitor :public ztl::miniSharp::IdentifierObject::IVisitor
		{
			VerificationTypeInferenceVisitor(MiniSharpSymbolManager* _manager) :manager(_manager)
			{

			}

			virtual void		Visit(IdentifierObject*) override { assert(false); }
			virtual void		Visit(ThisIdentifierObject* ) override {}
			virtual void		Visit(SubIdentifierObject* node) override 
			{
				node->scope->Accept(this);
				node->right->Accept(this);
			}
			virtual void		Visit(ArrayIdentifierObject* node) override;
			
			virtual void		Visit(NormalIdentifierObject*) override {}
			virtual void		Visit(PrimaryIdentifierObject*) override {}

			MiniSharpSymbolManager* manager;
		};

		struct VerificationTypeInferenceExpressionVisitor :public ztl::miniSharp::AstExpression::IVisitor
		{
			VerificationTypeInferenceExpressionVisitor(MiniSharpSymbolManager* _manager)
				:manager(_manager)
			{

			}
			virtual void		Visit(AstExpression* ) { assert(false); }
			virtual void		Visit(CastExpression* node) override
			{
				node->expression->Accept(this);
				auto typeSymbol = manager->GetIdentifierDefByIdentifierObject(node->type.get());
				auto llvmType = typeSymbol->GetLLVMTypeDef();
				manager->AddTypeInferenceMap(node, llvmType);
			}
			virtual void		Visit(AttachSuffixExpression* node) override
			{
				node->expression->Accept(this);
				auto expressionType = manager->GetTypeInferenceMapByLLVMType(node->expression.get());
				manager->AddTypeInferenceMap(node, expressionType);
			}
			virtual void		Visit(AttachPrefixExpression* node) override
			{
				node->expression->Accept(this);
				auto expressionType = manager->GetTypeInferenceMapByLLVMType(node->expression.get());
				manager->AddTypeInferenceMap(node, expressionType);
			}
			virtual void		Visit(LogicExpression* ) override
			{
				assert(false);
			}
			virtual void		Visit(LogicAndExpression* node) override
			{
				node->left->Accept(this);
				node->right->Accept(this);
				auto leftType = manager->GetTypeInferenceMapByLLVMType(node->left.get());
				auto rightType = manager->GetTypeInferenceMapByLLVMType(node->right.get());
				if (leftType->getTypeID() != rightType->getTypeID())
				{
					throw ztl_exception(L"LogicAndExpression type not match");
				}
				manager->AddTypeInferenceMap(node, leftType);
			}
			virtual void		Visit(LogicOrExpression* node) override
			{
				node->left->Accept(this);
				node->right->Accept(this);
				auto leftType = manager->GetTypeInferenceMapByLLVMType(node->left.get());
				auto rightType = manager->GetTypeInferenceMapByLLVMType(node->right.get());
				if (leftType->getTypeID() != rightType->getTypeID())
				{
					throw ztl_exception(L"LogicOrExpression type not match");
				}
				manager->AddTypeInferenceMap(node, leftType);
			}
			virtual void		Visit(CompareExpression* node) override
			{
				node->left->Accept(this);
				node->right->Accept(this);
				auto leftType = manager->GetTypeInferenceMapByLLVMType(node->left.get());
				auto rightType = manager->GetTypeInferenceMapByLLVMType(node->right.get());
				if (leftType->getTypeID() != rightType->getTypeID())
				{
					throw ztl_exception(L"CompareExpression type not match");
				}
				manager->AddTypeInferenceMap(node, leftType);
			}
			virtual void		Visit(ArithmeticExpression* ) override
			{
				assert(false);
			}
			virtual void		Visit(LowArithmeticExpression* node) override
			{
				node->left->Accept(this);
				node->right->Accept(this);
				auto leftType = manager->GetTypeInferenceMapByLLVMType(node->left.get());
				auto rightType = manager->GetTypeInferenceMapByLLVMType(node->right.get());
				if (leftType->getTypeID() != rightType->getTypeID())
				{
					throw ztl_exception(L"LowArithmeticExpression type not match");
				}
				manager->AddTypeInferenceMap(node, leftType);
			}
			virtual void		Visit(HighArithmeticExpression* node) override
			{
				node->left->Accept(this);
				node->right->Accept(this);
				auto leftType = manager->GetTypeInferenceMapByLLVMType(node->left.get());
				auto rightType = manager->GetTypeInferenceMapByLLVMType(node->right.get());
				if (leftType->getTypeID() != rightType->getTypeID())
				{
					throw ztl_exception(L"HighArithmeticExpression type not match");
				}
				manager->AddTypeInferenceMap(node, leftType);
			}
			virtual void		Visit(AssignExpression* node) override
			{
				node->right->Accept(this);
				auto variableSymbol = manager->GetIdentifierDefByIdentifierObject(node->variable.get());
				auto type = variableSymbol->GetDescription();
				auto llvmType = type->GetLLVMTypeDef();
				auto rightType = manager->GetTypeInferenceMapByLLVMType(node->right.get());
				if (llvmType->getTypeID() != rightType->getTypeID())
				{
					throw ztl_exception(L"AssignExpression type not match");
				}
				manager->AddTypeInferenceMap(node, llvmType);
			}
			virtual void		Visit(NewExpression* node) override
			{
				vector<llvm::Type*> elements;
				for (auto&&arg : node->args)
				{
					arg->Accept(this);
				}
				for (auto&&arg : node->args)
				{
					auto type = manager->GetTypeInferenceMapByLLVMType(arg.get());
					elements.emplace_back(type);

				}

				auto classSymbol = manager->GetIdentifierDefByIdentifierObject(node->type.get());
				assert(classSymbol->isClassTypeDef());

				auto functionSymbol = classSymbol->GetUniqueSymbolByName(classSymbol->GetName());

				vector<llvm::Type*> types;
				for (auto&& arg : functionSymbol->GetArgs())
				{
					auto argTypeSymbol = arg->GetDescription();
					auto argType = argTypeSymbol->GetLLVMTypeDef();
					types.emplace_back(argType);
				}
				auto result = equal(elements.begin(), elements.end(), types.begin(), types.end(), [](llvm::Type*left, llvm::Type* right)
				{
					return left->getTypeID() == right->getTypeID();
				});
				if (result == false)
				{
					throw ztl_exception(L"type not match in function args");
				}
				else
				{
					manager->AddTypeInferenceMap(node, functionSymbol->GetDescription()->GetLLVMTypeDef());
				}
			}
			virtual void		Visit(FunctionCallExpression* node) override
			{
				vector<llvm::Type*> elements;
				for (auto&&arg : node->args)
				{
					arg->Accept(this);
				}

				for (auto&&arg : node->args)
				{
					auto type = manager->GetTypeInferenceMapByLLVMType(arg.get());
					elements.emplace_back(type);
					
				}

				auto functionSymbol = manager->GetIdentifierDefByIdentifierObject(node->name.get());
				vector<llvm::Type*> types;
				for (auto&& arg : functionSymbol->GetArgs())
				{
					auto argTypeSymbol = arg->GetDescription();
					auto argType = argTypeSymbol->GetLLVMTypeDef();
					types.emplace_back(argType);
				}
				auto result = equal(elements.begin(), elements.end(), types.begin(), types.end(), [](llvm::Type*left, llvm::Type* right) 
				{
					return left->getTypeID() == right->getTypeID();
				});
				if (result == false)
				{
					throw ztl_exception(L"type not match in function args");
				}
				else
				{
					manager->AddTypeInferenceMap(node, functionSymbol->GetDescription()->GetLLVMTypeDef());
				}
			}
			virtual void		Visit(DeclarationExpression* node) override
			{
				auto typeSymbol = manager->GetIdentifierDefByIdentifierObject(node->type.get());
				for (auto&& expression : node->expressions)
				{
					expression->Accept(this);
				}
				vector<llvm::Type*> elements;
				for (auto&& expression : node->expressions)
				{
					auto type = manager->GetTypeInferenceMapByLLVMType(expression.get());
					elements.emplace_back(type);
				}
				auto result = std::all_of(elements.begin(), elements.end(), [typeSymbol](llvm::Type* value) {return value == typeSymbol->GetLLVMTypeDef();});
				if (result == false)
				{
					throw ztl_exception(L"DeclarationExpression type not match");
				}
			}
			virtual void		Visit(LiteralExpression* ) override
			{
				assert(false);
			}
			virtual void		Visit(CharExpression* node) override 
			{
				auto symbol = manager->GetLiteralSymbolByAstExpression(node);
				auto symbolTypeSymbol = symbol->GetDescription();
				assert(symbolTypeSymbol->isUnicodeCharTypeDef());
				assert(symbolTypeSymbol != nullptr);
				manager->AddTypeInferenceMap(node, symbolTypeSymbol->GetLLVMTypeDef());
			}
			virtual void		Visit(StringExpression* node) override
			{
				auto symbol = manager->GetLiteralSymbolByAstExpression(node);
				auto symbolTypeSymbol = symbol->GetDescription();
				assert(symbolTypeSymbol->isStringTypeDef());
				assert(symbolTypeSymbol != nullptr);
				manager->AddTypeInferenceMap(node, symbolTypeSymbol->GetLLVMTypeDef());

			}
			virtual void		Visit(NullExpression* node) override
			{
				auto symbol = manager->GetLiteralSymbolByAstExpression(node);
				auto symbolTypeSymbol = symbol->GetDescription();
				assert(symbolTypeSymbol->isIntTypeDef());
				assert(symbolTypeSymbol != nullptr);
				manager->AddTypeInferenceMap(node, symbolTypeSymbol->GetLLVMTypeDef());
			}
			virtual void		Visit(BoolExpression* node) override
			{
				auto symbol = manager->GetLiteralSymbolByAstExpression(node);
				auto symbolTypeSymbol = symbol->GetDescription();
				assert(symbolTypeSymbol->isBoolTypeDef());
				assert(symbolTypeSymbol != nullptr);
				manager->AddTypeInferenceMap(node, symbolTypeSymbol->GetLLVMTypeDef());
			}
			virtual void		Visit(DoubleExpression* node) override
			{
				auto symbol = manager->GetLiteralSymbolByAstExpression(node);
				auto symbolTypeSymbol = symbol->GetDescription();
				assert(symbolTypeSymbol->isDoubleTypeDef());
				assert(symbolTypeSymbol != nullptr);
				manager->AddTypeInferenceMap(node, symbolTypeSymbol->GetLLVMTypeDef());
			}
			virtual void		Visit(IntegerExpression* node)override
			{
				auto symbol = manager->GetLiteralSymbolByAstExpression(node);
				auto symbolTypeSymbol = symbol->GetDescription();
				assert(symbolTypeSymbol->isIntTypeDef());
				assert(symbolTypeSymbol != nullptr);
				manager->AddTypeInferenceMap(node, symbolTypeSymbol->GetLLVMTypeDef());
			}
			virtual void		Visit(VariableExpression* node)override
			{
				auto typeSymbol = manager->GetIdentifierDefByIdentifierObject(node->variable.get());
				manager->AddTypeInferenceMap(node, typeSymbol->GetLLVMTypeDef());
			}
			virtual void		Visit(AstUsingDeclExpression* ) override {}

			MiniSharpSymbolManager* manager;
			MiniSharpPasrseSymbol* scope;
		};


		struct VerificationTypeInferenceStatementVisitor :public ztl::miniSharp::AstStatement::IVisitor
		{
			VerificationTypeInferenceStatementVisitor(MiniSharpSymbolManager* _manager)
				:manager(_manager)
			{

			}
			virtual void		Visit(AstStatement* ) { assert(false); }
			virtual void		Visit(UsingStatement* node)
			{
				{
					VerificationTypeInferenceExpressionVisitor visitor(manager);
					node->source->Accept(&visitor);
				}
				{
					node->body->Accept(this);
				}
			};
			virtual void		Visit(ContinueStatement* ) {}
			virtual void		Visit(BreakStatement* ) {}
			virtual void		Visit(ReturnStatement* node)
			{
				{
					VerificationTypeInferenceExpressionVisitor visitor(manager);
					node->expression->Accept(&visitor);
				}
			}
			virtual void		Visit(SwitchStatement* node)
			{
				{
					VerificationTypeInferenceExpressionVisitor visitor(manager);
					node->conditional->Accept(&visitor);
				}

				{
					for (auto&&caser : node->cases)
					{
						caser->Accept(this);
					}
				}

				{
					node->default->Accept(this);
				}
			}
			virtual void		Visit(CaseStatement* node)
			{
				for (auto&& statement : node->statements)
				{
					statement->Accept(this);
				}
			}
			virtual void		Visit(ForStatement* node)
			{
				{
					VerificationTypeInferenceExpressionVisitor visitor(manager);
					node->conditional->Accept(&visitor);
				}

				node->initExpression->Accept(this);
				for (auto&&expression : node->stepExpressions)
				{
					VerificationTypeInferenceExpressionVisitor visitor(manager);
					expression->Accept(&visitor);
				}
				node->body->Accept(this);
			}
			virtual void		Visit(WhileStatement* node)
			{
				{
					VerificationTypeInferenceExpressionVisitor visitor(manager);
					node->conditional->Accept(&visitor);
				}
				node->body->Accept(this);
			}
			virtual void		Visit(IfStatement* node)
			{
				{
					VerificationTypeInferenceExpressionVisitor visitor(manager);
					node->conditional->Accept(&visitor);
				}

				if (node->falsePath == nullptr)
				{
					node->falsePath->Accept(this);
				}

				if (node->truePath == nullptr)
				{
					node->truePath->Accept(this);
				}
			}
			virtual void		Visit(ScopeStatement* node)
			{
				for (auto&& statement : node->statements)
				{
					statement->Accept(this);
				}
			}
			virtual void		Visit(ExpressionStatement* node)
			{
				VerificationTypeInferenceExpressionVisitor visitor(manager);
				node->expression->Accept(&visitor);
			}
			virtual void		Visit(DefaultStatement* node)
			{
				for (auto&& statement : node->statements)
				{
					statement->Accept(this);
				}
			}


			MiniSharpSymbolManager* manager;
			MiniSharpPasrseSymbol* scope;
		};

		struct VerificationTypeInferenceTypeDefineVisitor :public ztl::miniSharp::AstTypeDefine::IVisitor
		{
			VerificationTypeInferenceTypeDefineVisitor(MiniSharpSymbolManager* _manager) :manager(_manager)
			{

			}
			virtual void		Visit(AstTypeDefine* ) { assert(false); }
			virtual void		Visit(AstNamesapceDefine* node)
			{
				for (auto&& member : node->members)
				{
					member->define->Accept(this);
				}
			}
			virtual void		Visit(AstClassTypeDefine* node)
			{
				for (auto&& member : node->members)
				{
					member->define->Accept(this);
				}
			};
			virtual void		Visit(AstEnumTypeDefine* ) {};
			virtual void		Visit(AstInterfaceTypeDefine*) { throw ztl_exception(L"error not support"); }
			virtual void		Visit(AstFunctionTypeDefine* node)
			{
				VerificationTypeInferenceStatementVisitor visitor(manager);
				for (auto&& statement : node->body->statements)
				{
					statement->Accept(&visitor);
				}
			}
			virtual void		Visit(AstFieldTypeDefine* ) {};

			MiniSharpSymbolManager* manager;
		};


		//do 类型推导
		void VerificationTypeInference(MiniSharpSymbolManager * manager, ztl::miniSharp::ParserASTRoot& root)
		{
			VerificationTypeInferenceTypeDefineVisitor visitor(manager);
			for (auto&&member : root.members)
			{
				member->define->Accept(&visitor);
			}
		}

		void VerificationTypeInference(MiniSharpSymbolManager * manager, const vector<shared_ptr<ztl::miniSharp::ParserASTRoot>>& roots, llvm::LLVMContext * context)
		{
			for (auto&&root : roots)
			{
				VerificationTypeInference(manager, *root);
			}
		}

		void VerificationTypeInferenceVisitor::Visit(ArrayIdentifierObject * node)
		{
			for (auto&&number : node->numbers)
			{
				VerificationTypeInferenceExpressionVisitor visitor(manager);
				number->Accept(&visitor);
				auto type = manager->GetTypeInferenceMapByLLVMType(number.get());
				if (!type->isIntegerTy(32))
				{
					throw ztl_exception(L"type no match in array index");
				}
			}
		}

}
}