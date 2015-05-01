#pragma once
namespace ztl
{
	namespace general_parser
	{
		namespace parser_test
		{

			void TestJson()
			{
				ztl::general_parser::GeneralLALRParser LALRParser(ztl::json::BootStrapDefineTable());
				LALRParser.BuildParser(L"TestJson.txt");
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
				LALRParser.BuildParser(L"TestXml.txt");
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

			void TestPureRegex()
			{
				ztl::general_parser::GeneralLALRParser LALRParser(ztl::pure_regex::BootStrapDefineTable());
				LALRParser.BuildParser(ztl::pure_regex::PureRegexParseToken(LR"([a-zA-Z_]\w*)"));
				LARGE_INTEGER BegainTime;
				LARGE_INTEGER EndTime;
				LARGE_INTEGER Frequency;
				QueryPerformanceFrequency(&Frequency);
				QueryPerformanceCounter(&BegainTime);

				LALRParser.GenerateIsomorphismParserTree();

				auto tree = (ztl::pure_regex::Alternate*)ztl::pure_regex::GeneralHeterogeneousParserTree(LALRParser).get();

				QueryPerformanceCounter(&EndTime);

				//输出运行时间（单位：s）    
				wcout << (double) (EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart << endl;
			}
			void TestParseGeneralDefine()
			{
				ztl::general_parser::GeneralLALRParser LALRParser(ztl::general_parser::BootStrapDefineTable());
				LALRParser.BuildParser(L"ParserDefine.txt");
				LALRParser.GenerateIsomorphismParserTree();
				auto tree = ztl::general_parser::GeneralHeterogeneousParserTree(LALRParser);
			}

			void TestParseXMLDefine()
			{
				ztl::general_parser::GeneralLALRParser LALRParser(ztl::general_parser::BootStrapDefineTable());
				LALRParser.BuildParser(L"Xml.ParserDefine.txt");
				LALRParser.GenerateIsomorphismParserTree();
				auto tree = ztl::general_parser::GeneralHeterogeneousParserTree(LALRParser);
			}

			void TestParseJsonDefine()
			{
				ztl::general_parser::GeneralLALRParser LALRParser(ztl::general_parser::BootStrapDefineTable());
				LALRParser.BuildParser(L"Json.ParserDefine.txt");
				LALRParser.GenerateIsomorphismParserTree();
				auto tree = ztl::general_parser::GeneralHeterogeneousParserTree(LALRParser);
			}
		}
	}
}