#pragma once
#include "Include/stdafx.h"
#include "Include/GeneralTableDefine.h"
#include "Include/SymbolManager.h"
#include "../Lib/ZTL/ztl_generator.hpp"
#include "Include/ParserSymbol.h"
#include "Include/GeneralParserFile.h"
namespace ztl
{
	namespace general_parser
	{
		class GeneralTypeDefineVisitor;
		class GeneralTypeObjectVisitor;
		class GeneralTypeDefineVisitor:public GeneralTypeDefine::IVisitor
		{
		public:
			GeneralTypeDefineVisitor() = delete;
			GeneralTypeDefineVisitor(SymbolManager* _manager) :manager(_manager)
			{

			}
			~GeneralTypeDefineVisitor() noexcept = default;
			GeneralTypeDefineVisitor(GeneralTypeDefineVisitor&&) = default;
			GeneralTypeDefineVisitor(const GeneralTypeDefineVisitor&) = default;
			GeneralTypeDefineVisitor& operator=(GeneralTypeDefineVisitor&&) = default;
			GeneralTypeDefineVisitor& operator=(const GeneralTypeDefineVisitor&) = default;
		public:
			virtual void								GeneralTypeDefineVisitor::Visit(GeneralClassTypeDefine* node) override;
			virtual void								GeneralTypeDefineVisitor::Visit(GeneralEnumTypeDefine* node) override;
			virtual void								GeneralTypeDefineVisitor::Visit(GeneralClassMemberTypeDefine* node) override;
			virtual void								GeneralTypeDefineVisitor::Visit(GeneralEnumMemberTypeDefine* node) override;
			wstring GetResult()const
			{
				return content;
			}
		public:
			wstring GetClassParentString(GeneralClassTypeDefine* node);
			wstring GetClassMembersString(GeneralClassTypeDefine* node);
			wstring GetClassSubTypesString(GeneralClassTypeDefine* node);
			wstring GetEnumBodyString(GeneralEnumTypeDefine* node);
		private:
			wstring GetVisitorBodyString(ParserSymbol* classSymbol)
			{
				wstring templateString = 
					L"virtual void		Visit($<Name>* node) =0;\n";
				auto deriveClassList = manager->GetCacheAllDeriveByBaseSymbol(classSymbol);
				return accumulate(deriveClassList.begin(), deriveClassList.end(), wstring(), 
					[&templateString](const wstring& sum, ParserSymbol* symbol)
				{
					generator::MarcoGenerator generator(templateString, { L"$<Name>" });
					return sum + generator.GenerateText({ symbol->GetName() }).GetMacroResult();
				});
			}
			wstring GetVisitorString(ParserSymbol* classSymbol)
			{
				wstring templateString = LR"(
				class IVisitor
				{
				public:
					$<VisitorBody>
				};
				virtual void									Accept(IVisitor* )
				{

				}
				)";
				auto bodyString = GetVisitorBodyString(classSymbol);
				generator::MarcoGenerator generator(templateString, {L"$<VisitorBody>"});
				return generator.GenerateText({bodyString}).GetMacroResult();
			}
			wstring GetForwardDeclarationString(ParserSymbol* classSymbol)
			{
				wstring templateString = L"struct $<ClassName>;\n";
				auto derives = manager->GetCacheAllDeriveByBaseSymbol(classSymbol);
				return accumulate(derives.begin(), derives.end(), wstring(), [&templateString](const wstring& sum,ParserSymbol* symbol)
				{
					generator::MarcoGenerator generator(templateString, { L"$<ClassName>" });
					return sum + generator.GenerateText({ symbol->GetName() }).GetMacroResult();
				});
			}
			wstring GetvirtualFunctionString()const
			{
				return	LR"(
				virtual void									Accept(IVisitor* visitor)override
				{
					visitor->Visit(this);
				}
				)";
			}
		private:
			SymbolManager* manager;
			wstring content;
		};
		wstring GetParentTypeString(SymbolManager* manager,GeneralTypeObject* parent)
		{
			auto parentSymbol = manager->GetCacheTypeObjectSymbol(parent);
			vector<wstring> parentList;
			while(parentSymbol != nullptr)
			{
				auto name = parentSymbol->GetName();
				if (!name.empty())
				{
					parentList.emplace_back(parentSymbol->GetName());
				}
				parentSymbol = parentSymbol->GetParentSymbol();
			}
			reverse(parentList.begin(), parentList.end());
			return  accumulate(parentList.begin(), parentList.end(), wstring(),
				[](const wstring& sum, const wstring& value)
			{
				return sum + value + L".";
			});

		}
		class GeneralTypeObjectVisitor:public GeneralTypeObject::IVisitor
		{
		public:
			GeneralTypeObjectVisitor() = delete;
			GeneralTypeObjectVisitor(SymbolManager* _manager) :manager(_manager)
			{

			}
			~GeneralTypeObjectVisitor() noexcept = default;
			GeneralTypeObjectVisitor(GeneralTypeObjectVisitor&&) = default;
			GeneralTypeObjectVisitor(const GeneralTypeObjectVisitor&) = default;
			GeneralTypeObjectVisitor& operator=(GeneralTypeObjectVisitor&&) = default;
			GeneralTypeObjectVisitor& operator=(const GeneralTypeObjectVisitor&) = default;
		public:

		public:
			virtual void								Visit(GeneralArrayTypeObject* node) override
			{
				GeneralTypeObjectVisitor visitor(manager);
				node->element->Accept(&visitor);
				wstring tempalteString = LR"(vector<$<Type>>)";
				generator::MarcoGenerator generator(tempalteString, { L"$<Type>" });
				content = generator.GenerateText({ visitor.GetResult() }).GetMacroResult();
			}
			virtual void								Visit(GeneralTokenTypeObject*) override
			{
				content = L"wstring";
			}
			virtual void								Visit(GeneralNormalTypeObject* node) override
			{
				auto typeDefSymbol = manager->GetCacheTypeObjectSymbol(node);
				wstring tempalteString = L"";
				if (typeDefSymbol->IsClassType())
				{
					tempalteString = LR"(shared_ptr<$<Type>>)";
				}
				else if(typeDefSymbol->IsEnumType())
				{
					tempalteString = LR"($<Type>)";
				}
				else
				{
					assert(false);
				}
				generator::MarcoGenerator generator(tempalteString, { L"$<Type>" });
				content = generator.GenerateText({ node->name }).GetMacroResult();
			}
			virtual void								Visit(GeneralSubTypeObject* node) override
			{
				auto name = node->name;
				auto parentString = GetParentTypeString(manager,node->parent.get());
				GeneralTypeObjectVisitor visitor(manager);
				node->parent->Accept(&visitor);
				content = parentString + L"." + node->name;
			}
		public:
			wstring GetResult()const
			{
				return content;
			}
		private:
			SymbolManager* manager;
			wstring content;
		};
		wstring GeneralTypeDefineVisitor::GetClassParentString(GeneralClassTypeDefine* node)
		{
			wstring parentString = L"";
			if(node->parent != nullptr)
			{
				
				auto classSymbol = manager->GetCacheSymbolByTypeDefine(node);
				assert(classSymbol != nullptr);
				assert(classSymbol->IsClassType());
			
				parentString = GetParentTypeString(manager,  node->parent.get());
				parentString.pop_back();
				parentString = L": public " + parentString;
			}
			return parentString;
		}
		wstring GeneralTypeDefineVisitor::GetClassMembersString(GeneralClassTypeDefine* node)
		{
			return accumulate(node->members.begin(), node->members.end(), wstring(),
				[this](const wstring& sum, const shared_ptr<GeneralClassMemberTypeDefine>& element)
			{
				GeneralTypeDefineVisitor visitor(manager);
				element->Accept(&visitor);
				return sum + visitor.GetResult();
			});
		}
		wstring GeneralTypeDefineVisitor::GetClassSubTypesString(GeneralClassTypeDefine* node)
		{
			return accumulate(node->subTypes.begin(), node->subTypes.end(), wstring(),
				[this](const wstring& sum, const shared_ptr<GeneralTypeDefine>& element)
			{
				GeneralTypeDefineVisitor visitor(manager);
				element->Accept(&visitor);
				return sum + visitor.GetResult();
			});
		}
		wstring GeneralTypeDefineVisitor::GetEnumBodyString(GeneralEnumTypeDefine* node)
		{
			return  accumulate(node->members.begin(), node->members.end(), wstring(),
				[this](const wstring& sum, const shared_ptr<GeneralEnumMemberTypeDefine>& element)
			{
				GeneralTypeDefineVisitor visitor(manager);
				element->Accept(&visitor);
				return sum + visitor.GetResult();
			});
		}
		 void								GeneralTypeDefineVisitor::Visit(GeneralClassTypeDefine* node) 
		{
			
			wstring templateString = 
			LR"(
			$<ForwardDeclaration>
			struct $<ClassName> $<Parent>
			{
				$<SubTypes>
				$<Members>
				$<VirtualFunction>
				$<Visitor>
			};
			)";
			wstring parentString = GetClassParentString(node);
			auto membersString = GetClassMembersString(node);
			auto subTypeString = GetClassSubTypesString(node);
		
			generator::MarcoGenerator generator(templateString,
			{
				L"$<ForwardDeclaration>",
				L"$<ClassName>",L"$<Parent>",
				L"$<SubTypes>",L"$<Members>",
				L"$<VirtualFunction>",L"$<Visitor>"
			});
			//这里逻辑是:class 有基类,那么就有accept函数 class有派生类,就有ivisitor接口类
			wstring virtualFucntionString = L"";
			wstring visitorString = L"";
			if (!parentString.empty())
			{
				virtualFucntionString = GetvirtualFunctionString();
			}
			auto classSymbol = manager->GetCacheSymbolByTypeDefine(node);
			wstring forwardDeclarationString = L"";
			assert(classSymbol->IsClassType());
			if(classSymbol->GetDescriptorSymbol()==nullptr&&!manager->GetCacheDeriveByBaseSymbol(classSymbol).empty())
			{
				forwardDeclarationString = GetForwardDeclarationString(classSymbol);
				visitorString = GetVisitorString(classSymbol);
			}
			content = generator.GenerateText({
			forwardDeclarationString,
			node->name ,parentString ,
			subTypeString ,membersString,
			virtualFucntionString,visitorString }).GetMacroResult();
		}
		 void								GeneralTypeDefineVisitor::Visit(GeneralEnumTypeDefine* node) 
		{
			auto templateString =
				LR"(
					enum class $<ClassName>
					{
						$<EnumBody>
					};
					)";
			auto enumClassName = node->name;
			auto bodyString = GetEnumBodyString(node);
			
			generator::MarcoGenerator generator(templateString, {L"$<ClassName>",L"$<EnumBody>"});
			content = generator.GenerateText({enumClassName,bodyString}).GetMacroResult();
		}
		 void								GeneralTypeDefineVisitor::Visit(GeneralClassMemberTypeDefine* node) 
		{
			GeneralTypeObjectVisitor visitor(manager);
			node->type->Accept(&visitor);
			auto typeString = visitor.GetResult();
			auto fieldNameString = node->name;
			auto templateString = L"$<Type>    $<FieldName>;\n";
			generator::MarcoGenerator generator(templateString, { L"$<Type>",L"$<FieldName>" });
			content = generator.GenerateText({ typeString,fieldNameString }).GetMacroResult();
		}
		 void								GeneralTypeDefineVisitor::Visit(GeneralEnumMemberTypeDefine* node) 
		{
			content = node->name + L",\n";
		}

		 wstring GetNodeDefineModule(GeneralTableDefine* table,SymbolManager*manager)
		 {
			return accumulate(table->types.begin(), table->types.end(), wstring(), [&manager](const wstring& sum, const shared_ptr<GeneralTypeDefine>&target)
			 {
				 GeneralTypeDefineVisitor visitor(manager);
				 target->Accept(&visitor);
				 return sum +L"\n\n"+ visitor.GetResult();
			 });
		 }
		
		
	}
}