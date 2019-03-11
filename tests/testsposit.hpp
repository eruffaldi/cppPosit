/**
 * Copyright (C) 2017-2019 Emanuele Ruffaldi
 * Distributed under the terms of the BSD 3-Clause License.  
 * 
 * (See accompanying file LICENSE)
 * 
 * --
 */
/**
 * Emanuele Ruffaldi (C) 2017
 * Templated C++ Posit
 */
 //
// Created by Emanuele Ruffaldi on 20/10/2017.
//

#include <iomanip>

TEST_CASE( XA  "Unpacked Tests", SPOSIT ) {
    REQUIRE(X::UnpackedT(2.0).inv().inv() == X::UnpackedT(2.0));
}

TEST_CASE( XA "Posits Unpacked Tests", SPOSIT ) {
    REQUIRE(X(2.0).unpack() == X::UnpackedT(2.0));
    REQUIRE(X((X(2.0).unpack())) == X(2.0));
}

TEST_CASE( XA "Other Tests",SPOSIT) {

    std::cout << X(2.0) << "\n\n" << X(2.0).inv() << "\n\n" << X(2.0).inv().inv() << std::endl;
    REQUIRE(X(2.0).inv().inv() == X(2.0));

}

TEST_CASE (XA "Conversions", SPOSIT)
{
    float values[] = {1, 2, INFINITY, 4.0, 10.0, -1.0, -2.0, -4.0, -10.0, 1 / 2.0, 1 / 4.0};

    using TQ = double;
    //using TI = uint64_t;
    // TODO: bugs with fractional 1/2 and 1/4
    for (unsigned int i = 0; i < sizeof(values) / sizeof(values[0]); i++) {

    	TQ f = values[i];
	    X::UnpackedT fu(f);
        X fp(f);
	    TQ fuf = fu.pack_float<TQ>();
	    X fup(fu);
        X::UnpackedT fupu(fup.unpack());
	    TQ fupuf = fupu.pack_float<TQ>();

        std::cout << "step " << values[i] << std::endl;
        if(fupu != fu)
        {
            std::cout  << "d   is " << f << std::endl;
            std::cout  << "du  is " << fu << std::endl;
            std::cout  << "dp  is " << std::oct << fp << std::endl;
            std::cout  << "dup is "  << std::oct << fup << std::endl;
             std::cout << "dupu is " << fupu << std::endl;
            // std::cout << "dpu is " << fpu << std::endl;
             std::cout << "dpud is " << fupuf << std::endl;

        }
	    REQUIRE(f == fuf);   // unpack bug
        REQUIRE(fp == fup);  // impossible bug due to using fu to go to posit
	    REQUIRE(fupu == fu); // bug in unpack posit
	    REQUIRE(fupuf == f); // impossible after the above

    }

}
