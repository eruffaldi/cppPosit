/**
 * Copyright (C) 2017-2019 Emanuele Ruffaldi
 * Distributed under the terms of the BSD 3-Clause License.  
 * 
 * (See accompanying file LICENSE)
 * 
 * --
 */
/**
 * Float 16 native 32
 * Emanueel Ruffaldi 2017
 */
#pragma once
#include <cmath>
#include <iostream>
#include "unpacked.h"

using float16n_t = halffloat;

// use native F16C operation for conversion back & forth
// https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_extract_epi16
// https://db.in.tum.de/~finis/x86-intrin-cheatsheet-v2.1.pdf
#if defined(__AVX__) && defined(__F16C__)

// for _mm_cvtph_ps 
// for _mm_cvtss_f32
// for _mm_cvtsi128_si32
// for _mm_extract_epi16 SSE2
#include <immintrin.h>


// these intrinsics are rot constexpr
inline float float16nto32(float16n_t x)
{	
	// FP16C cost (4-5,1)
	return _mm_cvtss_f32(_mm_cvtph_ps( _mm_set1_epi16 (x.what) ));
}

inline float16n_t float32to16n(float x)
{
	// rounding from 32 to 16
	// _mm_cvtsi128_si32(x) &0xFFFF  with cost (2,1)
	// 		or
	// _mm_extract(x,0)  cost (3,1)
	return float16n_t((uint16_t)(_mm_cvtsi128_si32(_mm_cvtps_ph( _mm_set_ps1 (x),_MM_FROUND_TO_NEAREST_INT |_MM_FROUND_NO_EXC )) & 0xFFFF));
}

#else

// TODO: use optimized quicker version
// TODO: constexpr requires floatconst2bits.hpp
inline float float16nto32(float16n_t x)
{	
	// template <class SrcTrait, class DstTrait, class FT>
	// constexpr DstTrait::holder_t convertfloats(SrcTrait::holder_t src)
    return Unpacked<uint32_t,int>::template make_floati<half_trait>(x.what).template pack_xfloati<single_trait>();
}

// TODO: use optimized quicker version
inline float16n_t float32to16n(float x)
{
    return float16n_t(Unpacked<uint32_t,int>::template make_float<single_trait>(x).template pack_xfloati<half_trait>());
}


#endif


struct float16n32
{
	using trait_t = half_trait;
	using value_t= float16n_t;
	using impl_t = float;
	explicit float16n32() : value (0) {}
	explicit float16n32(float f): value(float32to16n(f)) {}
	explicit float16n32(float16n_t f): value(f) {}
	explicit constexpr float16n32(bool , uint16_t x): value(x) {}

	operator float () const { return float16nto32(value); }
	operator double () const { return float16nto32(value); }
	operator float16n_t() const { return value; }

	uint16_t raw() const { return value.what; }
	constexpr float16n32 abs()  const { return float16n32(0,value.what & ~0x8000);  }  // could be >= infinity because infinity is sign symmetric
	constexpr float16n32 neg()  const { return float16n32(0,value.what ^ 0x8000); }; 

	/// replace them with constants
	static float16n32 two() { return float16n32(false,trait_t::two_h); };
	static float16n32 one() { return float16n32(false,trait_t::one_h); };
	static float16n32 zero() { return float16n32(false,0); };

	friend float16n32 operator+ (float16n32 a, float16n32 b)  { return float16n32((impl_t)a+(impl_t)b); }
	friend float16n32 operator- (float16n32 a, float16n32 b)  { return float16n32((impl_t)a-(impl_t)b); }
	friend float16n32 operator* (float16n32 a, float16n32 b)  { return float16n32((impl_t)a*(impl_t)b); }
	friend float16n32 operator/ (float16n32 a, float16n32 b)  { return float16n32((impl_t)a/(impl_t)b); }
	float16n32 & operator+=(const float16n32 &a) { float16n32 x = *this + a; value = x.value; return *this; }
	float16n32 & operator*=(const float16n32 &a) { float16n32 x = *this * a; value = x.value; return *this; }

	float16n32 operator-() const {		return neg(); }

	friend float16n32 rem(float16n32 a,float16n32 b)  { return float16n32(remainder((impl_t)a,(impl_t)b));  }
	friend float16n32 sqrt(float16n32 a) { return float16n32(sqrt((impl_t)a));  }
	friend float16n32 mulAdd(float16n32 a, float16n32 b, float16n32 c) { return float16n32((impl_t)a*(impl_t)b+(impl_t)c); }

	friend float16n32 half(float16n32 a) { return a/two(); }
	friend float16n32 square(float16n32 a) { return a*a; }
	friend float16n32 inv(float16n32 a) { return one()/a; }

	friend bool operator == (float16n32 a, float16n32 b)  { return ((impl_t)a==(impl_t)b); }
	friend bool operator < (float16n32 a, float16n32 b)  { return ((impl_t)a<(impl_t)b); }
	friend bool operator > (float16n32 a, float16n32 b)  { return ((impl_t)a>(impl_t)b); }
	friend bool operator >= (float16n32 a, float16n32 b)  { return ((impl_t)a>=(impl_t)b); }
	friend bool operator <= (float16n32 a, float16n32 b)  { return ((impl_t)a<=(impl_t)b); }

	friend std::ostream & operator << (std::ostream & ons, const float16n32 & v)
	{
		ons << "f16(" << ((impl_t)v) << ")";
		return ons;
	}

private:
	value_t value;
};

namespace std
{
	inline CONSTEXPR14 float16n32 abs(float16n32 z)
	{
		return z.abs();
	}
}

#if 0
namespace std
{
	template <>
	struct numeric_limits<float16n32>
	{
		using T=float16n32;
		using FT=typename T::value_t;
		using TT=typename T::trait_t;
		//static constexpr T mk(uint32_t v) { return T(false,v);}
		#define mk(x) T(FT({x}))
		static constexpr bool is_specialized = true;
		static constexpr T min() noexcept { return mk(TT::min_h); }
		static constexpr T max() noexcept { return mk(TT::max_h); }
		static constexpr T lowest() noexcept { return mk(TT::lowest_h); }
		static constexpr int  digits = 24; //number of digits (in radix base) in the mantissa 
		static constexpr int  digits10 = 6;
		static constexpr bool is_signed = true;
		static constexpr bool is_integer = false;
		static constexpr bool is_exact = false;
		static constexpr int radix = 2;
		static constexpr T epsilon() noexcept { return mk(TT::epsilon_h); }
		//static constexpr T round_error() noexcept { return T(); } // 0.5f

		// this is also the maximum integer
		static constexpr int  min_exponent = -126;
		static constexpr int  min_exponent10 = -37;
		static constexpr int  max_exponent = 127;
		static constexpr int  max_exponent10 = 38;

		static constexpr bool has_infinity = true;
		static constexpr bool has_quiet_NaN = true;
		static constexpr bool has_signaling_NaN = false;
		static constexpr float_denorm_style has_denorm = std::denorm_present;
		static constexpr bool has_denorm_loss = false;
		static constexpr T infinity() noexcept { return mk(TT::pinfinity_h); }
		static constexpr T quiet_NaN() noexcept { return mk(TT::nan_h); }
		static constexpr T signaling_NaN() noexcept { return mk(TT::nan_h); }
		static constexpr T denorm_min() noexcept { return mk(TT::denorm_min_h); }

		static constexpr bool is_iec559 = true;
		static constexpr bool is_bounded = true;
		static constexpr bool is_modulo = false;

		static constexpr bool traps = false;
		static constexpr bool tinyness_before = false;
		#undef mk
	};	
}
#endif