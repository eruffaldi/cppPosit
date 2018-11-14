/**
 * Emanuele Ruffaldi (C) 2017
 * Templated C++ Posit
 */
#include <typeinfo>
#include <fstream>
#include <iostream>
#include "binary8.hpp"
#include "anyfloat.hpp"

#define SIGNEX(v, sb) ((v) | (((v) & (1 << (sb))) ? ~((1 << (sb))-1) : 0))

int main(int argc, char const *argv[])
{
	// template <int expbits, int fractionbits, class value_t, class holder_t, class impl_t>
	// match the posit8 from posit8.hpp
	using X=anyfloat_emu<BINARY8_EXP,BINARY8_MAN,int8_t,uint8_t, float>;
	union Q {
		float f;
		uint32_t i;
	} ;
	int8_t opadd[256*256],opmul[256*256],opdiv[256*256],opinv[256],opsquare[256],opexp2[256],ophalf[256];
	uint32_t op2float[256];

	for(int16_t s = -128 ; s < 128; s++)
	{
		X x;
		x.v = s;
		int32_t i = ((unsigned int)(uint16_t)s) & 0xFF;
		float fx(x);
		Q fxq;
        fxq.f = fx;

        opinv[i] = x.inv().v;
        opsquare[i] = X(fx*fx).v;
        op2float[i] = fxq.i; 
        ophalf[i] = X(fx/2).v;
        opexp2[i] = X(exp2(fx)).v;
        //std::cout << "twice " << (X)(fx*2) << " half " << (X)(fx/2) << std::endl;
    }

	for(int16_t s1 = -128 ; s1 < 128; s1++)
	{
		X x1;
		x1.v = s1;
		int32_t i1 = ((unsigned int)(uint8_t)s1) & 0xFF;
		float f1 = uint32_to_float(op2float[i1]); 

		for(int16_t s2 = -128 ; s2 < 128; s2++)
		{
			X x2;
			x2.v = s2;
			int32_t i2 = ((unsigned int)(uint8_t)s2) & 0xFF;
			float f2 = uint32_to_float(op2float[i2]); 

	        opadd[i1*256+i2] = X(f1+f2).v;
	        opmul[i1*256+i2] = X(f1*f2).v;
	        opdiv[i1*256+i2] = X(f1/f2).v;
	        //opadd[i*256+j] = (x*y).v;
		}
	}

	std::ofstream onf(argc == 1 ? "binary8_tbl.cpp" : argv[1]);
	onf << "#include <stdint.h>\nnamespace binary8ns {\n";
	onf << "// type is " << typeid(X).name() << std::endl;
	onf << "int8_t opadd[] = {\n";
	for(int i  = 0; i < 256*256; i++)
		onf << (int)(opadd[i]) << ",";
	onf << "}; " << std::endl;
	// emit the numbers as C file
	onf << "int8_t opmul[] = {\n";
	for(int i  = 0; i < 256*256; i++)
		onf << (int)(opmul[i]) << ",";
	onf << "}; " << std::endl;
	onf << "int8_t opdiv[] = {\n";
	for(int i  = 0; i < 256*256; i++)
		onf << (int)(opdiv[i]) << ",";
	onf << "}; " << std::endl;
	onf << "int8_t opinv[] = {\n";
	for(int i  = 0; i < 256; i++)
		onf << (int)(opinv[i]) << ",";
	onf << "}; " << std::endl;
	onf << "int8_t opsquare[] = {\n";
	for(int i  = 0; i < 256; i++)
		onf << (int)(opsquare[i]) << ",";
	onf << "}; " << std::endl;
	onf << "int8_t opexp2[] = {\n";
	for(int i  = 0; i < 256; i++)
		onf << (int)(opexp2[i]) << ",";
	onf << "}; " << std::endl;

	onf << "int8_t ophalf[] = {\n";
	for(int i  = 0; i < 256; i++)
		onf << (int)(ophalf[i]) << ",";
	onf << "}; " << std::endl;

	onf << "uint32_t op2float[] = {\n";
	for(int i  = 0; i < 256; i++)
		onf << (op2float[i]) << ",";
	onf << "};} " << std::endl;
	return  0;
}

