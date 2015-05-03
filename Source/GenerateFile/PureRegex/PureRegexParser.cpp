#include "Include/stdafx.h"

#include "PureRegexParser.h"
#include "..\..\Include\GeneralTableWriter.h"

namespace ztl
{
	namespace pure_regex
	{
		int WstringToEnumItem(const wstring& value)
		{
			static unordered_map<wstring, int> signMap =
			{
			{
				L"Repeat::RepeatType::KLEENE",
				static_cast<int>(Repeat::RepeatType::KLEENE)
			},
			{
				L"Repeat::RepeatType::KLEENEGREEDY",
				static_cast<int>(Repeat::RepeatType::KLEENEGREEDY)
			},
			{
				L"Repeat::RepeatType::CHOCIEKLEENE",
				static_cast<int>(Repeat::RepeatType::CHOCIEKLEENE)
			},
			{
				L"Repeat::RepeatType::CHOCIEKLEENEGREEDY",
				static_cast<int>(Repeat::RepeatType::CHOCIEKLEENEGREEDY)
			},
			{
				L"Repeat::RepeatType::POSITIVEKLEENE",
				static_cast<int>(Repeat::RepeatType::POSITIVEKLEENE)
			},
			{
				L"Repeat::RepeatType::POSITIVEKLEENEGREEDY",
				static_cast<int>(Repeat::RepeatType::POSITIVEKLEENEGREEDY)
			},
			{
				L"CharSet::CharSetOptional::Positve",
				static_cast<int>(CharSet::CharSetOptional::Positve)
			},
			{
				L"CharSet::CharSetOptional::Reverse",
				static_cast<int>(CharSet::CharSetOptional::Reverse)
			},
			{
				L"UserDefineFactor::UserDefineType::TROPEw",
				static_cast<int>(UserDefineFactor::UserDefineType::TROPEw)
			},
			{
				L"UserDefineFactor::UserDefineType::TROPEW",
				static_cast<int>(UserDefineFactor::UserDefineType::TROPEW)
			},
			{
				L"UserDefineFactor::UserDefineType::TROPEs",
				static_cast<int>(UserDefineFactor::UserDefineType::TROPEs)
			},
			{
				L"UserDefineFactor::UserDefineType::TROPES",
				static_cast<int>(UserDefineFactor::UserDefineType::TROPES)
			},
			{
				L"UserDefineFactor::UserDefineType::TROPEd",
				static_cast<int>(UserDefineFactor::UserDefineType::TROPEd)
			},
			{
				L"UserDefineFactor::UserDefineType::TROPED",
				static_cast<int>(UserDefineFactor::UserDefineType::TROPED)
			},
			{
				L"UserDefineFactor::UserDefineType::MatchAll",
				static_cast<int>(UserDefineFactor::UserDefineType::MatchAll)
			},
			};
			assert(signMap.find(value) != signMap.end());
			return signMap[value];
		}
		void ReflectionBuidler(const wstring& className, const wstring& fieldName, const shared_ptr<void>& classObject, const shared_ptr<void>& valueObject)
		{
			using builderType = void(*)(const shared_ptr<void>&, const shared_ptr<void>&);
			static unordered_map<wstring, unordered_map<wstring, builderType>> builderMap =
			{
				{
							L"Factor",
							{
							}
						},{
							L"Define",
							{
							}
						},{
							L"Sequence",
							{
								{
									L"first",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<Sequence>(classObject)->first = std::static_pointer_cast<Define>(valueObject);
										return;
									}
								},{
									L"second",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<Sequence>(classObject)->second = std::static_pointer_cast<Define>(valueObject);
										return;
									}
								},
							}
						},{
							L"Repeat",
							{
								{
									L"type",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<Repeat>(classObject)->type = static_cast<Repeat::RepeatType>(WstringToEnumItem(L"Repeat::RepeatType::" + std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content));
										return;
									}
								},
							}
						},{
							L"Alternate",
							{
								{
									L"left",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<Alternate>(classObject)->left = std::static_pointer_cast<Define>(valueObject);
										return;
									}
								},{
									L"right",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<Alternate>(classObject)->right = std::static_pointer_cast<Define>(valueObject);
										return;
									}
								},
							}
						},{
							L"NormalFactor",
							{
								{
									L"name",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<NormalFactor>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
										return;
									}
								},
							}
						},{
							L"CharSet",
							{
								{
									L"tokens",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<CharSet>(classObject)->tokens.emplace_back(std::static_pointer_cast<Factor>(valueObject));
										return;
									}
								},{
									L"type",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<CharSet>(classObject)->type = static_cast<CharSet::CharSetOptional>(WstringToEnumItem(L"CharSet::CharSetOptional::" + std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content));
										return;
									}
								},
							}
						},{
							L"BinaryFactor",
							{
								{
									L"end",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<BinaryFactor>(classObject)->end = std::static_pointer_cast<NormalFactor>(valueObject);
										return;
									}
								},{
									L"begin",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<BinaryFactor>(classObject)->begin = std::static_pointer_cast<NormalFactor>(valueObject);
										return;
									}
								},
							}
						},{
							L"UserDefineFactor",
							{
								{
									L"factor",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<UserDefineFactor>(classObject)->factor = std::static_pointer_cast<CharSet>(valueObject);
										return;
									}
								},{
									L"type",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<UserDefineFactor>(classObject)->type = static_cast<UserDefineFactor::UserDefineType>(WstringToEnumItem(L"UserDefineFactor::UserDefineType::" + std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content));
										return;
									}
								},
							}
						},{
							L"TokenUnit",
							{
								{
									L"charset",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<TokenUnit>(classObject)->charset = std::static_pointer_cast<Define>(valueObject);
										return;
									}
								},{
									L"repeat",
									[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
									{
										std::static_pointer_cast<TokenUnit>(classObject)->repeat = std::static_pointer_cast<Repeat>(valueObject);
										return;
									}
								},
							}
						},
			};
			auto findIter = builderMap.find(className);
			assert(findIter != builderMap.end());
			assert(findIter->second.find(fieldName) != findIter->second.end());
			builderMap[className][fieldName](classObject, valueObject);
		}
		shared_ptr<void> ReflecteObjectByName(const wstring& name)
		{
			using reflectObjectType = shared_ptr<void>(*)();
			static unordered_map < wstring, reflectObjectType> objectMap =
			{
			{
				L"Factor",
				[]()->shared_ptr<void>
				{
					return make_shared<Factor>();
				}
			},
			{
				L"Define",
				[]()->shared_ptr<void>
				{
					return make_shared<Define>();
				}
			},
			{
				L"Sequence",
				[]()->shared_ptr<void>
				{
					return make_shared<Sequence>();
				}
			},
			{
				L"Repeat",
				[]()->shared_ptr<void>
				{
					return make_shared<Repeat>();
				}
			},
			{
				L"Alternate",
				[]()->shared_ptr<void>
				{
					return make_shared<Alternate>();
				}
			},
			{
				L"NormalFactor",
				[]()->shared_ptr<void>
				{
					return make_shared<NormalFactor>();
				}
			},
			{
				L"CharSet",
				[]()->shared_ptr<void>
				{
					return make_shared<CharSet>();
				}
			},
			{
				L"BinaryFactor",
				[]()->shared_ptr<void>
				{
					return make_shared<BinaryFactor>();
				}
			},
			{
				L"UserDefineFactor",
				[]()->shared_ptr<void>
				{
					return make_shared<UserDefineFactor>();
				}
			},
			{
				L"TokenUnit",
				[]()->shared_ptr<void>
				{
					return make_shared<TokenUnit>();
				}
			},
			};
			assert(objectMap.find(name) != objectMap.end());
			return objectMap[name]();
		}
		void GeneralHeterogeneousParserTree(ztl::general_parser::GeneralParserBase& parser, ztl::general_parser::GeneralTreeNode* classNode, shared_ptr<void>& classObject)
		{
			assert(classObject != nullptr);
			assert(classNode != nullptr);
			auto className = classNode->GetName();
			for(auto&& iter : classNode->GetFieldMap())
			{
				auto fieldName = iter.first;
				for(auto&& nodeIter : iter.second)
				{
					auto fieldNode = parser.GetNonTermNodeByIndex(nodeIter);
					auto fieldObject = ReflecteObjectByName(fieldNode->GetName());
					parser.SaveHeterogeneousNode(fieldObject);
					ReflectionBuidler(className, fieldName, classObject, fieldObject);
					ztl::pure_regex::GeneralHeterogeneousParserTree(parser, fieldNode, fieldObject);
				}
			}
			for(auto&&iter : classNode->GetTermMap())
			{
				auto fieldName = iter.first;
				for(auto&& nodeIter : iter.second)
				{
					ReflectionBuidler(className, fieldName, classObject, parser.GetTermNodeByIndex(nodeIter));
				}
			}
		}

		shared_ptr<void> GeneralHeterogeneousParserTree(ztl::general_parser::GeneralParserBase& parser, ztl::general_parser::GeneralTreeNode* root)
		{
			assert(root != nullptr);
			auto rootObject = ReflecteObjectByName(root->GetName());
			parser.SaveHeterogeneousNode(rootObject);
			ztl::pure_regex::GeneralHeterogeneousParserTree(parser, root, rootObject);
			return rootObject;
		}
		shared_ptr<void>	GeneralHeterogeneousParserTree(ztl::general_parser::GeneralParserBase& parser)
		{
			return ztl::pure_regex::GeneralHeterogeneousParserTree(parser, parser.GetGeneralTreeRoot());
		}

		shared_ptr<ztl::general_parser::GeneralTableDefine> BootStrapDefineTable()
		{
			ztl::general_parser::GeneralTableWriter writer;
			writer.Head
				(
				ztl::general_parser::GeneralHeadInfoWriter()

				.Info(LR"(classprefix)", LR"(PureRegex)")

				.Info(LR"(filename)", LR"(PureRegexParser)")

				.Info(LR"(include)", LR"(Include/stdafx.h)")

				.Info(LR"(namespace)", LR"(ztl)")

				.Info(LR"(namespace)", LR"(pure_regex)")

				.Info(LR"(dirname)", LR"(PureRegex)")

				)
				.Token
				(
				ztl::general_parser::GeneralTokenWriter()
				.Token(L"FINISH", L"<\\$>")
				.Token(L"CHASETBEGIN", LR"(\[)")
				.Token(L"CHASETREVERSEBEGIN", LR"(\[^)")
				.Token(L"CHASETEND", LR"(\])")
				.Token(L"PARENTOPEN", LR"(\()")
				.Token(L"PARENTCLOSE", LR"(\))")
				.Token(L"KLEENEGREEDY", LR"(\*)")
				.Token(L"KLEENE", LR"(\*\?)")
				.Token(L"POSITIVEKLEENEGREEDY", LR"(\+)")
				.Token(L"POSITIVEKLEENE", LR"(\+\?)")
				.Token(L"CHOCIEKLEENEGREEDY", LR"(\?)")
				.Token(L"CHOCIEKLEENE", LR"(\?\?)")
				.Token(L"CHOCIE", LR"(\|)")
				.Token(L"CONPONENT", LR"(-)")
				.Token(L"TROPE", LR"(\\)")
				.Token(L"TROPEW", LR"(\\W)")
				.Token(L"TROPEw", LR"(\\w)")
				.Token(L"TROPED", LR"(\\D)")
				.Token(L"TROPEd", LR"(\\d)")
				.Token(L"TROPES", LR"(\\S)")
				.Token(L"TROPEs", LR"(\\s)")
				.Token(L"MatchAll", LR"(\.)")
				.Token(L"NORMAL", LR"(siguoyi)")

				)
				.Type
				(

				ztl::general_parser::GeneralTypeListWriter()

				.Class
				(
				ztl::general_parser::GeneralClassTypeWriter()
				.Name(L"Define")

				)

				.Class
				(
				ztl::general_parser::GeneralClassTypeWriter()
				.Name(L"Sequence")

				.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"Define"), L"first"))

				.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"Define"), L"second"))

				.ParentType(ztl::general_parser::Normal(L"Define"))
				)

				.Class
				(
				ztl::general_parser::GeneralClassTypeWriter()
				.Name(L"Alternate")

				.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"Define"), L"left"))

				.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"Define"), L"right"))

				.ParentType(ztl::general_parser::Normal(L"Define"))
				)

				.Class
				(
				ztl::general_parser::GeneralClassTypeWriter()
				.Name(L"Repeat")

				.SubType(

				ztl::general_parser::GeneralTypeListWriter()

				.Enum
				(
				ztl::general_parser::GeneralEnumTypeWriter()
				.Name(L"RepeatType")

				.Member(ztl::general_parser::EnumMember(L"KLEENEGREEDY"))

				.Member(ztl::general_parser::EnumMember(L"KLEENE"))

				.Member(ztl::general_parser::EnumMember(L"POSITIVEKLEENEGREEDY"))

				.Member(ztl::general_parser::EnumMember(L"POSITIVEKLEENE"))

				.Member(ztl::general_parser::EnumMember(L"CHOCIEKLEENEGREEDY"))

				.Member(ztl::general_parser::EnumMember(L"CHOCIEKLEENE"))

				)

				)

				.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"RepeatType"), L"type"))

				.ParentType(ztl::general_parser::Normal(L"Define"))
				)

				.Class
				(
				ztl::general_parser::GeneralClassTypeWriter()
				.Name(L"Factor")

				.ParentType(ztl::general_parser::Normal(L"Define"))
				)

				.Class
				(
				ztl::general_parser::GeneralClassTypeWriter()
				.Name(L"NormalFactor")

				.Member(ztl::general_parser::ClassMember(ztl::general_parser::String(), L"name"))

				.ParentType(ztl::general_parser::Normal(L"Factor"))
				)

				.Class
				(
				ztl::general_parser::GeneralClassTypeWriter()
				.Name(L"BinaryFactor")

				.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"NormalFactor"), L"begin"))

				.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"NormalFactor"), L"end"))

				.ParentType(ztl::general_parser::Normal(L"Factor"))
				)

				.Class
				(
				ztl::general_parser::GeneralClassTypeWriter()
				.Name(L"CharSet")

				.SubType(

				ztl::general_parser::GeneralTypeListWriter()

				.Enum
				(
				ztl::general_parser::GeneralEnumTypeWriter()
				.Name(L"CharSetOptional")

				.Member(ztl::general_parser::EnumMember(L"Positve"))

				.Member(ztl::general_parser::EnumMember(L"Reverse"))

				)

				)

				.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"CharSetOptional"), L"type"))

				.Member(ztl::general_parser::ClassMember(ztl::general_parser::Array(ztl::general_parser::Normal(L"Factor")), L"tokens"))

				.ParentType(ztl::general_parser::Normal(L"Define"))
				)

				.Class
				(
				ztl::general_parser::GeneralClassTypeWriter()
				.Name(L"UserDefineFactor")

				.SubType(

				ztl::general_parser::GeneralTypeListWriter()

				.Enum
				(
				ztl::general_parser::GeneralEnumTypeWriter()
				.Name(L"UserDefineType")

				.Member(ztl::general_parser::EnumMember(L"TROPEW"))

				.Member(ztl::general_parser::EnumMember(L"TROPEw"))

				.Member(ztl::general_parser::EnumMember(L"TROPES"))

				.Member(ztl::general_parser::EnumMember(L"TROPEs"))

				.Member(ztl::general_parser::EnumMember(L"TROPED"))

				.Member(ztl::general_parser::EnumMember(L"TROPEd"))

				.Member(ztl::general_parser::EnumMember(L"MatchAll"))

				)

				)

				.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"CharSet"), L"factor"))

				.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"UserDefineType"), L"type"))

				.ParentType(ztl::general_parser::Normal(L"Factor"))
				)

				.Class
				(
				ztl::general_parser::GeneralClassTypeWriter()
				.Name(L"TokenUnit")

				.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"Define"), L"charset"))

				.Member(ztl::general_parser::ClassMember(ztl::general_parser::Normal(L"Repeat"), L"repeat"))

				.ParentType(ztl::general_parser::Normal(L"Define"))
				)

				)
				.Rules
				(
				ztl::general_parser::GeneralRuleListWriter()

				.Rule
				(
				ztl::general_parser::GeneralRuleWriter()
				.Name(L"NormalFactorRule")
				.ReturnType(ztl::general_parser::Normal(L"NormalFactor"))

				|
				(

				ztl::general_parser::GrammarSymbol(L"NORMAL")[L"name"]

				)
				.Create(ztl::general_parser::Normal(L"NormalFactor"))

				)

				.Rule
				(
				ztl::general_parser::GeneralRuleWriter()
				.Name(L"BinaryFactorRule")
				.ReturnType(ztl::general_parser::Normal(L"Factor"))

				|
				(

				ztl::general_parser::GrammarSymbol(L"NormalFactorRule")[L"begin"]
				+ ztl::general_parser::GrammarSymbol(L"CONPONENT") +
				ztl::general_parser::GrammarSymbol(L"NormalFactorRule")[L"end"]

				)
				.Create(ztl::general_parser::Normal(L"BinaryFactor"))

				)

				.Rule
				(
				ztl::general_parser::GeneralRuleWriter()
				.Name(L"UserDefineFactorRule")
				.ReturnType(ztl::general_parser::Normal(L"Factor"))

				|
				(
				ztl::general_parser::GrammarSymbol(L"TROPEW")
				)
				.Create(ztl::general_parser::Normal(L"UserDefineFactor"))

				.Setter(L"type", L"TROPEW")

				|
				(
				ztl::general_parser::GrammarSymbol(L"TROPEw")
				)
				.Create(ztl::general_parser::Normal(L"UserDefineFactor"))

				.Setter(L"type", L"TROPEw")

				|
				(
				ztl::general_parser::GrammarSymbol(L"TROPED")
				)
				.Create(ztl::general_parser::Normal(L"UserDefineFactor"))

				.Setter(L"type", L"TROPED")

				|
				(
				ztl::general_parser::GrammarSymbol(L"TROPEd")
				)
				.Create(ztl::general_parser::Normal(L"UserDefineFactor"))

				.Setter(L"type", L"TROPEd")

				|
				(
				ztl::general_parser::GrammarSymbol(L"TROPES")
				)
				.Create(ztl::general_parser::Normal(L"UserDefineFactor"))

				.Setter(L"type", L"TROPES")

				|
				(
				ztl::general_parser::GrammarSymbol(L"TROPEs")
				)
				.Create(ztl::general_parser::Normal(L"UserDefineFactor"))

				.Setter(L"type", L"TROPEs")

				)

				.Rule
				(
				ztl::general_parser::GeneralRuleWriter()
				.Name(L"FactorRule")
				.ReturnType(ztl::general_parser::Normal(L"Factor"))

				|
				(!(ztl::general_parser::GrammarSymbol(L"NormalFactorRule")) |
				(!(ztl::general_parser::GrammarSymbol(L"BinaryFactorRule")) | !(ztl::general_parser::GrammarSymbol(L"UserDefineFactorRule")))
				)

				)

				.Rule
				(
				ztl::general_parser::GeneralRuleWriter()
				.Name(L"CharSetRule")
				.ReturnType(ztl::general_parser::Normal(L"Define"))

				|
				(
				ztl::general_parser::GrammarSymbol(L"CHASETBEGIN") + *(
				ztl::general_parser::GrammarSymbol(L"FactorRule")[L"tokens"]
				) + ztl::general_parser::GrammarSymbol(L"CHASETEND")
				)
				.Create(ztl::general_parser::Normal(L"CharSet"))

				.Setter(L"type", L"Positve")

				|
				(
				ztl::general_parser::GrammarSymbol(L"CHASETREVERSEBEGIN") + *(
				ztl::general_parser::GrammarSymbol(L"FactorRule")[L"tokens"]
				) + ztl::general_parser::GrammarSymbol(L"CHASETEND")
				)
				.Create(ztl::general_parser::Normal(L"CharSet"))

				.Setter(L"type", L"Reverse")

				| !(ztl::general_parser::GrammarSymbol(L"FactorRule"))

				)

				.Rule
				(
				ztl::general_parser::GeneralRuleWriter()
				.Name(L"RepeatRule")
				.ReturnType(ztl::general_parser::Normal(L"Repeat"))

				|
				(
				ztl::general_parser::GrammarSymbol(L"KLEENEGREEDY")
				)
				.Create(ztl::general_parser::Normal(L"Repeat"))

				.Setter(L"type", L"KLEENEGREEDY")

				|
				(
				ztl::general_parser::GrammarSymbol(L"KLEENE")
				)
				.Create(ztl::general_parser::Normal(L"Repeat"))

				.Setter(L"type", L"KLEENE")

				|
				(
				ztl::general_parser::GrammarSymbol(L"POSITIVEKLEENEGREEDY")
				)
				.Create(ztl::general_parser::Normal(L"Repeat"))

				.Setter(L"type", L"POSITIVEKLEENEGREEDY")

				|
				(
				ztl::general_parser::GrammarSymbol(L"POSITIVEKLEENE")
				)
				.Create(ztl::general_parser::Normal(L"Repeat"))

				.Setter(L"type", L"POSITIVEKLEENE")

				|
				(
				ztl::general_parser::GrammarSymbol(L"CHOCIEKLEENE")
				)
				.Create(ztl::general_parser::Normal(L"Repeat"))

				.Setter(L"type", L"CHOCIEKLEENEGREEDY")

				|
				(
				ztl::general_parser::GrammarSymbol(L"CHOCIEKLEENEGREEDY")
				)
				.Create(ztl::general_parser::Normal(L"Repeat"))

				.Setter(L"type", L"CHOCIEKLEENE")

				)

				.Rule
				(
				ztl::general_parser::GeneralRuleWriter()
				.Name(L"SequenceRule")
				.ReturnType(ztl::general_parser::Normal(L"Define"))

				|
				(

				ztl::general_parser::GrammarSymbol(L"SequenceRule")[L"first"]
				+
				ztl::general_parser::GrammarSymbol(L"TokenUnitRule")[L"second"]

				)
				.Create(ztl::general_parser::Normal(L"Sequence"))

				| !(ztl::general_parser::GrammarSymbol(L"TokenUnitRule"))

				)

				.Rule
				(
				ztl::general_parser::GeneralRuleWriter()
				.Name(L"TokenUnitRule")
				.ReturnType(ztl::general_parser::Normal(L"Define"))

				|
				(

				ztl::general_parser::GrammarSymbol(L"CharSetRule")[L"charset"]
				+
				ztl::general_parser::GrammarSymbol(L"RepeatRule")[L"repeat"]

				)
				.Create(ztl::general_parser::Normal(L"TokenUnit"))

				| !(ztl::general_parser::GrammarSymbol(L"CharSetRule"))

				| ztl::general_parser::GrammarSymbol(L"PARENTOPEN") + !(ztl::general_parser::GrammarSymbol(L"AlertRule")) + ztl::general_parser::GrammarSymbol(L"PARENTCLOSE")

				)

				.Rule
				(
				ztl::general_parser::GeneralRuleWriter()
				.Name(L"AlertRule")
				.ReturnType(ztl::general_parser::Normal(L"Define"))

				|
				(

				ztl::general_parser::GrammarSymbol(L"AlertRule")[L"left"]
				+
				ztl::general_parser::GrammarSymbol(L"SequenceRule")[L"right"]

				)
				.Create(ztl::general_parser::Normal(L"Alternate"))

				| !(ztl::general_parser::GrammarSymbol(L"SequenceRule"))

				)

				);
			return writer.table;;
		}
	}
}