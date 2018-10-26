/**
 * Emanuele Ruffaldi (C) 2017
 * Templated C++ Posit
 */
#include "catch.hpp"
#include "posit.h"
#include "args.hxx"
#include "posit12.hpp"
#include "posit10.hpp"
#include "posit8.hpp"
#include <iostream>
#include <fstream>
#include <string.h>
#include <typeinfo>
//#include "tvalids.hpp"
//#include "tvalids_8_1.hpp"

#ifdef POSIT_TYPEID
#include "posit12_tbl.cpp"
#include "posit12.cpp"
#include "posit10_tbl.cpp"
#include "posit10.cpp"
#include "posit8_tbl.cpp"
#include "posit8.cpp"

/*
#include "tvalids.hpp"

Supported: tvalid<int8_t,4,10,?>
#warning Available: tvalid<int8_t,4,10,?>
*/

using XT=POSIT_TYPEID;
#else

#ifndef POSIT_TYPE
using X1=Posit<int32_t,6,1,uint32_t,PositSpec::WithNan>;
using X2=Posit<int8_t,8,0,uint8_t,PositSpec::WithNan>;
using X3=Posit<int16_t,6    ,0,uint8_t,PositSpec::WithNan>;
using X4=Posit<int8_t,8  ,2,uint8_t,PositSpec::WithNanInf>;
using XT=X4;
#else
using XT=Posit<POSIT_TYPE>;
#endif
#endif

#define xstr(s) astr(s)
#define astr(s) #s

#define DEFAULTLISTFORMAT "iv"
#ifdef LISTFORMAT
#define LISTFORMATX xstr(LISTFORMAT)
#else
#define LISTFORMATX DEFAULTLISTFORMAT
#endif 

#define SIGNEX(v, sb) ((v) | (((v) & (1 << (sb))) ? ~((1 << (sb))-1) : 0))

#if 0
class TablePrinter
{
public:
    enum Alignment {Center, Left, Right};
    struct Column
    {
        std::vector<std::string> values;
        std::string name;
        Alignment align;
        int width = -1; // autosize
    };

    struct ColumnWriter
    {
        TablePrinter & tp;
        int coli;

        ~ColumnWriter()
        {

        }

    }

    std::map<std::string, int> ncolumns;
    std::vector<Column> columns;

    ColumnWriter col_stream(std::string name)
    {

    }


    friend std::ostream & operator << (std::ostream & os, const TablePrinter & tab)
    {
        return os;
    }
};
#endif
/**
 TODO:
 Formatting syntax:

    back conversion
    half 
    twice 
    inverse
 */
template <class X>
void print1(std::ostream &ons,X x, const char * format,bool binaryoutput)
{
    using PT = typename X::PT;
    using PTU = typename PT::POSIT_UTYPE;
    using PTS = typename PT::POSIT_STYPE;
    using pt = typename printableinttype<typename X::value_t>::type;
    PTS i = x.v;
    PTU u = *(PTU*)&i;
        auto up(x.unpack()); // unpack it OK
        auto l(x.unpack_low()); // unpack it OK
        if(!binaryoutput)
        {
            char c= '\t';
             for(const char * p0 = format; *p0 ; ++p0)
            {
                switch(*p0)
                {
                    case 'i': ons << c<<pt(i) ;  break;
                    case 'o': ons << c<<pt(u) ;  break;
                    case 'p': ons << c<<posit_formatter<X>(x) ; break;
                    case 'b': ons <<c << std::bitset<X::vtotalbits>(i); break;            
                    case 'k': ons << c << std::dec << pt(l.regime) ;  break;
                    case 'r': ons << c << pow(2,PT::POSIT_REG_SCALE*pt(l.regime)) ;  break;
                    case 'e': ons << c<< std::dec << pt(up.exponent); break;
                    case 'u': ons << c << up; break;
                    //TODO case 'f': ons << c<<"1." << pt(l.fraction); break;
                    //TODO case 'F': ons << c<<"1." << pt(l.fraction); break;
                    case 'v': ons << c << (double)x; break;
                    break;
                }
            }
        }
        else
        {
            double q =0;
            for(const char * p0 = format; *p0 ; ++p0)
            {
                bool ok =true;
                switch(*p0)
                {
                    case 'i':                         q = pt(i) ;  break;
                    case 'o':                         q = pt(u) ;  break;
                    case 'p': q = 0 ; break;
                    case 'b': q=0; break;            
                    case 'k': q= pt(l.regime) ;  break;
                    case 'r': q= pow(2,PT::POSIT_REG_SCALE*pt(l.regime)) ;  break;
                    case 'e': q= pt(up.exponent); break;
                    case 'u': q=up; break;
                    //TODO case 'f': ons << c<<"1." << pt(l.fraction); break;
                    //TODO case 'F': ons << c<<"1." << pt(l.fraction); break;
                    case 'v': q=(double)x; break;
                    default:
                        ok =false;
                }            
                if(ok)
                ons.write((char*)&q,sizeof(q));
            }
        }   
}

