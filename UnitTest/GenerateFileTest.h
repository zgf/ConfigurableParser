#pragma once
namespace ztl
{
	namespace general_parser
	{
		namespace generate_file_test
		{
			void GenerateGeneralParserFile()
			{
				ztl::general_parser::GeneralParserFile parserGen(L"ParserDefine.txt", make_shared<ztl::general_parser::GeneralLALRParser>(ztl::general_parser::BootStrapDefineTable()));
				parserGen.GenerateSpecialParserFile();
			}
			void GenerateXmlParserFile()
			{
				ztl::general_parser::GeneralParserFile parserGen(L"Xml.ParserDefine.txt", make_shared<ztl::general_parser::GeneralLALRParser>(ztl::general_parser::BootStrapDefineTable()));
				parserGen.GenerateSpecialParserFile();
			}
			void GenerateJsonParserFile()
			{
				ztl::general_parser::GeneralParserFile parserGen(L"Json.ParserDefine.txt", make_shared<ztl::general_parser::GeneralLALRParser>(ztl::general_parser::BootStrapDefineTable()));
				parserGen.GenerateSpecialParserFile();
			}
			void GeneratePureRegexParserFile()
			{
				ztl::general_parser::GeneralParserFile parserGen(L"PureRegex.ParserDefine.txt", make_shared<ztl::general_parser::GeneralLALRParser>(ztl::general_parser::BootStrapDefineTable()));
				parserGen.GenerateSpecialParserFile();
			}
			void GenerateFile()
			{
				LARGE_INTEGER BegainTime;
				LARGE_INTEGER EndTime;
				LARGE_INTEGER Frequency;
				QueryPerformanceFrequency(&Frequency);
				QueryPerformanceCounter(&BegainTime);
				GenerateGeneralParserFile();
				GenerateXmlParserFile();
				GenerateJsonParserFile();
				QueryPerformanceCounter(&EndTime);

				//�������ʱ�䣨��λ��s��    
				wcout << (double) (EndTime.QuadPart - BegainTime.QuadPart) / Frequency.QuadPart << endl;
			}
		}
	}
}