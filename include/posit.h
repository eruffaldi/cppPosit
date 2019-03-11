/**
 * Copyright (C) 2017-2019 Emanuele Ruffaldi
 * Distributed under the terms of the BSD 3-Clause License.  
 * 
 * (See accompanying file LICENSE)
 * 
 * --
 */
/**
Test

#include "posit.h"
using X=Posit<int32_t,4,0,uint32_t>;
X::PT::decode_posit_rs(1)
 */
#pragma once
#include "unpacked.h"

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

/**
 * Specifies the behavior of special numbers
 *
 * WithNan: one top Nan (or Nar in posit spec terminology)
 * WithInf: one top Inf signless (original posit design)
 * WithNanInf: one top Nan and two signed Infinites (for more float compatibility)
 */
enum class PositSpec { WithNan, WithInf, WithNanInf};

/**
 * Trait that provides the key properties of the Posit encoding
 */
template <class T, int totalbits, int esbits, PositSpec positspec_ >
struct PositTrait
{
	static_assert(std::is_signed<T>::value,"required signed T");
	static_assert(sizeof(T)*8 >= totalbits,"required enough storage T for provided bits  SREF");
	static_assert(esbits <= totalbits-3,"esbits should be at most N-3 for the cases [x01,E] and [x10,E]");

	using POSIT_STYPE = typename std::make_signed<T>::type;
	using POSIT_UTYPE = typename std::make_unsigned<T>::type;
	static constexpr PositSpec positspec = positspec_;
	static constexpr bool withnan = positspec_ != PositSpec::WithInf;
	using exponenttype = typename std::conditional<(totalbits+esbits >= sizeof(T)*8),typename  nextinttype<T>::type,T>::type;

	//enum : POSIT_UTYPE {
		static constexpr POSIT_UTYPE POSIT_MAXREGIME_BITS = totalbits-1;
		static constexpr POSIT_UTYPE POIST_ONEHELPER = 1;
		static constexpr POSIT_UTYPE POSIT_HOLDER_SIZE = sizeof(T)*8;
		static constexpr POSIT_UTYPE POSIT_SIZE = totalbits;
		static constexpr POSIT_UTYPE POSIT_ESP_SIZE = esbits;
		static constexpr POSIT_UTYPE POSIT_MSB = POIST_ONEHELPER<<(totalbits-1);
		static constexpr POSIT_UTYPE POSIT_HOLDER_MSB = POIST_ONEHELPER<<(POSIT_HOLDER_SIZE-1);
        static constexpr POSIT_UTYPE POSIT_MASK = ((POSIT_MSB-1)|(POSIT_MSB));
        static constexpr POSIT_UTYPE POSIT_ESP_MASK = (POIST_ONEHELPER<< esbits)-1;
        //POSIT_HOLDER_MSB = 1U<<(POSIT_HOLDER_SIZE-1),
        //POSIT_HOLDER_MASK = ((POSIT_HOLDER_SIZE-1)|(POSIT_HOLDER_SIZE)),
		static constexpr POSIT_UTYPE POSIT_EXTRA_BITS = POSIT_HOLDER_SIZE-totalbits;
		static constexpr POSIT_UTYPE POSIT_SIGNBIT = (POIST_ONEHELPER<<(totalbits-1)); // bit
		static constexpr POSIT_UTYPE POSIT_INVERTBIT = (POIST_ONEHELPER<<(totalbits-2));

		static constexpr POSIT_STYPE POSIT_REG_SCALE = 1<<esbits;

		// these are portable ways for representing 10000000 and the two adjacents numbers in 
		// the posit circle
		static constexpr POSIT_STYPE _POSIT_TOP = (POSIT_STYPE)((POSIT_UTYPE(~0) << (totalbits-1)));
		static constexpr POSIT_STYPE _POSIT_TOPRIGHT = (POSIT_STYPE)((POIST_ONEHELPER<< (totalbits-1))-1);
		static constexpr POSIT_STYPE _POSIT_TOPLEFT = (POSIT_STYPE)((POSIT_UTYPE(~0) << (totalbits-1)))+1;

		// Without Nan (classic Posit): there only one Infinity
		// With NaN: the top element is NaN and then its adjacents correspond to +- Infinity
		static constexpr POSIT_STYPE POSIT_PINF =  positspec_ == PositSpec::WithNanInf ? _POSIT_TOPRIGHT: _POSIT_TOP ; // 1[sign] 000000 or N-1 111 bits
		static constexpr POSIT_STYPE POSIT_NINF =  positspec_ == PositSpec::WithNanInf ? _POSIT_TOPLEFT: _POSIT_TOP;
		static constexpr POSIT_STYPE POSIT_NAN  = _POSIT_TOP;  // infinity in withnan=false otherwise it is truly nan
		static constexpr POSIT_STYPE POSIT_ONE =  POSIT_INVERTBIT; // fine due to position of invert bit
		static constexpr POSIT_STYPE POSIT_MONE = -POSIT_ONE ; // minus one

		// Two
		static constexpr POSIT_STYPE POSIT_TWO = (POSIT_INVERTBIT | (POSIT_INVERTBIT>>(1+esbits)));

		// 1/2
		// 00 1[esbits+1] 0[N-2-esbitis-1]
		static constexpr POSIT_STYPE POSIT_HALF = POSIT_STYPE( (POSIT_UTYPE(-1) >> (totalbits-esbits-1))) << (totalbits-3-esbits);
		
		// max value below Infinity
		// 1[holder-total] 1 0[total-1]
		static constexpr POSIT_STYPE POSIT_MAXPOS = _POSIT_TOPRIGHT - (positspec_ == PositSpec::WithNanInf ? 1:0);

