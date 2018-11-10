
#include "unpacked.h"
#include <iostream>

void itest_anyfloat()
{

	double x = 3.0;
	{
		Unpacked<uint64_t, int> u;
		std::cout << "study " << x << std::endl;
		u.unpack_xfloat<double_trait>((double)x);
		std::cout << "unpacking as double:" << u << std::endl;
		std::cout << "repacking as double:" << u.pack_xfloat<double_trait>() << std::endl;
		u.unpack_xfloat<double_trait>((float)x);
		std::cout << "unpacking as float:" << u << std::endl;
		std::cout << "repacking as float:" << u.pack_xfloat<single_trait>() << std::endl;
		std::cout << "repacking as double:" << u.pack_xfloat<double_trait>() << std::endl;
	}
	{
		Unpacked<uint32_t, int> u;
		std::cout << "study " << x << std::endl;
		u.unpack_xfloat<double_trait>((double)x);
		std::cout << "unpacking as double:" << u << std::endl;
		std::cout << "repacking as double:" << u.pack_xfloat<double_trait>() << std::endl;
		u.unpack_xfloat<double_trait>((float)x);
		std::cout << "unpacking as float:" << u << std::endl;
		std::cout << "repacking as float:" << u.pack_xfloat<single_trait>() << std::endl;
		std::cout << "repacking as double:" << u.pack_xfloat<double_trait>() << std::endl;
	}
}