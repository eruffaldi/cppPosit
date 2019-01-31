/**
 * Various Integr Type Helpers
 *
 * Emanuele Ruffaldi 2017
 */
#pragma once
#include <cstdint>
#include <inttypes.h>

#if false && (defined(__llvm__) && __clang_major__ > 3) || (defined(__GNUC__) && __GNUC__ >= 4) || defined(__INTEL_COMPILER)
typedef int signed128 __attribute__((mode(TI)));
typedef unsigned unsigned128 __attribute__((mode(TI)));
#define HAS128T
#else
struct signed128
{
	uint64_t hi,lo;
};
struct unsigned128
{
	uint64_t hi,lo;
};
#define HAS128T
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
	template<> struct int_least_helper<1> { typedef signed128 least; };
#endif
	template<> struct int_least_helper<2> { typedef int64_t least; };
	template<> struct int_least_helper<3> { typedef int32_t least; };
	template<> struct int_least_helper<4> { typedef int16_t least; };
	template<> struct int_least_helper<5> { typedef int8_t least; };

}

/// Given size in bits returns the integer with given size
template <unsigned int N>
struct int_least_bits
{
	 typedef typename detail_least::int_least_helper<((N) <= 8) + ((N) <= 16) + ((N) <= 32) + ((N) <= 64) + ((N) <= 128)>::least type;
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

#ifdef HAS128T
template <>
struct nextinttype<int64_t>
{
	using type = signed128;
};
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
