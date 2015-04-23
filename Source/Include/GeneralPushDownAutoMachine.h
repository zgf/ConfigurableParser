#pragma once
#include "stdafx.h"
#include "../Lib/ZTL/ztl_pair_builder.hpp"

namespace ztl
{
	namespace general_parser
	{
		class ParserSymbol;
		struct GeneralRuleDefine;
		struct GeneralGrammarTypeDefine;
		class SymbolManager;
		class PushDownAutoMachine;
		struct GeneralTableDefine;
		class PDANode;
		class PDAEdge;
		class CreatedNodeResolve;
		class ActionWrap;
		class PushDownAutoMachine
		{
		public:
			PushDownAutoMachine();
			PushDownAutoMachine(SymbolManager* _manager);
			~PushDownAutoMachine() = default;
			PushDownAutoMachine(PushDownAutoMachine&&) = default;
			PushDownAutoMachine(const PushDownAutoMachine&) = default;
			PushDownAutoMachine& operator=(PushDownAutoMachine&&) = default;
			PushDownAutoMachine& operator=(const PushDownAutoMachine&) = default;
		public:
			SymbolManager*				GetSymbolManager()const;
			GeneralTableDefine*			GetTable()const;
			CreatedNodeResolve*			GetCreateNodeResolve();
			unordered_map<wstring, pair<PDANode*, PDANode*>>& GetPDAMap();
			pair<PDANode*, PDANode*>	NewNodePair();
			PDANode*					NewNode();
			PDANode*					GetRoot() const;
			void						AddEdge(PDANode* source, PDANode* target, const ActionWrap& wrap);
			void						AddEdge(PDANode* source, PDANode* target, const vector<ActionWrap>& wrapList);
			void						DeleteEdge(PDAEdge* target);

			void						AddGeneratePDA(wstring ruleName, const pair<PDANode *, PDANode*>& pairNode);
			pair<PDANode*, PDANode*>	AddSequenceLinkNode(pair<PDANode*, PDANode*>& left, pair<PDANode*, PDANode*>&right);
			pair<PDANode*, PDANode*>	AddLoopLinkNode(PDANode* loopStart, PDANode* loopEnd);
			pair<PDANode*, PDANode*>	AddAlterationLinkNode(pair<PDANode*, PDANode*>& left, pair<PDANode*, PDANode*>&right);
			pair<PDANode*, PDANode*>	AddOptionalLinkNode(PDANode* optionalStart, PDANode* optionalEnd);
			void						FrontEdgesAdditionBackAction(PDANode* targetNode, const ActionWrap& wrap);
			void						FrontEdgesAdditionSetterAction(PDANode* targetNode, const ActionWrap& wrap);
			wstring						GetRootRuleName()const;
			//保留Left节点合并left right
			PDANode*					MergeIndependentNodes(PDANode* left, PDANode* right);
			void BackInsertAction(PDAEdge* edge, const ActionWrap& wrap);
			void FrontInsertAction(PDAEdge* edge, const ActionWrap& wrap);
			void AddNodeMapElement(PDANode* node, int ruleIndex);
			int GetRuleIndexByNode(PDANode* node)const;
			void AddNodeEdgeMapElement(PDANode* node, PDAEdge* edge);
			const vector<PDAEdge*>& GetEdgesByNode(PDANode* node)const;
			wstring LogPDANode(const wstring& ruleName,PDANode* node)const;
		private:
			PDAEdge* NewEdge(PDANode* source, PDANode* target, const ActionWrap& wrap);
			PDAEdge* NewEdge(PDANode* source, PDANode* target);

		private:
			vector<shared_ptr<PDAEdge>>											 edges;
			vector<shared_ptr<PDANode>>											 nodes;
			SymbolManager*														 manager;
			unordered_map<wstring, pair<PDANode*, PDANode*>>					 PDAMap;
			unordered_map<PDANode*, int>										 nodeMap;
			unordered_map<PDANode*, vector<PDAEdge*>>							 nodeEdgeMap;
		public:
		};
		vector<PDAEdge*> FindThePath(PDANode* start, PDANode* end);
		void CreateDPDAGraph(PushDownAutoMachine& machine);
		void MergeGrammarCommonFactor(PushDownAutoMachine& machine);
		void MergeEpsilonPDAGraph(PushDownAutoMachine& machine);
		void MergePDAEpsilonSymbol(PushDownAutoMachine& machine);
		void CollectCreateNodeRequires(PushDownAutoMachine& machine);
		void MergeStartAndEndNode(PushDownAutoMachine& machine, unordered_map<wstring, vector<pair<PDANode*, PDANode*>>>& PDAMap);
		void AddFinishNode(PushDownAutoMachine& machine);
		extern void LogGraphInfo(const wstring& fileName, PushDownAutoMachine& machine);
		void RecordNewNode(PDANode* target, vector<PDANode*>& allNode, unordered_set<PDANode*>& noNeed);
	}
}