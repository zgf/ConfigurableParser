#pragma once
#include "PureRegexParser.h"
namespace ztl
{
	namespace pure_regex
	{
		shared_ptr<Alternate> operator|(const shared_ptr<Define>& left, const shared_ptr<Define>& right)
		{
			auto result = make_shared<Alternate>();
			result->left = left;
			result->right = right;
			return result;
		}
		shared_ptr<Sequence> operator+(const shared_ptr<Define>& first, const shared_ptr<Define>& second)
		{
			auto result = make_shared<Sequence>();
			result->first = first;
			result->second = second;
			return result;
		}
		shared_ptr<TokenUnit> MakeTokenUnit(const shared_ptr<Define>& expression)
		{
			auto result = make_shared<TokenUnit>();
			result->charset = expression;
			result->repeat = nullptr;
			return result;
		}

		shared_ptr<Repeat> MakeRepeat(Repeat::RepeatType type)
		{
			auto result = make_shared<Repeat>();
			result->type = type;
			return result;
		}
		shared_ptr<TokenUnit> MakeTokenUnit(const shared_ptr<Define>& expression, Repeat::RepeatType type)
		{
			auto result = make_shared<TokenUnit>();
			result->charset = expression;
			result->repeat = MakeRepeat(type);
			return result;
		}
		shared_ptr<NormalFactor> MakeNormalFactor(const wstring& token)
		{
			auto reuslt = make_shared<NormalFactor>();
			reuslt->name = token;
			return reuslt;
		}
		shared_ptr<NormalFactor> MakeNormalFactor(wchar_t token)
		{
			auto reuslt = make_shared<NormalFactor>();
			reuslt->name = wstring(1,token);
			return reuslt;
		}
		shared_ptr<BinaryFactor> MakeBinaryFactor(const wstring& begin, const wstring& end)
		{
			auto reuslt = make_shared<BinaryFactor>();
			reuslt->begin = MakeNormalFactor(begin);
			reuslt->end = MakeNormalFactor(end);
			return reuslt;
		}
		shared_ptr<BinaryFactor> MakeBinaryFactor(wchar_t begin, wchar_t end)
		{
			auto reuslt = make_shared<BinaryFactor>();
			reuslt->begin = MakeNormalFactor(wstring(1,begin));
			reuslt->end = MakeNormalFactor(wstring(1, end));
			return reuslt;
		}
		shared_ptr<BinaryFactor> MakeBinaryFactor(const shared_ptr<NormalFactor>& begin, const shared_ptr<NormalFactor>& end)
		{
			auto reuslt = make_shared<BinaryFactor>();
			reuslt->begin = begin;
			reuslt->end = end;
			return reuslt;
		}
		
		shared_ptr<CharSet> MakeCharSet(std::initializer_list<shared_ptr<Factor>> tokens,CharSet::CharSetOptional type = CharSet::CharSetOptional::Positve)
		{
			auto result = make_shared<CharSet>();
			result->tokens = tokens;
			result->type = type;
			return result;
		}
		shared_ptr<CharSet> MakeTroped()
		{
			return MakeCharSet({MakeBinaryFactor(L"0",L"9")});
		}
		shared_ptr<CharSet> MakeTropeD()
		{
			return MakeCharSet({ MakeBinaryFactor(L"0",L"9") }, CharSet::CharSetOptional::Reverse);
		}
		shared_ptr<CharSet>MakeTropes()
		{
			return MakeCharSet({ MakeNormalFactor(L"\t"),MakeNormalFactor(L" "),MakeNormalFactor(L"\f"),MakeNormalFactor(L"\v") });
		}
		shared_ptr<CharSet> MakeTropeS()
		{
			return MakeCharSet({ MakeNormalFactor(L"\t"),MakeNormalFactor(L" "),MakeNormalFactor(L"\f"),MakeNormalFactor(L"\v") },CharSet::CharSetOptional::Reverse);
		}
		shared_ptr<CharSet> MakeTropeW()
		{
			return MakeCharSet({ MakeBinaryFactor(L"0",L"9") ,MakeBinaryFactor(L"a",L"z"),MakeBinaryFactor(L"A",L"Z"),MakeNormalFactor(L"_"),
			MakeBinaryFactor(0x4E00,0x9FA5),MakeBinaryFactor(0xF900,0xFA2D)}, CharSet::CharSetOptional::Reverse);

		}
		shared_ptr<CharSet> MakeTropew()
		{
			return MakeCharSet({ MakeBinaryFactor(L"0",L"9") ,MakeBinaryFactor(L"a",L"z"),MakeBinaryFactor(L"A",L"Z"),MakeNormalFactor(L"_"),
				MakeBinaryFactor(0x4E00,0x9FA5),MakeBinaryFactor(0xF900,0xFA2D) });
		}
		shared_ptr<CharSet> MakeMatchAll()
		{
			return  MakeCharSet({ MakeBinaryFactor(0,65535) });
		}
		shared_ptr<CharSet> MakeUseDefinChaSet(UserDefineFactor::UserDefineType type)
		{
			shared_ptr<CharSet> result;
			switch(type)
			{
				case ztl::pure_regex::UserDefineFactor::UserDefineType::TROPEW:
					result = MakeTropeW();
					break;
				case ztl::pure_regex::UserDefineFactor::UserDefineType::TROPEw:
					result = MakeTropew();
					break;
				case ztl::pure_regex::UserDefineFactor::UserDefineType::TROPES:
					result = MakeTropeS();
					break;
				case ztl::pure_regex::UserDefineFactor::UserDefineType::TROPEs:
					result = MakeTropes();
					break;
				case ztl::pure_regex::UserDefineFactor::UserDefineType::TROPED:
					result = MakeTropeD();
					break;
				case ztl::pure_regex::UserDefineFactor::UserDefineType::TROPEd:
					result = MakeTroped();
					break;
				case ztl::pure_regex::UserDefineFactor::UserDefineType::MatchAll:
					result = MakeMatchAll();
					break;
				default:
					assert(false);
					break;
			}
			return result;
		}
	}
}