/**
 * ZPosit8 library
 *
 * Emanuele Ruffaldi 2017
 */
#include "simdposit8.hpp"




std::ostream & operator << (std::ostream & ons, const simd_posit8_32 & p)
{
	ons << (std::array<float,32>)p;
	return ons;
}
simd_posit8_32::simd_posit8_32(int a)
{
	auto q = FPT(a).v;
	std::cout << "initing from integer " << a << " back as " << (float)(FPT(FPT::DeepInit(),q)) << " as dec " << std::dec << (int)q << std::dec <<std::endl;
	v = _mm256_set1_epi8(q);
}

simd_posit8_32::simd_posit8_32(float a)
{
	auto q = FPT(a).v;
	std::cout << "initing from float " << a << " back as  " << (float)(FPT(FPT::DeepInit(),q)) << " as dec " << std::dec << (int)q << std::dec <<std::endl;
	v = _mm256_set1_epi8(q);
}
/**
 Positive NUmber families

 00000000
 ..
 00001100 = .1875
 ..
 00010000 = .25
 ..
 00011000 = 0.375
 ..
 00100001 = 0.5
 ..
 00110000 = 0.75
 ..
 01000000 = 1
 ..
 01010000 = 1.5
 ..
 01100000 = 2
 ...
 01110000 = 4
 ...
 01111111 = max
 10000000 = infinity
 */

simd_posit8_32::operator std::array<float,32>() const
{
	std::array<float,32> r;
#if 0
	union Q {
		float f;
		uint32_t i;
	};
	alignas(__m256i) int8_t idata[32]; // uint8 if using table
	_mm256_store_si256((__m256i*)idata,v);
	uint32_t * pp = (uint32_t*)&r[0];
	for(int i = 0; i < 32; i++)
	{
		Q xyz;
		xyz.f = (float)FPT(FPT::DeepInit(),idata[i]);
		pp[i] = xyz.i;
	}
#else
	alignas(__m256i) uint8_t idata[32]; // uint8 if using table
	_mm256_store_si256((__m256i*)idata,v);
	uint32_t * pp = (uint32_t*)&r[0];
	for(int i = 0; i < 32; i++)
	{
		pp[i] = posit8ns::op2float[idata[i]];
	}
#endif
	return r;
}

