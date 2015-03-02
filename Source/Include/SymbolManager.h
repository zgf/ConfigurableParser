#pragma once
#include "stdafx.h"
#include "GeneralTableDefine.h"
#include "Include\ParserSymbolDefine.h"

namespace ztl
{
	namespace general_parser
	{
		class ParserSymbol;
		enum class SymbolType;
		//PAIR_BUILDER(ScopeTypePair, ParserSymbol*, scope, ParserSymbol*, type);
		class GeneratePath;
		class SymbolManager
		{
		public:
			enum class TagType
			{
				terminate,
				nonterminate,
			};
		private:
			vector<wstring>					tagToStringMap;
			unordered_map<wstring, int>		stringToTagMap;
			vector<TagType>					tagTypeList;
		

			using TypeObjectToSymbolMapType = unordered_map<GeneralTypeObject*, ParserSymbol*>;
			using RuleNameToSymbolMapType = unordered_map<wstring, ParserSymbol*>;
			using TokenNameToSymbolMapType = unordered_map<wstring, ParserSymbol*>;
			using RegexStringToSymbolMapType = unordered_map<wstring, ParserSymbol*>;
			using DefSymbolToRuleNodeMapType = unordered_map< ParserSymbol*,GeneralRuleDefine*>;
			using RuleNodeToDefSymbolMapType = unordered_map< GeneralRuleDefine*, ParserSymbol*>;
			using GrammarNodeToDefSymbolMapType = unordered_map<GeneralGrammarTypeDefine*, ParserSymbol*>;


		private:
			shared_ptr<GeneralTableDefine>   table;
			vector<shared_ptr<ParserSymbol>> createdSymbolList;//符号的对象池
			ParserSymbol*					 globalSymbol;//根符号
			ParserSymbol*					 tokenTypeSymbol;//token类型符号 
			
			//作用域内类型到符号的映射.因为Symbol->type类型被擦除了,所以这样绑定后,下次不用使用visitor FindType去查看实际类型了.

			//SymbolToClassDefineMapType		 symbolClassDefineMap;//从符号到类定义的绑定
			//ClassDefineToSymbolMapType		 classDefineSymbolMap;//从类定义到符号的绑定
			vector<wstring>						 startRuleList;
			
			//符号缓存
			TypeObjectToSymbolMapType			 typeSymbolMap;
			RuleNameToSymbolMapType				 ruleNameSymbolMap;
			TokenNameToSymbolMapType			 tokenNameSymbolMap;
			TokenNameToSymbolMapType			 disTokenNameSymbolMap;
			DefSymbolToRuleNodeMapType           symbolRuleDefineMap;
			GrammarNodeToDefSymbolMapType		 grammarNodeDefSymbolMap;
			RegexStringToSymbolMapType			 regexSymbolMap;
			unordered_map<wstring, int>			 nameToTagMap;
			vector<wstring>						 tagToNameList;

			
		public:
			SymbolManager();
			SymbolManager(const shared_ptr<GeneralTableDefine>& _table);
		public:
			const vector<shared_ptr<GeneralTokenDefine>>& GetTokens() const;
			const vector<shared_ptr<GeneralTypeDefine>>& GetTypes() const;
			int GetTagByString(const wstring& name)const;
			wstring GetStringByTag(int index)const;
			bool IsLexerTag(int index)const;
		private:
			void SetNameToMap(const wstring& name, TagType type);
			void CreatStringAndTagMap();
			unordered_map<wstring, LexTokenDefine> GetLexTokenDefine();


		public:
			
			vector<wstring>& GetStartRuleList();
			
			void SetStartRuleList(vector<wstring> val);
		
			GeneralTableDefine* GetTable();

			ParserSymbol* GetGlobalSymbol();
			ParserSymbol* GetTokenTypeSymbol();


			ParserSymbol* AddClass(const wstring& name, ParserSymbol* baseType, ParserSymbol* parentType);
			ParserSymbol* AddField(const wstring& name, ParserSymbol* fieldType, ParserSymbol* parentType);
			ParserSymbol* AddEnum(const wstring& name, ParserSymbol* parentType);
			ParserSymbol* AddEnumItem(const wstring& name, ParserSymbol* parentType);
			ParserSymbol* AddRuleDefine(const wstring& name, ParserSymbol* ruleType);
			ParserSymbol* AddTokenDefine(const wstring& name, const wstring& regex, bool ignore);
			ParserSymbol* AddArrayType(ParserSymbol* elementType);
			ParserSymbol* CreatASymbol(SymbolType type,
				const wstring & name, ParserSymbol * parent, ParserSymbol * descriptor);
			ParserSymbol* CreatASymbol(SymbolType type,
				const wstring & name, ParserSymbol * parent, ParserSymbol * descriptor,bool ignore);
			void		  CacheTypeObjectSymbolMap(ParserSymbol* scope, GeneralTypeObject* type, ParserSymbol* typeSymbol);
			ParserSymbol* GetCacheTypeObjectSymbol(ParserSymbol* scope, GeneralTypeObject* type);

