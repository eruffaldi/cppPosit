#pragma once

#include "posit.h"

// TODO: make it specialization of Posit natively
template <class T,int totalbits, int esbits, class FFT, PositSpec positspec>
class PositF
{
public:
	static_assert(std::is_floating_point<FFT>::value,"required floating pointer repr");
	using PT=PositTrait<T,totalbits,esbits,positspec>;
	//static_assert(float2trait<FFT>::trait::exponent_max <= PT::maxexponent(),"proposed floating is not sufficient for resulting posit max");

    enum { vtotalbits = totalbits, vesbits = esbits};
	struct DeepInit{};
	using value_t=T;
    using FT=typename float2trait<FFT>::trait::holder_t;
    using UnpackedT=Unpacked<FT,typename PT::exponenttype>;
    using PPT=Posit<T,totalbits,esbits,FT,positspec>; // friendly posit
    using UnpackedLow = UnpackedLow_t<UnpackedT,PT>;
    using fraction_t=FT;
    using exponenttype = typename PT::exponenttype;
	
	T v; // index in the N2 space

	PositF half() const { return PositF(0.5*(FFT)unpack()); }
	PositF twice() const { return PositF(2*(FFT)unpack()); }
	
	CONSTEXPR14 UnpackedLow unpack_low() const 
	{ 
		return PPT(PPT::DeepInit(),v).unpack_low(); 
	}

	static CONSTEXPR14 PositF pack_low(UnpackedLow u) 
	{
		return PositF(DeepInit(),(PPT::pack_low(u)).v);
	}

	static CONSTEXPR14 UnpackedT unpacked_low2full(UnpackedLow x)
	{
		return PPT::unpacked_low2full(x);
	}

	static CONSTEXPR14 UnpackedLow unpacked_full2low(UnpackedT x)
	{
		return PPT::unpacked_full2low(x);
	}

	static CONSTEXPR14 PositF pack_posit( UnpackedT  x)
	{
		return pack_low(unpacked_full2low(x));
	}

	static CONSTEXPR14 UnpackedT unpack_posit( PositF  x)
	{
		return unpacked_low2full(x.unpack_low());
	}

	PositF() {}

    friend constexpr bool operator == (const PositF & a, const PositF & u)  { return a.v == u.v; }
    friend constexpr bool operator != (const PositF & a, const PositF & u)  { return a.v != u.v; }
    friend constexpr bool operator < (const PositF & a, const PositF & u)  { return a.v < u.v; }
    friend constexpr bool operator <= (const PositF & a, const PositF & u)  { return a.v <= u.v; }

    friend constexpr bool operator > (const PositF & a, const PositF & u)  { return a.v > u.v; }
    friend constexpr bool operator >= (const PositF & a, const PositF & u)  { return a.v >= u.v; }

    static constexpr PositF ldexp(const PositF & u, int exp); // exponent product

    CONSTEXPR14 explicit PositF(float f) { v = pack_posit(UnpackedT(f)).v;  }
	CONSTEXPR14 explicit PositF(double d) { v = pack_posit(UnpackedT(d)).v;  }
	CONSTEXPR14 explicit PositF(int f) { v = pack_posit(UnpackedT((double)f)).v; }
	CONSTEXPR14 explicit PositF(DeepInit, T x) : v(x) {} 
	CONSTEXPR14 explicit PositF(UnpackedT u) : v(pack_posit(u).v) {} 
	CONSTEXPR14 explicit PositF(UnpackedLow u) : v(pack_low(u).v) {} 

	constexpr UnpackedT unpack() const { return PPT(typename PPT::DeepInit(),v).unpack(); }

	constexpr PositF abs()  const { return PositF(DeepInit(),(v < 0 ? -v : v));  }  // could be >= infinity because infinity is sign symmetric
	constexpr PositF neg()  const { return PositF(DeepInit(),-v); }; 
	PositF inv()  const { return PositF(1/(FFT)unpack());}

