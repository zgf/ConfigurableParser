#pragma once
#include "MiniSharpParserSymbol.hpp"

namespace ztl
{
	namespace miniSharp
	{

		class  MiniSharpSymbolManager
		{
			MiniSharpPasrseSymbol* globalSymbol;//收集符号定义。函数定义，变量定义。命名空间定义。类型定义。
			vector<unique_ptr<MiniSharpPasrseSymbol>> symbols;
			unordered_map<IdentifierObject*, MiniSharpPasrseSymbol*> identifierDefMap;//变量名绑定到变量定义。命名空间名或者类型名就绑定到命名空间或者类型定义，函数名就绑定到函数定义的symbol.
			unordered_map< MiniSharpPasrseSymbol*, miniSharp::AstTypeDefine*> typeDefToAstMap;//类型定义到AstTypeDefine*映射
			unordered_map< miniSharp::AstTypeDefine*,MiniSharpPasrseSymbol*> astToTypeDefMap;//AstTypeDefine*到类型定义映射
			unordered_map<miniSharp::AstUsingDeclExpression*, MiniSharpPasrseSymbol*> scopeSpaceMap;//Using绑定作用域
			unordered_map<miniSharp::AstStatement*, MiniSharpPasrseSymbol*> anonymousScopeMap;
			unordered_map<miniSharp::AstExpression*, MiniSharpPasrseSymbol*> literalSymbolMap;//字面值常量映射
			unordered_map<miniSharp::AstExpression*, llvm::Type*> typeInferenceMap;//类型推导映射

		public:
			MiniSharpSymbolManager()
			{
				globalSymbol = AddGlobalSymbolSymbol();
			    AddIntTypeDefSymbol(L"int", globalSymbol);
				AddDoubleTypeDefSymbol(L"double", globalSymbol);
				AddUnicodeCharTypeDefSymbol(L"char", globalSymbol);
				AddStringTypeDefSymbol(L"string", globalSymbol);
				AddBoolTypeDefSymbol(L"bool", globalSymbol);
				AddVoidTypeDefSymbol(L"void", globalSymbol);
			}
		public:
			MiniSharpPasrseSymbol* GetGlobalSymbol()
			{
				return globalSymbol;
			}
			MiniSharpPasrseSymbol* GetIdentifierDefByIdentifierObject(IdentifierObject* node)
			{
				return (identifierDefMap.find(node) == identifierDefMap.end()) ? nullptr : identifierDefMap[node];
			}
			void AddIdentifierObjectToDefineMap(IdentifierObject* node, MiniSharpPasrseSymbol* symbol)
			{
				assert(identifierDefMap.find(node) == identifierDefMap.end());
				identifierDefMap.insert({ node,symbol });
			}
			
		
			void AddAnonymousScopeMap(AstStatement* node, MiniSharpPasrseSymbol* symbol)
			{
				assert(anonymousScopeMap.find(node) == anonymousScopeMap.end());
				anonymousScopeMap.insert({ node,symbol });
			}
			MiniSharpPasrseSymbol* GetAnonymousScopeSymbol(AstStatement* node)
			{
				return anonymousScopeMap.find(node) == anonymousScopeMap.end() ? nullptr : anonymousScopeMap[node];
			}

			void AddTypeInferenceMap(miniSharp::AstExpression* expression, llvm::Type* type)
			{
				assert(typeInferenceMap.find(expression) == typeInferenceMap.end());
				typeInferenceMap.insert({ expression,type });
			}

			llvm::Type* GetTypeInferenceMapByLLVMType(miniSharp::AstExpression* expression)
			{
				return typeInferenceMap.find(expression) == typeInferenceMap.end() ? nullptr : typeInferenceMap[expression];
			}

			void AddLiteralSymbolMap(miniSharp::AstExpression* node, MiniSharpPasrseSymbol* symbol)
			{
				assert(symbol->isConstantsDef());
				assert(literalSymbolMap.find(node) == literalSymbolMap.end());
				literalSymbolMap.insert({ node,symbol });
			}

			MiniSharpPasrseSymbol* GetLiteralSymbolByAstExpression(miniSharp::AstExpression* node)
			{
				return (literalSymbolMap.find(node) == literalSymbolMap.end()) ? nullptr : literalSymbolMap[node];
			}

			void AddTypeDefToAstMap(MiniSharpPasrseSymbol* symbol, miniSharp::AstTypeDefine* node)
			{
				
				assert(typeDefToAstMap.find(symbol) == typeDefToAstMap.end());
				typeDefToAstMap.insert({ symbol,node });
			}
			miniSharp::AstTypeDefine* GetAstNodeByTypeDefSymbol(MiniSharpPasrseSymbol* symbol)
			{
				return typeDefToAstMap.find(symbol) == typeDefToAstMap.end() ? nullptr : typeDefToAstMap[symbol];
			}
			void AddAstToTypeDefMap(MiniSharpPasrseSymbol* symbol, miniSharp::AstTypeDefine* node)
			{
				assert(this->astToTypeDefMap.find(node) == astToTypeDefMap.end());
				astToTypeDefMap.insert({ node,symbol });
			}
			MiniSharpPasrseSymbol* GetTypeDefSymbolByAstNode(miniSharp::AstTypeDefine* node)
			{
				return astToTypeDefMap.find(node) == astToTypeDefMap.end() ? nullptr : astToTypeDefMap[node];
			}
			void AddScopeSpaceSymbolMap(AstUsingDeclExpression* node, MiniSharpPasrseSymbol* symbol)
			{
				assert (scopeSpaceMap.find(node) == scopeSpaceMap.end());
				scopeSpaceMap.insert({ node,symbol });
			}
			MiniSharpPasrseSymbol* GetScopeSpaceSymbol(AstUsingDeclExpression* node)
			{
				return scopeSpaceMap.find(node) == scopeSpaceMap.end() ? nullptr : scopeSpaceMap[node];
			}
#define GET_XXXTYPEDEFSYMBOL(symbolName,symbolToken) \
			MiniSharpPasrseSymbol* Get##symbolName##Symbol()\
			{\
				return GetGlobalSymbol()->GetUniqueSymbolByName(symbolToken);\
			}

