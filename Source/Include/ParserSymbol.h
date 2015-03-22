#pragma once
#include "stdafx.h"
namespace ztl
{
	namespace general_parser
	{
		enum class SymbolType
		{
			Global,
			EnumType,
			ClassType,		// descriptor == base type
			ArrayType,		// descriptor == element type
			TokenType,      //
			RegexDef,		// name == regex
			EnumDef,		// descriptor == parent
			FieldDef,		// descriptor == field type
			TokenDef,		// descriptor == RegexDef
			RuleDef,		// descriptor == rule type
		};
		class SymbolManager;
		class ParserSymbol
		{
			friend class SymbolManager;
		public:
			
		private:
			SymbolManager*										manager;
			SymbolType											type;
			wstring												name;
			ParserSymbol*										parent;
			ParserSymbol*										descriptor;
			bool												ignore;//token ignore.
			unique_ptr<unordered_map<wstring, ParserSymbol*>>	subSymbolMap;
		public:
			ParserSymbol() = delete;
			ParserSymbol(SymbolManager * _manager, SymbolType _type,
				const wstring & _name, ParserSymbol * _parent, ParserSymbol * _descriptor);
				
			ParserSymbol(SymbolManager * _manager, SymbolType _type,
				const wstring & _name, ParserSymbol * _parent, ParserSymbol * _descriptor,bool _ignore);
			~ParserSymbol() = default;
			ParserSymbol(ParserSymbol&&) = default;
			ParserSymbol(const ParserSymbol&) = default;
			ParserSymbol& operator=(const ParserSymbol&) = default;
			ParserSymbol& operator=(ParserSymbol&&) = default;
		public:

			SymbolManager*							GetManager()							 const;
			SymbolType								GetType()								 const;
			wstring									GetTypeToWString()						 const;
			const wstring&							GetName()								 const;
			ParserSymbol*							GetParentSymbol()						 const;
			ParserSymbol*							GetDescriptorSymbol()					 const;
			void									AddSubSymbol(ParserSymbol* subSymbol);
			void									SetDescriptor(ParserSymbol* _descriptor);

			ParserSymbol*							GetSubSymbolByName(const wstring& _name)	 const;
			size_t									SubSymbolCount()						 const;
			ParserSymbol*							SearchClassSubSymbol(const wstring& _name)const;
			vector<wstring>							GetClassAllFieldName()const;
		public:
			bool									IsType()     const;
			bool									IsDefine()   const;
			bool									IsGlobal()   const;
			bool									IsEnumType() const;
			bool									IsClassType()const;
			bool									IsArrayType()const;
			bool									IsTokenType()const;
			bool									IsRegexDef() const;
			bool									IsEnumDef()  const;
			bool									IsFieldDef() const;
			bool									IsTokenDef() const;
			bool									IsRuleDef()  const;
			bool									IsIgnore() const;

			bool EqualToBaseClassType(ParserSymbol* baseDef);
			ParserSymbol* SearchClassFieldSymbol(const wstring& _name);
		};
	}
}