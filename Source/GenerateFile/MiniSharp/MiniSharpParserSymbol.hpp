#pragma once
#include "MiniSharpParser.h"
#include <llvm\IR\IRBuilder.h>

namespace ztl
{
	namespace miniSharp
	{
		//标识符定义收集，把AST组织成MiniSharpSymbol 定义树
		//标识符绑定，把AST每个节点IdentifierObject对应的IdentifierDef的Symbol对应起来 填充这些symbol的llvm::type
		//类型演算推导，把AST的每个节点node和他们推导出的Type对应起来。
		using llvm::Value;
		enum class  MiniSharpSymbolType
		{
			GlobalSymbol,
			FunctionDef, //description returnType,args are function parameters
			VariableDef,//description Variable Type,
			ConstantsDef,//description Constants Type
			NamespaceDef,
			EnumTypeDef,
			ClassTypeDef,//description base type,args are interfaces;
			IntTypeDef,
			DoubleTypeDef,
			StringTypeDef,
			UnicodeCharTypeDef,
			InterfaceTypeDef,//description nullptr,args are interfaces;
			FunctionParameterDef,//description arg type; add data arg name
			BoolTypeDef,
			VoidTypeDef,
			ArrayType, //description element type
			SubType,// description parentScopeType.
			ThisType,//description classTypeDef,
			BaseType,//description classTypeDef,
			ClassFiledDef, //description classTypeDef,
			EnumFieldDef, //parent enumTypeDef,
			ScopeDef,//纯花括号那种- -。。。。这种名字是匿名的
			UsingDef,//using(var a = xxx)这种 这种名字是匿名的
			CaseDef,//case:a xxxx；这种名字是匿名的
			UsingAliasDef,//using name = X.X.X这种. 这个不做
		};
		class MiniSharpPasrseSymbol
		{
			MiniSharpSymbolType symbolType;
			wstring symbolName;
			unique_ptr<vector<MiniSharpPasrseSymbol*>> args;
			MiniSharpPasrseSymbol* parent;
			MiniSharpPasrseSymbol* description;

			boost::any data;

			llvm::Type* typeDef;//只有 type是xxxDef的时候有这个
			Value* value;//变量的时候。函数的时候
			vector<MiniSharpPasrseSymbol*> subSymbols;
			unordered_map<wstring, MiniSharpPasrseSymbol*> uniqueSymbolNameMap;
			unordered_map<wstring, vector<MiniSharpPasrseSymbol*>> functionSymbolNamedMap;


		public:
			MiniSharpPasrseSymbol(MiniSharpSymbolType _symbolType, const wstring& _name = L"", MiniSharpPasrseSymbol* _parent = nullptr, MiniSharpPasrseSymbol* _description = nullptr)
				:symbolType(_symbolType), symbolName(_name), description(_description), parent(_parent), args(std::move(make_unique<vector<MiniSharpPasrseSymbol*>>()))
			{

			}
		public:
			llvm::Type* GetLLVMTypeDef()
			{
				return typeDef;
			}
			void SetLLVMTypeDef(llvm::Type* type)
			{
				typeDef = type;
			}
			Value* GetValue()
			{
				
				return value;
			}
			
			void SetValue(llvm::Value* _value)
			{
				this->value = _value;
			}
			template<typename T>
			void AdditionData(T value)
			{
				data = value;
			}
			template<typename T>
			T* GetData()
			{
				return boost::unsafe_any_cast<T*>(&data);
			}
			MiniSharpSymbolType GetSymbolType()const
			{
				return symbolType;
			}
			const wstring& GetName()const
			{
				return symbolName;
			}
			void SetName(const wstring& name)
			{
				symbolName = name;
			}
			MiniSharpPasrseSymbol* GetDescription()
			{
				return description;
			}
			void SetDescription(MiniSharpPasrseSymbol* descript)
			{
				description = descript;
			}
			MiniSharpPasrseSymbol* GetParent()
			{
				return parent;
			}
			vector<MiniSharpPasrseSymbol*>::iterator begin()
			{
				return subSymbols.begin();
			}
			vector<MiniSharpPasrseSymbol*>::iterator end()
			{
				return subSymbols.end();
			}
			bool isIdentifyDefSymbol()const
			{
				return MiniSharpSymbolType::VariableDef == symbolType ||
					MiniSharpSymbolType::IntTypeDef == symbolType ||
					MiniSharpSymbolType::BoolTypeDef == symbolType ||
					MiniSharpSymbolType::VoidTypeDef == symbolType ||
					MiniSharpSymbolType::StringTypeDef == symbolType ||
					MiniSharpSymbolType::UnicodeCharTypeDef == symbolType ||
					MiniSharpSymbolType::ClassTypeDef == symbolType ||
					MiniSharpSymbolType::DoubleTypeDef == symbolType ||
					MiniSharpSymbolType::EnumTypeDef == symbolType ||
					MiniSharpSymbolType::FunctionDef == symbolType ||
					MiniSharpSymbolType::InterfaceTypeDef == symbolType ||

					MiniSharpSymbolType::ClassFiledDef == symbolType ||
					MiniSharpSymbolType::EnumFieldDef == symbolType ||
					MiniSharpSymbolType::FunctionParameterDef == symbolType ||
					MiniSharpSymbolType::NamespaceDef == symbolType||
					MiniSharpSymbolType::UsingAliasDef == symbolType||
					MiniSharpSymbolType::ConstantsDef == symbolType;
			}
			bool isAnonymousDefSymbol()const
			{
				return MiniSharpSymbolType::UsingDef == symbolType ||
					MiniSharpSymbolType::CaseDef == symbolType ||
					MiniSharpSymbolType::ScopeDef == symbolType;
			}
			bool isTypeStructureSymbol()const
			{
				return MiniSharpSymbolType::ArrayType == symbolType ||
					MiniSharpSymbolType::BaseType == symbolType ||
					MiniSharpSymbolType::SubType == symbolType ||
					MiniSharpSymbolType::ThisType == symbolType;
			}

