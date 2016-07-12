#pragma once

namespace ztl
{
	namespace general_parser
	{
		namespace generate_file_test
		{

			void GenerateTestAssignParserFile()
			{
				ztl::general_parser::GeneralParserFileGenerator parserGen(L"TestAsssign.ParserDefine.txt", make_shared<ztl::general_parser::GeneralLALRExecutor>(ztl::general_parser::BootStrapDefineTable()));
				parserGen.GenerateSpecialParserFile();
			}
			void GenerateMiniSharpParserFile()
			{
				ztl::general_parser::GeneralParserFileGenerator parserGen(L"MiniSharp.ParserDefine.txt", make_shared<ztl::general_parser::GeneralLALRExecutor>(ztl::general_parser::BootStrapDefineTable()));
				parserGen.GenerateSpecialParserFile();
			}
			void GenerateGeneralParserFileGenerator()
			{
				ztl::general_parser::GeneralParserFileGenerator parserGen(L"ParserDefine.txt", make_shared<ztl::general_parser::GeneralLALRExecutor>(ztl::general_parser::BootStrapDefineTable()));
				parserGen.GenerateSpecialParserFile();
			}
			void GenerateXmlParserFile()
			{
				ztl::general_parser::GeneralParserFileGenerator parserGen(L"Xml.ParserDefine.txt", make_shared<ztl::general_parser::GeneralLALRExecutor>(ztl::general_parser::BootStrapDefineTable()));
				parserGen.GenerateSpecialParserFile();
			}
			void GenerateJsonParserFile()
			{
				ztl::general_parser::GeneralParserFileGenerator parserGen(L"Json.ParserDefine.txt", make_shared<ztl::general_parser::GeneralLALRExecutor>(ztl::general_parser::BootStrapDefineTable()));
				parserGen.GenerateSpecialParserFile();
			}
			void GeneratePureRegexParserFile()
			{
				ztl::general_parser::GeneralParserFileGenerator parserGen(L"PureRegex.ParserDefine.txt", make_shared<ztl::general_parser::GeneralLALRExecutor>(ztl::general_parser::BootStrapDefineTable()));
				parserGen.GenerateSpecialParserFile();
			}
			void GenerateFile()
			{
				LARGE_INTEGER BegainTime;
				LARGE_INTEGER EndTime;
				LARGE_INTEGER Frequency;
				QueryPerformanceFrequency(&Frequency);
				QueryPerformanceCounter(&BegainTime);
				GenerateGeneralParserFileGenerator();
				GenerateXmlParserFile();
				GenerateJsonParserFile();
				QueryPerformanceCounter(&EndTime);

				//输出运行时间（单位：s）    
				wcout << (double) (EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart << endl;
			}
		}
	}
}