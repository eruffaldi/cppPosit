.I../include
#include "posit.h"
#include "posit12.hpp"
#include "posit10.hpp"
#include <iostream>
#include <typeinfo>

#include "posit12_tbl.cpp"
#include "posit12.cpp"
#include "posit10_tbl.cpp"
#include "posit10.cpp"

using PT=posit10;
auto a = PT(0.00001);
auto a1 = PT(1.0);
auto adiff=a1-a;
auto adiff_f = PT(1.0-0.0001);
auto asum = a1+a;
auto asum_f = PT(1.0+0.0001);
(float)a
(float)a1
(float)adiff
(float)adiffe
(float)asum
(float)asume