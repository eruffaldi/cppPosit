/**
 * Copyright (C) 2017-2019 Emanuele Ruffaldi
 * Distributed under the terms of the BSD 3-Clause License.  
 * 
 * (See accompanying file LICENSE)
 * 
 * --
 */
#pragma once
#include <limits>
#include <cstdint>
#include "bithippop.hpp"

#if defined(__SDSVHLS__) && !defined(FPGAHLS)
#define FPGAHLS
#endif

// wrapper for custom floats holdi
template <class T>
struct valuewrap
{
	T what;
    constexpr valuewrap(T w): what(w) {}
    constexpr valuewrap() : what(0) {}
};

struct halffloat : public valuewrap<uint16_t>
{
	using valuewrap<uint16_t>::valuewrap;
};

struct halffloatalt : public valuewrap<uint16_t>
{
	using valuewrap<uint16_t>::valuewrap;
};

struct microfloat : public valuewrap<uint8_t>
{
	using valuewrap<uint8_t>::valuewrap;
};


/// holder_T is an unsigned integer capable of storing 1+exp_bits+frac_bits exactly
/// value_T  is the struct or native type used for this 
template <int exp_bits, int frac_bits, class value_T, class holder_T, bool with_denorm_ = true>
struct any_floattrait
{
    using value_t = value_T;
    using holder_t = holder_T;

    static_assert(exp_bits+1+frac_bits == sizeof(holder_t)*8,"holding size");
    //static_assert<!std::is_signed<holder_t> >;

    static constexpr int data_bits = exp_bits+frac_bits+1;
    static constexpr int exponent_bits =  exp_bits;
	static constexpr int fraction_bits = frac_bits;
	static constexpr int exponent_bias = (1<<(exp_bits-1))-1; 
    static constexpr int exponent_max =  (1<<(exp_bits))-2;
	static constexpr int with_denorm = with_denorm_;
    static constexpr uint32_t exponent_mask = (1<<exponent_bits)-1;

    enum : holder_t {
    	signbit = ((holder_t)(1))<<(data_bits-1),

     pinfinity_h = bitmask<holder_t>(exp_bits) << frac_bits, // 0 1[e] 0[f]
     ninfinity_h = pinfinity_h | signbit, // 1 1[e] 0[f]

    // many nan ar possible, we pick the one with 
     nan_h = bitmask<holder_t>(exp_bits+1) << (frac_bits-1), // 0 1[e] 1 0[f-1]
     one_h = bitmask<holder_t>(exp_bits-1) << (frac_bits), // 0 0 1[e-1] 0[f]
     afterone_h = one_h+1, // 0 0 1[e-1] 0[f-1] 1
     max_h = (bitmask<holder_t>(exp_bits-1) << (frac_bits+1)) | bitmask<holder_t>(frac_bits), // 0 1[e-1] 0 1[f]
     lowest_h = max_h | signbit,
     min_h = ((holder_t)(1)) << frac_bits, // 0 0[e-1] 1 0[f]
     two_h = ((holder_t)(1)) << (exp_bits-1+frac_bits) // 0 1 0[e-1+f]
 };
};


// pulp8 alternative
using microfloat_trait =  any_floattrait<5,2,microfloat,uint8_t>;


// PULP 8E,7M vs classic 5E,10P
using half_traitalt =  any_floattrait<8,7,halffloatalt,uint16_t>;

// Intel bfloat16 as 8,7 without denormals
using bfloat16_trait = any_floattrait<8,7,halffloatalt,uint16_t, false>;

// https://en.wikipedia.org/wiki/16-bit
struct half_trait // : public any_floattrait<5,10,halffloat,uint16_t>
{
    using value_t = halffloat;
    using holder_t = uint16_t;
    static constexpr holder_t ninfinity_h = 0xFC00;
    static constexpr holder_t pinfinity_h = 0x7C00;
    static constexpr holder_t nan_h = 0x7E00;
    static constexpr holder_t one_h = 0x3C00; // one next is just + 1
    static constexpr holder_t two_h = 0x4000; 
    static constexpr holder_t max_h = 0x7bff; 
    static constexpr holder_t min_h = 0x0400;
    static constexpr holder_t lowest_h = 0xfbff; // -max_h
	static constexpr int with_denorm = true;
    // max subnormal 0 00000 1111111111 2−24 ≈ 6.09756 × 10−5
    // min subnormal 0 00000 0000000001 5.96046 × 10−8

