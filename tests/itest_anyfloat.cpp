
#include "anyfloat.hpp"
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
#define FT anyfloat<8,5,uint64_t>;

#ifndef FT
#error "Needs macro FT"
#endif
void itest_anyfloat()
{
	FT a(1.0);
	FT b(1.0);
	FT r = a+b;
	std::cout << r << " " << (double)r << std::endl;
}