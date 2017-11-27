/**
 * Emanuele Ruffaldi (C) 2017
 * Templated C++ Posit
 */
#include <climits>
#include <iostream>
#include "bithippop.hpp"

int main(int argc, char const *argv[])
{
		enum X1 { w1 = bitset_get((uint32_t)0x0A,2,3) };
		enum X2 { w2 = bitset_getT((uint32_t)0x0A,2,3) };
		std::cout << "constant " << w1 << " " << w2 << std::endl;

	std::cout << "mask " << bitset_all(bitmask<uint16_t>(5)) << std::endl;
	uint16_t a = 0x0110;
	std::cout << bitset_msb<uint16_t,5>(a) << std::endl;
	std::cout << bitset_lsb<uint16_t,5>(a) << std::endl;
	std::cout << "input: " << bitset_all(a) << std::endl;
	auto c = bitset_part(a,0xFF,2,5);
	std::cout << bitset_msb<uint16_t,5>(c) << std::endl;
	std::cout << bitset_lsb<uint16_t,5>(c) << std::endl;
	std::cout << "afterpart " << bitset_all(bitset_get(c,2,5)) << std::endl;
	std::cout << "beforpart " << bitset_all(bitset_get(a,2,5)) << std::endl;
	std::cout << "afterparthw " << bitset_all(bitset_gethw(c,2,5)) << std::endl;

	std::cout << "after: " << bitset_all(c) << std::endl;

	auto b = bitset_part<uint16_t,uint16_t,2,5>(a,0xFF);
	std::cout << bitset_msb<uint16_t,5>(b) << std::endl;
	std::cout << bitset_lsb<uint16_t,5>(b) << std::endl;
	std::cout << "after: " << bitset_all(b) << std::endl;
	std::cout << "afterpart " << bitset_all(bitset_get<uint16_t,2,5>(c)) << std::endl;
	std::cout << "beforpart " << bitset_all(bitset_get<uint16_t,2,5>(a)) << std::endl;


	return 0;
}