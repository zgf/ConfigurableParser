#pragma once
#include "stdafx.h"
namespace ztl
{
	namespace general_parser
	{
		class GeneralRuleDefine;
		class GeneratePath;
		class SymbolManager;
		class GeneralTableDefine;
		class PushDownAutoMachine
		{
		public:
			PushDownAutoMachine();
			~PushDownAutoMachine()  = default;
			PushDownAutoMachine(PushDownAutoMachine&&)  = default;
			PushDownAutoMachine(const PushDownAutoMachine&)  = default;
			PushDownAutoMachine& operator=(PushDownAutoMachine&&)  = default;
			PushDownAutoMachine& operator=(const PushDownAutoMachine&)   = default;
		private: 
		
		private:
			GeneralTableDefine* table;
			SymbolManager* manager;
			unique_ptr<unordered_map<GeneralRuleDefine*, vector<shared_ptr<GeneratePath>>> > pathMap;
		};
		

	}
}