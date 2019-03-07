/**
 * Copyright (C) 2017-2019 Emanuele Ruffaldi
 * Distributed under the terms of the BSD 3-Clause License.  
 * 
 * (See accompanying file LICENSE)
 * 
 * --
 */
/**
 * TPosit is a generalized tabled posit
 *
 * Emanuele Ruffaldi 2017
 */
#pragma once
#include <stdint.h>
#include <iostream>

#ifndef CONSTEXPR14
#define CONSTEXPR14
#endif

#include "posit.h"


template <class T, int totalbits, int esbits, class FT, bool withnan>
class tposit
{
public:
	// template <class T,int totalbits, int esbits, class FT, bool withnan>


	using value_t=T;
	using fraction_t=FT;
	static constexpr int vtotalbits = totalbits;
	static constexpr int vesbits = esbits;
	using FPT=Posit<T,totalbits,esbits,FT,withnan>;
	using PT=typename FPT::PT;
	using exponenttype=typename PT::exponenttype;
	using FT=fraction_t;
	using PTU=typename PT::POSIT_UTYPE;
	using UnpackedLow=typename FPT::UnpackedLow;
	using UnpackedT=typename FPT::UnpackedT;
	using info=typename FPT::info;

private:
	static T opinv[];
	static T ophalf[];
	static T opexp2[];
	static T optwice[];
	static T opsquare[];
	static T opmul[];
	static T opadd[];
	static T opdiv[];
	static float op2float[];

public:
	struct DeepInit{};

	explicit tposit(double a)
	explicit tposit(float a);
	explicit tposit(int a);
	explicit tposit(DeepInit , int8_t a) : v(a) {}
	explicit tposit() {}
	explicit tposit(FPT x) : v(x.v) {}
	explicit tposit(UnpackedLow u) : v(FPT(u).v) {}
	explicit tposit(UnpackedT u) : v(FPT(u).v) {}

	bool is_negative() const { return v < 0; }
	bool has_neg_exponent() const { return (v & PT::POSIT_INVERTBIT) == 0; }
	bool is_nan() const { return PT::withnan && v == PT::POSIT_NAN; }
	bool is_infinity() const { return v==PT::POSIT_NINF || v==PT::POSIT_PINF; }
	static tposit max() { return tposit(DeepInit(),PT::POSIT_MAX); }
	static tposit min() { return tposit(DeepInit(),PT::POSIT_MIN); }  // <------
	static tposit one() { return tposit(DeepInit(),PT::POSIT_ONE); }

	//static tposit two() { return tposit(DeepInit(),72); }  // <------
	//static tposit onehalf() { return tposit(DeepInit(),0x20); }  // <------

