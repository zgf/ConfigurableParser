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
		PAIR_BUILDER(JumpItem, int, targetIndex, vector<ActionWrap>, actions);
		class GeneralJumpTable
		{
		public:
			GeneralJumpTable()  = default;
			GeneralJumpTable(PushDownAutoMachine* _machine);
			~GeneralJumpTable() noexcept = default;
			GeneralJumpTable(GeneralJumpTable&&)  = default;
			GeneralJumpTable(const GeneralJumpTable&)  = default;
			GeneralJumpTable& operator=(GeneralJumpTable&&)  = default;
			GeneralJumpTable& operator=(const GeneralJumpTable&)   = default;
		public:
			SymbolManager* GetSymbolManager()const;
			PDANode* GetRoot()const;
			int  GetNodeIndex(PDANode* target);

			void													CreateJumpTable();
			void													ClearJumpTable();
			const unordered_map<int, vector<JumpItem>>&				GetJumpTable()const;
			vector<JumpItem>										CreateJumpItem(PDANode* source, unordered_set<PDAEdge*>& sign, 
				deque<PDANode*>& queue);
			void													InitNodeIndexMap();
			void													ClearNodeIndexMap();

		private:
			PushDownAutoMachine*												machine;

			//int 1 nodeIndex int 2 tagIndex  int targetNodeIndex

			unordered_map<int, vector<JumpItem>>								 jumpTable;
			unordered_map<PDANode*, int>										 nodeIndexMap;
		};
		void CreateJumpTable(GeneralJumpTable& jumpTable);
		void HelpLogJumpTable(wstring fileName, GeneralJumpTable& jumpTable);

	}
}