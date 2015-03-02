#include "Include/stdafx.h"
#include "Include/SymbolManager.h"
#include "Include/ParserSymbol.h"
namespace ztl
{
	namespace general_parser
	{
		//����typeobject�������Ƿ��ڵ�ǰ��������ϲ㶨�������Ѿ�������,
		//�������typeobject,����typeobject��������ӳ������
		//ԭ����typeobject����ָ��ͬһ�����Ͷ���.����typeobject
		class FindTypeVisitor: public GeneralTypeObject::IVisitor
		{
			ParserSymbol* scope;
			SymbolManager* manager;
			ParserSymbol* result;
		public:
			FindTypeVisitor() = default;
			FindTypeVisitor(FindTypeVisitor&&) = default;
			FindTypeVisitor(const FindTypeVisitor&) = delete;
			FindTypeVisitor& operator=(const FindTypeVisitor&) = delete;
			FindTypeVisitor& operator=(FindTypeVisitor&&) = default;
			~FindTypeVisitor() = default;
			FindTypeVisitor(SymbolManager* _manager, ParserSymbol* _scope) :manager(_manager), scope(_scope)
			{
			}
		public:
			void								Visit(GeneralArrayTypeObject* node)
			{
				auto&& elementTypeSymbol = FindType(manager, scope, node->element.get());
				auto&& arrayTypeSymbol = manager->AddArrayType(elementTypeSymbol);
				result = arrayTypeSymbol;
			}
			void								Visit(GeneralStringTypeObject*)
			{
				auto&& tokenTypeSymbol = manager->GetTokenTypeSymbol();
				result = tokenTypeSymbol;
			}
			void								Visit(GeneralNormalTypeObject* node)
			{
				//���XX���������ǲ����Ѿ��ж��������,���������˵����������- -
				auto typeSymbol = manager->CheckNameHasDefine(node->name, scope);
				if(!typeSymbol)
				{
					throw ztl_exception(L"type" + node->name + L"Can't be define in front");
				}
				result = typeSymbol;
			}
			//���������ͽṹ A.B.C.D �����Ͳ���Ҫע�����������ĸ���.findType��һ����������D,sope�ǳ���A.B.C.D��������������X.
			//Ȼ��ȥ����A.B.C��������ͽṹ,findType����subType,ֱ���ҵ�A,A��classType.���A��Symbol��,return��ȥ.
			//�ص�B,��ʱ����������X.��ȻB��X���Ҳ���,������A���ҵĵ�,����findType subSymbol��ʱ��Ӧ��ȥparent�������ڲ���
			void								Visit(GeneralSubTypeObject* node)
			{
				auto subTypeName = node->name;
				auto parentType = node->parent;
				auto parentTypeSymbol = FindType(manager, scope, parentType.get());
				if(!parentTypeSymbol->IsClassType())
				{
					throw ztl_exception(L"parentTypeSymbol Need a ClassType,it's type is" + parentTypeSymbol->GetTypeToWString());
				}
				result = parentTypeSymbol->SearchClassSubSymbol(subTypeName);
				if(!result)
				{
					throw ztl_exception(L"this" + subTypeName + L"Cannot Find In ParentType's ClassScope,ParentTypeSymbol's name is" + parentTypeSymbol->GetName());
				}
			}
		public:
			ParserSymbol*						GetResult()
			{
				return result;
			}
		};

		ParserSymbol* FindType(SymbolManager* manager, ParserSymbol* scope, GeneralTypeObject* type)
		{
			ParserSymbol* symbol = manager->GetCacheTypeObjectSymbol(scope, type);
			if(symbol == nullptr)
			{
				FindTypeVisitor visitor(manager, scope);
				type->Accept(&visitor);
				symbol = visitor.GetResult();
				assert(symbol != nullptr);
				assert(symbol->IsType());
				manager->CacheTypeObjectSymbolMap(scope, type, symbol);
			}
			return symbol;
		}