	static tposit pinfinity() { return tposit(DeepInit(),PT::POSIT_PINF); 
	static tposit ninfinity() { return tposit(DeepInit(),PT::POSIT_NINF); 
	static tposit nan() { return tposit(DeepInit(),PT::POSIT_NAN); }

	tposit inv() const { return tposit(DeepInit(),opinv[uu()]); }
	tposit half() const { return tposit(DeepInit(),ophalf[uu()]);}
	tposit twice() const { return tposit(DeepInit(),optwice[uu()]); }
	tposit exp2() const { return tposit(DeepInit(),opexp2[uu()]); }
	tposit square() const { return tposit(DeepInit(),opsquare[uu()]); }
	tposit operator-() const { return tposit(DeepInit(),-v);  }

	friend bool operator<(tposit a, tposit b) { return a.v < b.v; }
	friend bool operator>(tposit a, tposit b) { return a.v > b.v; }
	friend bool operator<=(tposit a, tposit b) { return a.v <= b.v;}
	friend bool operator>=(tposit a, tposit b) { return a.v >= b.v; }
	friend bool operator==(tposit a, tposit b) { return a.v == b.v; }
	friend bool operator!=(tposit a, tposit b) { return a.v !=b.v; }

	friend tposit operator+(tposit a, tposit b) { return tposit(DeepInit(),opadd[a.uu()*256+b.uu()]); }  // <------
	friend tposit operator*(tposit a, tposit b) { return tposit(DeepInit(),opmul[a.uu()*256+b.uu()]); }  // <------
	friend tposit operator-(tposit a, tposit b) { return a+(-b); }
	friend tposit operator/(tposit a, tposit b) { return tposit(DeepInit(),opdiv[a.uu()*256+b.uu()]); }  // <------
	
	tposit& operator+=(const tposit &a) { tposit r = *this+a; v = r.v; return *this; }
	tposit& operator*=(const tposit &a) { tposit r = *this*a; v = r.v; return *this; }

	PTU uu() const { return (PTU)v;}

	FPT as_posit() const { return FPT(typename FPT::DeepInit(), v); }
	operator FPT() const { return as_posit(); }
	operator float() const { return uint32_to_float(op2float[uu()]); }
	operator double() const { return (float)*this; }
	operator int() const { return (float)*this; }

	UnpackedLow unpack_low() const { return as_posit().unpack_low(); }
	UnpackedT unpack() const { return as_posit().unpack(); }
	info analyze() const { return as_posit().analyze(); }


	friend std::ostream & operator << (std::ostream & ons, const tposit & p)
	{
		ons << "posit(" << p.uu() << ")" ;
		return ons;
	}


	T v;

};

template <class T, int totalbits, int esbits, class FT, bool withnan>
inline tposit<T,totalbits,esbits,FT,withnan> half(tposit<T,totalbits,esbits,FT,withnan> z) { return z.half(); }

template <class T, int totalbits, int esbits, class FT, bool withnan>
inline tposit<T,totalbits,esbits,FT,withnan> twice(tposit<T,totalbits,esbits,FT,withnan> z) { return z.twice(); }

template <class T, int totalbits, int esbits, class FT, bool withnan>
inline tposit<T,totalbits,esbits,FT,withnan> square(tposit<T,totalbits,esbits,FT,withnan> z) { return z.square(); }

template <class T, int totalbits, int esbits, class FT, bool withnan>
inline tposit<T,totalbits,esbits,FT,withnan> inv(tposit<T,totalbits,esbits,FT,withnan> x) { return x.inv(); }

template <class T, int totalbits, int esbits, class FT, bool withnan>
CONSTEXPR14 inline tposit<T,totalbits,esbits,FT,withnan> neg(tposit<T,totalbits,esbits,FT,withnan> z) { return -z; }

template <class T, int totalbits, int esbits, class FT, bool withnan>
inline tposit<T,totalbits,esbits,FT,withnan> exp2(tposit<T,totalbits,esbits,FT,withnan> z) { return z.exp2(); }

template <class T, int totalbits, int esbits, class FT, bool withnan>
inline bool is_negative(tposit<T,totalbits,esbits,FT,withnan> a)
{
	return a.v < 0;
}

template <class T, int totalbits, int esbits, class FT, bool withnan>
tposit<T,totalbits,esbits,FT,withnan>::tposit(int a)
{
	if(a == 0)
		v = 0;
	else if(a == 1)
		v = PT::POSIT_ONE;
	else 
		v = FPT(a).v;

}

template <class T, int totalbits, int esbits, class FT, bool withnan>
tposit<T,totalbits,esbits,FT,withnan>::tposit(float a)
{
		if(a == 0)
		v = 0;
	else if(a == 1)
		v = PT::POSIT_ONE;
	else 
		v = FPT(a).v;

}

template <class T, int totalbits, int esbits, class FT, bool withnan>
tposit<T,totalbits,esbits,FT,withnan>::tposit(double a) 
{
	if(a == 0)
		v = 0;
	else if(a == 1)
		v = PT::POSIT_ONE; 
	else 
		v = FPT(a).v;		
}





namespace std {
    template<class T, int totalbits, int esbits, class FT, bool withnan> class numeric_limits<tposit<T,totalbits,esbits,FT,withnan> > {
    public:
       static tposit<T,totalbits,esbits,FT,withnan> max() {return tposit::max(); };
       static tposit<T,totalbits,esbits,FT,withnan> min() {return tposit::min(); };
        // One can implement other methods if needed
    };
}



namespace std
{
	template <class T, int totalbits, int esbits, class FT, bool withnan>
	inline CONSTEXPR14 tposit<T,totalbits,esbits,FT,withnan> abs(tposit<T,totalbits,esbits,FT,withnan> z) 
	{
		return tposit(tposit::DeepInit(),z.v < 0 ? -z.v : z.v);
	}

	template <class T, int totalbits, int esbits, class FT, bool withnan>
	inline CONSTEXPR14 tposit<T,totalbits,esbits,FT,withnan> min(tposit<T,totalbits,esbits,FT,withnan> a, tposit<T,totalbits,esbits,FT,withnan> b)
	{
		return a <=  b ? a : b;
	}

	template <class T, int totalbits, int esbits, class FT, bool withnan>
	inline CONSTEXPR14 tposit <T,totalbits,esbits,FT,withnan> max(tposit<T,totalbits,esbits,FT,withnan> a, tposit<T,totalbits,esbits,FT,withnan> b)
	{
		return a >= b ? a : b;
	}
}

