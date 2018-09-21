/**
 * ZPosit8n library
 *
 * Emanuele Ruffaldi 2017
 */
#pragma once
#include <stdint.h>
#include <iostream>
#include <limits>

#ifndef POSIT8_SPEC
#define POSIT8_SPEC
#define POSIT8_ES 2
#define POSIT8_NAN false
#endif

#include "posit.h"

namespace posit8ns
{
	extern int8_t opadd[];
	extern int8_t opmul[];
	extern int8_t opdiv[];
	extern int8_t opinv[];
	extern int8_t opexp2[];
	extern int8_t ophalf[];
	extern int8_t opsquare[];
	extern uint32_t op2float[];
}

class posit8
{
public:
	// template <class T,int totalbits, int esbits, class FT, bool withnan>


	using value_t=int8_t;
	using fraction_t=uint8_t;
	static constexpr int vtotalbits = 8;
	static constexpr int vesbits = POSIT8_ES;
	using exponenttype=int32_t; 
	using PT=PositTrait<int8_t,vtotalbits,POSIT8_ES,POSIT8_NAN>;
	using FT=fraction_t;
	using FPT=Posit<int8_t,vtotalbits,POSIT8_ES,FT,POSIT8_NAN>;
	using UnpackedLow=typename FPT::UnpackedLow;
	using UnpackedT=typename FPT::UnpackedT;
	using info=typename FPT::info;

	struct DeepInit{};

	explicit posit8(double a);
	explicit posit8(float a);
	posit8(int a);
	explicit posit8(DeepInit , int8_t a) : v(a) {}
	posit8(): v(0) {}
	explicit posit8(FPT x) : v(x.v) {}
	explicit posit8(UnpackedLow u) : v(FPT(u).v) {}
	explicit posit8(UnpackedT u) : v(FPT(u).v) {}

	bool is_negative() const { return v < 0; }
	bool has_neg_exponent() const { return (v & 0x80) == 0; }
	bool is_nan() const { return PT::withnan && v == PT::POSIT_NAN; }
	bool is_infinity() const { return (v == PT::POSIT_PINF || v == PT::POSIT_NINF);}
	static posit8 max() { return posit8(DeepInit(),PT::POSIT_MAXPOS); } // 126 or 127
	static posit8 min() { return posit8(DeepInit(),PT::POSIT_MINNEG); } //  ? -126 : -127
	static posit8 one() { return posit8(DeepInit(),PT::POSIT_ONE); } // 0x40
	static posit8 two() { return posit8(DeepInit(),72); }
	static posit8 onehalf() { return posit8(DeepInit(),0x20); }
	static posit8 pinfinity() { return posit8(DeepInit(),PT::POSIT_PINF); } // ? 127: -128); }
	static posit8 ninfinity() { return posit8(DeepInit(),PT::POSIT_NINF); } //,PT::withnan ?-127: -128); }
	static posit8 nan() { return posit8(DeepInit(),PT::POSIT_NAN); } // withnan ?-128: 0); }

	posit8 inv() const { return posit8(DeepInit(),posit8ns::opinv[uu()]); }
	posit8 half() const { return posit8(DeepInit(),posit8ns::ophalf[uu()]);}
	posit8 twice() const;
	posit8 exp2() const { return posit8(DeepInit(),posit8ns::opexp2[uu()]); }
	posit8 square() const { return posit8(DeepInit(),posit8ns::opsquare[uu()]); }
	posit8 operator-() const { return posit8(DeepInit(),-v);  }

	friend bool operator<(posit8 a, posit8 b) { return a.v < b.v; }
	friend bool operator>(posit8 a, posit8 b) { return a.v > b.v; }
	friend bool operator<=(posit8 a, posit8 b) { return a.v <= b.v;}
	friend bool operator>=(posit8 a, posit8 b) { return a.v >= b.v; }
	friend bool operator==(posit8 a, posit8 b) { return a.v == b.v; }
	friend bool operator!=(posit8 a, posit8 b) { return a.v !=b.v; }

	friend posit8 operator+(posit8 a, posit8 b) { return posit8(DeepInit(),posit8ns::opadd[a.uu()*256+b.uu()]); }
	friend posit8 operator*(posit8 a, posit8 b) { return posit8(DeepInit(),posit8ns::opmul[a.uu()*256+b.uu()]); }
	friend posit8 operator-(posit8 a, posit8 b) { return a+(-b); }
	friend posit8 operator/(posit8 a, posit8 b) { return posit8(DeepInit(),posit8ns::opdiv[a.uu()*256+b.uu()]); }
	friend std::ostream & operator << (std::ostream & ons, const posit8 & p);

	posit8& operator+=(const posit8 &a) { posit8 r = *this+a; v = r.v; return *this; }
	posit8& operator*=(const posit8 &a) { posit8 r = *this*a; v = r.v; return *this; }

	unsigned int uu() const { return (unsigned int)(uint8_t)v;}

	FPT as_posit() const { return FPT(typename FPT::DeepInit(), v); }
	operator FPT() const { return as_posit(); }
	operator float() const { return uint32_to_float(posit8ns::op2float[uu()]); }
	operator double() const { return (float)*this; }
	operator int() const { return (float)*this; }

	UnpackedLow unpack_low() const { return as_posit().unpack_low(); }
	UnpackedT unpack() const { return as_posit().unpack(); }
	info analyze() const { return as_posit().analyze(); }


	int8_t v;

};

inline posit8 half(posit8 z) { return z.half(); }

inline posit8 twice(posit8 z) { return z.twice(); }

inline posit8 square(posit8 z) { return z.square(); }

inline posit8 inv(posit8 x) { return x.inv(); }

inline posit8 neg(posit8 z) { return -z; }

inline posit8 exp2(posit8 z) { return z.exp2(); }

inline bool is_negative(posit8 a)
{
	return a.v < 0;
}


namespace std {
    template<> class numeric_limits<posit8> {
    public:
       static posit8 max() {return posit8::max(); };
       static posit8 min() {return posit8::min(); };
        // One can implement other methods if needed
    };
}



namespace std
{
	inline posit8 abs(posit8 z) 
	{
		return posit8(posit8::DeepInit(),z.v < 0 ? -z.v : z.v);
	}

	inline posit8 min(posit8 a, posit8 b)
	{
		return a <=  b ? a : b;
	}

	inline posit8 max(posit8 a, posit8 b)
	{
		return a >= b ? a : b;
	}
}

