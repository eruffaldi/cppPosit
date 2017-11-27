#include "include/posit.h"
#include <iostream>
#include <tuple>

using P=Posit<int8_t,8,3,uint16_t,true>;
P a(10.0f);
P b(20.0f);
std::cout << std::make_tuple((float)a.inv(),(float)(a-b),(float)a*b,(float)a.abs()) << std::endl;
