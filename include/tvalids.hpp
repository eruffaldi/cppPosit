/**
 * Copyright (C) 2017-2019 Emanuele Ruffaldi
 * Distributed under the terms of the BSD 3-Clause License.  
 * 
 * (See accompanying file LICENSE)
 * 
 * --
 */
// tvalids.hpp
//
// Emanuele Ruffaldi 2016
#pragma once 

#include <cstdint>
#include <iostream>
#include <limits>

#ifndef CONSTEXPR14
#define CONSTEXPR14
#endif

#include "posit.h"
/**
// T = support type (enough for 1<<N)
// N = exacts
// id= identifier
// T = two is the value of two (useful)
//
//
*/
template <class T, int N, int id, T vtwo>
class tvalid
{
public:
	static_assert(std::is_signed<T>::value,"required signed T");
	static_assert(N >= 2,"minimum is 2 for the case 0,1,2");
	static_assert(sizeof(T)*8 >= N*4,"required enough storage T for provided bits  SREF");

	using value_t=T;
	static constexpr int TN = N*4;
	using PT=PositTrait<T,TN,0,false>;
	static constexpr int vesbits = 0;
	using PTU=typename PT::POSIT_UTYPE;
	using tposit=value_t;

private:
	static T exacts[TN];
	static T opexp2[TN*TN];
	static T optwice[TN];
	static T ophalf[TN];
	static T opsquare[TN];
	static T opmul[TN*TN];
	static T opadd[TN*TN];
	static float op2float[TN];

public:
	struct DeepInit{};

	explicit tvalid(double a);
	explicit tvalid(float a);
	explicit tvalid(int a);
	explicit tvalid(DeepInit , int16_t a) : v(a) {}
	explicit tvalid() {}
	tvalid(const tvalid & a): v(a.v) {}

	bool is_negative() const { return v < 0; }
	bool has_neg_exponent() const { return (v & PT::POSIT_INVERTBIT) == 0; }
	bool is_nan() const { return PT::withnan && v == PT::POSIT_NAN; }
	bool is_infinity() const { return v==PT::POSIT_NINF || v==PT::POSIT_PINF; }
	static tposit max() { return tposit(DeepInit(),PT::POSIT_MAX); }
	static tposit min() { return tposit(DeepInit(),PT::POSIT_MIN); }  
	static tposit smallestpos() { return tposit(DeepInit(),PT::POSIT_SMALLPOS); }
	static tposit one() { return tposit(DeepInit(),PT::POSIT_ONE); }
	static tposit two() { return tposit(DeepInit(),vtwo); }  
	static tposit onehalf() { return inv(two()); }
	static tposit pinfinity() { return tposit(DeepInit(),PT::POSIT_PINF); 
	static tposit ninfinity() { return tposit(DeepInit(),PT::POSIT_NINF); 
	static tposit nan() { return tposit(DeepInit(),PT::POSIT_NAN); }

	friend bool operator<(const tvalid &a, const tvalid & b) { return a.v < b.v; }
	friend bool operator>(const tvalid &a, const tvalid & b) { return a.v > b.v; }
	friend bool operator<=(const tvalid &a, const tvalid & b) { return a.v <= b.v;}
	friend bool operator>=(const tvalid &a, const tvalid & b) { return a.v >= b.v; }
	friend bool operator==(const tvalid &a, const tvalid & b) { return a.v == b.v; }
	friend bool operator!=(const tvalid &a, const tvalid & b) { return a.v !=b.v; }

	tvalid inv() const { return tvalid(DeepInit(),-(v+TN)); }
	tvalid half() const { return (*this * inv(two())); }
	tvalid twice() const{ return tvalid(DeepInit(),optwice[uu()]); }
	tvalid exp2() const { return tvalid(DeepInit(),opexp2[uu()]); }
	tvalid square() const { return tvalid(DeepInit(),opsquare[uu()]); }
	tvalid operator-() const { return tvalid(DeepInit(),-v);  }

	friend tvalid operator+(const tvalid &a, const tvalid & b) { return tvalid(DeepInit(),opadd[a.uu()*TN+b.uu()]); }
	friend tvalid operator*(const tvalid &a, const tvalid & b) { return tvalid(DeepInit(),opmul[a.uu()*TN+b.uu()]); }
	friend tvalid operator-(const tvalid &a, const tvalid & b) { return a+(-b); }
	friend tvalid operator/(const tvalid &a, const tvalid & b) { return tvalid(DeepInit(),opmul[a.uu()*TN+(inv(b).uu())]); }

	friend std::ostream & operator << (std::ostream & ons, const tvalid & p);

	tvalid& operator+=(const tvalid &a) { tvalid r = *this+a; v = r.v; return *this; }
	tvalid& operator*=(const tvalid &a) { tvalid r = *this*a; v = r.v; return *this; }

	PTU uu() const { return (PTU)v;}
	
	operator float() const { return uint32_to_float(op2float[uu()]); }
	operator double() const { return (float)*this; }
	operator int() const { return (float)*this; }

	value_t v;

};

template<class T, int N, int id >
inline tvalid<T,N,id> half(tvalid<T,N,id> z) { return z.half(); }

template<class T, int N, int id >
inline tvalid<T,N,id> twice(tvalid<T,N,id> z) { return z.twice(); }

template<class T, int N, int id >
inline tvalid<T,N,id> square(tvalid<T,N,id> z) { 	return z.square();  }

template<class T, int N, int id >
inline tvalid<T,N,id> inv(tvalid<T,N,id> x) { return x.inv(); }

template<class T, int N, int id >
CONSTEXPR14 inline tvalid<T,N,id> neg(tvalid<T,N,id> z) { return -z; }

template<class T, int N, int id >
inline tvalid<T,N,id> exp2(tvalid<T,N,id> z) { return z.exp2(); }

template<class T, int N, int id >
inline bool is_negative(tvalid<T,N,id> a)
{
	return a.v < 0;
}

namespace std {
    template<class T, int N, int id > class numeric_limits<tvalid<T,N,id> > {
    public:
       static tvalid<T,N,id> max() {return tvalid<T,N,id> ::max(); };
       static tvalid<T,N,id> min() {return tvalid<T,N,id> ::min(); };
        // One can implement other methods if needed
    };
}

namespace std
{
	template <class T, int N, int id, T two>
	inline CONSTEXPR14 tvalid<T,N,id> abs(tvalid<T,N,id> z) 
	{
		return tvalid(tvalid::DeepInit(),z.v < 0 ? -z.v : z.v);
	}

	template <class T, int N, int id, T two>
	inline CONSTEXPR14 tvalid<T,N,id> min(const tvalid<T,N,id> &a, const tvalid<T,N,id> & b)
	{
		return a <=  b ? a : b;
	}

	template <class T, int N, int id, T two>
	inline CONSTEXPR14 tvalid<T,N,id> max(const tvalid<T,N,id> &a, const tvalid<T,N,id> & b)
	{
		return a >= b ? a : b;
	}
}

