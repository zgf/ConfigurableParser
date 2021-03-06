#pragma once
#include "stdafx.h"
#include "GeneralTableDefine.h"
namespace ztl
{
	namespace general_parser
	{
		//语法树手写工具

		//Head手写
		//include : name
		//classprefix : prefix
		//namespace : name
		//filename : name
		struct GeneralAttributeParamsWriter;
		struct GeneralAttributeWriter;
		struct GeneralHeadInfoWriter
		{
			vector<shared_ptr<GeneralHeadInfoDefine>> heads;
		public:
			GeneralHeadInfoWriter& Info(const wstring& property, const wstring& value);
			GeneralHeadInfoWriter& Info(const wstring& property, const wstring& value, const GeneralAttributeWriter& writer);
		};
		struct GeneralClassTypeWriter;
		struct GeneralEnumTypeWriter;
		
		//Token手写
		struct GeneralTokenWriter
		{
			vector<shared_ptr<GeneralTokenDefine>> tokens;
		public:
			GeneralTokenWriter& Token(const wstring& name, const wstring& regex);
			GeneralTokenWriter& Token(const wstring& name, const wstring& regex, const GeneralAttributeWriter& writer);
			GeneralTokenWriter& IgnoreToken(const wstring& name, const wstring& regex);
			GeneralTokenWriter& IgnoreToken(const wstring& name, const wstring& regex, const GeneralAttributeWriter& writer);
		private:
			GeneralTokenWriter& Token(const wstring& name, const wstring& regex, GeneralTokenDefine::TokenOptional ignore,const  GeneralAttributeWriter& writer);
		};
		//TypeObject手写
		shared_ptr<GeneralArrayTypeObject>			Array(const shared_ptr<GeneralTypeObject>& elementType);
		shared_ptr<GeneralClassMemberTypeDefine>	ClassMember(const shared_ptr<GeneralTypeObject>& type, const wstring& name);
		shared_ptr<GeneralClassMemberTypeDefine>	ClassMember(const shared_ptr<GeneralTypeObject>& type, const wstring& name,const GeneralAttributeWriter& writer);
		shared_ptr<GeneralEnumMemberTypeDefine>		EnumMember(const wstring& name);
		shared_ptr<GeneralEnumMemberTypeDefine>		EnumMember(const wstring& name,const GeneralAttributeWriter& writer);
		
		shared_ptr<GeneralTokenTypeObject>			String();
		shared_ptr<GeneralSubTypeObject>			SubTypeObject(const shared_ptr<GeneralTypeObject>& parent, const wstring& name);
		shared_ptr<GeneralNormalTypeObject> Normal(const wstring& name);
		//Type手写
		
		struct GeneralAttributeParamsWriter
		{
			vector<shared_ptr<GeneralAttributeArgumentDefine>> params;
			GeneralAttributeParamsWriter() 
			{

			}
		public:
			GeneralAttributeParamsWriter& Param(const wstring& param);

		};
		struct GeneralAttributeWriter
		{
			vector<shared_ptr<GeneralAttributeDefine>> attributes;
		public:
			GeneralAttributeWriter()
			{

			}
		public:
			GeneralAttributeWriter& Attribute(const wstring& name, const GeneralAttributeParamsWriter& writer);

			GeneralAttributeWriter& Attribute(const wstring& name);

		};
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
			GeneralClassTypeWriter& Member(const shared_ptr<GeneralClassMemberTypeDefine>& member);
			GeneralClassTypeWriter& SubType(const GeneralTypeListWriter& writer);
			GeneralClassTypeWriter& ParentType(const shared_ptr<GeneralTypeObject> & type);
			GeneralClassTypeWriter& Attributes(const GeneralAttributeWriter& writer);
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
			GeneralEnumTypeWriter& Attributes(const GeneralAttributeWriter& writer);
		};

		//规则手写
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
			GeneralRuleWriter& Attributes(const GeneralAttributeWriter& writer);
			GeneralRuleWriter& operator|(const GeneralGrammarWriter& writer);
			GeneralRuleWriter& Grammar(const GeneralGrammarWriter& writer);
		};
		
		//文法规则手写
		struct GeneralSetterGrammarWriter;
		struct GeneralGrammarWriter
		{
			shared_ptr<GeneralGrammarTypeDefine> grammar;
		public:
			GeneralGrammarWriter::GeneralGrammarWriter() :grammar(nullptr)
			{
			}
		public:

			//创建 As
			GeneralGrammarWriter&		Create(const shared_ptr<GeneralTypeObject>& type);
			//赋值
			GeneralGrammarWriter&		operator[](const wstring& name);
			GeneralGrammarWriter& Setter(const wstring& name, const wstring& value);
		};

		//序列+
		GeneralGrammarWriter operator+(const GeneralGrammarWriter& first, const GeneralGrammarWriter& second);
		//using!
		GeneralGrammarWriter operator!(const GeneralGrammarWriter& writer);
		//循环*
		GeneralGrammarWriter operator*(const GeneralGrammarWriter& writer);
		//可选~
		GeneralGrammarWriter operator~(const GeneralGrammarWriter& writer);
		//终结符号regex
		GeneralGrammarWriter Text(const wstring& text);

		//终结符号和tokenName
		GeneralGrammarWriter GrammarSymbol(const wstring& name);

		GeneralGrammarWriter operator|(const GeneralGrammarWriter& left, const GeneralGrammarWriter& right);

		//struct GeneralTokenWriter;
		//struct GeneralTypeWriter;
		//struct GeneralRuleWriter;
		//Table手写
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
			GeneralTableWriter& Rules(const GeneralRuleListWriter& writer);
			GeneralTableWriter& Head(const GeneralHeadInfoWriter& writer);
		};
	}
}