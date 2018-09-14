// cling -DXBT=float --std=c++14 -I../include test_numeric_limits.cpp
// cling "-DXBT=PositF<int32_t,32,1,double,true> --std=c++14 -I../include test_numeric_limits.cpp
#include "positf.h"     // std::cout
#include <iostream>     // std::cout
#include <limits>       // std::numeric_limits

#define xstr(s) str(s)
#define str(s) #s
#ifndef XBT
	#define XBT Posit<int32_t,32,1,uint64_t,true>
#endif

void test_numeric_limits () {
  using T=XBT;
  std::cout << std::boolalpha;
  std::cout << "\n\nMacro: " << xstr((XBT)) << std::endl;
  std::cout << "Signature: " << typeid(T).name() << std::endl;
  std::cout << "Minimum value for T: " << std::numeric_limits<T>::min() <<" "<< (double)  std::numeric_limits<T>::min()  << '\n';
  std::cout << "Maximum value for T: " << std::numeric_limits<T>::max() <<" "<< (double) std::numeric_limits<T>::max()<< '\n';
  std::cout << "Lowest value for T: " << std::numeric_limits<T>::lowest() << " "<<(double) std::numeric_limits<T>::lowest()<< '\n';
  std::cout << "Epsilon for T: " << std::numeric_limits<T>::epsilon() << " "<<(double) std::numeric_limits<T>::epsilon()<< '\n';

  std::cout << "T is signed: " << std::numeric_limits<T>::is_signed << '\n';
  //std::cout << std::numeric_limits<T>::epsilon().v << std::endl;
  //std::cout << "then " << (T::one()+std::numeric_limits<T>::epsilon() ==  T::one().next()) << std::endl;
  //std::cout << (double)(T::one().next())-(double)T::one() << std::endl;
  //std::cout << "Non-sign bits in T: " << std::numeric_limits<T>::digits << '\n';
  std::cout << "T has infinity: " << std::numeric_limits<T>::has_infinity << '\n';
  std::cout << "Infinity value for T: " << std::numeric_limits<T>::infinity() << " "<< (double)std::numeric_limits<T>::infinity() <<'\n';
  std::cout << "T has nan: " << std::numeric_limits<T>::has_quiet_NaN << '\n';
  std::cout << "NaN " << std::numeric_limits<T>::quiet_NaN() << " " << (double) std::numeric_limits<T>::quiet_NaN() << '\n';
  std::cout << "Exponent Range " << std::numeric_limits<T>::min_exponent << " to " << std::numeric_limits<T>::max_exponent << '\n';
  return ;
}