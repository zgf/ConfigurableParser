#include "Include\stdafx.h"
#include "Include\GeneralParser.h"
#include "Include\SymbolManager.h"
#include "Include\GeneralTreeNode.h"
namespace ztl
{
	namespace general_parser
	{
		int WstringToEnumItem(const wstring& value)
		{
			static unordered_map<wstring, int> signMap =
			{
				{
					L"GeneralTokenDefine::TokenOptional::False",
					static_cast<int>(GeneralTokenDefine::TokenOptional::False)
				},
				{
					L"GeneralTokenDefine::TokenOptional::True",
					static_cast<int>(GeneralTokenDefine::TokenOptional::True)
				},
			};
			assert(signMap.find(value) != signMap.end());
			return signMap[value];
		}
		void ReflectionBuidler(const wstring& className, const wstring& fieldName, void* classObject, void* valueObject)
		{
			using builderType = void(*)(void*, void*);
			static unordered_map<wstring, unordered_map<wstring, builderType>> builderMap =
			{
				{
					L"GeneralTableDefine",
					{
						{
							L"heads",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralTableDefine*>(classObject)->heads.emplace_back(static_cast<GeneralHeadInfoDefine*>(valueObject));
								return;
							}
						},{
							L"types",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralTableDefine*>(classObject)->types.emplace_back(static_cast<GeneralTypeDefine*>(valueObject));
								return;
							}
						},{
							L"tokens",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralTableDefine*>(classObject)->tokens.emplace_back(static_cast<GeneralTokenDefine*>(valueObject));
								return;
							}
						},{
							L"rules",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralTableDefine*>(classObject)->rules.emplace_back(static_cast<GeneralRuleDefine*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralClassMemberTypeDefine",
					{
						{
							L"type",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralClassMemberTypeDefine*>(classObject)->type.reset(static_cast<GeneralTypeObject*>(valueObject));
								return;
							}
						},{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralClassMemberTypeDefine*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralGrammarSetterTypeDefine",
					{
						{
							L"grammar",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarSetterTypeDefine*>(classObject)->grammar.reset(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarSetterTypeDefine*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},{
							L"value",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarSetterTypeDefine*>(classObject)->value = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralHeadInfoDefine",
					{
						{
							L"property",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralHeadInfoDefine*>(classObject)->property = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},{
							L"value",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralHeadInfoDefine*>(classObject)->value = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralGrammarLoopTypeDefine",
					{
						{
							L"grammar",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarLoopTypeDefine*>(classObject)->grammar.reset(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralNormalTypeObject",
					{
						{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralNormalTypeObject*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralGrammarCreateTypeDefine",
					{
						{
							L"type",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarCreateTypeDefine*>(classObject)->type.reset(static_cast<GeneralTypeObject*>(valueObject));
								return;
							}
						},{
							L"grammar",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarCreateTypeDefine*>(classObject)->grammar.reset(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralEnumMemberTypeDefine",
					{
						{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralEnumMemberTypeDefine*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralTokenDefine",
					{
						{
							L"regex",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralTokenDefine*>(classObject)->regex = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralTokenDefine*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},{
							L"ignore",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralTokenDefine*>(classObject)->ignore = static_cast<GeneralTokenDefine::TokenOptional>(WstringToEnumItem(L"GeneralTokenDefine::TokenOptional::" + static_cast<TokenInfo*>(valueObject)->content));
								return;
							}
						},
					}
				},{
					L"GeneralGrammarSequenceTypeDefine",
					{
						{
							L"first",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarSequenceTypeDefine*>(classObject)->first.reset(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},{
							L"second",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarSequenceTypeDefine*>(classObject)->second.reset(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralTypeDefine",
					{
					}
				},{
					L"GeneralTypeObject",
					{
					}
				},{
					L"GeneralArrayTypeObject",
					{
						{
							L"element",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralArrayTypeObject*>(classObject)->element.reset(static_cast<GeneralTypeObject*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralStringTypeObject",
					{
					}
				},{
					L"GeneralSubTypeObject",
					{
						{
							L"parent",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralSubTypeObject*>(classObject)->parent.reset(static_cast<GeneralTypeObject*>(valueObject));
								return;
							}
						},{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralSubTypeObject*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralClassTypeDefine",
					{
						{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralClassTypeDefine*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},{
							L"subTypes",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralClassTypeDefine*>(classObject)->subTypes.emplace_back(static_cast<GeneralTypeDefine*>(valueObject));
								return;
							}
						},{
							L"parent",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralClassTypeDefine*>(classObject)->parent.reset(static_cast<GeneralTypeObject*>(valueObject));
								return;
							}
						},{
							L"members",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralClassTypeDefine*>(classObject)->members.emplace_back(static_cast<GeneralClassMemberTypeDefine*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralEnumTypeDefine",
					{
						{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralEnumTypeDefine*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},{
							L"members",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralEnumTypeDefine*>(classObject)->members.emplace_back(static_cast<GeneralEnumMemberTypeDefine*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralGrammarTypeDefine",
					{
					}
				},{
					L"GeneralGrammarTextTypeDefine",
					{
						{
							L"text",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarTextTypeDefine*>(classObject)->text = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralGrammarNormalTypeDefine",
					{
						{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarNormalTypeDefine*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralGrammarOptionalTypeDefine",
					{
						{
							L"grammar",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarOptionalTypeDefine*>(classObject)->grammar.reset(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralGrammarAssignTypeDefine",
					{
						{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarAssignTypeDefine*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},{
							L"grammar",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarAssignTypeDefine*>(classObject)->grammar.reset(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralGrammarUsingTypeDefine",
					{
						{
							L"grammar",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarUsingTypeDefine*>(classObject)->grammar.reset(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralGrammarAlternationTypeDefine",
					{
						{
							L"left",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarAlternationTypeDefine*>(classObject)->left.reset(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},{
							L"right",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralGrammarAlternationTypeDefine*>(classObject)->right.reset(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralRuleDefine",
					{
						{
							L"type",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralRuleDefine*>(classObject)->type.reset(static_cast<GeneralTypeObject*>(valueObject));
								return;
							}
						},{
							L"name",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralRuleDefine*>(classObject)->name = static_cast<TokenInfo*>(valueObject)->content;
								return;
							}
						},{
							L"grammars",
							[](void* classObject,void* valueObject)
							{
								static_cast<GeneralRuleDefine*>(classObject)->grammars.emplace_back(static_cast<GeneralGrammarTypeDefine*>(valueObject));
								return;
							}
						},
					}
				},
			};
			auto findIter = builderMap.find(className);
			assert(findIter != builderMap.end());
			assert(findIter->second.find(fieldName) != findIter->second.end());
			builderMap[className][fieldName](classObject, valueObject);
		}
		shared_ptr<void> ReflecteObjectByName(const wstring& name)
		{
			using reflectObjectType = shared_ptr<void>(*)();
			static unordered_map < wstring, reflectObjectType> objectMap =
			{
				{
					L"GeneralTableDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralTableDefine>();
					}
				},
				{
					L"GeneralClassMemberTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralClassMemberTypeDefine>();
					}
				},
				{
					L"GeneralGrammarSetterTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarSetterTypeDefine>();
					}
				},
				{
					L"GeneralHeadInfoDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralHeadInfoDefine>();
					}
				},
				{
					L"GeneralGrammarLoopTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarLoopTypeDefine>();
					}
				},
				{
					L"GeneralNormalTypeObject",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralNormalTypeObject>();
					}
				},
				{
					L"GeneralGrammarCreateTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarCreateTypeDefine>();
					}
				},
				{
					L"GeneralEnumMemberTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralEnumMemberTypeDefine>();
					}
				},
				{
					L"GeneralTokenDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralTokenDefine>();
					}
				},
				{
					L"GeneralGrammarSequenceTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarSequenceTypeDefine>();
					}
				},
				{
					L"GeneralTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralTypeDefine>();
					}
				},
				{
					L"GeneralTypeObject",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralTypeObject>();
					}
				},
				{
					L"GeneralArrayTypeObject",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralArrayTypeObject>();
					}
				},
				{
					L"GeneralStringTypeObject",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralStringTypeObject>();
					}
				},
				{
					L"GeneralSubTypeObject",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralSubTypeObject>();
					}
				},
				{
					L"GeneralClassTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralClassTypeDefine>();
					}
				},
				{
					L"GeneralEnumTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralEnumTypeDefine>();
					}
				},
				{
					L"GeneralGrammarTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarTypeDefine>();
					}
				},
				{
					L"GeneralGrammarTextTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarTextTypeDefine>();
					}
				},
				{
					L"GeneralGrammarNormalTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarNormalTypeDefine>();
					}
				},
				{
					L"GeneralGrammarOptionalTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarOptionalTypeDefine>();
					}
				},
				{
					L"GeneralGrammarAssignTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarAssignTypeDefine>();
					}
				},
				{
					L"GeneralGrammarUsingTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarUsingTypeDefine>();
					}
				},
				{
					L"GeneralGrammarAlternationTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralGrammarAlternationTypeDefine>();
					}
				},
				{
					L"GeneralRuleDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralRuleDefine>();
					}
				},
			};
			assert(objectMap.find(name) != objectMap.end());
			return objectMap[name]();
		}
		void TrimLeftAndRightOneQuotation(wstring& value)
		{
			if(value.front() == '"')
			{
				value.erase(0, 1);
			}
			if(value.back() == '"')
			{
				value.pop_back();
			}
		}
		//处理捕获的字符串中多余的""和捕获的regex中多余的\"到"
		class GrammarQuotationVisitor:public GeneralGrammarTypeDefine::IVisitor
		{
		public:
			GrammarQuotationVisitor() noexcept = default;
			~GrammarQuotationVisitor() noexcept = default;
			GrammarQuotationVisitor(GrammarQuotationVisitor&&) noexcept = default;
			GrammarQuotationVisitor(const GrammarQuotationVisitor&) noexcept = default;
			GrammarQuotationVisitor& operator=(GrammarQuotationVisitor&&) noexcept = default;
			GrammarQuotationVisitor& operator=(const GrammarQuotationVisitor&) noexcept = default;

		protected:
			void								Visit(GeneralGrammarTextTypeDefine* node)
			{
				TrimLeftAndRightOneQuotation(node->text);
			}
			void								Visit(GeneralGrammarNormalTypeDefine*)
			{
			}
			void								Visit(GeneralGrammarSequenceTypeDefine* node)
			{
				node->first->Accept(this);
				node->second->Accept(this);
			}
			void								Visit(GeneralGrammarLoopTypeDefine* node)
			{
				node->grammar->Accept(this);
			}
			void								Visit(GeneralGrammarOptionalTypeDefine* node)
			{
				node->grammar->Accept(this);

			}
			void								Visit(GeneralGrammarSetterTypeDefine* node)
			{
				node->grammar->Accept(this);
				TrimLeftAndRightOneQuotation(node->value);
			}
			void								Visit(GeneralGrammarUsingTypeDefine* node)
			{
				node->grammar->Accept(this);

			}
			void								Visit(GeneralGrammarCreateTypeDefine* node)
			{
				node->grammar->Accept(this);
			}
			void								Visit(GeneralGrammarAlternationTypeDefine* node)
			{
				node->left->Accept(this);
				node->right->Accept(this);
			}
			void								Visit(GeneralGrammarAssignTypeDefine* node)
			{
				node->grammar->Accept(this);
			}
		private:
		};
		void DealWithQuotation(GeneralTableDefine* table)
		{
			for(auto&&iter : table->heads)
			{
				TrimLeftAndRightOneQuotation(iter->value);
			}
			for(auto&&iter : table->tokens)
			{
				TrimLeftAndRightOneQuotation(iter->regex);
				ztl::algorithm::replace_all_distinct<wstring>(iter->regex, LR"(\")", LR"(")");
			}
			GrammarQuotationVisitor visitor;
			for(auto&&iter : table->rules)
			{
				for(auto&& grammarIter : iter->grammars)
				{
					grammarIter->Accept(&visitor);

				}
			}
		}
	
		void GeneralHeterogeneousParserTree(GeneralParser& parser, GeneralTreeNode* classNode, shared_ptr<void>& classObject)
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
					ReflectionBuidler(className, fieldName, classObject.get(), fieldObject.get());
					GeneralHeterogeneousParserTree(parser,fieldNode, fieldObject);
				}
			}
			for(auto&&iter : classNode->GetTermMap())
			{
				auto fieldName = iter.first;
				for(auto&& nodeIter : iter.second)
				{
					ReflectionBuidler(className, fieldName, classObject.get(), parser.GetTermNodeByIndex(nodeIter));
				}
			}
		}
	
		shared_ptr<void> GeneralHeterogeneousParserTree(GeneralParser& parser,GeneralTreeNode* root)
		{
			assert(root != nullptr);
			auto rootObject = ReflecteObjectByName(root->GetName());
			parser.SaveHeterogeneousNode(rootObject);
			GeneralHeterogeneousParserTree(parser,root, rootObject);
			DealWithQuotation((GeneralTableDefine*)rootObject.get());
			return rootObject;
		}
		shared_ptr<void>	GeneralHeterogeneousParserTree(GeneralParser& parser)
		{
			return GeneralHeterogeneousParserTree(parser, parser.GetGeneralTreeRoot());
		}
	}
}