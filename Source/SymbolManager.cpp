#include "Include/stdafx.h"
#include "Include/SymbolManager.h"
#include "Include/ParserSymbol.h"
#include "../Lib/Regex/ztl_regex_lex.h"
#include "../Lib/Regex/ztl_regex_parser.h"
#include "../Lib/ZTL/ztl_exception.h"

namespace ztl
{
	namespace general_parser
	{
		vector<wstring>& SymbolManager::GetStartRuleList()
		{
			return startRuleList;
		}
		void SymbolManager::SetStartRuleList(vector<wstring> val)
		{
			startRuleList = val;
		}
		GeneralTableDefine * SymbolManager::GetTable()
		{
			return table.get();
		}
		ParserSymbol * SymbolManager::GetGlobalSymbol()
		{
			return this->globalSymbol;
		}
		ParserSymbol * SymbolManager::GetTokenTypeSymbol()
		{
			return this->tokenTypeSymbol;
		}

		void SymbolManager::TryAddSubSymbol(ParserSymbol * subSymbol, ParserSymbol * parentSymbol)
		{
			subSymbol->parent = parentSymbol;
			parentSymbol->AddSubSymbol(subSymbol);
		}
		void SymbolManager::CacheRuleDefineAndSymbolMap(GeneralRuleDefine * ruleDef, ParserSymbol * symbol)
		{
			this->symbolRuleDefineMap.insert(make_pair(symbol, ruleDef));
		}
		GeneralRuleDefine* SymbolManager::GetCacheRuleDefineBySymbol(ParserSymbol * symbol)
		{
			assert(symbol->IsRuleDef());
			auto findIter = symbolRuleDefineMap.find(symbol);
			return (findIter == symbolRuleDefineMap.end()) ? nullptr : findIter->second;
		}
		ParserSymbol * SymbolManager::AddClass(const wstring & name, ParserSymbol * baseType, ParserSymbol * parentType)
		{
			assert(parentType->IsClassType() || parentType->IsGlobal());
			CheckNameReDefineError(name, parentType);
			auto classTypeSymbol = CreatASymbol(SymbolType::ClassType, name, parentType, baseType);
			TryAddSubSymbol(classTypeSymbol, parentType);
			CacheBaseSymbolToDeriveMap(baseType, classTypeSymbol);
			return classTypeSymbol;
		}
		ParserSymbol * SymbolManager::AddField(const wstring & name, ParserSymbol * fieldType, ParserSymbol * parentType)
		{
			assert(parentType->IsClassType());
			CheckNameReDefineError(name, parentType);
			auto fieldSymbol = CreatASymbol(SymbolType::FieldDef, name, parentType, fieldType);
			TryAddSubSymbol(fieldSymbol, parentType);
			return fieldSymbol;
		}
		SymbolManager::SymbolManager() :table(BootStrapDefineTable())
		{
			globalSymbol = CreatASymbol(SymbolType::Global, L"", nullptr, nullptr);
			tokenTypeSymbol = CreatASymbol(SymbolType::TokenType, L"token", nullptr, nullptr);
		}
		SymbolManager::SymbolManager(const shared_ptr<GeneralTableDefine>& _table) : table(_table)
		{
			globalSymbol = CreatASymbol(SymbolType::Global, L"", nullptr, nullptr);
			tokenTypeSymbol = CreatASymbol(SymbolType::TokenType, L"TokenInfo", nullptr, nullptr);
		}
		const vector<shared_ptr<GeneralTokenDefine>>& SymbolManager::GetTokens()const
		{
			return table->tokens;
		}
		const vector<shared_ptr<GeneralTypeDefine>>& SymbolManager::GetTypes()const
		{
			return table->types;
		}

		ParserSymbol * SymbolManager::AddEnum(const wstring & name, ParserSymbol * parentType)
		{
			assert(parentType->IsClassType() || parentType->IsGlobal());
			this->CheckNameReDefineError(name, parentType);
			auto enumTypeSymbol = CreatASymbol(SymbolType::EnumType, name, parentType, nullptr);
			TryAddSubSymbol(enumTypeSymbol, parentType);
			return enumTypeSymbol;
		}