		// min value above -Infinity
		// 0[holder-total] 0 1[total-1]
		static constexpr POSIT_STYPE POSIT_MINNEG = _POSIT_TOPLEFT + (positspec_ == PositSpec::WithNanInf? 1:0);

		// minimal number above zero
		static constexpr POSIT_STYPE POSIT_AFTER0 = 1; // right to 0
		static constexpr POSIT_STYPE POSIT_BEFORE0 = -POSIT_AFTER0; // left to 0

		//static constexpr exponenttype maxexponent = withnan_ ? POSIT_REG_SCALE * (POSIT_SIZE - 3) : POSIT_REG_SCALE * (POSIT_SIZE - 2);  // sign+1st rs
		//static constexpr exponenttype minexponent = (-((exponenttype)POSIT_REG_SCALE) * (POSIT_SIZE - 2))  // sign+1st rs

		static constexpr exponenttype maxexponent() { return positspec_ == PositSpec::WithNanInf ? POSIT_REG_SCALE * (totalbits - 3) : POSIT_REG_SCALE * (totalbits - 2); }
		static constexpr exponenttype minexponent() { return (-((exponenttype)POSIT_REG_SCALE) * (totalbits - 2)) ; }
	//enum : exponenttype{
	//};
    //static constexpr POSIT_UTYPE LMASK(POSIT_UTYPE bits, POSIT_UTYPE size)
    //{ return ((bits) & (POSIT_MASK << (POSIT_SIZE - (size)))); }


    // pars is T_left
    static CONSTEXPR14 std::pair<int,int> decode_posit_rs(T pars)
    {  
        const bool x = (pars & POSIT_HOLDER_MSB) != 0; // marker bit for > 1
        int aindex = x ? (~pars == 0 ? POSIT_MAXREGIME_BITS : findbitleftmostC((POSIT_UTYPE)~pars)) : (pars == 0 ? POSIT_MAXREGIME_BITS : findbitleftmostC((POSIT_UTYPE)pars)); // index is LAST with !x
        int index = aindex; // aindex > POSIT_SIZE  ? POSIT_SIZE : aindex;
        int reg = x ? index-1 : -index;
        int rs =  (int)POSIT_MAXREGIME_BITS < index+1 ? POSIT_MAXREGIME_BITS : index+1; //std::min((int)POSIT_MAXREGIME_BITS,index+1);

        /**
         if(x)
         {
			pars = ~pars;
			if(!pars)
			{
				return { POSIT_MAXREGIME_BITS-1, POSIT_MAXREGIME_BITS };
			}
			else
			{
				int index = findbitleftmostC((POSIT_UTYPE)pars));
				return { index-1, index+1 };
			}
         }
         else
         {
			if(!pars)
			{
				return { -(int)POSIT_MAXREGIME_BITS, POSIT_MAXREGIME_BITS };
			}
			else
			{
				int index = findbitleftmostC((POSIT_UTYPE)pars));
				return { -index, index+1 };	
			}
         }
         */
        //std::cout << "decode posit " << std::hex << std::bitset<sizeof(T)*8>(pars) << " 1first " << x <<  " index " <<  index << "(aindex " << aindex << ") rs " << rs << "reg  " << reg  << std::endl;
        return {reg,rs};
    }

    static constexpr std::pair<POSIT_STYPE,POSIT_UTYPE> split_reg_exp(exponenttype eexponent)
    {
    	// FLOORDIV(a,b)  ((a) / (b) - ((a) % (b) < 0)) 
    	// int reg = FLOORDIV(up.exp, POW2(es));
    	// POSIT_UTYPE exp = up.exp - POW2(es) * reg;
        return {eexponent >> POSIT_ESP_SIZE, eexponent & POSIT_ESP_MASK };
    }

    /// compiler note: it generate simply: shl and or
    static constexpr exponenttype join_reg_exp(POSIT_STYPE reg, POSIT_UTYPE exp)
    {
    	return (((exponenttype)reg) * (1<<POSIT_ESP_SIZE))|exp;
    }
};

//template <class T, int totalbits, int esbits, PositSpec positspec_ >
//constexpr typename PositTrait<T,totalbits,esbits,withnan_>::exponenttype PositTrait<T,totalbits,esbits,withnan_>::minexponent;

template <class T,int totalbits, int esbits, class FT, PositSpec positspec>
class Posit;

template <class T,int totalbits, int esbits, class FT, PositSpec positspec>
CONSTEXPR14 auto unpack_posit(const Posit<T,totalbits,esbits,FT,positspec> & p) -> typename Posit<T,totalbits,esbits,FT,positspec>::UnpackedT ;

template <class T,int totalbits, int esbits, class FT, PositSpec positspec>
CONSTEXPR14 Posit<T,totalbits,esbits,FT,positspec> pack_posit(const typename Posit<T,totalbits,esbits,FT,positspec>::UnpackedT & x);


/**
 * Unpacked representaiton of the Posit with (sign,regime,exponent,fraction)
 * 
 * UT is UnpackedT
 * PT is the Trait
 */
template <class UT, class PT>
struct UnpackedLow_t
{
	using Type = typename UT::Type;

	constexpr UnpackedLow_t(Type t): type(t), negativeSign(false), regime(0),exp(0),fraction(0) {}
	constexpr UnpackedLow_t(Type t, bool anegativeSign): type(t), negativeSign(anegativeSign), regime(0),exp(0),fraction(0) {}
	constexpr UnpackedLow_t(bool n, typename PT::POSIT_STYPE r, typename PT::POSIT_UTYPE e, typename PT::POSIT_UTYPE f):
		 type(UT::Regular),negativeSign(n), regime(r), exp(e), fraction(f) {}

