#pragma once
#include "stdafx.h"
/*!
 * \file GeneralTableDefine.h
 *
 * \author ZGF
 * \date 十一月 2014
 *解析器框架的预定义类型
 *
 */
namespace ztl
{
	namespace general_parser
	{
		/*predefine*/
		struct TokenInfo
		{
			wstring content;
			wstring tag;
			int     position;
			int     length;
			int		lineNumber;
			TokenInfo() = default;
			TokenInfo(const wstring& _content,const wstring& _tag,int _position,int _length)
				:content(_content),tag(_tag), position(_position),length(_length), lineNumber(0)
			{
			
			}
			
			wstring GetTokenInfo()
			{
				return L" tag : " + tag + 
					L" content : " + content +
					L" position : " + to_wstring(position) + 
					L" length : " + to_wstring(length)+L"\n";
			}
			
			int GetLineNumber()const
			{
				return lineNumber;
			}

			void SetLineNumber(int value)
			{
				lineNumber = value;
			}
		};
		/*
		头信息定义
		//include : name
		//classprefix : prefix
		//namespace : name
		//filename : name
		*/
		struct GeneralAttributeDefine;
		struct GeneralAttributeArgumentDefine;
		struct GeneralHeadInfoDefine
		{
			wstring property;
			wstring value;
			vector<shared_ptr<GeneralAttributeDefine>>				attributes;
		};
		/*
		token和IgnoreToken的定义

		*/
		
		struct GeneralTokenDefine
		{
			enum class TokenOptional
			{
				False,
				True,
			};

			wstring name;
			wstring regex;
			TokenOptional	ignore;
			vector<shared_ptr<GeneralAttributeDefine>>				attributes;
			GeneralTokenDefine() = default;
			~GeneralTokenDefine() = default;
			GeneralTokenDefine(const wstring& name, const wstring& regex, const TokenOptional ignore)
				:name(name),regex(regex),ignore(ignore)
			{

			}
		};

		/*类型对象前置声明*/
		struct GeneralArrayTypeObject;
		struct GeneralTokenTypeObject;
		struct GeneralNormalTypeObject;
		struct GeneralSubTypeObject;
		
		/*类型对象定义*/
		struct GeneralTypeObject
		{

			class IVisitor
			{
			public:
				virtual void								Visit(GeneralArrayTypeObject* node)=0;
				virtual void								Visit(GeneralTokenTypeObject* node)=0;
				virtual void								Visit(GeneralNormalTypeObject* node)=0;
				virtual void								Visit(GeneralSubTypeObject* node)=0;
				
			};

			virtual void									Accept(IVisitor*)
			{

			}
		};

		struct GeneralArrayTypeObject: public GeneralTypeObject
		{
			shared_ptr<GeneralTypeObject>			element;
			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};
		//对string类型的使用
		struct GeneralTokenTypeObject: public GeneralTypeObject
		{
			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};
		//对自定义类型的使用
		struct GeneralNormalTypeObject: public GeneralTypeObject
		{
			wstring									name;
			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};
		//对嵌套定义类型的使用 例如 类型A下定义了类型B     A.B cc;
		struct GeneralSubTypeObject: public GeneralTypeObject
		{
			shared_ptr<GeneralTypeObject>			parent;
			wstring									name;
			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};
	

		/*
		前置声明
		*/
		struct GeneralTypeDefine;
		struct GeneralClassTypeDefine;
		struct GeneralEnumTypeDefine;
		struct GeneralClassMemberTypeDefine;
		struct GeneralEnumMemberTypeDefine;
		struct GeneralAttributeDefine;
		/*
		类型的声明的定义
		*/
		struct GeneralTypeDefine
		{

			class IVisitor
			{
			public:
				virtual void								Visit(GeneralClassTypeDefine* node)=0;
				virtual void								Visit(GeneralEnumTypeDefine* node)=0;
				virtual void								Visit(GeneralClassMemberTypeDefine* node)=0;
				virtual void								Visit(GeneralEnumMemberTypeDefine* node)=0;
				
			};

			virtual void									Accept(IVisitor*)
			{

			}
			
		};
		struct GeneralClassMemberTypeDefine: public GeneralTypeDefine
		{
			shared_ptr<GeneralTypeObject>			type;
			wstring									name;
			vector<shared_ptr<GeneralAttributeDefine>>				attributes;
			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};
		struct GeneralEnumMemberTypeDefine: public GeneralTypeDefine
		{
			wstring									name;
			vector<shared_ptr<GeneralAttributeDefine>>				attributes;
			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};
		struct GeneralAttributeArgumentDefine
		{
			wstring									name;
		};
		struct GeneralClassTypeDefine: public GeneralTypeDefine
		{
			vector<shared_ptr<GeneralTypeDefine>>				subTypes;
			vector<shared_ptr<GeneralClassMemberTypeDefine>>	members;
			shared_ptr<GeneralTypeObject>						parent;
			wstring												name;
			vector<shared_ptr<GeneralAttributeDefine>>				attributes;
			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};
		struct GeneralEnumTypeDefine: public GeneralTypeDefine
		{
			vector<shared_ptr<GeneralEnumMemberTypeDefine>>		members;
			wstring												name;
			vector<shared_ptr<GeneralAttributeDefine>>				attributes;
			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};

		struct GeneralAttributeDefine 
		{
			vector<shared_ptr<GeneralAttributeArgumentDefine>>	arguments;
			wstring												name;
		};
		/*
		文法定义
		*/
		//文法元素
		//终结符号
		struct GeneralGrammarTextTypeDefine;
		//非终结符号
		struct GeneralGrammarNormalTypeDefine;
		//文法的行为

