
			int WstringToEnumItem(const wstring& value)
			{
				static unordered_map<wstring, int> signMap =
				{
					
				{
					L"Literal::Value::True",
					static_cast<int>(Literal::Value::True)
				},
				{
					L"Literal::Value::False",
					static_cast<int>(Literal::Value::False)
				},
				{
					L"Literal::Value::Null",
					static_cast<int>(Literal::Value::Null)
				},
				};
				assert(signMap.find(value)!=signMap.end());
				return signMap[value];
			}
			void ReflectionBuidler(const wstring& className,const wstring& fieldName,void* classObject,void* valueObject)
			{
				using builderType = void(*)(void*,void*);
				static unordered_map<wstring, unordered_map<wstring, builderType>> builderMap =
				{
					{
								L"Node",
								{
									
								}
							},{
								L"Number",
								{
									{
										L"content",
										[](void* classObject,void* valueObject)
										{
											static_cast<Number*>(classObject)->content=static_cast<TokenInfo*>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"Literal",
								{
									{
										L"value",
										[](void* classObject,void* valueObject)
										{
											static_cast<Literal*>(classObject)->value=static_cast<Literal::Value>(WstringToEnumItem(L"Literal::Value::" + static_cast<TokenInfo*>(valueObject)->content));
											return;
										}
									},
								}
							},{
								L"Object",
								{
									{
										L"fields",
										[](void* classObject,void* valueObject)
										{
											static_cast<Object*>(classObject)->fields.emplace_back(static_cast<ObjectField*>(valueObject));
											return;
										}
									},
								}
							},{
								L"String",
								{
									{
										L"content",
										[](void* classObject,void* valueObject)
										{
											static_cast<String*>(classObject)->content=static_cast<TokenInfo*>(valueObject)->content;
											return;
										}
									},
								}
							},{
								L"Array",
								{
									{
										L"items",
										[](void* classObject,void* valueObject)
										{
											static_cast<Array*>(classObject)->items.emplace_back(static_cast<Node*>(valueObject));
											return;
										}
									},
								}
							},{
								L"ObjectField",
								{
									{
										L"name",
										[](void* classObject,void* valueObject)
										{
											static_cast<ObjectField*>(classObject)->name=static_cast<TokenInfo*>(valueObject)->content;
											return;
										}
									},{
										L"value",
										[](void* classObject,void* valueObject)
										{
											static_cast<ObjectField*>(classObject)->value.reset(static_cast<Node*>(valueObject));
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
				using reflectObjectType = shared_ptr<void>(*)();
				static unordered_map < wstring, reflectObjectType> objectMap =
				{
					
				{
					L"Node",
					[]()->shared_ptr<void>
					{
						return make_shared<Node>();
					}
				},
				{
					L"Number",
					[]()->shared_ptr<void>
					{
						return make_shared<Number>();
					}
				},
				{
					L"Literal",
					[]()->shared_ptr<void>
					{
						return make_shared<Literal>();
					}
				},
				{
					L"Object",
					[]()->shared_ptr<void>
					{
						return make_shared<Object>();
					}
				},
				{
					L"String",
					[]()->shared_ptr<void>
					{
						return make_shared<String>();
					}
				},
				{
					L"Array",
					[]()->shared_ptr<void>
					{
						return make_shared<Array>();
					}
				},
				{
					L"ObjectField",
					[]()->shared_ptr<void>
					{
						return make_shared<ObjectField>();
					}
				},
				};
				assert(objectMap.find(name) != objectMap.end());
				return objectMap[name]();
			}