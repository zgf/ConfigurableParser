#pragma once
#include "Include/stdafx.h"
#include "Include/SymbolManager.h"
#include "Include/GeneralTableDefine.h"
#include "Include/GeneralParserFile.h"
#include "Include/ParserSymbol.h"
#include "../Lib/ZTL/ztl_generator.hpp"
#include "Include/GeneralTreeNode.h"
namespace ztl
{
	namespace general_parser
	{
		static const wstring namespacePrefix = LR"(ztl::general_parser::)";
		wstring GetArrayTypeFiledReflectTemplate(const wstring& basicTemplateString, ParserSymbol* fieldTypeSymbol)
		{
			const wstring tokenTypeString = namespacePrefix +L"TokenInfo";
			assert(fieldTypeSymbol->IsArrayType());
			auto elementTypeSymbol = fieldTypeSymbol->GetDescriptorSymbol();
			static unordered_map<SymbolType, wstring> signMap =
			{
				{
					SymbolType::ClassType,
					LR"(std::static_pointer_cast<$<ClassType>>(classObject)->$<FieldName>.emplace_back(std::static_pointer_cast<$<FieldType>>(valueObject));)"
				},
				{
					SymbolType::TokenType,
					LR"(std::static_pointer_cast<$<ClassType>>(classObject)->$<FieldName>.emplace_back(std::static_pointer_cast<)"+ tokenTypeString +LR"(>(valueObject)->content);)"
				},
				{
					SymbolType::EnumType,
					LR"(std::static_pointer_cast<$<ClassType>>(classObject)->$<FieldName>.emplace_back(static_cast<$<FieldType>>(WstringToEnumItem(L"$<FieldType>::" + std::static_pointer_cast<)" + tokenTypeString + LR"(>(valueObject)->content)));)"
				}
			};
			assert(signMap.find(elementTypeSymbol->GetType()) != signMap.end());
			ztl::generator::MarcoGenerator generator(basicTemplateString, { L"$<SymbolType>" });
			return generator.GenerateText({ signMap[elementTypeSymbol->GetType()] }).GetMacroResult();
		}
		wstring GetNonArrayTypeFiledReflectTemplate(const wstring& basicTemplateString, ParserSymbol* fieldTypeSymbol)
		{
			const wstring tokenTypeString = namespacePrefix + L"TokenInfo";
			assert(!fieldTypeSymbol->IsArrayType());
			static unordered_map<SymbolType, wstring> signMap =
			{
				{
					SymbolType::ClassType,
					LR"(std::static_pointer_cast<$<ClassType>>(classObject)->$<FieldName> = std::static_pointer_cast<$<FieldType>>(valueObject);)"
				},
				{
					SymbolType::TokenType,
					LR"(std::static_pointer_cast<$<ClassType>>(classObject)->$<FieldName>=std::static_pointer_cast<)" + tokenTypeString + LR"(>(valueObject)->content;)"
				},
				{
					SymbolType::EnumType,
					LR"(std::static_pointer_cast<$<ClassType>>(classObject)->$<FieldName>=static_cast<$<FieldType>>(WstringToEnumItem(L"$<FieldType>::" + std::static_pointer_cast<)" + tokenTypeString + LR"(>(valueObject)->content));)"
				}
			};
			ztl::generator::MarcoGenerator generator(basicTemplateString, { L"$<SymbolType>" });
			assert(signMap.find(fieldTypeSymbol->GetType()) != signMap.end());
			return generator.GenerateText({ signMap[fieldTypeSymbol->GetType()] }).GetMacroResult();
		}
		wstring GetReflectionFiledString(ParserSymbol* classSymbol, ParserSymbol*fieldDefSymbol)
		{
			wstring filedReflectString;
			wstring classType = classSymbol->GetSymbolAbsoluteName();
			wstring fieldName = fieldDefSymbol->GetName();
			assert(fieldDefSymbol->GetDescriptorSymbol() != nullptr);
			auto fieldTypeSymbol = fieldDefSymbol->GetDescriptorSymbol();
			auto fieldTypeName = (fieldTypeSymbol->IsArrayType()) ?
				fieldTypeSymbol->GetDescriptorSymbol()->GetSymbolAbsoluteName() :
				fieldTypeSymbol->GetSymbolAbsoluteName();
			wstring basicTemplateString = LR"({
										L"$<FieldName>",
										[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
										{
											$<SymbolType>
											return;
										}
									},)";
			auto typeReflectionTemplateString = (fieldTypeSymbol->IsArrayType()) ?
				GetArrayTypeFiledReflectTemplate(basicTemplateString, fieldTypeSymbol) :
				GetNonArrayTypeFiledReflectTemplate(basicTemplateString, fieldTypeSymbol);
			ztl::generator::MarcoGenerator generator(typeReflectionTemplateString, { L"$<ClassType>",L"$<FieldName>",L"$<FieldType>" });
			return generator.GenerateText({ classType,fieldName,fieldTypeName }).GetMacroResult();
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
					assert(iter.second->GetDescriptorSymbol() != nullptr);
					FieldReflectList += GetReflectionFiledString(classSymbol, iter.second);
				}
			}
			return generator.GenerateText({ classSymbol->GetName() ,FieldReflectList }).GetMacroResult();
		}
		wstring GetReflectBuilderFunctionString(SymbolManager* manager)
		{
			wstring tempalteString = LR"(
			void ReflectionBuidler(const wstring& className,const wstring& fieldName,shared_ptr<void> classObject,shared_ptr<void> valueObject)
			{
				using builderType = void(*)(shared_ptr<void>,shared_ptr<void>);
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
		wstring GetEnumSymbolString(ParserSymbol* enumSymbol)
		{
			assert(enumSymbol->IsEnumType());
			wstring result;
			wstring templateString = LR"(
				{
					L"$<EnumItemString>",
					static_cast<int>($<EnumItemString>)
				},)";
			for(auto&&iter : enumSymbol->GetSubSymbolMap())
			{
				auto itemSymbol = iter.second;
				auto enumItemValue = itemSymbol->GetSymbolAbsoluteName();
				ztl::generator::MarcoGenerator generator(templateString, { L"$<EnumItemString>" });
				result += generator.GenerateText({ enumItemValue }).GetMacroResult();
			}
			return result;
		}
		wstring GetWStringToEnumFunctionString(SymbolManager* manager)
		{
			auto global = manager->GetGlobalSymbol();
			deque<ParserSymbol*>queue;
			queue.emplace_back(global);
			wstring templateString = LR"(
			int WstringToEnumItem(const wstring& value)
			{
				static unordered_map<wstring, int> signMap =
				{
					$<initEnumList>
				};
				assert(signMap.find(value)!=signMap.end());
				return signMap[value];
			})";
			wstring initEnumList;
			while(!queue.empty())
			{
				auto front = queue.front();
				queue.pop_front();
				for(auto&& iter : front->GetSubSymbolMap())
				{
					if(iter.second->IsClassType())
					{
						queue.emplace_back(iter.second);
					}
					else if(iter.second->IsEnumType())
					{
						initEnumList += GetEnumSymbolString(iter.second);
					}
				}
			}
			ztl::generator::MarcoGenerator generator(templateString, { L"$<initEnumList>" });
			return generator.GenerateText({ initEnumList }).GetMacroResult();
		}
		wstring GetReflectionFunctionNameSpace(SymbolManager* manager)
		{
			auto namespaces = manager->GetCacheValueByProperty(L"namespace");
			return accumulate(namespaces.begin(), namespaces.end(), wstring(), [](const wstring& sum, const wstring& value)
			{
				return sum + value + L"::";
			});
		}
		wstring GetReflectionExecuteString(SymbolManager* manager)
		{
			wstring templateString =
				LR"(
				void GeneralHeterogeneousParserTree($<NameSpace>GeneralParser& parser, $<NameSpace>GeneralTreeNode* classNode, shared_ptr<void>& classObject)
				{
					assert(classObject != nullptr);
					assert(classNode != nullptr);
					auto className = classNode->GetName();
					for(auto&& iter : classNode->GetFieldMap())
					{
						auto fieldName = iter.first;
						for(auto&& nodeIter : iter.second)
						{
							auto fieldNode = parser.GetNonTermNodeByIndex(nodeIter);
							auto fieldObject = ReflecteObjectByName(fieldNode->GetName());
							parser.SaveHeterogeneousNode(fieldObject);
							ReflectionBuidler(className, fieldName, classObject, fieldObject);
							$<ReflectionNameSpace>GeneralHeterogeneousParserTree(parser,fieldNode, fieldObject);
						}
					}
					for(auto&&iter : classNode->GetTermMap())
					{
						auto fieldName = iter.first;
						for(auto&& nodeIter : iter.second)
						{
							ReflectionBuidler(className, fieldName, classObject, parser.GetTermNodeByIndex(nodeIter));
						}
					}
				}
	
				shared_ptr<void> GeneralHeterogeneousParserTree($<NameSpace>GeneralParser& parser,$<NameSpace>GeneralTreeNode* root)
				{
					assert(root != nullptr);
					auto rootObject = ReflecteObjectByName(root->GetName());
					parser.SaveHeterogeneousNode(rootObject);
					$<ReflectionNameSpace>GeneralHeterogeneousParserTree(parser,root, rootObject);
					return rootObject;
				}
				shared_ptr<void>	GeneralHeterogeneousParserTree($<NameSpace>GeneralParser& parser)
				{
					return $<ReflectionNameSpace>GeneralHeterogeneousParserTree(parser, parser.GetGeneralTreeRoot());
				}
				)";
			ztl::generator::MarcoGenerator generator(templateString, { L"$<NameSpace>" ,L"$<ReflectionNameSpace>"});
			return generator.GenerateText({ namespacePrefix , GetReflectionFunctionNameSpace(manager) }).GetMacroResult();
		}
		wstring GetReflectionModuleHead()
		{
			
			wstring templateString =
				LR"(
			shared_ptr<void> GeneralHeterogeneousParserTree($<NameSpace>GeneralParser& parser,$<NameSpace>GeneralTreeNode* root);
			shared_ptr<void>	GeneralHeterogeneousParserTree($<NameSpace>GeneralParser& parser);
			)";
			ztl::generator::MarcoGenerator generator(templateString, { L"$<NameSpace>" });
			return generator.GenerateText({ namespacePrefix }).GetMacroResult();
		}
		wstring GetReflectionModuleImp(SymbolManager* manager)
		{
			
			auto enumToStringFunction = GetWStringToEnumFunctionString(manager);
			auto reflectionBuilderFunction = GetReflectBuilderFunctionString(manager);
			auto objectReflectFunction = GetOjectReflectioFunctionString(manager);
			auto relectionExecuteFunction = GetReflectionExecuteString(manager);
			return enumToStringFunction + reflectionBuilderFunction + objectReflectFunction + relectionExecuteFunction;
		}
	}
}