		class CollectTypeDefineVisitor:public GeneralTypeDefine::IVisitor
		{
			SymbolManager* manager;
			ParserSymbol* scope;
		public:
			CollectTypeDefineVisitor() = default;
			CollectTypeDefineVisitor(SymbolManager* _manager, ParserSymbol* _scope) :manager(_manager), scope(_scope)
			{
			}
		public:
			void								Visit(GeneralClassTypeDefine* node)
			{
				auto name = node->name;
				auto classTypeSymbol = manager->AddClass(name, node->parent == nullptr ? nullptr : FindType(manager, scope, node->parent.get()), scope);
				//	manager->CacheClassDefineAndSymbolMap(node, classTypeSymbol);
				swap(scope, classTypeSymbol);
				for(auto&& iter : node->subType)
				{
					iter->Accept(this);
				}
				for(auto&& iter : node->members)
				{
					iter->Accept(this);
				}
				swap(scope, classTypeSymbol);
			}
			void								Visit(GeneralEnumTypeDefine* node)
			{
				auto name = node->name;
				auto enumTypeSymbol = manager->AddEnum(name, scope);
				//manager->CacheEnumTypeAndSymbolMap(node, enumTypeSymbol);
				swap(scope, enumTypeSymbol);
				for(auto&&iter : node->members)
				{
					iter->Accept(this);
				}
				swap(scope, enumTypeSymbol);
			}
			void								Visit(GeneralClassMemberTypeDenfine* node)
			{
				auto name = node->name;
				auto fieldSymbol = manager->AddField(name, nullptr, scope);
				auto fieldType = FindType(manager, scope, node->type.get());
				fieldSymbol->SetDescriptor(fieldType);
			}
			void								Visit(GeneralEnumMemberTypeDefine* node)
			{
				auto enumItemSymbol = manager->AddEnumItem(node->name, scope);
			}
		};
		//�����﷨���ڵ�,������ű�ڵ�.
		//�������͵ı�ʾ���б���
		//���͵Ķ�������͵Ķ���
		//����:
		//	1.��֤��������Ĵ���:
		//		1.�������ڱ����ض�������.��ǰ�ӷ��ű����Ƿ��Ѵ��ڶ���
		//		2.��ǰ��ʾ��(type)�Ƿ��Ѿ������������;�����������.��̳���������.
		//		3.regex���ܴ���ͬ���Ķ��ʵ�岻ͬtokenName
		//����������Ͷ������Ͷ����ӳ��
		//(typeobject*,scope)->classDef/enumDef
		//typeobjec����ͬ��,���ǲ�������ͬ��ʵ��(��ַ��ͬ),�Ҿ�������û��Ҫ����scope,������ typeobject*->typeDef
		//���ڹ��������,����typeobejct���󱣴��,ͨ����typeobjectName,scope�Ĳ��ҾͿ��Ը㶨��
		//����(className,scope)->classDefSymbol,�����Ϊ�﷨���ⲻ��Ҫ��,�ķ��г��ֵ�classNameҪô��classDef��,Ҫô������typeobject������.����typeobject->typeDef�Ѿ�������.
		//����regexName->tokenDefSymbol
		//����tokenName->tokenDefSymbol
		//����ruleName->ruleDefSymbol
		//����GrmmarDef�﷨���ڵ�->classDefSybmol��ruleDefSymbol
		//  2.�����﷨���ڵ�,������ű�ķ������ͽṹ�ͷ��Ż���ṹ. ParserSymbol*����˷������νṹ.

		//  3.��֤���������Ƿ���������Ҫ��.�﷨���ṹ�Ƿ���������Ҫ��.

		void CollectAndValidateTypeDefine(SymbolManager * manager)
		{
			auto table = manager->GetTable();
			CollectTypeDefineVisitor visitor(manager, manager->GetGlobalSymbol());
			for(auto&& iter : table->types)
			{
				iter->Accept(&visitor);
			}

			for(auto&& iter : table->tokens)
			{
				auto&& tokenSymbol = manager->AddTokenDefine(iter->name, iter->regex, iter->ignore);
			}

			for(auto&&iter : table->rules)
			{
				//���rule��type�ǲ�����ǰ�Ѷ����,����rule�ڵ�����ű�ڵ�İ�
				auto ruleType = FindType(manager, manager->GetGlobalSymbol(), iter->type.get());
				if(ruleType->IsClassType())
				{
					auto ruleSymbol = manager->AddRuleDefine(iter->name, ruleType);
					manager->CacheRuleDefineAndSymbolMap(iter.get(), ruleSymbol);
				}
				else
				{
					throw ztl_exception(L"Rule type only is class type");
				}
			}
		}

		//��֤����Ľṹ�Ƿ���������Ҫ��
		//����NormalGammar������һ��token����regex����ruleName
		//TextGrammar������һ��regex.
		//CreatGrammar������ѭ��LoopGrammar�ڲ�
		//һ��·��ֻ����һ��CreatGammar
		//CreatGrammar���ɵı�����classType�ڵ�.CreatGrammar��type�������
		//UsingGrammar������grammarֻ����NormalGramar����NormalGrammar��������ruleName.
		//UsingGrammar ·����ֻ����Using��NormalGrammar.�����﷨���ڵ�
		//AssignGrammar��grammar������Normal�����ǹ�����
		//����grammar������NormalGrammar ����NormalGrammar������ruleName.
		//className ������A ������A.B��ʽ
		//����Normal�ڵ��text��Symbol��ӳ��
		//�滻regexΪ��Ӧ��tokenName

