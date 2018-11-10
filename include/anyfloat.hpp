// anyfloat_emu: any_floattrait + real float/double + UnpackedT encode/decode ~= 
//
// for the case of binary16 see float16native32 based on F16C
//
// TODO anyfloat non emu: missing comparision function of UnpackedT
// TODO testing of the converrsion functions
#pragma once
#include <cmath>
#include <iostream>
#include "unpacked.h"

template <int expbits, int fractionbits, class value_t, class holder_t, class impl_t>
class anyfloat_emu
{
public:
    enum { totalbits = expbits+fractionbits+1, vtotalbits = totalbits, vexpbits = expbits};
	struct DeepInit{};

	static_assert(totalbits <= sizeof(holder_t)*8,"not enough bits in holder");

	// MAYBE: check that impl_t has enough mantissa and exponent

	using T=holder_t;
	using exponenttype=int;
	using trait_t=any_floattrait<expbits,fractionbits,value_t,holder_t>;
	using FFT=impl_t; // float or double
	using impl_trait = typename float2trait<impl_t>::trait ; 
    using UnpackedT=Unpacked<holder_t,exponenttype>;
	
	T v; // index in the N2 space

	anyfloat_emu half() const { return anyfloat_emu(0.5*(FFT)*this); }
	anyfloat_emu twice() const { return anyfloat_emu(2*(FFT)*this); }

	anyfloat_emu() {}
 
	/// unpack value, then repack as implementation type 
	/// NOTE: some conversions could be done quicker: e.g. between native types or float16 if AVX available
	operator FFT () { return UnpackedT::template make_floati<trait_t>(v).template pack_xfloat<impl_trait>(); }

    friend constexpr bool operator == (const anyfloat_emu & a, const anyfloat_emu & u)  { return (FFT)a == (FFT)u; }
    friend constexpr bool operator != (const anyfloat_emu & a, const anyfloat_emu & u)  { return (FFT)a  != (FFT)u; }
    friend constexpr bool operator < (const anyfloat_emu & a, const anyfloat_emu & u)  { return (FFT)a  < (FFT)u; }
    friend constexpr bool operator <= (const anyfloat_emu & a, const anyfloat_emu & u)  { return (FFT)a  <= (FFT)u; }
    friend constexpr bool operator > (const anyfloat_emu & a, const anyfloat_emu & u)  { return (FFT)a  > (FFT)u; }
    friend constexpr bool operator >= (const anyfloat_emu & a, const anyfloat_emu & u)  { return (FFT)a  >= (FFT)u; }

    static constexpr anyfloat_emu ldexp(const anyfloat_emu & u, int exp); // exponent product

    CONSTEXPR14 explicit anyfloat_emu(float f) : v(UnpackedT(f).template pack_xfloati<trait_t>()) {}
	CONSTEXPR14 explicit anyfloat_emu(double d) : v(UnpackedT(d).template pack_xfloati<trait_t>()) {}
	CONSTEXPR14 explicit anyfloat_emu(int i) : v(UnpackedT(i).template pack_xfloati<trait_t>()) {}
	CONSTEXPR14 explicit anyfloat_emu(DeepInit, T x) : v(x) {} 
	CONSTEXPR14 explicit anyfloat_emu(UnpackedT u) : v(u.template pack_floati<trait_t>) {}
	//CONSTEXPR14 explicit anyfloat_emu(UnpackedLow u) : v(pack_low(u).v) {} 

	constexpr UnpackedT unpack() const { return UnpackedT::template make_floati<trait_t>(v); }

	constexpr anyfloat_emu abs()  const { return anyfloat_emu(DeepInit(),(v < 0 ? -v : v));  }  // could be >= infinity because infinity is sign symmetric
	constexpr anyfloat_emu neg()  const { return anyfloat_emu(DeepInit(),-v); }; 
	anyfloat_emu inv()  const { return anyfloat_emu(1/(FFT)*this);}

