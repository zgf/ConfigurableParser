namespace ztl
{
	namespace general_parser
	{
		class GeneralRuleDefine;
		class GeneratePath;
		void LogGeneratePath(const wstring fileName, const unordered_map<GeneralRuleDefine*, vector<unique_ptr<GeneratePath>>>& pathMap)
	}
}