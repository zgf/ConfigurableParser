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

		std::wstring GetTypeDefSymbolUniqueLLVMTypeName(ztl::miniSharp::MiniSharpPasrseSymbol* symbol)
		{
			wstring name;
			if (symbol->isTypeDefSymbol())
			{
				auto work = symbol;
				while (work!=nullptr)
				{
					name += work->GetName();
					work = work->GetParent();
				}
			}
			return name;
		}
		std::wstring GetTypeSymbolUniqueLLVMTypeName(ztl::miniSharp::MiniSharpPasrseSymbol* symbol)
		{
			wstring name;
			while (symbol->isArrayType())
			{
				name += L"array";
			}
			if (symbol->isTypeDefSymbol())
			{
				name = GetTypeDefSymbolUniqueLLVMTypeName(symbol) + name;
			}
			return name;
		}
		std::wstring GetFunctionTypeDefSymbolUniqueLLVMTypeName(ztl::miniSharp::MiniSharpPasrseSymbol* symbol)
		{
			assert(symbol->isFunctionDef());
			auto baseName = GetTypeDefSymbolUniqueLLVMTypeName(symbol);
			auto returnTypeSymbol = symbol->GetDescription();
			auto returnTypeName = GetTypeSymbolUniqueLLVMTypeName(returnTypeSymbol);
			baseName += L"." + returnTypeName;
			for (auto&&arg : symbol->GetArgs())
			{
				auto argTypeSymbol = arg->GetDescription();
				auto argName = GetTypeSymbolUniqueLLVMTypeName(argTypeSymbol);
				baseName += L"." + argName;
			}
			return baseName;
		}
		struct LLVMTypeObjectConstructVisitor :public ztl::miniSharp::AstTypeDefine::IVisitor
		{
			LLVMTypeObjectConstructVisitor(MiniSharpSymbolManager* _manager, MiniSharpPasrseSymbol* current, llvm::LLVMContext* _context) :manager(_manager), scope(current),context(_context)
			{

			}
			virtual void		Visit(AstTypeDefine* node) override;
			virtual void		Visit(AstNamesapceDefine* node) override;
			virtual void		Visit(AstClassTypeDefine* node) override;
			virtual void		Visit(AstEnumTypeDefine* node) override;
			virtual void		Visit(AstInterfaceTypeDefine* ) override;
			virtual void		Visit(AstFunctionTypeDefine* node) override;
			virtual void		Visit(AstFieldTypeDefine* node) override;
			MiniSharpPasrseSymbol* GetValueSymbol()
			{
				return value;
			}
			MiniSharpSymbolManager* manager;
			MiniSharpPasrseSymbol* scope;
			MiniSharpPasrseSymbol* value = nullptr;
			llvm::LLVMContext* context;
		};
		llvm::Type* GetLLVMTypeObject(MiniSharpPasrseSymbol* symbol,llvm::LLVMContext* context,MiniSharpSymbolManager* manager)
		{
			if (symbol->isArrayType())
			{
				auto elementType = GetLLVMTypeObject(symbol->GetDescription(), context, manager);
				return llvm::ArrayType::get(elementType, 20);
			}
			if (symbol->isInterfaceTypeDef())
			{
				throw ztl_exception(L"now not support interface type emit code");
			}
			if (symbol->GetLLVMTypeDef() == nullptr&&symbol->isTypeDefSymbol())
			{
				if (symbol->isPrimitiveTypeDefSymbol())
					{
						using actionType = llvm::Type*(*)(llvm::LLVMContext& context);
						static unordered_map<ztl::miniSharp::MiniSharpSymbolType, actionType> actions = 
						{
							{
								MiniSharpSymbolType::BoolTypeDef,
								[](llvm::LLVMContext& context)->llvm::Type*
								{
									return llvm::Type::getInt1Ty(context);
								}
							},
							{
								MiniSharpSymbolType::IntTypeDef,
								[](llvm::LLVMContext& context)->llvm::Type*
								{
									return llvm::Type::getInt32Ty(context);
								}
							},
							{
								MiniSharpSymbolType::DoubleTypeDef,
								[](llvm::LLVMContext& context)->llvm::Type*
								{
									return llvm::Type::getDoubleTy(context);
								}
							},
							{
								MiniSharpSymbolType::UnicodeCharTypeDef,
								[](llvm::LLVMContext& context)->llvm::Type*
								{
									return llvm::Type::getInt16Ty(context);
								}
							},
							{
								MiniSharpSymbolType::StringTypeDef,
								[](llvm::LLVMContext& context)->llvm::Type*
								{
									return llvm::Type::getInt16PtrTy(context);
								}
							},
							{
								MiniSharpSymbolType::VoidTypeDef,
								[](llvm::LLVMContext& context)->llvm::Type*
								{
									return llvm::Type::getVoidTy(context);
								}
							},
						};
						assert(actions.find(symbol->GetSymbolType()) != actions.end());
						return actions[symbol->GetSymbolType()](*context);
					}
					else
					{
						assert(symbol->isCustomTypeDefSymbol());
						LLVMTypeObjectConstructVisitor visitor(manager, symbol->GetParent(), context);
						auto astNode = manager->GetAstNodeByTypeDefSymbol(symbol);
						astNode->Accept(&visitor);
						auto valueSymbol = visitor.GetValueSymbol();
						return valueSymbol->GetLLVMTypeDef();
					}
			}
			
			return symbol->GetLLVMTypeDef();
		}
		inline void		LLVMTypeObjectConstructVisitor::Visit(AstTypeDefine* ) 
		{
			assert(false);
		}
		inline void		LLVMTypeObjectConstructVisitor::Visit(AstNamesapceDefine* node) 
		{
			for (auto&& member : node->members)
			{
				this->scope = manager->GetTypeDefSymbolByAstNode(node);
				member->define->Accept(this);
				scope->GetParent();
			}
		}
		inline void		LLVMTypeObjectConstructVisitor::Visit(AstClassTypeDefine* node) 
		{
			auto symbol = scope->GetUniqueSymbolByName(node->name);
			vector<llvm::Type*> elements;
			if (node->parent != nullptr)
			{
				//单基类忽略接口继承
				auto parentTypeSymbol = manager->GetIdentifierDefByIdentifierObject(node->parent.get());
				assert(parentTypeSymbol->isClassTypeDef());
				auto parentType = GetLLVMTypeObject(parentTypeSymbol, context, manager);
				elements.emplace_back(parentType);
			}
			
			for (auto&& member : node->members)
			{
				auto result = std::dynamic_pointer_cast<AstFieldTypeDefine>(member->define);
				if (result != nullptr)
				{
					//is class field
					auto visitor = *this;
					visitor.scope = symbol;
					result->Accept(&visitor);
					elements.emplace_back(visitor.GetValueSymbol()->GetLLVMTypeDef());
				}
			}
			auto name = GetTypeDefSymbolUniqueLLVMTypeName(symbol);
			llvm::StructType* llvmTypeDef = llvm::StructType::create(*context, elements, ztl::algorithm::ws2s(name));
			symbol->SetLLVMTypeDef(llvmTypeDef);
			this->value = symbol;


			for (auto&& member : node->members)
			{
				auto result = std::dynamic_pointer_cast<AstFunctionTypeDefine>(member->define);
				if (result != nullptr)
				{
					//is function
					auto visitor = *this;
					visitor.scope = symbol;
					result->Accept(&visitor);
				}
			}
		}
		inline void		LLVMTypeObjectConstructVisitor::Visit(AstEnumTypeDefine* node) 
		{
			auto intType = llvm::IntegerType::get(*context, 32);
			auto symbol = scope->GetUniqueSymbolByName(node->name);
			auto name = GetTypeDefSymbolUniqueLLVMTypeName(symbol);
			llvm::StructType* llvmTypeDef = llvm::StructType::create(*context, { intType }, ztl::algorithm::ws2s(name));
			symbol->SetLLVMTypeDef(llvmTypeDef);
			this->value = symbol;
		}
		inline void		LLVMTypeObjectConstructVisitor::Visit(AstInterfaceTypeDefine* ) 
		{
			throw ztl_exception(L"now not support interface type emit code");
		}
		inline void		LLVMTypeObjectConstructVisitor::Visit(AstFunctionTypeDefine* node) 
		{
			auto functionTypeSymbol = manager->GetTypeDefSymbolByAstNode(node);
			auto returnTypeSymbol = functionTypeSymbol->GetDescription();
			llvm::Type* resultType = GetLLVMTypeObject(returnTypeSymbol,context,manager);

			vector<llvm::Type*> elements;
			auto classTypeSymbol = functionTypeSymbol->GetParent();
			llvm::Type* classType = GetLLVMTypeObject(classTypeSymbol, context, manager);
			elements.emplace_back(classType);
			if (!functionTypeSymbol->GetArgs().size())
			{
				for (auto&& param : functionTypeSymbol->GetArgs())
				{
					llvm::Type* paramType = GetLLVMTypeObject(param->GetDescription(), context, manager);
					elements.emplace_back(paramType);
				}
			}
			
			auto functionType = llvm::FunctionType::get(resultType, elements, false);
			auto functionSymbol = manager->GetTypeDefSymbolByAstNode(node);
			functionSymbol->SetLLVMTypeDef(functionType);
			
			this->value = functionSymbol;
		}
		inline void		LLVMTypeObjectConstructVisitor::Visit(AstFieldTypeDefine* node) 
		{
			auto elementTypeSymbol = manager->GetIdentifierDefByIdentifierObject(node->type.get());
			llvm::Type* elementType = GetLLVMTypeObject(elementTypeSymbol, context, manager);
			auto fieldSymbol = manager->GetTypeDefSymbolByAstNode(node);
			assert(fieldSymbol->isClassFiledDef());
			fieldSymbol->SetLLVMTypeDef(elementType);
			this->value = fieldSymbol;
		}

		void LLVMTypeObjectConstruct(MiniSharpSymbolManager* manager, const ztl::miniSharp::ParserASTRoot& root, llvm::LLVMContext* context)
		{
			for (auto&& member : root.members)
			{
				LLVMTypeObjectConstructVisitor visitor(manager, manager->GetGlobalSymbol(), context);
				member->define->Accept(&visitor);
			}
		}

		void LLVMTypeObjectConstruct(MiniSharpSymbolManager * manager, const vector<shared_ptr<ztl::miniSharp::ParserASTRoot>>& roots, llvm::LLVMContext* context)
		{
			for (auto&&root : roots)
			{
				LLVMTypeObjectConstruct(manager, *root, context);
			}
		}
	}
}