	Type type; /// type of number: Regular, Zero, NaN, Infinity
	bool negativeSign; // for Regular and Infinity
	typename PT::POSIT_STYPE regime; // decoded with sign
	typename PT::POSIT_UTYPE exp;    // decoded
	typename PT::POSIT_UTYPE fraction; // fraction left aligned without 1.
};

/**
 * Stores the data in the MSB totalbits of T
 * Uses esbits bits
 *
 * Notes:
 * \tparam T is the holding type that has to be signed due to complement 2 sign method
 * \tparam totalbits is the significant bits of posit stored in T right aligned. Due to the 2 complement scheme the MSB bits are extension of the sign
 * \tparam esbits is the size of the exponent 
 * \tparam FT is the unisgned type holding the fraction with the 1 explicity specified
 *
 */
template <class T,int totalbits, int esbits, class FT, PositSpec positspec>
class Posit
{
public:
	using PT=PositTrait<T,totalbits,esbits,positspec>;
	//static_assert(std::is_unsigned<FT>::value,"required unsigned FT"); /// relaxed and delegated to Unpacked


    enum { vtotalbits = totalbits, vesbits = esbits};
	struct DeepInit{};
	static constexpr bool withnan = positspec != PositSpec::WithInf;
    using value_t=T;
    using fraction_t=FT;
    using UnpackedT=Unpacked<FT,typename PT::exponenttype>;
    using UnpackedLow = UnpackedLow_t<UnpackedT,PT>;
	using exponenttype = typename PT::exponenttype;
	T v; // index in the N2 space

#ifdef POSITMUL
	struct PositMul
	{
		constexpr PositMul(Posit av, Posit bv) : a(av),b(bv) {}
		Posit a,b;

		CONSTEXPR14 Posit asPosit() const { return pack_posit<T,totalbits,esbits,FT,positspec>(a.unpack()*b.unpack()); }

		CONSTEXPR14 operator Posit() const { return asPosit(); }

		CONSTEXPR14 operator UnpackedT() const { return asPosit(); }
	#ifndef FPGAHLS
		CONSTEXPR14 operator float() const { return asPosit(); }
		CONSTEXPR14 operator double() const { return asPosit(); }
	#endif
		CONSTEXPR14 operator int() const { return asPosit(); }

		// pa.a*pa.b+pb.a*pb.b => 
		friend CONSTEXPR14 Posit operator+(const PositMul & pa, const PositMul & pb) 
		{
			return pack_posit<T,totalbits,esbits,FT,positspec>(pa.a.unpack()*pa.b.unpack()+pb.a.unpack()*pb.b.unpack()); 
		}

		// pa.a*pa.b-pb.a*pb.b => 
		friend CONSTEXPR14 Posit operator-(const PositMul & pa, const PositMul & pb) 
		{
			return pack_posit<T,totalbits,esbits,FT,positspec>(pa.a.unpack()*pa.b.unpack()-pb.a.unpack()*pb.b.unpack()); 
		}

		// missing operators
		// &
		// -
	};
#endif

	CONSTEXPR14 Posit half() const;
	CONSTEXPR14 Posit twice() const;
	CONSTEXPR14 UnpackedLow unpack_low() const;
	static CONSTEXPR14 Posit pack_low(UnpackedLow);
	static CONSTEXPR14 UnpackedT unpacked_low2full(UnpackedLow x);
	static CONSTEXPR14 UnpackedLow unpacked_full2low(UnpackedT tx);


	/// diagnostics with full details
	struct info
	{
		bool infinity = false;
		bool nan = false;
		int k = 0;
		bool sign = false; // true if negative
		double regime = 0;
		int rs = 0;
		int es = 0;
		int fs = 0;
		int exp = 0;
		int exponent = 0;
		FT ifraction = 0;
		double fraction = 0;
		double value = 0;
	};

	info analyze();


    friend constexpr bool operator == (const Posit & a, const Posit & u)  { return withnan && (a.isNaN()||u.isNaN())?false :a.v == u.v; }
    friend constexpr bool operator != (const Posit & a, const Posit & u)  { return !(a == u); }
    friend constexpr bool operator < (const Posit & a, const Posit & u)  { return withnan && (a.isNaN()||u.isNaN())?false :a.v < u.v;; }
    friend constexpr bool operator <= (const Posit & a, const Posit & u)  { return withnan && (a.isNaN()||u.isNaN())?false :a.v <= u.v; }

    friend constexpr bool operator > (const Posit & a, const Posit & u)  { return withnan && (a.isNaN()||u.isNaN())?false :a.v > u.v; }
    friend constexpr bool operator >= (const Posit & a, const Posit & u)  { return withnan && (a.isNaN()||u.isNaN())?false :a.v >= u.v; }

    static constexpr Posit ldexp(const Posit & u, int exp); // exponent product

	using single_tag = typename UnpackedT::single_tag;

	constexpr Posit() : v(0) {}

	CONSTEXPR14 explicit Posit(single_tag t, uint32_t p) { v = pack_posit<T,totalbits,esbits,FT,positspec>(UnpackedT(t,p)).v; }

    /// construct passing the holding type x
	CONSTEXPR14 explicit Posit(DeepInit, T x) : v(x) {} 

	/// construct from decomposed (s, R,E,F)
	CONSTEXPR14 explicit Posit(UnpackedLow u) : v(pack_low(u).v) {} 