		ParserSymbol * SymbolManager::AddEnumItem(const wstring & name, ParserSymbol * parentType)
		{
			assert(parentType->IsEnumType());
			CheckNameReDefineError(name, parentType);

			auto&& enumItemSymbol = CreatASymbol(SymbolType::EnumDef, name, parentType, parentType);
			TryAddSubSymbol(enumItemSymbol, parentType);
			return enumItemSymbol;
		}

		ParserSymbol * SymbolManager::AddRuleDefine(const wstring & name, ParserSymbol * ruleType)
		{
			assert(ruleType->IsClassType());
			CheckNameReDefineError(name, GetGlobalSymbol());
			auto ruleSymbol = CreatASymbol(SymbolType::RuleDef, name, GetGlobalSymbol(), ruleType);
			TryAddSubSymbol(ruleSymbol, GetGlobalSymbol());
			CacheRuleNameToSymbolMap(name, ruleSymbol);

			return ruleSymbol;
		}
		void CheckRegexError(const wstring& regex)
		{
			RegexLex lexer(regex);
			lexer.ParsingPattern();
			RegexParser parser(lexer);
		}
		ParserSymbol* SymbolManager::AddTokenDefine(const wstring& name, const wstring& regex, bool ignore)
		{
			CheckNameReDefineError(name, GetGlobalSymbol());
			try
			{
				CheckRegexError(regex);
			}
			catch(const ztl_exception& ex)
			{
				throw ztl_exception(L"\ntoken name: " + name +
					L"token regex: " + regex +
					L"can't creat correct regex string! the regex error message is:" + ex.Message());
			}
			auto tokenSymbol = CreatASymbol(SymbolType::TokenDef, name, GetGlobalSymbol(), nullptr, ignore);

			TryAddSubSymbol(tokenSymbol, GetGlobalSymbol());
			if(tokenSymbol->IsIgnore())
			{
				CacheDisTokenNameSymbolMap(name, tokenSymbol);
			}
			else
			{
				CacheTokenNameToSymbolMap(name, tokenSymbol);
				CacheRegexStringToSymbolMap(regex, tokenSymbol);
			}

			return tokenSymbol;
		}

		ParserSymbol * SymbolManager::AddArrayType(ParserSymbol * elementType)
		{
			assert(elementType->IsType());
			auto arraySymbol = CreatASymbol(SymbolType::ArrayType, L"", nullptr, elementType);
			return arraySymbol;
		}

		ParserSymbol* SymbolManager::CreatASymbol(SymbolType type, const wstring & name, ParserSymbol * parent, ParserSymbol * descriptor)
		{
			createdSymbolList.emplace_back(make_shared<ParserSymbol>(this, type, name, parent, descriptor));
			return createdSymbolList.back().get();
		}

		ParserSymbol * SymbolManager::CreatASymbol(SymbolType type, const wstring & name, ParserSymbol * parent, ParserSymbol * descriptor, bool ignore)
		{
			createdSymbolList.emplace_back(make_shared<ParserSymbol>(this, type, name, parent, descriptor, ignore));
			return createdSymbolList.back().get();
		}

		void SymbolManager::CacheTypeObjectSymbolMap(GeneralTypeObject * type, ParserSymbol * typeSymbol)
		{
			assert(typeSymbolMap.find(type) == typeSymbolMap.end());
			typeSymbolMap.insert(make_pair(type, typeSymbol));
		}

		ParserSymbol * SymbolManager::GetCacheTypeObjectSymbol(GeneralTypeObject * type)
		{
			auto findScope = typeSymbolMap.find(type);
			return (findScope == typeSymbolMap.end()) ? nullptr : findScope->second;
		}

