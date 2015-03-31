#pragma once
#include "Include\stdafx.h"
#include "Include\GeneralTableWriter.h"
namespace ztl
{
	namespace general_parser
	{
		//GeneralTokenWriter
		GeneralTokenWriter& GeneralTokenWriter::Token(const wstring& name, const wstring& regex, GeneralTokenDefine::TokenOptional ignore)
		{
			auto result = make_shared<GeneralTokenDefine>();
			result->ignore = ignore;
			result->name = name;
			result->regex = regex;
			this->tokens.emplace_back(move(result));
			return *this;
		}
		GeneralTokenWriter& GeneralTokenWriter::IgnoreToken(const wstring& name, const wstring& regex)
		{
			return Token(name, regex, GeneralTokenDefine::TokenOptional::True);
		}
		GeneralTokenWriter& GeneralTokenWriter::Token(const wstring& name, const wstring& regex)
		{
			return Token(name, regex, GeneralTokenDefine::TokenOptional::False);
		}

		//手写Object对象
		shared_ptr<GeneralArrayTypeObject>			Array(const shared_ptr<GeneralTypeObject>& elementType)
		{
			auto result = make_shared<GeneralArrayTypeObject>();
			result->element = elementType;
			return result;
		}
		shared_ptr<GeneralClassMemberTypeDefine>	ClassMember(const shared_ptr<GeneralTypeObject>& type, const wstring& name)
		{
			auto result = make_shared<GeneralClassMemberTypeDefine>();
			result->type = type;
			result->name = name;
			return result;
		}
		shared_ptr<GeneralEnumMemberTypeDefine>		EnumMember(const wstring& name)
		{
			auto result = make_shared<GeneralEnumMemberTypeDefine>();
			result->name = name;
			return result;
		}
		shared_ptr<GeneralNormalTypeObject> Normal(const wstring& name)
		{
			auto result = make_shared<GeneralNormalTypeObject>();
			result->name = name;
			return result;
		}
		shared_ptr<GeneralStringTypeObject>			String()
		{
			auto result = make_shared<GeneralStringTypeObject>();
			return result;
		}
		shared_ptr<GeneralSubTypeObject>			SubTypeObject(const shared_ptr<GeneralTypeObject>& parent, const wstring& name)
		{
			auto result = make_shared<GeneralSubTypeObject>();
			result->name = name;
			result->parent = parent;
			return result;
		}

		//GeneralTypeListWriter

		GeneralTypeListWriter& GeneralTypeListWriter::Class(const GeneralClassTypeWriter& writer)
		{
			this->types.emplace_back(writer._struct);
			return *this;
		}
		GeneralTypeListWriter& GeneralTypeListWriter::Enum(const GeneralEnumTypeWriter& writer)
		{
			this->types.emplace_back(writer._enum);
			return *this;
		}
		//GeneralClassTypeWriter

		GeneralClassTypeWriter& GeneralClassTypeWriter::Name(const wstring& name)
		{
			this->_struct->name = name;
			return *this;
		}

		GeneralClassTypeWriter& GeneralClassTypeWriter::Member(const shared_ptr<GeneralClassMemberTypeDefine>& member)
		{
			this->_struct->members.emplace_back(member);
			return *this;
		}

		GeneralClassTypeWriter& GeneralClassTypeWriter::SubType(const GeneralTypeListWriter& writer)
		{
			this->_struct->subTypes = writer.types;
			return *this;
		}

		GeneralClassTypeWriter& GeneralClassTypeWriter::ParentType(const shared_ptr<GeneralTypeObject> & type)
		{
			this->_struct->parent = type;
			return *this;
		}
		//GeneralEnumTypeWriter

		GeneralEnumTypeWriter& GeneralEnumTypeWriter::Name(const wstring& name)
		{
			this->_enum->name = name;
			return *this;
		}

		GeneralEnumTypeWriter& GeneralEnumTypeWriter::Member(const shared_ptr<GeneralEnumMemberTypeDefine>& member)
		{
			this->_enum->members.emplace_back(member);
			return *this;
		}
		//GeneralRuleListWriter
		GeneralRuleListWriter& GeneralRuleListWriter::Rule(const GeneralRuleWriter& writer)
		{
			this->rules.emplace_back(writer.rule);
			return *this;
		}

		//GeneralRuleWriter

		GeneralRuleWriter& GeneralRuleWriter::Name(const wstring& name)
		{
			this->rule->name = name;
			return *this;
		}

		GeneralRuleWriter& GeneralRuleWriter::ReturnType(const shared_ptr<GeneralTypeObject>& type)
		{
			this->rule->type = type;
			return *this;
		}
		GeneralRuleWriter& GeneralRuleWriter::Grammar(const GeneralGrammarWriter& writer)
		{
			return *this | writer;
		}

