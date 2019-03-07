/**
 * Copyright (C) 2017-2019 Emanuele Ruffaldi
 * Distributed under the terms of the BSD 3-Clause License.  
 * 
 * (See accompanying file LICENSE)
 * 
 * --
 */
/**
 - express the constants as integer and then use constexpr conversion to the Unpacked template
 - implement polevl and p1evl
 - implement ldexp
 
 */
// https://github.com/aterrel/libdynd/blob/master/thirdparty/cephes/exp2.c
// https://github.com/jeremybarnes/cephes/blob/master/cprob/polevl.c

#ifdef UNK
static double P[] = {
    2.30933477057345225087E-2,
    2.02020656693165307700E1,
    1.51390680115615096133E3,
};

static double Q[] = {
    /* 1.00000000000000000000E0, */
    2.33184211722314911771E2,
    4.36821166879210612817E3,
};


#ifdef IBMPC
static unsigned short P[] = {
    0xead3, 0x549a, 0xa5c8, 0x3f97,
    0x5bde, 0x9361, 0x33ba, 0x4034,
    0x7693, 0x907b, 0xa7a0, 0x4097,
};

static unsigned short Q[] = {
    /*0x0000,0x0000,0x0000,0x3ff0, */
    0x5c3c, 0x0ffb, 0x25e5, 0x406d,
    0x0bae, 0x2fed, 0x1036, 0x40b1,
};

template <class FT, class ES>
Unpacked<FT,ES> polevl(Unpacked<FT,ES> x)
{

}

template <class FT, class ES>
Unpacked<FT,ES> p1evl(Unpacked<FT,ES> x)
{
	
}

template <class FT, class ES>
Unpacked<FT,ES> ldexp(Unpacked<FT,ES> x, int n)
{
	
}

template <class FT, class ES>
Unpacked<FT,ES> x exp2(Unpacked<FT,ES> x)
{
	using UT=Unpacked<FT,ES>;

	switch(x.type)
	{
		case UT::NaN: return x;
		case UT::Infinity: return x.negativeSign ? UT(UT::Zero) : x;
		default:
			break;
	}
    if(x > 1024) // TODO
    	return UT(UT::Infinity);
    else if(x < -1024) // TODO
    	return UT(UT::Zero); 
    T px, xx;
    short n;

    xx = x;			/* save x */
    /* separate into integer and fractional parts */
    px = floor(x + 0.5);
    n = px;
    x = x - px;

    /* rational approximation
     * exp2(x) = 1 +  2xP(xx)/(Q(xx) - P(xx))
     * where xx = x**2
     */
    xx = x * x;
    px = x * polevl(xx, P, 2);
    x = px / (p1evl(xx, Q, 2) - px);
    x = 1.0 + ldexp(x, 1);

    /* scale by power of 2 */
    x = ldexp(x, n);
    return (x);
}