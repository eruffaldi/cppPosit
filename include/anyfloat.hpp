#if 0

// TODO
// anyfloat_emu: any_floattrait + UnpackedT all ops ~= HARD requires comparison
// anyfloat_emu: any_floattrait + real float/double + UnpackedT encode/decode ~= 
#pragma once
#include "floattraits.hpp"
#include "unpacked.h"

template <int expbits, int fractionbits, class value_t, class holder_t, class impl_t>
class anyfloat_emu
{
public:
    enum { totalbits = expbits+fractionbits+1, vtotalbits = totalbits, vexpbits = expbits};
	struct DeepInit{};

	using T=holder_t;
	using PT=typename any_floattrait<expbits,fractionbits,value_t,holder_t>;
	using FFT=impl_t; // float or double
    using UnpackedT=Unpacked<holder_t,exponenttype>;
	
	T v; // index in the N2 space

	anyfloat_emu half() const { return anyfloat_emu(0.5*(FFT)*this); }
	anyfloat_emu twice() const { return anyfloat_emu(2*(FFT)*this); }

	anyfloat_emu() {}

    friend constexpr bool operator == (const anyfloat_emu & a, const anyfloat_emu & u)  { return (FFT)a == (FFT)u; }
    friend constexpr bool operator != (const anyfloat_emu & a, const anyfloat_emu & u)  { return (FFT)a  != (FFT)u; }
    friend constexpr bool operator < (const anyfloat_emu & a, const anyfloat_emu & u)  { return (FFT)a  < (FFT)u; }
    friend constexpr bool operator <= (const anyfloat_emu & a, const anyfloat_emu & u)  { return (FFT)a  <= (FFT)u; }
    friend constexpr bool operator > (const anyfloat_emu & a, const anyfloat_emu & u)  { return (FFT)a  > (FFT)u; }
    friend constexpr bool operator >= (const anyfloat_emu & a, const anyfloat_emu & u)  { return (FFT)a  >= (FFT)u; }

    static constexpr anyfloat_emu ldexp(const anyfloat_emu & u, int exp); // exponent product

    CONSTEXPR14 explicit anyfloat_emu(float f) { v = pack_posit(UnpackedT(f)).v;  }
	CONSTEXPR14 explicit anyfloat_emu(double d) { v = pack_posit(UnpackedT(d)).v;  }
	CONSTEXPR14 explicit anyfloat_emu(int f) { v = pack_posit(UnpackedT((double)f)).v; }
	CONSTEXPR14 explicit anyfloat_emu(DeepInit, T x) : v(x) {} 
	CONSTEXPR14 explicit anyfloat_emu(UnpackedT u) : v(pack_posit(u).v) {} 
	CONSTEXPR14 explicit anyfloat_emu(UnpackedLow u) : v(pack_low(u).v) {} 

	constexpr UnpackedT unpack() const { return PPT(typename PPT::DeepInit(),v).unpack(); }

	constexpr anyfloat_emu abs()  const { return anyfloat_emu(DeepInit(),(v < 0 ? -v : v));  }  // could be >= infinity because infinity is sign symmetric
	constexpr anyfloat_emu neg()  const { return anyfloat_emu(DeepInit(),-v); }; 
	anyfloat_emu inv()  const { return anyfloat_emu(1/(FFT)*this);}

	// SFINAE optionally: template<typename U = T, class = typename std::enable_if<withnan, U>::type>
    constexpr bool hasNaN() const { return withnan; }
	constexpr bool isNaN() const { return withnan && v == PT::POSIT_NAN; } 
	constexpr bool isnegative() const { return v < 0; } //(v &POSIT_SIGNBIT) != 0; }
	constexpr bool isinfinity() const { return v == PT::POSIT_PINF || v == PT::POSIT_NINF; }
	constexpr bool iszero() const { return v == 0; }
	constexpr bool isone() const { return v == PT::POSIT_ONE; }
	constexpr anyfloat_emu prev() const { return anyfloat_emu(DeepInit(),v > PT::POSIT_MAXPOS || v <= PT::POSIT_MINNEG ? v : v-1); }
	constexpr anyfloat_emu next() const { return anyfloat_emu(DeepInit(),v <= PT::POSIT_MINNEG || v > PT::POSIT_MAXPOS ? v : v+1); }
	//TBDconstexpr bool isNaN() const; 
	//TBD constexpr bool isexact() const { return (v&1) == 0; }

	//TBD constexpr bool isfractional() const { return v > 0 && (abs().v < (N2>>2)); } // (0 < x < 1) or  (-1 < x < 0) == (-1,1) removing 0
	//TBD constexpr bool isstrictlynegative() const { return v > (N2>>1); } // -inf < x < 0

	    // Level 1: unpacked
	// Level 0: something using posit specialties
	friend constexpr anyfloat_emu operator*(const anyfloat_emu & a, const anyfloat_emu & b) 
	{
		return anyfloat_emu((FFT)a*(FFT)b);
	}

