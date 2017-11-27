/**
 * Emanuele Ruffaldi (C) 2017
 * Templated C++ Posit
 */
#include "bithippop.hpp"
#include <iostream>

int main(int argc, char * argv[])
{
	//enum X { w = findbitleftmostC((uint64_t)0x0A00000000000000) };
	enum X2 { w2 = findbitleftmostC((uint32_t)0x0A000000) };
	enum X3 { w3 = findbitleftmostC((uint16_t)0x0A00) };
	enum X4 { w4 = findbitleftmostC((uint8_t)0xA) };

	std::cout << "Constants: " << " " << w2 << " " << w3 << " " << w4 << std::endl;

	std::cout << findbitleftmost((uint64_t)0x0A00000000000000) << std::endl;
	std::cout << findbitleftmost((uint32_t)0x0A000000) << std::endl;
	std::cout << findbitleftmost((uint16_t)0x0A00) << std::endl;
	std::cout << findbitleftmost((uint8_t)0x0A) << std::endl;
	std::cout << findbitleftmost((uint16_t)0) << std::endl;
	return 0;
}