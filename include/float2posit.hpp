/**
 * Copyright (C) 2017-2019 Emanuele Ruffaldi
 * Distributed under the terms of the BSD 3-Clause License.  
 * 
 * (See accompanying file LICENSE)
 * 
 * --
 */
/**
 * Expandedn one
 *
 g++ -I. --std=c++14 float2posit.hpp  -DTESTFLOAT2POSIT -c
 */
#include "posit.h"
#include "floatconst2bits.hpp"

//template <class T,int totalbits, int esbits, class FT, bool withnan, class Trait>
template <class PP, class Trait>
CONSTEXPR14 PP float2posit(typename Trait::holder_t value)
{
	//using PP =  Posit<T,totalbits,esbits,FT,withnan>;
    using PT=typename PP::PT; // trait
    constexpr auto totalbits = PP::vtotalbits;
    constexpr auto esbits = PP::vesbits;
    using FT=typename PP::fraction_t;
    using POSIT_UTYPE = typename PT::POSIT_UTYPE;
    using POSIT_STYPE = typename PT::POSIT_STYPE;
    using ET=typename PT::exponenttype;
    using UT=Unpacked<FT,typename PT::exponenttype>;

    // Phase 2: float to expanded (Unpacked) handling special cases

    ET rawexp = bitset_getT(value,Trait::fraction_bits,Trait::exponent_bits) ;
    bool negativeSign = value & (((typename Trait::holder_t)1) << (Trait::data_bits-1));
	auto exponentF = rawexp - Trait::exponent_bias; // ((un.u >> Trait::fraction_bits) & Trait::exponent_mask)
    auto fractionF = cast_right_to_left<typename Trait::holder_t,Trait::fraction_bits,FT,UT::POSIT_FRAC_TYPE_SIZE_BITS>()(value);

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

    auto rs = -reg+1 > reg+2 ? -reg+1:reg+2; //std::max(-reg + 1, reg + 2);  MSVC issue
    auto es = (totalbits-rs-1) < esbits ? (totalbits-rs-1): esbits; //std::min((int)(totalbits-rs-1),(int)esbits);  MSVC issue

    POSIT_UTYPE regbits = reg < 0 ? (PT::POSIT_HOLDER_MSB >> -reg) : (PT::POSIT_MASK << (PT::POSIT_HOLDER_SIZE-(reg+1))); // reg+1 bits on the left
	POSIT_UTYPE eexp = msb_exp<POSIT_UTYPE,PT::POSIT_HOLDER_SIZE,esbits,(esbits == 00)>()(exp);
	POSIT_STYPE p = ((fraction >> (rs+es+1)) | (eexp >> (rs+1)) | (regbits>>1)) >> (sizeof(PP)*8-totalbits);

    return PP(typename PP::DeepInit(),negativeSign ? -p : p);
}

template <class PP, class Trait>
//template <class T,int totalbits, int esbits, class FT, bool withnan, class Trait>
PP float2positF(typename Trait::value_t fvalue)
{
    union {
        typename Trait::holder_t i;
        typename Trait::value_t  f; 
    } uu;
    uu.f = fvalue;
    return float2posit<PP,Trait>(uu.i);
}

template <class PP>
CONSTEXPR14 PP float2positx(float value)
{
    return float2posit<PP,single_trait>(float2bits(value));
}

template <class PP>
CONSTEXPR14 PP float2positx(double value)
{
    return float2posit<PP,double_trait>(float2bits(value));
}


#ifdef TESTFLOAT2POSIT

// import struct; print("%04X" % struct.unpack("I",struct.pack('f',3.5)))
using PP=Posit<int16_t,16, 2, uint16_t, false>;
enum Q : int16_t { 
    v1 = float2posit<PP,single_trait>(0x40600000).v,
    v2 = float2posit<PP,single_trait>(float2bits(3.5f)).v,
    v3 = float2positx<PP>(3.5f).v
    //v4 = float2posit<PP,double_trait>(float2bits(3.5)).v,

};

int main(int argc, char const *argv[])
{
    std::cout << PP(typename PP::DeepInit(),Q::v1) << " " << (float)PP(typename PP::DeepInit(),Q::v1) << std::endl;
    std::cout << PP(typename PP::DeepInit(),Q::v2) << std::endl;
    std::cout << PP(typename PP::DeepInit(),Q::v3) << std::endl;
    //std::cout << PP(typename PP::DeepInit(),Q::v4) << std::endl; // FAILS
    return 0;
}
#endif