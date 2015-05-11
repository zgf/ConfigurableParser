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
		private:
			using TypeObjectToSymbolMapType = unordered_map<GeneralTypeObject*, ParserSymbol*>;
			using TypeDefineToSymbolMapType = unordered_map<GeneralTypeDefine*, ParserSymbol*>;
			using SymbolToTypeDefineMapType = unordered_map<  ParserSymbol*, GeneralTypeDefine*>;
			using DefSymbolToRuleNodeMapType = unordered_map< ParserSymbol*, GeneralRuleDefine*>;
			using GrammarNodeToDefSymbolMapType = unordered_map<GeneralGrammarTypeDefine*, ParserSymbol*>;
			//收集继承关系.
			using  BaseSymbolToDeriveSymbolMapType = unordered_map<ParserSymbol*, vector<ParserSymbol*>>;
		private:
			shared_ptr<GeneralTableDefine>   table;
			vector<shared_ptr<ParserSymbol>> createdSymbolList;//符号的对象池
			ParserSymbol*					 globalSymbol;//根符号
			ParserSymbol*					 tokenTypeSymbol;//token类型符号
			vector<wstring>						 startRuleList;
			unordered_map<wstring, int>			 ruleToIndexMap;//rule to Index的Map
			vector<wstring>						 indexToruleList;//index to rule List
			BaseSymbolToDeriveSymbolMapType		 baseSymbolToDeriveMap;//类和类的派生类继承链收集
			//符号缓存
			TypeObjectToSymbolMapType			 typeSymbolMap;
			TypeDefineToSymbolMapType			 typeDefSymbolMap;//类型定义到符号的绑定
			SymbolToTypeDefineMapType			 symbolTypeDefMap;//从符号到类型定义的绑定
			DefSymbolToRuleNodeMapType           symbolRuleDefineMap;
			GrammarNodeToDefSymbolMapType		 grammarNodeDefSymbolMap;
			unordered_map<wstring, vector<wstring>>propertyToValueMap;//headInfoMap property value;
		public:
			SymbolManager();
			SymbolManager(const shared_ptr<GeneralTableDefine>& _table);
		public:
			const vector<shared_ptr<GeneralTokenDefine>>& GetTokens() const;
			const vector<shared_ptr<GeneralTypeDefine>>& GetTypes() const;
			vector<wstring>& GetStartRuleList();
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
			ParserSymbol* GetRuleSymbolByName(const wstring& name)const;
			ParserSymbol* GetTokenSymbolByName(const wstring& name)const;
			ParserSymbol* GetRegexSymbolByName(const wstring& name)const;
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
			wstring GetGenerateUniqueProperty(const wstring& property, const wstring& default = wstring());
			vector<wstring> GetGenerateArrayProperty(const wstring& property);
			const TypeDefineToSymbolMapType& GetTypeDefSymbolMap()const;
			const BaseSymbolToDeriveSymbolMapType&	GetbaseSymbolToDeriveMap()const;
			void AddRuleMap(const wstring& ruleName,int index);
			int GetRuleIndexByName(const wstring& ruleName)const;
			const wstring& GetRuleNameByIndex(int index)const;
			ParserSymbol* GetFinishTokenSymbol()const;
		private:
			void TryAddSubSymbol(ParserSymbol* subSymbol, ParserSymbol* parentSymbol);
			template<typename predicate_type>
			ParserSymbol * GetSymbolByName(const wstring & name, predicate_type predicator) const;
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
		void GetStartSymbol(SymbolManager* manager);
	}
}