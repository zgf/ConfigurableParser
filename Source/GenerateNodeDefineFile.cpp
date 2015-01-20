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
		/*template<typename ReturnType,typename ParamType>
		class ITypeDefineVisitor:public GeneralTypeDefine::IVisitor
		{
		private:
			ReturnType returnValue;
			ParamType argument;
		public:
			virtual ReturnType								Apply(shared_ptr<GeneralTypeDefine> expression, ParamType param);
			virtual ReturnType								Apply(shared_ptr<GeneralClassTypeDefine> expression, ParamType param);
			virtual ReturnType								Apply(shared_ptr<GeneralEnumTypeDefine> expression, ParamType param);
			virtual ReturnType								Apply(shared_ptr<GeneralClassMemberTypeDenfine> expression, ParamType param);
			virtual ReturnType								Apply(shared_ptr<GeneralEnumMemberTypeDefine> expression, ParamType param);

		public:
			ReturnType Invoke(shared_ptr<GeneralTypeDefine> expression, ParamType param)
			{
				argument = param;
				expression->Accept(shared_from_this());
				return returnValue;
			}
			virtual void								Visit(shared_ptr<GeneralTypeDefine> expression)override
			{
				assert(false);
			}
			virtual void								Visit(shared_ptr<GeneralClassTypeDefine> expression)override
			{
				returnValue = this->Apply(expression, argument);
			}
			virtual void								Visit(shared_ptr<GeneralEnumTypeDefine> expression)override
			{
				returnValue = this->Apply(expression, argument);
			}
			virtual void								Visit(shared_ptr<GeneralClassMemberTypeDenfine> expression) override
			{
				returnValue = this->Apply(expression, argument);

			}
			virtual void								Visit(shared_ptr<GeneralEnumMemberTypeDefine> expression) override
			{
				returnValue = this->Apply(expression, argument);
			}
		};
		template<typename ParamType>
		class ITypeDefineVisitor<void,ParamType>:public GeneralTypeDefine::IVisitor
		{
		private:
			ParamType argument;
			using ReturnType = void;
		public:
			virtual ReturnType								Apply(shared_ptr<GeneralTypeDefine> expression, ParamType param);
			virtual ReturnType								Apply(shared_ptr<GeneralClassTypeDefine> expression, ParamType param);
			virtual ReturnType								Apply(shared_ptr<GeneralEnumTypeDefine> expression, ParamType param);
			virtual ReturnType								Apply(shared_ptr<GeneralClassMemberTypeDenfine> expression, ParamType param);
			virtual ReturnType								Apply(shared_ptr<GeneralEnumMemberTypeDefine> expression, ParamType param);

		public:
			ReturnType Invoke(shared_ptr<GeneralTypeDefine> expression, ParamType param)
			{
				argument = param;
				expression->Accept(shared_from_this());
			}
			virtual void								Visit(shared_ptr<GeneralTypeDefine> expression)override
			{
				assert(false);
			}
			virtual void								Visit(shared_ptr<GeneralClassTypeDefine> expression)override
			{
				this->Apply(expression, argument);
			}
			virtual void								Visit(shared_ptr<GeneralEnumTypeDefine> expression)override
			{
				this->Apply(expression, argument);
			}
			virtual void								Visit(shared_ptr<GeneralClassMemberTypeDenfine> expression) override
			{
				this->Apply(expression, argument);

			}
			virtual void								Visit(shared_ptr<GeneralEnumMemberTypeDefine> expression) override
			{
				this->Apply(expression, argument);
			}
		};

		

		template<typename ReturnType, typename ParamType>
		class ITypeObjectVisitor:public GeneralTypeObject::IVisitor
		{
		private:
			ReturnType returnValue;
			ParamType argument;
		public:
			virtual ReturnType								Apply(shared_ptr<GeneralTypeObject> expression, ParamType param);
			virtual ReturnType								Apply(shared_ptr<GeneralArrayTypeObject> expression, ParamType param);
			virtual ReturnType								Apply(shared_ptr<GeneralStringTypeObject> expression, ParamType param);
			virtual ReturnType								Apply(shared_ptr<GeneralNormalTypeObject> expression, ParamType param);
			virtual ReturnType								Apply(shared_ptr<GeneralSubTypeObject> expression, ParamType param);
			

		public:
			ReturnType Invoke(shared_ptr<GeneralTypeObject> expression, ParamType param)
			{
				argument = param;
				expression->Accept(shared_from_this());
				return returnValue;
			}
			
			virtual void								Visit(shared_ptr<GeneralTypeObject> expression) override
			{
				assert(false);
			}
			virtual void								Visit(shared_ptr<GeneralArrayTypeObject> expression) override
			{
				returnValue = this->Apply(expression, argument);
			}
			virtual void								Visit(shared_ptr<GeneralStringTypeObject> expression) override
			{
				returnValue = this->Apply(expression, argument);

			}
			virtual void								Visit(shared_ptr<GeneralNormalTypeObject> expression) override
			{
				returnValue = this->Apply(expression, argument);

			}
			virtual void								Visit(shared_ptr<GeneralSubTypeObject> expression) override
			{
				returnValue = this->Apply(expression, argument);

			}
		
		};
		template<typename ParamType>
		class ITypeObjectVisitor<void, ParamType>:public GeneralTypeObject::IVisitor
		{
		private:
			using ReturnType = void;
			ParamType argument;
		public:
			virtual ReturnType								Apply(shared_ptr<GeneralTypeObject> expression, ParamType param);
			virtual ReturnType								Apply(shared_ptr<GeneralArrayTypeObject> expression, ParamType param);
			virtual ReturnType								Apply(shared_ptr<GeneralStringTypeObject> expression, ParamType param);
			virtual ReturnType								Apply(shared_ptr<GeneralNormalTypeObject> expression, ParamType param);
			virtual ReturnType								Apply(shared_ptr<GeneralSubTypeObject> expression, ParamType param);
			
		public:
			ReturnType Invoke(shared_ptr<GeneralTypeObject> expression, ParamType param)
			{
				argument = param;
				expression->Accept(shared_from_this());
			}

			virtual void								Visit(shared_ptr<GeneralTypeObject> expression) override
			{
				assert(false);
			}
			virtual void								Visit(shared_ptr<GeneralArrayTypeObject> expression) override
			{
				this->Apply(expression, argument);
			}
			virtual void								Visit(shared_ptr<GeneralStringTypeObject> expression) override
			{
				this->Apply(expression, argument);

			}
			virtual void								Visit(shared_ptr<GeneralNormalTypeObject> expression) override
			{
				this->Apply(expression, argument);

			}
			virtual void								Visit(shared_ptr<GeneralSubTypeObject> expression) override
			{
				this->Apply(expression, argument);

			}
			
		};

		class NodeDefineFileGenrator
		{
		public:
			class TypeDefineGenerator: public ITypeDefineVisitor<void, shared_ptr<wstring>>
			{
				using ParamType = shared_ptr<wstring>;
				using ReturnType = void;
			public:
				virtual ReturnType							Apply(shared_ptr<GeneralTypeDefine> expression, ParamType param)override
				{
					assert(false);
				}
				virtual ReturnType							Apply(shared_ptr<GeneralClassTypeDefine> expression, ParamType param)override
				{

					auto& class_name = expression->name;

					auto&& parent_name = make_shared<wstring>();
					TypeObjectGenerator().Invoke(expression->parent, parent_name);

					wstring class_field_list;
					for (auto&&iter:expression->members)
					{
						auto&& filed = make_shared<wstring>();
						TypeDefineGenerator().Invoke(iter, filed);
						class_field_list.append(*filed);
					}
					
					wstring subtype_define_list;
					for (auto&&iter:expression->subType)
					{
						auto&& subtype = make_shared<wstring>();
						Invoke(iter, subtype);
						subtype_define_list.append(*subtype);
					}
					ztl::generator::MarcoGenerator class_define(LR"(
					class CLASSNAME : public PARENTNAME
					{
						SUBTYPELIST
						CLASSMEMBERLIST
					};)", {L"CLASSNAME",L"PARENTNAME",L"SUBTYPELIST",L"CLASSMEMBERLIST"});
					*param = class_define.GenerateText({ class_name,*parent_name,subtype_define_list,class_field_list }).GetMacroResult();
				}
				virtual ReturnType							Apply(shared_ptr<GeneralEnumTypeDefine> expression, ParamType param)override
				{

				}
				virtual ReturnType								Apply(shared_ptr<GeneralClassMemberTypeDenfine> expression, ParamType param)override
				{

				}
				virtual ReturnType								Apply(shared_ptr<GeneralEnumMemberTypeDefine> expression, ParamType param)override
				{

				}
			};
		public:

			class TypeObjectGenerator: public ITypeObjectVisitor<void, shared_ptr<wstring>>
			{
				using ParamType = shared_ptr<wstring>;
				using ReturnType = void;
			public:
				virtual ReturnType								Apply(shared_ptr<GeneralTypeObject> expression, ParamType param)override
				{
					assert(false);
				}
				virtual ReturnType								Apply(shared_ptr<GeneralArrayTypeObject> expression, ParamType param)override
				{

				}
				virtual ReturnType								Apply(shared_ptr<GeneralStringTypeObject> expression, ParamType param)override
				{

				}
				virtual ReturnType								Apply(shared_ptr<GeneralNormalTypeObject> expression, ParamType param)override
				{

				}
				virtual ReturnType								Apply(shared_ptr<GeneralSubTypeObject> expression, ParamType param)override
				{

				}
			
			};
		private:
			shared_ptr<SymbolManager>		manager;
			wstring							fileName;
		public:
			wstring CreatNodeDefineContent()
			{
				wstring result;
				for (auto&& iter :manager->GetTypes())
				{
					auto&& type_define_string = make_shared<wstring>();
					TypeDefineGenerator().Invoke(iter, type_define_string);
					result.append(*type_define_string);
				}
				return result;
			}
			void CreatNodeDefineFile()
			{
				wofstream output(this->fileName);
				ztl::contract::ThrowException(output.is_open(), "Can't open the file output!");
				auto content = CreatNodeDefineContent();
				output.write(content.c_str(), content.size());
			}
		};
*/
	}
}


