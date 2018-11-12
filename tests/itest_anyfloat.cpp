
#include <iostream>
#include "anyfloat.hpp"

#ifndef XFT
#define XFT binary8_emu
#warning "Using XFT type " XFT
#endif
void itest_anyfloat() {
  XFT a(3.52);
  XFT b(-1.0);
  XFT c;
  XFT ra = a + b;
  XFT rm = a * b;
  XFT rs = a - b;
  XFT rd = a / b;
  std::cout << "c=" << c << std::endl;
  std::cout << "a=" << a << std::endl;
  std::cout << "b=" << b << std::endl;
  std::cout << "1/a=" << a.inv() << std::endl;
  std::cout << "1/b=" << b.inv() << std::endl;
  std::cout << "1/1/a=" << a.inv().inv() << std::endl;
  std::cout << "1/1/b=" << b.inv().inv() << std::endl;
  std::cout << "a<b =" << (a < b) << std::endl;
  std::cout << "a>=b =" << (a >= b) << std::endl;
  std::cout << "|a| " << a.abs() << " -a " << a.neg() << std::endl;
  std::cout << "|b| " << b.abs() << " -b " << b.neg() << std::endl;
  std::cout << "a+b " << ra << std::endl;
  std::cout << "a*b " << rm << std::endl;
  std::cout << "a-b " << rs << std::endl;
  std::cout << "a/b " << rd << std::endl;
  std::cout << "inf " << XFT::pinf() << " to " << XFT::ninf() << std::endl;
  std::cout << "nan " << XFT::nan() << std::endl;
  std::cout << "max-min " << XFT::max() << " to " << XFT::min() << std::endl;
  if(sizeof(XFT)==1)
  {
  	//for(int i = -128; i < 127; i++)
  	//	std::cout << i << ": " << XFT(XFT::DeepInit(),*(int*)(unsigned *)&i) << std::endl;
  	for(int i = 0 ; i < 256; i++)
  	{
  		XFT x(XFT::DeepInit(),i);
  		double d = x;
  		double dd  = XFT(d);
  		unsigned int rawexp = (i >> XFT::vfractionbits) & 0x1F; //bitmask<int>(XFT::vexpbits);

  		/*
  		TODO fix conversion from that double to binary8:: unpack to denormalized 
  		1: 1.52588e-05 -> 0 0
		2: 3.05176e-05 -> 0 0
  		*/
  		if(d  != dd)
	  		std::cout << i << ": " << d << " -> " << dd << " " << (d==dd) << " exp " << rawexp << " hex " << std::hex << i << std::dec << std::endl;
	  	else
	  		std::cout << i << ": " << d << " exp " << rawexp << std::endl;

  	}
  }
}