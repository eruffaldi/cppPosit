/**
 * Emanuele Ruffaldi (C) 2017
 *
 * My personal bit hip pop using BMI extensions and a bit of constexpr!
 */
#pragma once

#include <bitset>
#include <stdint.h>
#include <type_traits>

#ifndef __arm__
#include "x86intrin.h"
#endif

#ifndef CONSTEXPR14
#if __cplusplus >= 201402L
#define CONSTEXPR14 constexpr
#else
#define CONSTEXPR14
#endif
#endif

// C version
#define BIT_MASK(__TYPE__, __ONE_COUNT__) \
    ((__TYPE__) (-((__ONE_COUNT__) != 0))) \
    & (((__TYPE__) -1) >> ((sizeof(__TYPE__) * 8) - (__ONE_COUNT__)))

template <typename R>
constexpr R bitmask(unsigned int const onecount)
{
    return static_cast<R>(-(onecount != 0))
        & (static_cast<R>(-1) >> ((sizeof(R) * 8) - onecount));
}

template<typename T> 
constexpr typename std::remove_reference<T>::type makeprval(T && t) 
{
    return t;
}

#define isprvalconstexpr(e) noexcept(makeprval(e))

// __builtin_clzll
constexpr inline uint64_t __builtin_clz64(uint64_t v) 
{
	return (v >> 32 != 0 ? __builtin_clz(v>>32) : 32 + __builtin_clz(v));
}

// this is constexpr, others not
// never call with input==0
constexpr inline int findbitleftmostC(uint64_t input)
{
	return __builtin_clz64(input);
}

// this is constexpr, others not
// never call with input==0
constexpr inline int findbitleftmostC(uint32_t input)
{
	return __builtin_clz(input);
}


// this is constexpr, others not
// never call with input==0
constexpr inline int findbitleftmostC(uint16_t input)
{
	return __builtin_clz((uint32_t)input)-16;
}

// this is constexpr, others not
// never call with input==0
constexpr inline int findbitleftmostC(uint8_t input)
{
	return __builtin_clz((uint32_t)input)-24;
}

#ifndef __arm__
inline int findbitleftmost(uint8_t input)
{
	return  __lzcnt16(input)-8;
}

 inline int findbitleftmost(uint16_t input)
{
	return  __lzcnt16(input);
}

 inline int findbitleftmost(uint32_t input)
{
	return  __lzcnt32(input);
}


 inline int findbitleftmost(uint64_t input)
{
	return  __lzcnt64(input);
}

// detect constexpr for X so we can speedup
#define findbitleftmost(X) (isprvalconstexpr(X)?findbitleftmostC(X):findbitleftmost(X))
#endif

// indices are with 0 on the right
template <class T, class Y, int offset, int size>
constexpr T bitset_part(T input, Y value)
{
	return (input & ~bitmask<T>(size))|  ((value&bitmask<T>(size))<<offset);
}


template <class T, class Y>
CONSTEXPR14 T bitset_part(T input, Y value, int offset, int size)
{
	auto M = bitmask<T>(size);
	return (input & ~M)|  ((value&M)<<offset);
}

template <class T, int offset, int size>
constexpr T bitset_get(T input)
{
	return (input >> offset) & bitmask<T>(size);
}

template <class T>
CONSTEXPR14 T bitset_get(T input, int offset, int size)
{
	auto M = bitmask<T>(size);
	return (input >> offset) & M;
}
#ifndef __arm__
inline uint64_t bitset_gethw(uint64_t input, int offset, int size)
{
	return _bextr_u64(input, offset, size);
}

inline  uint16_t bitset_gethw(uint16_t input, int offset, int size)
{
	return _bextr_u32(input, offset, size);
}

inline  uint32_t bitset_gethw(uint32_t input, int offset, int size)
{
	return _bextr_u32(input, offset, size);
}

template <int offset, int size>
uint64_t bitset_gethwT(uint64_t input)
{
	return _bextr_u64(input, offset, size);
}

template <int offset, int size>
uint32_t bitset_gethwT(uint32_t input)
{
	return _bextr_u32(input, offset, size);
}

