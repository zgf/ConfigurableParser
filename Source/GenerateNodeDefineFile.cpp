#pragma once
#include "Include/stdafx.h"
#include "Include/GeneralTableDefine.h"
#include "Include/SymbolManager.h"
#include "../Lib/ZTL/ztl_exception.h"
#include "../Lib/ZTL/ztl_generator.hpp"
namespace ztl
{
	namespace general_parser
	{
		class GeneralTypeDefineVisitor;
		class GeneralTypeObjectVisitor;
		class GeneralTypeDefineVisitor:public GeneralTypeDefine::IVisitor
		{
		public:
			GeneralTypeDefineVisitor() = default;
			~GeneralTypeDefineVisitor() noexcept = default;
			GeneralTypeDefineVisitor(GeneralTypeDefineVisitor&&) = default;
			GeneralTypeDefineVisitor(const GeneralTypeDefineVisitor&) = default;
			GeneralTypeDefineVisitor& operator=(GeneralTypeDefineVisitor&&) = default;
			GeneralTypeDefineVisitor& operator=(const GeneralTypeDefineVisitor&) = default;
		public:
			virtual void								GeneralTypeDefineVisitor::Visit(GeneralClassTypeDefine* node) override;
			virtual void								GeneralTypeDefineVisitor::Visit(GeneralEnumTypeDefine* node) override;
			virtual void								GeneralTypeDefineVisitor::Visit(GeneralClassMemberTypeDenfine* node) override;
			virtual void								GeneralTypeDefineVisitor::Visit(GeneralEnumMemberTypeDefine* node) override;
			wstring GetResult()const
			{
				return content;
			}
		private:
			wstring content;
		};
		class GeneralTypeObjectVisitor:public GeneralTypeObject::IVisitor
		{
		public:
			GeneralTypeObjectVisitor() = default;
			~GeneralTypeObjectVisitor() noexcept = default;
			GeneralTypeObjectVisitor(GeneralTypeObjectVisitor&&) = default;
			GeneralTypeObjectVisitor(const GeneralTypeObjectVisitor&) = default;
			GeneralTypeObjectVisitor& operator=(GeneralTypeObjectVisitor&&) = default;
			GeneralTypeObjectVisitor& operator=(const GeneralTypeObjectVisitor&) = default;
		public:
			virtual void								Visit(GeneralArrayTypeObject* node) override
			{
				GeneralTypeObjectVisitor visitor;
				node->element->Accept(&visitor);
				content = L"vector<" + visitor.GetResult() + L">";
			}
			virtual void								Visit(GeneralStringTypeObject*) override
			{
				content = L"wstring";
			}
			virtual void								Visit(GeneralNormalTypeObject* node) override
			{
				content = node->name;
			}
			virtual void								Visit(GeneralSubTypeObject* node) override
			{
				GeneralTypeObjectVisitor visitor;
				node->parent->Accept(&visitor);
				content = GetResult() + L"." + node->name;
			}
		public:
			wstring GetResult()const
			{
				return content;
			}
		private:
			wstring content;
		};

		 void								GeneralTypeDefineVisitor::Visit(GeneralClassTypeDefine* node) 
		{
			wstring parentString = L"";
			if(node->parent != nullptr)
			{
				GeneralTypeObjectVisitor visitor;
				node->parent->Accept(&visitor);
				parentString = L": public " + visitor.GetResult() + L",\n";
			}
			auto membersString = accumulate(node->members.begin(), node->members.end(), wstring(),
				[](const wstring& sum, const shared_ptr<GeneralClassMemberTypeDenfine>& element)
			{
				GeneralTypeDefineVisitor visitor;
				element->Accept(&visitor);
				return sum + visitor.GetResult();
			});
			auto subTypeString = accumulate(node->subType.begin(), node->subType.end(), wstring(),
				[](const wstring& sum, const shared_ptr<GeneralTypeDefine>& element)
			{
				GeneralTypeDefineVisitor visitor;
				element->Accept(&visitor);
				return sum + visitor.GetResult();
			});
			content = L"class " + node->name + L" " + parentString + L"\n{\n" + membersString + subTypeString + L"\n};\n";
		}
		 void								GeneralTypeDefineVisitor::Visit(GeneralEnumTypeDefine* node) 
		{
			auto enumClassName = node->name;
			auto bodyString = accumulate(node->members.begin(), node->members.end(), wstring(),
				[](const wstring& sum, const shared_ptr<GeneralEnumMemberTypeDefine>& element)
			{
				GeneralTypeDefineVisitor visitor;
				element->Accept(&visitor);
				return sum + visitor.GetResult();
			});
			content = L"enum class\n{\n" + bodyString + L"\n};\n";
		}
		 void								GeneralTypeDefineVisitor::Visit(GeneralClassMemberTypeDenfine* node) 
		{
			GeneralTypeObjectVisitor visitor;
			node->type->Accept(&visitor);
			auto typeString = visitor.GetResult();
			auto fieldNameString = node->name;
			content = typeString + L" " + fieldNameString + L";\n";
		}
		 void								GeneralTypeDefineVisitor::Visit(GeneralEnumMemberTypeDefine* node) 
		{
			content = node->name + L",\n";
		}
		 wstring GetNodeDefineFileNamespace(SymbolManager* manager)
		 {
			 auto values = manager->GetCacheValueByProperty(L"namespace");
			 assert(values.size() == 0 || values.size() == 1);
			 return values.empty()?wstring() : values[0];
		 }
		 wstring GetNodeDefineFileInclude(SymbolManager* manager)
		 {
			 auto values = manager->GetCacheValueByProperty(L"include");
			 wstring includeString = L"";
			 if (!values.empty())
			 {
				 includeString = accumulate(values.begin(), values.end(), wstring(),[](const wstring& sum, const wstring value)
				 {
					 return sum + L"#include " + value + L"\n";
				 });

			 }
			 return includeString;
		 }
		 wstring GetNodeDefineFileBody(GeneralTableDefine* table)
		 {
			return  accumulate(table->types.begin(), table->types.end(), wstring(), [](const wstring& sum, const shared_ptr<GeneralTypeDefine>&target)
			 {
				 GeneralTypeDefineVisitor visitor;
				 target->Accept(&visitor);
				 return sum +L"\n\n"+ visitor.GetResult();
			 });
		 }
		 void CreateNodeDefineFile(wstring fileName,SymbolManager* manager)
		 {
			 wofstream output(fileName);
			 auto body = GetNodeDefineFileBody(manager->GetTable());
			 auto includeString = GetNodeDefineFileInclude(manager);
			 auto namespaceString = GetNodeDefineFileNamespace(manager);
			 if (!namespaceString.empty())
			 {
				 body = namespaceString + L"\n{\n" + body + L"\n};\n";
			 }
			 auto content = includeString +L"\n\n"+ body;
			 output.write(content.c_str(), content.size());
		 }
	}
}