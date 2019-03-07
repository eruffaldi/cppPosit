/**
 * Copyright (C) 2017-2019 Emanuele Ruffaldi
 * Distributed under the terms of the BSD 3-Clause License.  
 * 
 * (See accompanying file LICENSE)
 * 
 * --
 */
/**
 * ZPosit8n library
 *
 * Emanuele Ruffaldi 2017
 */
#pragma once
#include <stdint.h>
#include <iostream>
#include <array>

#ifndef POSIT8_SPEC
#define POSIT8_SPEC
#define POSIT8_ES 2
#define POSIT8_NAN PositSpec::WithNan
#endif

#include "posit.h"

namespace posit8ns
{
	extern uint8_t opadd[];
	extern uint8_t opmul[];
	extern uint8_t opdiv[];
	extern uint8_t opinv[];
	extern uint8_t opexp2[];
	extern uint8_t opsquare[];
	extern uint32_t op2float[];
}

/*
 * Idea: optimize the extraction of 


 */

class simd_posit8_32
{
public:

	static __m256i lookup1(__m256i a, const uint8_t * tab)
	{
		alignas(__m256i) uint8_t idata[32];
		alignas(__m256i) uint8_t odata[32];

		_mm256_store_si256((__m256i*)idata,a);
		for(int i = 0; i < 32; i++)
			odata[i] = tab[idata[i]];
		return _mm256_load_si256((__m256i*)odata);
	}

	static __m256i lookup2(__m256i a, __m256i b, const uint8_t * tab)
	{
		alignas(__m256i) uint8_t idata1[32];
		alignas(__m256i) uint8_t idata2[32];
		alignas(__m256i) uint8_t odata[32];

		_mm256_store_si256((__m256i*)idata1,a);
		_mm256_store_si256((__m256i*)idata2,b);
		for(int i = 0; i < 32; i++)
			odata[i] = tab[idata1[i]*256+idata2[i]];
		return _mm256_load_si256((__m256i*)odata);		
	}

	using this_t=simd_posit8_32;
	using cmpresult=__m256i;
	using FPT=Posit<int8_t,8,POSIT8_ES,uint16_t,POSIT8_NAN>;
	using PT=typename FPT::PT;
	using FT=uint16_t;
	using base_t=FPT;

	enum { csize = 32 };
	struct Init{};

	explicit simd_posit8_32(float a);
	explicit simd_posit8_32(int a);
	explicit simd_posit8_32(Init , __m256i x) : v(x) {}
	explicit simd_posit8_32() {}
	explicit simd_posit8_32(__m256i x) : v(x) {}

	cmpresult is_negative() const { return (_mm256_cmpgt_epi8(_mm256_set1_epi8(0),v)); }
	cmpresult has_neg_exponent() const { return (_mm256_cmpeq_epi8(_mm256_set1_epi8(0),_mm256_and_si256 (_mm256_set1_epi8(0x40),v))); } //   (v & 0x40) == 0; }
	cmpresult is_nan() const { return (_mm256_cmpeq_epi8(_mm256_set1_epi8(-128),v)); }
	cmpresult is_infinity() const { return PT::withnan ? (_mm256_or_si256(  _mm256_cmpeq_epi8(_mm256_set1_epi8(-127),v), _mm256_cmpeq_epi8(_mm256_set1_epi8(127),v))) : (_mm256_cmpeq_epi8(_mm256_set1_epi8(-128),v));}

	this_t inv() const { return this_t(lookup1(v,posit8ns::opinv)); }
	this_t half() const { return *this / simd_posit8_32(2); }
	this_t twice() const { return *this * simd_posit8_32(2); }

	this_t exp2() const { return this_t(lookup1(v,posit8ns::opexp2)); }
	this_t square() const { return this_t(lookup1(v,posit8ns::opsquare)); }
	this_t operator-() const { return this_t(_mm256_sign_epi8 (v,v)); }


	friend this_t operator+(this_t a, this_t b) { return this_t(lookup2(a.uu(),b.uu(),posit8ns::opadd)); }
	friend this_t operator*(this_t a, this_t b) { return this_t(lookup2(a.uu(),b.uu(),posit8ns::opmul)); }
	friend this_t operator-(this_t a, this_t b) { return a+(-b); }
	friend this_t operator/(this_t a, this_t b) { return this_t(lookup2(a.uu(),b.uu(),posit8ns::opdiv)); }
	friend std::ostream & operator << (std::ostream & ons, const this_t & p);


	__m256i uu() const { return v;}

	operator std::array<float,32> () const ;

private:
	__m256i v;

};

inline std::ostream & operator << (std::ostream & ons, const std::array<float,32> & a)
{
	ons << "a32(" ;
	for(int i = 0; i < 32; i++)
		ons << " " << a[i];
	ons << ")";
	return ons;
}

inline std::ostream & operator << (std::ostream & ons, const __m256i & p)
{
		alignas(__m256i) uint8_t idata1[32];
		_mm256_store_si256((__m256i*)idata1,p);
		ons << "256i(" << std::hex ;
		for(int i = 0; i < 32; i++)
			ons << " " << (int)idata1[i];
		ons << std::dec << ")";
		return ons;
}

inline simd_posit8_32 half(simd_posit8_32 z) { return z.half(); }

inline simd_posit8_32 twice(simd_posit8_32 z) { return z.twice(); }

inline simd_posit8_32 square(simd_posit8_32 z) { return z.square(); }

inline simd_posit8_32 inv(simd_posit8_32 x) { return x.inv(); }

inline simd_posit8_32 neg(simd_posit8_32 z) { return -z; }

inline simd_posit8_32 exp2(simd_posit8_32 z) { return z.exp2(); }

template <class T> 
T gauss2(T x, T mu, T sigma)
{
	return exp2(-half(square((x-mu)/sigma)));
}