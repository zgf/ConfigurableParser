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
							.Name(L"GenHeadInfoDefine")
							.Member(ClassMember(String(),L"property"))
							.Member(ClassMember(String(),L"value"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenTokenDefine")
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
							.Name(L"GenTypeObject")
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenArrayTypeObject")
							.Member(ClassMember(Normal(L"GenTypeObject"), L"element"))
							.ParentType(Normal(L"GenTypeObject"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenStringTypeObject")
							.ParentType(Normal(L"GenTypeObject"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenNormalTypeObject")
							.Member(ClassMember(String(),L"name"))
							.ParentType(Normal(L"GenTypeObject"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenSubTypeObject")
							.Member(ClassMember(Normal(L"GenTypeObject"),L"parent"))
							.Member(ClassMember(String(), L"name"))
							.ParentType(Normal(L"GenTypeObject"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenTypeDefine")
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenClassMemberTypeDefine")
							.Member(ClassMember(Normal(L"GenTypeObject"), L"type"))
							.Member(ClassMember(String(), L"name"))
							.ParentType(Normal(L"GenTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenEnumMemberTypeDefine")
							.Member(ClassMember(String(), L"name"))
							.ParentType(Normal(L"GenTypeDefine"))
						)
						.Class
						(
						GeneralClassTypeWriter()
						.Name(L"GenClassTypeDefine")
						.Member(ClassMember(String(), L"name"))
						.Member(ClassMember(Normal(L"GenTypeObject"), L"parent"))
						.Member(ClassMember(Array(Normal(L"GenTypeDefine")), L"subTypes"))
						.Member(ClassMember(Array(Normal(L"GenClassMemberTypeDefine")), L"members"))
						.ParentType(Normal(L"GenTypeDefine"))
						)
						.Class
						(
						GeneralClassTypeWriter()
						.Name(L"GenEnumTypeDefine")
						.Member(ClassMember(String(), L"name"))
						.Member(ClassMember(Array(Normal(L"GenEnumMemberTypeDefine")), L"members"))
						.ParentType(Normal(L"GenTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenGrammarTypeDefine")
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenGrammarTextTypeDefine")
							.Member(ClassMember(String(), L"text"))
							.ParentType(Normal(L"GenGrammarTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenGrammarNormalTypeDefine")
							.Member(ClassMember(String(), L"name"))
							.ParentType(Normal(L"GenGrammarTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenGrammarSequenceTypeDefine")
							.Member(ClassMember(Normal(L"GenGrammarTypeDefine"), L"first"))
							.Member(ClassMember(Normal(L"GenGrammarTypeDefine"), L"second"))
							.ParentType(Normal(L"GenGrammarTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenGrammarLoopTypeDefine")
							.Member(ClassMember(Normal(L"GenGrammarTypeDefine"), L"grammar"))
							.ParentType(Normal(L"GenGrammarTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenGrammarOptionalTypeDefine")
							.Member(ClassMember(Normal(L"GenGrammarTypeDefine"), L"grammar"))
							.ParentType(Normal(L"GenGrammarTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenGrammarAssignTypeDefine")
							.Member(ClassMember(String(), L"name"))
							.Member(ClassMember(Normal(L"GenGrammarTypeDefine"), L"grammar"))
							.ParentType(Normal(L"GenGrammarTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenGrammarSetterTypeDefine")
							.Member(ClassMember(Normal(L"GenGrammarTypeDefine"), L"grammar"))
							.Member(ClassMember(String(), L"name"))
							.Member(ClassMember(String(), L"value"))
							.ParentType(Normal(L"GenGrammarTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenGrammarAdditionTypeDefine")
							.Member(ClassMember(Normal(L"GenGrammarTypeDefine"), L"grammar"))
							.Member(ClassMember(Array(Normal(L"GenGrammarSetterTypeDefine")), L"setters"))
							.ParentType(Normal(L"GenGrammarTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenGrammarUsingTypeDefine")
							.Member(ClassMember(String(), L"name"))
							.Member(ClassMember(Normal(L"GenGrammarTypeDefine"), L"grammar"))
							.ParentType(Normal(L"GenGrammarTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenGrammarCreateTypeDefine")
							.Member(ClassMember(Normal(L"GenTypeObject"), L"type"))
							.Member(ClassMember(Normal(L"GenGrammarTypeDefine"), L"grammar"))
							.ParentType(Normal(L"GenGrammarTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenGrammarAlternationTypeDefine")
							.Member(ClassMember(Normal(L"GenGrammarTypeDefine"), L"left"))
							.Member(ClassMember(Normal(L"GenGrammarTypeDefine"), L"right"))
							.ParentType(Normal(L"GenGrammarTypeDefine"))
						)
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenRuleDefine")
							.Member(ClassMember(Normal(L"GenTypeObject"), L"type"))
							.Member(ClassMember(String(), L"name"))
							.Member(ClassMember(Array(Normal(L"GenGrammarTypeDefine")), L"grammars"))
						)
						.Class
						(
						GeneralClassTypeWriter()
						.Name(L"GenTableDefine")
						.Member(ClassMember(Array(Normal(L"GenHeadInfoDefine")), L"heads"))
						.Member(ClassMember(Array(Normal(L"GenTypeDefine")), L"types"))
						.Member(ClassMember(Array(Normal(L"GenTokenDefine")), L"tokens"))
						.Member(ClassMember(Array(Normal(L"GenRuleDefine")), L"rules"))
						)
					)
					.RuleList
					(
						GeneralRuleListWriter()
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"Type")
							.ReturnType(Normal(L"GenTypeObject"))
							| GrammarSymbol(L"NAME")[L"name"]
							.Create(Normal(L"GenNormalTypeObject"))
							| Text(L"token")
							.Create(Normal(L"GenStringTypeObject"))
							| (
								GrammarSymbol(L"Type")[L"parent"] +
								Text(L".") + 
								GrammarSymbol(L"NAME")[L"name"])
							.Create(Normal(L"GenSubTypeObject"))
							| (
								GrammarSymbol(L"Type")[L"element"] + 
								Text(L"[") +
								Text(L"]"))
							.Create(Normal(L"GenArrayTypeObject"))
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"EnumMember")
							.ReturnType(Normal(L"GenEnumMemberTypeDefine"))
							| (
								GrammarSymbol(L"NAME")[L"name"] +
								Text(L","))
							.Create(Normal(L"GenEnumMemberTypeDefine"))
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"Enum")
							.ReturnType(Normal(L"GenEnumTypeDefine"))
							| (
								Text(L"enum") + 
								GrammarSymbol(L"NAME")[L"name"] +
								Text(L"{") + 
								*(
									GrammarSymbol(L"EnumMember")[L"members"]
								  ) +
									Text(L"}"))
							.Create(Normal(L"GenEnumTypeDefine"))
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"ClassMember")
							.ReturnType(Normal(L"GenClassMemberTypeDefine"))
							| (
								GrammarSymbol(L"Type")[L"type"] + 
								GrammarSymbol(L"NAME")[L"name"] +
								Text(L";"))
							.Create(Normal(L"GenClassMemberTypeDefine"))
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"TypeDecl")
							.ReturnType(Normal(L"GenTypeDefine"))
							| !GrammarSymbol(L"Enum")
							| !GrammarSymbol(L"Class")
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"Class")
							.ReturnType(Normal(L"GenClassTypeDefine"))
							| (
								Text(L"class") +
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
								Text(L"}")
							  )
							  .Create(Normal(L"GenClassTypeDefine"))
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"SequenceGrammar")
							.ReturnType(Normal(L"GenGrammarTypeDefine"))
							| !GrammarSymbol(L"PrimitiveGrammar")
							| (
								GrammarSymbol(L"SequenceGrammar")[L"first"]+
								GrammarSymbol(L"PrimitiveGrammar")[L"second"]
							  )
							  .Create(Normal(L"GenGrammarSequenceTypeDefine"))
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"AlternativeGrammar")
							.ReturnType(Normal(L"GenGrammarTypeDefine"))
							| !GrammarSymbol(L"SequenceGrammar")
							| (
								GrammarSymbol(L"AlternativeGrammar")[L"left"] +
								Text(L"|") +
								GrammarSymbol(L"SequenceGrammar")[L"right"]
							)
							.Create(Normal(L"GenGrammarAlternationTypeDefine"))
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"SetterGrammar")
							.ReturnType(Normal(L"GenGrammarTypeDefine"))
							| (
								GrammarSymbol(L"NAME")[L"name"] +
								Text(L"=")+
								GrammarSymbol(L"STRING")[L"value"]
							)
							.Create(Normal(L"GenGrammarSetterTypeDefine"))
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"Grammar")
							.ReturnType(Normal(L"GenGrammarTypeDefine"))
							| (
								GrammarSymbol(L"Grammar")[L"grammar"] +
								Text(L"as") +
								GrammarSymbol(L"Type")[L"type"]
							)
							.Create(Normal(L"GenGrammarCreateTypeDefine"))
							| (
								GrammarSymbol(L"Grammar")[L"grammar"] + 
								Text(L"with")+
								Text(L"{")+
								(
									//NAME : memberName "=" STRING : value
									GrammarSymbol(L"NAME")[L"name"] +
									Text(L"=") + 
									GrammarSymbol(L"STRING")[L"value"]
								 ) +
								Text(L"}")
							).Create(Normal(L"GenGrammarSetterTypeDefine"))
							|!GrammarSymbol(L"AlternativeGrammar")
							| !GrammarSymbol(L"SetterGrammar")
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"PrimitiveGrammar")
							.ReturnType(Normal(L"GenGrammarTypeDefine"))
							|(
								GrammarSymbol(L"NAME")[L"name"])
							.Create(Normal(L"GenGrammarNormalTypeDefine"))
							|(
								GrammarSymbol(L"STRING")[L"text"])
							.Create(Normal(L"GenGrammarTextTypeDefine"))
							|(
								GrammarSymbol(L"PrimitiveGrammar")[L"grammar"] +
								Text(L":") +
								GrammarSymbol(L"NAME")[L"name"]
							)
							.Create(Normal(L"GenGrammarAssignTypeDefine"))
							| (
								Text(L"!") + 
								GrammarSymbol(L"PrimitiveGrammar")[L"grammar"]
								).Create(Normal(L"GenGrammarUsingTypeDefine"))
							| (
								Text(L"[") + 
								GrammarSymbol(L"Grammar")[L"grammar"]+
								Text(L"]")
							  ).Create(Normal(L"GenGrammarOptionalTypeDefine"))
							| (
								Text(L"{") + 
								GrammarSymbol(L"Grammar")[L"grammar"] + 
								Text(L"}")
							  ).Create(Normal(L"GenGrammarLoopTypeDefine"))
							| Text(L"(")+
							  !GrammarSymbol(L"Grammar")+
							  Text(L")")
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"HeadDecl")
							.ReturnType(Normal(L"GenHeadInfoDefine"))
							| (
								GrammarSymbol(L"NAME")[L"property"] +
								Text(L":")+
								GrammarSymbol(L"STRING")[L"value"] +
								Text(L";")
							  )
							.Create(Normal(L"GenHeadInfoDefine"))
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"TokenDecl")
							.ReturnType(Normal(L"GenTokenDefine"))
							| (
								Text(L"token") +
								GrammarSymbol(L"NAME")[L"name"] +
								Text(L"=") +
								GrammarSymbol(L"STRING")[L"regex"] +
								Text(L";")
							  )
							  .Create(Normal(L"GenTokenDefine"))
								.Setter(L"ignore", L"False")
							| (
								Text(L"ignoretoken") +
								GrammarSymbol(L"NAME")[L"name"] +
								Text(L"=") +
								GrammarSymbol(L"STRING")[L"regex"] +
								Text(L";")
							  )
							.Create(Normal(L"GenTokenDefine"))
								.Setter(L"ignore", L"True")
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"RuleDecl")
							.ReturnType(Normal(L"GenRuleDefine"))
							| (
								Text(L"rule")+
								GrammarSymbol(L"Type")[L"type"]+
								GrammarSymbol(L"NAME")[L"name"] +
								*(
									Text(L"=") +
									GrammarSymbol(L"Grammar")[L"grammars"])+
									Text(L";")

								)
							.Create(Normal(L"GenRuleDefine"))
						)
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"ParserDecl")
							.ReturnType(Normal(L"GenTableDefine"))
							| (
								*(GrammarSymbol(L"HeadDecl")[L"heads"]) +
								*(GrammarSymbol(L"TypeDecl")[L"types"])  +
								*(GrammarSymbol(L"TokenDecl")[L"tokens"]) +
								*(GrammarSymbol(L"RuleDecl")[L"rules"]) +Text(L"<$>")
							)
							.Create(Normal(L"GenTableDefine"))
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