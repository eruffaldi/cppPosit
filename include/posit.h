	/**
 * Emanuele Ruffaldi (C) 2017
 * Templated C++ Posit

 Test

#include "posit.h"
using X=Posit<int32_t,4,0,uint32_t>;
X::PT::decode_posit_rs(1)
 */
#pragma once
#include "unpacked.h"


inline float uint32_to_float(uint32_t i)
{
	union {
		float f;
		uint32_t i;
	} x;
	x.i = i;
	return x.f;
}

template <class T, int totalbits, int esbits, bool withnan_ >
struct PositTrait
{
	static_assert(std::is_signed<T>::value,"required signed T");
	static_assert(sizeof(T)*8 >= totalbits,"required enough storage T for provided bits  SREF");
	static_assert(esbits <= totalbits-3,"esbits should be at most N-3 for the cases [x01,E] and [x10,E]");

	using POSIT_STYPE = typename std::make_signed<T>::type;
	using POSIT_UTYPE = typename std::make_unsigned<T>::type;
	static constexpr bool withnan = withnan_;
	using exponenttype = typename std::conditional<(totalbits+esbits >= sizeof(T)*8),typename  nextinttype<T>::type,T>::type;

	enum : POSIT_UTYPE {
		POSIT_MAXREGIME_BITS = totalbits-1,
		POIST_ONEHELPER = 1,
		POSIT_HOLDER_SIZE = sizeof(T)*8, 
		POSIT_SIZE = totalbits,
		POSIT_ESP_SIZE = esbits,
		POSIT_MSB = POIST_ONEHELPER<<(totalbits-1),
		POSIT_HOLDER_MSB = POIST_ONEHELPER<<(POSIT_HOLDER_SIZE-1),
        POSIT_MASK = ((POSIT_MSB-1)|(POSIT_MSB)),
        POSIT_ESP_MASK = (POIST_ONEHELPER<< esbits)-1,
        //POSIT_HOLDER_MSB = 1U<<(POSIT_HOLDER_SIZE-1),
        //POSIT_HOLDER_MASK = ((POSIT_HOLDER_SIZE-1)|(POSIT_HOLDER_SIZE)),
		POSIT_EXTRA_BITS = POSIT_HOLDER_SIZE-POSIT_SIZE,
		POSIT_SIGNBIT = (POIST_ONEHELPER<<(POSIT_SIZE-1)), // bit
		POSIT_INVERTBIT = (POIST_ONEHELPER<<(POSIT_SIZE-2)),

	};
	enum : POSIT_STYPE {
		POSIT_REG_SCALE = 1<<esbits,

		_POSIT_TOP = (POSIT_STYPE)((POSIT_UTYPE(~0) << (totalbits-1))),
		_POSIT_TOPRIGHT = (POSIT_STYPE)((POIST_ONEHELPER<< (totalbits-1))-1),
		_POSIT_TOPLEFT = (POSIT_STYPE)((POSIT_UTYPE(~0) << (totalbits-1)))+1,

		// 10000000 
		POSIT_PINF =  withnan_ ? _POSIT_TOPRIGHT: _POSIT_TOP , // 1[sign] 000000 or N-1 111 bits
		POSIT_NINF =  withnan_ ? _POSIT_TOPLEFT: _POSIT_TOP,
		POSIT_NAN  = _POSIT_TOP,  // infinity in withnan=false otherwise it is truly nan
		POSIT_ONE =  POSIT_INVERTBIT, // fine due to position of invert bit
		POSIT_MONE = -POSIT_ONE ,// signed

		POSIT_TWO = (POSIT_INVERTBIT | (POSIT_INVERTBIT>>1)),

		// 00 1[esbits+1] 0[N-2-esbitis-1]
		POSIT_HALF = POSIT_STYPE( (POSIT_UTYPE(-1) >> (totalbits-esbits-1))) << (totalbits-3-esbits),
		
		// 1[holder-total] 1 0[total-1]
		POSIT_MAX = _POSIT_TOPRIGHT - (withnan_ ? 1:0),
		// 0[holder-total] 0 1[total-1]
		POSIT_MIN = _POSIT_TOPLEFT + (withnan_? 1:0),

		POSIT_SMALLPOS = 1, // right to 0
		POSIT_SMALLNEG = -POSIT_SMALLPOS, // left to 0

	};

