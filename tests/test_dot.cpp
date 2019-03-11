#include "positf.h"     // std::cout
#include <iostream>     // std::cout
#include <limits>       // std::numeric_limits

#define xstr(s) str(s)
#define str(s) #s
#ifndef XBT
	#define XBT Posit<int32_t,32,1,uint64_t,PositSpec::WithNan>
#endif

int main()
{
	  using T=XBT;
	  XBT a[3] = {3,4,5};
	  XBT b[3] = {1,2,3};
	  std::cout << XBT::dot(a,b,3) << std::endl;
	  return 0;
}