		GeneralRuleWriter& GeneralRuleWriter::operator|(const GeneralGrammarWriter& writer)
		{
			this->rule->grammars.emplace_back(writer.grammar);
			return *this;
		}

		//GeneralSetterGrammarWriter
		GeneralGrammarWriter& GeneralGrammarWriter::Setter(const wstring& name, const wstring& value)
		{
			auto result = make_shared<GeneralGrammarSetterTypeDefine>();
			result->grammar = this->grammar;
			result->name = name;
			result->value = value;
			this->grammar = move(result);
			return *this;
		}

		GeneralGrammarWriter& GeneralGrammarWriter::Create(const shared_ptr<GeneralTypeObject>& type)
		{
			auto result = make_shared<GeneralGrammarCreateTypeDefine>();
			result->type = type;
			result->grammar = grammar;
			grammar = move(result);
			return *this;
		}

		GeneralGrammarWriter& GeneralGrammarWriter::operator[](const wstring& name)
		{
			auto result = make_shared<GeneralGrammarAssignTypeDefine>();
			result->name = name;
			result->grammar = grammar;
			grammar = move(result);
			return *this;
		}

		//序列+
		GeneralGrammarWriter operator+(const GeneralGrammarWriter& first, const GeneralGrammarWriter& second)
		{
			GeneralGrammarWriter writer;
			auto result = make_shared<GeneralGrammarSequenceTypeDefine>();
			result->first = first.grammar;
			result->second = second.grammar;
			writer.grammar = move(result);
			return writer;
		}
		//using!
		GeneralGrammarWriter operator!(const GeneralGrammarWriter& writer)
		{
			GeneralGrammarWriter grammarWriter;
			auto result = make_shared<GeneralGrammarUsingTypeDefine>();
			result->grammar = writer.grammar;
			grammarWriter.grammar = move(result);
			return grammarWriter;
		}
		//循环*
		GeneralGrammarWriter operator*(const GeneralGrammarWriter& writer)
		{
			GeneralGrammarWriter grammarWriter;
			auto result = make_shared<GeneralGrammarLoopTypeDefine>();
			result->grammar = writer.grammar;
			grammarWriter.grammar = move(result);
			return grammarWriter;
		}
		//可选~
		GeneralGrammarWriter operator~(const GeneralGrammarWriter& writer)
		{
			GeneralGrammarWriter grammarWriter;
			auto result = make_shared<GeneralGrammarOptionalTypeDefine>();
			result->grammar = writer.grammar;
			grammarWriter.grammar = move(result);
			return grammarWriter;
		}
		//终结符号
		GeneralGrammarWriter Text(const wstring& text)
		{
			GeneralGrammarWriter grammarWriter;
			auto result = make_shared<GeneralGrammarTextTypeDefine>();
			result->text = text;
			grammarWriter.grammar = move(result);
			return grammarWriter;
		}
		//终结符号
		GeneralGrammarWriter GrammarSymbol(const wstring& name)
		{
			GeneralGrammarWriter grammarWriter;
			auto result = make_shared<GeneralGrammarNormalTypeDefine>();
			result->name = name;
			grammarWriter.grammar = move(result);
			return grammarWriter;
		}
		GeneralGrammarWriter operator|(const GeneralGrammarWriter& left, const GeneralGrammarWriter& right)
		{
			GeneralGrammarWriter grammarWriter;
			auto result = make_shared<GeneralGrammarAlternationTypeDefine>();
			result->left = left.grammar;
			result->right = right.grammar;
			grammarWriter.grammar = move(result);
			return grammarWriter;
		}

		GeneralTableWriter& GeneralTableWriter::Token(const GeneralTokenWriter& writer)
		{
			table->tokens.assign(writer.tokens.begin(), writer.tokens.end());
			return *this;
		}
		GeneralTableWriter& GeneralTableWriter::Type(const GeneralTypeListWriter& writer)
		{
			table->types = writer.types;
			return *this;
		}
		GeneralTableWriter& GeneralTableWriter::Rules(const GeneralRuleListWriter& writer)
		{
			table->rules = writer.rules;
			return *this;
		}
		GeneralTableWriter & GeneralTableWriter::Head(const GeneralHeadInfoWriter & writer)
		{
			// TODO: insert return statement here
			table->heads = writer.heads;
			return *this;
		}
		GeneralHeadInfoWriter& GeneralHeadInfoWriter::Info(const wstring & property, const wstring & value)
		{
			// TODO: insert return statement here
			auto result = make_shared<GeneralHeadInfoDefine>();
			result->property = property;
			result->value = value;
			heads.emplace_back(move(result));
			return *this;
		}
	}
}