	/// construct from fully unpacked floating (s,e,F)
	CONSTEXPR14 explicit Posit(UnpackedT u) : v(pack_posit<T,totalbits,esbits,FT,positspec>(u).v) {} 

#ifndef FPGAHLS
    CONSTEXPR14 explicit Posit(float f): Posit(UnpackedT(f)) {}
	CONSTEXPR14 explicit Posit(double d): Posit(UnpackedT(d)) {}
#endif
	CONSTEXPR14 Posit(int i): Posit(UnpackedT(i)) {}

	constexpr UnpackedT unpack() const { return unpack_posit<T,totalbits,esbits,FT,positspec>(*this); }

	/// absolute value
	/// TODO: use (v ^ mask) - mask   OR (x+mask)^nasj
	/// where int const mask = v >> sizeof(int) * CHAR_BIT - 1;
	constexpr Posit abs()  const { return Posit(DeepInit(),pcabs(v));  }  // could be >= infinity because infinity is sign symmetric

	/// negation
	constexpr Posit neg()  const { return Posit(DeepInit(),-v); }; 

	/// 1/x
	CONSTEXPR14 Posit inv()  const;

	// SFINAE optionally: template<typename U = T, class = typename std::enable_if<withnan, U>::type>
    constexpr bool hasNaN() const { return positspec != PositSpec::WithInf; }
	constexpr bool isNaN() const { return positspec != PositSpec::WithInf && v == PT::POSIT_NAN; } 
	constexpr bool isnegative() const { return v < 0; } //(v &POSIT_SIGNBIT) != 0; }
	constexpr bool isinfinity() const { return positspec != PositSpec::WithNan && (v == PT::POSIT_PINF || v == PT::POSIT_NINF); }
	constexpr bool iszero() const { return v == 0; }
	constexpr bool isone() const { return v == PT::POSIT_ONE; }
	constexpr Posit prev() const { return Posit(DeepInit(),v > PT::POSIT_MAXPOS || v <= PT::POSIT_MINNEG ? v : v-1); }
	constexpr Posit next() const { return Posit(DeepInit(),v <= PT::POSIT_MINNEG || v > PT::POSIT_MAXPOS ? v : v+1); }
	//TBDconstexpr bool isNaN() const; 
	//TBD constexpr bool isexact() const { return (v&1) == 0; }

	//TBD constexpr bool isfractional() const { return v > 0 && (abs().v < (N2>>2)); } // (0 < x < 1) or  (-1 < x < 0) == (-1,1) removing 0
	//TBD constexpr bool isstrictlynegative() const { return v > (N2>>1); } // -inf < x < 0

	    // Level 1: unpacked
	// Level 0: something using posit specialties
#ifdef POSITMUL
	friend CONSTEXPR14 PositMul operator*(const Posit & a, const Posit & b) 
	{
		return PositMul(a,b); 
	}

	friend constexpr Posit operator+(const Posit & a, const PositMul & b)
	{
		return fma(b.a,b.b,a);
	}

	friend constexpr Posit operator+(const PositMul & a, const Posit & b)
	{
		return fma(a.a,a.b,b);
	}

#else
	friend CONSTEXPR14 Posit operator*(const Posit & a, const Posit & b) 
	{
		return pack_posit<T,totalbits,esbits,FT,positspec>(a.unpack()*b.unpack());
	}
#endif

	friend CONSTEXPR14 Posit fma(const Posit & a, const Posit & b, const Posit & c)
	{
		return pack_posit<T,totalbits,esbits,FT,positspec>(a.unpack()*b.unpack()+c.unpack());
	}

	CONSTEXPR14 Posit & operator*= (const Posit & b)
	{
		*this = pack_posit<T,totalbits,esbits,FT,positspec>(unpack()*b.unpack());
		return *this;
	}
    friend CONSTEXPR14 Posit operator+(const Posit & a, const Posit & b)
    {
        return a.iszero() ? b : b.iszero() ? a: pack_posit<T,totalbits,esbits,FT,positspec>(a.unpack()+b.unpack());
    }

	Posit& operator+=(const Posit &a) { Posit r = *this+a; v = r.v; return *this; }

	static constexpr Posit zero() { return Posit(DeepInit(),0); }
	static constexpr Posit inf() { return Posit(DeepInit(),PT::POSIT_PINF); }
	static constexpr Posit pinf() { return Posit(DeepInit(),PT::POSIT_PINF); }
	static constexpr Posit ninf() { return Posit(DeepInit(),PT::POSIT_NINF); }
	static constexpr Posit max() { return Posit(DeepInit(),PT::POSIT_MAXPOS); }
	static constexpr Posit min() { return Posit(DeepInit(),PT::POSIT_AFTER0); }
	static constexpr Posit lowest() { return Posit(DeepInit(),PT::POSIT_MINNEG); }

	// SFINAE optionally: template<typename U = T, class = typename std::enable_if<withnan, U>::type>
	static constexpr Posit nan() { return Posit(DeepInit(),PT::POSIT_NAN); }
	static constexpr Posit infinity() { return Posit(DeepInit(),PT::POSIT_PINF); }
	static constexpr Posit one() { return Posit(DeepInit(),PT::POSIT_ONE); }
	static constexpr Posit two() { return Posit(DeepInit(),PT::POSIT_TWO); }
	static constexpr Posit mone() { return Posit(DeepInit(),PT::POSIT_MONE); }
	static constexpr Posit onehalf() { return Posit(DeepInit(),PT::POSIT_HALF); }

