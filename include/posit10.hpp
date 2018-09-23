/**
 * Zposit10n library
 *
 * Emanuele Ruffaldi 2017
 */
#pragma once
#include <stdint.h>
#include <iostream>

#ifndef POSIT0_SPEC
#define POSIT10_SPEC
#define POSIT10_ES 2
#define POSIT10_NAN false
#endif

#include <limits>
#include "posit.h"

namespace posit10ns
{
	extern int16_t opinv[];
	extern int16_t ophalf[];
	extern int16_t opexp2[];
	extern int16_t opsquare[];
	extern uint32_t op2float[];
	extern int16_t opadd[];
	extern int16_t opmul[];
	extern int16_t opdiv[];
}


class posit10
{
public:
	// template <class T,int totalbits, int esbits, class FT, bool withnan>

	using value_t=int16_t;
	using fraction_t=uint16_t;
	static constexpr int vtotalbits = 10;
	static constexpr int vesbits = POSIT10_ES;
	using exponenttype=int32_t; 
	using PT=PositTrait<int16_t,vtotalbits,POSIT10_ES,POSIT10_NAN>;
	using FT=fraction_t;
	using FPT=Posit<int16_t,vtotalbits,POSIT10_ES,FT,POSIT10_NAN>;
	using UnpackedLow=typename FPT::UnpackedLow;
	using UnpackedT=typename FPT::UnpackedT;
	using info=typename FPT::info;

	struct DeepInit{};

	explicit posit10(double a);
	explicit posit10(float a);
	posit10(int a);
	explicit posit10(DeepInit , int16_t a) : v(a) {}
	posit10() : v(0) {}
	explicit posit10(FPT x) : v(x.v) {}
	explicit posit10(UnpackedLow u) : v(FPT(u).v) {}
	explicit posit10(UnpackedT u) : v(FPT(u).v) {}
	posit10(const posit10 & a): v(a.v) {}

	bool is_negative() const { return v < 0; }
	bool has_neg_exponent() const { return (v & 0x100) == 0; }
	bool is_nan() const { return PT::withnan && v == -512; }
	bool is_infinity() const { return PT::withnan ? (v == -511 || v == 511) : v == -512;}
	static posit10 max() { return posit10(DeepInit(),PT::withnan ? 510 : 511); }
	static posit10 min() { return posit10(DeepInit(),1); }
	static posit10 one() { return posit10(DeepInit(),0x100); }
	static posit10 afterone() { return posit10(DeepInit(),0x101); }
	static posit10 zero() { return posit10(DeepInit(),0); }
	static posit10 two() { return posit10(DeepInit(),288); }
	static posit10 onehalf() { return posit10(DeepInit(),0x80); }
	static posit10 pinfinity() { return posit10(DeepInit(),PT::withnan ? 511: -512); }
	static posit10 ninfinity() { return posit10(DeepInit(),PT::withnan ?-511: -512); }
	static posit10 nan() { return posit10(DeepInit(),PT::withnan ?-512: 0); }


	friend bool operator<(const posit10 &a, const posit10 & b) { return a.v < b.v; }
	friend bool operator>(const posit10 &a, const posit10 & b) { return a.v > b.v; }
	friend bool operator<=(const posit10 &a, const posit10 & b) { return a.v <= b.v;}
	friend bool operator>=(const posit10 &a, const posit10 & b) { return a.v >= b.v; }
	friend bool operator==(const posit10 &a, const posit10 & b) { return a.v == b.v; }
	friend bool operator!=(const posit10 &a, const posit10 & b) { return a.v !=b.v; }

