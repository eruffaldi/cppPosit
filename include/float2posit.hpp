/**
 * Expandedn one
 *
 g++ -I. --std=c++14 float2posit.hpp  -DTESTFLOAT2POSIT -c
 */
#include "posit.h"

template <class T,int totalbits, int esbits, class FT, bool withnan, class Trait>
CONSTEXPR14 T float2posit(typename Trait::holder_t value)
{
	using PP =  Posit<T,totalbits,esbits,FT,withnan>;
    using PT=typename Posit<T,totalbits,esbits,FT,withnan>::PT;
    using POSIT_UTYPE = typename PT::POSIT_UTYPE;
    using POSIT_STYPE = typename PT::POSIT_STYPE;
    using ET=typename PT::exponenttype;
    using UT=Unpacked<FT,typename PT::exponenttype>;

    // Phase 2: float to expanded (Unpacked) handling special cases

    ET rawexp = bitset_getT(value,Trait::fraction_bits,Trait::exponent_bits) ;
    auto negativeSign = value & (((typename Trait::holder_t)1) << (Trait::data_bits-1));
	auto exponentF = rawexp - Trait::exponent_bias; // ((un.u >> Trait::fraction_bits) & Trait::exponent_mask)
    auto fractionF = cast_right_to_left<typename Trait::holder_t,Trait::fraction_bits,FT,UT::POSIT_FRAC_TYPE_SIZE_BITS>()(value);

	if(rawexp == ((1 << Trait::exponent_bits)-1))
	{
		if(fractionF == 0)
		{
			return negativeSign ? PT::POSIT_NINF : PT::POSIT_PINF;
		}
		else
		{
			return PT::POSIT_NAN;
		}
	}
	else if (rawexp == 0)
    {
        if(fractionF == 0)
        {
            negativeSign = false;
            return 0;
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
	POSIT_STYPE p = ((fraction >> (rs+es+1)) | (eexp >> (rs+1)) | (regbits>>1)) >> (sizeof(T)*8-totalbits);

    return negativeSign ? -p : p;
}

template <class T,int totalbits, int esbits, class FT, bool withnan, class Trait>
T float2positF(typename Trait::value_t fvalue)
{
    union {
        typename Trait::holder_t i;
        typename Trait::value_t  f; 
    } uu;
    uu.f = fvalue;
    return float2posit<T,totalbits,esbits,FT,withnan,Trait>(uu.i);
}


#ifdef TESTFLOAT2POSIT

// import struct; print("%04X" % struct.unpack("I",struct.pack('f',3.5)))
enum Q : int16_t { vv = float2posit<int16_t,16, 2, uint16_t, false, single_trait>(0x40600000)};
#endif