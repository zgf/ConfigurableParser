#pragma once
#include "../Source/GenerateFile/PureRegex/PureRegexLexer.h"
#include "../Source/GenerateFile/PureRegex/GeneralLexer.h"
#include "GenerateFile\MiniSharp\MiniSharpParser.h"
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
				ztl::general_parser::GeneralLALRParser LALRParser(table);
				LALRParser.SetTokenPool(L"TestMiniSharp.txt");
				LALRParser.BuildParser();
				LARGE_INTEGER BegainTime;
				LARGE_INTEGER EndTime;
				LARGE_INTEGER Frequency;
				QueryPerformanceFrequency(&Frequency);
				QueryPerformanceCounter(&BegainTime);

				LALRParser.GenerateIsomorphismParserTree();

				auto tree = ztl::miniSharp::GeneralHeterogeneousParserTree(LALRParser);
				QueryPerformanceCounter(&EndTime);

				//输出运行时间（单位：s）
				wcout << (double)(EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart << endl;
			}
			void TestJson()
			{
				ztl::general_parser::GeneralLALRParser LALRParser(ztl::json::BootStrapDefineTable());
				LALRParser.SetTokenPool(L"TestJson.txt");
				LALRParser.BuildParser();
				LARGE_INTEGER BegainTime;
				LARGE_INTEGER EndTime;
				LARGE_INTEGER Frequency;
				QueryPerformanceFrequency(&Frequency);
				QueryPerformanceCounter(&BegainTime);

				LALRParser.GenerateIsomorphismParserTree();

				auto tree = ztl::json::GeneralHeterogeneousParserTree(LALRParser);
				QueryPerformanceCounter(&EndTime);

				//输出运行时间（单位：s）
				wcout << (double) (EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart << endl;
			}
			void TestXml()
			{
				ztl::general_parser::GeneralLALRParser LALRParser(ztl::xml::BootStrapDefineTable());
				LALRParser.SetTokenPool(L"TestXml.txt");
				LALRParser.BuildParser();
				LARGE_INTEGER BegainTime;
				LARGE_INTEGER EndTime;
				LARGE_INTEGER Frequency;
				QueryPerformanceFrequency(&Frequency);
				QueryPerformanceCounter(&BegainTime);

				LALRParser.GenerateIsomorphismParserTree();

				auto tree = ztl::xml::GeneralHeterogeneousParserTree(LALRParser);
				QueryPerformanceCounter(&EndTime);

				//输出运行时间（单位：s）
				wcout << (double) (EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart << endl;
			}
			void TestTestAssign()
			{
				ztl::general_parser::GeneralLALRParser LALRParser(ztl::testAssign::BootStrapDefineTable());
				
				LALRParser.SetTokenPool(L"TestTestAssign.txt");
				LALRParser.BuildParser();

				LARGE_INTEGER BegainTime;
				LARGE_INTEGER EndTime;
				LARGE_INTEGER Frequency;
				QueryPerformanceFrequency(&Frequency);
				QueryPerformanceCounter(&BegainTime);

				LALRParser.GenerateIsomorphismParserTree();

				auto tree = ztl::testAssign::GeneralHeterogeneousParserTree(LALRParser);
				LALRParser.ClearEnvironment();
				QueryPerformanceCounter(&EndTime);

				//输出运行时间（单位：s）
				wcout << (double)(EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart << endl;
			}
			void TestPureRegex()
			{
				ztl::general_parser::GeneralLALRParser LALRParser(ztl::pure_regex::BootStrapDefineTable());
				LALRParser.BuildParser();

				LALRParser.SetTokenPool(ztl::pure_regex::PureRegexParseToken(LR"([a-zA-Z_]\w*)"));
				LARGE_INTEGER BegainTime;
				LARGE_INTEGER EndTime;
				LARGE_INTEGER Frequency;
				QueryPerformanceFrequency(&Frequency);
				QueryPerformanceCounter(&BegainTime);

				LALRParser.GenerateIsomorphismParserTree();

				shared_ptr<ztl::pure_regex::Alternate> tree = std::static_pointer_cast<ztl::pure_regex::Alternate>(ztl::pure_regex::GeneralHeterogeneousParserTree(LALRParser));
				LALRParser.ClearEnvironment();
				QueryPerformanceCounter(&EndTime);

				//输出运行时间（单位：s）
				wcout << (double) (EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart << endl;
			}
			void TestParseGeneralDefine()
			{
				ztl::general_parser::GeneralLALRParser LALRParser(ztl::general_parser::BootStrapDefineTable());
				LALRParser.SetTokenPool(L"ParserDefine.txt");
				LALRParser.BuildParser();

				LALRParser.GenerateIsomorphismParserTree();
				auto tree = ztl::general_parser::GeneralHeterogeneousParserTree(LALRParser);
			}

			void TestParseXMLDefine()
			{
				ztl::general_parser::GeneralLALRParser LALRParser(ztl::general_parser::BootStrapDefineTable());
				LALRParser.SetTokenPool(L"Xml.ParserDefine.txt");
				LALRParser.BuildParser();
				LALRParser.GenerateIsomorphismParserTree();
				auto tree = ztl::general_parser::GeneralHeterogeneousParserTree(LALRParser);
			}

			void TestParseJsonDefine()
			{
				ztl::general_parser::GeneralLALRParser LALRParser(ztl::general_parser::BootStrapDefineTable());
				LALRParser.SetTokenPool(L"Json.ParserDefine.txt");
				LALRParser.BuildParser();
				LALRParser.GenerateIsomorphismParserTree();
				auto tree = ztl::general_parser::GeneralHeterogeneousParserTree(LALRParser);
			}
			
		}
	}
}