			void CheckNameReDefineError(const wstring& name, ParserSymbol* parentSymbol);
			ParserSymbol* CheckNameHasDefine(const wstring name, ParserSymbol* scope);

			//void CacheClassDefineAndSymbolMap(GeneralClassTypeDefine* classDef, ParserSymbol* symbol);
			void CacheRuleDefineAndSymbolMap(GeneralRuleDefine* ruleDef, ParserSymbol* symbol);
			GeneralRuleDefine* GetCacheRuleDefineBySymbol(ParserSymbol* symbol);
			//void CacheEnumTypeAndSymbolMap(GeneralEnumTypeDefine* enumType, ParserSymbol* symbol);
			//void CacheTokenDefineAndSymbolMap(GeneralTokenDefine* tokenDef, ParserSymbol* symbol);

			
			ParserSymbol* GetCacheRuleNameToSymbol(const wstring& name)const;
			ParserSymbol* GetCacheTokenNameToSymbol(const wstring& name)const;
			ParserSymbol* GetCacheRegexStringToSymbol(const wstring& name)const;
			ParserSymbol* GetCacheDisTokenNameSymbol(const wstring& name)const;
			void		  CacheNameAndTagMap(const wstring symbolName);
			int			  GetCacheTagByName(const wstring symbolName);
			wstring GetCacheNameByTag(size_t tag);

			//setter or assign
			void		  CacheGrammarToFieldDefSymbol(GeneralGrammarTypeDefine* grammar, ParserSymbol* fieldDefSymbol);
			//text
			void		  CacheTextGrammarToTokenDefSymbol(GeneralGrammarTypeDefine* textGrammar, ParserSymbol* tokenDefSymbol);
			//normal token
			void		  CacheNormalGrammarToTokenDefSymbol(GeneralGrammarTypeDefine* normalGrammar, ParserSymbol* tokenDefSymbol);
			//normal rule
			void		  CacheNormalGrammarToRuleDefSymbol(GeneralGrammarTypeDefine* normalGrammar, ParserSymbol* ruleDefSymbol);
			//using rule
			void		  CacheUsingGrammarToRuleDefSymbol(GeneralGrammarTypeDefine* usingGrammar, ParserSymbol* ruleDefSymbol);
			void		  CacheNameAndTagMap();

			ParserSymbol* GetCacheUsingGrammarToRuleDefSymbol(GeneralGrammarTypeDefine* usingGrammar);
			ParserSymbol* GetCacheNormalGrammarToRuleDefSymbol(GeneralGrammarTypeDefine* normalGrammar);
			ParserSymbol* GetCacheNonTerminateGrammarToRuleDefSymbol(GeneralGrammarTypeDefine* terminateGrammar);


		private:
			void TryAddSubSymbol(ParserSymbol* subSymbol, ParserSymbol* parentSymbol);

			void CacheRuleNameToSymbolMap(const wstring& name, ParserSymbol* symbol);
			void CacheTokenNameToSymbolMap(const wstring& name, ParserSymbol* symbol);
			void CacheDisTokenNameSymbolMap(const wstring& name, ParserSymbol* symbol);
			void CacheRegexStringToSymbolMap(const wstring& name, ParserSymbol* symbol);
		};

		ParserSymbol* FindType(SymbolManager* manager,ParserSymbol* scope,GeneralTypeObject* type);
		void CollectAndValidateTypeDefine(SymbolManager* manager);
		wstring LinearStringToRegex(const wstring&);
		void ValidateGrammarNode(SymbolManager* manager);
		void ValidateGeneratePathStructure(SymbolManager * manager, unordered_map<GeneralRuleDefine*, vector<unique_ptr<GeneratePath>>>& pathMap);
		unordered_map<GeneralRuleDefine*, vector<unique_ptr<GeneratePath>>> CollectGeneratePath(SymbolManager* manager);
		void ValidateGeneratorCoreSemantic(SymbolManager* manager);
		void GetStartSymbol(SymbolManager* manager);
	}
}