	enum: exponenttype { 
		maxexponent = withnan_ ? POSIT_REG_SCALE * (POSIT_SIZE - 3) : POSIT_REG_SCALE * (POSIT_SIZE - 2),  // sign+1st rs
		minexponent = (-((exponenttype)POSIT_REG_SCALE) * (POSIT_SIZE - 2))  // sign+1st rs
	};

    //static constexpr POSIT_UTYPE LMASK(POSIT_UTYPE bits, POSIT_UTYPE size)
    //{ return ((bits) & (POSIT_MASK << (POSIT_SIZE - (size)))); }


    // pars is T_left
    static CONSTEXPR14 std::pair<int,int> decode_posit_rs(T pars)
    {  
        const bool x = (pars & POSIT_HOLDER_MSB) != 0; // marker bit for > 1
        int aindex = x ? (~pars == 0 ? POSIT_MAXREGIME_BITS : findbitleftmostC((POSIT_UTYPE)~pars)) : (pars == 0 ? POSIT_MAXREGIME_BITS : findbitleftmostC((POSIT_UTYPE)pars)); // index is LAST with !x
        int index = aindex; // aindex > POSIT_SIZE  ? POSIT_SIZE : aindex;
        int reg = x ? index-1 : -index;
        int rs =  std::min((int)POSIT_MAXREGIME_BITS,index+1);
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

template <class T,int totalbits, int esbits, class FT, bool withnan>
class Posit;

template <class T,int totalbits, int esbits, class FT, bool withnan>
CONSTEXPR14 auto unpack_posit(const Posit<T,totalbits,esbits,FT,withnan> & p) -> typename Posit<T,totalbits,esbits,FT,withnan>::UnpackedT ;

template <class T,int totalbits, int esbits, class FT, bool withnan>
CONSTEXPR14 Posit<T,totalbits,esbits,FT,withnan> pack_posit(const typename Posit<T,totalbits,esbits,FT,withnan>::UnpackedT & x);

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
template <class T,int totalbits, int esbits, class FT, bool withnan>
class Posit
{
public:
	using PT=PositTrait<T,totalbits,esbits,withnan>;
	static_assert(std::is_unsigned<FT>::value,"required unsigned FT");


    enum { vtotalbits = totalbits, vesbits = esbits};
	struct DeepInit{};
    using value_t=T;
    using fraction_t=FT;
    using UnpackedT=Unpacked<FT,typename PT::exponenttype>;
	using exponenttype = typename PT::exponenttype;
	T v; // index in the N2 space

	struct PositMul
	{
		constexpr PositMul(Posit av, Posit bv) : a(av),b(bv) {}
		Posit a,b;

		constexpr operator Posit() const { return pack_posit<T,totalbits,esbits,FT,withnan>(a.unpack()*b.unpack()); }

		// missing operators
		// &
		// -
	};

	/**
	 * Minimal Unpacked representaiton of the Posit
	 */
	struct UnpackedLow
	{
		using Type = typename UnpackedT::Type;

		constexpr UnpackedLow(Type t): type(t) {}
		constexpr UnpackedLow(Type t, bool anegativeSign): type(t), negativeSign(anegativeSign) {}
		constexpr UnpackedLow(bool n, typename PT::POSIT_STYPE r, typename PT::POSIT_UTYPE e, typename PT::POSIT_UTYPE f):
			negativeSign(n), regime(r), exp(e), fraction(f), type(UnpackedT::Regular) {}

		Type type;
		bool negativeSign; // for Regular and Infinity if applicabl
		typename PT::POSIT_STYPE regime; // decoded with sign
		typename PT::POSIT_UTYPE exp;    // decoded
		typename PT::POSIT_UTYPE fraction; // fraction left aligned
	};

	CONSTEXPR14 Posit half() const;
	CONSTEXPR14 Posit twice() const;
	CONSTEXPR14 UnpackedLow unpack_low() const;
	static CONSTEXPR14 Posit pack_low(UnpackedLow);
	static CONSTEXPR14 UnpackedT unpacked_low2full(UnpackedLow x);
	static CONSTEXPR14 UnpackedLow unpacked_full2low(UnpackedT x);


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


	Posit() {}



    friend constexpr bool operator == (const Posit & a, const Posit & u)  { return a.v == u.v; }
    friend constexpr bool operator != (const Posit & a, const Posit & u)  { return a.v != u.v; }
    friend constexpr bool operator < (const Posit & a, const Posit & u)  { return a.v < u.v; }
    friend constexpr bool operator <= (const Posit & a, const Posit & u)  { return a.v <= u.v; }

    friend constexpr bool operator > (const Posit & a, const Posit & u)  { return a.v > u.v; }
    friend constexpr bool operator >= (const Posit & a, const Posit & u)  { return a.v >= u.v; }

    static constexpr Posit ldexp(const Posit & u, int exp); // exponent product

    CONSTEXPR14 explicit Posit(float f) { v = pack_posit<T,totalbits,esbits,FT,withnan>(UnpackedT(f)).v;  }
	CONSTEXPR14 explicit Posit(double d) { v = pack_posit<T,totalbits,esbits,FT,withnan>(UnpackedT(d)).v;  }
	CONSTEXPR14 explicit Posit(int f) { v = pack_posit<T,totalbits,esbits,FT,withnan>(UnpackedT((double)f)).v; }
	CONSTEXPR14 explicit Posit(DeepInit, T x) : v(x) {} 
	CONSTEXPR14 explicit Posit(UnpackedT u) : v(pack_posit<T,totalbits,esbits,FT,withnan>(u).v) {} 
	CONSTEXPR14 explicit Posit(UnpackedLow u) : v(pack_low(u).v) {} 

	constexpr UnpackedT unpack() const { return unpack_posit<T,totalbits,esbits,FT,withnan>(*this); }

	constexpr Posit abs()  const { return Posit(DeepInit(),(v < 0 ? -v : v));  }  // could be >= infinity because infinity is sign symmetric
	constexpr Posit neg()  const { return Posit(DeepInit(),-v); }; 
	CONSTEXPR14 Posit inv()  const;

	// SFINAE optionally: template<typename U = T, class = typename std::enable_if<withnan, U>::type>
    constexpr bool hasNaN() const { return withnan; }
	constexpr bool isNaN() const { return withnan && v == PT::POSIT_NAN; } 
	constexpr bool isnegative() const { return v < 0; } //(v &POSIT_SIGNBIT) != 0; }
	constexpr bool isinfinity() const { return v == PT::POSIT_PINF || v == PT::POSIT_NINF; }
	constexpr bool iszero() const { return v == 0; }
	constexpr bool isone() const { return v == PT::POSIT_ONE; }
	//TBDconstexpr bool isNaN() const; 
	//TBD constexpr bool isexact() const { return (v&1) == 0; }

	//TBD constexpr bool isfractional() const { return v > 0 && (abs().v < (N2>>2)); } // (0 < x < 1) or  (-1 < x < 0) == (-1,1) removing 0
	//TBD constexpr bool isstrictlynegative() const { return v > (N2>>1); } // -inf < x < 0

	    // Level 1: unpacked
	// Level 0: something using posit specialties
	friend constexpr PositMul operator*(const Posit & a, const Posit & b) 
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

	friend constexpr Posit fma(const Posit & a, const Posit & b, const Posit & c)
	{
		return pack_posit<T,totalbits,esbits,FT,withnan>(a.unpack()*b.unpack()+c.unpack());
	}

	CONSTEXPR14 Posit & operator*= (const Posit & b)
	{
		*this = pack_posit<T,totalbits,esbits,FT,withnan>(unpack()*b.unpack());
		return *this;
	}
    friend constexpr Posit operator+(const Posit & a, const Posit & b)
    {
        return a.iszero() ? b : b.iszero() ? a: pack_posit<T,totalbits,esbits,FT,withnan>(a.unpack()+b.unpack());
    }

	constexpr Posit& operator+=(const Posit &a) { Posit r = *this+a; v = r.v; return *this; }

	static constexpr Posit zero() { return Posit(DeepInit(),0); }
	static constexpr Posit inf() { return Posit(DeepInit(),PT::POSIT_PINF); }
	static constexpr Posit pinf() { return Posit(DeepInit(),PT::POSIT_PINF); }
	static constexpr Posit ninf() { return Posit(DeepInit(),PT::POSIT_NINF); }

	// SFINAE optionally: template<typename U = T, class = typename std::enable_if<withnan, U>::type>
	static constexpr Posit nan() { return Posit(DeepInit(),PT::POSIT_NAN); }
	static constexpr Posit one() { return Posit(DeepInit(),PT::POSIT_ONE); }
	static constexpr Posit two() { return Posit(DeepInit(),PT::POSIT_TWO); }
	static constexpr Posit mone() { return Posit(DeepInit(),PT::POSIT_MONE); }
	static constexpr Posit onehalf() { return Posit(DeepInit(),PT::POSIT_HALF); }

	// custom operators
	constexpr Posit operator-() const { return neg(); } 
	constexpr Posit operator~() const { return inv(); } 
	friend constexpr Posit operator-(const Posit & a, const Posit & b)  { return a + (-b); }
	friend constexpr Posit operator/(const Posit & a, const Posit & b)  { return pack_posit< T,totalbits,esbits,FT,withnan > (a.unpack()/b.unpack()); }


   
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
	constexpr operator float() const { return unpack(); }
	constexpr operator double() const { return unpack(); }
	constexpr operator UnpackedT() const { return unpack(); }

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

	struct FullWriter
	{
		FullWriter(T x): ax(x) {}
		T ax;
	};	

	FullWriter describe() const { return FullWriter(v); }

	friend std::ostream & operator << (std::ostream &ons, const FullWriter & w)
	{
		return ons;
	}

};

template <class T, int totalbits, int esbits, class FT, bool withnan>
std::ostream & operator << (std::ostream & ons, Posit<T,totalbits,esbits,FT,withnan> const & o)
{
	ons << o.unpack();
	return ons;
}




/// Level 1: -exponent of unpacked
/// Level 0: flip bits of rs
template <class T, int totalbits, int esbits, class FT, bool withnan>
CONSTEXPR14 auto Posit<T,totalbits,esbits,FT,withnan>::inv() const -> Posit
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
		return pack_posit< T,totalbits,esbits,FT,withnan> (unpacked_low2full(u).inv());
	}
}