/* Ä£°å
template<typename ReturnType,typename ParamType>
class ITypeDefineVisitor:public GeneralTypeDefine::IVisitor
{
private:
ReturnType returnValue;
ParamType argument;
public:
virtual ReturnType								Apply(shared_ptr<GeneralTypeDefine> expression, ParamType param);
virtual ReturnType								Apply(shared_ptr<GeneralClassTypeDefine> expression, ParamType param);
virtual ReturnType								Apply(shared_ptr<GeneralEnumTypeDefine> expression, ParamType param);
public:
ReturnType Invoke(shared_ptr<GeneralTypeDefine> expression, ParamType param)
{
argument = param;
expression->Accept(shared_from_this());
return returnValue;
}
virtual void								Visit(shared_ptr<GeneralTypeDefine> expression)override
{
assert(false);
}
virtual void								Visit(shared_ptr<GeneralClassTypeDefine> expression)override
{
returnValue = this->Apply(expression, argument);
}
virtual void								Visit(shared_ptr<GeneralEnumTypeDefine> expression)override
{
returnValue = this->Apply(expression, argument);
}

};

class TypeDefineGenerator: public ITypeDefineVisitor<wstring,shared_ptr<SymbolManager>>
{
using ParamType = shared_ptr<SymbolManager>;
using ReturnType = wstring;
wstring defineString;
public:
virtual ReturnType							Apply(shared_ptr<GeneralTypeDefine> expression, ParamType manager)override
{
assert(false);
}
virtual ReturnType							Apply(shared_ptr<GeneralClassTypeDefine> expression, ParamType manager)override
{

}
virtual ReturnType							Apply(shared_ptr<GeneralEnumTypeDefine> expression, ParamType manager)override
{

}
};

template<typename ReturnType, typename ParamType>
class ITypeObjectVisitor:public GeneralTypeObject::IVisitor
{
private:
ReturnType returnValue;
ParamType argument;
public:
virtual ReturnType								Apply(shared_ptr<GeneralTypeObject> expression, ParamType param);
virtual ReturnType								Apply(shared_ptr<GeneralArrayTypeObject> expression, ParamType param);
virtual ReturnType								Apply(shared_ptr<GeneralStringTypeObject> expression, ParamType param);
virtual ReturnType								Apply(shared_ptr<GeneralNormalTypeObject> expression, ParamType param);
virtual ReturnType								Apply(shared_ptr<GeneralSubTypeObject> expression, ParamType param);
virtual ReturnType								Apply(shared_ptr<GeneralClassMemberTypeObject> expression, ParamType param);
virtual ReturnType								Apply(shared_ptr<GeneralEnumMemberTypeObject> expression, ParamType param);

public:
ReturnType Invoke(shared_ptr<GeneralTypeObject> expression, ParamType param)
{
argument = param;
expression->Accept(shared_from_this());
return returnValue;
}

virtual void								Visit(shared_ptr<GeneralTypeObject> expression) override
{
assert(false);
}
virtual void								Visit(shared_ptr<GeneralArrayTypeObject> expression) override
{
returnValue = this->Apply(expression, argument);
}
virtual void								Visit(shared_ptr<GeneralStringTypeObject> expression) override
{
returnValue = this->Apply(expression, argument);

}
virtual void								Visit(shared_ptr<GeneralNormalTypeObject> expression) override
{
returnValue = this->Apply(expression, argument);

}
virtual void								Visit(shared_ptr<GeneralSubTypeObject> expression) override
{
returnValue = this->Apply(expression, argument);

}
virtual void								Visit(shared_ptr<GeneralClassMemberTypeObject> expression) override
{
returnValue = this->Apply(expression, argument);

}
virtual void								Visit(shared_ptr<GeneralEnumMemberTypeObject> expression) override
{
returnValue = this->Apply(expression, argument);
}
};


class TypeObjectGenerator: public ITypeObjectVisitor<wstring,shared_ptr<SymbolManager>>
{
using ParamType = shared_ptr<SymbolManager>;
using ReturnType = wstring;
wstring objectString;
public:
virtual ReturnType								Apply(shared_ptr<GeneralTypeObject> expression, ParamType param)override
{
assert(false);
}
virtual ReturnType								Apply(shared_ptr<GeneralArrayTypeObject> expression, ParamType param)override
{

}
virtual ReturnType								Apply(shared_ptr<GeneralStringTypeObject> expression, ParamType param)override
{

}
virtual ReturnType								Apply(shared_ptr<GeneralNormalTypeObject> expression, ParamType param)override
{

}
virtual ReturnType								Apply(shared_ptr<GeneralSubTypeObject> expression, ParamType param)override
{

}
virtual ReturnType								Apply(shared_ptr<GeneralClassMemberTypeObject> expression, ParamType param)override
{

}
virtual ReturnType								Apply(shared_ptr<GeneralEnumMemberTypeObject> expression, ParamType param)override
{

}
};

*/