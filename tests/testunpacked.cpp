/**
 * Emanuele Ruffaldi (C) 2017
 * Templated C++ Posit
 */
#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "posit.h"
#include <iostream>
#include <cmath>

float values[] = {1, 2, INFINITY, 0, 4.0, 10.0, -1.0, -2.0, -4.0, -10.0, 1 / 2.0, 1 / 4.0, 0.3, 1200, 1/1200.0, 1e20};


const double epscheck = 1e-5;
TEST_CASE ("Unpacked Conversions double")
{
    using TQ = double;
    using TI = uint64_t;

    for (unsigned int i = 0; i < sizeof(values) / sizeof(values[0]); i++) 
    {
    	char what[128];
    	sprintf(what,"Testing TQ=%s TI=%s value=%f case=%d",typeid(TQ).name(),typeid(TI).name(),(double)values[i],i);
    	SECTION( what) 
    	{
	    	TQ f = values[i];
	    	TQ fi = ((TQ)1.0)/f;
		    Unpacked<TI> fu(f);
		    auto fui = fu.inv();
		    auto fuii = fui.inv();
		    auto fu_by_fui = fu*fui;
		    auto fu2 = Unpacked<TI>((TQ)2)*fu;
		    auto fu2_div_fu = fu2/fu;
		    auto fu_plus_fu = fu+fu;
	    	switch(std::fpclassify(f)) {
		        case FP_INFINITE:  REQUIRE(fu.type == Unpacked<TI>::Infinity); break;
		        case FP_NAN:       REQUIRE(fu.type == Unpacked<TI>::NaN); break;
		        case FP_ZERO:
			        {
			        	REQUIRE(fu.type == Unpacked<TI>::Zero);
					    int i;
				    	//std::cout << " " << f << " becomes " << fu << std::endl;
				    	//REQUIRE(f2 == fu.fraction);
				    	//REQUIRE(i == fu.exponent+1);
				    	REQUIRE(std::signbit(f) == fu.negativeSign);
				    	REQUIRE(f == (double)fu);
				    	break;
			        }
			        break;
		        default:
		        {
		        	REQUIRE(fu.type == Unpacked<TI>::Regular);
				    int i;
			    	auto f2 = std::frexp(f, &i);
			    	//std::cout << " " << f << " becomes " << fu << std::endl;
			    	//REQUIRE(f2 == fu.fraction);
			    	REQUIRE(i == fu.exponent+1);
			    	REQUIRE(std::signbit(f) == fu.negativeSign);
			    	REQUIRE((double)fu == Approx(f));
			    	REQUIRE((double)fuii == Approx(f));
			    	REQUIRE((double)fui == Approx(fi));
			    	REQUIRE((double)fu_by_fui == Approx((TQ)1.0));
			    	REQUIRE((double)fu2_div_fu == Approx((TQ)2.0));
			    	REQUIRE(fu_plus_fu == fu2);
			    	REQUIRE((double)fu_plus_fu == Approx((TQ)2*f));
			    	break;
		        }
		    }
		}
    }
}

TEST_CASE ("Unpacked Conversions float")
{
    using TQ = float;
    using TI = uint32_t;

    for (unsigned int i = 0; i < sizeof(values) / sizeof(values[0]); i++) 
    {
    	char what[128];
    	sprintf(what,"Testing TQ=%s TI=%s value=%f case=%d",typeid(TQ).name(),typeid(TI).name(),(double)values[i],i);
    	SECTION( what) 
    	{
	    	TQ f = values[i];
		    Unpacked<TI> fu(f);
		    auto fui = fu.inv();
		    auto fuii = fui.inv();
	    	TQ fi = ((TQ)1.0)/f;
		    auto fu_by_fui = fu*fui;
		    auto fu2 = Unpacked<TI>((TQ)2)*fu;
		    auto fu2_div_fu = fu2/fu;
		    auto fu_plus_fu = fu+fu;

	    	switch(std::fpclassify(f)) {
		        case FP_INFINITE:  REQUIRE(fu.type == Unpacked<TI>::Infinity); break;
		        case FP_NAN:       REQUIRE(fu.type == Unpacked<TI>::NaN); break;
		        case FP_ZERO:
			        {
			        	REQUIRE(fu.type == Unpacked<TI>::Zero);
					    int i;
				    	//std::cout << " " << f << " becomes " << fu << std::endl;
				    	//REQUIRE(f2 == fu.fraction);
				    	//REQUIRE(i == fu.exponent+1);
				    	REQUIRE(std::signbit(f) == fu.negativeSign);
				    	REQUIRE(fabs(f - fu.pack_float<TQ>()) < epscheck);
				    	break;
			        }
			        break;
		        default:
		        {
		        	REQUIRE(fu.type == Unpacked<TI>::Regular);
				    int i;
			    	auto f2 = std::frexp(f, &i);
			    	//std::cout << " " << f << " becomes " << fu << std::endl;
			    	//REQUIRE(f2 == fu.fraction);
			    	REQUIRE(i == fu.exponent+1);
			    	REQUIRE(std::signbit(f) == fu.negativeSign);
			    	REQUIRE(fu.pack_float<TQ>() == Approx(f));
			    	REQUIRE(fuii.pack_float<TQ>() == Approx(f));
			    	REQUIRE(fui.pack_float<TQ>() == Approx(fi));
			    	REQUIRE(fu_by_fui.pack_float<TQ>() == Approx((TQ)1.0));
			    	REQUIRE(fu2_div_fu.pack_float<TQ>() == Approx((TQ)2.0));
			    	REQUIRE(fu_plus_fu == fu2);
			    	REQUIRE(fu_plus_fu.pack_float<TQ>() == Approx((TQ)2*f));
			    	break;
		        }
		    }
		}
    }
}