	posit10 inv() const { return posit10(DeepInit(),posit10ns::opinv[uu()]); }
	posit10 half() const { return posit10(DeepInit(),posit10ns::ophalf[uu()]);}
	posit10 twice() const;
	posit10 exp2() const { return posit10(DeepInit(),posit10ns::opexp2[uu()]); }
	posit10 square() const { return posit10(DeepInit(),posit10ns::opsquare[uu()]); }
	posit10 operator-() const { return posit10(DeepInit(),-v);  }

#if 0
//	friend posit10 operator+(const posit10 &a, const posit10 & b) { return posit10(DeepInit(),a.v<0? -posit10ns::opadd[(-a).uu()*1024+(-b).uu()] : posit10ns::opadd[a.uu()*1024+b.uu()]); }
	friend posit10 operator+(const posit10 &a, const posit10 & b) { return posit10(DeepInit(),posit10ns::opadd[a.uu()*1024+b.uu()]); }
	friend posit10 operator*(const posit10 &a, const posit10 & b) { return posit10(DeepInit(),a.v<0? -posit10ns::opmul[-a.uu()*1024+(-b).uu()] : posit10ns::opmul[a.uu()*1024+b.uu()]); }
	friend posit10 operator-(const posit10 &a, const posit10 & b) { return posit10(DeepInit(),a.v<0? -posit10ns::opadd[(-a).uu()*1024+(b).uu()] : posit10ns::opadd[a.uu()*1024+(-b).uu()]); }
	friend posit10 operator/(const posit10 &a, const posit10 & b) { return posit10(DeepInit(),posit10ns::opdiv[a.uu()*1024+b.uu()]); }
#else
	friend posit10 operator+(const posit10 &a, const posit10 & b) { return posit10(DeepInit(),posit10ns::opadd[a.uu()*1024+b.uu()]); }
	friend posit10 operator*(const posit10 &a, const posit10 & b) { return posit10(DeepInit(),posit10ns::opmul[a.uu()*1024+b.uu()]); }
	friend posit10 operator-(const posit10 &a, const posit10 & b) { return a+(-b); }
	friend posit10 operator/(const posit10 &a, const posit10 & b) { return posit10(DeepInit(),posit10ns::opdiv[a.uu()*1024+b.uu()]); }
#endif
	friend std::ostream & operator << (std::ostream & ons, const posit10 & p);

	posit10& operator+=(const posit10 &a) { posit10 r = *this+a; v = r.v; return *this; }
	posit10& operator*=(const posit10 &a) { posit10 r = *this*a; v = r.v; return *this; }

	unsigned int uu() const { return ((unsigned int)(uint16_t)v) & 0x03FF;}
	UnpackedLow unpack_low() const { return as_posit().unpack_low(); }
	UnpackedT unpack() const { return as_posit().unpack(); }
	info analyze() const { return as_posit().analyze(); }

	FPT as_posit() const { return FPT(typename FPT::DeepInit(), v); }
	operator FPT() const { return as_posit(); }
	operator float() const { return uint32_to_float(posit10ns::op2float[uu()]); }
	operator double() const { return (float)*this; }
	operator int() const { return (float)*this; }

	int16_t v;

};

inline posit10 half(posit10 z) { return z.half(); }

inline posit10 twice(posit10 z) { return z.twice(); }

inline posit10 square(posit10 z) { 	return z.square();  }

inline posit10 inv(posit10 x) { return x.inv(); }

inline posit10 neg(posit10 z) { return -z; }

inline posit10 exp2(posit10 z) { return z.exp2(); }

inline bool is_negative(posit10 a)
{
	return a.v < 0;
}

namespace std {
    template<> class numeric_limits<posit10> {
    public:
       static posit10 max() {return posit10::max(); };
       static posit10 min() {return posit10::min(); };
       static posit10 epsilon() {return posit10::afterone()-posit10::one(); };
        // One can implement other methods if needed
    };
}

namespace std
{
	inline posit10 abs(posit10 z) 
	{
		return posit10(posit10::DeepInit(),z.v < 0 ? -z.v : z.v);
	}

	inline posit10 min(const posit10 &a, const posit10 & b)
	{
		return a <=  b ? a : b;
	}

	inline posit10 max(const posit10 &a, const posit10 & b)
	{
		return a >= b ? a : b;
	}
}
