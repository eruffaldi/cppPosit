/**
 * Emanuele Ruffaldi (C) 2017
 * Templated C++ Posit
 */
// g++ -march=native -I. --std=c++14 testregime.cpp  && ./a.out 
#include "bithippop.hpp"
#include <bitset>
#include <algorithm>
#include <iostream>
#include <cstdint>
	using POSIT_UTYPE = uint32_t;
namespace PT
{
	enum {POSIT_SIZE = 32, POSIT_INVERTBIT = 1<<(sizeof(POSIT_UTYPE)*8)-1};
}


std::pair<int,int> extract(int pars)


{	
	const bool x = (pars & PT::POSIT_INVERTBIT) != 0;
    int aindex = x ? (~pars == 0 ? PT::POSIT_SIZE : findbitleftmostC((POSIT_UTYPE)~pars)) : (pars == 0 ? PT::POSIT_SIZE : findbitleftmostC((POSIT_UTYPE)pars)); // where is stop 0... 1 beyond
    int index = aindex > PT::POSIT_SIZE  ? PT::POSIT_SIZE : aindex;
    int reg = x ? index-1 : -index;
    int rs =  index == PT::POSIT_SIZE ? PT::POSIT_SIZE: index+1;
    return {reg,rs};
}



int main(int argc , char * argv[])
{
	if(0)
	{
		for(int i = 0; i < 1<< PT::POSIT_SIZE; i++)
		{
			POSIT_UTYPE a = (i <<4 )|0x0F;
			auto q = extract(a); 
			std::cout << (std::bitset<sizeof(a)*8>(a)) << " reg=" << q.first << " rs=" << q.second  << std::endl;
		}
	}
	else
	{
		uint32_t a = 0xe0000000;
			auto q = extract(a); 
			std::cout << (std::bitset<sizeof(a)*8>(a)) << " reg=" << q.first << " rs=" << q.second  << std::endl;
	}
	return 0;
}