TEST_CASE("Sum")
{
	using TI=double;
    using TQ = uint64_t;
    double values[] = { 3200.0, 2.0, 1500000.0 };
    for(unsigned int i = 0; i < sizeof(values)/sizeof(values[0]); i++)
    {
    	for(unsigned int j=i; j < sizeof(values)/sizeof(values[0]); j++)
		{
	    	char what[128];
	    	sprintf(what,"Sum TQ=%s TI=%s value1=%f value2=%f",typeid(TQ).name(),typeid(TI).name(),(double)values[i],(double)values[j]);
	    	Unpacked<TQ> a(values[i]);
	    	Unpacked<TQ> b(values[j]);
			Unpacked<TQ> ab(values[i]+values[j]);
			Unpacked<TQ> amb(values[i]-values[j]);
			Unpacked<TQ> mab(-values[i]+values[j]);
	    	
	    	sprintf(what,"Sum TQ=%s TI=%s value1=%f value2=%f f(a+b)=%f u(a+b)=%f",typeid(TQ).name(),typeid(TI).name(),(double)values[i],(double)values[j],(double)ab,(double)(a+b));
	    	SECTION( what) 
	    	{
	    			REQUIRE((a+b) == ab);
	    			REQUIRE((a-b) == amb);
					REQUIRE((-a+b) == mab);
			}

			/*
	    	sprintf(what,"fSum TQ=%s TI=%s value1=%f value2=%f",typeid(TQ).name(),typeid(TI).name(),(double)values[i],(double)values[j]);
	    	SECTION( what) 
	    	{
					REQUIRE((a+b).pack_float<double>() == Approx((double)(values[i]+values[j])));
					REQUIRE((a-b).pack_float<double>() == Approx((double)(values[i]-values[j])));
					REQUIRE((-a+b).pack_float<double>() == Approx((double)(-values[i]+values[j])));
			}
			*/
    	}	
    	
    }

}

// https://www.h-schmidt.net/FloatConverter/IEEE754.html
// Matlab Symbolic Toolbox says: sum(sym([3.2e8, 1, -1, 8.0e7]).*sym([4.0e7, 1, -1, -1.6e8])) == 2
//	each [ 12800000000000000, 1, 1, -12800000000000000]
// Matlab Says: sum([3.2e8, 1, -1, 8.0e7].*[ 4.0e7, 1, -1, -1.6e8]) == 0
//	each 1E16 [ 1.280000000000000   0.000000000000000   0.000000000000000  -1.280000000000000 ]
TEST_CASE ("High Precision Test")
{
    using TI = uint64_t;
	double a[] = { 3.2e8, 1, -1, 8.0e7};
	double b[] = { 4.0e7, 1, -1, -1.6e8};
	Unpacked<TI> r = Unpacked<TI>::zero(); // ZERO is NOT DETECTED HERE
	for(int i = 0; i < 4 ; i++)
	{
		auto z =  (Unpacked<TI>(a[i])*Unpacked<TI>(b[i]));
		r = r + z;
		//std::cout << " " << i << " " << z << " " << z << "\n\ttmp " << r << " " << r << " left is " << Unpacked<TI>(a[i]) <<  std::endl;
	}
	REQUIRE((double)r == 2);
}
