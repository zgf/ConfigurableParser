#pragma once
#include "stdafx.h"
#include "GeneralTableDefine.h"

namespace ztl
{
	namespace general_parser
	{
		class ParserSymbol;
		enum class SymbolType;
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
			using TypeObjectToSymbolMapType = unordered_map<GeneralTypeObject*, ParserSymbol*>;
			using TypeDefineToSymbolMapType = unordered_map<GeneralTypeDefine*, ParserSymbol*>;
			using SymbolToTypeDefineMapType = unordered_map<  ParserSymbol*, GeneralTypeDefine*>;

			using RuleNameToSymbolMapType = unordered_map<wstring, ParserSymbol*>;
			using TokenNameToSymbolMapType = unordered_map<wstring, ParserSymbol*>;
			using RegexStringToSymbolMapType = unordered_map<wstring, ParserSymbol*>;
			using DefSymbolToRuleNodeMapType = unordered_map< ParserSymbol*, GeneralRuleDefine*>;
			using RuleNodeToDefSymbolMapType = unordered_map< GeneralRuleDefine*, ParserSymbol*>;
			using GrammarNodeToDefSymbolMapType = unordered_map<GeneralGrammarTypeDefine*, ParserSymbol*>;
			//using AbsoluteNameToSymbolType = unordered_map<wstring, ParserSymbol*>;
			//收集继承关系.
			using  BaseSymbolToDeriveSymbolMapType = unordered_map<ParserSymbol*, vector<ParserSymbol*>>;
		private:
			shared_ptr<GeneralTableDefine>   table;
			vector<shared_ptr<ParserSymbol>> createdSymbolList;//符号的对象池
			ParserSymbol*					 globalSymbol;//根符号
			ParserSymbol*					 tokenTypeSymbol;//token类型符号
			//作用域内类型到符号的映射.因为Symbol->type类型被擦除了,所以这样绑定后,下次不用使用visitor FindType去查看实际类型了.
			vector<wstring>						 startRuleList;
			BaseSymbolToDeriveSymbolMapType		 baseSymbolToDeriveMap;//类和类的派生类继承链收集
			//符号缓存
			TypeObjectToSymbolMapType			 typeSymbolMap;
			TypeDefineToSymbolMapType			 typeDefSymbolMap;//类型定义到符号的绑定
			SymbolToTypeDefineMapType			 symbolTypeDefMap;//从符号到类型定义的绑定

			RuleNameToSymbolMapType				 ruleNameSymbolMap;
			TokenNameToSymbolMapType			 tokenNameSymbolMap;
			TokenNameToSymbolMapType			 disTokenNameSymbolMap;
			DefSymbolToRuleNodeMapType           symbolRuleDefineMap;
			GrammarNodeToDefSymbolMapType		 grammarNodeDefSymbolMap;
			RegexStringToSymbolMapType			 regexSymbolMap;
			unordered_map<wstring, vector<wstring>>propertyToValueMap;//headInfoMap property value;

		public:
			SymbolManager();
			SymbolManager(const shared_ptr<GeneralTableDefine>& _table);
		public:
			const vector<shared_ptr<GeneralTokenDefine>>& GetTokens() const;
			const vector<shared_ptr<GeneralTypeDefine>>& GetTypes() const;
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
				const wstring & name, ParserSymbol * parent, ParserSymbol * descriptor, bool ignore);
			void		  CacheTypeObjectSymbolMap(GeneralTypeObject* type, ParserSymbol* typeSymbol);
			ParserSymbol* GetCacheTypeObjectSymbol(GeneralTypeObject* type);

			void		  CacheTypeDefineAndSymbolMap(GeneralTypeDefine* type, ParserSymbol* typeSymbol);
			ParserSymbol* GetCacheSymbolByTypeDefine(GeneralTypeDefine* type);
			GeneralTypeDefine* GetCacheTypeDefineBySymbol(ParserSymbol* type);

			void CheckNameReDefineError(const wstring& name, ParserSymbol* parentSymbol);
			ParserSymbol* CheckNameHasDefine(const wstring name, ParserSymbol* scope);

			void CacheRuleDefineAndSymbolMap(GeneralRuleDefine* ruleDef, ParserSymbol* symbol);
			GeneralRuleDefine* GetCacheRuleDefineBySymbol(ParserSymbol* symbol);

			ParserSymbol* GetCacheRuleNameToSymbol(const wstring& name)const;
			ParserSymbol* GetCacheTokenNameToSymbol(const wstring& name)const;
			ParserSymbol* GetCacheRegexStringToSymbol(const wstring& name)const;
			ParserSymbol* GetCacheDisTokenNameSymbol(const wstring& name)const;
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
			ParserSymbol* GetCacheUsingGrammarToRuleDefSymbol(GeneralGrammarTypeDefine* usingGrammar);
			ParserSymbol* GetCacheNormalGrammarToRuleDefSymbol(GeneralGrammarTypeDefine* normalGrammar);
			ParserSymbol* GetCacheNonTerminateGrammarToRuleDefSymbol(GeneralGrammarTypeDefine* terminateGrammar);
			ParserSymbol* GetCacheFieldDefSymbolByGrammar(GeneralGrammarTypeDefine* grammar);
			void				  CachePropertyToValueMap(const wstring& property, const vector<wstring>& value);
			void				  CachePropertyToValueMap(const wstring& property, const wstring& value);
			vector<wstring>		  GetCacheValueByProperty(const wstring& property)const;
			void				  CacheBaseSymbolToDeriveMap(ParserSymbol* baseSymbol, ParserSymbol* deriveSymbol);
			vector<ParserSymbol*>		  GetCacheDeriveByBaseSymbol(ParserSymbol* baseSymbol)const;
			vector<ParserSymbol*>		  GetCacheAllDeriveByBaseSymbol(ParserSymbol* baseSymbol)const;

			unordered_map<wstring, vector<wstring>>& GetPropertyToValueMap();
			const TypeDefineToSymbolMapType& GetTypeDefSymbolMap()const;
			const BaseSymbolToDeriveSymbolMapType&	GetbaseSymbolToDeriveMap()const;
		private:
			void TryAddSubSymbol(ParserSymbol* subSymbol, ParserSymbol* parentSymbol);
			void CacheRuleNameToSymbolMap(const wstring& name, ParserSymbol* symbol);
			void CacheTokenNameToSymbolMap(const wstring& name, ParserSymbol* symbol);
			void CacheDisTokenNameSymbolMap(const wstring& name, ParserSymbol* symbol);
			void CacheRegexStringToSymbolMap(const wstring& name, ParserSymbol* symbol);
		};
		wstring LinearStringToRegex(const wstring&);
		ParserSymbol* FindType(SymbolManager* manager, ParserSymbol* scope, GeneralTypeObject* type);
		void ValidateGeneratorCoreSemantic(SymbolManager* manager);
		void CollectHeadInfo(SymbolManager* manager);
		void ValidateHeadInfo(SymbolManager* manager);
		void CollectAndValidateTypeDefine(SymbolManager* manager);
		void ValidateGrammarNode(SymbolManager* manager);
		unordered_map<GeneralRuleDefine*, vector<unique_ptr<GeneratePath>>> CollectGeneratePath(SymbolManager* manager);
		void ValidateGeneratePathStructure(SymbolManager * manager, unordered_map<GeneralRuleDefine*, vector<unique_ptr<GeneratePath>>>& pathMap);
		void AnalyzeClassChoiceField(SymbolManager* manager);
		void GetStartSymbol(SymbolManager* manager);
	}
}