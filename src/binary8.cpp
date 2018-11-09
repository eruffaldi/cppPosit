#if 0
/**
 * Zbinary8 library
 *
 * Emanuele Ruffaldi 2017
 */
#include "anyfloat.hpp"


std::ostream & operator << (std::ostream & ons, const binary8 & p)
{
	ons << "binary8(" << p.uu() << ")" ;
	return ons;
}
binary8::binary8(int a)
{
	if(a == 0)
		v = 0;
	else if(a == 1)
		v = 64;
	else 
		v = FPT(a).v;

}

binary8::binary8(float a)
{
		if(a == 0)
		v = 0;
	else if(a == 1)
		v = 64;
	else 
		v = FPT(a).v;

}

binary8::binary8(double a) 
{
	if(a == 0)
		v = 0;
	else if(a == 1)
		v = 64;
	else 
		v = FPT(a).v;

}


binary8 binary8::twice() const
{
	/*
	int8_t aa = v < 0 ? -v : v;
	if(has_neg_exponent()) // [0..1)
		aa = (aa << 1) & 0x3F; // up to [1..] 
	else // [2...]
		aa = (aa >> 1) | 0x40; // up to 011111111 without overflow
	return v < 0 ? -aa: aa;
	*/
	return (binary8)(as_posit()*(FPT)2);
}


#endif