
#include "float16native32.hpp"
#include <iostream>

#define FT float16n32;

#ifdef __AVX__
#warning Using HW acceleration for F16 conversion
#else
#warning NOT Using HW acceleration for F16 conversion
#endif

void itest_anyfloat()
{
	#ifdef __AVX__
	std::cout  <<"Using HW acceleration for F16 conversion\n";
	#else
	std::cout  <<"NOT Using HW acceleration for F16 conversion\n";
	#endif

	FT a(1.0);
	FT b(1.0);
	FT r = a+b;

	std::cout << r << " " << (double)r << std::endl;
}