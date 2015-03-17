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

		
		class GeneralJumpTable
		{
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
			void													CacheEnterRuleMap(PDAEdge* edge);
		private:
			PushDownAutoMachine*															 machine;
			shared_ptr<unordered_map<int, vector<JumpItem>>>								 jumpTable;
			shared_ptr<unordered_map<PDAEdge*, vector<wstring>>>							 ruleRequiresMap;
			shared_ptr<unordered_map<PDAEdge*, wstring>>									 terminateMap;
			shared_ptr<unordered_map<PDAEdge*, vector<CreateInfo>>>							 createdNodeRequiresMap;
			shared_ptr<unordered_map<PDAEdge*, wstring>>									 enterRuleMap;
			
		};
		void CreateJumpTable(GeneralJumpTable& jumpTable);
		void HelpLogJumpTable(wstring fileName, GeneralJumpTable& jumpTable);

	}
}