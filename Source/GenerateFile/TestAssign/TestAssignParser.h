
				#pragma once
				#include <memory>
				#include <string>
				#include <vector>
				#include <unordered_map>
				#include <iostream>
				#include <assert.h>
				using std::pair;
				using std::wstring;
				using std::shared_ptr;
				using std::vector;
				using std::unordered_map;
				using std::wifstream;
				using std::make_shared;
				#include "..\..\Include\GeneralTableDefine.h"
				#include "..\..\Include\GeneralTreeNode.h"
				#include "..\..\Include\GeneralLALRExecutor.h"
				
							namespace ztl
							{
								
							namespace testAssign
							{
								


			
			struct Variable 
			{
				
				wstring    name;

				
				
				
			};
			


			struct Expression;
struct BinaryExpression;
struct AssignExpression;
struct PrimaryExpression;

			struct Expression 
			{
				
				
				
				
				class IVisitor
				{
				public:
					virtual void		Visit(Expression* node) =0;
virtual void		Visit(BinaryExpression* node) =0;
virtual void		Visit(AssignExpression* node) =0;
virtual void		Visit(PrimaryExpression* node) =0;

				};
				virtual void									Accept(IVisitor* )
				{

				}
				
				
			};
			


			
			struct PrimaryExpression : public Expression
			{
				
				wstring    name;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct AssignExpression : public Expression
			{
				
				shared_ptr<Variable>    left;
shared_ptr<Expression>    right;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			


			
			struct BinaryExpression : public Expression
			{
				
				shared_ptr<Expression>    left;
shared_ptr<Expression>    right;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
				
			};
			
				shared_ptr<ztl::general_parser::GeneralTableDefine> BootStrapDefineTable();
				
			shared_ptr<void> GenerateHeterogeneousParserTree(ztl::general_parser::GeneralLRExecutor& parser,ztl::general_parser::GeneralTreeNode* root);
			shared_ptr<void>	GenerateHeterogeneousParserTree(ztl::general_parser::GeneralLRExecutor& parser);
			
							}
						 
							}
						 