		void SymbolManager::CacheTypeDefineAndSymbolMap(GeneralTypeDefine * type, ParserSymbol * typeSymbol)
		{
			assert(dynamic_cast<GeneralClassTypeDefine*>(type) != nullptr || dynamic_cast<GeneralEnumTypeDefine*>(type) != nullptr);
			assert(typeDefSymbolMap.find(type) == typeDefSymbolMap.end());
			assert(typeSymbol->IsClassType() || typeSymbol->IsEnumType());
			assert(symbolTypeDefMap.find(typeSymbol) == symbolTypeDefMap.end());
			typeDefSymbolMap.insert(make_pair(type, typeSymbol));
			symbolTypeDefMap.insert(make_pair( typeSymbol, type));
		}

		ParserSymbol * SymbolManager::GetCacheSymbolByTypeDefine(GeneralTypeDefine * type)
		{
			assert(!typeDefSymbolMap.empty());
			assert(dynamic_cast<GeneralClassTypeDefine*>(type) != nullptr || dynamic_cast<GeneralEnumTypeDefine*>(type) != nullptr);
			auto findIter = typeDefSymbolMap.find(type);
			return (findIter == typeDefSymbolMap.end()) ? nullptr : findIter->second;
		}

		GeneralTypeDefine * SymbolManager::GetCacheTypeDefineBySymbol(ParserSymbol * type)
		{
			assert(!this->symbolTypeDefMap.empty());
			assert(type->IsClassType()|| type->IsEnumType());
			auto findIter = symbolTypeDefMap.find(type);
			return (findIter == symbolTypeDefMap.end()) ? nullptr : findIter->second;
		}

		void SymbolManager::CacheRuleNameToSymbolMap(const wstring & name, ParserSymbol * symbol)
		{
			assert(symbol->IsRuleDef());

			assert(ruleNameSymbolMap.find(name) == ruleNameSymbolMap.end());
			this->ruleNameSymbolMap.insert({ name, symbol });
		}

		void SymbolManager::CacheTokenNameToSymbolMap(const wstring & name, ParserSymbol * symbol)
		{
			assert(symbol->IsTokenDef());
			assert(tokenNameSymbolMap.find(name) == tokenNameSymbolMap.end());
			this->tokenNameSymbolMap.insert({ name, symbol });
		}

		void SymbolManager::CacheDisTokenNameSymbolMap(const wstring & name, ParserSymbol * symbol)
		{
			assert(symbol->IsTokenDef());
			assert(disTokenNameSymbolMap.find(name) == disTokenNameSymbolMap.end());
			this->disTokenNameSymbolMap.insert({ name, symbol });
		}

		void SymbolManager::CacheRegexStringToSymbolMap(const wstring & name, ParserSymbol * symbol)
		{
			if(regexSymbolMap.find(name) == regexSymbolMap.end())
			{
				this->regexSymbolMap.insert({ name, symbol });
			}
			else
			{
				throw ztl_exception(L"token's regex need unique!");
			}
		}
		ParserSymbol* SymbolManager::GetCacheRuleNameToSymbol(const wstring& name) const
		{
			auto findIter = ruleNameSymbolMap.find(name);
			return (findIter == ruleNameSymbolMap.end()) ? nullptr : findIter->second;
		}

		ParserSymbol* SymbolManager::GetCacheTokenNameToSymbol(const wstring& name) const
		{
			auto findIter = tokenNameSymbolMap.find(name);
			return (findIter == tokenNameSymbolMap.end()) ? nullptr : findIter->second;
		}

		ParserSymbol* SymbolManager::GetCacheRegexStringToSymbol(const wstring& name) const
		{
			auto findIter = regexSymbolMap.find(name);
			return (findIter == regexSymbolMap.end()) ? nullptr : findIter->second;
		}

		ParserSymbol * SymbolManager::GetCacheDisTokenNameSymbol(const wstring & name) const
		{
			auto findIter = disTokenNameSymbolMap.find(name);
			return (findIter == disTokenNameSymbolMap.end()) ? nullptr : findIter->second;
		}

