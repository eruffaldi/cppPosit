
#include "floattraits.hpp"
#include <iostream>

/*
template <int exp_bits, int frac_bits, class value_T, class holder_T>
struct any_floattrait

template <>
struct float2trait<double>
{
	using type = double;
	using trait = double_trait;
};
*/
#ifndef FT
#error "Needs macro FT"
#endif
void itest_anyvsreallimit()
{
	using Trait=float2trait<FT>::trait;
	using AT=any_floattrait<Trait::exponent_bits,Trait::fraction_bits,Trait::value_t,Trait::holder_t>;
	static_assert(AT::one_h == Trait::one_h,"same one");
	static_assert(AT::two_h == Trait::two_h,"same two_h");
	//static_assert(AT::afterone_h == Trait::afterone_h,"same afterone_h");
	static_assert(AT::min_h == Trait::min_h,"same min_h");
	static_assert(AT::max_h == Trait::max_h,"same max_h");
	static_assert(AT::ninfinity_h == Trait::ninfinity_h,"same ninfinity_h");
	static_assert(AT::pinfinity_h == Trait::pinfinity_h,"same pinfinity_h");
	static_assert(AT::nan_h == Trait::nan_h,"same nan_h");
}