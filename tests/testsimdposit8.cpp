#include "simdposit8.hpp"
#include <iostream>

/**
 * TODO:
 	load from array
 	verification speed test
 */

int main(int argc, char const *argv[])
{
	simd_posit8_32 a(0.5f);
	simd_posit8_32 b(1.0f);

	std::cout << "as integers:\n\t" << a.uu() << "\n\t" << a << "\n\t" << b.uu() << "\n\t" << b <<  std::endl;
 	std::cout << "then product:\n\t" << a*b << "\ndifference:\n\t" << (a-b) << "\ndivision:\n\t" << (a/b) << "\ninversion:\n\t" << a.inv() << "\ninversionb:\n\t" << b.inv() << std::endl;
	return 0;
}