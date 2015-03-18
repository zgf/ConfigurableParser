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
		class GeneralJumpTable
		{
			using TerminateMapType = unordered_map<int, TerminateToEdgesMapType>;
		public:
			GeneralJumpTable()  = delete;
			GeneralJumpTable(PushDownAutoMachine* _machine);
			~GeneralJumpTable() noexcept = default;
			GeneralJumpTable(GeneralJumpTable&&)  = default;
			GeneralJumpTable(const GeneralJumpTable&)  = default;
			GeneralJumpTable& operator=(GeneralJumpTable&&)  = default;
			GeneralJumpTable& operator=(const GeneralJumpTable&)   = default;
		public:
			SymbolManager*  GetSymbolManager()const;
			PDANode*		GetRoot()const;

			void													CreateJumpTable();
			void													ClearJumpTable();
			const unordered_map<int, vector<JumpItem>>&				GetJumpTable()const;
			vector<JumpItem>										CreateJumpItem(PDANode* source, unordered_set<PDAEdge*>& sign, 
				deque<PDANode*>& queue);
			void												    CacheEdgeInfo(PDAEdge* edge);
			void													CacheCreatedNodeRequiresMap(PDAEdge * edge, const vector<ActionWrap>& nodeStack, vector<CreateInfo>& createInfos);
			void													CacheRuleRequiresMap(PDAEdge* edge, const vector< ActionWrap>& ruleStack, vector<wstring>&ruleInfos);
			void													CacheEnterRule(PDAEdge* edge);
			void													CacheTerminateMap(PDAEdge* edge,const wstring& terminate);
			const vector<PDAEdge*>*									GetCachePDAEdgeByTerminate(const int number, const wstring& terminate)const;
			int														GetRootNumber()const;
			const vector<wstring>*									GetRuleRequires(PDAEdge* edge)const;
			const vector<CreateInfo>*								GetCreateNodeRequires(PDAEdge* edge)const;
		private:
			PushDownAutoMachine*															 machine;
			shared_ptr<unordered_map<int, vector<JumpItem>>>								 jumpTable;
			shared_ptr<unordered_map<PDAEdge*, unique_ptr<vector<wstring>>>>				 ruleRequiresMap;
			shared_ptr<unordered_map<PDAEdge*, vector<CreateInfo>>>							 createdNodeRequiresMap;
			shared_ptr<TerminateMapType>													 terminateMap;
			int																				 rootNumber;
		
		};
		void CreateJumpTable(GeneralJumpTable& jumpTable);
		void HelpLogJumpTable(wstring fileName, GeneralJumpTable& jumpTable);

	}
}