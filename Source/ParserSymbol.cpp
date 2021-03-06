#include "Include\stdafx.h"
#include "Include\ParserSymbol.h"
#include "Include\SymbolManager.h"
#include "Include\GeneralGrammarBuilder.h"

namespace ztl
{
	namespace general_parser
	{
		int ParserSymbol::count = 0;
		ParserSymbol::ParserSymbol(SymbolManager * _manager, SymbolType _type,
			const wstring & _name, ParserSymbol * _parent, ParserSymbol * _descriptor)
			:manager(_manager), type(_type), name(_name), parent(_parent),
			descriptor(_descriptor),
			subSymbolMap(make_unique<unordered_map<wstring, ParserSymbol*>>())
		{
			number = count++;
		}

		ParserSymbol::ParserSymbol(SymbolManager * _manager, SymbolType _type, const wstring & _name, ParserSymbol * _parent, ParserSymbol * _descriptor, bool _ignore) :manager(_manager), type(_type), name(_name), parent(_parent),
			descriptor(_descriptor),
			subSymbolMap(make_unique<unordered_map<wstring, ParserSymbol*>>()), ignore(_ignore)
		{
			number = count++;
		}
		
		int ParserSymbol::GetNumber() const
		{
			return number;
		}

		SymbolManager * ztl::general_parser::ParserSymbol::GetManager()const
		{
			return this->manager;
		}


		SymbolType ParserSymbol::GetType()const
		{
			return this->type;
		}

		const wstring& ParserSymbol::GetName()const
		{
			return this->name;
		}

		ParserSymbol * ParserSymbol::GetParentSymbol()const
		{
			return this->parent;
		}

		ParserSymbol * ParserSymbol::GetDescriptorSymbol()const
		{
			return descriptor;
		}

		void ParserSymbol::AddSubSymbol(ParserSymbol * subSymbol)
		{
			assert(subSymbolMap != nullptr);
			subSymbol->parent = this;
			subSymbolMap->insert(make_pair(subSymbol->GetName(), subSymbol));
		}

		void ParserSymbol::SetDescriptor(ParserSymbol * _descriptor)
		{
			this->descriptor = _descriptor;
		}

	
		ParserSymbol * ParserSymbol::GetSubSymbolByName(const wstring & _name) const
		{
			assert(subSymbolMap != nullptr);
			auto&& findIter = subSymbolMap->find(_name);
			return (findIter == subSymbolMap->end()) ? nullptr : findIter->second;
		}

		size_t ParserSymbol::SubSymbolCount() const
		{
			return this->subSymbolMap->size();
		}

		bool ParserSymbol::IsType()const
		{
			return type == SymbolType::ArrayType ||
				   type == SymbolType::EnumType  || 
				   type == SymbolType::ClassType || 
				   type == SymbolType::TokenType;
		}

		bool ParserSymbol::IsDefine() const
		{
			return type== SymbolType::FieldDef||
				type == SymbolType::EnumDef||
				type == SymbolType::RegexDef||
				type == SymbolType::RuleDef||
				type == SymbolType::TokenDef;
		}

		bool ParserSymbol::IsGlobal() const
		{
			return type == SymbolType::Global;
		}

		bool ParserSymbol::IsEnumType() const
		{
			return type == SymbolType::EnumType;
		}

		bool ParserSymbol::IsClassType() const
		{
			return type == SymbolType::ClassType;
		}

		bool ParserSymbol::IsArrayType() const
		{
			return type == SymbolType::ArrayType;
		}

		bool ParserSymbol::IsTokenType() const
		{
			return type == SymbolType::TokenType;
		}

		bool ParserSymbol::IsRegexDef() const
		{
			return type == SymbolType::RegexDef;
		}

		bool ParserSymbol::IsEnumDef() const
		{
			return type == SymbolType::EnumDef;
		}

		bool ParserSymbol::IsFieldDef() const
		{
			return type == SymbolType::FieldDef;
		}

		bool ParserSymbol::IsTokenDef() const
		{
			return type == SymbolType::TokenDef;
		}

		bool ParserSymbol::IsRuleDef() const
		{
			return type == SymbolType::RuleDef;
		}

		bool ParserSymbol::IsIgnore() const
		{
			return this->ignore;
		}

		bool ParserSymbol::EqualToBaseClassType(ParserSymbol* baseDef)const
		{
			assert(baseDef->IsClassType()&&this->IsClassType());
			auto work = this;
			while(work!=nullptr)
			{
				if (work == baseDef)
				{
					return true;
				}
				work = work->GetDescriptorSymbol();
			}
			return false;
		}

		ParserSymbol* ParserSymbol::SearchClassFieldDefSymbol(const wstring& _name) const
		{
			auto result = SearchClassSubSymbol(_name);
			return (result != nullptr&&result->IsFieldDef())? result :nullptr;
		}

		

		ParserSymbol * ParserSymbol::SearchClassSubSymbol(const wstring & _name)const
		{
			assert(this->GetType() == SymbolType::ClassType);
			const ParserSymbol* scope = this;
			while(scope!=nullptr)
			{
				auto result = scope->GetSubSymbolByName(_name);
				if (result)
				{
					return result;
				}
				scope = scope->GetDescriptorSymbol();
			}
			return nullptr;
		}

		vector<ParserSymbol*> ParserSymbol::GetClassAllFieldDefSymbol() const
		{
			vector<ParserSymbol*> result;
			for (auto&& iter:*subSymbolMap)
			{
				if (iter.second->IsFieldDef())
				{
					result.emplace_back(iter.second);
				}
			}
			return result;
		}
		void ParserSymbol::SetAbsoluteScope(const wstring& scope)
		{
			assert(this->IsClassType() || this->IsEnumType() || this->IsEnumDef() || this->IsFieldDef());
			absoluteScope = scope;
		}
		wstring		ParserSymbol::GetSymbolAbsoluteName()const
		{
			return  (absoluteScope.empty()) ?name:absoluteScope + L"::" + name;
		}
		const unordered_map<wstring, ParserSymbol*>& ParserSymbol::GetSubSymbolMap() const
{
			return *subSymbolMap;
		}

		wstring			ParserSymbol::GetTypeToWString()	 const
		{
			wstring result;
			switch(type)
			{
				case ztl::general_parser::SymbolType::Global:
					result = L"Global";
					break;
				case ztl::general_parser::SymbolType::EnumType:
					result = L"EnumType";
					break;
				case ztl::general_parser::SymbolType::ClassType:
					result = L"ClassType";

					break;
				case ztl::general_parser::SymbolType::ArrayType:
					result = L"ArrayType";

					break;
				case ztl::general_parser::SymbolType::TokenType:
					result = L"TokenType";

					break;
				case ztl::general_parser::SymbolType::RegexDef:
					result = L"RegexDef";

					break;
				case ztl::general_parser::SymbolType::EnumDef:
					result = L"EnumDef";

					break;
				case ztl::general_parser::SymbolType::FieldDef:
					result = L"FieldDef";

					break;
				case ztl::general_parser::SymbolType::TokenDef:
					result = L"TokenDef";

					break;
				case ztl::general_parser::SymbolType::RuleDef:
					result = L"RuleDef";

					break;
				default:
					abort();
					break;
			}
			return result;
		}


	

}
}