	// SFINAE optionally: template<typename U = T, class = typename std::enable_if<withnan, U>::type>
    constexpr bool hasNaN() const { return positspec != PositSpec::WithInf; }
	constexpr bool isNaN() const { return positspec != PositSpec::WithInf && v == PT::POSIT_NAN; } 
	constexpr bool isnegative() const { return v < 0; } //(v &POSIT_SIGNBIT) != 0; }
	constexpr bool isinfinity() const { return v == PT::POSIT_PINF || v == PT::POSIT_NINF; }
	constexpr bool iszero() const { return v == 0; }
	constexpr bool isone() const { return v == PT::POSIT_ONE; }
	constexpr PositF prev() const { return PositF(DeepInit(),v > PT::POSIT_MAXPOS || v <= PT::POSIT_MINNEG ? v : v-1); }
	constexpr PositF next() const { return PositF(DeepInit(),v <= PT::POSIT_MINNEG || v > PT::POSIT_MAXPOS ? v : v+1); }
	//TBDconstexpr bool isNaN() const; 
	//TBD constexpr bool isexact() const { return (v&1) == 0; }

	//TBD constexpr bool isfractional() const { return v > 0 && (abs().v < (N2>>2)); } // (0 < x < 1) or  (-1 < x < 0) == (-1,1) removing 0
	//TBD constexpr bool isstrictlynegative() const { return v > (N2>>1); } // -inf < x < 0

	    // Level 1: unpacked
	// Level 0: something using posit specialties
	friend constexpr PositF operator*(const PositF & a, const PositF & b) 
	{
		return PositF((FFT)a*(FFT)b);
	}

	friend constexpr PositF fma(const PositF & a, const PositF & b, const PositF & c)
	{
		return PositF::pack_posit((FFT)a*(FFT)b+(FFT)c);
	}

	CONSTEXPR14 PositF & operator*= (const PositF & b)
	{
		*this = PositF::pack_posit((FFT)*this *(FFT)b);
		return *this;
	}
    friend constexpr PositF operator+(const PositF & a, const PositF & b)
    {
        return a.iszero() ? b : b.iszero() ? a: PositF((FFT)a+(FFT)b);
    }

	PositF& operator+=(const PositF &a) { PositF r = *this+a; v = r.v; return *this; }

	static constexpr PositF zero() { return PositF(DeepInit(),0); }
	static constexpr PositF inf() { return PositF(DeepInit(),PT::POSIT_PINF); }
	static constexpr PositF pinf() { return PositF(DeepInit(),PT::POSIT_PINF); }
	static constexpr PositF ninf() { return PositF(DeepInit(),PT::POSIT_NINF); }
	static constexpr PositF max() { return PositF(DeepInit(),PT::POSIT_MAXPOS); }
	static constexpr PositF min() { return PositF(DeepInit(),PT::POSIT_AFTER0); }
	static constexpr PositF lowest() { return PositF(DeepInit(),PT::POSIT_MINNEG); }

	// SFINAE optionally: template<typename U = T, class = typename std::enable_if<withnan, U>::type>
	static constexpr PositF nan() { return PositF(DeepInit(),PT::POSIT_NAN); }
	static constexpr PositF infinity() { return PositF(DeepInit(),PT::POSIT_PINF); }
	static constexpr PositF one() { return PositF(DeepInit(),PT::POSIT_ONE); }
	static constexpr PositF two() { return PositF(DeepInit(),PT::POSIT_TWO); }
	static constexpr PositF mone() { return PositF(DeepInit(),PT::POSIT_MONE); }
	static constexpr PositF onehalf() { return PositF(DeepInit(),PT::POSIT_HALF); }

	// custom operators
	constexpr PositF operator-() const { return neg(); } 
	constexpr PositF operator~() const { return inv(); } 
	friend constexpr PositF operator-(const PositF & a, const PositF & b)  { return a + (-b); }
	friend constexpr PositF operator/(const PositF & a, const PositF & b)  { return pack_posit(UnpackedT((FFT)a/(FFT)b)); }

	// max
	constexpr operator float() const { return unpack(); }
	constexpr operator double() const { return unpack(); }
	constexpr operator UnpackedT() const { return unpack(); }
	constexpr operator int() const { return unpack(); }

