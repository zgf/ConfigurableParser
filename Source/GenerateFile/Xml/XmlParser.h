
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
				#include "GeneralTableDefine.h"
				#include "GeneralTreeNode.h"
				#include "GeneralParser.h"
				
							namespace ztl
							{
								
							namespace xml
							{
								


			struct Document;
struct Instruction;
struct Element;
struct Comment;
struct Attribute;
struct CData;
struct Text;

			struct Node 
			{
				
				
				
				
				class IVisitor
				{
				public:
					virtual void		Visit(Document* node) =0;
virtual void		Visit(Instruction* node) =0;
virtual void		Visit(Element* node) =0;
virtual void		Visit(Comment* node) =0;
virtual void		Visit(Attribute* node) =0;
virtual void		Visit(CData* node) =0;
virtual void		Visit(Text* node) =0;

				};
				virtual void									Accept(IVisitor* )
				{

				}
				
			};
			


			
			struct Text : public Node
			{
				
				wstring    content;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct CData : public Node
			{
				
				wstring    content;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct Attribute : public Node
			{
				
				wstring    name;
wstring    value;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct Comment : public Node
			{
				
				wstring    content;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct Element : public Node
			{
				
				wstring    name;
wstring    closingName;
vector<shared_ptr<Attribute>>    attributes;
vector<shared_ptr<Node>>    subNodes;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct Instruction : public Node
			{
				
				wstring    name;
vector<shared_ptr<Attribute>>    attributes;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			


			
			struct Document : public Node
			{
				
				vector<shared_ptr<Node>>    prologs;
shared_ptr<Element>    rootElement;

				
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				
				
			};
			
				shared_ptr<ztl::general_parser::GeneralTableDefine> BootStrapDefineTable();
				
			shared_ptr<void> GeneralHeterogeneousParserTree(ztl::general_parser::GeneralParser& parser,ztl::general_parser::GeneralTreeNode* root);
			shared_ptr<void>	GeneralHeterogeneousParserTree(ztl::general_parser::GeneralParser& parser);
			
							}
						 
							}
						 