		void SymbolManager::CacheGrammarToFieldDefSymbol(GeneralGrammarTypeDefine * grammar, ParserSymbol * fieldDefSymbol)
		{
			assert(dynamic_cast<GeneralGrammarAssignTypeDefine*>(grammar) || dynamic_cast<GeneralGrammarSetterTypeDefine*>(grammar));
			assert(fieldDefSymbol->IsFieldDef());
			grammarNodeDefSymbolMap.insert({ grammar,fieldDefSymbol });
		}

		void SymbolManager::CacheTextGrammarToTokenDefSymbol(GeneralGrammarTypeDefine * textGrammar, ParserSymbol * tokenDefSymbol)
		{
			assert(tokenDefSymbol->IsTokenDef());
			assert(dynamic_cast<GeneralGrammarTextTypeDefine*>(textGrammar));
			this->grammarNodeDefSymbolMap.insert({ textGrammar,tokenDefSymbol });
		}

		void SymbolManager::CacheNormalGrammarToTokenDefSymbol(GeneralGrammarTypeDefine * normalGrammar, ParserSymbol * tokenDefSymbol)
		{
			assert(tokenDefSymbol->IsTokenDef());
			assert(dynamic_cast<GeneralGrammarNormalTypeDefine*>(normalGrammar));
			this->grammarNodeDefSymbolMap.insert({ normalGrammar,tokenDefSymbol });
		}

		void SymbolManager::CacheNormalGrammarToRuleDefSymbol(GeneralGrammarTypeDefine* normalGrammar, ParserSymbol* ruleDefSymbol)
		{
			assert(ruleDefSymbol->IsRuleDef());
			assert(dynamic_cast<GeneralGrammarNormalTypeDefine*>(normalGrammar));
			this->grammarNodeDefSymbolMap.insert({ normalGrammar,ruleDefSymbol });
		}
		void SymbolManager::CacheUsingGrammarToRuleDefSymbol(GeneralGrammarTypeDefine * usingGrammar, ParserSymbol * ruleDefSymbol)
		{
			assert(ruleDefSymbol->IsRuleDef());
			assert(dynamic_cast<GeneralGrammarUsingTypeDefine*>(usingGrammar));
			this->grammarNodeDefSymbolMap.insert({ usingGrammar,ruleDefSymbol });
		}
		ParserSymbol * SymbolManager::GetCacheUsingGrammarToRuleDefSymbol(GeneralGrammarTypeDefine * usingGrammar)
		{
			auto findIter = grammarNodeDefSymbolMap.find(usingGrammar);
			return findIter == grammarNodeDefSymbolMap.end() ? nullptr : findIter->second;
		}
		ParserSymbol* SymbolManager::GetCacheNormalGrammarToRuleDefSymbol(GeneralGrammarTypeDefine* normalGrammar)
		{
			auto findIter = grammarNodeDefSymbolMap.find(normalGrammar);
			return findIter == grammarNodeDefSymbolMap.end() ? nullptr : findIter->second;
		}

		ParserSymbol * SymbolManager::GetCacheNonTerminateGrammarToRuleDefSymbol(GeneralGrammarTypeDefine * terminateGrammar)
		{
			assert(dynamic_cast<GeneralGrammarNormalTypeDefine*>(terminateGrammar) || dynamic_cast<GeneralGrammarTextTypeDefine*>(terminateGrammar));
			auto findIter = grammarNodeDefSymbolMap.find(terminateGrammar);
			return findIter == grammarNodeDefSymbolMap.end() ? nullptr : findIter->second;
		}

		ParserSymbol * SymbolManager::GetCacheFieldDefSymbolByGrammar(GeneralGrammarTypeDefine * grammar)
		{
			assert(dynamic_cast<GeneralGrammarAssignTypeDefine*>(grammar) || dynamic_cast<GeneralGrammarSetterTypeDefine*>(grammar));
			auto findIter = grammarNodeDefSymbolMap.find(grammar);
			return findIter == grammarNodeDefSymbolMap.end() ? nullptr : findIter->second;
		}

