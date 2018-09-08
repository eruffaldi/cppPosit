/**
 * Emanuele Ruffaldi (C) 2017
 * Templated C++ Posit
 */
 #include "posit.h"

using X=Posit<int32_t,32,0,uint64_t,false> ; // es, total
#include <iostream>

template <class T,class W>
void checkfloat(T f)
{
union {
   T f;
   W i;
} tmp;
   tmp.f = f;
    W x[1] = {tmp.i};
    std::cout << "--\n";
    X::UnpackedT fu(f);
    std::cout << "f:float   " << f << std::endl;
    std::cout << "f:float hex " << std::hex << x[0] << std::dec << std::endl;
    std::cout << "fu:unpacked" << fu << std::endl;
    T fuf = (float)fu;
   tmp.f = fu;
    x[0] = {tmp.i};
    std::cout << "fuf: float " << fuf << std::endl;
    std::cout << "fuf:float hex " << std::hex << x[0] << std::dec << std::endl;
    auto fup = X(fu);
    std::cout << "fup is " << std::hex << fup << std::endl;
    //X q;
    //q.setBits(0x10);
    //up = q.v;
    X::UnpackedT fupu(fup.unpack());
    std::cout << "fupu   " << fupu << std::endl;
    T fupuf = fupu.pack_float<T>();
    std::cout << "fupuf  " << fupuf  << std::endl;

    if(f != fuf)
    {
        std::cout << "ERROR fuf vs f\n";
    }

    if(fupu != fu)
    {
        std::cout << "ERROR fupu != fu\n";
    }
    else if(fupuf != f)
    {
        std::cout << "ERROR fupuf vs fup\n";
    }
}

template <class T,class W>
void checkup(X::UnpackedT u)
{
    W x[1];
    std::cout << "--\n";
    std::cout << "u:unpacked" << u << std::endl;
    T uf = u.pack_float<T>();
union {
   T f;
   W i;
} tmp;
   tmp.f = uf;
    x[0] = {tmp.i};
    std::cout << "uf: float " << uf << std::endl;
    std::cout << "uf:float hex " << std::hex << x[0] << std::dec << std::endl;
    X::UnpackedT ufu(uf);
    std::cout << "ufu " << ufu << std::endl;

    auto up = X(u);
    std::cout << "fup is " << std::hex << up << std::endl;
    //X q;
    //q.setBits(0x10);
    //up = q.v;
    X::UnpackedT upu(up.unpack());
    std::cout << "fupu   " << upu << std::endl;
    T upuf = upu.pack_float<T>();
    std::cout << "fupuf  " << upuf  << std::endl;

    if(ufu != u)
    {
        std::cout << "ERROR ufu != u\n";
    }

}

int main(int argc, char const *argv[]) {
    std::cout << "N " << X::PT::POSIT_SIZE << " " << X::PT::POSIT_ESP_SIZE << std::endl;
    std::cout << "Configuration " << std::hex << " MSB = " << X::PT::POSIT_MSB << " Mask=" << X::PT::POSIT_MASK
              << " Sign=" << X::PT::POSIT_SIGNBIT << " one=" << X::PT::POSIT_ONE << " inf=" << X::PT::POSIT_PINF
              << " half=" << (double)X(X::PT::POSIT_HALF) << " twice=" <<  (double)X(X::PT::POSIT_TWO) 
              << " min=" << (double)X(X::PT::POSIT_MINNEG) << " max=" << X(X::PT::POSIT_MAXPOS) 
              << " maxexponent " << X::PT::maxexponent() << std::dec << std::endl;
    if(false)
    {
            float values[] = {1, 2, INFINITY, NAN, 0, 4.0, 10.0, -1.0, -2.0, -4.0, -10.0, 1 / 2.0, 1 / 4.0};

            using TQ = double;
            using TI = uint64_t;
            // TODO: bugs with fractional 1/2 and 1/4
            for (unsigned int i = 0; i < sizeof(values) / sizeof(values[0]); i++) {
                checkfloat<TQ, TI>(values[i]);
            }
            std::cout << "Zero is " << X::zero().v << " unpacked " << X::zero().unpack() << std::endl;
            X x(2.0);
            std::cout << "Inverse " << x << " " << x.inv() << " " << X(0.5) << std::endl;
            //static_assert(X(2.0).inv() == X(0.5),"test"); // PROBLEM DUE to UNION
        std::cout << "---------\n";
        std::cout << "TESTING UNPACKED\n";
        // now check the unpack
        checkup<TQ,TI>(X::UnpackedT(40,0,true));
        checkup<TQ,TI>(X::UnpackedT(X::PT::maxexponent(),0,true));
        checkup<TQ,TI>(X::UnpackedT(-X::PT::maxexponent(),0,true));
        checkup<TQ,TI>(X::UnpackedT(X::PT::maxexponent(),0,false));
        checkup<TQ,TI>(X::UnpackedT(-X::PT::maxexponent(),0,false));
        }

    X::UnpackedT yu(0.3f);
    std::cout << "YUff " << (float)yu << std::endl;
    std::cout << "YUfd " << (double)yu << std::endl;

    X::UnpackedT yud(0.3);
    std::cout << "YUdf " << (float)yud << std::endl;
    std::cout << "YUdd " << (double)yud << std::endl;
    X y(0.3);
    std::cout << "One " << (float)X::one() << std::endl;
    std::cout << "Is in normalized range: " << (float)X::one() << " " << X::one().isUnitRange() << std::endl;
    std::cout << "Is in normalized range: " << (float)y << " " << y.isUnitRange() << std::endl;
    std::cout << "Is !negative " << y.isnegative() << std::endl;
    std::cout << "One Minus " << (float)y << "(hex " << std::hex << y.v << ") " << (float)(X::one()-y) << "(hex " << (X::one()-y).v <<  ") and fast " << (float)(y.urOneMinus()) << " (hex " << (y.urOneMinus()).v  <<  ")"<< std::endl;

    /*
    int bits[3];
    int rs,es;
    uint64_t fs;
    std::cout << "y unpacked is " << y.unpack() << std::endl;
    y.analy(bits[0],bits[1],bits[2],rs,es,fs);
    std::cout << "analyzing " << std::hex << y.v << " bits:" << bits[0] << " " << bits[1] << " " << bits[2] << " fields (rs,es,fs) " << rs << " " << es << " "  << fs << std::endl;
    */
	return 0;
}
