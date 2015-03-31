
				shared_ptr<GeneralTableDefine> BootStrapDefineTable()
				{
					GeneralTableWriter writer;
					writer.Head
						(
							GeneralHeadInfoWriter()
							
					.Info(L"include",L""zgf"")
					
					.Info(L"namespace",L""xml"")
					
						)
						.Token
						(
							GeneralTokenWriter()
							.IgnoreToken(L"CLASS",LR"("class")")
.IgnoreToken(L"STRUCT",LR"("struct")")
.IgnoreToken(L"ENUM",LR"("enum")")
.IgnoreToken(L"TOKEN",LR"("token")")
.IgnoreToken(L"DISCARDTOKEN",LR"("ignoretoken")")
.IgnoreToken(L"RULE",LR"("rule")")
.IgnoreToken(L"AS",LR"("as")")
.IgnoreToken(L"WITH",LR"("with")")
.IgnoreToken(L"OPEN",LR"("{")")
.IgnoreToken(L"CLOSE",LR"("}")")
.IgnoreToken(L"SEMICOLON",LR"(";")")
.IgnoreToken(L"COLON",LR"(":")")
.IgnoreToken(L"COMMA",LR"(",")")
.IgnoreToken(L"DOT",LR"(".")")
.IgnoreToken(L"ASSIGN",LR"("=")")
.IgnoreToken(L"USING",LR"("!")")
.IgnoreToken(L"OR",LR"("|")")
.IgnoreToken(L"OPTOPEN",LR"("\[")")
.IgnoreToken(L"OPTCLOSE",LR"("\]")")
.IgnoreToken(L"PREOPEN",LR"("\(")")
.IgnoreToken(L"PRECLOSE",LR"("\)")")

						)
						.Type
						(
							
					GeneralTypeListWriter()
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenHeadInfoDefine")
							
							
					.Member(ClassMember(Normal(L"wstring"), L"property"))
				
					.Member(ClassMember(Normal(L"wstring"), L"value"))
				
							
						)
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenTokenDefine")
							
							.SubType(
						
					GeneralTypeListWriter()
					
						.Enum
						(
							GeneralEnumTypeWriter()
							.Name(L"TokenOptional")
							
						.Member(EnumMember(L"False"))
					
						.Member(EnumMember(L"True"))
					
						)
					
				
									)
						
							
					.Member(ClassMember(Normal(L"wstring"), L"name"))
				
					.Member(ClassMember(Normal(L"wstring"), L"regex"))
				
					.Member(ClassMember(Normal(L"TokenOptional"), L"ignore"))
				
							
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
							
							
					.Member(ClassMember(Normal(L"wstring"), L"name"))
				
							.ParentType(Normal(L"GenTypeObject"))
						)
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenSubTypeObject")
							
							
					.Member(ClassMember(Normal(L"GenTypeObject"), L"parent"))
				
					.Member(ClassMember(Normal(L"wstring"), L"name"))
				
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
							.Name(L"GenClassTypeDefine")
							
							
					.Member(ClassMember(Array(Normal(L"GenTypeDefine")), L"subTypes"))
				
					.Member(ClassMember(Array(Normal(L"GenClassMemberTypeDefine")), L"members"))
				
					.Member(ClassMember(Normal(L"GenTypeObject"), L"parent"))
				
					.Member(ClassMember(Normal(L"wstring"), L"name"))
				
							.ParentType(Normal(L"GenTypeDefine"))
						)
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenEnumTypeDefine")
							
							
					.Member(ClassMember(Array(Normal(L"GenEnumMemberTypeDefine")), L"members"))
				
					.Member(ClassMember(Normal(L"wstring"), L"name"))
				
							.ParentType(Normal(L"GenTypeDefine"))
						)
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenClassMemberTypeDefine")
							
							
					.Member(ClassMember(Normal(L"GenTypeObject"), L"type"))
				
					.Member(ClassMember(Normal(L"wstring"), L"name"))
				
							.ParentType(Normal(L"GenTypeDefine"))
						)
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenEnumMemberTypeDefine")
							
							
					.Member(ClassMember(Normal(L"wstring"), L"name"))
				
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
							
							
					.Member(ClassMember(Normal(L"wstring"), L"text"))
				
							.ParentType(Normal(L"GenGrammarTypeDefine"))
						)
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenGrammarNormalTypeDefine")
							
							
					.Member(ClassMember(Normal(L"wstring"), L"name"))
				
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
							
							
					.Member(ClassMember(Normal(L"wstring"), L"name"))
				
					.Member(ClassMember(Normal(L"GenGrammarTypeDefine"), L"grammar"))
				
							.ParentType(Normal(L"GenGrammarTypeDefine"))
						)
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"GenGrammarSetterTypeDefine")
							
							
					.Member(ClassMember(Normal(L"GrammarDef"), L"grammar"))
				
					.Member(ClassMember(Normal(L"wstring"), L"name"))
				
					.Member(ClassMember(Normal(L"wstring"), L"value"))
				
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
				
					.Member(ClassMember(Normal(L"wstring"), L"name"))
				
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
						.Rules
						(
							GeneralRuleListWriter()
							
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"Type")
							.ReturnType(Normal(L"GenTypeObject"))
							
						|
						GrammarSymbol(L"NAME")[L"name"]
					
					.Create(Normal(L"GenNormalTypeObject"))
					
				
						|Text(L""token"")
					.Create(Normal(L"GenTokenTypeObject"))
					
				
						|
						GrammarSymbol(L"Type")[L"parent"]
					+Text(L""."")+
						GrammarSymbol(L"NAME")[L"name"]
					
					.Create(Normal(L"GenSubTypeObject"))
					
				
						|
						GrammarSymbol(L"Type")[L"element"]
					+Text(L""["")+Text(L""]"")
					.Create(Normal(L"GenArrayTypeObject"))
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"EnumMember")
							.ReturnType(Normal(L"GenEnumMemberTypeDefine"))
							
						|
						GrammarSymbol(L"NAME")[L"name"]
					+Text(L"","")
					.Create(Normal(L"GenEnumMemberTypeDefine"))
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"Enum")
							.ReturnType(Normal(L"GenEnumTypeDefine"))
							
						|Text(L""enum"")+
						GrammarSymbol(L"NAME")[L"name"]
					+Text(L""{"")+*(
						GrammarSymbol(L"EnumMember")[L"members"]
					)+Text(L""}"")
					.Create(Normal(L"GenEnumTypeDefine"))
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"ClassMember")
							.ReturnType(Normal(L"GenClassMemberTypeObject"))
							
						|
						GrammarSymbol(L"Type")[L"type"]
					+
						GrammarSymbol(L"NAME")[L"name"]
					+Text(L"";"")
					.Create(Normal(L"GenClassMemberTypeObject"))
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"TypeDecl")
							.ReturnType(Normal(L"GenTypeDefine"))
							
						|
						( !(GrammarSymbol(L"Enum")) | !(GrammarSymbol(L"Class")) )
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"Class")
							.ReturnType(Normal(L"GenClassTypeDefine"))
							
						|
						( 
						( Text(L""struct"") | Text(L""class"")+
						GrammarSymbol(L"NAME")[L"name"]
					+Text(L"":"")+~(
						GrammarSymbol(L"Type")[L"parent"]
					)+Text(L""{"")+
						GrammarSymbol(L"ClassMember")[L"members"]
					 )
					 | *(
						GrammarSymbol(L"TypeDecl")[L"subTypes"]
					)+Text(L""}"")+Text(L"";"") )
					
					.Create(Normal(L"GenClassTypeDefine"))
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"SequenceGrammar")
							.ReturnType(Normal(L"GenGrammarTypeDefine"))
							
						|!(GrammarSymbol(L"PrimitiveGrammar"))
				
						|
						GrammarSymbol(L"SequenceGrammar")[L"first"]
					+
						GrammarSymbol(L"PrimitiveGrammar")[L"second"]
					
					.Create(Normal(L"GenGrammarSequenceTypeDefine"))
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"AlternativeGrammar")
							.ReturnType(Normal(L"GenGrammarTypeDefine"))
							
						|!(GrammarSymbol(L"SequenceGrammar"))
				
						|
						GrammarSymbol(L"AlternativeGrammar")[L"left"]
					+Text(L""|"")+
						GrammarSymbol(L"SequenceGrammar")[L"right"]
					
					.Create(Normal(L"GenGrammarAlternationTypeDefine"))
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"Grammar")
							.ReturnType(Normal(L"GenGrammarTypeDefine"))
							
						|!(GrammarSymbol(L"AlternativeGrammar"))
				
						|
						GrammarSymbol(L"Grammar")[L"grammar"]
					+Text(L""as"")+
						GrammarSymbol(L"Type")[L"type"]
					
					.Create(Normal(L"GenGrammarCreateTypeDefine"))
					
				
						|
						GrammarSymbol(L"Grammar")[L"grammar"]
					+Text(L""with"")+Text(L""{"")+
						GrammarSymbol(L"NAME")[L"name"]
					+Text(L""="")+
						GrammarSymbol(L"STRING")[L"value"]
					+Text(L""}"")
					.Create(Normal(L"GenGrammarSetterTypeDefine"))
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"NormalGrammar")
							.ReturnType(Normal(L"GenGrammarTypeDefine"))
							
						|
						GrammarSymbol(L"NAME")[L"name"]
					
					.Create(Normal(L"GenGrammarNormalTypeDefine"))
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"AssginGrammar")
							.ReturnType(Normal(L"GenGrammarTypeDefine"))
							
						|
						GrammarSymbol(L"NormalGrammar")[L"grammar"]
					+Text(L"":"")+
						GrammarSymbol(L"NAME")[L"name"]
					
					.Create(Normal(L"GenGrammarAssignTypeDefine"))
					
				
						|
						GrammarSymbol(L"STRING")[L"text"]
					
					.Create(Normal(L"GenGrammarTextTypeDefine"))
					
				
						|Text(L""("")+!(GrammarSymbol(L"Grammar"))+Text(L"")"")
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"PrimitiveGrammar")
							.ReturnType(Normal(L"GenGrammarTypeDefine"))
							
						|!(GrammarSymbol(L"AssginGrammar"))
				
						|Text(L""!"")+
						GrammarSymbol(L"NormalGrammar")[L"grammar"]
					
					.Create(Normal(L"GenGrammarUsingTypeDefine"))
					
				
						|Text(L""["")+
						GrammarSymbol(L"Grammar")[L"grammar"]
					+Text(L""]"")
					.Create(Normal(L"GenGrammarOptionalTypeDefine"))
					
				
						|Text(L""{"")+
						GrammarSymbol(L"Grammar")[L"grammar"]
					+Text(L""}"")
					.Create(Normal(L"GenGrammarLoopTypeDefine"))
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"TokenDecl")
							.ReturnType(Normal(L"GenTokenDefine"))
							
						|Text(L""token"")+
						GrammarSymbol(L"NAME")[L"name"]
					+Text(L""="")+
						GrammarSymbol(L"STRING")[L"regex"]
					+Text(L"";"")
					.Create(Normal(L"GenTokenDefine"))
					
						.Setter(L"ignore", L""False"")
					
				
						|Text(L""ignoretoken"")+
						GrammarSymbol(L"NAME")[L"name"]
					+Text(L""="")+
						GrammarSymbol(L"STRING")[L"regex"]
					+Text(L"";"")
					.Create(Normal(L"GenTokenDefine"))
					
						.Setter(L"ignore", L""True"")
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"HeadDecl")
							.ReturnType(Normal(L"GenHeadInfoDefine"))
							
						|
						GrammarSymbol(L"NAME")[L"property"]
					+Text(L"":"")+
						GrammarSymbol(L"STRING")[L"value"]
					+Text(L"";"")
					.Create(Normal(L"GenHeadInfoDefine"))
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"RuleDecl")
							.ReturnType(Normal(L"GenRuleDefine"))
							
						|Text(L""rule"")+
						GrammarSymbol(L"Type")[L"type"]
					+
						GrammarSymbol(L"NAME")[L"name"]
					+Text(L""="")+*(
						GrammarSymbol(L"Grammar")[L"grammars"]
					)+Text(L"";"")
					.Create(Normal(L"GenRuleDefine"))
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"ParserDecl")
							.ReturnType(Normal(L"GenTableDefine"))
							
						|
						( 
						( 
						( *(
						GrammarSymbol(L"HeadDecl")[L"heads"]
					) | *(
						GrammarSymbol(L"TypeDecl")[L"definitions"]
					) )
					 | *(
						GrammarSymbol(L"TokenDecl")[L"definitions"]
					) )
					 | *(
						GrammarSymbol(L"RuleDecl")[L"definitions"]
					) )
					
					.Create(Normal(L"GenTableDefine"))
					
				
						)
				
						);
					return writer.table;;
				}
			