template <class T, int totalbits, int esbits, class FT, bool withnan>
constexpr Posit<T,totalbits,esbits,FT,withnan> neg(Posit<T,totalbits,esbits,FT,withnan> x) { return -x; }

template <class T, int totalbits, int esbits, class FT, bool withnan>
constexpr Posit<T,totalbits,esbits,FT,withnan> inv(Posit<T,totalbits,esbits,FT,withnan> x) { return ~x; }

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

template <class T,int totalbits, int esbits, class FT, bool withnan>
CONSTEXPR14 auto Posit<T,totalbits,esbits,FT,withnan>::unpack_low() const -> UnpackedLow
{
    using PT=PositTrait<T,totalbits,esbits,withnan>;
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
		//std::cout << "unpacking " << std::bitset<sizeof(T)*8>(pa) << " abs " << std::bitset<sizeof(T)*8>(pabs(pa)) << " r.negativeSign? " << r.negativeSign << std::endl;
        T pa = negativeSign ? -v : v;
	//	std::cout << "after " << std::hex << pa << std::endl;

        POSIT_UTYPE pars1 = pa << (PT::POSIT_EXTRA_BITS+1); // MSB: RS ES FS MSB
        auto q = PT::decode_posit_rs(pars1);
        int reg = q.first;
        int rs = q.second;
        POSIT_UTYPE pars2 = pars1 << rs; // MSB: ES FS
        POSIT_UTYPE exp = bitset_leftmost_get_const<T,esbits>()(pars2); //        bitset_leftmost_getT(pars,esbits);
        POSIT_UTYPE pars = pars2 << esbits; // MSB: FS left aligned in T

        return UnpackedLow(negativeSign,reg,exp,pars);
        //r.fraction = pars;
       //std::cout << "fraction is " << std::bitset<sizeof(FT)*8>(r.fraction) << " with rs bits " << rs << " for reg " << reg << std::endl;
        //r.exp = exp;
        //r.regime = reg;
	}
}