			bool isTypeDefSymbol()const
			{
				return	MiniSharpSymbolType::IntTypeDef == symbolType ||
					MiniSharpSymbolType::BoolTypeDef == symbolType ||
					MiniSharpSymbolType::VoidTypeDef == symbolType ||
					MiniSharpSymbolType::StringTypeDef == symbolType ||
					MiniSharpSymbolType::UnicodeCharTypeDef == symbolType ||
					MiniSharpSymbolType::ClassTypeDef == symbolType ||
					MiniSharpSymbolType::DoubleTypeDef == symbolType ||
					MiniSharpSymbolType::EnumTypeDef == symbolType ||
					MiniSharpSymbolType::FunctionDef == symbolType ||
					MiniSharpSymbolType::InterfaceTypeDef == symbolType;
			}

			bool isCustomTypeDefSymbol()const
			{
				return	MiniSharpSymbolType::ClassTypeDef == symbolType ||
					MiniSharpSymbolType::EnumTypeDef == symbolType ||
					MiniSharpSymbolType::FunctionDef == symbolType ||
					MiniSharpSymbolType::InterfaceTypeDef == symbolType;
			}

			bool IsPrimitiveSymbolName()const
			{
				return GetName() == L"string" ||
					GetName() == L"char" || 
					GetName() == L"int" || 
					GetName() == L"double"|| 
					GetName()==L"bool";
			}
			bool isPrimitiveTypeDefSymbol()const
			{
				return	MiniSharpSymbolType::IntTypeDef == symbolType ||
					MiniSharpSymbolType::BoolTypeDef == symbolType ||
					MiniSharpSymbolType::VoidTypeDef == symbolType ||
					MiniSharpSymbolType::StringTypeDef == symbolType ||
					MiniSharpSymbolType::UnicodeCharTypeDef == symbolType ||
					MiniSharpSymbolType::DoubleTypeDef == symbolType;
			}
#define	MAKE_ISXXXXSYMBOL(symbolName) \
			bool is##symbolName()const\
			{\
				return this->symbolType == MiniSharpSymbolType::symbolName;\
			}
			MAKE_ISXXXXSYMBOL(GlobalSymbol);
			MAKE_ISXXXXSYMBOL(FunctionDef);
			MAKE_ISXXXXSYMBOL(VariableDef);
			MAKE_ISXXXXSYMBOL(ConstantsDef);
			MAKE_ISXXXXSYMBOL(NamespaceDef);
			MAKE_ISXXXXSYMBOL(EnumTypeDef);
			MAKE_ISXXXXSYMBOL(ClassTypeDef);
			MAKE_ISXXXXSYMBOL(IntTypeDef);
			MAKE_ISXXXXSYMBOL(DoubleTypeDef);
			MAKE_ISXXXXSYMBOL(StringTypeDef);
			MAKE_ISXXXXSYMBOL(UnicodeCharTypeDef);
			MAKE_ISXXXXSYMBOL(InterfaceTypeDef);
			MAKE_ISXXXXSYMBOL(FunctionParameterDef);
			MAKE_ISXXXXSYMBOL(BoolTypeDef);
			MAKE_ISXXXXSYMBOL(VoidTypeDef);
			MAKE_ISXXXXSYMBOL(ArrayType);
			MAKE_ISXXXXSYMBOL(SubType);
			MAKE_ISXXXXSYMBOL(ThisType);
			MAKE_ISXXXXSYMBOL(BaseType);
			MAKE_ISXXXXSYMBOL(ClassFiledDef);
			MAKE_ISXXXXSYMBOL(EnumFieldDef);
			MAKE_ISXXXXSYMBOL(UsingDef);
			MAKE_ISXXXXSYMBOL(ScopeDef);
			MAKE_ISXXXXSYMBOL(CaseDef);
			MAKE_ISXXXXSYMBOL(UsingAliasDef);
#undef MAKE_ISXXXXSYMBOL
			private:
				template<typename T>
				T GetSymbolByName(const wstring& name)
				{
					assert(false);
					return nullptr;
				}
				template<>
				MiniSharpPasrseSymbol* GetSymbolByName<MiniSharpPasrseSymbol*>(const wstring& name)
				{
					return (uniqueSymbolNameMap.find(name) == uniqueSymbolNameMap.end()) ? nullptr : uniqueSymbolNameMap[name];
				}
				template<>
				vector<MiniSharpPasrseSymbol*>* GetSymbolByName<vector<MiniSharpPasrseSymbol*>*>(const wstring& name)
				{
					return (functionSymbolNamedMap.find(name) == functionSymbolNamedMap.end()) ? nullptr : &functionSymbolNamedMap[name];
				}

