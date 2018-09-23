// cling -I../include itest_lowlimits.cpp -DQQ=5,10,void,uint16_t
// cling -I../include itest_lowlimits.cpp -DQQ=8,7,void,uint16_t
// cling -I../include itest_lowlimits.cpp -DQQ=8,23,float,uint32_t
// cling -I../include itest_lowlimits.cpp -DQQ=11,52,double,uint64_t
// cout issue: cling -I../include itest_lowlimits.cpp -DQQ=15,112,void,__uint128_t
#include "floattraits.hpp"
#include <iostream>

void itest_lowlimits()
{
	std::cout << "one: " << std::hex << (uint64_t)any_floattrait<QQ>::one_h << std::endl;
	std::cout << "two: " << std::hex << (uint64_t)any_floattrait<QQ>::two_h << std::endl;
	std::cout << "max: " << std::hex << (uint64_t)any_floattrait<QQ>::max_h << std::endl;
	std::cout << "min: " << std::hex << (uint64_t)any_floattrait<QQ>::min_h << std::endl;
	std::cout << "nan: " << std::hex << (uint64_t)any_floattrait<QQ>::nan_h << std::endl;
	std::cout << "pinfinity: " << std::hex <<(uint64_t) any_floattrait<QQ>::pinfinity_h << std::endl;
}