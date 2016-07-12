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
		struct CollectIdentifierSymbolDefineStatementVisitor;
		struct CollectIdentifierSymbolDefineExpressionVisitor;

		struct CollectIdentifierSymbolDefineTypeDefineVisitor :public ztl::miniSharp::AstTypeDefine::IVisitor
		{
			CollectIdentifierSymbolDefineTypeDefineVisitor(MiniSharpSymbolManager* _manager, MiniSharpPasrseSymbol* _scope) :manager(_manager), scope(_scope)
			{

			}

			virtual void		Visit(AstTypeDefine*) override
			{
				assert(false);
			}
			virtual void		Visit(AstNamesapceDefine* node)override
			{
				auto symbol = manager->AddNamespaceDefSymbol(node->name, scope);
				manager->AddTypeDefToAstMap(symbol, node);
				manager->AddAstToTypeDefMap(symbol, node);
				scope = symbol;
				for (auto&&member : node->members)
				{
					member->define->Accept(this);
				}
				scope = scope->GetParent();
			}
			virtual void		Visit(AstClassTypeDefine* node)override
			{
				auto symbol = manager->AddClassTypeDefSymbol(node->name, scope);
				manager->AddTypeDefToAstMap(symbol, node);
				manager->AddAstToTypeDefMap(symbol, node);
				scope = symbol;
				for (auto&&member : node->members)
				{
					member->define->Accept(this);
				}
				scope = scope->GetParent();
			}
			void Visit(EnumFieldTypeDefine* node, int& count, MiniSharpPasrseSymbol* parent)
			{
				int value;
				if (node->value.empty())
				{
					value = count;
					count += 1;
				}
				else
				{
					value = std::stoi(node->value);
					count = value + 1;
				}
				auto fieldSymbol = manager->AddEnumFieldDefSymbol(node->name, parent);
				fieldSymbol->AdditionData(value);
			}
			virtual void		Visit(AstEnumTypeDefine* node)override
			{
				auto symbol = manager->AddEnumTypeDefSymbol(node->name, scope);
				manager->AddTypeDefToAstMap(symbol, node);
				manager->AddAstToTypeDefMap(symbol, node);
				auto count = 0;
				for (auto&&member : node->members)
				{
					Visit(member.get(), count, symbol);
				}
			}
			virtual void		Visit(AstInterfaceTypeDefine*)override
			{
				ztl_exception(L"not support interface");
				/*auto symbol = manager->AddInterfaceTypeDefSymbol(node->name, scope);
				manager->AddTypeDefToAstMap(symbol, node);
				manager->AddAstToTypeDefMap(symbol, node);
				for (auto&&member : node->members)
				{
				Visit(member->sign.get(), symbol);
				}*/
			}
			void Visit(AstFunctionParameter* node, MiniSharpPasrseSymbol* parent)
			{
				manager->AddFunctionParameterDefSymbol(node->name, parent);
			}
			MiniSharpPasrseSymbol*  Visit(FunctionSignature* node, MiniSharpPasrseSymbol* parent)
			{
				auto symbol = manager->AddFunctionDefSymbol(node->name, parent);

				for (auto&& parameter : node->parameters)
				{
					Visit(parameter.get(), symbol);
				}
				return symbol;
			}

			virtual void		Visit(AstFunctionTypeDefine*)override;
			virtual void		Visit(AstFieldTypeDefine* node)
			{
				auto symbol = manager->AddClassFiledDefSymbol(node->name, scope);
				manager->AddTypeDefToAstMap(symbol, node);
				manager->AddAstToTypeDefMap(symbol, node);
			}

			MiniSharpSymbolManager* manager;
			MiniSharpPasrseSymbol* scope;
		};

		struct CollectIdentifierSymbolDefineExpressionVisitor :public ztl::miniSharp::AstExpression::IVisitor
		{
			CollectIdentifierSymbolDefineExpressionVisitor(MiniSharpSymbolManager* _manager, MiniSharpPasrseSymbol* _scope) :manager(_manager), scope(_scope)
			{

			}
			virtual void		Visit(AstExpression*)override { assert(false); }
			virtual void		Visit(CastExpression* node) override 
			{
				node->expression->Accept(this);
			}
			virtual void		Visit(AttachSuffixExpression* node) override 
			{
				node->expression->Accept(this);
			}
			virtual void		Visit(AttachPrefixExpression* node) override 
			{
				node->expression->Accept(this);
			}
			virtual void		Visit(LogicExpression* )override 
			{
				assert(false);
			}
			virtual void		Visit(LogicAndExpression* node)override 
			{
				node->left->Accept(this);
				node->right->Accept(this);
			}
			virtual void		Visit(LogicOrExpression* node) override
			{
				node->left->Accept(this);
				node->right->Accept(this);
			}
			virtual void		Visit(CompareExpression*node)override 
			{
				node->left->Accept(this);
				node->right->Accept(this);
			}
			virtual void		Visit(ArithmeticExpression*)override 
			{
				assert(false);
			}
			virtual void		Visit(LowArithmeticExpression* node) override 
			{
				node->left->Accept(this);
				node->right->Accept(this);
			}
			virtual void		Visit(HighArithmeticExpression* node)override 
			{
				node->left->Accept(this);
				node->right->Accept(this);
			}
			virtual void		Visit(AssignExpression* node)override 
			{
				node->right->Accept(this);
			}
			virtual void		Visit(AstUsingDeclExpression*node)override
			{
				if (!node->name.empty())
				{
					manager->AddUsingAliasDefSymbol(node->name, scope);
				}
			}
			virtual void		Visit(NewExpression*node)override 
			{
				for (auto&& arg : node->args)
				{
					arg->Accept(this);
				}
			}
			
			virtual void		Visit(FunctionCallExpression* node) override 
			{
				for (auto&& arg : node->args)
				{
					arg->Accept(this);
				}
			}
			virtual void		Visit(DeclarationExpression* node) override
			{
				for (auto&& expression : node->expressions)
				{
					auto&& expr = std::dynamic_pointer_cast<ztl::miniSharp::AssignExpression>(expression);
					assert(expr != nullptr);
					auto&& normal = std::dynamic_pointer_cast<ztl::miniSharp::NormalIdentifierObject>(expr->variable);
					assert(normal != nullptr);
					manager->AddVariableDefSymbol(normal->name, scope);
					expression->Accept(this);
				}
			}
			virtual void		Visit(LiteralExpression*) override { assert(false); }
			virtual void		Visit(CharExpression* node)override 
			{
				auto symbol = manager->AddConstantsDefSymbol(L"", scope);
				wchar_t value = node->value[0];
				assert(node->value.size() == 1);
				symbol->AdditionData(value);
				manager->AddLiteralSymbolMap(node, symbol);
			}
			virtual void		Visit(StringExpression* node) override
			{
				auto symbol = manager->AddConstantsDefSymbol(L"", scope);
				symbol->AdditionData(node->value);
				manager->AddLiteralSymbolMap(node, symbol);
			}
			virtual void		Visit(NullExpression* node) override
			{
				auto symbol = manager->AddConstantsDefSymbol(L"", scope);
				symbol->AdditionData(0);
				manager->AddLiteralSymbolMap(node, symbol);

			}
			virtual void		Visit(BoolExpression* node) override 
			{
				auto symbol = manager->AddConstantsDefSymbol(L"", scope);
				bool value=true;
				if (node->value == L"TRUEVALUE")
				{
					value = true;
				}
				else if(node->value == L"FALSEVALUE")
				{
					value = false;
				}
				else
				{
					assert(false);
				}
				symbol->AdditionData(value);
				manager->AddLiteralSymbolMap(node, symbol);

			}
			virtual void		Visit(DoubleExpression* node)override 
			{
				auto symbol = manager->AddConstantsDefSymbol(L"", scope);
				auto value = std::stod(node->value);
				symbol->AdditionData(value);
				manager->AddLiteralSymbolMap(node, symbol);

			}
			virtual void		Visit(IntegerExpression* node)override 
			{
				auto symbol = manager->AddConstantsDefSymbol(L"", scope);
				auto value = std::stoi(node->value);
				symbol->AdditionData(value);
				manager->AddLiteralSymbolMap(node, symbol);

			}
			virtual void		Visit(VariableExpression*)override {}
		

			MiniSharpSymbolManager* manager;
			MiniSharpPasrseSymbol* scope;

		};
		struct CollectIdentifierSymbolDefineStatementVisitor : public ztl::miniSharp::AstStatement::IVisitor
		{
			CollectIdentifierSymbolDefineStatementVisitor(MiniSharpSymbolManager* _manager, MiniSharpPasrseSymbol* _scope) :manager(_manager), scope(_scope)
			{

			}

			virtual void		Visit(AstStatement*) override
			{
				assert(false);
			}
			virtual void		Visit(UsingStatement* node) override
			{
				auto&&symbol = manager->AddUsingDefSymbol(L"", scope);
				manager->AddAnonymousScopeMap(node, symbol);

				CollectIdentifierSymbolDefineExpressionVisitor visitor(manager, symbol);
				node->source->Accept(&visitor);
				{
					scope = symbol;
					auto body = std::dynamic_pointer_cast<ScopeStatement>(node->body);
					for (auto&&statement : body->statements)
					{
						statement->Accept(this);
					}
					scope = scope->GetParent();
				}
			}
			virtual void		Visit(ContinueStatement*)override {}
			virtual void		Visit(BreakStatement*)override {}
			virtual void		Visit(ReturnStatement*)override {}
			virtual void		Visit(DefaultStatement*node)override
			{
				auto defaultScopeSymbol = manager->AddScopeDefSymbol(L"", scope);
				manager->AddAnonymousScopeMap(node, defaultScopeSymbol);
				scope = defaultScopeSymbol;
				for (auto&&statement : node->statements)
				{
					statement->Accept(this);
				}
				scope = scope->GetParent();
			}
			virtual void		Visit(SwitchStatement* node) override
			{
				{
					CollectIdentifierSymbolDefineExpressionVisitor visitor(manager, scope);
					node->conditional->Accept(&visitor);
				}

				for (auto&& caser : node->cases)
				{
					caser->Accept(this);
				}

				{
					node->default->Accept(this);
				}
			}
			virtual void		Visit(CaseStatement* node) override
			{
				auto symbol = manager->AddCaseDefSymbol(L"", scope);
				manager->AddAnonymousScopeMap(node, symbol);

				CollectIdentifierSymbolDefineStatementVisitor visitor(manager, symbol);
				for (auto&& statment : node->statements)
				{
					statment->Accept(&visitor);
				}
			}
			virtual void		Visit(ForStatement* node) override
			{
				auto symbol = manager->AddScopeDefSymbol(L"", scope);
				manager->AddAnonymousScopeMap(node, symbol);

				scope = symbol;
				node->initExpression->Accept(this);
				auto body = std::dynamic_pointer_cast<ScopeStatement>(node->body);
				for (auto&&statement : body->statements)
				{
					statement->Accept(this);
				}
				scope = scope->GetParent();
			}
			virtual void		Visit(WhileStatement* node)override
			{
				node->body->Accept(this);

			}
			virtual void		Visit(IfStatement* node)override
			{
				CollectIdentifierSymbolDefineExpressionVisitor visitor(manager, scope);
				node->conditional->Accept(&visitor);

				if (node->truePath != nullptr)
				{
					node->truePath->Accept(this);
				}

				if (node->falsePath != nullptr)
				{
					node->falsePath->Accept(this);
				}

			}
			virtual void		Visit(ScopeStatement* node) override
			{
				auto symbol = manager->AddScopeDefSymbol(L"", scope);
				manager->AddAnonymousScopeMap(node, symbol);

				scope = symbol;

				for (auto&& statement : node->statements)
				{
					statement->Accept(this);
				}
				scope = scope->GetParent();
			}
			virtual void		Visit(ExpressionStatement* node) override
			{
				CollectIdentifierSymbolDefineExpressionVisitor visitor(manager, scope);
				node->expression->Accept(&visitor);
			}

			MiniSharpSymbolManager* manager;
			MiniSharpPasrseSymbol* scope;
		};
		inline void CollectIdentifierSymbolDefineTypeDefineVisitor::Visit(AstFunctionTypeDefine * node)
		{
			auto symbol = Visit(node->sign.get(), scope);
			manager->AddTypeDefToAstMap(symbol, node);
			manager->AddAstToTypeDefMap(symbol, node);
			CollectIdentifierSymbolDefineStatementVisitor visitor(manager, symbol);
			auto body = std::dynamic_pointer_cast<ScopeStatement>(node->body);
			for (auto&&statement : body->statements)
			{
				statement->Accept(&visitor);
			}
		}


		//建立具名，匿名作用域
		//收集各种符号定义
		//收集AST到typeDef的映射
		//收集AstStatement*到Symbol的映射
		//enum value 作为附加data
		//收集字面值的ASTExpression到symbol的映射
		void CollectIdentifierSymbolDefine(MiniSharpSymbolManager * manager, const ztl::miniSharp::ParserASTRoot& root)
		{
			for (auto&& member : root.members)
			{
				CollectIdentifierSymbolDefineTypeDefineVisitor visitor(manager, manager->GetGlobalSymbol());
				member->define->Accept(&visitor);
			}

		}
		void ztl::miniSharp::CollectIdentifierSymbolDefine(MiniSharpSymbolManager* manager, const vector<shared_ptr<ztl::miniSharp::ParserASTRoot>>& roots)
		{
			for (auto&&root : roots)
			{
				CollectIdentifierSymbolDefine(manager, *root);
			}
		}
	}
}