			GET_XXXTYPEDEFSYMBOL(IntTypeDef,L"int");
			GET_XXXTYPEDEFSYMBOL(DoubleTypeDef,L"double");
			GET_XXXTYPEDEFSYMBOL(StringTypeDef,L"string");
			GET_XXXTYPEDEFSYMBOL(UnicodeCharTypeDef,L"char");
			GET_XXXTYPEDEFSYMBOL(BoolTypeDef,L"bool");
			GET_XXXTYPEDEFSYMBOL(VoidTypeDef, L"bool");
#undef GET_XXXTYPEDEFSYMBOL
			MiniSharpPasrseSymbol* AddNewSymbol(MiniSharpSymbolType symbolType, const wstring& name = L"", MiniSharpPasrseSymbol* parent = nullptr, MiniSharpPasrseSymbol* description = nullptr)
			{
				symbols.emplace_back(make_unique<MiniSharpPasrseSymbol>(symbolType, name, parent, description));
				auto symbol = symbols.back().get();
				if (parent != nullptr&& !symbol->GetName().empty())
				{
					if (symbol->isFunctionDef())
					{
						parent->AddFunctioneSubSymbol(symbol);
					}
					else
					{
						parent->AddUniqueSubSymbol(symbol);
					}
				}
				return symbol;
			}
#define MAKE_ADDXXXSYMBOL(symbolName)\
			MiniSharpPasrseSymbol* Add##symbolName##Symbol(const wstring& name = L"", MiniSharpPasrseSymbol* parent = nullptr, MiniSharpPasrseSymbol* description = nullptr)\
			{\
				auto symbol = AddNewSymbol(MiniSharpSymbolType::##symbolName, name, parent, description);\
				return symbol;\
			}

			MAKE_ADDXXXSYMBOL(GlobalSymbol);
			MAKE_ADDXXXSYMBOL(FunctionDef);
			MAKE_ADDXXXSYMBOL(VariableDef);
			MAKE_ADDXXXSYMBOL(ConstantsDef);
			MAKE_ADDXXXSYMBOL(NamespaceDef);
			MAKE_ADDXXXSYMBOL(EnumTypeDef);
			MAKE_ADDXXXSYMBOL(ClassTypeDef);
			MAKE_ADDXXXSYMBOL(IntTypeDef);
			MAKE_ADDXXXSYMBOL(DoubleTypeDef);
			MAKE_ADDXXXSYMBOL(StringTypeDef);
			MAKE_ADDXXXSYMBOL(UnicodeCharTypeDef);
			MAKE_ADDXXXSYMBOL(InterfaceTypeDef);
			MAKE_ADDXXXSYMBOL(FunctionParameterDef);
			MAKE_ADDXXXSYMBOL(BoolTypeDef);
			MAKE_ADDXXXSYMBOL(VoidTypeDef);
			MAKE_ADDXXXSYMBOL(ArrayType);
			MAKE_ADDXXXSYMBOL(SubType);
			MAKE_ADDXXXSYMBOL(ThisType);
			MAKE_ADDXXXSYMBOL(BaseType);
			MAKE_ADDXXXSYMBOL(ClassFiledDef);
			MAKE_ADDXXXSYMBOL(EnumFieldDef);
			MAKE_ADDXXXSYMBOL(UsingDef);
			MAKE_ADDXXXSYMBOL(ScopeDef);
			MAKE_ADDXXXSYMBOL(CaseDef);
			MAKE_ADDXXXSYMBOL(UsingAliasDef);
#undef MAKE_ADDXXXSYMBOL
		};

		//绑定继承关系和处理类型前缀之类的.
		extern void CollectIdentifierSymbolDefine(MiniSharpSymbolManager* manager,const vector<shared_ptr<ztl::miniSharp::ParserASTRoot>>& roots);
		extern void MapIdentifierObjectToDefine(MiniSharpSymbolManager* manager, const vector<shared_ptr<ztl::miniSharp::ParserASTRoot>>& roots);
		extern void LLVMTypeObjectConstruct(MiniSharpSymbolManager* manager, const vector<shared_ptr<ztl::miniSharp::ParserASTRoot>>& roots,llvm::LLVMContext* context);
		extern void VerificationTypeInference(MiniSharpSymbolManager* manager, const vector<shared_ptr<ztl::miniSharp::ParserASTRoot>>& roots, llvm::LLVMContext* context);
	//	extern void NameMangling(MiniSharpSymbolManager* manager);
		unique_ptr<llvm::Module> IRCodeGen(const vector<shared_ptr<ztl::miniSharp::ParserASTRoot>>& root);
		std::wstring GetTypeDefSymbolUniqueLLVMTypeName(ztl::miniSharp::MiniSharpPasrseSymbol* symbol);
		std::wstring GetFunctionTypeDefSymbolUniqueLLVMTypeName(ztl::miniSharp::MiniSharpPasrseSymbol* symbol);
	}
}