template <int offset, int size>
uint16_t bitset_gethwT(uint16_t input)
{
	return _bextr_u32(input, offset, size);
}

template <int offset, int size>
uint8_t bitset_gethwT(uint8_t input)
{
	return _bextr_u32(input, offset, size);
}

// with template
#define bitset_getT(X,A,B) (isprvalconstexpr(X)?bitset_gethwT<A,B>(X):bitset_get<decltype(X),A,B>(X))

// with flexible arguments
#define bitset_get(X,A,B) (isprvalconstexpr(X)&&isprvalconstexpr(A)&&isprvalconstexpr(B)?bitset_gethw(X,A,B):bitset_get(X,A,B))
#else

#define bitset_getT(X,A,B) (bitset_get<decltype(X),A,B>(X))

#endif
template <class T, int N>
struct bitset_leftmost_get_const
{
	constexpr T operator()(T X) const
	{
		return (T)bitset_getT((typename std::make_unsigned<T>::type)(X),sizeof(X)*8-N,N);
	}
};

template <class T>
struct bitset_leftmost_get_const<T,0>
{
	constexpr T operator()(T X) const
	{
		return 0;
	}
};

#define bitset_leftmost_getT(X,N) bitset_getT(X,sizeof(X)*8-N,N)

template <class T>
constexpr std::bitset<sizeof(T)*8> bitset_all(const T x)
{
	return std::bitset<sizeof(T)*8>(x);
}


template <class T, int N>
constexpr std::bitset<N> bitset_msb(const T x)
{
	return std::bitset<N>(x >> (sizeof(T)*8-N));
}

template <class T, int N>
constexpr std::bitset<N> bitset_lsb(const T x)
{
	return std::bitset<N>(x);
}

/// absolute value of signed integer with conditions
template <class T>
constexpr T pcabs(T x)
{
	return x < 0 ? -x : x;
}

/// 
/// absolute value of signed integer without conditions
template <class T> // ,typename std::enable_if<std::is_integral<T>::value ,int>::type* = nullptr>
CONSTEXPR14 T pabs(T x)
{
	T mask = (x >> (sizeof(T) * 8 - 1));
	return (x + mask) ^ mask;
}

// support
template <class A, int abits, class B, int  bbits, bool abits_gt_bbits>
struct cast_msb_
{
};

// support
template <class A, int abits, class B, int  bbits>
struct cast_msb_<A,abits,B,bbits,true>
{

	constexpr B operator()(A value) const
	{
		// #A > #B
        return value >> (abits-bbits);
	}
};

// support
// #B >= #A
template <class A, int abits, class B, int  bbits>
struct cast_msb_<A,abits,B,bbits,false>
{
	constexpr B operator()(A value) const
	{
        return ((B)value) << (bbits-abits);
	}
};


/// MSB aligned data cast from type A to type B
template <class A, int abits, class B, int  bbits>
struct cast_msb: public cast_msb_<A,abits,B,bbits,(abits > bbits)>
{
	static_assert(std::is_unsigned<A>::value,"required unsigned A");
	static_assert(std::is_unsigned<B>::value,"required unsigned B");
};







// support
template <class A, int abits, class B, int  bbits, bool abits_gt_bbits>
struct cast_right_to_left_
{
};

// support
template <class A, int abits, class B, int  bbits>
struct cast_right_to_left_<A,abits,B,bbits,true>
{

	constexpr B operator()(A value) const
	{
		// #A > #B
        return bitset_getT(value,0,abits) >> (abits-bbits);
	}
};

// support
// #B >= #A
template <class A, int abits, class B, int  bbits>
struct cast_right_to_left_<A,abits,B,bbits,false>
{
	constexpr B operator()(A value) const
	{
        return ((B)bitset_getT(value,0,abits)) << (bbits-abits);
	}
};


/// MSB aligned data cast from type A to type B
template <class A, int abits, class B, int  bbits>
struct cast_right_to_left: public cast_right_to_left_<A,abits,B,bbits,(abits > bbits)>
{
	static_assert(std::is_unsigned<A>::value,"required unsigned A");
	static_assert(std::is_unsigned<B>::value,"required unsigned B");
};
