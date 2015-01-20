#pragma once
namespace ztl
{
	namespace pair_builder
	{
#define PAIR_BUILDER(PAIR_NAME,KEY1_TYPE_NAME,KEY1_NAME,KEY2_TYPE_NAME,KEY2_NAME)\
		struct PAIR_NAME\
		{\
			KEY1_TYPE_NAME		KEY1_NAME;\
			KEY2_TYPE_NAME		KEY2_NAME;\
			PAIR_NAME()\
			{\
			}\
			\
			PAIR_NAME(KEY1_TYPE_NAME _##KEY1_NAME, KEY2_TYPE_NAME _##KEY2_NAME)\
				:KEY1_NAME(_##KEY1_NAME)\
				, KEY2_NAME(_##KEY2_NAME)\
			{\
			}\
			int Compare(const PAIR_NAME& pair)\
			{\
				if(KEY1_NAME<pair.KEY1_NAME) return -1;\
				if(KEY1_NAME>pair.KEY1_NAME) return 1;\
				if(KEY2_NAME<pair.KEY2_NAME) return -1;\
				if(KEY2_NAME>pair.KEY2_NAME) return 1;\
				return 0;\
			}\
			bool operator==(const PAIR_NAME& pair)\
			{\
				return Compare(pair) == 0;\
			}\
			bool operator!=(const PAIR_NAME& pair)\
			{\
				return Compare(pair) != 0;\
			}\
			bool operator>(const PAIR_NAME& pair)\
			{\
				return Compare(pair)>0;\
			}\
			bool operator>=(const PAIR_NAME& pair)\
			{\
				return Compare(pair) >= 0;\
			}\
			bool operator<(const PAIR_NAME& pair)\
			{\
				return Compare(pair)<0;\
			}\
			bool operator<=(const PAIR_NAME& pair)\
			{\
				return Compare(pair) <= 0;\
			}\
		};
	}
}