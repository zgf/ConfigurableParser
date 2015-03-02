#include "Include/stdafx.h"
#include "Include/SymbolManager.h"
#include "Include/ParserSymbol.h"
namespace ztl
{
	namespace general_parser
	{
		//查找typeobject的类型是否在当前作用域和上层定义域中已经定义了,
		//将定义和typeobject,发现typeobject的作用域映射起来
		//原因是typeobject可能指向同一份类型定义.所以typeobject
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
				//检查XX的类型名是不是已经有定义存在了,如果不存在说明声明不对- -
				auto typeSymbol = manager->CheckNameHasDefine(node->name, scope);
				if(!typeSymbol)
				{
					throw ztl_exception(L"type" + node->name + L"Can't be define in front");
				}
				result = typeSymbol;
			}
			//对于子类型结构 A.B.C.D 的类型查找要注意查找作用域的更改.findType第一次遇到的是D,sope是出现A.B.C.D这个对象的作用域X.
			//然后去还是A.B.C这个子类型结构,findType还是subType,直到找到A,A是classType.获得A的Symbol后,return出去.
			//回到B,这时候作用域还是X.显然B在X内找不到,但是在A内找的到,所以findType subSymbol的时候应该去parent作用域内查找
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
		//根据语法树节点,构造符号表节点.
		//具有类型的标示符叫变量
		//类型的定义和类型的对象
		//作用:
		//	1.验证语义分析的错误:
		//		1.作用域内变量重定义问题.当前子符号表内是否已存在定义
		//		2.当前标示符(type)是否已经被定义的问题;作用域层层查找.类继承链层层查找.
		//		3.regex不能存在同样的多个实体不同tokenName
		//缓存各个类型对象到类型定义的映射
		//(typeobject*,scope)->classDef/enumDef
		//typeobjec可能同名,但是不可能是同个实例(地址不同),我觉得这里没必要存在scope,所以用 typeobject*->typeDef
		//对于规则的类型,是已typeobejct对象保存的,通过对typeobjectName,scope的查找就可以搞定了
		//缓存(className,scope)->classDefSymbol,这个因为语法问题不需要了,文法中出现的className要么在classDef中,要么包含在typeobject对象中.所以typeobject->typeDef已经缓存了.
		//缓存regexName->tokenDefSymbol
		//缓存tokenName->tokenDefSymbol
		//缓存ruleName->ruleDefSymbol
		//缓存GrmmarDef语法树节点->classDefSybmol或ruleDefSymbol
		//  2.根据语法树节点,构造符号表的符号树型结构和符号缓存结构. ParserSymbol*组成了符号树形结构.

		//  3.验证符号序列是否满足语义要求.语法树结构是否满足语义要求.

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
				//检测rule的type是不是先前已定义过,缓存rule节点与符号表节点的绑定
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

		//验证规则的结构是否满足语义要求
		//例如NormalGammar必须是一个token或者regex或者ruleName
		//TextGrammar必须是一个regex.
		//CreatGrammar不能在循环LoopGrammar内部
		//一条路径只有有一个CreatGammar
		//CreatGrammar生成的必须是classType节点.CreatGrammar的type必须存在
		//UsingGrammar包含的grammar只能是NormalGramar并且NormalGrammar包含的是ruleName.
		//UsingGrammar 路径上只有有Using和NormalGrammar.两个语法树节点
		//AssignGrammar的grammar必须是Normal并且是规则名
		//并且grammar必须是NormalGrammar 而且NormalGrammar必须是ruleName.
		//className 可以是A 可以是A.B形式
		//缓存Normal节点和text到Symbol的映射
		//替换regex为对应的tokenName

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


		//生成路径设计:
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
			
			//节点对应的文法
			//assgin只能是classtype和tokentype
			//setter只能是tokentype和enumtype 对于set value可以是string. tokenType直接就可以获得值了.enumType可以从缓存拿到字段type然后搜索子符号得到enumDefType和name
			//对于assign.只能是classType或tokenType和rule返回节点.返回节点没法知道,但是返回节点的类型可以拿到.返回节点value类型从规则名就可以知道,所以需要一个wstring的normalGrammar的name
			//字段类型可以从缓存得到.
			//tokenType的值是wstring.所以应该用wstring
			//这里可以获得字段名和{字段值} setter 字段值是wstring value.assign的value是grammar返回的节点.可以从
			//对于assgin文法.字段类型只能是class和token的type.赋值的value,得不到,是语法树返回节点,从ruleName可以知道,ruleName到ruleDef已经缓存了.
			//对于setter文法.字段类型只能是enum和token的type.filedSymbol已经被缓存,类型也就被缓存了.value的值是string获得enum值,enum值可以从enumType的subSymbol获得.
			//对于using文法.直接存ruleName就OK
			//对于created.parserSymbol存在path结构内.
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
		//对rule下的每一条grammar收集路径
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

		//生成路径验证
		//一个路径上只有一个creat Node
		//对于非数组的成员,多次赋值给警告.
		//对于没有对Node所有成员赋值的,给出警告
		//路径验证 SetterGrammar要求name是CreatGrammar语法中指定的class的成员.SetterGrammer要求有name是某个class的成员名value要兼容成员的类型
		//如果成员是TokenType,要么value肯定匹配
		//Setter成员类型只能是token和enum
		//Loop路径内的setter和assign的name必须是数组类型 子类型兼容.
		//路径验证 Assign要求grammar的type和name的type兼容,name是CreatGrammar指定的class的成员名.
		//CraetGrammar一条路径上只能有一个
		//CreatGrammar要求能转换为Rule声明的类型
		//Using的返回的语法树节点应该可以转换到rule的type节点
		//路径上只要有Using就不需要Creat了
		//缓存GrammarNode到classFieldSymbol的映射
		//Setter必须在as或者!后面才可以使用
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
				//value 类型
				auto ruleTypeSymbol = GetRuleSymbolByNormalGrammarParent(node->grammar.get())->GetDescriptorSymbol();

				//class 成员类型
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
				//在类型定义收集与验证中检测过了.
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