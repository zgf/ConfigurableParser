
							namespace XML
							{
								
							namespace test
							{
								
			struct GeneralTokenInfo
			{
				wstring name;
				wstring regex;
				bool    ignore;
				GeneralTokenInfo() = default;
				GeneralTokenInfo(const wstring& _name, const wstring& _regex, const bool _ignore)
					: name(_name), regex(_regex), ignore(_ignore)
				{
				}
			};
			vector<TokenInfo> Parse(const wstring& fileName)
			{
				struct GeneralTokenInfo
				{
					wstring name;
					wstring regex;
					bool    ignore;
					GeneralTokenInfo() = default;
					GeneralTokenInfo(const wstring& _name, const wstring& _regex, const bool _ignore)
						: name(_name), regex(_regex), ignore(_ignore)
					{
					}
				};
				unordered_map<wstring, GeneralTokenInfo> infos;
				infos.insert({CLOSE,GeneralTokenInfo(CLOSE, \}, 0)});
infos.insert({RULE,GeneralTokenInfo(RULE, rule, 0)});
infos.insert({CLASS,GeneralTokenInfo(CLASS, class, 0)});
infos.insert({TOKEN,GeneralTokenInfo(TOKEN, token, 0)});
infos.insert({ENUM,GeneralTokenInfo(ENUM, enum, 0)});
infos.insert({DISCARDTOKEN,GeneralTokenInfo(DISCARDTOKEN, ignoretoken, 0)});
infos.insert({WITH,GeneralTokenInfo(WITH, with, 0)});
infos.insert({AS,GeneralTokenInfo(AS, as, 0)});
infos.insert({SPACE,GeneralTokenInfo(SPACE, \s+, 1)});
infos.insert({OPEN,GeneralTokenInfo(OPEN, \{, 0)});
infos.insert({SEMICOLON,GeneralTokenInfo(SEMICOLON, ;, 0)});
infos.insert({ASSIGN,GeneralTokenInfo(ASSIGN, =, 0)});
infos.insert({COLON,GeneralTokenInfo(COLON, :, 0)});
infos.insert({COMMA,GeneralTokenInfo(COMMA, ,, 0)});
infos.insert({DOT,GeneralTokenInfo(DOT, \., 0)});
infos.insert({USING,GeneralTokenInfo(USING, !, 0)});
infos.insert({OR,GeneralTokenInfo(OR, \|, 0)});
infos.insert({OPTOPEN,GeneralTokenInfo(OPTOPEN, \[, 0)});
infos.insert({STRING,GeneralTokenInfo(STRING, \"(\\"|[^"])*", 0)});
infos.insert({OPTCLOSE,GeneralTokenInfo(OPTCLOSE, \], 0)});
infos.insert({PREOPEN,GeneralTokenInfo(PREOPEN, \(, 0)});
infos.insert({PRECLOSE,GeneralTokenInfo(PRECLOSE, \), 0)});
infos.insert({NAME,GeneralTokenInfo(NAME, [a-zA-Z_]\w*, 0)});
infos.insert({FINISH,GeneralTokenInfo(FINISH, <\$>, 0)});
infos.insert({LINENOTE,GeneralTokenInfo(LINENOTE, (//[^
]*
), 1)});
infos.insert({BLOCKNOTE,GeneralTokenInfo(BLOCKNOTE, /*.*?*/ //, 1)});

				wifstream input(fileName);
				if(!input.is_open())
				{
					throw ztl_exception(L"error:file" + fileName + L"not open!");
				}
				vector<TokenInfo> stream;
				std::wstring content((std::istreambuf_iterator<wchar_t>(input)),
					std::istreambuf_iterator<wchar_t>());
				auto pattern = accumulate(infos.begin(), infos.end(), wstring(), 
				[](const wstring& sum, const pair<const wstring, GeneralTokenInfo>& token)
				{
					return sum + L"(?<" + token.second.name + L">" + token.second.regex + L")|";
				});
				pattern.pop_back();
				RegexInterpretor interpretor(pattern);
				auto result = interpretor.Matches(content);
				for(auto&& iter : result)
				{
					assert(iter.group.size() == 1);
					auto groupIter = *iter.group.begin();
					auto name = groupIter.first;
					assert(infos.find(name) != infos.end());
					if(!infos[name].ignore)
					{
						stream.emplace_back(name, groupIter.second.content, groupIter.second.position, groupIter.second.length);
					}
				}
				return stream;
			}
			
							}
						 
							}
						 