		class ValidateGrammarNodeIVisitor:public GeneralGrammarTypeDefine::IVisitor
		{
			SymbolManager* manager;
		public:
			ValidateGrammarNodeIVisitor(SymbolManager* _manager) :manager(_manager)
			{
			}
		private:
			void CheckGrammarUseDisTokenNameError(const wstring& name)
			{
				auto disTokensymbol = manager->GetCacheDisTokenNameSymbol(name);
				if (!disTokensymbol)
				{
					ztl_exception(L"can' use the distoken name in grammar");
				}
			}
			ParserSymbol* CheckRuleNameDisExistError(const wstring& name)
			{
				auto ruleSymbol = manager->GetCacheRuleNameToSymbol(name);
				if(!ruleSymbol)
				{
					throw ztl_exception(L"This rule name not exist");
				}
				return ruleSymbol;
			}
		public:
			void								Visit(GeneralGrammarTextTypeDefine* node)
			{
				auto regex = LinearStringToRegex(node->text);
				auto tokenSymbol = manager->GetCacheRegexStringToSymbol(regex);
				if(!tokenSymbol)
				{
					throw ztl_exception(L"This text"+node->text+L" not exist");
				}
				node->text = tokenSymbol->GetName();
				manager->CacheTextGrammarToTokenDefSymbol(node, tokenSymbol);
			}
			void								Visit(GeneralGrammarNormalTypeDefine* node)
			{
				CheckGrammarUseDisTokenNameError(node->name);
				auto tokenSymbol = manager->GetCacheTokenNameToSymbol(node->name);
				if(!tokenSymbol)
				{
					auto ruleSymbol = CheckRuleNameDisExistError(node->name);
					manager->CacheNormalGrammarToRuleDefSymbol(node, ruleSymbol);
				}
				else
				{
					assert(!tokenSymbol->IsIgnore());
					manager->CacheNormalGrammarToTokenDefSymbol(node, tokenSymbol);
				}
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
			}
			void								Visit(GeneralGrammarUsingTypeDefine* node)
			{
				node->grammar->Accept(this);
			}
			void								Visit(GeneralGrammarCreateTypeDefine* node)
			{
				
				auto findSymbol = FindType(manager, manager->GetGlobalSymbol(), node->type.get());
				if(!findSymbol->IsClassType())
				{
					throw ztl_exception(L"Creat Node only is Class Type");
				}
				node->grammar->Accept(this);
			}
			void								Visit(GeneralGrammarAlterationTypeDefine* node)
			{
				node->left->Accept(this);
				ValidateGrammarNodeIVisitor visitor(*this);
				node->right->Accept(&visitor);
			}
			void								Visit(GeneralGrammarAssignTypeDefine* node)
			{
				node->grammar->Accept(this);
				auto normalTypeNodeDefine = dynamic_cast<GeneralGrammarNormalTypeDefine*>(node->grammar.get());
				if (!normalTypeNodeDefine)
				{
					throw ztl_exception(L"assgin's sub node type only is GeneralGrammarNormalTypeDefine");
				}
			}
		};
		void ValidateGrammarNode(SymbolManager * manager)
		{
			for(auto&& ruleIter : manager->GetTable()->rules)
			{
				for(auto&& grammarIter : ruleIter->grammars)
				{
					ValidateGrammarNodeIVisitor visitor(manager);
					grammarIter->Accept(&visitor);
				}
			}
		}


		//����·�����:
		//
		enum class NodeType
		{
			Create,
			Normal,
			Text,
			Assign,
			Using,
			//Optional,
			Setter,
			//Alteration,
			LoopStart,
			LoopEnd,
			//Sequence,
		};
		class GeneratePathNode
		{
			
			//�ڵ��Ӧ���ķ�
			//assginֻ����classtype��tokentype
			//setterֻ����tokentype��enumtype ����set value������string. tokenTypeֱ�ӾͿ��Ի��ֵ��.enumType���Դӻ����õ��ֶ�typeȻ�������ӷ��ŵõ�enumDefType��name
			//����assign.ֻ����classType��tokenType��rule���ؽڵ�.���ؽڵ�û��֪��,���Ƿ��ؽڵ�����Ϳ����õ�.���ؽڵ�value���ʹӹ������Ϳ���֪��,������Ҫһ��wstring��normalGrammar��name
			//�ֶ����Ϳ��Դӻ���õ�.
			//tokenType��ֵ��wstring.����Ӧ����wstring
			//������Ի���ֶ�����{�ֶ�ֵ} setter �ֶ�ֵ��wstring value.assign��value��grammar���صĽڵ�.���Դ�
			//����assgin�ķ�.�ֶ�����ֻ����class��token��type.��ֵ��value,�ò���,���﷨�����ؽڵ�,��ruleName����֪��,ruleName��ruleDef�Ѿ�������.
			//����setter�ķ�.�ֶ�����ֻ����enum��token��type.filedSymbol�Ѿ�������,����Ҳ�ͱ�������.value��ֵ��string���enumֵ,enumֵ���Դ�enumType��subSymbol���.
			//����using�ķ�.ֱ�Ӵ�ruleName��OK
			//����created.parserSymbol����path�ṹ��.
			GeneralGrammarTypeDefine*	grammar;
			NodeType					type;
			wstring						descriptor;

		public:
			GeneratePathNode()  = default;
			~GeneratePathNode()  = default;
			GeneratePathNode(GeneratePathNode&&)  = default;
			GeneratePathNode(const GeneratePathNode&)  = default;
			GeneratePathNode& operator=(GeneratePathNode&&)  = default;
			GeneratePathNode& operator=(const GeneratePathNode&)   = default;
			GeneratePathNode(GeneralGrammarTypeDefine* _grammar, NodeType _type, const wstring& _descriptor) :grammar(_grammar), type(_type), descriptor(_descriptor)
			{

			}
		public:
			NodeType GetType() const
			{
				return type;
			}