template <class T,int totalbits, int esbits, class FT, bool withnan>
CONSTEXPR14 auto Posit<T,totalbits,esbits,FT,withnan>::pack_low(UnpackedLow x) -> Posit
{
	using PP=Posit<T,totalbits,esbits,FT,withnan>;
    using PT=typename Posit<T,totalbits,esbits,FT,withnan>::PT;
    using POSIT_UTYPE = typename PT::POSIT_UTYPE;
    using POSIT_STYPE = typename PT::POSIT_STYPE;

    switch(x.type)
	{
		case UnpackedT::Infinity:
			return PT::withnan ? (x.negativeSign ? PP::ninf(): PP::pinf()): PP::inf();
		case UnpackedT::Zero:
			return PP(typename PP::DeepInit(),0);
		case UnpackedT::NaN:
			return PP::nan();
		default:
			break;
	}

	auto exp = x.exp;
	auto reg = x.regime;

    // for reg>=0: 1 0[reg+1] => size is reg+2 
    // for reg <0: 0[-reg] 0  => size is reg+1
    auto rs = std::max(-reg + 1, reg + 2); 
    auto es = std::min((int)(totalbits-rs-1),(int)esbits);


    POSIT_UTYPE regbits = reg < 0 ? (PT::POSIT_HOLDER_MSB >> -reg) : (PT::POSIT_MASK << (PT::POSIT_HOLDER_SIZE-(reg+1))); // reg+1 bits on the left
	POSIT_UTYPE eexp = msb_exp<POSIT_UTYPE,PT::POSIT_HOLDER_SIZE,esbits,(esbits == 00)>()(exp);
	POSIT_UTYPE fraction =  x.fraction;
	POSIT_STYPE p = ((fraction >> (rs+es+1)) | (eexp >> (rs+1)) | (regbits>>1)) >> (sizeof(T)*8-totalbits);
	//std::cout << "incoming " << x << std::endl;
	//std::cout << "fraction before " << std::bitset<sizeof(FT)*8>(x.fraction) << " and " << " after " << std::bitset<sizeof(POSIT_UTYPE)*8>(fraction) << " residual exponent " << exp << " from " << eexponent <<  " and regime " << reg << std::endl;
	//std::cout << "output sign " << std::bitset<sizeof(T)*8>(p) << " then " << std::bitset<sizeof(T)*8>(-p) << std::endl; 
    return PP(typename PP::DeepInit(),x.negativeSign ? -p : p);
}

