#include "Include/stdafx.h"
#include "Include/GeneralTableDefine.h"
#include "Include/SymbolManager.h"
#include "../Lib/ZTL/ztl_generator.hpp"
#include "Include/ParserSymbol.h"
#include "Include/GeneralParserFileGenerator.h"
namespace ztl
{
	namespace general_parser
	{
		/*class IRCodeGenContext
		{

		};
		template<typename BaseClassType>
		class IRCodeGen : public BaseClassType::IVisitor
		{
			static_assert(false, "must have BaseClassType::IVisitor");
			IRCodeGenContext* context;
		};*/
		class GeneralTypeDefineVisitor;
		class GeneralTypeObjectVisitor;
		class GeneralIRTypeDefineVisitor :public GeneralTypeDefine::IVisitor
		{
		public:
			GeneralIRTypeDefineVisitor() = delete;
			GeneralIRTypeDefineVisitor(SymbolManager* _manager) :manager(_manager)
			{

			}
			~GeneralIRTypeDefineVisitor() noexcept = default;
			GeneralIRTypeDefineVisitor(GeneralIRTypeDefineVisitor&&) = default;
			GeneralIRTypeDefineVisitor(const GeneralIRTypeDefineVisitor&) = default;
			GeneralIRTypeDefineVisitor& operator=(GeneralIRTypeDefineVisitor&&) = default;
			GeneralIRTypeDefineVisitor& operator=(const GeneralIRTypeDefineVisitor&) = default;
		public:
			template<typename... ArgTypes>
			wstring GetSpanListString(const vector<ParserSymbol*>& deriveClassList, const wstring& templateString, ArgTypes&&... param)
			{
				//这里要基于模版元的LINQ LINQ<ArgTypes...>.All(x=>pred);
				return accumulate(deriveClassList.begin(), deriveClassList.end(), wstring(),
					[&templateString,&param...](const wstring& sum, ParserSymbol* symbol)
				{
					generator::MarcoGenerator generator(templateString, { param... });
					return sum + generator.GenerateText({ symbol->GetName() }).GetMacroResult();
				});
			}
			virtual void								GeneralIRTypeDefineVisitor::Visit(GeneralClassTypeDefine* node) override
			{
				auto classSymbol = manager->GetCacheSymbolByTypeDefine(node);
				if (node->parent == nullptr&& 
					!manager->GetCacheDeriveByBaseSymbol(classSymbol).empty())
				{
					//说明是基类
					wstring templateString =
						LR"(
								$<ForwardList>
								template<>
								class IRCodeGen<$<BaseClassType>> : public $<BaseClassType>::IVisitor
								{
								public:
									IRCodeGen(IRCodeGenContext* _context):context(_context)
									{

									}

									static IRCodeGen<AstExpression> CodeGen(AstExpression* node, IRCodeGenContext* context)
									{
										IRCodeGen<AstExpression> object(context);
										object.Visit(node);
										return object;
									}
								public:
									$<VirtualFunctions>	
								public:
									Value* GetValue()
									{
										return value;
									}
									IRCodeGenContext* Context()
									{
										return context;
									}
								private:
									Value* value;
									IRCodeGenContext* context;
								};

						)";
					generator::MarcoGenerator generator(templateString, { L"$<ForwardList>",L"$<BaseClassType>" ,L"$<VirtualFunctions>"});
					auto deriveClassList = manager->GetCacheAllDeriveByBaseSymbol(classSymbol);
					wstring virtualFunctionTempalte = LR"(virtual void		Visit($<Name>* ) override
														{
							
														}
														)";
					wstring forwardTemplate = L"class $<Name>;\n";
					auto forwardListString = GetSpanListString(deriveClassList, forwardTemplate, L"$<Name>");
					auto virtualFunctionsString = GetSpanListString(deriveClassList, virtualFunctionTempalte,L"$<Name>");
					content = generator.GenerateText({ forwardListString,classSymbol->GetName(),virtualFunctionsString }).GetMacroResult();
				}
			}
		
			virtual void								GeneralIRTypeDefineVisitor::Visit(GeneralClassMemberTypeDefine* ) override
			{

			}
			virtual void								GeneralIRTypeDefineVisitor::Visit(GeneralEnumMemberTypeDefine* ) override
			{

			}
			virtual void								GeneralIRTypeDefineVisitor::Visit(GeneralEnumTypeDefine*) override
			{

			}
			wstring GetResult()const
			{
				return content;
			}
	
		private:
			SymbolManager* manager;
			wstring content;
		};
		wstring GetIRCodeGenModuleHead()
		{
			auto templateString = 
				LR"(
					struct IRCodeGenContext
					{

					};
					template<typename BaseClassType>
					class IRCodeGen : public BaseClassType::IVisitor
					{
						IRCodeGenContext* context;
					};
				)";
			return templateString;
		}
		wstring GetIRCodeGenModuleBody(GeneralTableDefine* table, SymbolManager*manager)
		{
			return accumulate(table->types.begin(), table->types.end(), wstring(), [&manager](const wstring& sum, const shared_ptr<GeneralTypeDefine>&target)
			{
				GeneralIRTypeDefineVisitor visitor(manager);
				target->Accept(&visitor);
				return sum + L"\n\n" + visitor.GetResult();
			});
		}
		wstring GetIRCodeGenModule(GeneralTableDefine* table, SymbolManager*manager)
		{
			auto head = GetIRCodeGenModuleHead();
			auto body = GetIRCodeGenModuleBody(table, manager);
			return head + body;
		}
	}
}