			wstring GetDescriptor()const
			{
				return descriptor;
			}
			GeneralGrammarTypeDefine* GetGrammarTypeDefine()const
			{
				return grammar;
			}
			wstring GetTypeToWString() const
			{
				wstring result;
				switch(type)
				{
					case ztl::general_parser::NodeType::Create:
						result = L"Create";
						break;
					case ztl::general_parser::NodeType::Normal:
						result = L"Normal";
						break;
					case ztl::general_parser::NodeType::Text:
						result = L"Text";
						break;
					case ztl::general_parser::NodeType::Assign:
						result = L"Assign";
						break;
					case ztl::general_parser::NodeType::Using:
						result = L"Using";
						break;
					case ztl::general_parser::NodeType::Setter:
						result = L"Setter";
						break;
					case ztl::general_parser::NodeType::LoopStart:
						result = L"LoopStart";
						break;
					case ztl::general_parser::NodeType::LoopEnd:
						result = L"LoopEnd";
						break;
					default:
						assert(false);
						break;
				}
				return result;
			}

			bool IsCreate()const
			{
				return type == NodeType::Create;
			}
			bool IsNormal()const
			{
				return type == NodeType::Normal;
			}
			bool IsText()const
			{
				return type == NodeType::Text;
			}
			bool IsAssign()const
			{
				return type == NodeType::Assign;
			}
			bool IsUsing()const
			{
				return type == NodeType::Using;
			}
			bool IsLoopStart()const
			{
				return type == NodeType::LoopStart;
			}
			bool IsSetter()const
			{
				return type == NodeType::Setter;
			}
			bool IsLoopEnd()const
			{
				return type == NodeType::LoopEnd;
			}
			/*bool IsLoop()const
			{
				return type == NodeType::Loop;
			}*/
		/*	bool IsSequence()const
			{
				return type == NodeType::Sequence;
			}
*/
		};
		class GeneratePath
		{
			vector<GeneratePathNode> pathNodeList;
			ParserSymbol* createdTyepSymbol = nullptr;
		public:
			GeneratePath()  =default;
			~GeneratePath()  = default;
			GeneratePath(GeneratePath&&)  = default;
			GeneratePath(const GeneratePath&)  = default;
			GeneratePath& operator=(GeneratePath&&)  = default;
			GeneratePath& operator=(const GeneratePath&)   = default;
		public:
			
			void AddNextNode(GeneralGrammarTypeDefine* grammar, NodeType type, const wstring& descriptor)
			{
				pathNodeList.emplace_back(GeneratePathNode( grammar,type,descriptor ));
				
			}
			void SetCreatedTypeSymbol(ParserSymbol* _createdTypeSymbol)
			{
				this->createdTyepSymbol = _createdTypeSymbol;
			}
			ParserSymbol* GetCreatTypeSymbol()const
			{
				return createdTyepSymbol;
			}
			vector<GeneratePathNode>& GetPathNodeList()
			{
				return pathNodeList;
			}
		};
		//��rule�µ�ÿһ��grammar�ռ�·��
		//
		class CollectGeneratePathVisitor: public GeneralGrammarTypeDefine::IVisitor
		{
			SymbolManager*							manager;//grammar->TypeSymbol
			vector<unique_ptr<GeneratePath>>		paths;
			