	// custom operators
	constexpr Posit operator-() const { return neg(); } 
	constexpr Posit operator~() const { return inv(); } 
	friend CONSTEXPR14 Posit operator-(const Posit & a, const Posit & b)  { return a + (-b); }
	friend CONSTEXPR14 Posit operator/(const Posit & a, const Posit & b)  { return pack_posit< T,totalbits,esbits,FT,positspec> (a.unpack()/b.unpack()); }
    Posit & operator/= (const Posit & a) { auto x = *this / a; v = x.v; return *this; }

    template <class It1, class It2>
    static CONSTEXPR14 Posit dot(It1 begin1, It2 begin2, int N)
    {
    	if(N < 1)
    		return Posit::zero(); 
    	else
    	{
	    	auto v0 = (Posit(*begin1++)).unpack()*(Posit(*begin2++)).unpack(); // first
	    	for(int i = 1; i < N; i++)
		    	v0 += (Posit(*begin1++)).unpack()*(Posit(*begin2++)).unpack();
	    	return Posit(v0);
	    }
    }
   
	/*	
	void setBits(POSIT_UTYPE bits)
	{
		if(bits & POSIT_SIGNBIT)
		{
			v = ((~0) << POSIT_SIZE) | bits;
		}
		else
		{
			v = bits;
		}
	}
	*/

	// slowproduct

	// slowsum

	// exactvalue

	// iostream

	// sqrt

	// exp

	// conversion

	// max
	constexpr uint32_t as_float_bin() const { return unpack().template pack_xfloati<single_trait>(); }
	constexpr operator UnpackedT() const { return unpack(); }
#ifndef FPGAHLS
	constexpr operator float() const { return unpack(); }
	constexpr operator double() const { return unpack(); }
#endif
	constexpr operator int() const { return unpack(); }

	/// 1/(exp(-x)+1)
	/// TODO: infintity check + __round of result
	constexpr Posit pseudosigmoid() const { return Posit(DeepInit(),(v ^ PT::POSIT_SIGNBIT) >> 2); };

	/// ln(1+exp(x))
	constexpr Posit pseudosoftplus() const { return Posit(DeepInit(),(v ^ PT::POSIT_SIGNBIT) >> 1); };

	/// returns true if in [0,1]
	constexpr bool isUnitRange() const { return v >= 0 && v <= PT::POSIT_ONE; };

	/// unitary range 1-x
	constexpr Posit urOneMinus() const { return Posit(DeepInit(),PT::POSIT_INVERTBIT-v); }

	/// unitary range x(1-x)
	constexpr Posit urDeltaPs() const { return (*this)*urOneMinus(); }

};

#if 0
//template <class T,int totalbits, int esbits, class FT, PositSpec positspec, class Trait>
template <class T, int totalbits, int esbits, class FT, PositSpec positspec>
CONSTEXPR14 Posit<T,totalbits,esbits,FT,positspec>::Posit(int xvalue)
{
	using Trait=PT;
    using POSIT_UTYPE = typename PT::POSIT_UTYPE;
    using POSIT_STYPE = typename PT::POSIT_STYPE;
    using UT=Unpacked<FT,typename PT::exponenttype>;

    if(xvalue == 0)
    {
    	v = 0;
    }
    else
    {
	    bool negativeSign = xvalue < 0;
	    int value = xvalue < 0 ? -xvalue: xvalue;

		auto exponentF = rawexp - Trait::exponent_bias; // ((un.u >> Trait::fraction_bits) & Trait::exponent_mask)
	    auto fractionF = cast_right_to_left<typename Trait::holder_t,Trait::fraction_bits,FT,UT::FT_bits>()(value);

		if(rawexp == ((1 << Trait::exponent_bits)-1))
		{
			if(fractionF == 0)
			{
				return PP(typename PP::DeepInit(),negativeSign ? PT::POSIT_NINF : PT::POSIT_PINF);
			}
			else
			{
				return PP(typename PP::DeepInit(),PT::POSIT_NAN);
			}
		}
		else if (rawexp == 0)
	    {
	        if(fractionF == 0)
	        {
	            negativeSign = false;
	            return PP::zero();
	        }
	        else
	        {
	            int k = findbitleftmostC(fractionF);

	            exponentF -= k;
	            fractionF <<= k+1; // plus normalization
	        }	
	   	}

	    // Phase 3: compute low as regime (Unpacked_Low)

		auto eexponent = clamp<decltype(exponentF)>(exponentF,PT::minexponent,PT::maxexponent); // no overflow
		auto rr = PT::split_reg_exp(exponentF);
		auto fraction = cast_msb<FT,sizeof(FT)*8,typename PT::POSIT_UTYPE,sizeof(typename PT::POSIT_UTYPE)*8>()(fractionF); 
		auto reg = rr.first;
		auto exp = rr.second;

		// Phase 4: UnpackedLow to Posit

	    auto rs = std::max(-reg + 1, reg + 2); 
	    auto es = std::min((int)(totalbits-rs-1),(int)esbits);

	    POSIT_UTYPE regbits = reg < 0 ? (PT::POSIT_HOLDER_MSB >> -reg) : (PT::POSIT_MASK << (PT::POSIT_HOLDER_SIZE-(reg+1))); // reg+1 bits on the left
		POSIT_UTYPE eexp = msb_exp<POSIT_UTYPE,PT::POSIT_HOLDER_SIZE,esbits,(esbits == 00)>()(exp);
		POSIT_STYPE p = ((fraction >> (rs+es+1)) | (eexp >> (rs+1)) | (regbits>>1)) >> (sizeof(PP)*8-totalbits);

	    return PP(typename PP::DeepInit(),negativeSign ? -p : p);
	}
}
#endif
#ifndef FPGAHLS
template <class T, int totalbits, int esbits, class FT, PositSpec positspec>
std::ostream & operator << (std::ostream & ons, Posit<T,totalbits,esbits,FT,positspec> const & o)
{
	// mapping std iostream flags as Posit printing
	// - as float
	// - as raw
	// - as parsed
	auto af = ons.flags();
	auto f = af & std::ios::basefield;
	ons.setf(std::ios::dec, std::ios::basefield);
	if(f == std::ios::hex)
	{
		ons << o.v;
	}
	else if(f == std::ios::dec)
	{
		ons << (float)o;
	}
	else if(f == std::ios::oct)
	{
		ons << o.unpack();
	}	
	// reset
	ons.setf(af & std::ios::basefield, std::ios::basefield);
	return ons;
}
#endif


