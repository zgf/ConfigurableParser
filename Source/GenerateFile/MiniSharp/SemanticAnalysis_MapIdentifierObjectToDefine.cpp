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
		struct MapIdentifierIdentifierObjectVisitor :public ztl::miniSharp::IdentifierObject::IVisitor
		{
			MapIdentifierIdentifierObjectVisitor(MiniSharpSymbolManager* _manager, MiniSharpPasrseSymbol* current, vector<MiniSharpPasrseSymbol*>& scope) :manager(_manager), scope(current), otherScopes(scope)
			{

			}

			virtual void		Visit(IdentifierObject* node) override;
			virtual void		Visit(ThisIdentifierObject* node) override;
			virtual void		Visit(SubIdentifierObject* node) override;
			virtual void		Visit(ArrayIdentifierObject* node) override;
			virtual void		Visit(NormalIdentifierObject* node) override;
			virtual void		Visit(PrimaryIdentifierObject* node) override;

			MiniSharpSymbolManager* manager;
			MiniSharpPasrseSymbol* scope;
			vector<MiniSharpPasrseSymbol*>& otherScopes;
		};

		struct MapIdentifierTypeDefineVisitor :public ztl::miniSharp::AstTypeDefine::IVisitor
		{
			MapIdentifierTypeDefineVisitor(MiniSharpSymbolManager* _manager, MiniSharpPasrseSymbol* current, vector<MiniSharpPasrseSymbol*>& scope) :manager(_manager), scope(current), otherScopes(scope)
			{

			}
			virtual void		Visit(AstTypeDefine* node) override;
			virtual void		Visit(AstNamesapceDefine* node) override;
			virtual void		Visit(AstClassTypeDefine* node) override;
			virtual void		Visit(AstEnumTypeDefine* node) override;
			virtual void		Visit(AstInterfaceTypeDefine*) override;
			virtual void		Visit(AstFunctionTypeDefine* node) override;
			virtual void		Visit(AstFieldTypeDefine* node) override;

			MiniSharpSymbolManager* manager;
			MiniSharpPasrseSymbol* scope;
			vector<MiniSharpPasrseSymbol*>& otherScopes;
		};

		struct MapIdentifierStatementVisitor :public ztl::miniSharp::AstStatement::IVisitor
		{
			MapIdentifierStatementVisitor(MiniSharpSymbolManager* _manager, MiniSharpPasrseSymbol* current, vector<MiniSharpPasrseSymbol*>& scope)
				:manager(_manager), scope(current), otherScopes(scope)
			{

			}
			virtual void		Visit(AstStatement* node) override;
			virtual void		Visit(UsingStatement* node) override;
			virtual void		Visit(ContinueStatement* node) override;
			virtual void		Visit(BreakStatement* node) override;
			virtual void		Visit(ReturnStatement* node) override;
			virtual void		Visit(SwitchStatement* node) override;
			virtual void		Visit(CaseStatement* node) override;
			virtual void		Visit(ForStatement* node) override;
			virtual void		Visit(WhileStatement* node) override;
			virtual void		Visit(IfStatement* node) override;
			virtual void		Visit(ScopeStatement* node) override;
			virtual void		Visit(ExpressionStatement* node) override;
			virtual void		Visit(DefaultStatement* node) override;


			MiniSharpSymbolManager* manager;
			MiniSharpPasrseSymbol* scope;
			vector<MiniSharpPasrseSymbol*>& otherScopes;
		};
		struct MapIdentifierExpressionVisitor :public ztl::miniSharp::AstExpression::IVisitor
		{
			MapIdentifierExpressionVisitor(MiniSharpSymbolManager* _manager, MiniSharpPasrseSymbol* current, vector<MiniSharpPasrseSymbol*>& scope)
				:manager(_manager), scope(current), otherScopes(scope)
			{

			}
			virtual void		Visit(AstExpression* node) override;
			virtual void		Visit(CastExpression* node) override;
			virtual void		Visit(AttachSuffixExpression* node) override;
			virtual void		Visit(AttachPrefixExpression* node) override;
			virtual void		Visit(LogicExpression* node) override;
			virtual void		Visit(LogicAndExpression* node) override;
			virtual void		Visit(LogicOrExpression* node) override;
			virtual void		Visit(CompareExpression* node) override;
			virtual void		Visit(ArithmeticExpression* node) override;
			virtual void		Visit(LowArithmeticExpression* node) override;
			virtual void		Visit(HighArithmeticExpression* node) override;
			virtual void		Visit(AssignExpression* node) override;
			virtual void		Visit(NewExpression* node) override;
			virtual void		Visit(FunctionCallExpression* node) override;
			virtual void		Visit(DeclarationExpression* node) override;
			virtual void		Visit(LiteralExpression* node) override;
			virtual void		Visit(CharExpression* node) override;
			virtual void		Visit(StringExpression* node) override;
			virtual void		Visit(NullExpression* node) override;
			virtual void		Visit(BoolExpression* node) override;
			virtual void		Visit(DoubleExpression* node) override;
			virtual void		Visit(IntegerExpression* node) override;
			virtual void		Visit(VariableExpression* node) override;
			virtual void		Visit(AstUsingDeclExpression* node) override;

			MiniSharpSymbolManager* manager;
			MiniSharpPasrseSymbol* scope;
			vector<MiniSharpPasrseSymbol*>& otherScopes;
		};

		inline void		MapIdentifierIdentifierObjectVisitor::Visit(IdentifierObject*) { assert(false); }
		inline void		MapIdentifierIdentifierObjectVisitor::Visit(ThisIdentifierObject* node)
		{
			/*if (!scope->isFunctionDef() ||
				scope->GetParent() == nullptr ||
				!scope->GetParent()->isClassTypeDef())
			{
				throw ztl_exception(L"this only used in class member function");
			}*/
			//all function was class member function
			auto symbol = scope->GetParent();
			manager->AddIdentifierObjectToDefineMap(node, symbol);
			scope = symbol;
		}

		inline void		MapIdentifierIdentifierObjectVisitor::Visit(ArrayIdentifierObject* node)
		{
			node->element->Accept(this);
			auto elementSymbol = manager->GetIdentifierDefByIdentifierObject(node->element.get());
			for (auto&&number : node->numbers)
			{
				elementSymbol = manager->AddArrayTypeSymbol(L"", nullptr, elementSymbol);
			}
			//A.B[33].C
			manager->AddIdentifierObjectToDefineMap(node, elementSymbol);
		}

		MiniSharpPasrseSymbol* CheckRedefineSymbol(NormalIdentifierObject* node, const vector<MiniSharpPasrseSymbol*>& scopes)
		{
			vector<MiniSharpPasrseSymbol*> result;
			for (auto&&iter : scopes)
			{
				auto symbol = iter->GetUniqueSymbolByName(node->name);
				if (symbol != nullptr)
				{
					if (symbol->isTypeDefSymbol()||symbol->isNamespaceDef())
					{
						result.emplace_back(symbol);
					}
					else if(symbol->isVariableDef()|| symbol->isClassFiledDef())
					{
						result.emplace_back(symbol->GetDescription());
					}
					
				}
			}
			if (result.size() > 1)
			{
				throw ztl_exception(L"symbol redefine!");
			}

			if (result.empty())
			{
				throw ztl_exception(L"can't find this symbol in all using and current scope");
			}
			return result.back();
		}



		MiniSharpPasrseSymbol* CheckRedefineSymbol(NormalIdentifierObject* node, MiniSharpPasrseSymbol* parent)
		{
			auto symbol = parent->SearchUniqueSymbolByName(node->name);
			if (symbol == nullptr)
			{
				throw ztl_exception(L"can't find this sub symbol in current scope");
			}
			return symbol;
		}
		inline void		MapIdentifierIdentifierObjectVisitor::Visit(NormalIdentifierObject* node)
		{
			MiniSharpPasrseSymbol* symbol = nullptr;
			if (scope == nullptr)
			{
				symbol = scope->SearchUniqueSymbolByName(node->name);
				if (symbol == nullptr)
				{
					symbol = CheckRedefineSymbol(node, otherScopes);
				}
				scope = symbol;
			}
			else
			{
				symbol = CheckRedefineSymbol(node, scope);
				scope = symbol;
			}
			assert(symbol != nullptr);
			manager->AddIdentifierObjectToDefineMap(node, symbol);
		}
		inline void		MapIdentifierIdentifierObjectVisitor::Visit(PrimaryIdentifierObject* node)
		{
			using actionType = void(*)(MiniSharpSymbolManager* manager, PrimaryIdentifierObject* node);
			static unordered_map<wstring, actionType> action =
			{
				{
					L"int",
					[](MiniSharpSymbolManager* manager, PrimaryIdentifierObject* node)
			{
				assert(manager->GetIntTypeDefSymbol() != nullptr);
				manager->AddIdentifierObjectToDefineMap(node, manager->GetIntTypeDefSymbol());
			}
				},
				{
					L"double",
					[](MiniSharpSymbolManager* manager, PrimaryIdentifierObject* node)
			{
				assert(manager->GetDoubleTypeDefSymbol() != nullptr);
				manager->AddIdentifierObjectToDefineMap(node, manager->GetDoubleTypeDefSymbol());
			}
				},
				{
					L"string",
					[](MiniSharpSymbolManager* manager, PrimaryIdentifierObject* node)
			{
				assert(manager->GetStringTypeDefSymbol() != nullptr);
				manager->AddIdentifierObjectToDefineMap(node, manager->GetStringTypeDefSymbol());
			}
				},
				{
					L"char",
					[](MiniSharpSymbolManager* manager, PrimaryIdentifierObject* node)
			{
				assert(manager->GetUnicodeCharTypeDefSymbol() != nullptr);
				manager->AddIdentifierObjectToDefineMap(node, manager->GetUnicodeCharTypeDefSymbol());
			}
				},
				{
					L"bool",
					[](MiniSharpSymbolManager* manager, PrimaryIdentifierObject* node)
			{
				assert(manager->GetBoolTypeDefSymbol() != nullptr);
				manager->AddIdentifierObjectToDefineMap(node, manager->GetBoolTypeDefSymbol());
			}
				},
				{
					L"void",
					[](MiniSharpSymbolManager* manager, PrimaryIdentifierObject* node)
			{
				assert(manager->GetVoidTypeDefSymbol() != nullptr);
				manager->AddIdentifierObjectToDefineMap(node, manager->GetVoidTypeDefSymbol());
			}
				},
			};

			auto findIter = action.find(node->primary);
			if (findIter == action.end())
			{
				throw ztl_exception(L"unkown op symbol");
			}
			findIter->second(manager, node);
		}

		inline void		MapIdentifierIdentifierObjectVisitor::Visit(SubIdentifierObject* node)
		{
			node->scope->Accept(this);
			node->right->Accept(this);
			auto symbol = manager->GetIdentifierDefByIdentifierObject(node->right.get());
			manager->AddIdentifierObjectToDefineMap(node, symbol);
		}


		inline void		MapIdentifierTypeDefineVisitor::Visit(AstTypeDefine*) { assert(false); }
		inline void		MapIdentifierTypeDefineVisitor::Visit(AstNamesapceDefine* node)
		{
			scope = scope->GetUniqueSymbolByName(node->name);
			for (auto&&member : node->members)
			{
				member->define->Accept(this);
			}
			scope = scope->GetParent();
		}
		inline void		MapIdentifierTypeDefineVisitor::Visit(AstClassTypeDefine* node)
		{
			if (!node->parentInterfaces.empty())
			{
				throw ztl_exception(L"not support interface");
			}

			if (node->parent != nullptr)
			{
				MapIdentifierIdentifierObjectVisitor visitor(manager, scope, otherScopes);
				node->parent->Accept(&visitor);
				auto parentTypeSymbol = manager->GetIdentifierDefByIdentifierObject(node->parent.get());
				auto classSymbol = manager->GetTypeDefSymbolByAstNode(node);
				classSymbol->SetDescription(parentTypeSymbol);
			}

			scope = scope->GetUniqueSymbolByName(node->name);
			for (auto&&member : node->members)
			{
				member->define->Accept(this);
			}
			scope = scope->GetParent();
		}
		inline void		MapIdentifierTypeDefineVisitor::Visit(AstEnumTypeDefine*) {}
		void Visit(FunctionSignature* sign, MiniSharpPasrseSymbol* symbol, MiniSharpSymbolManager* manager, vector<MiniSharpPasrseSymbol*>& otherScopes)
		{
			assert(symbol->isFunctionDef());
			for (auto&&parameter : sign->parameters)
			{
				MapIdentifierIdentifierObjectVisitor visitor(manager, symbol, otherScopes);
				parameter->type->Accept(&visitor);
				auto paramSymbol = symbol->GetUniqueSymbolByName(parameter->name);
				auto paramTypeSymbol = manager->GetIdentifierDefByIdentifierObject(parameter->type.get());
				paramSymbol->AdditionData(parameter->name);
				paramSymbol->SetDescription(paramTypeSymbol);
				symbol->AddArgs(paramSymbol);
			}

				if (sign->returnType != nullptr)
				{
					MapIdentifierIdentifierObjectVisitor visitor(manager, symbol->GetParent(), otherScopes);
					sign->returnType->Accept(&visitor);
					auto returnTypeSymbol = manager->GetIdentifierDefByIdentifierObject(sign->returnType.get());
					symbol->SetDescription(returnTypeSymbol);
				}
				else
				{
					auto classTypeSymbol = symbol->GetParent();
					assert(classTypeSymbol->isClassTypeDef());
					symbol->SetDescription(classTypeSymbol);
				}
			//	auto uniqueFunctionName = GetFunctionTypeDefSymbolUniqueLLVMTypeName(symbol);
				//symbol->SetName(uniqueFunctionName);
				symbol->GetParent()->AddUniqueSubSymbol(symbol);
		}
		inline void		MapIdentifierTypeDefineVisitor::Visit(AstInterfaceTypeDefine*)
		{
			throw ztl_exception(L"not support");
			/*for (auto&&parent : node->parents)
			{
			MapIdentifierIdentifierObjectVisitor visitor(manager, scope, otherScopes);
			parent->Accept(&visitor);
			}

			for (auto&&member : node->members)
			{
			auto symbol = scope->GetUniqueSymbolByName(node->name);
			ztl::miniSharp::Visit(member->sign.get(), symbol, manager, otherScopes);
			}*/
		}
		inline void		MapIdentifierTypeDefineVisitor::Visit(AstFunctionTypeDefine* node)
		{
			auto symbol = manager->GetTypeDefSymbolByAstNode(node);
			assert(symbol->isFunctionDef());
			ztl::miniSharp::Visit(node->sign.get(), symbol, manager, otherScopes);
			scope = symbol;
			for (auto&&element : node->body->statements)
			{
				MapIdentifierStatementVisitor visitor(manager, scope, otherScopes);
				element->Accept(&visitor);
			}
			scope = scope->GetParent();
			
		}
		inline void		MapIdentifierTypeDefineVisitor::Visit(AstFieldTypeDefine* node)
		{
			MapIdentifierIdentifierObjectVisitor visitor(manager, scope, otherScopes);
			node->type->Accept(&visitor);

			auto fieldTypeSymbol = manager->GetIdentifierDefByIdentifierObject(node->type.get());
			auto fieldSymbol = manager->GetTypeDefSymbolByAstNode(node);
			fieldSymbol->SetDescription(fieldTypeSymbol);
		}


		inline void		MapIdentifierStatementVisitor::Visit(AstStatement*) { assert(false); }
		inline void		MapIdentifierStatementVisitor::Visit(UsingStatement* node)
		{
			auto symbol = manager->GetAnonymousScopeSymbol(node);
			assert(symbol != nullptr);
			{
				MapIdentifierExpressionVisitor visitor(manager, symbol, otherScopes);
				node->source->Accept(&visitor);
			}
			scope = symbol;
			node->body->Accept(this);
			scope = scope->GetParent();

		}
		inline void		MapIdentifierStatementVisitor::Visit(ContinueStatement*) {}
		inline void		MapIdentifierStatementVisitor::Visit(BreakStatement*) {}
		inline void		MapIdentifierStatementVisitor::Visit(ReturnStatement* node)
		{
			MapIdentifierExpressionVisitor visitor(manager, scope, otherScopes);
			node->expression->Accept(&visitor);
		}
		inline void		MapIdentifierStatementVisitor::Visit(SwitchStatement* node)
		{
			{
				MapIdentifierExpressionVisitor visitor(manager, scope, otherScopes);
				node->conditional->Accept(&visitor);
			}
			for (auto&& caser : node->cases)
			{
				caser->Accept(this);
			}
			node->default->Accept(this);
		}
		inline void		MapIdentifierStatementVisitor::Visit(CaseStatement* node)
		{
			scope = manager->GetAnonymousScopeSymbol(node);
			for (auto&&statement : node->statements)
			{
				statement->Accept(this);
			}
			scope = scope->GetParent();
		}
		inline void		MapIdentifierStatementVisitor::Visit(DefaultStatement* node)
		{
			scope = manager->GetAnonymousScopeSymbol(node);
			for (auto&&statement : node->statements)
			{
				statement->Accept(this);
			}
			scope = scope->GetParent();
		}

		inline void		MapIdentifierStatementVisitor::Visit(ForStatement* node)
		{

			scope = manager->GetAnonymousScopeSymbol(node);
			{
				node->initExpression->Accept(this);
			}
			{
				MapIdentifierExpressionVisitor visitor(manager, scope, otherScopes);
				node->conditional->Accept(&visitor);
			}

			{
				for (auto&&expression : node->stepExpressions)
				{
					MapIdentifierExpressionVisitor visitor(manager, scope, otherScopes);
					expression->Accept(&visitor);
				}
			}
			{
				for (auto&&statement : node->body->statements)
				{
					statement->Accept(this);
				}
				
			}
			scope = scope->GetParent();
		}
		inline void		MapIdentifierStatementVisitor::Visit(WhileStatement* node)
		{
			scope = manager->GetAnonymousScopeSymbol(node);

			{
				MapIdentifierExpressionVisitor visitor(manager, scope, otherScopes);
				node->conditional->Accept(&visitor);
			}

			{
				for (auto&&statement : node->body->statements)
				{
					statement->Accept(this);
				}
			}

			scope = scope->GetParent();
		}

		inline void		MapIdentifierStatementVisitor::Visit(IfStatement* node)
		{
			{
				MapIdentifierExpressionVisitor visitor(manager, scope, otherScopes);
				node->conditional->Accept(&visitor);
			}
			scope = manager->GetAnonymousScopeSymbol(node->truePath.get());
			{
				node->truePath->Accept(this);
			}
			scope = scope->GetParent();
			scope = manager->GetAnonymousScopeSymbol(node->falsePath.get());
			{
				node->falsePath->Accept(this);
			}
			scope = scope->GetParent();
		}
		inline void		MapIdentifierStatementVisitor::Visit(ScopeStatement* node)
		{
			scope = manager->GetAnonymousScopeSymbol(node);
			for (auto&& statement : node->statements)
			{
				statement->Accept(this);
			}
			scope = scope->GetParent();
		}
		inline void	MapIdentifierStatementVisitor::Visit(ExpressionStatement* node)
		{
			MapIdentifierExpressionVisitor visitor(manager, scope, otherScopes);
			node->expression->Accept(&visitor);
		}


		inline void		MapIdentifierExpressionVisitor::Visit(AstExpression*) { assert(false); }
		inline void		MapIdentifierExpressionVisitor::Visit(CastExpression* node)
		{
			{
				MapIdentifierIdentifierObjectVisitor visitor(manager, scope, otherScopes);
				node->type->Accept(&visitor);
			}
			{
				node->expression->Accept(this);
			}
		}
		inline void		MapIdentifierExpressionVisitor::Visit(AttachSuffixExpression* node)
		{
			node->expression->Accept(this);
		}
		inline void		MapIdentifierExpressionVisitor::Visit(AttachPrefixExpression* node)
		{
			node->expression->Accept(this);
		}
		inline void		MapIdentifierExpressionVisitor::Visit(LogicExpression*) { assert(false); }
		inline void		MapIdentifierExpressionVisitor::Visit(LogicAndExpression* node)
		{
			node->left->Accept(this);
			node->right->Accept(this);
		}
		inline void		MapIdentifierExpressionVisitor::Visit(LogicOrExpression* node)
		{
			node->left->Accept(this);
			node->right->Accept(this);
		}
		inline void		MapIdentifierExpressionVisitor::Visit(CompareExpression* node)
		{
			node->left->Accept(this);
			node->right->Accept(this);
		}
		inline void		MapIdentifierExpressionVisitor::Visit(ArithmeticExpression*)
		{
			assert(false);
		}
		inline void		MapIdentifierExpressionVisitor::Visit(LowArithmeticExpression* node)
		{
			node->left->Accept(this);
			node->right->Accept(this);
		}
		inline void		MapIdentifierExpressionVisitor::Visit(HighArithmeticExpression* node)
		{
			node->left->Accept(this);
			node->right->Accept(this);
		}
		inline void		MapIdentifierExpressionVisitor::Visit(AssignExpression* node)
		{
			{
				MapIdentifierIdentifierObjectVisitor visitor(manager, scope, otherScopes);
				node->variable->Accept(&visitor);
			}
			{
				node->right->Accept(this);
			}
		}
		inline void		MapIdentifierExpressionVisitor::Visit(NewExpression* node)
		{
			MapIdentifierIdentifierObjectVisitor visitor(manager, scope, otherScopes);
			node->type->Accept(&visitor);
			for (auto&& arg : node->args)
			{
				arg->Accept(this);
			}
		}
		inline void		MapIdentifierExpressionVisitor::Visit(FunctionCallExpression* node)
		{
			for (auto&& arg : node->args)
			{
				arg->Accept(this);
			}
			MapIdentifierIdentifierObjectVisitor visitor(manager, scope, otherScopes);
			node->name->Accept(&visitor);
		}
		inline void		MapIdentifierExpressionVisitor::Visit(DeclarationExpression* node)
		{
			
			{
				MapIdentifierIdentifierObjectVisitor visitor(manager, scope, otherScopes);
				node->type->Accept(&visitor);
			}
			auto variableTypeSymbol = manager->GetIdentifierDefByIdentifierObject(node->type.get());
				
			
			for (auto&&expression : node->expressions)
			{
				auto expr = std::dynamic_pointer_cast<AssignExpression>(expression);
				MapIdentifierIdentifierObjectVisitor visitor(manager, scope, otherScopes);
				expr->variable->Accept(&visitor);
				auto variableSymbol = manager->GetIdentifierDefByIdentifierObject(expr->variable.get());
				variableSymbol->SetDescription(variableTypeSymbol);
			}
		}
		inline void		MapIdentifierExpressionVisitor::Visit(LiteralExpression*) { assert(false); }
		inline void		MapIdentifierExpressionVisitor::Visit(CharExpression* node) 
		{
			auto symbol = manager->GetLiteralSymbolByAstExpression(node);
			symbol->SetDescription(manager->GetUnicodeCharTypeDefSymbol());
		}
		inline void		MapIdentifierExpressionVisitor::Visit(StringExpression* node) 
		{
			auto symbol = manager->GetLiteralSymbolByAstExpression(node);
			symbol->SetDescription(manager->GetStringTypeDefSymbol());
		}
		inline void		MapIdentifierExpressionVisitor::Visit(NullExpression* node) 
		{
			auto symbol = manager->GetLiteralSymbolByAstExpression(node);
			symbol->SetDescription(manager->GetIntTypeDefSymbol());
		}
		inline void		MapIdentifierExpressionVisitor::Visit(BoolExpression*node)
		{
			auto symbol = manager->GetLiteralSymbolByAstExpression(node);
			symbol->SetDescription(manager->GetBoolTypeDefSymbol());
		}
		inline void		MapIdentifierExpressionVisitor::Visit(DoubleExpression* node)
		{
			auto symbol = manager->GetLiteralSymbolByAstExpression(node);
			symbol->SetDescription(manager->GetDoubleTypeDefSymbol());
		}
		inline void		MapIdentifierExpressionVisitor::Visit(IntegerExpression*node)
		{
			auto symbol = manager->GetLiteralSymbolByAstExpression(node);
			symbol->SetDescription(manager->GetIntTypeDefSymbol());
		}
		inline void		MapIdentifierExpressionVisitor::Visit(VariableExpression* node)
		{
			MapIdentifierIdentifierObjectVisitor visitor(manager, scope, otherScopes);
			node->variable->Accept(&visitor);
		}
		inline void		MapIdentifierExpressionVisitor::Visit(AstUsingDeclExpression* node)
		{
			MapIdentifierIdentifierObjectVisitor visitor(manager, this->scope, this->otherScopes);
			node->module->Accept(&visitor);
			auto symbol = manager->GetIdentifierDefByIdentifierObject(node->module.get());
			if (!symbol->isClassTypeDef() && !symbol->isNamespaceDef())
			{
				throw ztl_exception(L"using space must be class or namespace");
			}
			manager->AddScopeSpaceSymbolMap(node, symbol);
			otherScopes.emplace_back(symbol);
		}
		vector<MiniSharpPasrseSymbol*> BindUsingSapceToSymbol(MiniSharpSymbolManager* manager, const ztl::miniSharp::ParserASTRoot& root)
		{
			vector<MiniSharpPasrseSymbol*> otherScopes;
			for (auto&&usingIter : root.usings)
			{

				MapIdentifierExpressionVisitor visitor(manager, manager->GetGlobalSymbol(), otherScopes);
				usingIter->Accept(&visitor);
			}
			return otherScopes;
		}
		//映射using名，变量名和类型名到变量定义和类型定义上去
		//处理desciption的问题。绑定description到实际的symbol上.
		//处理函数的绑定 推迟到类型推导结束 重整函数名
		void MapIdentifierObjectToDefine(MiniSharpSymbolManager* manager, const ztl::miniSharp::ParserASTRoot& root)
		{
			auto scopes = BindUsingSapceToSymbol(manager, root);

			for (auto&& member : root.members)
			{
				MapIdentifierTypeDefineVisitor visitor(manager, manager->GetGlobalSymbol(), scopes);
				member->define->Accept(&visitor);
			}
		}

		void MapIdentifierObjectToDefine(MiniSharpSymbolManager* manager, const vector<shared_ptr<ztl::miniSharp::ParserASTRoot>>& roots)
		{
			for (auto&&root : roots)
			{
				MapIdentifierObjectToDefine(manager, *root);
			}
		}

	}
}