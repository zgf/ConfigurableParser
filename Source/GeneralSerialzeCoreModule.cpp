#include "Include\stdafx.h"
#include "Include\GeneralLALRParser.h"
#include "Include\SymbolManager.h"
#include "..\Lib\ZTL\ztl_algorithm.h"
namespace ztl
{
	namespace general_parser
	{
		static const wstring namespacePrefix = L"ztl::general_parser::";
		wstring SerializeTypeList(const vector<shared_ptr<GeneralTypeDefine>>& types);

		wstring SerializeAttributeArguments(const vector<shared_ptr<GeneralAttributeArgumentDefine>>& arguments)
		{
			wstring result = LR"(
					$<Namespace>GeneralAttributeParamsWriter()$<Arguments>
					)";
			auto argumentList = accumulate(arguments.begin(), arguments.end(), wstring(),
				[](const wstring& sum, const shared_ptr<GeneralAttributeArgumentDefine>& argument) {
				wstring templateString = LR"(
					.Param(L$<Name>)
				)";
				wstring name = argument->name;
				MarcoGenerator generator(templateString, { L"$<Name>" });
				return sum + generator.GenerateText({ name }).GetMacroResult();
			});
			MarcoGenerator generator(result, { L"$<Namespace>", L"$<Arguments>" });
			return generator.GenerateText({ namespacePrefix,argumentList }).GetMacroResult();

		}
		wstring SerializeAttribute(const vector<shared_ptr<GeneralAttributeDefine>>& attributes)
		{
			wstring result = LR"(
				$<Namespace>GeneralAttributeWriter()
				$<Attributes>
				)";
			auto attributeList = accumulate(attributes.begin(), attributes.end(), wstring(),
				[](const wstring& sum, const shared_ptr<GeneralAttributeDefine>& attribute)
			{
				wstring templateString = LR"(
					.Attribute(L"$<Name>",$<Arguments>)
				)";
				auto name = attribute->name;
				auto arguments = SerializeAttributeArguments(attribute->arguments);
				MarcoGenerator generator(templateString, { L"$<Name>", L"$<Arguments>" });
				return sum + generator.GenerateText({ name,arguments }).GetMacroResult();
			});
			MarcoGenerator generator(result, { L"$<Namespace>", L"$<Attributes>" });
			return generator.GenerateText({ namespacePrefix ,attributeList }).GetMacroResult();

		}

		wstring SerializeHeadInfoList(const vector<shared_ptr<GeneralHeadInfoDefine>>& infos)
		{
			wstring templateString =
				L".Info(LR\"($<Property>)\",LR\"($<Value>,$<Attributes>)\")";
			templateString = 
				LR"(
					)"+ templateString +LR"(
				)";
			return accumulate(infos.begin(), infos.end(), wstring(), [&templateString](const wstring& sum, const shared_ptr<GeneralHeadInfoDefine>& info)
			{
				wstring attributes = SerializeAttribute(info->attributes);
				ztl::generator::MarcoGenerator generator(templateString, { L"$<Property>",L"$<Value>" ,L"$<Attributes>"});
				return sum + generator.GenerateText({ info->property,info->value,attributes }).GetMacroResult();
			});
		}

		wstring SerializeTokenInfoList(const vector<shared_ptr<GeneralTokenDefine>>& tokens)
		{
			wstring templateString =
				L".$<TokenType>(L\"$<TokenName>\",LR\"($<Value>)\",$<Attributes>)\n";
			wstring headString = LR"(						.Token(L"FINISH",L"<\\$>")
			)";
			return accumulate(tokens.begin(), tokens.end(), headString, [&templateString](const wstring& sum, const shared_ptr<GeneralTokenDefine>& token)
			{
				ztl::generator::MarcoGenerator generator(templateString, { L"$<TokenType>",LR"($<TokenName>)",LR"($<Value>)",L"$<Attributes>" });
				wstring tokenType = (token->ignore == GeneralTokenDefine::TokenOptional::True) ? L"IgnoreToken" : L"Token";
				wstring attributes = SerializeAttribute(token->attributes);
				return sum + generator.GenerateText({ tokenType,token->name,token->regex,attributes }).GetMacroResult();
			});
		}
		class SerialzeTypeObjectVisitor:public GeneralTypeObject::IVisitor
		{
		public:
			SerialzeTypeObjectVisitor() = default;
			~SerialzeTypeObjectVisitor() noexcept = default;
			SerialzeTypeObjectVisitor(SerialzeTypeObjectVisitor&&) = default;
			SerialzeTypeObjectVisitor(const SerialzeTypeObjectVisitor&) = default;
			SerialzeTypeObjectVisitor& operator=(SerialzeTypeObjectVisitor&&) = default;
			SerialzeTypeObjectVisitor& operator=(const SerialzeTypeObjectVisitor&) = default;
		public:
			wstring GetResult()const
			{
				return result;
			}
			void								Visit(GeneralArrayTypeObject* node)
			{
				SerialzeTypeObjectVisitor visitor;
				node->element->Accept(&visitor);
				result = namespacePrefix+L"Array(" + visitor.GetResult() + L")";
			}
			void								Visit(GeneralTokenTypeObject*)
			{
				result = namespacePrefix+L"String()";
			}
			void								Visit(GeneralNormalTypeObject* node)
			{
				result = namespacePrefix+L"Normal(L\"" + node->name + L"\")";
			}
			void								Visit(GeneralSubTypeObject* node)
			{
				SerialzeTypeObjectVisitor visitor;
				node->parent->Accept(&visitor);
				wstring templateString = namespacePrefix + L"SubTypeObject($<Parent>,$<Name>)";
				MarcoGenerator generator(templateString, { L"$<Parent>",L"$<Name>" });
				result = generator.GenerateText({ visitor.GetResult() ,node->name }).GetMacroResult();
			}

		private:
			wstring result;
		};
		class SerializeTypeDefineVisitor:public GeneralTypeDefine::IVisitor
		{
		public:
			SerializeTypeDefineVisitor() = default;
			~SerializeTypeDefineVisitor() noexcept = default;
			SerializeTypeDefineVisitor(SerializeTypeDefineVisitor&&) = default;
			SerializeTypeDefineVisitor(const SerializeTypeDefineVisitor&) = default;
			SerializeTypeDefineVisitor& operator=(SerializeTypeDefineVisitor&&) = default;
			SerializeTypeDefineVisitor& operator=(const SerializeTypeDefineVisitor&) = default;
		public:
			wstring GetResult() const
			{
				return result;
			}

			void	Visit(GeneralClassTypeDefine* node)
			{
				wstring templateString =
					LR"(
						.Class
						(
							$<Namespace>GeneralClassTypeWriter()
							.Name(L"$<ClassName>")
							$<SubTypeList>
							$<ClassMemberList>
							$<ParentName>
							.Attributes($<Attributes>)
						)
					)";

				wstring subTypeList;
				wstring classMemberList;
				wstring parentName;

				if(node->parent != nullptr)
				{
					SerialzeTypeObjectVisitor visitor;
					node->parent->Accept(&visitor);
					parentName = L".ParentType(" + visitor.GetResult() + L")";
				}
				if(!node->subTypes.empty())
				{
					subTypeList =
						LR"(
							.SubType(
						)" + SerializeTypeList(node->subTypes) +
						LR"(
									)
						)";
				}
				if(!node->members.empty())
				{
					classMemberList = accumulate(node->members.begin(), node->members.end(), wstring(), [](const wstring& sum, const shared_ptr<GeneralClassMemberTypeDefine>& node)
					{
						SerializeTypeDefineVisitor visitor;
						node->Accept(&visitor);
						return sum + visitor.result;
					});
				}
				
					wstring	attributes = SerializeAttribute(node->attributes);
				
				MarcoGenerator generator(templateString, {L"$<Namespace>", L"$<ClassName>",L"$<SubTypeList>",L"$<ClassMemberList>",L"$<ParentName>" ,L"$<Attributes>"});
				result = generator.GenerateText({namespacePrefix, node->name,subTypeList,classMemberList, parentName,attributes }).GetMacroResult();
			}
			void								Visit(GeneralEnumTypeDefine* node)
			{
				wstring templateString =
					LR"(
						.Enum
						(
							$<Namespace>GeneralEnumTypeWriter()
							.Name(L"$<EnumName>")
							$<EnumItemList>
							.Attributes($<Attributes>)
						)
					)";
				wstring enumItemList = accumulate(node->members.begin(), node->members.end(), wstring(), [](const wstring& sum, const shared_ptr<GeneralEnumMemberTypeDefine>& node)
				{
					SerializeTypeDefineVisitor visitor;
					node->Accept(&visitor);
					return sum + visitor.result;
				});
				
				auto attributes = SerializeAttribute(node->attributes);
				
				MarcoGenerator generator(templateString, { L"$<Namespace>",L"$<EnumName>",L"$<EnumItemList>",L"$<Attributes>" });
				result = generator.GenerateText({namespacePrefix, node->name,enumItemList,attributes }).GetMacroResult();
			}
			void								Visit(GeneralClassMemberTypeDefine* node)
			{
				SerialzeTypeObjectVisitor visitor;
				node->type->Accept(&visitor);
				auto fieldType = visitor.GetResult();
				wstring templateString =
					LR"(
					.Member($<Namespace>ClassMember($<FieldType>, L"$<Value>",$<Attributes>))
				)";
				wstring attributes = SerializeAttribute(node->attributes);
				MarcoGenerator generator(templateString, {L"$<Namespace>", L"$<FieldType>",L"$<Value>" ,L"$<Attributes>"});
				result = generator.GenerateText({ namespacePrefix, fieldType,node->name,attributes }).GetMacroResult();
			}
			void								Visit(GeneralEnumMemberTypeDefine* node)
			{
				wstring templateString =
					LR"(
						.Member($<Namespace>EnumMember(L"$<Value>",$<Attributes>))
					)";
				wstring attributes = SerializeAttribute(node->attributes);
				MarcoGenerator generator(templateString, {L"$<Namespace>", L"$<Value>" ,L"$<Attributes>" });
				result = generator.GenerateText({ namespacePrefix, node->name ,attributes }).GetMacroResult();
			}
		
		private:
			wstring result;
		};
		wstring SerializeTypeList(const vector<shared_ptr<GeneralTypeDefine>>& types)
		{
			wstring templateString =
				
				LR"(
					$<Namespace>GeneralTypeListWriter()
					$<TypeList>
				)";
			auto typeList = accumulate(types.begin(), types.end(), wstring(), [](const wstring& sum, const shared_ptr<GeneralTypeDefine>& type)
			{
				SerializeTypeDefineVisitor visitor;
				type->Accept(&visitor);
				return sum + visitor.GetResult();
			});
			MarcoGenerator generator(templateString, { L"$<Namespace>", L"$<TypeList>" });
			return generator.GenerateText({ namespacePrefix,typeList }).GetMacroResult();
		}
		
		class SerializeGrammarDefineVisitor:public GeneralGrammarTypeDefine::IVisitor
		{
		public:
			SerializeGrammarDefineVisitor() = default;
			~SerializeGrammarDefineVisitor() noexcept = default;
			SerializeGrammarDefineVisitor(SerializeGrammarDefineVisitor&&) = default;
			SerializeGrammarDefineVisitor(const SerializeGrammarDefineVisitor&) = default;
			SerializeGrammarDefineVisitor& operator=(SerializeGrammarDefineVisitor&&) = default;
			SerializeGrammarDefineVisitor& operator=(const SerializeGrammarDefineVisitor&) = default;
		public:
			wstring GetResult()const
			{
				return result;
			}
			void								Visit(GeneralGrammarTextTypeDefine* node)
			{
				//这里要注意,因为Text在验证语义的时候regex被转成了tokenName,所以用GrammarSymbol
				result = namespacePrefix+L"GrammarSymbol(L\"" + node->text + L"\")";
			}
			void								Visit(GeneralGrammarNormalTypeDefine* node)
			{
				result = namespacePrefix+ L"GrammarSymbol(L\"" + node->name + L"\")";
			}
			void								Visit(GeneralGrammarSequenceTypeDefine* node)
			{
				node->first->Accept(this);
				SerializeGrammarDefineVisitor secondVistor;
				node->second->Accept(&secondVistor);
				result += L"+" + secondVistor.GetResult();
			}
			void								Visit(GeneralGrammarLoopTypeDefine* node)
			{
				node->grammar->Accept(this);
				result = L"*(" + result + L")";
			}
			void								Visit(GeneralGrammarOptionalTypeDefine* node)
			{
				node->grammar->Accept(this);
				result = L"~(" + result + L")";
			}
			void								Visit(GeneralGrammarSetterTypeDefine* node)
			{
				node->grammar->Accept(this);
				const wstring templateString =
					LR"(
						.Setter(L"$<Key>", L"$<Value>")
					)";
				MarcoGenerator generator(templateString, { L"$<Key>",L"$<Value>" });
				result += generator.GenerateText({ node->name,node->value }).GetMacroResult();
			}
			void								Visit(GeneralGrammarUsingTypeDefine* node)
			{
				node->grammar->Accept(this);
				result = L"!(" + result + L")";
			}
			void								Visit(GeneralGrammarCreateTypeDefine* node)
			{
				SerialzeTypeObjectVisitor visitor;
				node->type->Accept(&visitor);
				wstring createType = visitor.GetResult();
				node->grammar->Accept(this);
				const wstring templateString =
					LR"(
					(
						$<Statement>
					)
					.Create($<ReturnType>)
					)";
				MarcoGenerator generator(templateString, { L"$<Statement>",L"$<ReturnType>" });
				result = generator.GenerateText({ result, createType }).GetMacroResult();
			}
			void								Visit(GeneralGrammarAlternationTypeDefine* node)
			{
				const wstring templateString =
					LR"(
						( $<Left> | $<Right> )
					)";
				MarcoGenerator generator(templateString, { L"$<Left>",L"$<Right>" });
				node->left->Accept(this);
				SerializeGrammarDefineVisitor visitor;
				node->right->Accept(&visitor);
				result = generator.GenerateText({ result,visitor.GetResult() }).GetMacroResult();
			}
			void								Visit(GeneralGrammarAssignTypeDefine* node)
			{
				const wstring templateString =
					LR"(
						$<Value>[L"$<FieldName>"]
					)";
				node->grammar->Accept(this);
				MarcoGenerator generator(templateString, { L"$<Value>",L"$<FieldName>" });
				result = generator.GenerateText({ result,node->name }).GetMacroResult();
			}
		private:
			wstring result;
		};
		wstring SerializeGrammarList(const vector<shared_ptr<GeneralGrammarTypeDefine>>& grammars)
		{
			const wstring templateString =
				LR"(
						|$<GrammarString>
				)";
			return accumulate(grammars.begin(), grammars.end(), wstring(), [&templateString](const wstring& sum, const shared_ptr<GeneralGrammarTypeDefine>& grammar)
			{
				SerializeGrammarDefineVisitor visitor;
				grammar->Accept(&visitor);
				MarcoGenerator generator(templateString, { L"$<GrammarString>" });
				return sum + generator.GenerateText({ visitor.GetResult() }).GetMacroResult();
			});
		}
		wstring SerializeRuleList(const vector<shared_ptr<GeneralRuleDefine>>& rules)
		{
			wstring templateString =
				LR"(
						.Rule
						(
							$<Namespace>GeneralRuleWriter()
							.Name(L"$<RuleName>")
							.ReturnType($<Returnype>)
							$<GrammarList>
							$<Attributes>
						)
				)";
			return	accumulate(rules.begin(), rules.end(), wstring(), [&templateString](const wstring&sum, const shared_ptr<GeneralRuleDefine>& rule)
			{
				wstring grammarList = SerializeGrammarList(rule->grammars);
				SerialzeTypeObjectVisitor visitor;
				rule->type->Accept(&visitor);
				wstring returnType = visitor.GetResult();
				wstring attributes = L"";
				if (!rule->attributes.empty())
				{
					attributes = SerializeAttribute(rule->attributes);
				}
				MarcoGenerator generator(templateString, {L"$<Namespace>", L"$<RuleName>",L"$<Returnype>",L"$<GrammarList>",L"$<Attributes>" });
				return sum + generator.GenerateText({ namespacePrefix,rule->name,returnType,grammarList,attributes }).GetMacroResult();
			});
		}
		wstring SerializeEBNFCoreModuleHead()
		{
			wstring templateString =
				LR"(
				shared_ptr<$<Namespace>GeneralTableDefine> BootStrapDefineTable();
				)";
			ztl::generator::MarcoGenerator generator(templateString, { L"$<Namespace>" });
			return generator.GenerateText({ namespacePrefix }).GetMacroResult();
		}

		 

		wstring SerializeEBNFCoreModuleImp(void* tableDefine)
		{
			auto table = static_cast<ztl::general_parser::GeneralTableDefine*>(tableDefine);
			wstring templateString =
				LR"(
				shared_ptr<$<Namespace>GeneralTableDefine> BootStrapDefineTable()
				{
					$<Namespace>GeneralTableWriter writer;
					writer.Head
						(
							$<Namespace>GeneralHeadInfoWriter()
							$<HeadInfoList>
						)
						.Token
						(
							$<Namespace>GeneralTokenWriter()
							$<TokenInfoList>
						)
						.Type
						(
							$<TypeList>
						)
						.Rules
						(
							$<Namespace>GeneralRuleListWriter()
							$<RuleList>
						);
					return writer.table;;
				}
			)";
			wstring headInfoList = SerializeHeadInfoList(table->heads);
			wstring tokenInfoList = SerializeTokenInfoList(table->tokens);
			wstring typeList = SerializeTypeList(table->types);
			wstring ruleList = SerializeRuleList(table->rules);
			ztl::generator::MarcoGenerator generator(templateString, {L"$<Namespace>", L"$<HeadInfoList>",L"$<TokenInfoList>",L"$<TypeList>",L"$<RuleList>" });
			return generator.GenerateText({ namespacePrefix,headInfoList ,tokenInfoList,typeList,ruleList }).GetMacroResult();
		}
	}
}