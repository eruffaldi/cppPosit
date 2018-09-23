#if 0
// TODO
// anyfloat: any_floattrait + UnpackedT all ops ~= Posit 
// anyfloat_emu: any_floattrait + real float/double + UnpackedT encode/decode ~= PositF
#pragma once
#include "floattraits.hpp"

template <int expbits, int fractionbits, class FT>
class anyfloat
{
public:
    enum { vtotalbits = totalbits, vesbits = esbits};
	struct DeepInit{};

	using value_t=T;
    using FT=typename float2trait<FFT>::trait::holder_t;
    using UnpackedT=Unpacked<FT,typename PT::exponenttype>;
    using PPT=Posit<T,totalbits,esbits,FT,withnan>; // friendly posit
    using UnpackedLow = UnpackedLow_t<UnpackedT,PT>;
    using fraction_t=FT;
    using exponenttype = typename PT::exponenttype;
	
	T v; // index in the N2 space

	anyfloat half() const { return anyfloat(0.5*(FFT)unpack()); }
	anyfloat twice() const { return anyfloat(2*(FFT)unpack()); }
	
	CONSTEXPR14 UnpackedLow unpack_low() const 
	{ 
		return PPT(PPT::DeepInit(),v).unpack_low(); 
	}

	static CONSTEXPR14 anyfloat pack_low(UnpackedLow u) 
	{
		return anyfloat(DeepInit(),(PPT::pack_low(u)).v);
	}

	static CONSTEXPR14 UnpackedT unpacked_low2full(UnpackedLow x)
	{
		return PPT::unpacked_low2full(x);
	}

	static CONSTEXPR14 UnpackedLow unpacked_full2low(UnpackedT x)
	{
		return PPT::unpacked_full2low(x);
	}

	static CONSTEXPR14 anyfloat pack_posit( UnpackedT  x)
	{
		return pack_low(unpacked_full2low(x));
	}

	static CONSTEXPR14 UnpackedT unpack_posit( anyfloat  x)
	{
		return unpacked_low2full(x.unpack_low());
	}

	anyfloat() {}

    friend constexpr bool operator == (const anyfloat & a, const anyfloat & u)  { return a.v == u.v; }
    friend constexpr bool operator != (const anyfloat & a, const anyfloat & u)  { return a.v != u.v; }
    friend constexpr bool operator < (const anyfloat & a, const anyfloat & u)  { return a.v < u.v; }
    friend constexpr bool operator <= (const anyfloat & a, const anyfloat & u)  { return a.v <= u.v; }

    friend constexpr bool operator > (const anyfloat & a, const anyfloat & u)  { return a.v > u.v; }
    friend constexpr bool operator >= (const anyfloat & a, const anyfloat & u)  { return a.v >= u.v; }

    static constexpr anyfloat ldexp(const anyfloat & u, int exp); // exponent product

    CONSTEXPR14 explicit anyfloat(float f) { v = pack_posit(UnpackedT(f)).v;  }
	CONSTEXPR14 explicit anyfloat(double d) { v = pack_posit(UnpackedT(d)).v;  }
	CONSTEXPR14 explicit anyfloat(int f) { v = pack_posit(UnpackedT((double)f)).v; }
	CONSTEXPR14 explicit anyfloat(DeepInit, T x) : v(x) {} 
	CONSTEXPR14 explicit anyfloat(UnpackedT u) : v(pack_posit(u).v) {} 
	CONSTEXPR14 explicit anyfloat(UnpackedLow u) : v(pack_low(u).v) {} 

	constexpr UnpackedT unpack() const { return PPT(typename PPT::DeepInit(),v).unpack(); }

	constexpr anyfloat abs()  const { return anyfloat(DeepInit(),(v < 0 ? -v : v));  }  // could be >= infinity because infinity is sign symmetric
	constexpr anyfloat neg()  const { return anyfloat(DeepInit(),-v); }; 
	anyfloat inv()  const { return anyfloat(1/(FFT)unpack());}

	// SFINAE optionally: template<typename U = T, class = typename std::enable_if<withnan, U>::type>
    constexpr bool hasNaN() const { return withnan; }
	constexpr bool isNaN() const { return withnan && v == PT::POSIT_NAN; } 
	constexpr bool isnegative() const { return v < 0; } //(v &POSIT_SIGNBIT) != 0; }
	constexpr bool isinfinity() const { return v == PT::POSIT_PINF || v == PT::POSIT_NINF; }
	constexpr bool iszero() const { return v == 0; }
	constexpr bool isone() const { return v == PT::POSIT_ONE; }
	constexpr anyfloat prev() const { return anyfloat(DeepInit(),v > PT::POSIT_MAXPOS || v <= PT::POSIT_MINNEG ? v : v-1); }
	constexpr anyfloat next() const { return anyfloat(DeepInit(),v <= PT::POSIT_MINNEG || v > PT::POSIT_MAXPOS ? v : v+1); }
	//TBDconstexpr bool isNaN() const; 
	//TBD constexpr bool isexact() const { return (v&1) == 0; }

	//TBD constexpr bool isfractional() const { return v > 0 && (abs().v < (N2>>2)); } // (0 < x < 1) or  (-1 < x < 0) == (-1,1) removing 0
	//TBD constexpr bool isstrictlynegative() const { return v > (N2>>1); } // -inf < x < 0