template <class T,int totalbits, int esbits, class FT, bool withnan>
CONSTEXPR14 auto Posit<T,totalbits,esbits,FT,withnan>::half() const -> Posit<T,totalbits,esbits,FT,withnan>
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

template <class T,int totalbits, int esbits, class FT, bool withnan>
CONSTEXPR14 auto Posit<T,totalbits,esbits,FT,withnan>::twice() const -> Posit<T,totalbits,esbits,FT,withnan>
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
template <class T,int totalbits, int esbits, class FT, bool withnan>
CONSTEXPR14 auto Posit<T,totalbits,esbits,FT,withnan>::unpacked_low2full(UnpackedLow q) -> UnpackedT
{
    using POSIT_UTYPE = typename PT::POSIT_UTYPE;
    UnpackedT r;
    r.type = q.type;
    r.negativeSign = q.negativeSign;

    if(q.type == UnpackedT::Regular)
    {
        r.fraction = cast_msb<POSIT_UTYPE,PT::POSIT_HOLDER_SIZE,FT,UnpackedT::POSIT_FRAC_TYPE_SIZE_BITS>()(q.fraction);
        r.exponent = PT::join_reg_exp(q.regime,q.exp);
    }
	return r;
}

template <class T,int totalbits, int esbits, class FT, bool withnan>
CONSTEXPR14 auto Posit<T,totalbits,esbits,FT,withnan>::unpacked_full2low(UnpackedT x) -> UnpackedLow
{
	if(x.type == UnpackedT::Regular)
	{
		auto eexponent = clamp<decltype(x.exponent)>(x.exponent,PT::minexponent,PT::maxexponent); // no overflow
		auto rr = PT::split_reg_exp(eexponent);
		auto frac = cast_msb<FT,sizeof(FT)*8,typename PT::POSIT_UTYPE,sizeof(typename PT::POSIT_UTYPE)*8>()(x.fraction);
		return UnpackedLow(x.negativeSign,rr.first,rr.second,frac);
	}
	else
	{
		return UnpackedLow(x.type,x.negativeSign);
	}
}


template <class T,int totalbits, int esbits, class FT, bool withnan>
CONSTEXPR14 Posit<T,totalbits,esbits,FT,withnan> pack_posit(const typename Posit<T,totalbits,esbits,FT,withnan>::UnpackedT & x)
{
	using PP=Posit<T,totalbits,esbits,FT,withnan>;
	return PP::pack_low(PP::unpacked_full2low(x));
}