	friend constexpr anyfloat_emu fma(const anyfloat_emu & a, const anyfloat_emu & b, const anyfloat_emu & c)
	{
		return anyfloat_emu::pack((FFT)a*(FFT)b+(FFT)c);
	}

	CONSTEXPR14 anyfloat_emu & operator*= (const anyfloat_emu & b)
	{
		*this = anyfloat_emu::pack((FFT)*this *(FFT)b);
		return *this;
	}
    friend constexpr anyfloat_emu operator+(const anyfloat_emu & a, const anyfloat_emu & b)
    {
        return a.iszero() ? b : b.iszero() ? a: anyfloat_emu((FFT)a+(FFT)b);
    }

	anyfloat_emu& operator+=(const anyfloat_emu &a) { anyfloat_emu r = *this+a; v = r.v; return *this; }

	static constexpr anyfloat_emu zero() { return anyfloat_emu(DeepInit(),0); }
	static constexpr anyfloat_emu inf() { return anyfloat_emu(DeepInit(),PT::POSIT_PINF); }
	static constexpr anyfloat_emu pinf() { return anyfloat_emu(DeepInit(),PT::POSIT_PINF); }
	static constexpr anyfloat_emu ninf() { return anyfloat_emu(DeepInit(),PT::POSIT_NINF); }
	static constexpr anyfloat_emu max() { return anyfloat_emu(DeepInit(),PT::POSIT_MAXPOS); }
	static constexpr anyfloat_emu min() { return anyfloat_emu(DeepInit(),PT::POSIT_AFTER0); }
	static constexpr anyfloat_emu lowest() { return anyfloat_emu(DeepInit(),PT::POSIT_MINNEG); }

	// SFINAE optionally: template<typename U = T, class = typename std::enable_if<withnan, U>::type>
	static constexpr anyfloat_emu nan() { return anyfloat_emu(DeepInit(),PT::POSIT_NAN); }
	static constexpr anyfloat_emu infinity() { return anyfloat_emu(DeepInit(),PT::POSIT_PINF); }
	static constexpr anyfloat_emu one() { return anyfloat_emu(DeepInit(),PT::POSIT_ONE); }
	static constexpr anyfloat_emu two() { return anyfloat_emu(DeepInit(),PT::POSIT_TWO); }
	static constexpr anyfloat_emu mone() { return anyfloat_emu(DeepInit(),PT::POSIT_MONE); }
	static constexpr anyfloat_emu onehalf() { return anyfloat_emu(DeepInit(),PT::POSIT_HALF); }

	// custom operators
	constexpr anyfloat_emu operator-() const { return neg(); } 
	constexpr anyfloat_emu operator~() const { return inv(); } 
	friend constexpr anyfloat_emu operator-(const anyfloat_emu & a, const anyfloat_emu & b)  { return a + (-b); }
	friend constexpr anyfloat_emu operator/(const anyfloat_emu & a, const anyfloat_emu & b)  { return pack_posit(UnpackedT((FFT)a/(FFT)b)); }

	// max
	constexpr operator float() const { return unpack(); }
	constexpr operator double() const { return unpack(); }
	constexpr operator UnpackedT() const { return unpack(); }
	constexpr operator int() const { return unpack(); }

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
	template <int expbits, int fractionbits, class value_t, class holder_t, class impl_t>
	inline CONSTEXPR14 anyfloat_emu<expbits,fractionbits,value_t,holder_t,impl_t> abs(anyfloat_emu<expbits,fractionbits,value_t,holder_t,impl_t> z) 
	{
		using PP=anyfloat_emu<T,totalbits,expbits,FFT,withnan>;
		return PP(PP::DeepInit(),z.v < 0 ? -z.v : z.v);
	}

	template <int expbits, int fractionbits, class FT>
	inline CONSTEXPR14 anyfloat_emu<T,totalbits,expbits,FFT,withnan> min(anyfloat_emu<T,totalbits,expbits,FFT,withnan> a, anyfloat_emu<T,totalbits,expbits,FFT,withnan> b)
	{
		return a <=  b ? a : b;
	}

	template <int expbits, int fractionbits, class FT>
	inline CONSTEXPR14 anyfloat_emu<T,totalbits,expbits,FFT,withnan> max(anyfloat_emu<T,totalbits,expbits,FFT,withnan> a, anyfloat_emu<T,totalbits,expbits,FFT,withnan> b)
	{
		return a >= b ? a : b;
	}

	template <class B,int totalbits, int expbits, class FFT, bool withnan> 
		class numeric_limits<anyfloat_emu<B,totalbits,expbits,FFT,withnan> > : public std::numeric_limits<Posit<B,totalbits,expbits,typename anyfloat_emu<B,totalbits,expbits,FFT,withnan>::FT,withnan> > {
	public:
	  using T=anyfloat_emu<B,totalbits,expbits,FFT,withnan>;
	};
}

#endif