		//序列
		struct GeneralGrammarSequenceTypeDefine;
		//循环
		struct GeneralGrammarLoopTypeDefine;
		//可选
		struct GeneralGrammarOptionalTypeDefine;
		//赋值
		struct GeneralGrammarAssignTypeDefine;
		//附加的key value
		struct GeneralGrammarSetterTypeDefine;

		//Using操作. 隐式类型转换
		struct GeneralGrammarUsingTypeDefine;
		//创建节点.区别返回类型
		struct GeneralGrammarCreateTypeDefine;
		//选择
		struct GeneralGrammarAlternationTypeDefine;
		struct GeneralGrammarTypeDefine
		{
			class IVisitor
			{
			public:
				virtual void								Visit(GeneralGrammarTextTypeDefine* node)=0;
				virtual void								Visit(GeneralGrammarNormalTypeDefine* node)=0;
				virtual void								Visit(GeneralGrammarSequenceTypeDefine* node)=0;
				virtual void								Visit(GeneralGrammarLoopTypeDefine* node)=0;
				virtual void								Visit(GeneralGrammarOptionalTypeDefine* node)=0;
				virtual void								Visit(GeneralGrammarSetterTypeDefine* node)=0;
				virtual void								Visit(GeneralGrammarUsingTypeDefine* node)=0;
				virtual void								Visit(GeneralGrammarCreateTypeDefine* node)=0;
				virtual void								Visit(GeneralGrammarAlternationTypeDefine* node)=0;
				virtual void								Visit(GeneralGrammarAssignTypeDefine* node)=0;


			};

			virtual void									Accept(IVisitor*)
			{

			}
		};
		struct GeneralGrammarTextTypeDefine: public GeneralGrammarTypeDefine
		{
			wstring												text;
			void									Accept(IVisitor* visitor) override
			{
				visitor->Visit(this);
			}
		};
		struct GeneralGrammarNormalTypeDefine: public GeneralGrammarTypeDefine
		{
			wstring												name;
			void									Accept(IVisitor* visitor) override
			{
				visitor->Visit(this);
			}
		};
		struct GeneralGrammarSequenceTypeDefine: public GeneralGrammarTypeDefine
		{
			shared_ptr<GeneralGrammarTypeDefine>				first;
			shared_ptr<GeneralGrammarTypeDefine>				second;
			void									Accept(IVisitor* visitor) override
			{
				visitor->Visit(this);
			}
		};
		//循环
		struct GeneralGrammarLoopTypeDefine: public GeneralGrammarTypeDefine
		{
			shared_ptr<GeneralGrammarTypeDefine>				grammar;
			void         Accept(IVisitor* visitor) override
			{
				visitor->Visit(this);
			}
		};
		//可选
		struct GeneralGrammarOptionalTypeDefine: public GeneralGrammarTypeDefine
		{
			shared_ptr<GeneralGrammarTypeDefine>				grammar;
			void         Accept(IVisitor* visitor) override
			{
				visitor->Visit(this);
			}
		};
		//赋值
		struct GeneralGrammarAssignTypeDefine: public GeneralGrammarTypeDefine
		{
			wstring												name;
			shared_ptr<GeneralGrammarTypeDefine>				grammar;
			void         Accept(IVisitor* visitor) override
			{
				visitor->Visit(this);
			}
		};
		
		//附加的key-value对
		struct GeneralGrammarSetterTypeDefine: public GeneralGrammarTypeDefine
		{
			shared_ptr<GeneralGrammarTypeDefine>				grammar;
			wstring												name;
			wstring												value;
			void         Accept(IVisitor* visitor) override
			{
				visitor->Visit(this);
			}
		};
		//Using操作. 隐式类型转换
		struct GeneralGrammarUsingTypeDefine: public GeneralGrammarTypeDefine
		{
			shared_ptr<GeneralGrammarTypeDefine>				grammar;
			void         Accept(IVisitor* visitor) override
			{
				visitor->Visit(this);
			}
		};
		//创建节点 区别返回类型
		struct GeneralGrammarCreateTypeDefine: public GeneralGrammarTypeDefine
		{
			shared_ptr<GeneralTypeObject>						type;
			shared_ptr<GeneralGrammarTypeDefine>				grammar;
			void         Accept(IVisitor* visitor) override
			{
				visitor->Visit(this);
			}
		};
		struct GeneralGrammarAlternationTypeDefine: public GeneralGrammarTypeDefine
		{
			shared_ptr<GeneralGrammarTypeDefine>				left;
			shared_ptr<GeneralGrammarTypeDefine>				right;
			void         Accept(IVisitor* visitor) override
			{
				visitor->Visit(this);
			}
		};

		struct GeneralRuleDefine
		{
			shared_ptr<GeneralTypeObject>						type;
			wstring												name;
			vector<shared_ptr<GeneralGrammarTypeDefine>>		grammars;
			vector<shared_ptr<GeneralAttributeDefine>>				attributes;
		};

		struct GeneralTableDefine
		{
			vector<shared_ptr<GeneralHeadInfoDefine>> heads;
			vector<shared_ptr<GeneralTypeDefine>>   types;
			vector<shared_ptr<GeneralTokenDefine>>  tokens;
			vector<shared_ptr<GeneralRuleDefine>>   rules;
		};
		shared_ptr<GeneralTableDefine> BootStrapDefineTable();
		

		
	}
}