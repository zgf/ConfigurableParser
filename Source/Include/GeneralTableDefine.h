#pragma once
#include "stdafx.h"
/*!
 * \file GeneralTableDefine.h
 *
 * \author ZGF
 * \date ʮһ�� 2014
 *��������ܵ�Ԥ��������
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
			TokenInfo() = default;
			TokenInfo(const wstring& _content,const wstring& _tag,int _position,int _length)
				:content(_content),tag(_tag), position(_position),length(_length)
			{
			
			}
			wstring GetTokenInfo()
			{
				return L" tag : " + tag + 
					L" content : " + content +
					L" position : " + to_wstring(position) + 
					L" length : " + to_wstring(length)+L"\n";
			}
		};
		/*
		ͷ��Ϣ����
		//include : name
		//classprefix : prefix
		//namespace : name
		//filename : name
		*/

		struct GeneralHeadInfoDefine
		{
			wstring property;
			wstring value;

		};
		/*
		token��IgnoreToken�Ķ���

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
			GeneralTokenDefine() = default;
			~GeneralTokenDefine() = default;
			GeneralTokenDefine(const wstring& name, const wstring& regex, const TokenOptional ignore)
				:name(name),regex(regex),ignore(ignore)
			{

			}
			GeneralTokenDefine(const wstring& name, const wstring& regex, const bool ignore)
				:name(name), regex(regex), ignore(ignore?TokenOptional::True:TokenOptional::False)
			{

			}
		};

		/*���Ͷ���ǰ������*/
		struct GeneralArrayTypeObject;
		struct GeneralStringTypeObject;
		struct GeneralNormalTypeObject;
		struct GeneralSubTypeObject;
		
		/*���Ͷ�����*/
		struct GeneralTypeObject
		{

			class IVisitor
			{
			public:
				virtual void								Visit(GeneralArrayTypeObject* node)=0;
				virtual void								Visit(GeneralStringTypeObject* node)=0;
				virtual void								Visit(GeneralNormalTypeObject* node)=0;
				virtual void								Visit(GeneralSubTypeObject* node)=0;
				
			};

			virtual void									Accept(IVisitor* visitor) = 0;
		};

		struct GeneralArrayTypeObject: public GeneralTypeObject
		{
			shared_ptr<GeneralTypeObject>			element;
			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};
		//��string���͵�ʹ��
		struct GeneralStringTypeObject: public GeneralTypeObject
		{
			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};
		//���Զ������͵�ʹ��
		struct GeneralNormalTypeObject: public GeneralTypeObject
		{
			wstring									name;
			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};
		//��Ƕ�׶������͵�ʹ�� ���� ����A�¶���������B     A.B cc;
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
		ǰ������
		*/
		struct GeneralTypeDefine;
		struct GeneralClassTypeDefine;
		struct GeneralEnumTypeDefine;
		struct GeneralClassMemberTypeDenfine;
		struct GeneralEnumMemberTypeDefine;
		/*
		���͵������Ķ���
		*/
		struct GeneralTypeDefine
		{

			class IVisitor
			{
			public:
				virtual void								Visit(GeneralClassTypeDefine* node)=0;
				virtual void								Visit(GeneralEnumTypeDefine* node)=0;
				virtual void								Visit(GeneralClassMemberTypeDenfine* node)=0;
				virtual void								Visit(GeneralEnumMemberTypeDefine* node)=0;

			};

			virtual void									Accept(IVisitor* visitor)=0;
			
		};
		struct GeneralClassMemberTypeDenfine: public GeneralTypeDefine
		{
			shared_ptr<GeneralTypeObject>			type;
			wstring									name;
			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};
		struct GeneralEnumMemberTypeDefine: public GeneralTypeDefine
		{
			wstring									name;
			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};
		struct GeneralClassTypeDefine: public GeneralTypeDefine
		{
			vector<shared_ptr<GeneralTypeDefine>>				subType;
			vector<shared_ptr<GeneralClassMemberTypeDenfine>>	members;
			shared_ptr<GeneralTypeObject>						parent;
			wstring												name;
			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};
		struct GeneralEnumTypeDefine: public GeneralTypeDefine
		{
			vector<shared_ptr<GeneralEnumMemberTypeDefine>>		members;
			wstring												name;
			virtual void									Accept(IVisitor* visitor)override
			{
				visitor->Visit(this);
			}
		};

		/*
		�ķ�����
		*/
		//�ķ�Ԫ��
		//�ս����
		struct GeneralGrammarTextTypeDefine;
		//���ս����
		struct GeneralGrammarNormalTypeDefine;
		//�ķ�����Ϊ

		//����
		struct GeneralGrammarSequenceTypeDefine;
		//ѭ��
		struct GeneralGrammarLoopTypeDefine;
		//��ѡ
		struct GeneralGrammarOptionalTypeDefine;
		//��ֵ
		struct GeneralGrammarAssignTypeDefine;
		//���ӵ�key value
		struct GeneralGrammarSetterTypeDefine;

		//Using����. ��ʽ����ת��
		struct GeneralGrammarUsingTypeDefine;
		//�����ڵ�.���𷵻�����
		struct GeneralGrammarCreateTypeDefine;
		//ѡ��
		struct GeneralGrammarAlterationTypeDefine;
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
				virtual void								Visit(GeneralGrammarAlterationTypeDefine* node)=0;
				virtual void								Visit(GeneralGrammarAssignTypeDefine* node)=0;


			};

			virtual void									Accept(IVisitor* visitor)=0;
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
		//ѭ��
		struct GeneralGrammarLoopTypeDefine: public GeneralGrammarTypeDefine
		{
			shared_ptr<GeneralGrammarTypeDefine>				grammar;
			void         Accept(IVisitor* visitor) override
			{
				visitor->Visit(this);
			}
		};
		//��ѡ
		struct GeneralGrammarOptionalTypeDefine: public GeneralGrammarTypeDefine
		{
			shared_ptr<GeneralGrammarTypeDefine>				grammar;
			void         Accept(IVisitor* visitor) override
			{
				visitor->Visit(this);
			}
		};
		//��ֵ
		struct GeneralGrammarAssignTypeDefine: public GeneralGrammarTypeDefine
		{
			wstring												name;
			shared_ptr<GeneralGrammarTypeDefine>				grammar;
			void         Accept(IVisitor* visitor) override
			{
				visitor->Visit(this);
			}
		};
		
		//���ӵ�key-value��
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
		//Using����. ��ʽ����ת��
		struct GeneralGrammarUsingTypeDefine: public GeneralGrammarTypeDefine
		{
			shared_ptr<GeneralGrammarTypeDefine>				grammar;
			void         Accept(IVisitor* visitor) override
			{
				visitor->Visit(this);
			}
		};
		//�����ڵ� ���𷵻�����
		struct GeneralGrammarCreateTypeDefine: public GeneralGrammarTypeDefine
		{
			shared_ptr<GeneralTypeObject>						type;
			shared_ptr<GeneralGrammarTypeDefine>				grammar;
			void         Accept(IVisitor* visitor) override
			{
				visitor->Visit(this);
			}
		};
		struct GeneralGrammarAlterationTypeDefine: public GeneralGrammarTypeDefine
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