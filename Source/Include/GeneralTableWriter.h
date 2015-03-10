#pragma once
#include "stdafx.h"
#include "GeneralTableDefine.h"
namespace ztl
{
	namespace general_parser
	{
		namespace table_writer
		{
			//�﷨����д����

			//Head��д
			//include : name
			//classprefix : prefix
			//namespace : name
			//filename : name
			struct GeneralHeadInfoWriter
			{
				vector<shared_ptr<GeneralHeadInfoDefine>> heads;
			public:
				GeneralHeadInfoWriter& Info(const wstring& property, const wstring& value);
			};

			//Token��д
			struct GeneralTokenWriter
			{
				vector<shared_ptr<GeneralTokenDefine>> tokens;
			public:
				GeneralTokenWriter& Token(const wstring& name, const wstring& regex);
				GeneralTokenWriter& IgnoreToken(const wstring& name, const wstring& regex);
			private:
				GeneralTokenWriter& Token(const wstring& name, const wstring& regex, GeneralTokenDefine::TokenOptional ignore);
			};
			//TypeObject��д
			shared_ptr<GeneralArrayTypeObject>			Array(const shared_ptr<GeneralTypeObject>& elementType);
			shared_ptr<GeneralClassMemberTypeDenfine>	ClassMember(const shared_ptr<GeneralTypeObject>& type, const wstring& name);
			shared_ptr<GeneralEnumMemberTypeDefine>		EnumMember(const wstring& name);
			shared_ptr<GeneralStringTypeObject>			String();
			shared_ptr<GeneralSubTypeObject>			SubTypeObject(const shared_ptr<GeneralTypeObject>& parent, const wstring& name);
			shared_ptr<GeneralNormalTypeObject> Normal(const wstring& name);
			//Type��д
			struct GeneralClassTypeWriter;
			struct GeneralEnumTypeWriter;
			struct GeneralTypeListWriter
			{
				vector<shared_ptr<GeneralTypeDefine>> types;

			public:
				GeneralTypeListWriter& Class(const GeneralClassTypeWriter& writer);
				GeneralTypeListWriter& Enum(const GeneralEnumTypeWriter& writer);
			};

			struct GeneralClassTypeWriter
			{
				shared_ptr<GeneralClassTypeDefine> _struct;
			public:
				GeneralClassTypeWriter::GeneralClassTypeWriter() :_struct(make_shared<GeneralClassTypeDefine>())
				{
				}
			public:
				GeneralClassTypeWriter& Name(const wstring& name);
				GeneralClassTypeWriter& Member(const shared_ptr<GeneralClassMemberTypeDenfine>& member);
				GeneralClassTypeWriter& SubType(const GeneralTypeListWriter& writer);
				GeneralClassTypeWriter& ParentType(const shared_ptr<GeneralTypeObject>& type);
			};

			struct GeneralEnumTypeWriter
			{
				shared_ptr<GeneralEnumTypeDefine> _enum;
			public:
				GeneralEnumTypeWriter::GeneralEnumTypeWriter() :_enum(make_shared<GeneralEnumTypeDefine>())
				{
				}
			public:
				GeneralEnumTypeWriter& Name(const wstring& name);
				GeneralEnumTypeWriter& Member(const shared_ptr<GeneralEnumMemberTypeDefine>& member);
			};

			//������д
			struct GeneralRuleWriter;
			struct GeneralRuleListWriter
			{
				vector<shared_ptr<GeneralRuleDefine>> rules;
			public:
				GeneralRuleListWriter& Rule(const GeneralRuleWriter& writer);
			};

			struct GeneralGrammarWriter;
			struct GeneralRuleWriter
			{
				shared_ptr<GeneralRuleDefine> rule;
			public:
				GeneralRuleWriter::GeneralRuleWriter() :rule(make_shared<GeneralRuleDefine>())
				{
				}
			public:
				GeneralRuleWriter& Name(const wstring& name);
				GeneralRuleWriter& ReturnType(const shared_ptr<GeneralTypeObject>& type);

				GeneralRuleWriter& operator|(const GeneralGrammarWriter& writer);
				GeneralRuleWriter& Grammar(const GeneralGrammarWriter& writer);
			};

			//�ķ�������д
			struct GeneralSetterGrammarWriter;
			struct GeneralGrammarWriter
			{
				shared_ptr<GeneralGrammarTypeDefine> grammar;
			public:
				GeneralGrammarWriter::GeneralGrammarWriter() :grammar(nullptr)
				{
				}
			public:
				
				//���� As
				GeneralGrammarWriter&		Create(const shared_ptr<GeneralTypeObject>& type);
				//��ֵ
				GeneralGrammarWriter&		operator[](const wstring& name);
				GeneralGrammarWriter& Setter(const wstring& name, const wstring& value);
			};

			
			//����+
			GeneralGrammarWriter operator+(const GeneralGrammarWriter& first, const GeneralGrammarWriter& second);
			//using!
			GeneralGrammarWriter operator!(const GeneralGrammarWriter& writer);
			//ѭ��*
			GeneralGrammarWriter operator*(const GeneralGrammarWriter& writer);
			//��ѡ~
			GeneralGrammarWriter operator~(const GeneralGrammarWriter& writer);
			//�ս����regex
			GeneralGrammarWriter Text(const wstring& text);
			

			//�ս���ź�tokenName
			GeneralGrammarWriter GrammarSymbol(const wstring& name);

			GeneralGrammarWriter operator|(const GeneralGrammarWriter& left, const GeneralGrammarWriter& right);

			//struct GeneralTokenWriter;
			//struct GeneralTypeWriter;
			//struct GeneralRuleWriter;
			//Table��д
			struct GeneralTableWriter
			{
				shared_ptr<GeneralTableDefine>	table;
			public:
				GeneralTableWriter::GeneralTableWriter() :table(make_shared<GeneralTableDefine>())
				{
				}
			public:
				GeneralTableWriter& Token(const GeneralTokenWriter& writer);
				GeneralTableWriter& Type(const GeneralTypeListWriter& writer);
				GeneralTableWriter& RuleList(const GeneralRuleListWriter& writer);
				GeneralTableWriter& Head(const GeneralHeadInfoWriter& writer);
			};
			
		}
	}
}