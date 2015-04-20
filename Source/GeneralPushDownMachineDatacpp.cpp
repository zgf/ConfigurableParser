#include "Include/stdafx.h"
#include "Include/GeneralPushDownMachineData.h"
#include "Include/ParserSymbol.h"
namespace ztl
{
	namespace general_parser
	{
		const wstring& ActionWrap::GetName()const
		{
			return data->GetName();
		}
	}
}