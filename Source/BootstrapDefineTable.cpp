#pragma once
#include "Include\stdafx.h"

#include "Include\GeneralTableWriter.h"
/*!
 * \file BootstrapDefineTable.cpp
 *
 * \author ZGF
 * \date 二月 2015
 *手写的序列化文法
 *
 */
namespace ztl
{
	namespace general_parser
	{
		//手写ENBF核心表

		shared_ptr<ztl::general_parser::GeneralTableDefine> BootStrapDefineTable()
		{
			ztl::general_parser::GeneralTableWriter writer;
			writer.Head
				(
					ztl::general_parser::GeneralHeadInfoWriter()

					.Info(LR"(filename)", LR"(GenerateParser)")

					.Info(LR"(include)", LR"(Include/stdafx.h)")

					.Info(LR"(namespace)", LR"(ztl)")

					.Info(LR"(namespace)", LR"(general_parser)")

					.Info(LR"(dirname)", LR"(General)")

					)
				.Token
				(
					ztl::general_parser::GeneralTokenWriter()
					.Token(L"FINISH", L"<\\$>")
					.Token(L"CLASS", LR"(class)",
						GeneralAttributeWriter().Attribute(L"Color",
							GeneralAttributeParamsWriter().Param(L"Keywords")))
					.Token(L"STRUCT", LR"(struct)",
						GeneralAttributeWriter().Attribute(L"Color",
							GeneralAttributeParamsWriter().Param(L"Keywords")))
					.Token(L"ENUM", LR"(enum)",
						GeneralAttributeWriter().Attribute(L"Color",
							GeneralAttributeParamsWriter().Param(L"Keywords")))
					.Token(L"TOKEN", LR"(token)",
						GeneralAttributeWriter().Attribute(L"Color",
							GeneralAttributeParamsWriter().Param(L"Keywords")))
					.Token(L"DISCARDTOKEN", LR"(ignoretoken)",
						GeneralAttributeWriter().Attribute(L"Color",
							GeneralAttributeParamsWriter().Param(L"Keywords")))
					.Token(L"RULE", LR"(rule)",
						GeneralAttributeWriter().Attribute(L"Color",
							GeneralAttributeParamsWriter().Param(L"Keywords")))
					.Token(L"AS", LR"(as)",
						GeneralAttributeWriter().Attribute(L"Color",
							GeneralAttributeParamsWriter().Param(L"Keywords")))
					.Token(L"WITH", LR"(with)",
						GeneralAttributeWriter().Attribute(L"Color",
							GeneralAttributeParamsWriter().Param(L"Keywords")))
					.Token(L"OPEN", LR"(\{)")
					.Token(L"CLOSE", LR"(\})")
					.Token(L"SEMICOLON", LR"(;)")
					.Token(L"COLON", LR"(:)")
					.Token(L"COMMA", LR"(,)")
					.Token(L"DOT", LR"(\.)")
					.Token(L"ASSIGN", LR"(=)")
					.Token(L"USING", LR"(!)")
					.Token(L"OR", LR"(\|)")
					.Token(L"ATSIGN", L"@",
						GeneralAttributeWriter().Attribute(L"Color",
							GeneralAttributeParamsWriter().Param(L"Attribute")))
					.Token(L"OPTOPEN", LR"(\[)")
					.Token(L"OPTCLOSE", LR"(\])")
					.Token(L"PREOPEN", LR"(\()")
					.Token(L"PRECLOSE", LR"(\))")
					.Token(L"NAME", LR"([a-zA-Z_]\w*)")
					.Token(L"STRING", LR"("(\\\\|\\"|[^"])*")",
						GeneralAttributeWriter().Attribute(L"Color",
							GeneralAttributeParamsWriter().Param(L"String")))
					.IgnoreToken(L"SPACE", LR"(\s+)")
					.IgnoreToken(L"LINENOTE", LR"(//[^\n]*\n)",
						GeneralAttributeWriter().Attribute(L"Color",
							GeneralAttributeParamsWriter().Param(L"Note")))
					.IgnoreToken(L"BLOCKNOTE", LR"(/\*.*?\*/)",
						GeneralAttributeWriter().Attribute(L"Color",
							GeneralAttributeParamsWriter().Param(L"Note")))
					)
				.Type
				(

					ztl::general_parser::GeneralTypeListWriter()
					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralTypeDefine")

						)
					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralAttributeArgumentDefine")
						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
							GeneralAttributeWriter().Attribute(L"Color",
								GeneralAttributeParamsWriter().Param(L"AttributeParam"))))
						)
					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralAttributeDefine")
						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
							GeneralAttributeWriter().Attribute(L"Color",
								GeneralAttributeParamsWriter().Param(L"AttributeName"))))
						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(Normal(L"GeneralAttributeArgumentDefine")), L"arguments"))
						)
					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralHeadInfoDefine")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"property"))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"value"))
						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(Normal(L"GeneralAttributeDefine")), L"attributes"))
						.Attributes(GeneralAttributeWriter()
							.Attribute(L"Color",
								GeneralAttributeParamsWriter().Param(L"HeadInfo")))
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralTokenDefine")

						.SubType(

							ztl::general_parser::GeneralTypeListWriter()

							.Enum
							(
								ztl::general_parser::GeneralEnumTypeWriter()
								.Name(L"TokenOptional")

								.Member(ztl::general_parser::EnumMember(L"False"))

								.Member(ztl::general_parser::EnumMember(L"True"))

								)

							)

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
							GeneralAttributeWriter()
							.Attribute(L"Color",
								GeneralAttributeParamsWriter().Param(L"Token"))))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"regex"))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"TokenOptional"), L"ignore"))
						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(Normal(L"GeneralAttributeDefine")), L"attributes"))
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralTypeObject")

						)
					
					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralArrayTypeObject")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralTypeObject"), L"element"))

						.ParentType(ztl::general_parser::Normal(L"GeneralTypeObject"))
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralTokenTypeObject")

						.ParentType(ztl::general_parser::Normal(L"GeneralTypeObject"))
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralNormalTypeObject")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name"))

						.ParentType(ztl::general_parser::Normal(L"GeneralTypeObject"))
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralSubTypeObject")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralTypeObject"), L"parent"))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name"))

						.ParentType(ztl::general_parser::Normal(L"GeneralTypeObject"))
						)


					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralClassMemberTypeDefine")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralTypeObject"), L"type"))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
							GeneralAttributeWriter().Attribute(L"Color",
								GeneralAttributeParamsWriter().Param(L"ClassField"))))
						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(Normal(L"GeneralAttributeDefine")), L"attributes"))
						.ParentType(ztl::general_parser::Normal(L"GeneralTypeDefine"))
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralEnumMemberTypeDefine")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
							GeneralAttributeWriter().Attribute(L"Color",
								GeneralAttributeParamsWriter().Param(L"EnumField"))))
						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(Normal(L"GeneralAttributeDefine")), L"attributes"))
						.ParentType(ztl::general_parser::Normal(L"GeneralTypeDefine"))
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralClassTypeDefine")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"GeneralTypeDefine")), L"subTypes"))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"GeneralClassMemberTypeDefine")), L"members"))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralTypeObject"), L"parent"))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
							GeneralAttributeWriter().Attribute(L"Color",
								GeneralAttributeParamsWriter().Param(L"ClassName"))))
						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(Normal(L"GeneralAttributeDefine")), L"attributes"))
						.ParentType(ztl::general_parser::Normal(L"GeneralTypeDefine"))
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralEnumTypeDefine")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"GeneralEnumMemberTypeDefine")), L"members"))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
							GeneralAttributeWriter().Attribute(L"Color",
								GeneralAttributeParamsWriter().Param(L"EnumClass"))))
						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(Normal(L"GeneralAttributeDefine")), L"attributes"))
						.ParentType(ztl::general_parser::Normal(L"GeneralTypeDefine"))
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralGrammarTypeDefine")

						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralGrammarTextTypeDefine")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"text"))

						.ParentType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralGrammarNormalTypeDefine")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name"))

						.ParentType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralGrammarSequenceTypeDefine")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"), L"first"))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"), L"second"))

						.ParentType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralGrammarLoopTypeDefine")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"), L"grammar"))

						.ParentType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralGrammarOptionalTypeDefine")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"), L"grammar"))

						.ParentType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralGrammarAssignTypeDefine")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name"))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"), L"grammar"))

						.ParentType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralGrammarSetterTypeDefine")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"), L"grammar"))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name"))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"value"))

						.ParentType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralGrammarUsingTypeDefine")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"), L"grammar"))

						.ParentType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralGrammarCreateTypeDefine")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralTypeObject"), L"type"))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"), L"grammar"))

						.ParentType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralGrammarAlternationTypeDefine")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"), L"left"))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"), L"right"))

						.ParentType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralRuleDefine")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"GeneralTypeObject"), L"type"))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name",
							GeneralAttributeWriter().Attribute(L"Color",
								GeneralAttributeParamsWriter().Param(L"RuleName"))))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine")), L"grammars"))
						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(Normal(L"GeneralAttributeDefine")), L"attributes"))
						)

					.Class
					(
						ztl::general_parser::GeneralClassTypeWriter()
						.Name(L"GeneralTableDefine")

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"GeneralHeadInfoDefine")), L"heads"))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"GeneralTypeDefine")), L"types"))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"GeneralTokenDefine")), L"tokens"))

						.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"GeneralRuleDefine")), L"rules"))

						)

					)
				.Rules
				(
					ztl::general_parser::GeneralRuleListWriter()
					.Rule
					(
						ztl::general_parser::GeneralRuleWriter()
						.Name(L"AttributeArgument")
						.ReturnType(ztl::general_parser::Normal(L"GeneralAttributeArgumentDefine"))
						|
						(
							ztl::general_parser::GrammarSymbol(L"STRING")[L"name"]
							)
						.Create(ztl::general_parser::Normal(L"GeneralAttributeArgumentDefine")))
					.Rule
					(
						ztl::general_parser::GeneralRuleWriter()
						.Name(L"Attribute")
						.ReturnType(ztl::general_parser::Normal(L"GeneralAttributeDefine"))
						|
						(
							ztl::general_parser::Text(L"@") +
							ztl::general_parser::GrammarSymbol(L"NAME")[L"name"] + ztl::general_parser::Text(L"(") +
							~(ztl::general_parser::GrammarSymbol(L"AttributeArgument")[L"arguments"]) +
								*(ztl::general_parser::Text(L",") + ztl::general_parser::GrammarSymbol(L"AttributeArgument")[L"arguments"]) +
							ztl::general_parser::Text(L")")

							)
						.Create(ztl::general_parser::Normal(L"GeneralAttributeDefine")))
					.Rule
					(
						ztl::general_parser::GeneralRuleWriter()
						.Name(L"TypeDecl")
						.ReturnType(ztl::general_parser::Normal(L"GeneralTypeDefine"))

						|
						(!(ztl::general_parser::GrammarSymbol(L"Enum")) | !(ztl::general_parser::GrammarSymbol(L"Class")))

						)

					.Rule
					(
						ztl::general_parser::GeneralRuleWriter()
						.Name(L"Type")
						.ReturnType(ztl::general_parser::Normal(L"GeneralTypeObject"))

						|
						(

							ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]

							)
						.Create(ztl::general_parser::Normal(L"GeneralNormalTypeObject"))

						|
						(
							ztl::general_parser::GrammarSymbol(L"TOKEN")
							)
						.Create(ztl::general_parser::Normal(L"GeneralTokenTypeObject"))

						|
						(

							ztl::general_parser::GrammarSymbol(L"Type")[L"parent"]
							+ ztl::general_parser::GrammarSymbol(L"DOT") +
							ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]

							)
						.Create(ztl::general_parser::Normal(L"GeneralSubTypeObject"))

						|
						(

							ztl::general_parser::GrammarSymbol(L"Type")[L"element"]
							+ ztl::general_parser::GrammarSymbol(L"OPTOPEN") + ztl::general_parser::GrammarSymbol(L"OPTCLOSE")
							)
						.Create(ztl::general_parser::Normal(L"GeneralArrayTypeObject"))

						)

					.Rule
					(
						ztl::general_parser::GeneralRuleWriter()
						.Name(L"EnumMember")
						.ReturnType(ztl::general_parser::Normal(L"GeneralEnumMemberTypeDefine"))

						|
						(

							ztl::general_parser::GrammarSymbol(L"NAME")[L"name"] + 
								*(ztl::general_parser::GrammarSymbol(L"Attribute")[L"attributes"])
							+ ztl::general_parser::GrammarSymbol(L"COMMA")
							)
						.Create(ztl::general_parser::Normal(L"GeneralEnumMemberTypeDefine"))

						)

					.Rule
					(
						ztl::general_parser::GeneralRuleWriter()
						.Name(L"Enum")
						.ReturnType(ztl::general_parser::Normal(L"GeneralEnumTypeDefine"))

						|
						(
							ztl::general_parser::GrammarSymbol(L"ENUM") +
							ztl::general_parser::GrammarSymbol(L"NAME")[L"name"] +
							*(ztl::general_parser::GrammarSymbol(L"Attribute")[L"attributes"])
							+ ztl::general_parser::GrammarSymbol(L"OPEN") + *(
								ztl::general_parser::GrammarSymbol(L"EnumMember")[L"members"]
								) + ztl::general_parser::GrammarSymbol(L"CLOSE") + ztl::general_parser::GrammarSymbol(L"SEMICOLON")
							)
						.Create(ztl::general_parser::Normal(L"GeneralEnumTypeDefine"))

						)

					.Rule
					(
						ztl::general_parser::GeneralRuleWriter()
						.Name(L"ClassMember")
						.ReturnType(ztl::general_parser::Normal(L"GeneralClassMemberTypeDefine"))

						|
						(
							ztl::general_parser::GrammarSymbol(L"Type")[L"type"]
							+
							ztl::general_parser::GrammarSymbol(L"NAME")[L"name"] + 
							*(ztl::general_parser::GrammarSymbol(L"Attribute")[L"attributes"])
							+ ztl::general_parser::GrammarSymbol(L"SEMICOLON")
							)
						.Create(ztl::general_parser::Normal(L"GeneralClassMemberTypeDefine"))

						)

					.Rule
					(
						ztl::general_parser::GeneralRuleWriter()
						.Name(L"Class")
						.ReturnType(ztl::general_parser::Normal(L"GeneralClassTypeDefine"))

						|
						(

							(ztl::general_parser::GrammarSymbol(L"STRUCT") | ztl::general_parser::GrammarSymbol(L"CLASS"))
							+
							ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
							+ ~(ztl::general_parser::GrammarSymbol(L"COLON") +
								ztl::general_parser::GrammarSymbol(L"Type")[L"parent"]
								) + 
							*( ztl::general_parser::GrammarSymbol(L"Attribute")[L"attributes"])
							+ ztl::general_parser::GrammarSymbol(L"OPEN") + *(
										(
											ztl::general_parser::GrammarSymbol(L"ClassMember")[L"members"]
											|
											ztl::general_parser::GrammarSymbol(L"TypeDecl")[L"subTypes"]
											)
										) + ztl::general_parser::GrammarSymbol(L"CLOSE") + ztl::general_parser::GrammarSymbol(L"SEMICOLON")
							)
						.Create(ztl::general_parser::Normal(L"GeneralClassTypeDefine"))

						)

					.Rule
					(
						ztl::general_parser::GeneralRuleWriter()
						.Name(L"SequenceGrammar")
						.ReturnType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))

						| !(ztl::general_parser::GrammarSymbol(L"PrimitiveGrammar"))

						|
						(

							ztl::general_parser::GrammarSymbol(L"PrimitiveGrammar")[L"first"]
							+
							ztl::general_parser::GrammarSymbol(L"SequenceGrammar")[L"second"]

							)
						.Create(ztl::general_parser::Normal(L"GeneralGrammarSequenceTypeDefine"))

						)

					.Rule
					(
						ztl::general_parser::GeneralRuleWriter()
						.Name(L"AlternativeGrammar")
						.ReturnType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))

						| !(ztl::general_parser::GrammarSymbol(L"SequenceGrammar"))

						|
						(

							ztl::general_parser::GrammarSymbol(L"SequenceGrammar")[L"left"]
							+ ztl::general_parser::GrammarSymbol(L"OR") +
							ztl::general_parser::GrammarSymbol(L"AlternativeGrammar")[L"right"]

							)
						.Create(ztl::general_parser::Normal(L"GeneralGrammarAlternationTypeDefine"))

						)

					.Rule
					(
						ztl::general_parser::GeneralRuleWriter()
						.Name(L"Grammar")
						.ReturnType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))

						| !(ztl::general_parser::GrammarSymbol(L"AlternativeGrammar"))

						|
						(

							ztl::general_parser::GrammarSymbol(L"Grammar")[L"grammar"]
							+ ztl::general_parser::GrammarSymbol(L"AS") +
							ztl::general_parser::GrammarSymbol(L"Type")[L"type"]

							)
						.Create(ztl::general_parser::Normal(L"GeneralGrammarCreateTypeDefine"))

						|
						(

							ztl::general_parser::GrammarSymbol(L"Grammar")[L"grammar"]
							+ ztl::general_parser::GrammarSymbol(L"WITH") + ztl::general_parser::GrammarSymbol(L"OPEN") +
							ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
							+ ztl::general_parser::GrammarSymbol(L"ASSIGN") +
							ztl::general_parser::GrammarSymbol(L"STRING")[L"value"]
							+ ztl::general_parser::GrammarSymbol(L"CLOSE")
							)
						.Create(ztl::general_parser::Normal(L"GeneralGrammarSetterTypeDefine"))

						)

					.Rule
					(
						ztl::general_parser::GeneralRuleWriter()
						.Name(L"PrimitiveGrammar")
						.ReturnType(ztl::general_parser::Normal(L"GeneralGrammarTypeDefine"))

						|
						(

							ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]

							)
						.Create(ztl::general_parser::Normal(L"GeneralGrammarNormalTypeDefine"))

						|
						(

							ztl::general_parser::GrammarSymbol(L"STRING")[L"text"]

							)
						.Create(ztl::general_parser::Normal(L"GeneralGrammarTextTypeDefine"))

						|
						(

							ztl::general_parser::GrammarSymbol(L"PrimitiveGrammar")[L"grammar"]
							+ ztl::general_parser::GrammarSymbol(L"COLON") +
							ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]

							)
						.Create(ztl::general_parser::Normal(L"GeneralGrammarAssignTypeDefine"))

						|
						(
							ztl::general_parser::GrammarSymbol(L"USING") +
							ztl::general_parser::GrammarSymbol(L"PrimitiveGrammar")[L"grammar"]

							)
						.Create(ztl::general_parser::Normal(L"GeneralGrammarUsingTypeDefine"))

						|
						(
							ztl::general_parser::GrammarSymbol(L"OPTOPEN") +
							ztl::general_parser::GrammarSymbol(L"Grammar")[L"grammar"]
							+ ztl::general_parser::GrammarSymbol(L"OPTCLOSE")
							)
						.Create(ztl::general_parser::Normal(L"GeneralGrammarOptionalTypeDefine"))

						|
						(
							ztl::general_parser::GrammarSymbol(L"OPEN") +
							ztl::general_parser::GrammarSymbol(L"Grammar")[L"grammar"]
							+ ztl::general_parser::GrammarSymbol(L"CLOSE")
							)
						.Create(ztl::general_parser::Normal(L"GeneralGrammarLoopTypeDefine"))

						| ztl::general_parser::GrammarSymbol(L"PREOPEN") + !(ztl::general_parser::GrammarSymbol(L"Grammar")) + ztl::general_parser::GrammarSymbol(L"PRECLOSE")

						)

					.Rule
					(
						ztl::general_parser::GeneralRuleWriter()
						.Name(L"TokenDecl")
						.ReturnType(ztl::general_parser::Normal(L"GeneralTokenDefine"))

						|
						(
							ztl::general_parser::GrammarSymbol(L"TOKEN") +
							ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
							+ ztl::general_parser::GrammarSymbol(L"ASSIGN") +
							ztl::general_parser::GrammarSymbol(L"STRING")[L"regex"]+
							*(ztl::general_parser::GrammarSymbol(L"Attribute")[L"attributes"])
							+ ztl::general_parser::GrammarSymbol(L"SEMICOLON")
							)
						.Create(ztl::general_parser::Normal(L"GeneralTokenDefine"))

						.Setter(L"ignore", L"False")

						|
						(
							ztl::general_parser::GrammarSymbol(L"DISCARDTOKEN") +
							ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]
							+ ztl::general_parser::GrammarSymbol(L"ASSIGN") +
							ztl::general_parser::GrammarSymbol(L"STRING")[L"regex"]+
							*(ztl::general_parser::GrammarSymbol(L"Attribute")[L"attributes"])
							+ ztl::general_parser::GrammarSymbol(L"SEMICOLON")
							)
						.Create(ztl::general_parser::Normal(L"GeneralTokenDefine"))

						.Setter(L"ignore", L"True")

						)

					.Rule
					(
						ztl::general_parser::GeneralRuleWriter()
						.Name(L"HeadDecl")
						.ReturnType(ztl::general_parser::Normal(L"GeneralHeadInfoDefine"))

						|
						(

							ztl::general_parser::GrammarSymbol(L"NAME")[L"property"]
							+ ztl::general_parser::GrammarSymbol(L"COLON") +
							ztl::general_parser::GrammarSymbol(L"STRING")[L"value"]
							+ 
							*( ztl::general_parser::GrammarSymbol(L"Attribute")[L"attributes"])
							+ ztl::general_parser::GrammarSymbol(L"SEMICOLON")
							)
						.Create(ztl::general_parser::Normal(L"GeneralHeadInfoDefine"))

						)

					.Rule
					(
						ztl::general_parser::GeneralRuleWriter()
						.Name(L"RuleDecl")
						.ReturnType(ztl::general_parser::Normal(L"GeneralRuleDefine"))

						|
						(
							ztl::general_parser::GrammarSymbol(L"RULE") +
							ztl::general_parser::GrammarSymbol(L"Type")[L"type"]
							+
							ztl::general_parser::GrammarSymbol(L"NAME")[L"name"]+ 
							*( ztl::general_parser::GrammarSymbol(L"Attribute")[L"attributes"])
							+ *(ztl::general_parser::GrammarSymbol(L"ASSIGN") +
								ztl::general_parser::GrammarSymbol(L"Grammar")[L"grammars"]
								) + ztl::general_parser::GrammarSymbol(L"SEMICOLON")
							)
						.Create(ztl::general_parser::Normal(L"GeneralRuleDefine"))

						)

					.Rule
					(
						ztl::general_parser::GeneralRuleWriter()
						.Name(L"ParserDecl")
						.ReturnType(ztl::general_parser::Normal(L"GeneralTableDefine"))
						|
						(

							(*(
								ztl::general_parser::GrammarSymbol(L"HeadDecl")[L"heads"]
								) |
								(*(
									ztl::general_parser::GrammarSymbol(L"TypeDecl")[L"types"]
									) |
									(*(
										ztl::general_parser::GrammarSymbol(L"TokenDecl")[L"tokens"]
										) | *(
											ztl::general_parser::GrammarSymbol(L"RuleDecl")[L"rules"]
											))
									)
								)

							)
						.Create(ztl::general_parser::Normal(L"GeneralTableDefine"))

						)

					);
			return writer.table;;
		}
	}
}