template <class T,int totalbits, int esbits, class FT, bool withnan>
auto Posit<T,totalbits,esbits,FT,withnan>::analyze() -> info
{
	using UT=UnpackedT;
    using POSIT_UTYPE = typename PT::POSIT_UTYPE;
    //using POSIT_STYPE = typename PT::POSIT_STYPE;

    
    	auto pa = v;
	info i;	
	if(isinfinity())
    {
    	if(PT::withnan)
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
        pa = pa < 0 ? -pa : pa;
        POSIT_UTYPE pars = pa << (PT::POSIT_EXTRA_BITS+1); // output MSB: RS ES FS 
        auto q = PT::decode_posit_rs(pars);	
        int reg = q.first;
        int rs = q.second;
        pars <<= rs; // MSB: ES FS
        POSIT_UTYPE exp = bitset_leftmost_getT(pars,esbits);
        pars <<= esbits; // output MSB: FS left aligned in T
        //std::cout << std::bitset<PT::POSIT_HOLDER_SIZE>(pars) << std::endl;
        i.ifraction = sizeof(FT) >= sizeof(T) ? pars << (UT::POSIT_FRAC_TYPE_SIZE_BITS-PT::POSIT_HOLDER_SIZE) : pars >> (PT::POSIT_HOLDER_SIZE-UT::POSIT_FRAC_TYPE_SIZE_BITS); // output: FS left aligned in FT (larger or equal to T)
    	i.exponent = PT::join_reg_exp(reg,exp);
    	i.exp = exp;
    	i.rs = rs;
    	i.k = reg;
    	i.es = std::min((int)(totalbits-rs-1),(int)esbits);
    	i.fs = totalbits-rs-i.es-1; 
    	return i;
    }

}


template <class T,int totalbits, int esbits, class FT, bool withnan>
CONSTEXPR14 auto unpack_posit(const Posit<T,totalbits,esbits,FT,withnan> & p) -> typename Posit<T,totalbits,esbits,FT,withnan>::UnpackedT 
{
	using PP=Posit<T,totalbits,esbits,FT,withnan>;
	return PP::unpacked_low2full(p.unpack_low());
}

template <class X>
void printinfo(std::ostream & ons, typename X::value_t v)
{
	using Q= typename printableinttype<typename X::value_t>::type;
	X x(typename X::DeepInit(),v); // load the posit OK
	typename X::UnpackedT u(x.unpack()); // unpack it OK
	X xux(u); // pack
    typename X::info ii = x.analyze();
    if(ii.infinity)
    	ons << (X::PT::withnan ? (ii.sign ? "posit(-infinity)" : "posit(+infinity)") : "posit(infinity)");
    else if(ii.nan)
    	ons << "posit(nan)";
    else
    {
    	ons << " posit(" << (ii.sign ? "-" : "+") ;
    	ons << " rs/es/fs:" << std::dec <<  ii.rs << "/" << ii.es << "/" << ii.fs << " ";
    	ons << " k:" << std::dec << (Q)ii.k ;
    	ons << " exp:" << std::dec << (1<<ii.exp);
    	ons << " ifraction:" << std::hex << (Q)ii.ifraction;
        ons << " binary:" << std::bitset<sizeof(typename X::value_t)*8>(xux.v) << ")";
     }	
}

template <class T>
struct posit_formatter
{
public:
	posit_formatter(T p): posit(p) {}

	friend std::ostream & operator << (std::ostream & ons, const posit_formatter & x)
	{
		printinfo<T>(ons,x.posit.v);
		return ons;
	}

	T posit;
};

namespace std
{
	template <class T,int totalbits, int esbits, class FT, bool withnan>
	inline CONSTEXPR14 Posit<T,totalbits,esbits,FT,withnan> abs(Posit<T,totalbits,esbits,FT,withnan> z) 
	{
		using PP=Posit<T,totalbits,esbits,FT,withnan>;
		return PP(PP::DeepInit(),z.v < 0 ? -z.v : z.v);
	}

	template <class T,int totalbits, int esbits, class FT, bool withnan>
	inline CONSTEXPR14 Posit<T,totalbits,esbits,FT,withnan> min(Posit<T,totalbits,esbits,FT,withnan> a, Posit<T,totalbits,esbits,FT,withnan> b)
	{
		return a <=  b ? a : b;
	}

	template <class T,int totalbits, int esbits, class FT, bool withnan>
	inline CONSTEXPR14 Posit<T,totalbits,esbits,FT,withnan> max(Posit<T,totalbits,esbits,FT,withnan> a, Posit<T,totalbits,esbits,FT,withnan> b)
	{
		return a >= b ? a : b;
	}
}


