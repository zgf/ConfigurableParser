
			void ReflectionBuidler(const wstring& className,const wstring& fieldName,void* classObject,void* valueObject)
			{
				using buiderType = void(*)(void*,void*);
				static unordered_map<wstring, unordered_map<wstring, buiderType>> buidlerMap =
				{
					{
								L"GenGrammarAssignTypeDefine",
								{
									
									{
										L"name",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenGrammarAssignTypeDefine>(classObject)->name=static_cast<TokenInfo>(valueObject);
											return;
										}
									},
									{
										L"grammar",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenGrammarAssignTypeDefine>(classObject)->grammar=static_cast<GenGrammarTypeDefine>(valueObject);
											return;
										}
									},
								}
							},{
								L"GenClassMemberTypeDefine",
								{
									
									{
										L"type",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenClassMemberTypeDefine>(classObject)->type=static_cast<GenTypeObject>(valueObject);
											return;
										}
									},
									{
										L"name",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenClassMemberTypeDefine>(classObject)->name=static_cast<TokenInfo>(valueObject);
											return;
										}
									},
								}
							},{
								L"GenHeadInfoDefine",
								{
									
									{
										L"property",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenHeadInfoDefine>(classObject)->property=static_cast<TokenInfo>(valueObject);
											return;
										}
									},
									{
										L"value",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenHeadInfoDefine>(classObject)->value=static_cast<TokenInfo>(valueObject);
											return;
										}
									},
								}
							},{
								L"GenNormalTypeObject",
								{
									
									{
										L"name",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenNormalTypeObject>(classObject)->name=static_cast<TokenInfo>(valueObject);
											return;
										}
									},
								}
							},{
								L"GenGrammarCreateTypeDefine",
								{
									
									{
										L"type",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenGrammarCreateTypeDefine>(classObject)->type=static_cast<GenTypeObject>(valueObject);
											return;
										}
									},
									{
										L"grammar",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenGrammarCreateTypeDefine>(classObject)->grammar=static_cast<GenGrammarTypeDefine>(valueObject);
											return;
										}
									},
								}
							},{
								L"GenGrammarLoopTypeDefine",
								{
									
									{
										L"grammar",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenGrammarLoopTypeDefine>(classObject)->grammar=static_cast<GenGrammarTypeDefine>(valueObject);
											return;
										}
									},
								}
							},{
								L"GenTokenDefine",
								{
									
									{
										L"regex",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenTokenDefine>(classObject)->regex=static_cast<TokenInfo>(valueObject);
											return;
										}
									},
									{
										L"name",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenTokenDefine>(classObject)->name=static_cast<TokenInfo>(valueObject);
											return;
										}
									},
									{
										L"ignore",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenTokenDefine>(classObject)->ignore=static_cast<TokenOptional>(valueObject);
											return;
										}
									},
								}
							},{
								L"GenGrammarSequenceTypeDefine",
								{
									
									{
										L"first",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenGrammarSequenceTypeDefine>(classObject)->first=static_cast<GenGrammarTypeDefine>(valueObject);
											return;
										}
									},
									{
										L"second",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenGrammarSequenceTypeDefine>(classObject)->second=static_cast<GenGrammarTypeDefine>(valueObject);
											return;
										}
									},
								}
							},{
								L"GenTypeDefine",
								{
									
								}
							},{
								L"GenTypeObject",
								{
									
								}
							},{
								L"GenArrayTypeObject",
								{
									
									{
										L"element",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenArrayTypeObject>(classObject)->element=static_cast<GenTypeObject>(valueObject);
											return;
										}
									},
								}
							},{
								L"GenStringTypeObject",
								{
									
								}
							},{
								L"GenSubTypeObject",
								{
									
									{
										L"parent",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenSubTypeObject>(classObject)->parent=static_cast<GenTypeObject>(valueObject);
											return;
										}
									},
									{
										L"name",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenSubTypeObject>(classObject)->name=static_cast<TokenInfo>(valueObject);
											return;
										}
									},
								}
							},{
								L"GenEnumMemberTypeDefine",
								{
									
									{
										L"name",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenEnumMemberTypeDefine>(classObject)->name=static_cast<TokenInfo>(valueObject);
											return;
										}
									},
								}
							},{
								L"GenClassTypeDefine",
								{
									
									{
										L"name",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenClassTypeDefine>(classObject)->name=static_cast<TokenInfo>(valueObject);
											return;
										}
									},
									{
										L"subTypes",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenClassTypeDefine>(classObject)->subTypes.emplace_back(static_cast<GenTypeDefine>(valueObject));
											return;
										}
									},
									{
										L"parent",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenClassTypeDefine>(classObject)->parent=static_cast<GenTypeObject>(valueObject);
											return;
										}
									},
									{
										L"members",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenClassTypeDefine>(classObject)->members.emplace_back(static_cast<GenClassMemberTypeDefine>(valueObject));
											return;
										}
									},
								}
							},{
								L"GenEnumTypeDefine",
								{
									
									{
										L"name",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenEnumTypeDefine>(classObject)->name=static_cast<TokenInfo>(valueObject);
											return;
										}
									},
									{
										L"members",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenEnumTypeDefine>(classObject)->members.emplace_back(static_cast<GenEnumMemberTypeDefine>(valueObject));
											return;
										}
									},
								}
							},{
								L"GenGrammarTypeDefine",
								{
									
								}
							},{
								L"GenGrammarTextTypeDefine",
								{
									
									{
										L"text",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenGrammarTextTypeDefine>(classObject)->text=static_cast<TokenInfo>(valueObject);
											return;
										}
									},
								}
							},{
								L"GenGrammarNormalTypeDefine",
								{
									
									{
										L"name",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenGrammarNormalTypeDefine>(classObject)->name=static_cast<TokenInfo>(valueObject);
											return;
										}
									},
								}
							},{
								L"GenGrammarOptionalTypeDefine",
								{
									
									{
										L"grammar",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenGrammarOptionalTypeDefine>(classObject)->grammar=static_cast<GenGrammarTypeDefine>(valueObject);
											return;
										}
									},
								}
							},{
								L"GenGrammarSetterTypeDefine",
								{
									
									{
										L"grammar",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenGrammarSetterTypeDefine>(classObject)->grammar=static_cast<GenGrammarTypeDefine>(valueObject);
											return;
										}
									},
									{
										L"name",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenGrammarSetterTypeDefine>(classObject)->name=static_cast<TokenInfo>(valueObject);
											return;
										}
									},
									{
										L"value",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenGrammarSetterTypeDefine>(classObject)->value=static_cast<TokenInfo>(valueObject);
											return;
										}
									},
								}
							},{
								L"GenGrammarAdditionTypeDefine",
								{
									
									{
										L"grammar",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenGrammarAdditionTypeDefine>(classObject)->grammar=static_cast<GenGrammarTypeDefine>(valueObject);
											return;
										}
									},
									{
										L"setters",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenGrammarAdditionTypeDefine>(classObject)->setters.emplace_back(static_cast<GenGrammarSetterTypeDefine>(valueObject));
											return;
										}
									},
								}
							},{
								L"GenGrammarUsingTypeDefine",
								{
									
									{
										L"name",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenGrammarUsingTypeDefine>(classObject)->name=static_cast<TokenInfo>(valueObject);
											return;
										}
									},
									{
										L"grammar",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenGrammarUsingTypeDefine>(classObject)->grammar=static_cast<GenGrammarTypeDefine>(valueObject);
											return;
										}
									},
								}
							},{
								L"GenGrammarAlternationTypeDefine",
								{
									
									{
										L"left",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenGrammarAlternationTypeDefine>(classObject)->left=static_cast<GenGrammarTypeDefine>(valueObject);
											return;
										}
									},
									{
										L"right",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenGrammarAlternationTypeDefine>(classObject)->right=static_cast<GenGrammarTypeDefine>(valueObject);
											return;
										}
									},
								}
							},{
								L"GenRuleDefine",
								{
									
									{
										L"type",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenRuleDefine>(classObject)->type=static_cast<GenTypeObject>(valueObject);
											return;
										}
									},
									{
										L"name",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenRuleDefine>(classObject)->name=static_cast<TokenInfo>(valueObject);
											return;
										}
									},
									{
										L"grammars",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenRuleDefine>(classObject)->grammars.emplace_back(static_cast<GenGrammarTypeDefine>(valueObject));
											return;
										}
									},
								}
							},{
								L"GenTableDefine",
								{
									
									{
										L"heads",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenTableDefine>(classObject)->heads.emplace_back(static_cast<GenHeadInfoDefine>(valueObject));
											return;
										}
									},
									{
										L"types",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenTableDefine>(classObject)->types.emplace_back(static_cast<GenTypeDefine>(valueObject));
											return;
										}
									},
									{
										L"tokens",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenTableDefine>(classObject)->tokens.emplace_back(static_cast<GenTokenDefine>(valueObject));
											return;
										}
									},
									{
										L"rules",
										[](void* classObject,void* valueObject)
										{
											static_cast<GenTableDefine>(classObject)->rules.emplace_back(static_cast<GenRuleDefine>(valueObject));
											return;
										}
									},
								}
							},
				};
				auto findIter = builderMap.find(className);
				assert(findIter!=builderMap.end());
				assert(findIter->second.find(fieldName)!=findIter->second.end());
				builderMap[className][fieldName](classObject,valueObject);
			}
			shared_ptr<void> ReflecteObjectByName(const wstring& name)
			{
				using lambdaType = shared_ptr<void>(*)();
				static unordered_map < wstring, lambdaType> objectMap =
				{
					
				{
					L"GenGrammarAssignTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GenGrammarAssignTypeDefine>();
					}
				},
				{
					L"GenClassMemberTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GenClassMemberTypeDefine>();
					}
				},
				{
					L"GenHeadInfoDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GenHeadInfoDefine>();
					}
				},
				{
					L"GenNormalTypeObject",
					[]()->shared_ptr<void>
					{
						return make_shared<GenNormalTypeObject>();
					}
				},
				{
					L"GenGrammarCreateTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GenGrammarCreateTypeDefine>();
					}
				},
				{
					L"GenGrammarLoopTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GenGrammarLoopTypeDefine>();
					}
				},
				{
					L"GenTokenDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GenTokenDefine>();
					}
				},
				{
					L"GenGrammarSequenceTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GenGrammarSequenceTypeDefine>();
					}
				},
				{
					L"GenTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GenTypeDefine>();
					}
				},
				{
					L"GenTypeObject",
					[]()->shared_ptr<void>
					{
						return make_shared<GenTypeObject>();
					}
				},
				{
					L"GenArrayTypeObject",
					[]()->shared_ptr<void>
					{
						return make_shared<GenArrayTypeObject>();
					}
				},
				{
					L"GenStringTypeObject",
					[]()->shared_ptr<void>
					{
						return make_shared<GenStringTypeObject>();
					}
				},
				{
					L"GenSubTypeObject",
					[]()->shared_ptr<void>
					{
						return make_shared<GenSubTypeObject>();
					}
				},
				{
					L"GenEnumMemberTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GenEnumMemberTypeDefine>();
					}
				},
				{
					L"GenClassTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GenClassTypeDefine>();
					}
				},
				{
					L"GenEnumTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GenEnumTypeDefine>();
					}
				},
				{
					L"GenGrammarTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GenGrammarTypeDefine>();
					}
				},
				{
					L"GenGrammarTextTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GenGrammarTextTypeDefine>();
					}
				},
				{
					L"GenGrammarNormalTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GenGrammarNormalTypeDefine>();
					}
				},
				{
					L"GenGrammarOptionalTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GenGrammarOptionalTypeDefine>();
					}
				},
				{
					L"GenGrammarSetterTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GenGrammarSetterTypeDefine>();
					}
				},
				{
					L"GenGrammarAdditionTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GenGrammarAdditionTypeDefine>();
					}
				},
				{
					L"GenGrammarUsingTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GenGrammarUsingTypeDefine>();
					}
				},
				{
					L"GenGrammarAlternationTypeDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GenGrammarAlternationTypeDefine>();
					}
				},
				{
					L"GenRuleDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GenRuleDefine>();
					}
				},
				{
					L"GenTableDefine",
					[]()->shared_ptr<void>
					{
						return make_shared<GenTableDefine>();
					}
				},
				};
				assert(objectMap.find(name) != objectMap.end());
				return objectMap[name]();
			}