    static constexpr int data_bits = 16; // can be derived from value_t
    static constexpr int exponent_bits =  5;
	static constexpr int fraction_bits = 10; // can be derived from data_bits and exponent_bits
    static constexpr int exponent_bias = 15;
    static constexpr int exponent_max =  30; // can be derived from bias and bits
	static constexpr holder_t signbit = ((holder_t)(1))<<(data_bits-1);
    static constexpr uint32_t exponent_mask = (1<<exponent_bits)-1; // TODO make it from exponent_bits
};

// https://en.wikipedia.org/wiki/Single-precision_floating-point_format
struct single_trait
{
#ifndef FPGAHLS
	using value_t = float;
#endif
	using holder_t = uint32_t;
#ifndef FPGAHLS
	static constexpr value_t zero = 0.0f;
	static constexpr value_t ninfinity = -std::numeric_limits<value_t>::infinity();
	static constexpr value_t pinfinity = std::numeric_limits<value_t>::infinity();
#endif

    static constexpr holder_t ninfinity_h = 0xff800000;
    static constexpr holder_t pinfinity_h = 0x7f800000;
    static constexpr holder_t nan_h = 0x7fc00000;
    static constexpr holder_t one_h = 0x3f800000;
    static constexpr holder_t two_h = 0x40000000;
    static constexpr holder_t max_h = 0x7f7fffff;
    static constexpr holder_t min_h = 0x00800000;
    static constexpr holder_t denorm_min_h = 1; // just 1 after 0
    static constexpr holder_t epsilon_h = 0x34000000; // trickier
    static constexpr holder_t lowest_h = 0xff7fffff; // -max_h
	static constexpr int with_denorm = true;

	static constexpr int data_bits = 32; // can be derived from value_t
	static constexpr int exponent_bits =  8;
	static constexpr int fraction_bits = 23; // can be derived from data_bits and exponent_bits
	static constexpr int exponent_bias = 127;
	static constexpr int exponent_max =  254; // can be derived from bias and bits
	static constexpr holder_t signbit = ((holder_t)(1))<<(data_bits-1);
	static constexpr uint32_t exponent_mask = (1<<exponent_bits)-1; // TODO make it from exponent_bits
};

// https://en.wikipedia.org/wiki/Double-precision_floating-point_format
struct double_trait
{
#ifndef FPGAHLS
	using value_t = double;
#endif
	using holder_t = uint64_t;
#ifndef FPGAHLS
	static constexpr value_t zero = 0.0;
	static constexpr value_t ninfinity = -std::numeric_limits<value_t>::infinity();
	static constexpr value_t pinfinity = std::numeric_limits<value_t>::infinity();
#endif
    static constexpr holder_t ninfinity_h = 0xfff0000000000000ULL;
    static constexpr holder_t pinfinity_h = 0x7ff0000000000000ULL;
    static constexpr holder_t nan_h = 0x7ff8000000000000ULL;
    static constexpr holder_t one_h = 0x3ff0000000000000ULL;
    static constexpr holder_t two_h = 0x4000000000000000ULL;
    static constexpr holder_t max_h = 0x7fefffffffffffff; // TODO
    static constexpr holder_t lowest_h = 0xffefffffffffffff; // TODO
    static constexpr holder_t min_h = 0x10000000000000; // TODO
	static constexpr int with_denorm = true;

	static constexpr int data_bits = 64; // can be derived from value_t
	static constexpr int exponent_bits =  11;
	static constexpr int fraction_bits = 52; // can be derived from data_bits and exponent_bits
	static constexpr int exponent_bias = 1023;
	static constexpr int exponent_max =  2046;
	static constexpr holder_t signbit = ((holder_t)(1))<<(data_bits-1);
	static constexpr uint32_t exponent_mask = (((holder_t)(1))<<exponent_bits)-1; // 7FF
};

#ifdef FLT128_MAX
// https://en.wikipedia.org/wiki/Quadruple-precision_floating-point_format
// https://gcc.gnu.org/onlinedocs/gcc/Floating-Types.html
struct float128_trait
{
#ifndef FPGAHLS
	using value_t = __float128;
#endif
	using holder_t = unsigned __int128;
#ifndef FPGAHLS
	static constexpr value_t zero = 0.0;
	static constexpr value_t ninfinity = -std::numeric_limits<value_t>::infinity();
	static constexpr value_t pinfinity = std::numeric_limits<value_t>::infinity();
#endif

