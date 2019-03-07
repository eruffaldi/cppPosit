/**
 * Copyright (C) 2017-2019 Emanuele Ruffaldi
 * Distributed under the terms of the BSD 3-Clause License.  
 * 
 * (See accompanying file LICENSE)
 * 
 * --
 */
/**
 * Zbinary8tabn library
 *
 * Emanuele Ruffaldi 2017
 */
#pragma once
#include <stdint.h>
#include <iostream>
#include <limits>

#ifndef BINARY8_SPEC
#define BINARY8_SPEC
#define BINARY8_EXP 5
#define BINARY8_MAN 2
#endif

#include "floattraits.hpp"

#ifndef FPGAHLS
inline float uint32_to_float(uint32_t i)
{
	union {
		float f;
		uint32_t i;
	} x;
	x.i = i;
	return x.f;
}
#endif


namespace binary8ns
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

class binary8tab
{
public:
	// template <class T,int totalbits, int esbits, class FT, bool withnan>


	using value_t=int8_t;
	using fraction_t=uint16_t;
	static constexpr int vtotalbits = 8;
	static constexpr int vesbits = BINARY8_EXP;
	using exponenttype=int32_t; 
	using trait_t=any_floattrait<BINARY8_EXP,BINARY8_MAN,int8_t,uint8_t>;
	using FT=fraction_t;

	struct DeepInit{};

	explicit binary8tab(double a);
	explicit binary8tab(float a);
	binary8tab(int a);
	explicit binary8tab(DeepInit , int8_t a) : v(a) {}
	binary8tab(): v(0) {}

	bool is_negative() const { return v < 0; }
	//bool is_nan() const { return trait_t::withnan && v == trait_t::POSIT_NAN; }
	//bool is_infinity() const { return (v == trait_t::POSIT_PINF || v == trait_t::POSIT_NINF);}
	static binary8tab max() { return binary8tab(DeepInit(),trait_t::max_h); } // 126 or 127
	static binary8tab min() { return binary8tab(DeepInit(),trait_t::min_h); } //  ? -126 : -127
	static binary8tab one() { return binary8tab(DeepInit(),trait_t::one_h); } // 0x40
	static binary8tab afterone() { return binary8tab(DeepInit(),trait_t::afterone_h); } // 0x40
	//static binary8tab two() { return binary8tab(DeepInit(),trait_t::two); }
	static binary8tab zero() { return binary8tab(DeepInit(),0); }
	//static binary8tab onehalf() { return binary8tab(DeepInit(),trait_t::onehalf); }
	//static binary8tab pinfinity() { return binary8tab(DeepInit(),trait_t::pinfinity); } // ? 127: -128); }
	//static binary8tab ninfinity() { return binary8tab(DeepInit(),trait_t::ninfinity); } //,trait_t::withnan ?-127: -128); }
	//static binary8tab nan() { return binary8tab(DeepInit(),trait_t::nan); } // withnan ?-128: 0); }

	binary8tab inv() const { return binary8tab(DeepInit(),binary8ns::opinv[uu()]); }
	binary8tab half() const { return binary8tab(DeepInit(),binary8ns::ophalf[uu()]);}
	binary8tab twice() const;
	binary8tab exp2() const { return binary8tab(DeepInit(),binary8ns::opexp2[uu()]); }
	binary8tab square() const { return binary8tab(DeepInit(),binary8ns::opsquare[uu()]); }
	binary8tab operator-() const { return binary8tab(DeepInit(),-v);  }

	friend bool operator<(binary8tab a, binary8tab b) { return a.v < b.v; }
	friend bool operator>(binary8tab a, binary8tab b) { return a.v > b.v; }
	friend bool operator<=(binary8tab a, binary8tab b) { return a.v <= b.v;}
	friend bool operator>=(binary8tab a, binary8tab b) { return a.v >= b.v; }
	friend bool operator==(binary8tab a, binary8tab b) { return a.v == b.v; }
	friend bool operator!=(binary8tab a, binary8tab b) { return a.v !=b.v; }

	friend binary8tab operator+(binary8tab a, binary8tab b) { return binary8tab(DeepInit(),binary8ns::opadd[a.uu()*256+b.uu()]); }
	friend binary8tab operator*(binary8tab a, binary8tab b) { return binary8tab(DeepInit(),binary8ns::opmul[a.uu()*256+b.uu()]); }
	friend binary8tab operator-(binary8tab a, binary8tab b) { return a+(-b); }
	friend binary8tab operator/(binary8tab a, binary8tab b) { return binary8tab(DeepInit(),binary8ns::opdiv[a.uu()*256+b.uu()]); }
	friend std::ostream & operator << (std::ostream & ons, const binary8tab & p);

	binary8tab& operator+=(const binary8tab &a) { binary8tab r = *this+a; v = r.v; return *this; }
	binary8tab& operator*=(const binary8tab &a) { binary8tab r = *this*a; v = r.v; return *this; }

	unsigned int uu() const { return (unsigned int)(uint8_t)v;}

	operator float() const { return uint32_to_float(binary8ns::op2float[uu()]); }
	operator double() const { return (float)*this; }
	operator int() const { return (float)*this; }

	int8_t v;

};

inline binary8tab half(binary8tab z) { return z.half(); }

inline binary8tab twice(binary8tab z) { return z.twice(); }

inline binary8tab square(binary8tab z) { return z.square(); }

inline binary8tab inv(binary8tab x) { return x.inv(); }

inline binary8tab neg(binary8tab z) { return -z; }

inline binary8tab exp2(binary8tab z) { return z.exp2(); }

inline bool is_negative(binary8tab a)
{
	return a.v < 0;
}


namespace std {
    template<> class numeric_limits<binary8tab> {
    public:
       static binary8tab max() {return binary8tab::max(); };
       static binary8tab min() {return binary8tab::min(); };
       static binary8tab epsilon() {return binary8tab::afterone()-binary8tab::one(); };
        // One can implement other methods if needed
    };
}



namespace std
{
	inline binary8tab abs(binary8tab z) 
	{
		return binary8tab(binary8tab::DeepInit(),z.v & 0x7F);
	}

	inline binary8tab min(binary8tab a, binary8tab b)
	{
		return a <=  b ? a : b;
	}

	inline binary8tab max(binary8tab a, binary8tab b)
	{
		return a >= b ? a : b;
	}
}

inline std::ostream & operator << (std::ostream & ons, const binary8tab & p)
{
	ons << "binary8tab(" << p.v << ")" ;
	return ons;
}