#pragma once
#include <limits>

#if defined(__SDSVHLS__) && !defined(FPGAHLS)
#define FPGAHLS
#endif


struct halffloat
{
    uint16_t what;
};

struct halffloatalt
{
    uint16_t what;
};

struct microfloat
{
    uint8_t what;
};


/// holder_T is an unsigned integer capable of storing 1+exp_bits+frac_bits exactly
/// value_T  is the struct or native type used for this 
template <int exp_bits, int frac_bits, class value_T, class holder_T>
struct any_floattrait
{
    using value_t = value_T;
    using holder_t = holder_T;

    static constexpr int data_bits = exp_bits+frac_bits+1;
    static constexpr int exponent_bits =  exp_bits;
	static constexpr int fraction_bits = frac_bits;
	static constexpr int exponent_bias = (1<<(exp_bits-1))-1; 
    static constexpr int exponent_max =  (1<<(exp_bits))-2; 
    static constexpr holder_t signbit = ((holder_t)(1))<<(data_bits-1);
    static constexpr uint32_t exponent_mask = (1<<exponent_bits)-1;

    static constexpr holder_t ninfinity_h = signbit | 0xFC00; // 1 1[e] 0...
    static constexpr holder_t pinfinity_h = 0x7C00; // 0 1[e] 0...
    static constexpr holder_t nan_h = 0x7E00; // 0 1[e] 1 0...
    static constexpr holder_t one_h = 0x3C00; // 0 0 1[e-1] 0...
    static constexpr holder_t two_h = 0x4000; // 0 1 0...
};

struct microfloat_trait : public any_floattrait<8,5,microfloat,uint8_t>
{
};


// PULP 8E,7M vs classic 5E,10P
struct half_traitalt : public any_floattrait<8,7,halffloatalt,uint16_t>
{
};

// https://en.wikipedia.org/wiki/16-bit
struct half_trait : public any_floattrait<15,10,halffloat,uint16_t>
{
    using value_t = halffloat;
    using holder_t = uint16_t;
    static constexpr holder_t ninfinity_h = 0xFC00;
    static constexpr holder_t pinfinity_h = 0x7C00;
    static constexpr holder_t nan_h = 0x7E00;
    static constexpr holder_t one_h = 0x3C00;
    static constexpr holder_t two_h = 0x4000;

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
struct double128_trait
{
#ifndef FPGAHLS
	using value_t = __float128;
#endif
	using holder_t = unsigned __int128;

    static constexpr holder_t ninfinity_h = 0xb00000000000ffff8000000000000000;
    static constexpr holder_t pinfinity_h = 0x900007f975247fff8000000000000000;
    static constexpr holder_t nan_h = 0xb000000000007fffc000000000000000;
    static constexpr holder_t one_h = 0xd00007f975243fff8000000000000000;
    static constexpr holder_t two_h = 0x40000000000000000000000000000000;

	static constexpr int data_bits = 128; // can be derived from value_t
	static constexpr int exponent_bits =  15;
	static constexpr int fraction_bits = 112; // can be derived from data_bits and exponent_bits
	static constexpr int exponent_bias = 16383;
	static constexpr int exponent_max =  16383*2;
	static constexpr holder_t signbit = (1<<(data_bits-1));
	static constexpr uint32_t exponent_mask = (1<<exponent_bits)-1; // TODO make it from exponent_bits
};

#endif


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
struct float2trait<half_traitalt>
{
	using type = halffloatalt;
	using trait = half_traitalt;
};

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
	  static constexpr T epsilon() noexcept { return T::one().next()-T::one(); }
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