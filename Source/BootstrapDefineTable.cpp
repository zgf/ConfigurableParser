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
		namespace table_writer
		{
			//手写ENBF核心表
			shared_ptr<GeneralTableDefine> BootStrapDefineTable()
			{
				GeneralTableWriter writer;
				writer.Head
					(
						GeneralHeadInfoWriter()
						.Info(L"namespace",L"XML")
						.Info(L"classprefix",L"ZGF")
						.Info(L"namespace",L"test")
						.Info(L"include",L"Source/Include/stdafx.h")
					)
					.Token
					(
						GeneralTokenWriter()
						.Token(L"CLASS", L"class")
						.Token(L"STRUCT",L"struct")
						.Token(L"ENUM", L"enum")
						.Token(L"TOKEN", L"token")
						.Token(L"DISCARDTOKEN", L"ignoretoken")
						.Token(L"RULE", L"rule")
						.Token(L"AS", L"as")
						.Token(L"WITH", L"with")
						.Token(L"OPEN", LR"(\{)")
						.Token(L"CLOSE", LR"(\})")
						.Token(L"SEMICOLON", L";")
						.Token(L"COLON", L":")
						.Token(L"COMMA", L",")
						.Token(L"DOT", LR"(\.)")
						.Token(L"ASSIGN", LR"(=)")
						.Token(L"USING", LR"(!)")
						.Token(L"OR", LR"(\|)")
						.Token(L"OPTOPEN", LR"(\[)")
						.Token(L"OPTCLOSE", LR"(\])")
						.Token(L"PREOPEN", LR"(\()")
						.Token(L"PRECLOSE", LR"(\))")
						.Token(L"NAME", LR"([a-zA-Z_]\w*)")
						.Token(L"STRING", LR"("(\\"|[^"])*")")
						.Token(L"FINISH",L"<\\$>")
						.IgnoreToken(L"SPACE", LR"(\s+)")
						.IgnoreToken(L"LINENOTE", L"(//[^\\n]*\\n)")
						.IgnoreToken(L"BLOCKNOTE", L"/*.*?*/ //")
					)
					.Type
					(
						GeneralTypeListWriter()
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralHeadInfoDefine")
							.Member(ClassMember(String(),L"property"))
							.Member(ClassMember(String(),L"value"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralTokenDefine")
							.Member(ClassMember(String(), L"name"))
							.Member(ClassMember(String(), L"regex"))
							.Member(ClassMember(Normal(L"TokenOptional"),L"ignore"))
							.SubType
							(
								GeneralTypeListWriter()
								.Enum
								(
									GeneralEnumTypeWriter()
									.Name(L"TokenOptional")
									.Member(EnumMember(L"False"))
									.Member(EnumMember(L"True"))
								)
							)
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralTypeObject")
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralArrayTypeObject")
							.Member(ClassMember(Normal(L"GeneralTypeObject"), L"element"))
							.ParentType(Normal(L"GeneralTypeObject"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralStringTypeObject")
							.ParentType(Normal(L"GeneralTypeObject"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralNormalTypeObject")
							.Member(ClassMember(String(),L"name"))
							.ParentType(Normal(L"GeneralTypeObject"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralSubTypeObject")
							.Member(ClassMember(Normal(L"GeneralTypeObject"),L"parent"))
							.Member(ClassMember(String(), L"name"))
							.ParentType(Normal(L"GeneralTypeObject"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralTypeDefine")
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralClassMemberTypeDefine")
							.Member(ClassMember(Normal(L"GeneralTypeObject"), L"type"))
							.Member(ClassMember(String(), L"name"))
							.ParentType(Normal(L"GeneralTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralEnumMemberTypeDefine")
							.Member(ClassMember(String(), L"name"))
							.ParentType(Normal(L"GeneralTypeDefine"))
						)
						.Class
						(
						GeneralClassTypeWriter()
						.Name(L"GeneralClassTypeDefine")
						.Member(ClassMember(String(), L"name"))
						.Member(ClassMember(Normal(L"GeneralTypeObject"), L"parent"))
						.Member(ClassMember(Array(Normal(L"GeneralTypeDefine")), L"subTypes"))
						.Member(ClassMember(Array(Normal(L"GeneralClassMemberTypeDefine")), L"members"))
						.ParentType(Normal(L"GeneralTypeDefine"))
						)
						.Class
						(
						GeneralClassTypeWriter()
						.Name(L"GeneralEnumTypeDefine")
						.Member(ClassMember(String(), L"name"))
						.Member(ClassMember(Array(Normal(L"GeneralEnumMemberTypeDefine")), L"members"))
						.ParentType(Normal(L"GeneralTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralGrammarTypeDefine")
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralGrammarTextTypeDefine")
							.Member(ClassMember(String(), L"text"))
							.ParentType(Normal(L"GeneralGrammarTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralGrammarNormalTypeDefine")
							.Member(ClassMember(String(), L"name"))
							.ParentType(Normal(L"GeneralGrammarTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralGrammarSequenceTypeDefine")
							.Member(ClassMember(Normal(L"GeneralGrammarTypeDefine"), L"first"))
							.Member(ClassMember(Normal(L"GeneralGrammarTypeDefine"), L"second"))
							.ParentType(Normal(L"GeneralGrammarTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralGrammarLoopTypeDefine")
							.Member(ClassMember(Normal(L"GeneralGrammarTypeDefine"), L"grammar"))
							.ParentType(Normal(L"GeneralGrammarTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralGrammarOptionalTypeDefine")
							.Member(ClassMember(Normal(L"GeneralGrammarTypeDefine"), L"grammar"))
							.ParentType(Normal(L"GeneralGrammarTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralGrammarAssignTypeDefine")
							.Member(ClassMember(String(), L"name"))
							.Member(ClassMember(Normal(L"GeneralGrammarTypeDefine"), L"grammar"))
							.ParentType(Normal(L"GeneralGrammarTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralGrammarSetterTypeDefine")
							.Member(ClassMember(Normal(L"GeneralGrammarTypeDefine"), L"grammar"))
							.Member(ClassMember(String(), L"name"))
							.Member(ClassMember(String(), L"value"))
							.ParentType(Normal(L"GeneralGrammarTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralGrammarUsingTypeDefine")
							.Member(ClassMember(Normal(L"GeneralGrammarTypeDefine"), L"grammar"))
							.ParentType(Normal(L"GeneralGrammarTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralGrammarCreateTypeDefine")
							.Member(ClassMember(Normal(L"GeneralTypeObject"), L"type"))
							.Member(ClassMember(Normal(L"GeneralGrammarTypeDefine"), L"grammar"))
							.ParentType(Normal(L"GeneralGrammarTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralGrammarAlternationTypeDefine")
							.Member(ClassMember(Normal(L"GeneralGrammarTypeDefine"), L"left"))
							.Member(ClassMember(Normal(L"GeneralGrammarTypeDefine"), L"right"))
							.ParentType(Normal(L"GeneralGrammarTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GeneralRuleDefine")
							.Member(ClassMember(Normal(L"GeneralTypeObject"), L"type"))
							.Member(ClassMember(String(), L"name"))
							.Member(ClassMember(Array(Normal(L"GeneralGrammarTypeDefine")), L"grammars"))
						)
						.Class
						(
						GeneralClassTypeWriter()
						.Name(L"GeneralTableDefine")
						.Member(ClassMember(Array(Normal(L"GeneralHeadInfoDefine")), L"heads"))
						.Member(ClassMember(Array(Normal(L"GeneralTypeDefine")), L"types"))
						.Member(ClassMember(Array(Normal(L"GeneralTokenDefine")), L"tokens"))
						.Member(ClassMember(Array(Normal(L"GeneralRuleDefine")), L"rules"))
						)
					)
					.RuleList
					(
						GeneralRuleListWriter()
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"Type")
							.ReturnType(Normal(L"GeneralTypeObject"))
							| GrammarSymbol(L"NAME")[L"name"]
							.Create(Normal(L"GeneralNormalTypeObject"))
							| Text(L"token")
							.Create(Normal(L"GeneralStringTypeObject"))
							| (
								GrammarSymbol(L"Type")[L"parent"] +
								Text(L".") + 
								GrammarSymbol(L"NAME")[L"name"])
							.Create(Normal(L"GeneralSubTypeObject"))
							| (
								GrammarSymbol(L"Type")[L"element"] + 
								Text(L"[") +
								Text(L"]"))
							.Create(Normal(L"GeneralArrayTypeObject"))
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"EnumMember")
							.ReturnType(Normal(L"GeneralEnumMemberTypeDefine"))
							| (
								GrammarSymbol(L"NAME")[L"name"] +
								Text(L","))
							.Create(Normal(L"GeneralEnumMemberTypeDefine"))
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"Enum")
							.ReturnType(Normal(L"GeneralEnumTypeDefine"))
							| (
								Text(L"enum") + 
								GrammarSymbol(L"NAME")[L"name"] +
								Text(L"{") + 
								*(
									GrammarSymbol(L"EnumMember")[L"members"]
								  ) +
									Text(L"}") + Text(L";")
							   )
							.Create(Normal(L"GeneralEnumTypeDefine"))
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"ClassMember")
							.ReturnType(Normal(L"GeneralClassMemberTypeDefine"))
							| (
								GrammarSymbol(L"Type")[L"type"] + 
								GrammarSymbol(L"NAME")[L"name"] +
								Text(L";")
							   )
							.Create(Normal(L"GeneralClassMemberTypeDefine"))
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"TypeDecl")
							.ReturnType(Normal(L"GeneralTypeDefine"))
							| !GrammarSymbol(L"Enum")
							| !GrammarSymbol(L"Class")
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"Class")
							.ReturnType(Normal(L"GeneralClassTypeDefine"))
							| (
								(Text(L"class")|Text(L"struct")) +
								GrammarSymbol(L"NAME")[L"name"] +
								~(
									Text(L":") + 
									GrammarSymbol(L"Type")[L"parent"]
									) +
								Text(L"{") +
								*(
									GrammarSymbol(L"ClassMember")[L"members"] 
									| GrammarSymbol(L"TypeDecl")[L"subTypes"]
								 ) +
								Text(L"}")+Text(L";")
							  )
							  .Create(Normal(L"GeneralClassTypeDefine"))
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"SequenceGrammar")
							.ReturnType(Normal(L"GeneralGrammarTypeDefine"))
							| !GrammarSymbol(L"PrimitiveGrammar")
							| (
								GrammarSymbol(L"SequenceGrammar")[L"first"]+
								GrammarSymbol(L"PrimitiveGrammar")[L"second"]
							  )
							  .Create(Normal(L"GeneralGrammarSequenceTypeDefine"))
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"AlternativeGrammar")
							.ReturnType(Normal(L"GeneralGrammarTypeDefine"))
							| !GrammarSymbol(L"SequenceGrammar")
							| (
								GrammarSymbol(L"AlternativeGrammar")[L"left"] +
								Text(L"|") +
								GrammarSymbol(L"SequenceGrammar")[L"right"]
							)
							.Create(Normal(L"GeneralGrammarAlternationTypeDefine"))
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"Grammar")
							.ReturnType(Normal(L"GeneralGrammarTypeDefine"))
							| (
								GrammarSymbol(L"Grammar")[L"grammar"] +
								Text(L"as") +
								GrammarSymbol(L"Type")[L"type"]
							)
							.Create(Normal(L"GeneralGrammarCreateTypeDefine"))
							| (
								GrammarSymbol(L"Grammar")[L"grammar"] + 
								Text(L"with") +
								Text(L"{") +
								(
									GrammarSymbol(L"NAME")[L"name"] +
									Text(L"=") + 
									GrammarSymbol(L"STRING")[L"value"]
								 ) +
								Text(L"}")
							).Create(Normal(L"GeneralGrammarSetterTypeDefine"))
							|!GrammarSymbol(L"AlternativeGrammar")
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"NormalGrammar")
							.ReturnType(Normal(L"GeneralGrammarTypeDefine"))
							| 
								(GrammarSymbol(L"NAME")[L"name"])
							  .Create(Normal(L"GeneralGrammarNormalTypeDefine"))
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"AssginGrammar")
							.ReturnType(Normal(L"GeneralGrammarTypeDefine"))
							|	(
									GrammarSymbol(L"NormalGrammar")[L"grammar"] +
									Text(L":") +
									GrammarSymbol(L"NAME")[L"name"]
								).Create(Normal(L"GeneralGrammarAssignTypeDefine"))
							| 
								(
									GrammarSymbol(L"STRING")[L"text"]
								)
							  .Create(Normal(L"GeneralGrammarTextTypeDefine"))
							| Text(L"(") +
								 !GrammarSymbol(L"Grammar") +
							  Text(L")")
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"PrimitiveGrammar")
							.ReturnType(Normal(L"GeneralGrammarTypeDefine"))
							|
								!GrammarSymbol(L"AssginGrammar")
							| (
								Text(L"!") + 
								GrammarSymbol(L"NormalGrammar")[L"grammar"]
								).Create(Normal(L"GeneralGrammarUsingTypeDefine"))
							| (
								Text(L"[") + 
								GrammarSymbol(L"Grammar")[L"grammar"]+
								Text(L"]")
							  ).Create(Normal(L"GeneralGrammarOptionalTypeDefine"))
							| (
								Text(L"{") + 
								GrammarSymbol(L"Grammar")[L"grammar"] + 
								Text(L"}")
							  ).Create(Normal(L"GeneralGrammarLoopTypeDefine"))
							  )
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"HeadDecl")
							.ReturnType(Normal(L"GeneralHeadInfoDefine"))
							| (
								GrammarSymbol(L"NAME")[L"property"] +
								Text(L":")+
								GrammarSymbol(L"STRING")[L"value"] +
								Text(L";")
							  )
							.Create(Normal(L"GeneralHeadInfoDefine"))
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"TokenDecl")
							.ReturnType(Normal(L"GeneralTokenDefine"))
							| (
								Text(L"token") +
								GrammarSymbol(L"NAME")[L"name"] +
								Text(L"=") +
								GrammarSymbol(L"STRING")[L"regex"] +
								Text(L";")
							  )
							  .Create(Normal(L"GeneralTokenDefine"))
								.Setter(L"ignore", L"False")
							| (
								Text(L"ignoretoken") +
								GrammarSymbol(L"NAME")[L"name"] +
								Text(L"=") +
								GrammarSymbol(L"STRING")[L"regex"] +
								Text(L";")
							  )
							.Create(Normal(L"GeneralTokenDefine"))
								.Setter(L"ignore", L"True")
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"RuleDecl")
							.ReturnType(Normal(L"GeneralRuleDefine"))
							| (
								Text(L"rule")+
								GrammarSymbol(L"Type")[L"type"]+
								GrammarSymbol(L"NAME")[L"name"] +
								*(
									Text(L"=") +
									GrammarSymbol(L"Grammar")[L"grammars"])+
									Text(L";")

								)
							.Create(Normal(L"GeneralRuleDefine"))
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"ParserDecl")
							.ReturnType(Normal(L"GeneralTableDefine"))
							| (
								*(GrammarSymbol(L"HeadDecl")[L"heads"]) +
								*(GrammarSymbol(L"TypeDecl")[L"types"])  +
								*(GrammarSymbol(L"TokenDecl")[L"tokens"]) +
								*(GrammarSymbol(L"RuleDecl")[L"rules"]) 
							)
							.Create(Normal(L"GeneralTableDefine"))
						)
					);
				return writer.table;
			}

		
		}
		shared_ptr<GeneralTableDefine> BootStrapDefineTable()
		{
			return table_writer::BootStrapDefineTable();
		}
	}
}