    static constexpr holder_t ninfinity_h = 0xb00000000000ffff8000000000000000;
    static constexpr holder_t pinfinity_h = 0x900007f975247fff8000000000000000;
    static constexpr holder_t nan_h = 0xb000000000007fffc000000000000000;
    static constexpr holder_t one_h = 0xd00007f975243fff8000000000000000;
    static constexpr holder_t two_h = 0x40000000000000000000000000000000;
    static constexpr holder_t max_h = 0x4000; // TODO
    static constexpr holder_t lowest_h = 0x4000; // TODO
    static constexpr holder_t min_h = 0x4000; // TODO
	static constexpr int with_denorm = true;

	static constexpr int data_bits = 128; // can be derived from value_t
	static constexpr int exponent_bits =  15;
	static constexpr int fraction_bits = 112; // can be derived from data_bits and exponent_bits
	static constexpr int exponent_bias = 16383;
	static constexpr int exponent_max =  16383*2;
	static constexpr holder_t signbit = (1<<(data_bits-1));
	static constexpr uint32_t exponent_mask = (1<<exponent_bits)-1; // TODO make it from exponent_bits
};

#endif

/**
 Casting between differently arbitrary floats requires:

 - exponent:
 		oldexponent - oldbias +newbias

 - fraction assumed to be left aligned
 		nothing just keep left aligned, only newfraction bits be used

 See:
	template <class FT,class ET>
	template <class Trait>
	CONSTEXPR14 typename Trait::holder_t Unpacked<FT,ET>::pack_xfloati() const

	template <class Trait, typename = typename  std::enable_if<!std::is_integral<typename Trait::value_t>::value>::type> 
    explicit CONSTEXPR14 Unpacked(typename Trait::holder_t i) { unpack_xfloat<Trait>(i); }

 */



template <class T>
struct float2trait
{};

template <>
struct float2trait<float>
{
	using type = float;
	using trait = single_trait;
};

template <>
struct float2trait<double>
{
	using type = double;
	using trait = double_trait;
};

template <>
struct float2trait<halffloat>
{
	using type = halffloat;
	using trait = half_trait;
};

template <>
struct float2trait<halffloatalt>
{
	using type = halffloatalt;
	using trait = half_traitalt;
};

#ifdef FLT128_MAX
template <>
struct float2trait<__float128>
{
	using type = __float128;
	using trait = float128_trait;
};
#endif

#if 0
template <class Trait>
struct limithelper
{
	using T=Trait;
	  static constexpr bool is_specialized = true;
	  static constexpr T min() noexcept { return T::min(); }
	  static constexpr T max() noexcept { return T::max(); }
	  static constexpr T lowest() noexcept { return T::lowest	(); }
	  //static constexpr int  digits = 0; number of digits (in radix base) in the mantissa 
	  //static constexpr int  digits10 = 0;
	  static constexpr bool is_signed = true;
	  static constexpr bool is_integer = false;
	  static constexpr bool is_exact = false;
	  static constexpr int radix = 2;
	  static constexpr T epsilon() noexcept { return T::one_next()-T::one(); }
	  //static constexpr T round_error() noexcept { return T(); } 

	  // this is also the maximum integer
	  static constexpr int  min_exponent = PT::minexponent();
	  // static constexpr int  min_exponent10 = 0;
	  static constexpr int  max_exponent = PT::maxexponent();
	  //static constexpr int  max_exponent10 = 0;

	  static constexpr bool has_infinity = true;
	  static constexpr bool has_quiet_NaN = withnan;
	  static constexpr bool has_signaling_NaN = false;
	  //static constexpr float_denorm_style has_denorm = denorm_absent;
	  static constexpr bool has_denorm_loss = false;
	  static constexpr T infinity() noexcept { return T::infinity(); }
	  static constexpr T quiet_NaN() noexcept { return T::nan(); }
	  //static constexpr T signaling_NaN() noexcept { return T(); }
	  static constexpr T denorm_min() noexcept { return T::min(); }

	  static constexpr bool is_iec559 = false;
	  static constexpr bool is_bounded = false;
	  static constexpr bool is_modulo = false;

	  static constexpr bool traps = false;
	  static constexpr bool tinyness_before = false;
};
#endif

#if 0

namespace std
{
	template <>
	struct limits<halffloat>: public limithelper<half_trait>
	{

	};

	template <>
	struct limits<halffloatalt>: public limithelper<half_traitalt>
	{

	};

	template <>
	struct limits<microfloat>: public limithelper<microfloat_trait>
	{

	};

}
#endif