/// Level 1: -exponent of unpacked
/// Level 0: flip bits of rs
template <class T, int totalbits, int esbits, class FT, PositSpec positspec>
CONSTEXPR14 auto Posit<T,totalbits,esbits,FT,positspec>::inv() const -> Posit
{
	auto u = unpack_low();
	if(u.fraction == 0)
	{
		// 2^(R scale + E) with E unsigned
		// becomes -R scale + (scale-E)
		if(u.exp == 0)
			u.regime = -u.regime;
		else
		{
			u.regime = -(u.regime+1);
			u.exp = PT::POSIT_REG_SCALE-u.exp; // NOP if esbits == 0
		}
		return pack_low(u);
	}
	else
	{
		return pack_posit< T,totalbits,esbits,FT,positspec> (unpacked_low2full(u).inv());
	}
}

template <class T, int totalbits, int esbits, class FT, PositSpec positspec>
constexpr Posit<T,totalbits,esbits,FT,positspec> neg(Posit<T,totalbits,esbits,FT,positspec> x) { return -x; }

template <class T, int totalbits, int esbits, class FT, PositSpec positspec>
constexpr Posit<T,totalbits,esbits,FT,positspec> inv(Posit<T,totalbits,esbits,FT,positspec> x) { return ~x; }

template <class T, int hbits,int ebits, bool zeroes>
struct msb_exp
{

};

template <class T, int hbits,int ebits>
struct msb_exp<T,hbits,ebits,true>
{
	 constexpr T operator()(T) const
	{
		return 0;
	}
};

template <class T, int hbits,int ebits>
struct msb_exp<T,hbits,ebits,false>
{
 constexpr T operator()(T exp) const
	{
		return exp << (hbits-ebits);
	}

};

template <class T,int totalbits, int esbits, class FT, PositSpec positspec>
CONSTEXPR14 auto Posit<T,totalbits,esbits,FT,positspec>::unpack_low() const -> UnpackedLow
{
    using PT=PositTrait<T,totalbits,esbits,positspec>;
    using POSIT_UTYPE = typename PT::POSIT_UTYPE;
    //using POSIT_STYPE = typename PT::POSIT_STYPE;

	if(isinfinity()) // infinity
    {
    	return UnpackedLow(UnpackedT::Infinity, v < 0);
    }
    else if(isNaN())
   	{	
    	return UnpackedLow(UnpackedT::NaN);
   	}	
    else if(v == 0)
    	return UnpackedLow(UnpackedT::Zero);
	else
	{
        //constexpr int POSIT_RS_MAX = PT::POSIT_SIZE-1-esbits;

		//r.type = UnpackedT::Regular;
		bool negativeSign = (v & PT::POSIT_SIGNBIT) != 0;
		//std::cout << "unpacking " << std::bitset<sizeof(T)*8>(pa) << " abs " << std::bitset<sizeof(T)*8>(pcabs(pa)) << " r.negativeSign? " << r.negativeSign << std::endl;
        T pa = negativeSign ? -v : v;
	//	std::cout << "after " << std::hex << pa << std::endl;

        POSIT_UTYPE pars1 = pa << (PT::POSIT_EXTRA_BITS+1); // MSB: RS ES FS MSB
        auto q = PT::decode_posit_rs(pars1);
        int reg = q.first;
        int rs = q.second;
        POSIT_UTYPE pars2 = pars1 << rs; // MSB: ES FS
        POSIT_UTYPE exp = bitset_leftmost_get_const<POSIT_UTYPE,esbits>()(pars2); //        bitset_leftmost_getT(pars,esbits);
        POSIT_UTYPE pars = pars2 << esbits; // MSB: FS left aligned in T

        return UnpackedLow(negativeSign,reg,exp,pars);
        //r.fraction = pars;
       //std::cout << "fraction is " << std::bitset<sizeof(FT)*8>(r.fraction) << " with rs bits " << rs << " for reg " << reg << std::endl;
        //r.exp = exp;
        //r.regime = reg;
	}
}

