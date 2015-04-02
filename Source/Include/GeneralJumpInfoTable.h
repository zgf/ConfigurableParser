#pragma once
#include "../Lib/ZTL/ztl_pair_builder.hpp"

namespace ztl
{
	namespace general_parser
	{
		class PushDownAutoMachine;
		class SymbolManager;
		class PDANode;
		class ActionWrap;
		class PDAEdge;
	
		PAIR_BUILDER(JumpItem, int, targetIndex, PDAEdge*, edges);
		PAIR_BUILDER(CreateInfo, wstring, createType, wstring, fieldName);

		struct TerminateToEdgesMapType
		{
			unordered_map<wstring, vector<PDAEdge*>> termnateToEdgesMap;
		};
		class GeneralJumpInfoTable
		{
			using TerminateMapType = unordered_map<int, TerminateToEdgesMapType>;
		public:
			GeneralJumpInfoTable()  = delete;
			GeneralJumpInfoTable(PushDownAutoMachine* _machine);
			~GeneralJumpInfoTable() noexcept = default;
			GeneralJumpInfoTable(GeneralJumpInfoTable&&)  = default;
			GeneralJumpInfoTable(const GeneralJumpInfoTable&)  = default;
			GeneralJumpInfoTable& operator=(GeneralJumpInfoTable&&)  = default;
			GeneralJumpInfoTable& operator=(const GeneralJumpInfoTable&)   = default;
		public:
			SymbolManager*  GetSymbolManager()const;
			PDANode*		GetRoot()const;
			
			void													CreateJumpInfoTable();
			void												    CacheEdgeInfo(PDAEdge* edge);
			void													CacheCreatedNodeRequiresMap(PDAEdge * edge, const vector<ActionWrap>& nodeStack, vector<CreateInfo>& createInfos);
			void													CacheRuleRequiresMap(PDAEdge* edge, const vector< ActionWrap>& ruleStack, vector<wstring>&ruleInfos);
			void													CacheEnterRule(PDAEdge* edge);
			void													CacheTerminateMap(PDAEdge* edge);
			vector<PDAEdge*>*										GetPDAEdgeByTerminate(const int number, const wstring& terminate)const;
			int														GetRootNumber()const;
			const vector<wstring>&									GetRuleRequires(PDAEdge* edge)const;
			const vector<CreateInfo>*								GetCreateNodeRequires(PDAEdge* edge)const;
		private:
			PushDownAutoMachine*															 machine;
			shared_ptr<unordered_map<PDAEdge*, vector<wstring>>>							 ruleRequiresMap;
			shared_ptr<unordered_map<PDAEdge*, vector<CreateInfo>>>							 createdNodeRequiresMap;
			shared_ptr<TerminateMapType>													 terminateMap;
			int																				 rootNumber;
		
		};
		void CreateJumpInfoTable(GeneralJumpInfoTable& jumpTable);

	}
}