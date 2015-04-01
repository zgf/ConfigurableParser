
				shared_ptr<GeneralTableDefine> BootStrapDefineTable()
				{
					GeneralTableWriter writer;
					writer.Head
						(
							GeneralHeadInfoWriter()
							
						)
						.Token
						(
							GeneralTokenWriter()
							.Token(L"TRUEVALUE",LR"(true)")
.Token(L"FALSEVALUE",LR"(false)")
.Token(L"NULLVALUE",LR"(null)")
.Token(L"OBJOPEN",LR"(\{)")
.Token(L"OBJCLOSE",LR"(\})")
.Token(L"ARROPEN",LR"(\[)")
.Token(L"ARRCLOSE",LR"(\])")
.Token(L"COMMA",LR"(,)")
.Token(L"COLON",LR"(:)")
.Token(L"NUMBER",LR"([\-]?\d+(.\d+)?([eE][+\-]?\d+)?)")
.Token(L"STRING",LR"("(\\"|[^"])*")")
.IgnoreToken(L"SPACE",LR"(\s+)")

						)
						.Type
						(
							
					GeneralTypeListWriter()
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"Node")
							
							
							
						)
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"Literal")
							
							.SubType(
						
					GeneralTypeListWriter()
					
						.Enum
						(
							GeneralEnumTypeWriter()
							.Name(L"Value")
							
						.Member(EnumMember(L"True"))
					
						.Member(EnumMember(L"False"))
					
						.Member(EnumMember(L"Null"))
					
						)
					
				
									)
						
							
					.Member(ClassMember(Normal(L"Value"), L"value"))
				
							.ParentType(Normal(L"Node"))
						)
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"String")
							
							
					.Member(ClassMember(String(), L"content"))
				
							.ParentType(Normal(L"Node"))
						)
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"Number")
							
							
					.Member(ClassMember(String(), L"content"))
				
							.ParentType(Normal(L"Node"))
						)
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"Array")
							
							
					.Member(ClassMember(Array(Normal(L"Node")), L"items"))
				
							.ParentType(Normal(L"Node"))
						)
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"ObjectField")
							
							
					.Member(ClassMember(String(), L"name"))
				
					.Member(ClassMember(Normal(L"Node"), L"value"))
				
							.ParentType(Normal(L"Node"))
						)
					
						.Class
						(
							GeneralClassTypeWriter()
							.Name(L"Object")
							
							
					.Member(ClassMember(Array(Normal(L"ObjectField")), L"fields"))
				
							.ParentType(Normal(L"Node"))
						)
					
				
						)
						.Rules
						(
							GeneralRuleListWriter()
							
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"JLiteral")
							.ReturnType(Normal(L"Node"))
							
						|
						GrammarSymbol(L"STRING")[L"content"]
					
					.Create(Normal(L"String"))
					
				
						|
						GrammarSymbol(L"NUMBER")[L"content"]
					
					.Create(Normal(L"Number"))
					
				
						|Text(L"TRUEVALUE")
					.Create(Normal(L"Literal"))
					
						.Setter(L"value", L"True")
					
				
						|Text(L"FALSEVALUE")
					.Create(Normal(L"Literal"))
					
						.Setter(L"value", L"False")
					
				
						|Text(L"NULLVALUE")
					.Create(Normal(L"Literal"))
					
						.Setter(L"value", L"Null")
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"JField")
							.ReturnType(Normal(L"ObjectField"))
							
						|
						GrammarSymbol(L"STRING")[L"name"]
					+Text(L"COLON")+
						GrammarSymbol(L"JValue")[L"value"]
					
					.Create(Normal(L"ObjectField"))
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"JObject")
							.ReturnType(Normal(L"Object"))
							
						|Text(L"OBJOPEN")+
						GrammarSymbol(L"JField")[L"fields"]
					+Text(L"COMMA")+~(*(
						GrammarSymbol(L"JField")[L"fields"]
					))+Text(L"OBJCLOSE")
					.Create(Normal(L"Object"))
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"JArray")
							.ReturnType(Normal(L"Array"))
							
						|Text(L"ARROPEN")+
						GrammarSymbol(L"JValue")[L"items"]
					+Text(L"COMMA")+~(*(
						GrammarSymbol(L"JValue")[L"items"]
					))+Text(L"ARRCLOSE")
					.Create(Normal(L"Array"))
					
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"JValue")
							.ReturnType(Normal(L"Node"))
							
						|!(GrammarSymbol(L"JLiteral"))
				
						|!(GrammarSymbol(L"JObject"))
				
						|!(GrammarSymbol(L"JArray"))
				
						)
				
						.Rule
						(
							GeneralRuleWriter()
							.Name(L"JRoot")
							.ReturnType(Normal(L"Node"))
							
						|!(GrammarSymbol(L"JObject"))
				
						|!(GrammarSymbol(L"JArray"))
				
						)
				
						);
					return writer.table;;
				}
			