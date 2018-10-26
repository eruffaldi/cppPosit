/**
 * Emanuele Ruffaldi (C) 2017
 * Templated C++ Posit
 */
#include <typeinfo>
#include <fstream>
#include <iostream>
#include "posit.h"
#include "posit12.hpp"

#define SIGNEX(v, sb) ((v) | (((v) & (1 << (sb))) ? ~((1 << (sb))-1) : 0))

int16_t opinv[4096],opsquare[4096],opexp2[4096];
uint32_t op2float[4096];

int main(int argc, char const *argv[])
{
	// match the posit16 from posit16.hpp
	using X=Posit<typename posit12::PT::POSIT_STYPE,12,posit12::PT::POSIT_ESP_SIZE,typename posit12::FT,posit12::PT::positspec>;
	union Q {
		float f;
		uint32_t i;
	} ;
	for(int16_t s = -2048 ; s < 2048; s++)
	{
		X x;
		x.v = s;
		int32_t i = ((unsigned int)(uint16_t)s) & 0x0FFF;
        float fx = (float)x;
        //std::cout << s << " " << i << " " << fx << std::endl;
        Q fxq;
        fxq.f = fx;
        opinv[i] = x.inv().v;
        opsquare[i] = X(fx*fx).v;
        op2float[i] = fxq.i; 
        opexp2[i] = X(exp2(fx)).v;
        //std::cout << "twice " << (X)(fx*2) << " half " << (X)(fx/2) << std::endl;
        /*
		for(int j = 0; j < 4096; j++)
		{
	        X y(SIGNEX(j,X::vtotalbits-1));
	        float fy = (float)y;
	        opadd[i*4096+j] = X(fx+fy).v;
	        opmul[i*4096+j] = X(fx*fy).v;
	        opdiv[i*4096+j] = X(fx/fy).v;
	        //opadd[i*4096+j] = (x*y).v;
		}
		*/
	}

	std::ofstream onf(argc == 1 ? "posit12_tbl.cpp": argv[1]);
	onf << "#include <stdint.h>\nnamespace posit12ns {\n";
	onf << "// type is " << typeid(X).name() << std::endl;
	onf << "int16_t opinv[] = {\n";
	for(int i  = 0; i < 4096; i++)
		onf << (int)(opinv[i]) << ",";
	onf << "}; " << std::endl;
	onf << "int16_t opsquare[] = {\n";
	for(int i  = 0; i < 4096; i++)
		onf << (int)(opsquare[i]) << ",";
	onf << "}; " << std::endl;
	onf << "int16_t opexp2[] = {\n";
	for(int i  = 0; i < 4096; i++)
		onf << (int)(opexp2[i]) << ",";
	onf << "}; " << std::endl;
	onf << "uint32_t op2float[] = {\n";
	for(int i  = 0; i < 4096; i++)
		onf << (op2float[i]) << ",";
	onf << "};} " << std::endl;
	return  0;
}
