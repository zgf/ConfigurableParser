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
		//当前路径下创建的节点类型和一定赋值了的字段名
		struct RightRecursionInfo
		{
			bool isRecursion = false;
			ptrdiff_t begin;
			ptrdiff_t back;
		};
		struct TerminateToEdgesMapType
		{
			unordered_map<wstring, vector<PDAEdge*>> termnateToEdgesMap;
		};
		class GeneralJumpInfo
		{
			using TerminateMapType = unordered_map<int, TerminateToEdgesMapType>;
		public:
			GeneralJumpInfo()  = delete;
			GeneralJumpInfo(PushDownAutoMachine* _machine);
			~GeneralJumpInfo() noexcept = default;
			GeneralJumpInfo(GeneralJumpInfo&&)  = default;
			GeneralJumpInfo(const GeneralJumpInfo&)  = default;
			GeneralJumpInfo& operator=(GeneralJumpInfo&&)  = default;
			GeneralJumpInfo& operator=(const GeneralJumpInfo&)   = default;
		public:
			SymbolManager*  GetSymbolManager()const;
			PDANode*		GetRoot()const;
			PushDownAutoMachine* GetMachine()const;
			void													CacheJumpInfo();
			void												    CacheEdgeInfo(PDAEdge* edge);
			void													CacheRuleRequiresMap(PDAEdge* edge, const vector< ActionWrap>& ruleStack, vector<wstring>&ruleInfos);


			void													CacheEnterRule(PDAEdge* edge);
			void													CacheTerminateMap(PDAEdge* edge);
			vector<PDAEdge*>*										GetPDAEdgeByTerminate(const int number, const wstring& terminate)const;
			int														GetRootNumber()const;
			const vector<wstring>&									GetRuleRequires(PDAEdge* edge)const;
			bool													IsRightRecursionEdge(PDAEdge* edge)const;
			const  vector<wstring>&									GetRightRecursionRuleRequires(PDAEdge* edge)const;
		private:
			PushDownAutoMachine*															 machine;
			shared_ptr<unordered_map<PDAEdge*, vector<wstring>>>							 ruleRequiresMap;
			shared_ptr<unordered_map<PDAEdge*, vector<wstring>>>							 rightRecursionMap;
			shared_ptr<TerminateMapType>													 terminateMap;
			int																				 rootNumber;
		
		};
		void CreateJumpInfo(GeneralJumpInfo& jumpTable);
		vector<RightRecursionInfo> FindRightRecursionArea(const vector<wstring>&ruleInfos);
		void LogGraphInfo(const wstring& fileName, PushDownAutoMachine& machine);

	}
}