	    // Level 1: unpacked
	// Level 0: something using posit specialties
	friend constexpr anyfloat operator*(const anyfloat & a, const anyfloat & b) 
	{
		return anyfloat((FFT)a*(FFT)b);
	}

	friend constexpr anyfloat fma(const anyfloat & a, const anyfloat & b, const anyfloat & c)
	{
		return anyfloat::pack_posit((FFT)a*(FFT)b+(FFT)c);
	}

	CONSTEXPR14 anyfloat & operator*= (const anyfloat & b)
	{
		*this = anyfloat::pack_posit((FFT)*this *(FFT)b);
		return *this;
	}
    friend constexpr anyfloat operator+(const anyfloat & a, const anyfloat & b)
    {
        return a.iszero() ? b : b.iszero() ? a: anyfloat((FFT)a+(FFT)b);
    }

	anyfloat& operator+=(const anyfloat &a) { anyfloat r = *this+a; v = r.v; return *this; }

	static constexpr anyfloat zero() { return anyfloat(DeepInit(),0); }
	static constexpr anyfloat inf() { return anyfloat(DeepInit(),PT::POSIT_PINF); }
	static constexpr anyfloat pinf() { return anyfloat(DeepInit(),PT::POSIT_PINF); }
	static constexpr anyfloat ninf() { return anyfloat(DeepInit(),PT::POSIT_NINF); }
	static constexpr anyfloat max() { return anyfloat(DeepInit(),PT::POSIT_MAXPOS); }
	static constexpr anyfloat min() { return anyfloat(DeepInit(),PT::POSIT_AFTER0); }
	static constexpr anyfloat lowest() { return anyfloat(DeepInit(),PT::POSIT_MINNEG); }

	// SFINAE optionally: template<typename U = T, class = typename std::enable_if<withnan, U>::type>
	static constexpr anyfloat nan() { return anyfloat(DeepInit(),PT::POSIT_NAN); }
	static constexpr anyfloat infinity() { return anyfloat(DeepInit(),PT::POSIT_PINF); }
	static constexpr anyfloat one() { return anyfloat(DeepInit(),PT::POSIT_ONE); }
	static constexpr anyfloat two() { return anyfloat(DeepInit(),PT::POSIT_TWO); }
	static constexpr anyfloat mone() { return anyfloat(DeepInit(),PT::POSIT_MONE); }
	static constexpr anyfloat onehalf() { return anyfloat(DeepInit(),PT::POSIT_HALF); }

	// custom operators
	constexpr anyfloat operator-() const { return neg(); } 
	constexpr anyfloat operator~() const { return inv(); } 
	friend constexpr anyfloat operator-(const anyfloat & a, const anyfloat & b)  { return a + (-b); }
	friend constexpr anyfloat operator/(const anyfloat & a, const anyfloat & b)  { return pack_posit(UnpackedT((FFT)a/(FFT)b)); }

	// max
	constexpr operator float() const { return unpack(); }
	constexpr operator double() const { return unpack(); }
	constexpr operator UnpackedT() const { return unpack(); }
	constexpr operator int() const { return unpack(); }

	/// 1/(exp(-x)+1)
	/// TODO: infintity check + __round of result
	constexpr anyfloat pseudosigmoid() const { return anyfloat(DeepInit(),(v ^ PT::POSIT_SIGNBIT) >> 2); };

	/// ln(1+exp(x))
	constexpr anyfloat pseudosoftplus() const { return anyfloat(DeepInit(),(v ^ PT::POSIT_SIGNBIT) >> 1); };

	/// returns true if in [0,1]
	constexpr bool isUnitRange() const { return v >= 0 && v <= PT::POSIT_ONE; };

	/// unitary range 1-x
	constexpr anyfloat urOneMinus() const { return anyfloat(DeepInit(),PT::POSIT_INVERTBIT-v); }

	/// unitary range x(1-x)
	constexpr anyfloat urDeltaPs() const { return (*this)*urOneMinus(); }

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

namespace std
{
	template <int expbits, int fractionbits, class FT>
	inline CONSTEXPR14 anyfloat<T,totalbits,esbits,FFT,withnan> abs(anyfloat<T,totalbits,esbits,FFT,withnan> z) 
	{
		using PP=anyfloat<T,totalbits,esbits,FFT,withnan>;
		return PP(PP::DeepInit(),z.v < 0 ? -z.v : z.v);
	}

	template <int expbits, int fractionbits, class FT>
	inline CONSTEXPR14 anyfloat<T,totalbits,esbits,FFT,withnan> min(anyfloat<T,totalbits,esbits,FFT,withnan> a, anyfloat<T,totalbits,esbits,FFT,withnan> b)
	{
		return a <=  b ? a : b;
	}

	template <int expbits, int fractionbits, class FT>
	inline CONSTEXPR14 anyfloat<T,totalbits,esbits,FFT,withnan> max(anyfloat<T,totalbits,esbits,FFT,withnan> a, anyfloat<T,totalbits,esbits,FFT,withnan> b)
	{
		return a >= b ? a : b;
	}

	template <class B,int totalbits, int esbits, class FFT, bool withnan> 
		class numeric_limits<anyfloat<B,totalbits,esbits,FFT,withnan> > : public std::numeric_limits<Posit<B,totalbits,esbits,typename anyfloat<B,totalbits,esbits,FFT,withnan>::FT,withnan> > {
	public:
	  using T=anyfloat<B,totalbits,esbits,FFT,withnan>;
	};
}
#endif