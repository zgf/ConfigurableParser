#pragma once
#include "Include/stdafx.h"
#include "Include/SymbolManager.h"
#include "Include/GeneralTableDefine.h"
#include "Include/GeneralFile.h"
#include "Include/ParserSymbol.h"
#include "../Lib/ZTL/ztl_generator.hpp"
#include "Include/GeneralTreeNode.h"
namespace ztl
{
	namespace general_parser
	{
		wstring GetReflectionFiledString(ParserSymbol* classSymbol, ParserSymbol*filedDefSymbol)
		{
			wstring filedReflectString;
			wstring classType = classSymbol->GetName();
			wstring fieldName = filedDefSymbol->GetName();
			assert(filedDefSymbol->GetDescriptorSymbol() != nullptr);
			if(filedDefSymbol->GetDescriptorSymbol()->IsArrayType())
			{
				auto arrayTypeSymbol = filedDefSymbol->GetDescriptorSymbol();
				wstring arrayFieldReflectTemplate = LR"(
									{
										L"$<FieldName>",
										[](void* classObject,void* valueObject)
										{
											static_cast<$<ClassType>>(classObject)->$<FieldName>.emplace_back(static_cast<$<ElementType>>(valueObject));
											return;
										}
									},)";
				ztl::generator::MarcoGenerator generator(arrayFieldReflectTemplate, { L"$<ClassType>",L"$<FieldName>",L"$<ElementType>" });
				wstring elementType = arrayTypeSymbol->GetDescriptorSymbol()->GetName();
				return generator.GenerateText({ classType, fieldName, elementType }).GetMacroResult();
			}
			else
			{
				wstring normalFieldReflectTempalte = LR"(
									{
										L"$<FieldName>",
										[](void* classObject,void* valueObject)
										{
											static_cast<$<ClassType>>(classObject)->$<FieldName>=static_cast<$<FieldType>>(valueObject);
											return;
										}
									},)";
				auto fieldType = filedDefSymbol->GetDescriptorSymbol()->GetName();
				ztl::generator::MarcoGenerator generator(normalFieldReflectTempalte, { L"$<ClassType>",L"$<FieldName>",L"$<FieldType>" });
				return generator.GenerateText({ classType,fieldName,fieldType }).GetMacroResult();

			}
		}
		wstring GetClassReflectionString(ParserSymbol* classSymbol)
		{
			wstring tempalteString = LR"({
								L"$<ClassTypeName>",
								{
									$<FieldReflectList>
								}
							},)";
			ztl::generator::MarcoGenerator generator(tempalteString, { L"$<ClassTypeName>",L"$<FieldReflectList>" });
			wstring FieldReflectList;
			for(auto&&iter : classSymbol->GetSubSymbolMap())
			{
				if(iter.second->IsFieldDef())
				{
					FieldReflectList += GetReflectionFiledString(classSymbol, iter.second);
				}
			}
			return generator.GenerateText({ classSymbol->GetName() ,FieldReflectList }).GetMacroResult();
		}
		wstring GetReflectBuilderFunctionString(SymbolManager* manager)
		{
			wstring tempalteString = LR"(
			void ReflectionBuidler(const wstring& className,const wstring& fieldName,void* classObject,void* valueObject)
			{
				using builderType = void(*)(void*,void*);
				static unordered_map<wstring, unordered_map<wstring, builderType>> builderMap =
				{
					$<ClassReflecteList>
				};
				auto findIter = builderMap.find(className);
				assert(findIter!=builderMap.end());
				assert(findIter->second.find(fieldName)!=findIter->second.end());
				builderMap[className][fieldName](classObject,valueObject);
			})";
			ztl::generator::MarcoGenerator generator(tempalteString, { L"$<ClassReflecteList>" });
			wstring ClassReflecteList;
			auto global = manager->GetGlobalSymbol();
			for(auto&& iter : global->GetSubSymbolMap())
			{
				if(iter.second->IsClassType())
				{
					ClassReflecteList += GetClassReflectionString(iter.second);
				}
			}
			return generator.GenerateText({ ClassReflecteList }).GetMacroResult();
		}
		void GetHeterogeneousParseFunction()
		{
			wstring tempalteString = LR"(shared_ptr<void> )";

		}
		wstring GetObjectReflectionListString(const wstring& className)
		{
			wstring classReflecteString = LR"(
				{
					L"$<ClassName>",
					[]()->shared_ptr<void>
					{
						return make_shared<$<ClassName>>();
					}
				},)";
			ztl::generator::MarcoGenerator generator(classReflecteString, { L"$<ClassName>" });
			return generator.GenerateText({ className }).GetMacroResult();
		}
		wstring GetOjectReflectioFunctionString(SymbolManager* manager)
		{
			wstring templateString = LR"(
			shared_ptr<void> ReflecteObjectByName(const wstring& name)
			{
				using reflectObjectType = shared_ptr<void>(*)();
				static unordered_map < wstring, reflectObjectType> objectMap =
				{
					$<ClassReflecteString>
				};
				assert(objectMap.find(name) != objectMap.end());
				return objectMap[name]();
			})";
			ztl::generator::MarcoGenerator generator(templateString, { L"$<ClassReflecteString>" });

			wstring classReflecteString;
			auto global = manager->GetGlobalSymbol();
			for(auto&& iter : global->GetSubSymbolMap())
			{
				if(iter.second->IsClassType())
				{
					classReflecteString += GetObjectReflectionListString(iter.second->GetName());
				}
			}
			return generator.GenerateText({ classReflecteString }).GetMacroResult();

		}
		void CreatReflectionFile(SymbolManager* manager)
		{
			auto reflectionBuilderFunction  = GetReflectBuilderFunctionString(manager);
			auto objectReflectFunction		= GetOjectReflectioFunctionString(manager);

			CreateFile(L"test.cpp", reflectionBuilderFunction + objectReflectFunction);
		}
	}
}