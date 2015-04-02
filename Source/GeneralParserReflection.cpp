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
		void ReflectionBuidler(const wstring& className, const wstring& fieldName, shared_ptr<void> classObject, shared_ptr<void> valueObject)
		{
			using builderType = void(*)(shared_ptr<void>, shared_ptr<void>);
			static unordered_map<wstring, unordered_map<wstring, builderType>> builderMap =
			{
				{
					L"GeneralTableDefine",
					{
						{
							L"heads",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralTableDefine>(classObject)->heads.emplace_back(std::static_pointer_cast<GeneralHeadInfoDefine>(valueObject));
								return;
							}
						},{
							L"types",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralTableDefine>(classObject)->types.emplace_back(std::static_pointer_cast<GeneralTypeDefine>(valueObject));
								return;
							}
						},{
							L"tokens",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralTableDefine>(classObject)->tokens.emplace_back(std::static_pointer_cast<GeneralTokenDefine>(valueObject));
								return;
							}
						},{
							L"rules",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralTableDefine>(classObject)->rules.emplace_back(std::static_pointer_cast<GeneralRuleDefine>(valueObject));
								return;
							}
						},
					}
				},{
					L"GeneralClassMemberTypeDefine",
					{
						{
							L"type",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralClassMemberTypeDefine>(classObject)->type = std::static_pointer_cast<GeneralTypeObject>(valueObject);
								return;
							}
						},{
							L"name",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralClassMemberTypeDefine>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralGrammarSetterTypeDefine",
					{
						{
							L"grammar",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralGrammarSetterTypeDefine>(classObject)->grammar = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
								return;
							}
						},{
							L"name",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralGrammarSetterTypeDefine>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
								return;
							}
						},{
							L"value",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralGrammarSetterTypeDefine>(classObject)->value = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralHeadInfoDefine",
					{
						{
							L"property",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralHeadInfoDefine>(classObject)->property = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
								return;
							}
						},{
							L"value",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralHeadInfoDefine>(classObject)->value = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralGrammarLoopTypeDefine",
					{
						{
							L"grammar",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralGrammarLoopTypeDefine>(classObject)->grammar = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
								return;
							}
						},
					}
				},{
					L"GeneralNormalTypeObject",
					{
						{
							L"name",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralNormalTypeObject>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralTokenTypeObject",
					{

					}
				},{
					L"GeneralGrammarCreateTypeDefine",
					{
						{
							L"type",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralGrammarCreateTypeDefine>(classObject)->type = std::static_pointer_cast<GeneralTypeObject>(valueObject);
								return;
							}
						},{
							L"grammar",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralGrammarCreateTypeDefine>(classObject)->grammar = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
								return;
							}
						},
					}
				},{
					L"GeneralEnumMemberTypeDefine",
					{
						{
							L"name",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralEnumMemberTypeDefine>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralTokenDefine",
					{
						{
							L"regex",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralTokenDefine>(classObject)->regex = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
								return;
							}
						},{
							L"name",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralTokenDefine>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
								return;
							}
						},{
							L"ignore",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralTokenDefine>(classObject)->ignore = static_cast<GeneralTokenDefine::TokenOptional>(WstringToEnumItem(L"GeneralTokenDefine::TokenOptional::" + std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content));
								return;
							}
						},
					}
				},{
					L"GeneralGrammarSequenceTypeDefine",
					{
						{
							L"first",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralGrammarSequenceTypeDefine>(classObject)->first = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
								return;
							}
						},{
							L"second",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralGrammarSequenceTypeDefine>(classObject)->second = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
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
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralArrayTypeObject>(classObject)->element = std::static_pointer_cast<GeneralTypeObject>(valueObject);
								return;
							}
						},
					}
				},{
					L"GeneralSubTypeObject",
					{
						{
							L"parent",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralSubTypeObject>(classObject)->parent = std::static_pointer_cast<GeneralTypeObject>(valueObject);
								return;
							}
						},{
							L"name",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralSubTypeObject>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralClassTypeDefine",
					{
						{
							L"subTypes",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralClassTypeDefine>(classObject)->subTypes.emplace_back(std::static_pointer_cast<GeneralTypeDefine>(valueObject));
								return;
							}
						},{
							L"name",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralClassTypeDefine>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
								return;
							}
						},{
							L"members",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralClassTypeDefine>(classObject)->members.emplace_back(std::static_pointer_cast<GeneralClassMemberTypeDefine>(valueObject));
								return;
							}
						},{
							L"parent",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralClassTypeDefine>(classObject)->parent = std::static_pointer_cast<GeneralTypeObject>(valueObject);
								return;
							}
						},
					}
				},{
					L"GeneralEnumTypeDefine",
					{
						{
							L"members",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralEnumTypeDefine>(classObject)->members.emplace_back(std::static_pointer_cast<GeneralEnumMemberTypeDefine>(valueObject));
								return;
							}
						},{
							L"name",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralEnumTypeDefine>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
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
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralGrammarTextTypeDefine>(classObject)->text = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralGrammarNormalTypeDefine",
					{
						{
							L"name",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralGrammarNormalTypeDefine>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
								return;
							}
						},
					}
				},{
					L"GeneralGrammarOptionalTypeDefine",
					{
						{
							L"grammar",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralGrammarOptionalTypeDefine>(classObject)->grammar = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
								return;
							}
						},
					}
				},{
					L"GeneralGrammarAssignTypeDefine",
					{
						{
							L"name",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralGrammarAssignTypeDefine>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
								return;
							}
						},{
							L"grammar",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralGrammarAssignTypeDefine>(classObject)->grammar = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
								return;
							}
						},
					}
				},{
					L"GeneralGrammarUsingTypeDefine",
					{
						{
							L"grammar",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralGrammarUsingTypeDefine>(classObject)->grammar = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
								return;
							}
						},
					}
				},{
					L"GeneralGrammarAlternationTypeDefine",
					{
						{
							L"left",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralGrammarAlternationTypeDefine>(classObject)->left = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
								return;
							}
						},{
							L"right",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralGrammarAlternationTypeDefine>(classObject)->right = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
								return;
							}
						},
					}
				},{
					L"GeneralRuleDefine",
					{
						{
							L"type",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralRuleDefine>(classObject)->type = std::static_pointer_cast<GeneralTypeObject>(valueObject);
								return;
							}
						},{
							L"name",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralRuleDefine>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
								return;
							}
						},{
							L"grammars",
							[](shared_ptr<void> classObject,shared_ptr<void> valueObject)
							{
								std::static_pointer_cast<GeneralRuleDefine>(classObject)->grammars.emplace_back(std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject));
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
					L"GeneralTokenTypeObject",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralTokenTypeObject>();
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
		void GeneralHeterogeneousParserTree(ztl::general_parser::GeneralParser& parser, ztl::general_parser::GeneralTreeNode* classNode, shared_ptr<void>& classObject)
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
					ztl::general_parser::GeneralHeterogeneousParserTree(parser, fieldNode, fieldObject);
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

		shared_ptr<void> GeneralHeterogeneousParserTree(ztl::general_parser::GeneralParser& parser, ztl::general_parser::GeneralTreeNode* root)
		{
			assert(root != nullptr);
			auto rootObject = ReflecteObjectByName(root->GetName());
			parser.SaveHeterogeneousNode(rootObject);
			ztl::general_parser::GeneralHeterogeneousParserTree(parser, root, rootObject);
			DealWithQuotation((GeneralTableDefine*) rootObject.get());
			return rootObject;
		}
		shared_ptr<void>	GeneralHeterogeneousParserTree(ztl::general_parser::GeneralParser& parser)
		{
			return ztl::general_parser::GeneralHeterogeneousParserTree(parser, parser.GetGeneralTreeRoot());
		}
	}
}