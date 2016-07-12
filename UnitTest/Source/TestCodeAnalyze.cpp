#pragma once
#include "Include/stdafx.h"
#include "../Include/TestCodeAnalyze.h"

#include "../../Source/Include/ParserSymbol.h"
#include "../../Source/Include/SymbolManager.h"
#include "../../Source/Include/GeneralGrammarBuilder.h"
#include "../../Source/Include/GeneralGrammarBuilderData.h"
#include "../../Source/Include/GeneralLRBuilder.h"
#include "../../Source/GenerateFile/Xml/XmlParser.h"
#include "../../Source/GenerateFile/Json/JsonParser.h"
#include "../../Source/GenerateFile/PureRegex/PureRegexParser.h"

void ztl::general_parser::unit_test::TestGetStartSymbol()
{
	auto testInstance = [](auto&& table,auto&& name)
	{
		ztl::general_parser::SymbolManager manager(table);
		SemanticAnalysis(&manager);
		assert(manager.GetRootSymbol() == manager.GetRuleSymbolByName(name));
	};
	testInstance(ztl::general_parser::BootStrapDefineTable(), L"ParserDecl");
	testInstance(ztl::xml::BootStrapDefineTable(), L"XDocument");
	testInstance(ztl::json::BootStrapDefineTable(), L"JRoot");
	testInstance(ztl::pure_regex::BootStrapDefineTable(), L"RegexRootRule");
}

void ztl::general_parser::unit_test::TestFirstSet()
{
	auto table = ztl::general_parser::BootStrapDefineTable();
	auto manager = make_shared<ztl::general_parser::SymbolManager>(table);
	SemanticAnalysis(manager.get());
	auto machine = make_shared<GrammarBuilder>(manager.get());
	ztl::general_parser::CreateDPDAGraph(*machine.get());
	auto LRMachine = make_shared<GeneralLRBuilder>(machine);
	LRMachine->CalculateFirstSet();
	unordered_map<wstring, set<wstring>> target =
	{
		{ L"AttributeArgument",{L"STRING"} },
		{ L"Attribute",{ L"ATSIGN" } },
		{ L"Type",{ L"NAME",L"TOKEN" } },
		{ L"TypeDecl",{ L"CLASS",L"STRUCT",L"ENUM" } },
		{ L"EnumMember",{ L"NAME" } },
		{ L"Enum",{ L"ENUM" } },
		{ L"ClassMember",{ L"NAME",L"TOKEN" } },
		{ L"Class",{ L"CLASS",L"STRUCT" } },
		{ L"SequenceGrammar",{ L"STRING" ,L"NAME",L"OPEN",L"OPTOPEN",L"PREOPEN",L"USING" } },
		{ L"AlternativeGrammar",{ L"STRING" ,L"NAME",L"OPEN",L"OPTOPEN",L"PREOPEN",L"USING" } },
		{ L"PrimitiveGrammar",{ L"STRING" ,L"NAME",L"OPEN",L"OPTOPEN",L"PREOPEN",L"USING"} },
		{ L"Grammar",{ L"STRING" ,L"NAME",L"OPEN",L"OPTOPEN",L"PREOPEN",L"USING" } },
		{ L"TokenDecl",{ L"TOKEN" ,L"DISCARDTOKEN"} },
		{ L"HeadDecl",{ L"NAME" } },
		{ L"RuleDecl",{ L"RULE" } },
		{ L"ParserDecl",{ L"NAME" , L"RULE" ,L"TOKEN" ,L"DISCARDTOKEN" ,L"CLASS",L"STRUCT",L"ENUM" } },
	};
	auto set = LRMachine->GetFirstSet();
	for (auto&& setIter : set)
	{
		auto key = setIter.first->GetName();
		std::set<wstring> value;
		std::for_each(setIter.second.begin(), setIter.second.end(),  [&value](ParserSymbol* val) { value.insert(val->GetName());});
		auto expect = target[key];
		assert(value == expect);
	}
}
