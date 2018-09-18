#include "unpacked.h"
#include <iostream>

void itest_unpackfixed()
{
	static_assert(Unpacked<>(1) == Unpacked<>::one(),"one int is one");
	std::cout << (float)Unpacked<>(0) << std::endl;
	std::cout << Unpacked<>(1) << " == " << Unpacked<>::one()<< std::endl;
	std::cout << (float)Unpacked<>(121) << " " << Unpacked<>(121) << std::endl;
	std::cout << (float)Unpacked<>::make_fixed<fixedtrait<int,32,1> > (121*2+1) << std::endl;
	std::cout << (float)Unpacked<>(1.0) << std::endl;
	std::cout << (float)Unpacked<>(1.0f) << std::endl;
	std::cout << (int)Unpacked<>(10.5) << std::endl;
	std::cout << Unpacked<>(10.75).pack_xfixed<fixedtrait<int,32,0>> () << std::endl;
	std::cout << Unpacked<>(10.75).pack_xfixed<fixedtrait<int,32,1>> () << std::endl;
	std::cout << Unpacked<>(10.75).pack_xfixed<fixedtrait<int,32,2>> () << std::endl;
}