		public:
			CollectGeneratePathVisitor()  = default;
			~CollectGeneratePathVisitor()  = default;
			CollectGeneratePathVisitor(CollectGeneratePathVisitor&&)  = default;
			CollectGeneratePathVisitor(CollectGeneratePathVisitor& target):manager(target.manager)
			{
				for (auto& iter:target.paths)
				{
					paths.emplace_back(make_unique<GeneratePath>(*iter));
				}
				
			}
			CollectGeneratePathVisitor& operator=(CollectGeneratePathVisitor&&)  = default;
			CollectGeneratePathVisitor& operator=(CollectGeneratePathVisitor&target)
			{
				manager = target.manager;
				for(auto& iter : target.paths)
				{
					paths.emplace_back(make_unique<GeneratePath>(*iter));
				}
			}
			CollectGeneratePathVisitor(SymbolManager* _manager) :manager(_manager)
			{

			}
		public:
			void AddNode(GeneralGrammarTypeDefine* grammar, NodeType type, const wstring& descriptor)
			{
				if (paths.empty())
				{
					paths.emplace_back(make_unique<GeneratePath>());
					paths.back()->AddNextNode(grammar, type, descriptor);
				}
				else
				{
					for (auto&& pathIter : paths)
					{
						pathIter->AddNextNode(grammar,type,descriptor);
					}
				}
				
			}
			void SetCreatSymbolNode(ParserSymbol* createdTypeSymbol)
			{
				assert(!paths.empty());
				
				for(auto&& pathIter : paths)
				{
					pathIter->SetCreatedTypeSymbol(createdTypeSymbol);
				}
				

			}
			void JoinPaths(CollectGeneratePathVisitor& target)
			{
				for (auto&& iter:target.paths)
				{
					paths.emplace_back(std::move(iter));
				}
				target.paths.clear();
			}
			auto begin()->decltype(paths.begin())
			{
				return paths.begin();
			}
			auto end() ->decltype(paths.end())
			{
				return paths.end();
			}
			void								Visit(GeneralGrammarTextTypeDefine* node)
			{
				AddNode(node, NodeType::Text,node->text);
			}
			void								Visit(GeneralGrammarNormalTypeDefine* node)
			{
				AddNode(node, NodeType::Normal,node->name);
			}
			void								Visit(GeneralGrammarSequenceTypeDefine* node)
			{
				node->first->Accept(this);
				node->second->Accept(this);
			}
			void								Visit(GeneralGrammarLoopTypeDefine* node)
			{
				CollectGeneratePathVisitor visitor(*this);
				visitor.AddNode(node, NodeType::LoopStart, L"");
				node->grammar->Accept(&visitor);
				visitor.AddNode(node, NodeType::LoopEnd, L"");
				JoinPaths(visitor);
			}
			void								Visit(GeneralGrammarOptionalTypeDefine* node)
			{
				CollectGeneratePathVisitor visitor(*this);
				node->grammar->Accept(&visitor);
				JoinPaths(visitor);
			}
			void								Visit(GeneralGrammarSetterTypeDefine* node)
			{
				node->grammar->Accept(this);
				AddNode(node, NodeType::Setter, node->value);
			}
			void								Visit(GeneralGrammarUsingTypeDefine* node)
			{
				node->grammar->Accept(this);
				assert(dynamic_cast<GeneralGrammarNormalTypeDefine*>(node->grammar.get()));
				auto ruleName = manager->GetCacheNormalGrammarToRuleDefSymbol(node->grammar.get())->GetName();
				AddNode(node, NodeType::Using, ruleName);
			}
			void								Visit(GeneralGrammarCreateTypeDefine* node)
			{
				auto createdTypeSymbol = FindType(manager, manager->GetGlobalSymbol(), node->type.get());
				AddNode(node, NodeType::Create,createdTypeSymbol->GetName());
				node->grammar->Accept(this);
				SetCreatSymbolNode(createdTypeSymbol);
			}
			void								Visit(GeneralGrammarAlterationTypeDefine* node)
			{
				CollectGeneratePathVisitor visitor(*this);
				node->left->Accept(&visitor);
				node->right->Accept(this);
				JoinPaths(visitor);
			}
			void								Visit(GeneralGrammarAssignTypeDefine* node)
			{
				node->grammar->Accept(this);
				assert(dynamic_cast<GeneralGrammarNormalTypeDefine*>(node->grammar.get()));
				auto ruleName = manager->GetCacheNormalGrammarToRuleDefSymbol(node->grammar.get())->GetName();
				AddNode(node, NodeType::Assign, ruleName);
			}
		};
		
		unordered_map<GeneralRuleDefine*, vector<unique_ptr<GeneratePath>>>  CollectGeneratePath(SymbolManager* manager)
		{
			unordered_map<GeneralRuleDefine*, vector<unique_ptr<GeneratePath>>> pathMap;
			for(auto&& ruleIter : manager->GetTable()->rules)
			{
				auto rulePointer = ruleIter.get();
				pathMap[rulePointer];
				for(auto&& grammarIter : ruleIter->grammars)
				{
					CollectGeneratePathVisitor visitor(manager);
					grammarIter->Accept(&visitor);
					for (auto&& visitorIter: visitor)
					{
						pathMap[rulePointer].emplace_back(move(visitorIter));
					}
					
				}
			}
			return pathMap;
		}

		//����·����֤
		//һ��·����ֻ��һ��creat Node
		//���ڷ�����ĳ�Ա,��θ�ֵ������.
		//����û�ж�Node���г�Ա��ֵ��,��������
		//·����֤ SetterGrammarҪ��name��CreatGrammar�﷨��ָ����class�ĳ�Ա.SetterGrammerҪ����name��ĳ��class�ĳ�Ա��valueҪ���ݳ�Ա������
		//�����Ա��TokenType,Ҫôvalue�϶�ƥ��
		//Setter��Ա����ֻ����token��enum
		//Loop·���ڵ�setter��assign��name�������������� �����ͼ���.
		//·����֤ AssignҪ��grammar��type��name��type����,name��CreatGrammarָ����class�ĳ�Ա��.
		//CraetGrammarһ��·����ֻ����һ��
		//CreatGrammarҪ����ת��ΪRule����������
		//Using�ķ��ص��﷨���ڵ�Ӧ�ÿ���ת����rule��type�ڵ�
		//·����ֻҪ��Using�Ͳ���ҪCreat��
		//����GrammarNode��classFieldSymbol��ӳ��
		//Setter������as����!����ſ���ʹ��
		class ValidateGeneratePathIVisitor:public GeneralGrammarTypeDefine::IVisitor
		{
			SymbolManager* manager;
			GeneralRuleDefine*  ruleDefine;
			ParserSymbol*  createdTypeSymbol = nullptr;

			size_t		   creatNodeCount = 0;
			unique_ptr<unordered_set<ParserSymbol*>> fieldSet;
			bool		   inLoop = false;