	/// 1/(exp(-x)+1)
	/// TODO: infintity check + __round of result
	constexpr PositF pseudosigmoid() const { return PositF(DeepInit(),(v ^ PT::POSIT_SIGNBIT) >> 2); };

	/// ln(1+exp(x))
	constexpr PositF pseudosoftplus() const { return PositF(DeepInit(),(v ^ PT::POSIT_SIGNBIT) >> 1); };

	/// returns true if in [0,1]
	constexpr bool isUnitRange() const { return v >= 0 && v <= PT::POSIT_ONE; };

	/// unitary range 1-x
	constexpr PositF urOneMinus() const { return PositF(DeepInit(),PT::POSIT_INVERTBIT-v); }

	/// unitary range x(1-x)
	constexpr PositF urDeltaPs() const { return (*this)*urOneMinus(); }

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

/*
NOT WORKING
template <class T,int totalbits, int esbits, PositSpec positspec>
class Posit<T,totalbits,esbits,float,positspec>: public PositF<T,totalbits,esbits,float,positspec> 
{
public:
	using PositF<T,totalbits,esbits,float,positspec>::PositF;
};

template <class T,int totalbits, int esbits, PositSpec positspec>
class Posit<T,totalbits,esbits,double,positspec>: public PositF<T,totalbits,esbits,double,positspec>
{
public:
	using PositF<T,totalbits,esbits,double,positspec>::PositF;
};
*/


template <class T, int totalbits, int esbits, class FFT, PositSpec positspec>
std::ostream & operator << (std::ostream & ons, PositF<T,totalbits,esbits,FFT,positspec> const & o)
{
	ons << o.unpack();
	return ons;
}


template <class T, int totalbits, int esbits, class FFT, PositSpec positspec>
constexpr PositF<T,totalbits,esbits,FFT,positspec> neg(PositF<T,totalbits,esbits,FFT,positspec> x) { return -x; }

template <class T, int totalbits, int esbits, class FFT, PositSpec positspec>
constexpr PositF<T,totalbits,esbits,FFT,positspec> inv(PositF<T,totalbits,esbits,FFT,positspec> x) { return ~x; }





namespace std
{
	template <class T,int totalbits, int esbits, class FFT, PositSpec positspec>
	inline CONSTEXPR14 PositF<T,totalbits,esbits,FFT,positspec> abs(PositF<T,totalbits,esbits,FFT,positspec> z) 
	{
		using PP=PositF<T,totalbits,esbits,FFT,positspec>;
		return PP(PP::DeepInit(),z.v < 0 ? -z.v : z.v);
	}

	template <class T,int totalbits, int esbits, class FFT, PositSpec positspec>
	inline CONSTEXPR14 PositF<T,totalbits,esbits,FFT,positspec> min(PositF<T,totalbits,esbits,FFT,positspec> a, PositF<T,totalbits,esbits,FFT,positspec> b)
	{
		return a <=  b ? a : b;
	}

	template <class T,int totalbits, int esbits, class FFT, PositSpec positspec>
	inline CONSTEXPR14 PositF<T,totalbits,esbits,FFT,positspec> max(PositF<T,totalbits,esbits,FFT,positspec> a, PositF<T,totalbits,esbits,FFT,positspec> b)
	{
		return a >= b ? a : b;
	}

	template <class B,int totalbits, int esbits, class FFT, PositSpec positspec> class numeric_limits<PositF<B,totalbits,esbits,FFT,positspec> > {
	public:
	  using T=PositF<B,totalbits,esbits,FFT,positspec>;
	  using PT=typename T::PT;
	  static constexpr bool is_specialized = true;
	  static constexpr T min() noexcept { return T::min(); }
	  static constexpr T max() noexcept { return T::max(); }
	  static constexpr T lowest() noexcept { return T::lowest	(); }
	  //static constexpr int  digits = 0; number of digits (in radix base) in the mantissa 
	  //static constexpr int  digits10 = 0;
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