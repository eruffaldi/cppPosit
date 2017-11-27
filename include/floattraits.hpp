#pragma once


struct halffloat
{
    uint16_t what;
};


// https://en.wikipedia.org/wiki/16-bit
struct half_trait
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
	using value_t = float;
	using holder_t = uint32_t;
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
	using value_t = double;
	using holder_t = uint64_t;
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
	using value_t = __float128;
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