	// SFINAE optionally: template<typename U = T, class = typename std::enable_if<withnan, U>::type>
    constexpr bool hasNaN() const { return true; }
	//constexpr bool isNaN() const { return v ==  && v == trait_t::POSIT_NAN; } 
	constexpr bool isnegative() const { return v < 0; } //(v &POSIT_SIGNBIT) != 0; }
	//constexpr bool isinfinity() const { return v == trait_t::POSIT_PINF || v == trait_t::POSIT_NINF; }
	constexpr bool iszero() const { return v == 0; }
	//constexpr bool isone() const { return v == trait_t::POSIT_ONE; }
	//constexpr anyfloat_emu prev() const { return anyfloat_emu(DeepInit(),v > trait_t::POSIT_MAXPOS || v <= trait_t::POSIT_MINNEG ? v : v-1); }
	//constexpr anyfloat_emu next() const { return anyfloat_emu(DeepInit(),v <= trait_t::POSIT_MINNEG || v > trait_t::POSIT_MAXPOS ? v : v+1); }
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
		return anyfloat_emu((FFT)a*(FFT)b+(FFT)c);
	}

	CONSTEXPR14 anyfloat_emu & operator*= (const anyfloat_emu & b)
	{
		*this = anyfloat_emu((FFT)*this *(FFT)b);
		return *this;
	}
    friend constexpr anyfloat_emu operator+(const anyfloat_emu & a, const anyfloat_emu & b)
    {
        return a.iszero() ? b : b.iszero() ? a: anyfloat_emu((FFT)a+(FFT)b);
    }

	anyfloat_emu& operator+=(const anyfloat_emu &a) { anyfloat_emu r = *this+a; v = r.v; return *this; }

	static constexpr anyfloat_emu zero() { return anyfloat_emu(DeepInit(),0); }
	static constexpr anyfloat_emu inf() { return anyfloat_emu(DeepInit(),trait_t::pinfinity_h); }
	static constexpr anyfloat_emu pinf() { return anyfloat_emu(DeepInit(),trait_t::pinfinity_h); }
	static constexpr anyfloat_emu ninf() { return anyfloat_emu(DeepInit(),trait_t::pinfinity_h); }
	static constexpr anyfloat_emu max() { return anyfloat_emu(DeepInit(),trait_t::max_h); }
	static constexpr anyfloat_emu min() { return anyfloat_emu(DeepInit(),trait_t::min_h); }
	static constexpr anyfloat_emu lowest() { return anyfloat_emu(DeepInit(),-trait_t::max_h); }

	// SFINAE optionally: template<typename U = T, class = typename std::enable_if<withnan, U>::type>
	static constexpr anyfloat_emu nan() { return anyfloat_emu(DeepInit(),trait_t::nan_h); }
	static constexpr anyfloat_emu infinity() { return anyfloat_emu(DeepInit(),trait_t::pinfinity_h); }
	static constexpr anyfloat_emu one() { return anyfloat_emu(DeepInit(),trait_t::one_h); }
	static constexpr anyfloat_emu two() { return anyfloat_emu(DeepInit(),trait_t::two_h); }
	//static constexpr anyfloat_emu mone() { return anyfloat_emu(DeepInit(),trait_t::mone_h); }
	//static constexpr anyfloat_emu onehalf() { return anyfloat_emu(DeepInit(),trait_t::POSIT_HALF); }

	// custom operators
	constexpr anyfloat_emu operator-() const { return neg(); } 
	constexpr anyfloat_emu operator~() const { return inv(); } 
	friend constexpr anyfloat_emu operator-(const anyfloat_emu & a, const anyfloat_emu & b)  { return a + (-b); }
	friend constexpr anyfloat_emu operator/(const anyfloat_emu & a, const anyfloat_emu & b)  { return anyfloat_emu((FFT)a/(FFT)b); }

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


using binary8_emu = anyfloat_emu<5,2,microfloat,uint8_t,float>;
using binary16alt_emu = anyfloat_emu<8,7,halffloatalt,uint16_t,float>;
using binary16_emu = anyfloat_emu<5,10,halffloat,uint16_t,float>;
using binary32_emu = anyfloat_emu<8,23,float,uint32_t,float>;
using binary64_emu = anyfloat_emu<11,52,double,uint64_t,double>;

namespace std
{
	template <int expbits, int fractionbits, class value_t, class holder_t, class impl_t>
	inline CONSTEXPR14 anyfloat_emu<expbits,fractionbits,value_t,holder_t,impl_t> abs(anyfloat_emu<expbits,fractionbits,value_t,holder_t,impl_t> z) 
	{
		using PP=anyfloat_emu<expbits,fractionbits,value_t,holder_t,impl_t>;
		return PP(PP::DeepInit(),z.v < 0 ? -z.v : z.v);
	}

	template <int expbits, int fractionbits, class value_t, class holder_t, class impl_t>
	inline CONSTEXPR14 anyfloat_emu<expbits,fractionbits,value_t,holder_t,impl_t> min(anyfloat_emu<expbits,fractionbits,value_t,holder_t,impl_t> a, anyfloat_emu<expbits,fractionbits,value_t,holder_t,impl_t> b)
	{
		return a <=  b ? a : b;
	}

	template <int expbits, int fractionbits, class value_t, class holder_t, class impl_t>
	inline CONSTEXPR14 anyfloat_emu<expbits,fractionbits,value_t,holder_t,impl_t> max(anyfloat_emu<expbits,fractionbits,value_t,holder_t,impl_t> a, anyfloat_emu<expbits,fractionbits,value_t,holder_t,impl_t> b)
	{
		return a >= b ? a : b;
	}

	// TODO limits
}
