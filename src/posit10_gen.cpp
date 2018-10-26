/**
 * Emanuele Ruffaldi (C) 2017
 * Templated C++ Posit
 */
#include <typeinfo>
#include <fstream>
#include <iostream>
#include "posit.h"
#include "posit10.hpp"

#define SIGNEX(v, sb) ((v) | (((v) & (1 << (sb))) ? ~((1 << (sb))-1) : 0))

int16_t opinv[1024],opsquare[1024],opexp2[1024],ophalf[1024];
uint32_t op2float[1024];
int16_t opadd[1024*1024],opmul[1024*1024],opdiv[1024*1024];


int main(int argc, char const *argv[])
{
	// match the posit16 from posit16.hpp
	using X=Posit<typename posit10::PT::POSIT_STYPE,10,posit10::PT::POSIT_ESP_SIZE,typename posit10::FT,posit10::PT::positspec>;
	union Q {
		float f;
		uint32_t i;
	} ;
	for(int16_t s = -512 ; s < 512; s++)
	{
		X x;
		x.v = s;
		int32_t i = ((unsigned int)(uint16_t)s) & 0x03FF;
		float fx(x);
		Q fxq;
        fxq.f = fx;
        opinv[i] = x.inv().v;
        opsquare[i] = X(fx*fx).v;
        op2float[i] = fxq.i; 
        ophalf[i] = X(fx/2).v;
        opexp2[i] = X(exp2(fx)).v;
    }

	for(int16_t s1 = -512 ; s1 < 512; s1++)
	{
		X x1;
		x1.v = s1;
		int32_t i1 = ((unsigned int)(uint16_t)s1) & 0x03FF;
		float f1 = uint32_to_float(op2float[i1]); 

		for(int16_t s2 = -512 ; s2 < 512; s2++)
		{
			X x2;
			x2.v = s2;
			int32_t i2 = ((unsigned int)(uint16_t)s2) & 0x03FF;
			float f2 = uint32_to_float(op2float[i2]); 

	        opadd[i1*1024+i2] = X(f1+f2).v;
	        opmul[i1*1024+i2] = X(f1*f2).v;
	        opdiv[i1*1024+i2] = X(f1/f2).v;
	        //opadd[i*1024+j] = (x*y).v;
		}
		
	}

	std::ofstream onf(argc == 1 ? "posit10_tbl.cpp": argv[1]);
	onf << "#include <stdint.h>\nnamespace posit10ns {\n";
	onf << "// type is " << typeid(X).name() << std::endl;
	onf << "int16_t opmul[] = {\n";
	for(int i  = 0; i < 1024*1024; i++)
		onf << (int)(opmul[i]) << ",";
	onf << "}; " << std::endl;
	onf << "int16_t opadd[] = {\n";
	for(int i  = 0; i < 1024*1024; i++)
		onf << (int)(opadd[i]) << ",";
	onf << "}; " << std::endl;
	onf << "int16_t opdiv[] = {\n";
	for(int i  = 0; i < 1024*1024; i++)
		onf << (int)(opdiv[i]) << ",";
	onf << "}; " << std::endl;
	onf << "int16_t opinv[] = {\n";
	for(int i  = 0; i < 1024; i++)
		onf << (int)(opinv[i]) << ",";
	onf << "}; " << std::endl;

	onf << "int16_t ophalf[] = {\n";
	for(int i  = 0; i < 1024; i++)
		onf << (int)(ophalf[i]) << ",";
	onf << "}; " << std::endl;

	onf << "int16_t opsquare[] = {\n";
	for(int i  = 0; i < 1024; i++)
		onf << (int)(opsquare[i]) << ",";
	onf << "}; " << std::endl;
	onf << "int16_t opexp2[] = {\n";
	for(int i  = 0; i < 1024; i++)
		onf << (int)(opexp2[i]) << ",";
	onf << "}; " << std::endl;
	onf << "uint32_t op2float[] = {\n";
	for(int i  = 0; i < 1024; i++)
		onf << (op2float[i]) << ",";
	onf << "};} " << std::endl;
	return  0;
}
