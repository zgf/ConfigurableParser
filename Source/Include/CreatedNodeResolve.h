#pragma once
#include "stdafx.h"
namespace ztl
{
	namespace general_parser
	{
		class PDANode;
		class PDAEdge;
		class ParserSymbol;
		class ActionWrap;
		class PushDownAutoMachine;
		class CreatedNodeResolve
		{
		public:
			CreatedNodeResolve() = default;
			~CreatedNodeResolve() noexcept = default;
			CreatedNodeResolve(CreatedNodeResolve&&) = default;
			CreatedNodeResolve(const CreatedNodeResolve&) = default;
			CreatedNodeResolve& operator=(CreatedNodeResolve&&) = default;
			CreatedNodeResolve& operator=(const CreatedNodeResolve&) = default;
		public:
			unordered_map<PDANode*, unordered_map<ParserSymbol*, PDANode*>>	& GetCreatedDFAMap();
			unordered_map<wstring, PDANode*>& GetCreatedDFA();

			unordered_map<PDANode*, unordered_map<ParserSymbol*, PDANode*>>	& GetRuleDFAMap();
			unordered_map<wstring, PDANode*>& GetRuleDFA();
			void CacheRightRecursionArea(wstring number, const vector<ActionWrap>& wrapList);
			const vector<ActionWrap>& GetNullRightRecursionAreas(const wstring& number)const;
			bool IsNullRightRecursionAreas(const wstring& number)const;
			
		private:

			unordered_map<wstring, PDANode*>									 createdDFA;
			unordered_map<PDANode*, unordered_map<ParserSymbol*, PDANode*>>		 createdDFAMap;
			unordered_map<wstring, PDANode* >									 ruleDFA;
			unordered_map<PDANode*, unordered_map<ParserSymbol*, PDANode*>>		 ruleDFAMap;
			unordered_map<wstring, vector<ActionWrap>>							 nullRightRecursionAreas;

		};
		void CreateResolveDFA(PushDownAutoMachine& machine);
		void CacheNullRightRecursionAreas(PushDownAutoMachine& machine, PDANode* start, PDANode* target, const vector<ActionWrap>& startActions);
	}
}