template <class T,int totalbits, int esbits, class FT, PositSpec positspec>
CONSTEXPR14 auto Posit<T,totalbits,esbits,FT,positspec>::pack_low(UnpackedLow x) -> Posit
{
	using PP=Posit<T,totalbits,esbits,FT,positspec>;
    using PT=typename Posit<T,totalbits,esbits,FT,positspec>::PT;
    using POSIT_UTYPE = typename PT::POSIT_UTYPE;
    using POSIT_STYPE = typename PT::POSIT_STYPE;

    switch(x.type)
	{
		case UnpackedT::Infinity:
			// if infinity is missing return nan
			return positspec != PositSpec::WithNan ? (x.negativeSign ? PP::ninf(): PP::pinf()): PP::nan();
		case UnpackedT::Zero:
			return PP(typename PP::DeepInit(),0);
		case UnpackedT::NaN:
			// if nan is missing return infinity
			return positspec != PositSpec::WithInf ? PP::nan() : PP::pinf();
		default:
			break;
	}

	auto exp = x.exp;
	auto reg = x.regime;

    // for reg>=0: 1 0[reg+1] => size is reg+2 
    // for reg <0: 0[-reg] 0  => size is reg+1
    auto rs = -reg+1 > reg+2 ? -reg+1:reg+2; //std::max(-reg + 1, reg + 2);  MSVC issue
    auto es = (totalbits-rs-1) < esbits ? (totalbits-rs-1): esbits; //std::min((int)(totalbits-rs-1),(int)esbits);  MSVC issue
    

    POSIT_UTYPE regbits = reg < 0 ? (PT::POSIT_HOLDER_MSB >> -reg) : (PT::POSIT_MASK << (PT::POSIT_HOLDER_SIZE-(reg+1))); // reg+1 bits on the left
	POSIT_UTYPE eexp = msb_exp<POSIT_UTYPE,PT::POSIT_HOLDER_SIZE,esbits,(esbits == 00)>()(exp);
	POSIT_UTYPE fraction =  x.fraction;
	POSIT_STYPE p = ((fraction >> (rs+es+1)) | (eexp >> (rs+1)) | (regbits>>1)) >> (sizeof(T)*8-totalbits);
	//std::cout << "incoming " << x << std::endl;
	//std::cout << "fraction before " << std::bitset<sizeof(FT)*8>(x.fraction) << " and " << " after " << std::bitset<sizeof(POSIT_UTYPE)*8>(fraction) << " residual exponent " << exp << " from " << eexponent <<  " and regime " << reg << std::endl;
	//std::cout << "output sign " << std::bitset<sizeof(T)*8>(p) << " then " << std::bitset<sizeof(T)*8>(-p) << std::endl; 
    return PP(typename PP::DeepInit(),x.negativeSign ? -p : p);
}

template <class T,int totalbits, int esbits, class FT, PositSpec positspec>
CONSTEXPR14 auto Posit<T,totalbits,esbits,FT,positspec>::half() const -> Posit<T,totalbits,esbits,FT,positspec>
{
	UnpackedLow q = unpack_low();
	if(q.type == UnpackedT::Regular)
	{
		// +- 2^(R expmax + E) 1.xyz  == +- 2^(exp) 1.xyz
		// where xyz are decimal digits
		// 1.xyz / 2     => 0.1xyz ==> just exp--
		//
		// exp-- mean E-- if E s not null
		// otherwise R-- and exp 
		if(q.exp == 0)
		{
			q.regime--; // will it undrflow?
			q.exp = PT::POSIT_REG_SCALE-1; // maximum exponent
		}
		else
		{
			q.exp--;
		}
		return pack_low(q);
	}
	else
	{
		return *this;
	}
}

template <class T,int totalbits, int esbits, class FT, PositSpec positspec>
CONSTEXPR14 auto Posit<T,totalbits,esbits,FT,positspec>::twice() const -> Posit<T,totalbits,esbits,FT,positspec>
{
	UnpackedLow q = unpack_low();
	if(q.type == UnpackedT::Regular)
	{
		// +- 2^(R expmax + E) 1.xyz  == +- 2^(exp) 1.xyz
		// where xyz are decimal digits
		// 1.xyz / 2     => 0.1xyz ==> just exp--
		//
		// exp-- mean E-- if E s not null
		// otherwise R-- and exp 
		if(q.exp == PT::POSIT_REG_SCALE-1)
		{
			q.regime++; // will it overflo??
			q.exp = 0; // maximum exponent
		}
		else
		{
			q.exp++;
		}
		return pack_low(q);
	}
	else
	{
		return *this;
	}
}
template <class T,int totalbits, int esbits, class FT, PositSpec positspec>
CONSTEXPR14 auto Posit<T,totalbits,esbits,FT,positspec>::unpacked_low2full(UnpackedLow q) -> UnpackedT
{
    using POSIT_UTYPE = typename PT::POSIT_UTYPE;
    UnpackedT r;
    r.type = q.type;
    r.negativeSign = q.negativeSign;

    if(q.type == UnpackedT::Regular)
    {
        r.fraction = cast_msb<POSIT_UTYPE,PT::POSIT_HOLDER_SIZE,FT,UnpackedT::FT_bits>()(q.fraction);
        r.exponent = PT::join_reg_exp(q.regime,q.exp);
    }
	return r;
}

template <class T,int totalbits, int esbits, class FT, PositSpec positspec>
CONSTEXPR14 auto Posit<T,totalbits,esbits,FT,positspec>::unpacked_full2low(UnpackedT x) -> UnpackedLow
{
	if(x.type == UnpackedT::Regular)
	{
		auto eexponent = clamp<decltype(x.exponent)>(x.exponent,PT::minexponent(),PT::maxexponent()); // no overflow
		auto rr = PT::split_reg_exp(eexponent);
		auto frac = cast_msb<FT,sizeof(FT)*8,typename PT::POSIT_UTYPE,sizeof(typename PT::POSIT_UTYPE)*8>()(x.fraction);
		return UnpackedLow(x.negativeSign,rr.first,rr.second,frac);
	}
	else
	{
		return UnpackedLow(x.type,x.negativeSign);
	}
}