				template<typename T>
				T SearchSymbolInClassByName(const wstring& name)
				{
					auto work = this;
					while (work != nullptr)
					{
						auto symbol = work->GetSymbolByName<T>(name);
						if (symbol != nullptr)
						{
							return symbol;
						}
						work = work->GetDescription();
					}
					return nullptr;
				}
				template<typename T>
				T SearchSymbolByName(const wstring& name)
				{
					auto work = this;
					while (work != nullptr)
					{
						auto symbol = work->GetSymbolByName<T>(name);
						if (symbol != nullptr)
						{
							return symbol;
						}

						if (work->isClassTypeDef())
						{
							auto result = SearchSymbolInClassByName<T>(name);
							if (result != nullptr)
							{
								return result;
							}
						}

						work = work->GetParent();
					}
					return nullptr;
				}
			public:
			
			

			void AddUniqueSubSymbol(MiniSharpPasrseSymbol* symbol)
			{
				static int count = 0;
				assert(!symbol->GetName().empty()||(symbol->GetName().empty() && symbol->isAnonymousDefSymbol()));
				subSymbols.emplace_back(symbol);
				if (!symbol->GetName().empty())
				{
					if (symbol->IsPrimitiveSymbolName()&&!symbol->isPrimitiveTypeDefSymbol())
					{
						throw ztl_exception(L"can't use primitive type name");
					}

					if(uniqueSymbolNameMap.find(symbol->GetName()) != uniqueSymbolNameMap.end())
					{
						throw ztl_exception(L"identifier redefine");
					}
					uniqueSymbolNameMap.insert({ symbol->GetName(),symbol });
				}
				else
				{
					auto name = L"AnonymousScope" + to_wstring(count);
					symbol->SetName(name);
					count += 1;
					uniqueSymbolNameMap.insert({ symbol->GetName(),symbol });
				}
			}
			void AddFunctioneSubSymbol(MiniSharpPasrseSymbol* symbol)
			{
				assert(symbol->GetSymbolType() == MiniSharpSymbolType::FunctionDef);
				assert(!symbol->GetName().empty());
				subSymbols.emplace_back(symbol);
				if (functionSymbolNamedMap.find(symbol->GetName()) == functionSymbolNamedMap.end())
				{
					functionSymbolNamedMap[symbol->symbolName];
				}
				functionSymbolNamedMap[symbol->symbolName].emplace_back(symbol);
			}
			


			MiniSharpPasrseSymbol* GetUniqueSymbolByName(const wstring& name)
			{
				return GetSymbolByName<MiniSharpPasrseSymbol*>(name);
			}
			vector<MiniSharpPasrseSymbol*>* GetFunctionSymbolByName(const wstring& name) 
			{
				return GetSymbolByName<vector<MiniSharpPasrseSymbol*>*>(name);
			}
			void AddArgs(MiniSharpPasrseSymbol* param)
			{
				assert((isClassTypeDef() && param->isInterfaceTypeDef()) || 
					(isInterfaceTypeDef() && param->isInterfaceTypeDef()) ||
					(isFunctionDef() && param->isFunctionParameterDef()));
				if (args == nullptr)
				{
					args = make_unique<vector<MiniSharpPasrseSymbol*>>();
				}
				args->emplace_back(param);
			}
			void AddArgs(const vector<MiniSharpPasrseSymbol*>& params)
			{
				assert((isClassTypeDef() && std::all_of(params.begin(), params.end(),
					[](MiniSharpPasrseSymbol* param) {return param->isInterfaceTypeDef();})) ||
					(isInterfaceTypeDef() && std::all_of(params.begin(), params.end(),
						[](MiniSharpPasrseSymbol* param) {return param->isInterfaceTypeDef();})) ||
					(isFunctionDef() && std::all_of(params.begin(), params.end(),
						[](MiniSharpPasrseSymbol* param) {return param->isFunctionParameterDef();})));
				if (args == nullptr)
				{
					args = make_unique<vector<MiniSharpPasrseSymbol*>>();
				}
				args->insert(args->end(), params.begin(), params.end());
			}
		
			const vector<MiniSharpPasrseSymbol*>& GetArgs()const
			{
			
				return *args;
			}

			MiniSharpPasrseSymbol* SearchUniqueSymbolByName(const wstring& name)
			{
				return SearchSymbolByName<MiniSharpPasrseSymbol*>(name);
			}
		
			vector<MiniSharpPasrseSymbol*>* SearchFunctionSymbolByName(const wstring& name)
			{
				return SearchSymbolByName<vector<MiniSharpPasrseSymbol*>*>(name);
			}
		};
	}
}