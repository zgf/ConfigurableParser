#include "Include\stdafx.h"
#include "Include\GeneralLRExecutor.h"
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
		void ReflectionBuidler(const wstring& className, const wstring& fieldName, const shared_ptr<void>& classObject, const shared_ptr<void>& valueObject)
		{
			using builderType = void(*)(const shared_ptr<void>&, const shared_ptr<void>&);
			static unordered_map<wstring, unordered_map<wstring, builderType>> builderMap =
			{
				{
					L"GeneralTypeObject",
					{
					}
				},{
					L"GeneralGrammarSequenceTypeDefine",
					{
						{
							L"first",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralGrammarSequenceTypeDefine>(classObject)->first = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
				return;
			}
						},{
							L"second",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
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
					L"GeneralGrammarTypeDefine",
					{
					}
				},{
					L"GeneralAttributeArgumentDefine",
					{
						{
							L"name",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralAttributeArgumentDefine>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
				return;
			}
						},
					}
				},{
					L"GeneralSubTypeObject",
					{
						{
							L"parent",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralSubTypeObject>(classObject)->parent = std::static_pointer_cast<GeneralTypeObject>(valueObject);
				return;
			}
						},{
							L"name",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralSubTypeObject>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
				return;
			}
						},
					}
				},{
					L"GeneralAttributeDefine",
					{
						{
							L"name",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralAttributeDefine>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
				return;
			}
						},{
							L"arguments",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralAttributeDefine>(classObject)->arguments.emplace_back(std::static_pointer_cast<GeneralAttributeArgumentDefine>(valueObject));
				return;
			}
						},
					}
				},{
					L"GeneralGrammarSetterTypeDefine",
					{
						{
							L"name",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralGrammarSetterTypeDefine>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
				return;
			}
						},{
							L"grammar",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralGrammarSetterTypeDefine>(classObject)->grammar = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
				return;
			}
						},{
							L"value",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
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
							L"attributes",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralHeadInfoDefine>(classObject)->attributes.emplace_back(std::static_pointer_cast<GeneralAttributeDefine>(valueObject));
				return;
			}
						},{
							L"property",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralHeadInfoDefine>(classObject)->property = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
				return;
			}
						},{
							L"value",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralHeadInfoDefine>(classObject)->value = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
				return;
			}
						},
					}
				},{
					L"GeneralNormalTypeObject",
					{
						{
							L"name",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralNormalTypeObject>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
				return;
			}
						},
					}
				},{
					L"GeneralGrammarLoopTypeDefine",
					{
						{
							L"grammar",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralGrammarLoopTypeDefine>(classObject)->grammar = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
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
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralGrammarCreateTypeDefine>(classObject)->type = std::static_pointer_cast<GeneralTypeObject>(valueObject);
				return;
			}
						},{
							L"grammar",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
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
							L"attributes",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralEnumMemberTypeDefine>(classObject)->attributes.emplace_back(std::static_pointer_cast<GeneralAttributeDefine>(valueObject));
				return;
			}
						},{
							L"name",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
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
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralTokenDefine>(classObject)->regex = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
				return;
			}
						},{
							L"attributes",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralTokenDefine>(classObject)->attributes.emplace_back(std::static_pointer_cast<GeneralAttributeDefine>(valueObject));
				return;
			}
						},{
							L"name",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralTokenDefine>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
				return;
			}
						},{
							L"ignore",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralTokenDefine>(classObject)->ignore = static_cast<GeneralTokenDefine::TokenOptional>(WstringToEnumItem(L"GeneralTokenDefine::TokenOptional::" + std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content));
				return;
			}
						},
					}
				},{
					L"GeneralArrayTypeObject",
					{
						{
							L"element",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralArrayTypeObject>(classObject)->element = std::static_pointer_cast<GeneralTypeObject>(valueObject);
				return;
			}
						},
					}
				},{
					L"GeneralTableDefine",
					{
						{
							L"types",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralTableDefine>(classObject)->types.emplace_back(std::static_pointer_cast<GeneralTypeDefine>(valueObject));
				return;
			}
						},{
							L"heads",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralTableDefine>(classObject)->heads.emplace_back(std::static_pointer_cast<GeneralHeadInfoDefine>(valueObject));
				return;
			}
						},{
							L"tokens",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralTableDefine>(classObject)->tokens.emplace_back(std::static_pointer_cast<GeneralTokenDefine>(valueObject));
				return;
			}
						},{
							L"rules",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
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
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralClassMemberTypeDefine>(classObject)->type = std::static_pointer_cast<GeneralTypeObject>(valueObject);
				return;
			}
						},{
							L"attributes",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralClassMemberTypeDefine>(classObject)->attributes.emplace_back(std::static_pointer_cast<GeneralAttributeDefine>(valueObject));
				return;
			}
						},{
							L"name",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralClassMemberTypeDefine>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
				return;
			}
						},
					}
				},{
					L"GeneralClassTypeDefine",
					{
						{
							L"attributes",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralClassTypeDefine>(classObject)->attributes.emplace_back(std::static_pointer_cast<GeneralAttributeDefine>(valueObject));
				return;
			}
						},{
							L"name",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralClassTypeDefine>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
				return;
			}
						},{
							L"subTypes",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralClassTypeDefine>(classObject)->subTypes.emplace_back(std::static_pointer_cast<GeneralTypeDefine>(valueObject));
				return;
			}
						},{
							L"members",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralClassTypeDefine>(classObject)->members.emplace_back(std::static_pointer_cast<GeneralClassMemberTypeDefine>(valueObject));
				return;
			}
						},{
							L"parent",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
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
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralEnumTypeDefine>(classObject)->members.emplace_back(std::static_pointer_cast<GeneralEnumMemberTypeDefine>(valueObject));
				return;
			}
						},{
							L"attributes",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralEnumTypeDefine>(classObject)->attributes.emplace_back(std::static_pointer_cast<GeneralAttributeDefine>(valueObject));
				return;
			}
						},{
							L"name",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralEnumTypeDefine>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
				return;
			}
						},
					}
				},{
					L"GeneralGrammarTextTypeDefine",
					{
						{
							L"text",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
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
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
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
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
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
							L"grammar",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralGrammarAssignTypeDefine>(classObject)->grammar = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
				return;
			}
						},{
							L"name",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralGrammarAssignTypeDefine>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
				return;
			}
						},
					}
				},{
					L"GeneralGrammarUsingTypeDefine",
					{
						{
							L"grammar",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
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
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralGrammarAlternationTypeDefine>(classObject)->left = std::static_pointer_cast<GeneralGrammarTypeDefine>(valueObject);
				return;
			}
						},{
							L"right",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
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
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralRuleDefine>(classObject)->type = std::static_pointer_cast<GeneralTypeObject>(valueObject);
				return;
			}
						},{
							L"attributes",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralRuleDefine>(classObject)->attributes.emplace_back(std::static_pointer_cast<GeneralAttributeDefine>(valueObject));
				return;
			}
						},{
							L"name",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
			{
				std::static_pointer_cast<GeneralRuleDefine>(classObject)->name = std::static_pointer_cast<ztl::general_parser::TokenInfo>(valueObject)->content;
				return;
			}
						},{
							L"grammars",
							[](const shared_ptr<void>& classObject,const shared_ptr<void>& valueObject)
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
				},{
					L"GeneralAttributeArgumentDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralAttributeArgumentDefine>();
					}
				},{
					L"GeneralAttributeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GeneralAttributeDefine>();
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

		void GenerateHeterogeneousParserTree(ztl::general_parser::GeneralLRExecutor& parser, ztl::general_parser::GeneralTreeNode* classNode, shared_ptr<void>& classObject)
		{
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
					ztl::general_parser::GenerateHeterogeneousParserTree(parser, fieldNode, fieldObject);
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

		shared_ptr<void> GenerateHeterogeneousParserTree(ztl::general_parser::GeneralLRExecutor& parser, ztl::general_parser::GeneralTreeNode* root)
		{
			assert(root != nullptr);
			auto rootObject = ReflecteObjectByName(root->GetName());
			parser.SaveHeterogeneousNode(rootObject);
			ztl::general_parser::GenerateHeterogeneousParserTree(parser, root, rootObject);
			DealWithQuotation((GeneralTableDefine*) rootObject.get());

			return rootObject;
		}
		shared_ptr<void>	GenerateHeterogeneousParserTree(ztl::general_parser::GeneralLRExecutor& parser)
		{
			return ztl::general_parser::GenerateHeterogeneousParserTree(parser, parser.GetGeneralTreeRoot());
		}

	
	}
}