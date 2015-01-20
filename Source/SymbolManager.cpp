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
		void SymbolManager::CreatStringAndTagMap()
		{
			for(auto&& iter : table->tokens)
			{
				SetNameToMap(iter->name, TagType::terminate);
				
			}
			for(auto&& iter : table->rules)
			{
				SetNameToMap(iter->name, TagType::nonterminate);
			}
		}
		unordered_map<wstring, LexTokenDefine> SymbolManager::GetLexTokenDefine()
		{
			unordered_map<wstring, LexTokenDefine> result;
			for(auto&&iter : table->tokens)
			{
				result.insert(make_pair(iter->name, LexTokenDefine(iter->name, iter->regex, iter->ignore)));
			}
			return result;
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
		/*void SymbolManager::CacheTokenDefineAndSymbolMap(GeneralTokenDefine *tokenDef, ParserSymbol * symbol)
		{
			tokenDefineSymbolMap.insert(make_pair(tokenDef, symbol));
			symbolTokenDefineMap.insert(make_pair(symbol, tokenDef));
		}*/
		//void SymbolManager::CacheClassDefineAndSymbolMap(GeneralClassTypeDefine * , ParserSymbol * symbol)
		//{
		//	/*classDefineSymbolMap.insert(make_pair(classDef, symbol));
		//	symbolClassDefineMap.insert(make_pair(symbol, classDef));*/
		//}
		//
		//void SymbolManager::CacheRuleDefineAndSymbolMap(GeneralRuleDefine * , ParserSymbol * symbol)
		//{
		//	/*this->ruleDefineSymbolMap.insert(make_pair(ruleDef, symbol));
		//	this->symbolRuleDefineMap.insert(make_pair(symbol, ruleDef));*/
		//}
		//void SymbolManager::CacheEnumTypeAndSymbolMap(GeneralEnumTypeDefine * , ParserSymbol * symbol)
		//{
		//	/*this->enumTypeSymbolMap.insert(make_pair(enumType, symbol));
		//	this->symboEnumTypeMap.insert(make_pair(symbol, enumType));*/
		//}
		ParserSymbol * SymbolManager::AddClass(const wstring & name, ParserSymbol * baseType, ParserSymbol * parentType)
		{
			assert(parentType->IsClassType() || parentType->IsGlobal());
			CheckNameReDefineError(name,parentType);
			auto classTypeSymbol = CreatASymbol(SymbolType::ClassType, name, parentType,baseType);
			TryAddSubSymbol(classTypeSymbol, parentType);
			
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
			CreatStringAndTagMap();

			globalSymbol = CreatASymbol(SymbolType::Global, L"", nullptr, nullptr);
			tokenTypeSymbol = CreatASymbol(SymbolType::TokenType, L"token", nullptr, nullptr);
		}
		SymbolManager::SymbolManager(const shared_ptr<GeneralTableDefine>& _table) :table(_table)
		{
			CreatStringAndTagMap();

			globalSymbol = CreatASymbol(SymbolType::Global, L"", nullptr, nullptr);
			tokenTypeSymbol = CreatASymbol(SymbolType::TokenType, L"token", nullptr, nullptr);
		}
				const vector<shared_ptr<GeneralTokenDefine>>& SymbolManager::GetTokens()const
		{
			return table->tokens;
		}
		const vector<shared_ptr<GeneralTypeDefine>>& SymbolManager::GetTypes()const
		{
			// TODO: insert return statement here
			return table->types;
		}
		int SymbolManager::GetTagByString(const wstring & name)const
		{
			return stringToTagMap.at(name);
		}

		wstring SymbolManager::GetStringByTag(int index)const
		{
			return tagToStringMap.at(index);
		}

		bool SymbolManager::IsLexerTag(int index)const
		{
			return tagTypeList[index] == TagType::terminate;
		}

		void SymbolManager::SetNameToMap(const wstring& name, TagType type)
		{
			this->tagToStringMap.emplace_back(name);
			auto currentIndex = tagToStringMap.size() - 1;
			tagTypeList.emplace_back(type);
			this->stringToTagMap.insert(make_pair(tagToStringMap.back(), currentIndex));
		}

		ParserSymbol * SymbolManager::AddEnum(const wstring & name, ParserSymbol * parentType)
		{
			assert(parentType->IsClassType() || parentType->IsGlobal());
			this->CheckNameReDefineError(name, parentType);
			auto enumTypeSymbol = CreatASymbol(SymbolType::EnumType,name, parentType, nullptr);
			TryAddSubSymbol(enumTypeSymbol, parentType);
			return enumTypeSymbol;
		}

		ParserSymbol * SymbolManager::AddEnumItem(const wstring & name, ParserSymbol * parentType)
		{
			assert(parentType->IsEnumType());
			CheckNameReDefineError(name, parentType);

			auto&& enumItemSymbol = CreatASymbol(SymbolType::EnumDef,name,parentType,parentType);
			TryAddSubSymbol(enumItemSymbol, parentType);
			return enumItemSymbol;
		}

		ParserSymbol * SymbolManager::AddRuleDefine(const wstring & name, ParserSymbol * ruleType)
		{
			assert(ruleType->IsClassType());
			CheckNameReDefineError(name,GetGlobalSymbol());
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
					L"can't creat correct regex string! the regex error message is:"+ex.Message());
			}
			auto tokenSymbol = CreatASymbol(SymbolType::TokenDef, name, GetGlobalSymbol(),nullptr,/* AddRegexDefine(regex),*/ignore);

			TryAddSubSymbol(tokenSymbol, GetGlobalSymbol());
			CacheTokenNameToSymbolMap(name, tokenSymbol);
			CacheRegexStringToSymbolMap(regex, tokenSymbol);
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
			createdSymbolList.emplace_back(make_shared<ParserSymbol>(this, type, name, parent, descriptor,ignore));
			return createdSymbolList.back().get();
		}

		void SymbolManager::CacheTypeObjectSymbolMap(ParserSymbol * , GeneralTypeObject * type, ParserSymbol * typeSymbol)
		{
			assert(typeSymbolMap.find(type) == typeSymbolMap.end());
			typeSymbolMap.insert(make_pair(type, typeSymbol));
		}

		ParserSymbol * SymbolManager::GetCacheTypeObjectSymbol(ParserSymbol * , GeneralTypeObject * type)
		{
			auto findScope = typeSymbolMap.find(type);
			return (findScope == typeSymbolMap.end()) ?nullptr : findScope->second;
		}

		void SymbolManager::CacheRuleNameToSymbolMap(const wstring & name, ParserSymbol * symbol)
		{
			assert(ruleNameSymbolMap.find(name) == ruleNameSymbolMap.end());
			this->ruleNameSymbolMap.insert({ name, symbol });
		}

		void SymbolManager::CacheTokenNameToSymbolMap(const wstring & name, ParserSymbol * symbol)
		{
			assert(tokenNameSymbolMap.find(name) == tokenNameSymbolMap.end());
			this->tokenNameSymbolMap.insert({ name, symbol });
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
			return (findIter == ruleNameSymbolMap.end()) ?  nullptr : findIter->second;
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
			this->grammarNodeDefSymbolMap.insert({textGrammar,tokenDefSymbol});
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
		ParserSymbol* SymbolManager::GetCacheNormalGrammarToRuleDefSymbol(GeneralGrammarTypeDefine* normalGrammar)
		{
			auto findIter = grammarNodeDefSymbolMap.find(normalGrammar);
			return findIter == grammarNodeDefSymbolMap.end()?nullptr:findIter->second;
		}


	/*	ParserSymbol * SymbolManager::AddRegexDefine(const wstring & regex)
		{
			return CreatASymbol(SymbolType::RegexDef, regex, nullptr, nullptr);
		}*/
		

		void SymbolManager::CheckNameReDefineError(const wstring& name, ParserSymbol * parentSymbol)
		{
			
			auto result = parentSymbol->GetSubSymbolByName(name);
			if (result)
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
				if (result)
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