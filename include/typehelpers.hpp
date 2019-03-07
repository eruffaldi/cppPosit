/**
 * Copyright (C) 2017-2019 Emanuele Ruffaldi
 * Distributed under the terms of the BSD 3-Clause License.  
 * 
 * (See accompanying file LICENSE)
 * 
 * --
 */
/**
 * Various Integr Type Helpers
 *
 * Emanuele Ruffaldi 2017
 */
#pragma once
#include <cstdint>
#include <inttypes.h>
#include "bithippop.hpp"

#if ((defined(__llvm__) && __clang_major__ > 3) || (defined(__GNUC__) && __GNUC__ >= 4) || defined(__INTEL_COMPILER)) && defined(__SIZEOF_INT128__)
typedef int signed128 __attribute__((mode(TI)));
typedef unsigned unsigned128 __attribute__((mode(TI)));
#define HAS128T
#else

#ifdef _MSVC_VER
#pragma intrinsic (__shiftleft128, __shiftright128)
#endif

#if 0
struct signed128
{
	int64_t hi,lo;
	signed128(int64_t x): lo(x), hi(x < 0 ? -1: 0)
	{

	}
	operator int64_t () const {return lo; }

	signed128 operator << (int n) const 
	{
		signed128 r;
		if(n >= 64)
		{
			r.lo = 0;
			r.hi = lo << (n-64);
		}
		else
		{
			r.lo = lo << n;
			r.hi = __shiftleft128(hi,lo,n);
		}
		return r;
	}

	signed128 operator >> (int n) const 
	{
		signed128 r;
		if(n >= 64)
		{
			r.hi = r.hi < 0 ? -1 : 0;
			r.lo = r.hi >> (n-64);
		}
		else
		{
			r.hi = hi >> n;
			r.lo = __shiftright128(hi,lo,n);
		}
		return r;
	}

};
#endif

/// not implemented because we do not need it yet
struct signed128
{

};

/// used for fractional manipulation
struct unsigned128
{
	uint64_t hi,lo;
	constexpr unsigned128(uint64_t xlo = 0): hi(0),lo(xlo)
	{

	}
	constexpr unsigned128(uint64_t xhi,uint64_t xlo): hi(xhi),lo(xlo)
	{

	}

	constexpr operator uint64_t () const {return lo; }

	friend constexpr bool operator < (unsigned128 a, unsigned128 b) 
	{
		return a.hi < b.hi && a.lo < b.lo;
	}

	friend CONSTEXPR14 unsigned128 operator - (unsigned128 a, unsigned128 b) 
	{
		unsigned128 r = unsigned128(a.hi-b.hi,a.lo-b.lo);
		if(a.lo < b.lo)
			return unsigned128(r.hi-1,r.lo);
		else
			return r;
	}

	friend CONSTEXPR14 unsigned128 operator + (unsigned128 a, unsigned128 b) 
	{
		unsigned128 r = unsigned128(a.hi+b.hi,a.lo+b.lo);
		if(r.lo < a.lo)
			return unsigned128(r.hi+1,r.lo); // add 1 and wrap around
		else
			return r;
	}

	CONSTEXPR14 unsigned128 operator << (int n) const 
	{
		unsigned128 r;
		if(n >= 64)
		{
			return unsigned128(lo << (n-64),0);
		}
		else
		{
			r.lo = lo << n;
#ifdef _MSVC_VER
			r.hi = __shiftleft128(hi,lo,n);
#else
			r.hi = (hi << n)|(lo >> (64-n));
#endif
		}
		return r;
	}

	// NOTE: n maximum 64
	CONSTEXPR14 unsigned128 operator >> (int n) const 
	{
		unsigned128 r;
		if(n >= 64)
		{
			return unsigned128(0, hi >> (n-64));
		}
		else
		{
			r.hi = hi >> n;
#ifdef _MSVC_VER
			r.lo = __shiftright128(hi,lo,n);
#else
			r.lo = (lo >> n)|(hi << (64-n));
#endif
		}
		return r;
	}
};
#define HAS128T

namespace std
{
	template<>
	class is_unsigned<unsigned128>:  std::true_type {};

	template<>
	class is_unsigned<signed128>:  std::false_type {};
}
#endif

/// returns the larges type between two
template <class A,class B>
using largest_type = typename std::conditional<sizeof(A) >= sizeof(B), A, B>::type;

/// Given size in bits returns the integer with given size
/// Equivalent to: http://www.boost.org/doc/libs/1_48_0/libs/integer/doc/html/boost_integer/integer.html#boost_integer.integer.sized
namespace detail_least 
{
	template< int Category > struct int_least_helper {}; 
#ifdef HAS128T
	template<> struct int_least_helper<1> { typedef signed128 signed_type;  typedef unsigned128 unsigned_type; };
#endif
	template<> struct int_least_helper<2> { typedef int64_t signed_type; typedef uint64_t unsigned_type; };
	template<> struct int_least_helper<3> { typedef int32_t signed_type; typedef uint32_t unsigned_type; };
	template<> struct int_least_helper<4> { typedef int16_t signed_type; typedef uint16_t unsigned_type; };
	template<> struct int_least_helper<5> { typedef int8_t signed_type; typedef uint8_t unsigned_type; };

}