template <class X>
int listpositsT(const char * format)
{
#if 0
     args::ArgumentParser parser("lists posits",   "");
    //args::PositionalList<std::string> ptags(parser, "tags", "Tags to read");
    //args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    //args::ValueFlag<bool> pparallel(parser, "parallel", "Parallel", {'p', "parallel"});
    //args::Flag foo(group, "foo", "The foo flag", {'f', "foo"});

    try {
        parser.ParseCLI(argc, argv);
    }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       B                BB
    catch (args::ParseError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
#endif
    if(!*format || strchr(format,'h') != 0)
    {
        std::cout <<
        "B: binary output\n" 
        "H: overview only\n"
        "N: no overview\n"
        "U: unsigned order\n"
        "I: inverse\n"
        "2: twice\n"
        "A: half\n"
        "p: posit object print\n"
        "i: signed integer\n"
        "b: binary form\n"
        "u: unpacked\n"
        "o: unsigned \n"
        //"B: bits sizes: R E F\n"
        "k: k factor\n"
        "e: exponent\n"
        "r: regime\n"
        //"f: fraction (decimal)\n"
        //"F: fraction (binary)\n"
        "v: value\n"
        ;
        return 0;
    }
    bool headonly = false;
    bool noheader = false;
    bool doinverse = false;
    bool dotwice = false;
    bool dohalf = false;
    bool unsignedorder = false;
    bool binaryoutput = false;
    for(const char * po = format; *po ; ++po)
    {
        switch(*po)
        {
            case 'H': headonly = true; break;
            case 'N': noheader = true; break;
            case 'I': doinverse = true; break;
            case '2': dotwice = true; break;
            case 'A': dohalf = true; break;
            case 'U': unsignedorder = true;break;
            case 'B': binaryoutput = true; noheader=true; break;
            default:
                break;
        }
    }
    if(binaryoutput)
    {

    }
    if(!noheader)
    {
    	std::cout << "posit\n" << std::endl;
        std::cout << "holding type:" << typeid(typename X::value_t).name() << std::endl;
        std::cout << "holding size bits: " << sizeof(typename X::value_t)*8 << std::endl;
        std::cout << "totalbits:" <<  X::vtotalbits << std::endl;
        std::cout << "fractiontype:" << typeid(typename X::fraction_t).name() << std::endl;
        std::cout << "fractiontype size bits:" << sizeof(typename X::fraction_t)*8 << std::endl;
        std::cout << "exponent size bits:" << X::vesbits << std::endl;
        std::cout << "exponent type:" << typeid(typename X::exponenttype).name()<< std::endl;
        if(X::PT::withnan)
        {
            std::cout << "with signed infinity and NaN" << std::endl;
            std::cout << "+infinity: "  << (typename printableinttype<typename X::value_t>::type)(X::PT::POSIT_PINF) << " " << std::bitset<sizeof(typename X::value_t)*8>(X::PT::POSIT_PINF)  << std::endl;
            std::cout << "-infinity: "  << (typename printableinttype<typename X::value_t>::type)(X::PT::POSIT_NINF) << " " << std::bitset<sizeof(typename X::value_t)*8>(X::PT::POSIT_NINF)  << std::endl;
            std::cout << "nan: "  << (typename printableinttype<typename X::value_t>::type)(X::PT::POSIT_NAN) << " " << std::bitset<sizeof(typename X::value_t)*8>(X::PT::POSIT_NAN)  << std::endl;
        }
        else
        {
            std::cout << "without signed infinity and NaN" << std::endl;
            std::cout << "infinity "  << (typename printableinttype<typename X::value_t>::type)(X::PT::POSIT_PINF) << " " << std::bitset<sizeof(typename X::value_t)*8>(X::PT::POSIT_PINF)  << std::endl;
        }
        std::cout   << " half=" << (double)X(X::PT::POSIT_HALF) << " twice=" <<  (double)X(X::PT::POSIT_TWO) <<std::endl;
        std::cout   << " min=" << (double)X(X::PT::POSIT_MINNEG) << " " << (std::bitset<X::vtotalbits>(X::PT::POSIT_MINNEG)) <<  " max=" << X(X::PT::POSIT_MAXPOS)  <<std::endl;

        /*
        {
            // TODO populate std::limits
            typename X::value_t i1 = X::PT::POSIT_PINF-1;
            X Mx(typename X::DeepInit(),i1); // load the posit OK
            typename X::value_t i2 = 1;
            X mx(typename X::DeepInit(),i2); // load the posit OK

            std::cout << "smallest:" << std::hex << typename printableinttype<typename X::value_t>::type(i1) << " " << (std::bitset<X::vtotalbits>(i1)) << " " << posit_formatter<X>(mx) << " " << std::setprecision(12) << (double)mx<< std::endl;
            std::cout << "maximum:" << std::hex << typename  printableinttype<typename X::value_t>::type(i2) << " " << (std::bitset<X::vtotalbits>(i2)) << " " << posit_formatter<X>(Mx) << " " << std::setprecision(12) << (double)Mx<< std::endl;
        }
        int failed = 0;
        */
        if(headonly)
            return 0;

    }

    using nt=typename nextinttype<typename X::value_t>::type;
    using pt = typename printableinttype<typename X::value_t>::type;
    if(!binaryoutput)
    {
        if(unsignedorder)
        {
            std::cout << "looping unsigned " <<  std::dec <<  nt(0) << " to " <<  nt((1<<X::vtotalbits)-1)<< std::endl;
        }
        else
        {
            std::cout << "looping signed " <<  std::dec <<  pt(X::PT::POSIT_MINNEG) << " to " <<  pt(X::PT::POSIT_MAXPOS) << std::endl;
        }
    }

    if(!binaryoutput)
    {
        for(const char * p0 = format; *p0 ; ++p0)
        {
            char c= '\t';
            switch(*p0)
            {
                case 'i': std::cout << c<<"signed_int" ;  break;
                case 'p': std::cout << c<<"posit" ; break;
                case 'b': std::cout <<c << "bits"; break;            
                case 'k': std::cout << c << "k";  break;
                case 'e': std::cout << c<< "exponent" ; break;
                case 'u': std::cout << c << "unpacked"; break;
                case 'r': std::cout << c << "regime"; break;
                case 'v': std::cout << c << "float"; break;
                break;
                default:
                    break;
            }
        }
        std::cout << std::endl;
    }

    std::ofstream conf("tmp",std::ios::binary); // the true
    std::ostream & ons = binaryoutput? conf: std::cout;


    // scan all the 2**totalbits values
    if(unsignedorder)
    {
        for(nt j = 0; j < (1<<X::vtotalbits); j++)
        {
            typename X::value_t i = SIGNEX(j,X::vtotalbits-1);
            X x(typename X::DeepInit(),i); // load the posit OK
            print1(ons,x,format,binaryoutput);
            if(doinverse)   
                print1(ons,x.inv(),format,binaryoutput);
            if(dotwice)
                print1(ons,(X)(x*X(2)),format,binaryoutput);
            if(dohalf)
                print1(ons,(X)x/X(2),format,binaryoutput);
            if(!binaryoutput)
                std::cout << std::endl;
        }
    }
    else
    {
        typename X::value_t i = X::PT::POSIT_MINNEG; 
        do
    	{
            //typename X::value_t i = SIGNEX(j,X::vtotalbits-1);
    		X x(typename X::DeepInit(),i); // load the posit OK
            print1(ons,x,format,binaryoutput);
            if(doinverse)   
                print1(ons,x.inv(),format,binaryoutput);
            if(dotwice)
                print1(ons,(X)(x*X(2)),format,binaryoutput);
            if(dohalf)
                print1(ons,x/X(2),format,binaryoutput);
            if(!binaryoutput)
                ons << std::endl;
            /*
            X xt(x.twice());
            X xh(x.half());
            double xf = (double)x;
    		typename X::UnpackedT u(x.unpack()); // unpack it OK
    		X xux(u); // pack

        //    auto q = X::PT::decode_posit_rs(pabs(i));
            int rs = 0;//q.second;
            int reg = 0;//q.first;
             x.analyze();
            	std::cout << std::dec << std::dec << std::setw(8) << (int)i << " " << (std::bitset<X::vtotalbits>(i)) << " " ;
                printinfo<X>(std::cout,xux.v);
             std::cout << " " << xf << " x/2=(p " << (double)xh << " f " << (xf/2) << ") 2x=(p " << (double)xt <<  " f " << (2*xf) << ") 1/x=(p " << (double)(inv(x)) << " f " << (1/xf) << ")" << std::endl;

            if(xux.v != x.v)
            {
                failed++;
            	std::cout << "\tERROR at item " << std::dec  << i << " with value " << x.v << " != repacked " << xux.v << " unpacked: " << u << std::endl;        
            }
            //if(x.isinfinity())
    		//		std::cout << std::dec << std::setw(8) << i << " " << std::setw(X::vtotalbits+1) << (std::bitset<X::vtotalbits>(i)) << " " << std::setw(10) << "infinity"  << " " << std::setw(8) << std::dec <<  xux.v << " " << std::setw(X::vtotalbits+1) << (std::bitset<X::vtotalbits>(xux.v)) << " " << (xux.v == i) << " " << (double)xux << " " << rs << " " << reg << std::endl;
    		//else
    		//		std::cout << std::dec << std::setw(8) << i << " " << std::setw(X::vtotalbits+1) << (std::bitset<X::vtotalbits>(i)) << " " << std::setw(10) << (double)u  << " " << std::setw(8) << std::dec <<  xux.v << " " << std::setw(X::vtotalbits+1) << (std::bitset<X::vtotalbits>(xux.v)) << " " << (xux.v == i) << " " << (double)xux << " " << rs << " " << reg << std::endl;
            */
    	} while (i++ < X::PT::POSIT_MAXPOS);
    }
    ons << std::flush;
	return 0;
}

// cling entrypoint
void listposits()
{
     listpositsT<XT>(LISTFORMATX);
}


int main(int argc, char const *argv[])
{
    int failed = listpositsT<XT>(argc == 1 ? DEFAULTLISTFORMAT : argv[1]);
    return failed ? 5 : 0;
}
