#pragma once
#include <xstddef>
#include <memory>
#include <type_traits>
#include <numeric>
#include <unordered_set>
namespace ztl
{
	namespace hasher
	{

		// TEMPLATE STRUCT hash
		template<class _Kty>
		struct ztl_hash
		{	// hash functor for enums
			
		};
		template<typename _Kty>
		struct ztl_hash<std::shared_ptr<_Kty>>
		{	// hash functor for enums
			size_t operator()(const std::shared_ptr<_Kty>& _Keyval) const
			{	// hash _Keyval to size_t value by pseudorandomizing transform
				return std::_Hash_seq((const unsigned char *) &_Keyval, sizeof(ptrdiff_t));
			}
		};
		template<typename _Kty>
		struct ztl_hash<std::unordered_set<_Kty*>>
		{	// hash functor for enums
			size_t operator()(const std::unordered_set<_Kty*>& target) const
			{	// hash _Keyval to size_t value by pseudorandomizing transform
				return 	std::accumulate(target.begin(), target.end(), 0, [](size_t sum, _Kty*val)
				{
					return sum + std::_Hash_seq((const unsigned char *) &val, sizeof(_Kty*));
				});
			}
		};
		template<template<typename,typename> class pair_type,typename pointer1_type,typename pointer2_type>
		struct ztl_hash<pair_type<pointer1_type*, pointer2_type*>>
		{
			size_t operator()(const pair_type<pointer1_type*, pointer2_type*>& _Keyval) const
			{	// hash _Keyval to size_t value by pseudorandomizing transform
				return std::_Hash_seq((const unsigned char *) &_Keyval, sizeof(pair_type<pointer1_type*, pointer2_type*>));
			}
		};
		template<template<typename, typename> class pair_type, typename pointer1_type, typename pointer2_type>
		struct ztl_hash<pair_type<pointer1_type, pointer2_type>>
		{
			size_t operator()(const pair_type<pointer1_type, pointer2_type>& _Keyval) const
			{	// hash _Keyval to size_t value by pseudorandomizing transform
				return std::_Hash_seq((const unsigned char *) &_Keyval, sizeof(pair_type<pointer1_type, pointer2_type>));
			}
		};
	}
}