		public:
			ValidateGeneratePathIVisitor() noexcept = default;
			~ValidateGeneratePathIVisitor() noexcept = default;
			ValidateGeneratePathIVisitor(ValidateGeneratePathIVisitor&&) noexcept = default;
			ValidateGeneratePathIVisitor(const ValidateGeneratePathIVisitor&) noexcept = default;
			ValidateGeneratePathIVisitor& operator=(ValidateGeneratePathIVisitor&&) noexcept = default;
			ValidateGeneratePathIVisitor& operator=(const ValidateGeneratePathIVisitor&) noexcept = default;
			ValidateGeneratePathIVisitor(SymbolManager* _manager, GeneralRuleDefine* _ruleDefine)noexcept :manager(_manager), ruleDefine(_ruleDefine), fieldSet(make_unique<unordered_set<ParserSymbol*>>())
			{
			}
		public:
			void SetLoopFlag()
			{
				inLoop = true;
			}
			void ClearLoopFlag()
			{
				inLoop = false;
			}
		private:
			void CheckEnumTypeFieldCompatibleValue(ParserSymbol* fieldTypeSymbol, GeneralGrammarSetterTypeDefine* grammar)
			{
				assert(fieldTypeSymbol->IsEnumType());

				auto findIter = fieldTypeSymbol->GetSubSymbolByName(grammar->value);
				if(!findIter)
				{
					throw ztl_exception(L"Can't find this enum value" + grammar->value + L"in enum type" + fieldTypeSymbol->GetName());
				}
			}

			void CheckClassTypeFieldCompatibleValue(ParserSymbol* fieldTypeSymbol, GeneralGrammarAssignTypeDefine* node)
			{
				assert(fieldTypeSymbol->IsClassType());
				//value ����
				auto ruleTypeSymbol = GetRuleSymbolByNormalGrammarParent(node->grammar.get())->GetDescriptorSymbol();

				//class ��Ա����
				auto& createdTypeFieldTypeSymbol = fieldTypeSymbol;
				if(createdTypeFieldTypeSymbol->IsArrayType())
				{
					createdTypeFieldTypeSymbol = createdTypeFieldTypeSymbol->GetDescriptorSymbol();
				}
				auto result = ruleTypeSymbol->EqualToBaseClassType(createdTypeFieldTypeSymbol);
				if(!result)
				{
					throw ztl_exception(L"Can't convet assgin value type to assgin field type");
				}
			}
			void CheckFieldTypeCompatibleValue(ParserSymbol* fieldTypeSymbol, GeneralGrammarAssignTypeDefine* node)
			{
				if(fieldTypeSymbol->IsClassType())
				{
					CheckClassTypeFieldCompatibleValue(fieldTypeSymbol, node);
				}
				else if(!fieldTypeSymbol->IsTokenType())
				{
					throw ztl_exception(L"assign field type only is token or class type");
				}
			}
			void CheckFieldTypeCompatibleValue(ParserSymbol* fieldTypeSymbol, GeneralGrammarSetterTypeDefine* node)
			{
				if(fieldTypeSymbol->IsEnumType() || fieldTypeSymbol->IsTokenType())
				{
					CheckEnumTypeFieldCompatibleValue(fieldTypeSymbol, node);
				}
				else
				{
					throw ztl_exception(L"setter field type only is token or enum type");
				}
			}
			ParserSymbol* GetRuleSymbolByNormalGrammarParent(GeneralGrammarTypeDefine* node)
			{
				GeneralGrammarNormalTypeDefine* normalTypeNodeDefine = dynamic_cast<GeneralGrammarNormalTypeDefine*>(node);
				if(!normalTypeNodeDefine)
				{
					throw ztl_exception(L"this node not a normal node");
				}
				else
				{
					auto ruleSymbol = manager->GetCacheRuleNameToSymbol(normalTypeNodeDefine->name);
					if(!ruleSymbol)
					{
						throw ztl_exception(L"This rule name or token name not exist");
					}
					return ruleSymbol;
				}
			}
			void CheckCreatNodeMoreTimeError()
			{
				if(++creatNodeCount > 1)
				{
					throw ztl_exception(L"One generator path only could have one creat node or using");
				}
			}
			void CheckSetFiledMoreTime(ParserSymbol* filedDefSymbol)
			{
				assert(filedDefSymbol->IsFieldDef());
				assert(!filedDefSymbol->GetDescriptorSymbol()->IsArrayType());
				auto findIter = fieldSet->find(filedDefSymbol);
				if(findIter == fieldSet->end())
				{
					fieldSet->insert(filedDefSymbol);
				}
				else
				{
					throw ztl_exception(L"field not array type,but assgin more time");
				}
			}
			void CheckCreatNodeExist()
			{
				if(!createdTypeSymbol)
				{
					throw ztl_exception(L"grammar must have '!' or 'as'");
				}
			}
			void CheckTypeSymbolIsArrayType(ParserSymbol* typeSymbol, const wstring& error)
			{
				if(!typeSymbol->IsArrayType())
				{
					throw ztl_exception(error);
				}
			}
			void CheckCreatAndUsingInLoopError()
			{
				if(this->inLoop)
				{
					throw ztl_exception(L"'!' or 'as' symbol can't in loop");
				}
			}
		public:

