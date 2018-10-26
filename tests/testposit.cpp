/**
 * Emanuele Ruffaldi (C) 2017
 * Templated C++ Posit
 */
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "posit.h"
#include <iostream>

#define STR_EXPAND(tok) #tok
#define STR(tok) STR_EXPAND(tok)


using X1=Posit<int32_t,28,5,uint32_t,PositSpec::WithNan>;
#define X X1
#define XA "T28_E5 "
#define SPOSIT "Posit<int32_t,28,5,uint32_t,false>"
#include "testsposit.hpp"
#undef X
#undef XA
#undef SPOSIT

/*using X2=Posit<int16_t,16,0,uint32_t>;
#define X X2
#define XA "T16_E0 "
#define SPOSIT "Posit<int16_t,16,0,uint32_t>"
#include "testsposit.hpp"
#undef X
#undef SPOSIT
#undef XA
*/

//	af<< 3.2e8, 1, -1, 8.0e7;

TEST_CASE ("Special Conversion", "")
{
	// IEEE32 All integers with 6 or fewer significant decimal digits can be converted to an IEEE 754 floating-point value without loss of precision
	//using Y1=Posit<int32_t,32,3,uint32_t>;
	using Y=Posit<int32_t,32,3,uint32_t,PositSpec::WithNan>;

	double values1[] = { 3.2e8, 100, 10000, 20000, 1,-1,8.0e7, 4.0e7, 1, -1, -1.6e8};
	double values[] = { -1.6e8};
	for(unsigned int i = 0; i < sizeof(values)/sizeof(values[0]); i++)
	{
		char w[128];
		sprintf(w,"Special SECTION %f",values[i]);
		SECTION(w)
		{
			double d = values[i];
			Y::UnpackedT du(d);
			Y dp(values[i]);
			Y::UnpackedT dpu = dp;
			Y dup = Y(du);
			double dupd = dup;
			double dud = du;
			double dpd = dp;
			/*
			std::cout << "d   is " << d << std::endl;
			std::cout << "du  is " << du << std::endl;
            std::cout << "dp  is " << posit_formatter<Y>(dp) << std::endl;
            std::cout << "dup is "  << posit_formatter<Y>(dup) << std::endl;
 	         std::cout << "dpu is " << dpu << std::endl;
	         std::cout << "dpd is " << dpd << std::endl;
	         */
			REQUIRE(dud == d);
			REQUIRE(dup == dp);
			REQUIRE(dpu == du);
			REQUIRE(dpd == d);
			REQUIRE(dupd == d);
		}
	}
}
