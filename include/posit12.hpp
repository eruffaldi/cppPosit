/**
 * ZPosit8n library
 *
 * Emanuele Ruffaldi 2017
 */
#pragma once
#include <stdint.h>
#include <iostream>

#ifndef CONSTEXPR14
#define CONSTEXPR14
#endif

#ifndef POSIT12_SPEC
#define POSIT12_SPEC
#define POSIT12_ES 2
#define POSIT12_NAN false
#endif

#include "posit.h"

namespace posit12ns
{
	extern int16_t opinv[];
	extern int16_t opexp2[];
	extern int16_t opsquare[];
	extern uint32_t op2float[];
}


class posit12
{
public:
	// template <class T,int totalbits, int esbits, class FT, bool withnan>
	using value_t=int16_t;
	using fraction_t=uint16_t;
	static constexpr int vtotalbits = 12;
	static constexpr int vesbits = POSIT12_ES;
	using exponenttype=int32_t; 
	using PT=PositTrait<int16_t,vtotalbits,POSIT12_ES,POSIT12_NAN>;
	using FT=fraction_t;
	using FPT=Posit<int16_t,vtotalbits,POSIT12_ES,FT,POSIT12_NAN>;
	using UnpackedLow=typename FPT::UnpackedLow;
	using UnpackedT=typename FPT::UnpackedT;
	using info=typename FPT::info;

	struct DeepInit{};

	explicit posit12(double a);
	explicit posit12(float a);
	explicit posit12(int a);
	explicit posit12(DeepInit , int16_t a) : v(a) {}
	explicit posit12() {}
	explicit posit12(FPT x) : v(x.v) {}
	explicit posit12(UnpackedLow u) : v(FPT(u).v) {}
	explicit posit12(UnpackedT u) : v(FPT(u).v) {}

	bool is_negative() const { return v < 0; }
	bool has_neg_exponent() const { return (v & 0x400) == 0; }
	bool is_nan() const { return PT::withnan && v == -2048; }
	bool is_infinity() const { return PT::withnan ? (v == -2047 || v == 2047) : v == -2048;}
	static posit12 one() { return posit12(DeepInit(),0x400); }
	static posit12 onehalf() { return posit12(DeepInit(),0x200); }
	static posit12 pinfinity() { return posit12(DeepInit(),PT::withnan ? 2047: -2048); }
	static posit12 ninfinity() { return posit12(DeepInit(),PT::withnan ?-2047: -2048); }
	static posit12 nan() { return posit12(DeepInit(),PT::withnan ?-2048: 0); }

	posit12 inv() const { return posit12(DeepInit(),posit12ns::opinv[uu()]); }
	posit12 half() const;
	posit12 twice() const;
	posit12 exp2() const { return posit12(DeepInit(),posit12ns::opexp2[uu()]); }
	posit12 square() const { return posit12(DeepInit(),posit12ns::opsquare[uu()]); }
	posit12 operator-() const { return posit12(DeepInit(),-v);  }

	friend bool operator<(posit12 a, posit12 b) { return a.v < b.v; }
	friend bool operator>(posit12 a, posit12 b) { return a.v > b.v; }
	friend bool operator<=(posit12 a, posit12 b) { return a.v <= b.v;}
	friend bool operator>=(posit12 a, posit12 b) { return a.v >= b.v; }
	friend bool operator==(posit12 a, posit12 b) { return a.v == b.v; }
	friend bool operator!=(posit12 a, posit12 b) { return a.v !=b.v; }

	friend posit12 operator+(posit12 a, posit12 b) { return (posit12)(a.as_posit()+b.as_posit()); }
	friend posit12 operator*(posit12 a, posit12 b) { return (posit12)(a.as_posit()*b.as_posit()); }
	friend posit12 operator-(posit12 a, posit12 b) { return a+(-b); }
	friend posit12 operator/(posit12 a, posit12 b) { return (posit12)(a.as_posit()/b.as_posit()); }
	friend std::ostream & operator << (std::ostream & ons, const posit12 & p);

	posit12& operator+=(const posit12 &a) { posit12 r = *this+a; v = r.v; return *this; }
	posit12& operator*=(const posit12 &a) { posit12 r = *this*a; v = r.v; return *this; }

	unsigned int uu() const { return ((unsigned int)(uint16_t)v) & 0x0FFF;}

	UnpackedLow unpack_low() const { return as_posit().unpack_low(); }
	UnpackedT unpack() const { return as_posit().unpack(); }
	info analyze() const { return as_posit().analyze(); }

	FPT as_posit() const { return FPT(typename FPT::DeepInit(), v); }
	operator FPT() const { return as_posit(); }
	operator float() const { return uint32_to_float(posit12ns::op2float[uu()]); }
	operator double() const  { return (double)(float)*this; }
	operator int() const  { return (int)(float)*this; }

	int16_t v;

};

inline posit12 half(posit12 z) { return z.half(); }

inline posit12 twice(posit12 z) { return z.twice(); }

inline posit12 square(posit12 z) { return z.square(); }

inline posit12 inv(posit12 x) { return x.inv(); }

CONSTEXPR14 inline posit12 neg(posit12 z) { return -z; }

inline posit12 exp2(posit12 z) { return z.exp2(); }


namespace std
{
	inline CONSTEXPR14 posit12 abs(posit12 z) 
	{
		return posit12(posit12::DeepInit(),z.v < 0 ? -z.v : z.v);
	}


	inline CONSTEXPR14 posit12 min(posit12 a, posit12 b)
	{
		return a <=  b ? a : b;
	}

	inline CONSTEXPR14 posit12 max(posit12 a, posit12 b)
	{
		return a >= b ? a : b;
	}
}