			void								Visit(GeneralGrammarTextTypeDefine*)
			{
				return;
			}
			void								Visit(GeneralGrammarNormalTypeDefine*)
			{
				return;
			}
			void								Visit(GeneralGrammarSequenceTypeDefine*)
			{
				assert(false);
			}
			void								Visit(GeneralGrammarLoopTypeDefine*)
			{
				assert(false);
			}
			void								Visit(GeneralGrammarOptionalTypeDefine*)
			{
				assert(false);
			}

			void								Visit(GeneralGrammarSetterTypeDefine* node)
			{
				CheckCreatNodeExist();
				auto fieldSymbol = createdTypeSymbol->SearchClassFieldSymbol(node->name);
				if(!fieldSymbol)
				{
					throw ztl_exception(L"Can't find filed name" + node->name + L"in the class" + createdTypeSymbol->GetName());
				}
				auto fieldTypeSymbol = fieldSymbol->GetDescriptorSymbol();
				//�����Ͷ����ռ�����֤�м�����.
				assert(fieldTypeSymbol);


				if(!inLoop)
				{
					CheckSetFiledMoreTime(fieldSymbol);
				}
				else
				{
					CheckTypeSymbolIsArrayType(fieldTypeSymbol, L"In the loop,set field must is array type");

					fieldTypeSymbol = fieldTypeSymbol->GetDescriptorSymbol();
				}
				CheckFieldTypeCompatibleValue(fieldTypeSymbol, node);
				manager->CacheGrammarToFieldDefSymbol(node, fieldSymbol);
			}
			void								Visit(GeneralGrammarAssignTypeDefine* node)
			{
				CheckCreatNodeExist();
				auto fieldSymbol = createdTypeSymbol->SearchClassFieldSymbol(node->name);
				if(!fieldSymbol)
				{
					throw ztl_exception(L"Can't find filed name" + node->name + L"in the class" + createdTypeSymbol->GetName());
				}
				auto fieldTypeSymbol = fieldSymbol->GetDescriptorSymbol();

				assert(fieldTypeSymbol);
				if(!inLoop)
				{
					CheckSetFiledMoreTime(fieldSymbol);
				}
				else
				{
					CheckTypeSymbolIsArrayType(fieldTypeSymbol, L"In the loop,set field must is array type");
					fieldTypeSymbol = fieldTypeSymbol->GetDescriptorSymbol();
				}
				CheckFieldTypeCompatibleValue(fieldTypeSymbol, node);
				manager->CacheGrammarToFieldDefSymbol(node, fieldSymbol);
			}
			void								Visit(GeneralGrammarUsingTypeDefine* node)
			{
				CheckCreatNodeMoreTimeError();
				CheckCreatAndUsingInLoopError();

				auto returnRuleDefineSymbol = GetRuleSymbolByNormalGrammarParent(node->grammar.get());
				auto returnRuleTypeSymbol = returnRuleDefineSymbol->GetDescriptorSymbol();

				assert(returnRuleTypeSymbol->IsClassType());
				auto ruleTypeSymbol = FindType(manager, manager->GetGlobalSymbol(), ruleDefine->type.get());
				auto result = returnRuleTypeSymbol->EqualToBaseClassType(ruleTypeSymbol);
				if(!result)
				{
					throw ztl_exception(L"'!' return tree node need could convert to rule create node");
				}
				manager->CacheUsingGrammarToRuleDefSymbol(node, returnRuleDefineSymbol);
			}
			void								Visit(GeneralGrammarCreateTypeDefine* node)
			{

				CheckCreatNodeMoreTimeError();
				CheckCreatAndUsingInLoopError();

				createdTypeSymbol = FindType(manager, manager->GetGlobalSymbol(), node->type.get());
				auto&& ruleTypeSymbol = FindType(manager, manager->GetGlobalSymbol(), ruleDefine->type.get());
				auto result = createdTypeSymbol->EqualToBaseClassType(ruleTypeSymbol);
				if(!result)
				{
					throw ztl_exception(L"Can't convet grammar creat node type to rule node type");
				}
			}
			void								Visit(GeneralGrammarAlterationTypeDefine*)
			{
				assert(false);
			}

		private:
		};


		void ValidateGeneratePathStructure(SymbolManager * manager, unordered_map<GeneralRuleDefine*,vector<unique_ptr<GeneratePath>>>& pathMap)
		{
			for(auto&& mapIter : pathMap)
			{
				auto& ruleDef = mapIter.first;
				auto& paths = mapIter.second;
				for(auto&& pathIter : paths)
				{
					ValidateGeneratePathIVisitor visitor(manager, ruleDef);
					for(auto&& pathNodeIter : pathIter->GetPathNodeList())
					{
						if (pathNodeIter.IsLoopStart())
						{
							visitor.SetLoopFlag();
						}
						else if(pathNodeIter.IsLoopEnd())
						{
							visitor.ClearLoopFlag();
						}
						else
						{
							pathNodeIter.GetGrammarTypeDefine()->Accept(&visitor);
						}
					}
				}
			}
		}