/// Given size in bits returns the integer with given size
template <unsigned int N>
struct int_least_bits : public detail_least::int_least_helper<((N) <= 8) + ((N) <= 16) + ((N) <= 32) + ((N) <= 64) + ((N) <= 128)> 
{
};

/// Helper for avoiding the fact that int8_t and uint8_t are printerd as chars in iostream
template <class T>
struct printableinttype
{
	using type = T;
};

template <class T>
struct printableinttype<const T>
{
    using typex = typename printableinttype<T>::type;
    using type = const typex;
};

template <>
struct printableinttype<uint8_t>
{
	using type = uint16_t;
};

template <>
struct printableinttype<int8_t>
{
	using type = int16_t;
};

/// next integer type in size: signed and unsigned
template <class T>
struct nextinttype
{
	struct error_type_is_too_large_or_missing_128bit_integer {};
	using type = error_type_is_too_large_or_missing_128bit_integer;
};

#ifdef HAS128T
template <>
struct nextinttype<uint64_t>
{
	using type = unsigned128;
};
#endif

template <>
struct nextinttype<uint32_t>
{
	using type = uint64_t;
};

template <>
struct nextinttype<uint16_t>
{
	using type = uint32_t;
};

template <>
struct nextinttype<uint8_t>
{
	using type = uint16_t;
};

#if 0
#ifdef HAS128T
template <>
struct nextinttype<int64_t>
{
	using type = signed128;
};
#endif
#endif

template <>
struct nextinttype<int32_t>
{
	using type = int64_t;
};

template <>
struct nextinttype<int16_t>
{
	using type = int32_t;
};

template <>
struct nextinttype<int8_t>
{
	using type = int16_t;
};

template <class T>
struct nextintop
{
	static inline typename   nextinttype<T>::type extramul(T a, T b)
	{
		return ((typename nextinttype<T>::type)a)*b;
	}

	static inline typename  nextinttype<T>::type extradiv(typename nextinttype<T>::type a, T b)
	{
		return (a/b);
	}

};
#ifdef _MSVC_VER

#if 0
// only unsigned needed
template <>
struct nextintop<int64_t>
{
	typename inline  signed128 extramul(int64_t a, int64_t b)
	{
		signed128 r;
		r.lo = __mul128(a,b,&r);
		return r;
	}

	typename inline  int64_t extradiv(signed128 a, uint64_t b)
	{
		return (T)(a/b);
	}

};
#endif

CLZCONSTEXPR inline int findbitleftmostC(unsigned128 input)
{
	return input.hi !=0 ? findbitleftmostC(input.hi) + 64 : findbitleftmostC(input.lo);
}

template <>
struct nextintop<uint64_t>
{
	static  inline typename  unsigned128 extramul(uint64_t a, uint64_t b)
	{
		unsigned128 result;
#ifdef _MSVC_VER
		result.lo = __umul128(a,b,&result.hi);
#else
		// https://github.com/abseil/abseil-cpp/ modified
		uint64_t a32 = a.lo >> 32;
		uint64_t a00 = a.lo & 0xffffffff;
		uint64_t b32 = b >> 32;
		uint64_t b00 = b & 0xffffffff;
		unsigned128 result(a.hi * b.lo + a32 * b32, a00 * b00);
		result += unsigned128(a32 * b00) << 32;
		result += unsigned128(a00 * b32) << 32;
#endif
		return result;
	}

	static inline typename   unsigned128 extradiv(unsigned128 dividend, uint64_t divisor)
	{
		// https://github.com/abseil/abseil-cpp/blob/master/absl/numeric/int128.cc
		// from http://stackoverflow.com/questions/5386377/division-without-using
		if (divisor == dividend) 
			return 1;
		else if (dividend < divisor)
			return 0;
		else
		{
			unsigned128 denominator = divisor;
			unsigned128 quotient = 0;

			// Left aligns the MSB of the denominator and the dividend.
			const int shift = findbitleftmostC(dividend) - findbitleftmostC(denominator);
			denominator = denominator << shift;

			// Uses shift-subtract algorithm to divide dividend by denominator. The
			// remainder will be left in dividend.
			for (int i = 0; i <= shift; ++i) 
			{
				quotient = quotient << 1;
				if (!(dividend < denominator)) {
					dividend = dividend - denominator;
					quotient.lo |= 1; //quotient |= 1;
				}
				denominator = denominator >> 1;
			}
			return quotient;
		}
	}
};

#endif