template <class T,int totalbits, int esbits, class FT, PositSpec positspec>
CONSTEXPR14 Posit<T,totalbits,esbits,FT,positspec> pack_posit(const typename Posit<T,totalbits,esbits,FT,positspec>::UnpackedT & x)
{
	using PP=Posit<T,totalbits,esbits,FT,positspec>;
	return PP::pack_low(PP::unpacked_full2low(x));
}


template <class T,int totalbits, int esbits, class FT, PositSpec positspec>
auto Posit<T,totalbits,esbits,FT,positspec>::analyze() -> info
{
	using UT=UnpackedT;
    using POSIT_UTYPE = typename PT::POSIT_UTYPE;
    //using POSIT_STYPE = typename PT::POSIT_STYPE;

    
    	auto pa = v;
	info i;	
	if(isinfinity())
    {
    	if(positspec == PositSpec::WithNanInf)
			i.sign = (pa & PT::POSIT_SIGNBIT) != 0;
    	i.infinity = true;
    	return i;
    }
    else if(isNaN())
    {
    	i.nan = true;
    	return i;
    }
    else if(v == 0)
    {
    	return i;
    }
    else
    {
        //constexpr int POSIT_RS_MAX = PT::POSIT_SIZE-1-esbits;

		i.sign = (pa & PT::POSIT_SIGNBIT) != 0;
        pa = pcabs(pa);
        POSIT_UTYPE pars = pa << (PT::POSIT_EXTRA_BITS+1); // output MSB: RS ES FS 
        auto q = PT::decode_posit_rs(pars);	
        int reg = q.first;
        int rs = q.second;
        pars <<= rs; // MSB: ES FS
        POSIT_UTYPE exp = bitset_leftmost_getT(pars,esbits);
        pars <<= esbits; // output MSB: FS left aligned in T
        //std::cout << std::bitset<PT::POSIT_HOLDER_SIZE>(pars) << std::endl;
        i.ifraction = sizeof(FT) >= sizeof(T) ? pars << (UT::FT_bits-PT::POSIT_HOLDER_SIZE) : pars >> (PT::POSIT_HOLDER_SIZE-UT::FT_bits); // output: FS left aligned in FT (larger or equal to T)
    	i.exponent = PT::join_reg_exp(reg,exp);
    	i.exp = exp;
    	i.rs = rs;
    	i.k = reg;
    	i.es = totalbits-rs-1 < esbits ? totalbits-rs-1 : esbits; // std::min((int)(totalbits-rs-1),(int)esbits); MSVC issue
    	i.fs = totalbits-rs-i.es-1; 
    	return i;
    }

}



template <class T,int totalbits, int esbits, class FT, PositSpec positspec>
CONSTEXPR14 auto unpack_posit(const Posit<T,totalbits,esbits,FT,positspec> & p) -> typename Posit<T,totalbits,esbits,FT,positspec>::UnpackedT 
{
	using PP=Posit<T,totalbits,esbits,FT,positspec>;
	return PP::unpacked_low2full(p.unpack_low());
}

#ifndef FPGAHLS


#endif

namespace std
{
	template <class T,int totalbits, int esbits, class FT, PositSpec positspec>
	inline CONSTEXPR14 Posit<T,totalbits,esbits,FT,positspec> abs(Posit<T,totalbits,esbits,FT,positspec> z) 
	{
		using PP=Posit<T,totalbits,esbits,FT,positspec>;
		return PP(PP::DeepInit(),pcabs(z.v));
	}

	template <class T,int totalbits, int esbits, class FT, PositSpec positspec>
	inline CONSTEXPR14 Posit<T,totalbits,esbits,FT,positspec> sqrt(Posit<T,totalbits,esbits,FT,positspec> z) 
	{
		using PP=Posit<T,totalbits,esbits,FT,positspec>;
		return PP(PP::DeepInit(),pcabs(z.v));
	}

	template <class T,int totalbits, int esbits, class FT, PositSpec positspec>
	inline CONSTEXPR14 Posit<T,totalbits,esbits,FT,positspec> min(Posit<T,totalbits,esbits,FT,positspec> a, Posit<T,totalbits,esbits,FT,positspec> b)
	{
		return a <=  b ? a : b;
	}

	template <class T,int totalbits, int esbits, class FT, PositSpec positspec>
	inline CONSTEXPR14 Posit<T,totalbits,esbits,FT,positspec> max(Posit<T,totalbits,esbits,FT,positspec> a, Posit<T,totalbits,esbits,FT,positspec> b)
	{
		return a >= b ? a : b;
	}

	template <class B,int totalbits, int esbits, class FT, PositSpec positspec> class numeric_limits<Posit<B,totalbits,esbits,FT,positspec> > {
	public:
	  using T=Posit<B,totalbits,esbits,FT,positspec>;
	  using PT=typename T::PT;
	  static constexpr bool is_specialized = true;
	  static constexpr T min() noexcept { return T::min(); }
	  static constexpr T max() noexcept { return T::max(); }
	  static constexpr T lowest() noexcept { return T::lowest	(); }
	  //static constexpr int  digits = 0; number of digits (in radix base) in the mantissa 
	  static constexpr int  digits10 = ((totalbits-3)*30000)/100000;  // *log10(2)
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
	  static constexpr bool has_quiet_NaN = positspec != PositSpec::WithInf;
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
	  //static constexpr float_round_style round_style = round_toward_zero;
	  /*
	  round_toward_zero, if it rounds toward zero.
round_to_nearest, if it rounds to the nearest representable value.
round_toward_infinity, if it rounds toward infinity.
round_toward_neg_infinity, if it rounds toward negative infinity.
round_indeterminate, if the rounding style is indeterminable at compile time.
*/
	};

}


