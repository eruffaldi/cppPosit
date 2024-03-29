cmake_minimum_required(VERSION 3.0)
project(cppposit)

#Issues with clang. Build soft float manually 
#add_subdirectory(extern/softfloat)
#link_directories(extern/lib)

#set(SYSPROC arm)
if(SYSPROC MATCHES "arm")
add_definitions (-D__arm__ ) 
ENDIF()

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" /WX "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
  endif()
else()
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall -Wno-long-long -pedantic")
endif()

option(USE_CXX14 "C++14" )
if(USE_CXX14)
add_definitions(--std=c++14)
else()
add_definitions(--std=c++11)
endif()

option(SOFTFLOAT "softfloat" TRUE)
if(SOFTFLOAT)
add_definitions(-DWITH_SOFTFLOAT)
endif()
add_definitions(-march=native)

include_directories(include)
find_package(Eigen3 )
include_directories(${EIGEN3_INCLUDE_DIRS})
add_executable(main src/main.cpp)
#add_executable(maineigen src/maineigen.cpp)
add_executable(testunpacked tests/testunpacked.cpp)
add_executable(testposit tests/testposit.cpp)

# TABULATED posits
# TODO Generalize to any arbitrary configuration of posit: given a posit configuration then
# automatically make the generator, the table file and the cmake

option(POSIT12 "posit12 tabulated" TRUE)
option(POSIT10 "posit10 tabulated" TRUE)
option(POSIT8 "posit8 tabulated" TRUE)
option(BINARY8 "binary8_5 tabulated" TRUE)
if(POSIT12)
  add_executable(posit12_gen src/posit12_gen.cpp)
  set(POSIT12_SRC src/posit12.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/posit12_tbl.cpp)
  if(NOT CMAKE_GENERATOR STREQUAL Xcode)
  add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/posit12_tbl.cpp COMMAND $<TARGET_FILE:posit12_gen> ${CMAKE_CURRENT_SOURCE_DIR}/src/posit12_tbl.cpp DEPENDS posit12_gen )
  endif()
endif()

if(POSIT10)
	add_executable(posit10_gen src/posit10_gen.cpp)
	set(POSIT10_SRC src/posit10.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/posit10_tbl.cpp)
  if(NOT CMAKE_GENERATOR STREQUAL Xcode)
  	add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/posit10_tbl.cpp COMMAND $<TARGET_FILE:posit10_gen> ${CMAKE_CURRENT_SOURCE_DIR}/src/posit10_tbl.cpp DEPENDS posit10_gen )
  endif()
endif()

if(POSIT8)
  add_executable(posit8_gen src/posit8_gen.cpp)
  set(POSIT8_SRC src/posit8.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/posit8_tbl.cpp)
  if(NOT CMAKE_GENERATOR STREQUAL Xcode)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/posit8_tbl.cpp COMMAND $<TARGET_FILE:posit8_gen>   ${CMAKE_CURRENT_SOURCE_DIR}/src/posit8_tbl.cpp DEPENDS posit8_gen )
  endif()
endif()

if(BINARY8)
  add_executable(binary8_gen src/binary8_gen.cpp)
  set(BINARY8_SRC src/binary8.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/binary8_tbl.cpp)
  if(NOT CMAKE_GENERATOR STREQUAL Xcode)
    add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/binary8_tbl.cpp COMMAND $<TARGET_FILE:binary8_gen>  ${CMAKE_CURRENT_SOURCE_DIR}/src/binary8_tbl.cpp DEPENDS binary8_gen )
  endif()
endif()

if(BINARY8 OR POSIT8 OR POSIT10 OR POSIT12)
set(POSITTABTARGET posittab)
add_library(posittab STATIC ${POSIT8_SRC} ${BINARY8_SRC} ${POSIT10_SRC} ${POSIT12_SRC})
set_property(TARGET posittab PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include)
endif()

#if(BINARY8)
#add_executable(testbinary8 tests/testbinary8.cpp)
#target_link_libraries(testbinary8 posittab)
#endif()


if(POSIT8)
add_executable(testposit8 tests/testposit8.cpp)
target_link_libraries(testposit8 posittab)
endif()

if(POSIT10)
add_executable(testposit10 tests/testposit10.cpp)
target_link_libraries(testposit10 posittab)
endif()

if(POSIT12)
add_executable(testposit12 tests/testposit12.cpp)
target_link_libraries(testposit12 posittab)
endif()

#if(SYSPROC MATCHES "arm")
#else()
#include_directories(include/simd)
#add_executable(testsimdposit8 tests/testsimdposit8.cpp src/simdposit8.cpp)
#target_link_libraries(testsimdposit8 ${POSITTABTARGET})
#endif()
add_library(posit INTERFACE)
set_property(TARGET posit PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(${POSITTABTARGET})
add_executable(listposits src/listposits.cpp)
target_link_libraries(listposits posittab)
endif()

if(SOFTFLOAT)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern)
add_library(p::softfloat INTERFACE IMPORTED)
set_property(TARGET p::softfloat PROPERTY INTERFACE_LINK_LIBRARIES softfloat.a)
set_property(TARGET p::softfloat PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/extern)

add_executable(softfloat_gen src/softfloat_gen.cpp)
target_link_libraries(softfloat_gen p::softfloat)

if(NOT CMAKE_GENERATOR STREQUAL Xcode)
add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/float16_tbl.cpp COMMAND DYLD_LIBRARY_PATH=${CMAKE_CURRENT_SOURCE_DIR}/extern LD_LIBRARY_PATH=${CMAKE_CURRENT_SOURCE_DIR}/extern $<TARGET_FILE:softfloat_gen> ${CMAKE_CURRENT_SOURCE_DIR}/src/float16_tbl.cpp DEPENDS softfloat_gen)
endif()
add_library(softfloat16 STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/float16_tbl.cpp)
set_property(TARGET softfloat16 PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/extern)
target_link_libraries(softfloat16 p::softfloat)


add_executable(testsoftfloat tests/testsoftfloat.cpp)
target_link_libraries(testsoftfloat softfloat16)

#set_property(TARGET softfloat16 PROPERTY INTERFACE_LINK_LIBRARIES ${CMAKE_CURRENT_SOURCE_DIR}/extern/softfloat)

install (TARGETS softfloat16 DESTINATION lib)
endif()

#export(EXPORT cppPosit  FILE cppPosit.cmake)
export(TARGETS ${POSITTABTARGET} posit FILE cppPosit.cmake)
include_directories(extern)

install (TARGETS ${POSITTABTARGET} DESTINATION lib) #INTERFACE_INCLUDE_DIRECTORIES include )
install(DIRECTORY include/ DESTINATION include/cppPosit)
#install(EXPORT cppPosit DESTINATION lib/cppPosit )

add_subdirectory(doc EXCLUDE_FROM_ALL)

