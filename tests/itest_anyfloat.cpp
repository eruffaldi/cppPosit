
#include "anyfloat.hpp"
#include <iostream>

#ifndef XFT
#define XFT binary16_emu
#warning "Using XFT type " XFT
#endif
void itest_anyfloat()
{
	XFT a(3.52);
	XFT b(1.0);
	XFT r = a+b;
	std::cout << r << " " << (double)r << std::endl;


}