		void LogGeneratePath(const wstring fileName, const unordered_map<GeneralRuleDefine*, vector<unique_ptr<GeneratePath>>>& pathMap)
		{
			wofstream output(fileName);
			assert(output.is_open());
			for(auto&& mapIter : pathMap)
			{
				auto& ruleDef = mapIter.first;
				auto& paths = mapIter.second;
				output << L"RuleName: " << ruleDef->name  << endl;
				for(auto&& pathIter : paths)
				{
					output << L"		Grammar: " << ruleDef->name  << endl;
					output << L"				";
					for(auto&& pathNodeIter : pathIter->GetPathNodeList())
					{
						output << pathNodeIter.GetTypeToWString() << L" : "<<pathNodeIter.GetDescriptor()<<L" ";
					}
					output << endl;
				}
			}
		}
		void ValidateGeneratorCoreSemantic(SymbolManager* manager)
		{
			CollectAndValidateTypeDefine(manager);
			ValidateGrammarNode(manager);
			auto&& pathMap = CollectGeneratePath(manager);
			ValidateGeneratePathStructure(manager, pathMap);
			GetStartSymbol(manager);
			manager->CacheNameAndTagMap();
			//LogGeneratePath(L"test.txt", pathMap);
		}
		class GetStartSymbolVisitor :public GeneralGrammarTypeDefine::IVisitor
		{
		private:
			SymbolManager*									manager;
			unordered_map<ParserSymbol*, bool>				rules;
			int												hitCount;
		public:
			GetStartSymbolVisitor()  = default;
			~GetStartSymbolVisitor() noexcept = default;
			GetStartSymbolVisitor(GetStartSymbolVisitor&&)  = default;
			GetStartSymbolVisitor(const GetStartSymbolVisitor&)  = default;
			GetStartSymbolVisitor& operator=(GetStartSymbolVisitor&&)  = default;
			GetStartSymbolVisitor& operator=(const GetStartSymbolVisitor&)   = default;
			GetStartSymbolVisitor(SymbolManager* _manager, unordered_map<ParserSymbol*, bool> _rules,int count) :manager(_manager),rules(_rules),hitCount(count)
			{
				
			}
		public:
			size_t GetHitCount()const
			{
				return hitCount;
			}
		public:
			void								Visit(GeneralGrammarTextTypeDefine*)
			{
				return;
			}
			void								Visit(GeneralGrammarNormalTypeDefine* node)
			{
				auto symbol = manager->GetCacheNormalGrammarToRuleDefSymbol(node);
				if(symbol !=nullptr && 
					symbol->IsRuleDef()&&
					rules.find(symbol)->second == false)
				{
					rules[symbol] = true;
					manager->GetStartRuleList().emplace_back(symbol->GetName());
					auto ruleNode = manager->GetCacheRuleDefineBySymbol(symbol);
					assert(ruleNode != nullptr);
					hitCount++;
					for (auto&& grammarIter : ruleNode->grammars)
					{
						grammarIter->Accept(this);
					}
				}
			}
			void								Visit(GeneralGrammarSequenceTypeDefine* node)
			{
				node->first->Accept(this);
				node->second->Accept(this);
			}
			void								Visit(GeneralGrammarLoopTypeDefine*node)
			{
				node->grammar->Accept(this);
			}
			void								Visit(GeneralGrammarOptionalTypeDefine*node)
			{
				node->grammar->Accept(this);
			}

			void								Visit(GeneralGrammarSetterTypeDefine* node)
			{
				node->grammar->Accept(this);

			}
			void								Visit(GeneralGrammarAssignTypeDefine* node)
			{
				node->grammar->Accept(this);

			}
			void								Visit(GeneralGrammarUsingTypeDefine* node)
			{
				node->grammar->Accept(this);

			}
			void								Visit(GeneralGrammarCreateTypeDefine* node)
			{
				node->grammar->Accept(this);


			}
			void								Visit(GeneralGrammarAlterationTypeDefine*node)
			{
				node->left->Accept(this);
				node->right->Accept(this);

			}

		
		};
		void GetStartSymbol(SymbolManager * manager)
		{
			auto&& table = manager->GetTable();
			unordered_map<ParserSymbol*, bool> rules;
			for(auto&& ruleIter : table->rules)
			{
				auto ruleSymbol = manager->GetCacheRuleNameToSymbol(ruleIter->name);
				rules[ruleSymbol] = false;
			}
			for(auto&& ruleIter : table->rules)
			{
				auto ruleSymbol = manager->GetCacheRuleNameToSymbol(ruleIter->name);
				rules[ruleSymbol] = true;
				manager->GetStartRuleList().emplace_back(ruleSymbol->GetName());
				GetStartSymbolVisitor visitor(manager,rules,1);
				rules[ruleSymbol] = false;
				for (auto&& grammarIter:ruleIter->grammars)
				{
					grammarIter->Accept(&visitor);
				}

				if(visitor.GetHitCount() != rules.size())
				{
					manager->GetStartRuleList().clear();
				}
			}
		}
	}
}