		void SymbolManager::CachePropertyToValueMap(const wstring& property, const vector<wstring>& value)
		{
			propertyToValueMap.insert({ property,value });
		}

		void SymbolManager::CachePropertyToValueMap(const wstring& property, const wstring& value)
		{
			auto&& findIter = propertyToValueMap.find(property);
			if(findIter == propertyToValueMap.end())
			{
				propertyToValueMap[property];
			}
			propertyToValueMap[property].emplace_back(value);
		}

		vector<wstring> SymbolManager::GetCacheValueByProperty(const wstring& property) const
		{
			auto&& findIter = propertyToValueMap.find(property);
			return (findIter == propertyToValueMap.end()) ? vector<wstring>() : findIter->second;
		}

		void SymbolManager::CacheBaseSymbolToDeriveMap(ParserSymbol * baseSymbol, ParserSymbol * deriveSymbol)
		{
			auto&& findIter = baseSymbolToDeriveMap.find(baseSymbol);
			if(findIter == baseSymbolToDeriveMap.end())
			{
				baseSymbolToDeriveMap[baseSymbol];
			}
			baseSymbolToDeriveMap[baseSymbol].emplace_back(deriveSymbol);
		}

		vector<ParserSymbol*> SymbolManager::GetCacheDeriveByBaseSymbol(ParserSymbol * baseSymbol) const
		{
			auto&& findIter = baseSymbolToDeriveMap.find(baseSymbol);
			return (findIter == baseSymbolToDeriveMap.end()) ? vector<ParserSymbol*>() : findIter->second;
		}

		vector<ParserSymbol*> SymbolManager::GetCacheAllDeriveByBaseSymbol(ParserSymbol * baseSymbol) const
		{
			vector<ParserSymbol*> result;
			vector<ParserSymbol*> stack;
			stack.emplace_back(baseSymbol);
			while(!stack.empty())
			{
				auto back = stack.back();
				stack.pop_back();
				auto list = this->GetCacheDeriveByBaseSymbol(back);
				if(list.empty())
				{
					result.emplace_back(back);
				}
				else
				{
					stack.insert(stack.end(), list.begin(), list.end());
				}
			}
			return result;
		}

		unordered_map<wstring, vector<wstring>>& SymbolManager::GetPropertyToValueMap()
		{
			return propertyToValueMap;
		}

		const SymbolManager::TypeDefineToSymbolMapType & SymbolManager::GetTypeDefSymbolMap()const
		{
			return typeDefSymbolMap;
			// TODO: insert return statement here
		}

		const SymbolManager::BaseSymbolToDeriveSymbolMapType & SymbolManager::GetbaseSymbolToDeriveMap() const
		{
			return baseSymbolToDeriveMap;
		}

		void SymbolManager::CheckNameReDefineError(const wstring& name, ParserSymbol * parentSymbol)
		{
			auto result = parentSymbol->GetSubSymbolByName(name);
			if(result)
			{
				throw ztl::contract::ztl_exception(
					L"Cannot Redefine samll name :" + name +
					L"the parentSymbol type is:" + parentSymbol->GetTypeToWString() +
					L"the same symbol type is" + result->GetTypeToWString());
			}
		}

		ParserSymbol* SymbolManager::CheckNameHasDefine(const wstring name, ParserSymbol* scope)
		{
			ParserSymbol* result = nullptr;
			while(scope != nullptr)
			{
				if(scope->IsClassType())
				{
					result = scope->SearchClassSubSymbol(name);
				}
				else
				{
					result = scope->GetSubSymbolByName(name);
				}
				if(result)
				{
					return result;
				}
				scope = scope->GetParentSymbol();
			}
			return result;
		}
		wstring LinearStringToRegex(const wstring &regex)
		{
			return ztl::LinearStringToRegexString(regex);
		}
	}
}