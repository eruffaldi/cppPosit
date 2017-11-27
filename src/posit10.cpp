/**
 * ZPosit8 library
 *
 * Emanuele Ruffaldi 2017
 */
#include "posit10.hpp"


std::ostream & operator << (std::ostream & ons, const posit10 & p)
{
	ons << "posit(" << p.uu() << ")" ;
	return ons;
}
posit10::posit10(int a)
{
	if(a == 0)
		v = 0;
	else if(a == 1)
		v = 0x100;
	else 
		v = FPT(a).v;
}

posit10::posit10(float a)
{
	if(a == 0)
		v = 0;
	else if(a == 1)
		v = 0x100;
	else 
		v = FPT(a).v;
}

posit10::posit10(double a)
{
	if(a == 0)
		v = 0;
	else if(a == 1)
		v = 0x100;
	else 
		v = FPT(a).v;

}

/**
 Positive NUmber families

 00000000
 ..
 00001100 = .1875
 ..
 00010000 = .25
 ..
 00011000 = 0.375
 ..
 00100001 = 0.5
 ..
 00110000 = 0.75
 ..
 01000000 = 1
 ..
 01010000 = 1.5
 ..
 01100000 = 2
 ...
 01110000 = 4
 ...
 01111111 = max
 10000000 = infinity
 */

	/*
posit10 posit10::half() const
{
	int8_t aa = v < 0 ? -v : v;
	if(has_neg_exponent()) // [0..1)
		aa = aa >> 1; // down to 0
	else if aa < 2 // [1..2)
		aa = ((aa & 0x3F)|0x20);
	else
		aa = ((aa << 1) & 0x7F) | 0x40;  // down to 01000000 == 0x4  
	return v < 0 ? -aa: aa;
	*/


posit10 posit10::twice() const
{
	/*
	int8_t aa = v < 0 ? -v : v;
	if(has_neg_exponent()) // [0..1)
		aa = (aa << 1) & 0x3F; // up to [1..] 
	else // [2...]
		aa = (aa >> 1) | 0x40; // up to 011111111 without overflow
	return v < 0 ? -aa: aa;
	*/
	return (posit10)(as_posit()*(FPT)2);
}



