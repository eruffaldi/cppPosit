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
#pragma once
#include "posit.h"
#include <Eigen/Core>

namespace Eigen {

template <class T,int totalbits, int esbits, class FT, PositSpec positspec>
struct NumTraits<Posit<T,totalbits,esbits,FT, positspec> >
 :  GenericNumTraits<Posit<T,totalbits,esbits,FT,positspec> > // permits to get the epsilon, dummy_precision, lowest, highest functions
{
  typedef Posit<T,totalbits,esbits,FT,positspec> P;

  typedef P Real;
  typedef P NonInteger;
  typedef P Nested;

  //static inline Real epsilon() { return 0; }
  //static inline Real dummy_precision() { return 0; }
  //static inline Real digits10() { return 0; }
  // highest() and lowest() functions returning the highest and lowest possible values respectively.
  // An epsilon() function which, unlike std::numeric_limits::epsilon(), it returns a Real instead of a T.
  // digits10() function returning the number of decimal digits that can be represented without change. This is the analogue of std::numeric_limits<T>::digits10 which is used as the default implementation if specialized.
  

  enum {
    IsComplex = 0,
    IsInteger = 0,
    IsSigned = 1,
    RequireInitialization = 1,
    ReadCost = 1,
    AddCost = 3,
    MulCost = 3
  };
};



template <class FT, class ET>
struct NumTraits<Unpacked<FT,ET> >
 :  GenericNumTraits<Unpacked<FT,ET> > // permits to get the epsilon, dummy_precision, lowest, highest functions
{
  typedef Unpacked<FT,ET> P;

  typedef P Real;
  typedef P NonInteger;
  typedef P Nested;

  //static inline Real epsilon() { return 0; }
  //static inline Real dummy_precision() { return 0; }
  //static inline Real digits10() { return 0; }
  // highest() and lowest() functions returning the highest and lowest possible values respectively.
  // An epsilon() function which, unlike std::numeric_limits::epsilon(), it returns a Real instead of a T.
  // digits10() function returning the number of decimal digits that can be represented without change. This is the analogue of std::numeric_limits<T>::digits10 which is used as the default implementation if specialized.
  

  enum {
    IsComplex = 0,
    IsInteger = 0,
    IsSigned = 1,
    RequireInitialization = 1,
    ReadCost = 1,
    AddCost = 3,
    MulCost = 3
  };
};

namespace internal {

  template <class T,int totalbits, int esbits, class FT, PositSpec positspec>
      inline typename Posit<T,totalbits,esbits,FT,positspec>::UnpackedT cast(const Posit<T,totalbits,esbits,FT,positspec>& x)
    { return x.unpack(); }
}

#if 0
namespace internal {

  template <class T,int totalbits, int esbits, class FT>
      inline double cast<Posit<T,totalbits,esbits,FT>,double>(const Posit<T,totalbits,esbits,FT>& x)
    { return x.todouble(); }

  template <class T,int totalbits, int esbits, class FT>
  inline float cast<Posit<T,totalbits,esbits,FT>,float>(const Posit<T,totalbits,esbits,FT>& x)
    { return x.tofloat(); }

  template <class T,int totalbits, int esbits, class FT>
    inline Posit<T,totalbits,esbits,FT> cast<float,Posit<T,totalbits,esbits,FT> >(const float & x)
    { return Posit<T,totalbits,esbits,FT>::convert(x); }

  template <class T,int totalbits, int esbits, class FT>
    inline Posit<T,totalbits,esbits,FT> cast<double,Posit<T,totalbits,esbits,FT> >(const double & x)
    { return Posit<T,totalbits,esbits,FT>::convert(x); }

}
#endif

}