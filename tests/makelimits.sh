#!/bin/bash
export cling='/Applications/cling_2017-10-26_mac1012/bin/cling --std=c++14 -march=native'
$cling "-DXBT=Posit<int32_t,32,2,uint64_t,true>" --std=c++14 -I../include test_numeric_limits.cpp
$cling "-DXBT=Posit<int32_t,16,1,uint64_t,true>" --std=c++14 -I../include test_numeric_limits.cpp
$cling "-DXBT=Posit<int32_t,8,0,uint64_t,true>" --std=c++14 -I../include test_numeric_limits.cpp
$cling "-DXBT=Posit<int32_t,32,2,uint64_t,false>" --std=c++14 -I../include test_numeric_limits.cpp
$cling "-DXBT=Posit<int32_t,16,1,uint64_t,false>" --std=c++14 -I../include test_numeric_limits.cpp
$cling "-DXBT=Posit<int32_t,8,0,uint64_t,false>" --std=c++14 -I../include test_numeric_limits.cpp
$cling -DXBT=float --std=c++14 -I../include test_numeric_limits.cpp
$cling -DXBT=double --std=c++14 -I../include test_numeric_limits.cpp
#$cling -DXBT=halffloat --std=c++14 -I../include test_numeric_limits.cpp
#$cling -DXBT=halffloatalt --std=c++14 -I../include test_numeric_limits.cpp
