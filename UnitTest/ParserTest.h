#pragma once
#include "../Source/GenerateFile/PureRegex/PureRegexLexer.h"
#include "../Source/GenerateFile/PureRegex/GeneralRegexLexer.h"
#include "GenerateFile\MiniSharp\MiniSharpParser.h"
#include "GenerateFile\MiniSharp\MiniSharpSymbolManager.h"

#include "GenerateFile\TestAssign\TestAssignParser.h"
namespace ztl
{
	namespace general_parser
	{
		namespace parser_test
		{
			void TestMiniSharp()
			{
				auto table = ztl::miniSharp::BootStrapDefineTable();
				ztl::general_parser::GeneralLALRExecutor LALRParser(table);
				LALRParser.SetTokenPool(L"TestMiniSharp.txt");
				LALRParser.BuildParser();
				LARGE_INTEGER BegainTime;
				LARGE_INTEGER EndTime;
				LARGE_INTEGER Frequency;
				QueryPerformanceFrequency(&Frequency);
				QueryPerformanceCounter(&BegainTime);

				LALRParser.GenerateIsomorphismParserTree();

				auto tree = ztl::miniSharp::GenerateHeterogeneousParserTree(LALRParser);
				QueryPerformanceCounter(&EndTime);

				//输出运行时间（单位：s）
				wcout << (double)(EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart << endl;
			}
			void TestJson()
			{
				ztl::general_parser::GeneralLRExecutor LALRParser(ztl::json::BootStrapDefineTable());
				LALRParser.SetTokenPool(L"TestJson.txt");
				LALRParser.BuildParser();
				LARGE_INTEGER BegainTime;
				LARGE_INTEGER EndTime;
				LARGE_INTEGER Frequency;
				QueryPerformanceFrequency(&Frequency);
				QueryPerformanceCounter(&BegainTime);

				LALRParser.GenerateIsomorphismParserTree();

				auto tree = ztl::json::GenerateHeterogeneousParserTree(LALRParser);
				QueryPerformanceCounter(&EndTime);

				//输出运行时间（单位：s）
				wcout << (double) (EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart << endl;
			}
			void TestXml()
			{
				ztl::general_parser::GeneralLRExecutor LALRParser(ztl::xml::BootStrapDefineTable());
				LALRParser.SetTokenPool(L"TestXml.txt");
				LALRParser.BuildParser();
				LARGE_INTEGER BegainTime;
				LARGE_INTEGER EndTime;
				LARGE_INTEGER Frequency;
				QueryPerformanceFrequency(&Frequency);
				QueryPerformanceCounter(&BegainTime);

				LALRParser.GenerateIsomorphismParserTree();

				auto tree = ztl::xml::GenerateHeterogeneousParserTree(LALRParser);
				QueryPerformanceCounter(&EndTime);

				//输出运行时间（单位：s）
				wcout << (double) (EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart << endl;
			}
			void TestTestAssign()
			{
				ztl::general_parser::GeneralLRExecutor LALRParser(ztl::testAssign::BootStrapDefineTable());
				
				LALRParser.SetTokenPool(L"TestTestAssign.txt");
				LALRParser.BuildParser();

				LARGE_INTEGER BegainTime;
				LARGE_INTEGER EndTime;
				LARGE_INTEGER Frequency;
				QueryPerformanceFrequency(&Frequency);
				QueryPerformanceCounter(&BegainTime);

				LALRParser.GenerateIsomorphismParserTree();

				auto tree = ztl::testAssign::GenerateHeterogeneousParserTree(LALRParser);
				LALRParser.ClearEnvironment();
				QueryPerformanceCounter(&EndTime);

				//输出运行时间（单位：s）
				wcout << (double)(EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart << endl;
			}
			void TestPureRegex()
			{
				ztl::general_parser::GeneralLRExecutor LALRParser(ztl::pure_regex::BootStrapDefineTable());
				LALRParser.BuildParser();

				LALRParser.SetTokenPool(ztl::pure_regex::PureRegexParseToken(LR"([a-zA-Z_]\w*)"));
				LARGE_INTEGER BegainTime;
				LARGE_INTEGER EndTime;
				LARGE_INTEGER Frequency;
				QueryPerformanceFrequency(&Frequency);
				QueryPerformanceCounter(&BegainTime);

				LALRParser.GenerateIsomorphismParserTree();

				shared_ptr<ztl::pure_regex::Alternate> tree = std::static_pointer_cast<ztl::pure_regex::Alternate>(ztl::pure_regex::GenerateHeterogeneousParserTree(LALRParser));
				LALRParser.ClearEnvironment();
				QueryPerformanceCounter(&EndTime);

				//输出运行时间（单位：s）
				wcout << (double) (EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart << endl;
			}
			void TestParseGeneralDefine()
			{
				ztl::general_parser::GeneralLRExecutor LALRParser(ztl::general_parser::BootStrapDefineTable());
				LALRParser.SetTokenPool(L"ParserDefine.txt");
				LALRParser.BuildParser();

				LALRParser.GenerateIsomorphismParserTree();
				auto tree = ztl::general_parser::GenerateHeterogeneousParserTree(LALRParser);
			}

			void TestParseXMLDefine()
			{
				ztl::general_parser::GeneralLRExecutor LALRParser(ztl::general_parser::BootStrapDefineTable());
				LALRParser.SetTokenPool(L"Xml.ParserDefine.txt");
				LALRParser.BuildParser();
				LALRParser.GenerateIsomorphismParserTree();
				auto tree = ztl::general_parser::GenerateHeterogeneousParserTree(LALRParser);
			}

			void TestParseJsonDefine()
			{
				ztl::general_parser::GeneralLRExecutor LALRParser(ztl::general_parser::BootStrapDefineTable());
				LALRParser.SetTokenPool(L"Json.ParserDefine.txt");
				LALRParser.BuildParser();
				LALRParser.GenerateIsomorphismParserTree();
				auto tree = ztl::general_parser::GenerateHeterogeneousParserTree(LALRParser);
			}
			
			void TestMiniSharpIRCodeGen()
			{
				auto table = ztl::miniSharp::BootStrapDefineTable();
				ztl::general_parser::GeneralLALRExecutor LALRParser(table);
				LALRParser.BuildParser();
				LALRParser.SetTokenPool(L"TestMiniSharp.Sum.txt");
				LARGE_INTEGER BegainTime;
				LARGE_INTEGER EndTime;
				LARGE_INTEGER Frequency;
				QueryPerformanceFrequency(&Frequency);
				QueryPerformanceCounter(&BegainTime);
				LALRParser.GenerateIsomorphismParserTree();
				auto tree = std::static_pointer_cast<ztl::miniSharp::ParserASTRoot>( ztl::miniSharp::GenerateHeterogeneousParserTree(LALRParser));
				ztl::miniSharp::MiniSharpSymbolManager manager;
				vector<shared_ptr<ztl::miniSharp::ParserASTRoot>> roots = { tree };
				ztl::miniSharp::CollectIdentifierSymbolDefine(&manager, roots);
				ztl::miniSharp::MapIdentifierObjectToDefine(&manager, roots);
				llvm::LLVMContext context;
				ztl::miniSharp::LLVMTypeObjectConstruct(&manager, roots, &context);
				ztl::miniSharp::VerificationTypeInference(&manager, roots, &context);
				QueryPerformanceCounter(&EndTime);

				//输出运行时间（单位：s）
				wcout << (double)(EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart << endl;
			}
		}
	}
}