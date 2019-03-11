/**
 * Copyright (C) 2017-2019 Emanuele Ruffaldi
 * Distributed under the terms of the BSD 3-Clause License.  
 * 
 * (See accompanying file LICENSE)
 * 
 * --
 */
/**
 * Emanuele Ruffaldi (C) 2017-2018
 *
 * cppPosit project
 * gneralized soft float in unpackd form

 *
 */
#pragma once

#include <cstdint>
#ifndef FPGAHLS
#include <iostream>
#endif
#include <inttypes.h>
#include <math.h>
#include <bitset>
#include <limits>
#include <ratio>
#include "bithippop.hpp"
#include "fixedtraits.hpp"
#include "floattraits.hpp"
#include "typehelpers.hpp"


template <class T>
constexpr const T &clamp(const T &v, const T &lo, const T &hi)
{
  return v < lo ? lo : v > hi ? hi : v;
}

template <class T>
constexpr T FLOORDIV(T a, T b)
{
  return ((a) / (b) - ((a) % (b) < 0));
}


template <class FT = uint64_t, class ET = int32_t>
struct Unpacked
{
  static_assert(std::is_unsigned<FT>::value,
                "Unpacked requires unsigned fractiont type or floating");
  static_assert(std::is_signed<ET>::value,
                "Unpacked requires signed exponent type");
  using POSIT_LUTYPE = FT;
  enum
  {
    FT_bits = sizeof(FT) * 8
  };
  enum : FT
  {
    FT_leftmost_bit = (((FT)1) << (FT_bits - 1))
  };
#ifndef UnpackedDualSel
#define UnpackedDualSel(a, b) ((a) + (b)*4)
#endif
  enum Type
  {
    Regular,
    Infinity,
    NaN,
    Zero
  }; /// signed infinity and nan require the extra X bit


  Type type = Regular;
  bool negativeSign = false;
  ET exponent = 0; // with sign
  FT fraction = 0; // this can be 52bit for holding double.

  struct single_tag
  {
  };

  explicit CONSTEXPR14 Unpacked(single_tag, uint32_t p)
  {
    unpack_xfloati<single_trait>(p);
  }

  explicit constexpr Unpacked() {}

  // assume regular
  CONSTEXPR14 Unpacked normalized() const
  {
    if (fraction == 0)
    {
      return Unpacked(Zero, false);
    }
    else
    {
      int k = findbitleftmostC(fraction);
      return Unpacked(exponent - k, fraction << (k + 1), negativeSign);
    }
  }

  explicit CONSTEXPR14 Unpacked(halffloat p) { unpack_half(p); }
  explicit CONSTEXPR14 Unpacked(int i) { unpack_int(i); }
  explicit CONSTEXPR14 Unpacked(Type t, bool anegativeSign = false)
      : type(t), negativeSign(anegativeSign){};

  template <class Trait, typename = typename std::enable_if<std::is_integral<
                             typename Trait::value_t>::value>::type>
  explicit CONSTEXPR14 Unpacked(typename Trait::value_t i)
  {
    unpack_xfixed<Trait>(i);
  }

  template <class Trait, typename = typename std::enable_if<!std::is_integral<
                             typename Trait::value_t>::value>::type>
  explicit CONSTEXPR14 Unpacked(typename Trait::holder_t i)
  {
    unpack_xfloat<Trait>(i);
  }

  // expect 1.xxxxxx otherwise make it 0.xxxxxxxxx
  explicit CONSTEXPR14 Unpacked(ET aexponent, FT afraction, bool anegativeSign)
      : type(Regular),
        negativeSign(anegativeSign),
        exponent(aexponent),
        fraction(afraction) {}

#ifndef FPGAHLS
  explicit CONSTEXPR14 Unpacked(float p)
  {
    unpack_float(p);
  }
  explicit CONSTEXPR14 Unpacked(double p) { unpack_double(p); }
  CONSTEXPR14 Unpacked &unpack_float(float f)
  {
    return unpack_xfloat<single_trait>(f);
  }
  CONSTEXPR14 Unpacked &unpack_double(double d)
  {
    return unpack_xfloat<double_trait>(d);
  }
  constexpr operator float() const { return pack_xfloat<single_trait>(); }
  constexpr operator double() const { return pack_xfloat<double_trait>(); }
#endif

  CONSTEXPR14 Unpacked &unpack_half(halffloat d)
  {
    return unpack_xfloat<half_trait>(d);
  }
  CONSTEXPR14 Unpacked &unpack_int(int i)
  {
    return unpack_xfixed<fixedtrait<int, sizeof(int) * 8, 0>>(i);
  }

  constexpr operator halffloat() const { return pack_xfloat<half_trait>(); }
  constexpr operator int() const
  {
    return pack_xfixed<fixedtrait<int, sizeof(int) * 8, 0>>();
  }

  template <class Trait>
  CONSTEXPR14 typename Trait::holder_t pack_xfloati() const;

  template <class Trait>
  CONSTEXPR14 typename Trait::value_t pack_xfixed() const;

  template <class Trait>
  typename Trait::value_t pack_xfloat() const
  {
    union {
      typename Trait::holder_t i;
      typename Trait::value_t f;
    } uu;
    uu.i = pack_xfloati<Trait>();
    return uu.f;
  }

  template <class T>
  constexpr T pack_float() const
  {
    return pack_xfloat<typename float2trait<T>::trait>();
  }

  constexpr bool isInfinity() const { return type == Infinity; }
  constexpr bool isRegular() const { return type == Regular; }
  constexpr bool isNaN() const { return type == NaN; }
  constexpr bool isZero() const { return type == Zero; }
  constexpr bool isPositive() const { return !negativeSign; }

  static constexpr Unpacked infinity() { return Unpacked(Infinity); }
  static constexpr Unpacked pinfinity() { return Unpacked(Infinity, false); }
  static constexpr Unpacked ninfinity() { return Unpacked(Infinity, true); }
  static constexpr Unpacked nan() { return Unpacked(NaN); }
  static constexpr Unpacked one() { return Unpacked(0, 0, false); }
  static constexpr Unpacked zero() { return Unpacked(Zero); }
  template <class Trait>
  static constexpr Unpacked make_fixed(typename Trait::value_t x)
  {
    return Unpacked().unpack_xfixed<Trait>(x);
  }

  template <class Trait>
  static constexpr Unpacked make_floati(typename Trait::holder_t x)
  {
    return Unpacked().unpack_xfloati<Trait>(x);
  }

  template <class Trait>
  static constexpr Unpacked make_float(typename Trait::value_t x)
  {
    return Unpacked().unpack_xfloat<Trait>(x);
  }

  constexpr bool operator==(const Unpacked &u) const
  {
    // nan != nan ALWAYS
    return type == NaN || u.type == NaN
               ? false
               : negativeSign == u.negativeSign && type == u.type &&
                     (type == Regular
                          ? (exponent == u.exponent && fraction == u.fraction)
                          : true);
  }

  constexpr bool operator!=(const Unpacked &u) const
  {
    // nan != nan ALWAYS
    return type == NaN || u.type == NaN ? true : (*this == u);
  }

  constexpr Unpacked operator-() const
  {
    return Unpacked(exponent, fraction, !negativeSign);
  }

  CONSTEXPR14 Unpacked inv() const
  {
    switch (type)
    {
    case Regular:
      if (fraction == 0)
      {
        // std::cout << "[exponent inversion " <<  std::dec  << " exponent" <<
        // exponent <<  "] becomes " << -exponent << std::endl;
        return Unpacked(-exponent, 0, negativeSign);
      }
      else
      {
        // one == 0,0,false
        // TODO FIX SIGN/INFINITY/NAN
        // put hidden 1. in mantiss
        POSIT_LUTYPE afrac = FT_leftmost_bit;
        POSIT_LUTYPE bfrac = FT_leftmost_bit | (fraction >> 1);
        //   std::cout << "inversion " << std::hex  << bfrac << " exponent" <<
        //   exponent << std::endl;
        auto exp = -exponent;

        if (afrac < bfrac)
        {
          exp--;
          bfrac >>= 1;
        }

        return Unpacked(exp, nextintop<FT>::extradiv(((typename nextinttype<FT>::type)afrac) << FT_bits,bfrac),
            negativeSign);

        // return one()/(*this);
      }
      break;
    case Infinity:
      return zero();
    case Zero:
      return infinity();
    case NaN:
    default:
      return *this;
    }
  }

  /// unpacks a value stored as fixed or integer. Value and holder match
  template <class Trait>
  CONSTEXPR14 Unpacked &unpack_xfixed(typename Trait::value_t value);

  /// unpacks a floating point value as expressed by its holding type (uint32
  /// for single)
  template <class Trait>
  CONSTEXPR14 Unpacked &unpack_xfloati(typename Trait::holder_t value);

  /// unpacks a floating point value by its value type (single)
  template <class Trait>
  Unpacked &unpack_xfloat(typename Trait::value_t value) // CANNOT be
                                                         // constexpr, except
                                                         // using the expensive
                                                         // float2bits
  {
    union {
      typename Trait::holder_t i;
      typename Trait::value_t f;
    } uu;
    uu.f = value;
    return unpack_xfloati<Trait>(uu.i);
  }

  CONSTEXPR14 friend Unpacked operator-(Unpacked a, Unpacked b)
  {
    return a + (-b);
  }

  CONSTEXPR14 Unpacked &operator+=(const Unpacked &a)
  {
    Unpacked r = *this + a;
    *this = r;
    return *this;
  }
  CONSTEXPR14 Unpacked &operator-=(const Unpacked &a)
  {
    *this += (-a);
    return *this;
  }

  // TODO overflow?
  CONSTEXPR14 friend Unpacked operator+(Unpacked a, Unpacked b)
  {
    // UnpackedDualSel(*,NaN)
    // UnpackedDualSel(NaN,*)
    if (a.isNaN() || b.isNaN())
      return a;
    switch (UnpackedDualSel(a.type, b.type))
    {
    case UnpackedDualSel(Regular, Regular):
    {
      auto dir = a.exponent - b.exponent;
      const ET exp = (dir < 0 ? b.exponent : a.exponent) + 1;

      // move right means increment exponent
      // 1.xxxx => 0.1xxxxxx
      // 1.yyyy => 0.1yyyyyy
      POSIT_LUTYPE afrac1 =
          (FT_leftmost_bit >> 1) |
          (a.fraction >> 2); // denormalized and shifted right
      POSIT_LUTYPE bfrac1 = (FT_leftmost_bit >> 1) | (b.fraction >> 2);
      POSIT_LUTYPE afrac = dir < 0
                               ? (afrac1 >> -dir)
                               : afrac1; // denormalized and shifted right
      POSIT_LUTYPE bfrac = dir < 0 ? bfrac1 : (bfrac1 >> dir);

      // 1.xxxx => 0.1xxxxx => 0.0k 1 xxxx
      //
      // if dir==0 then:
      //   0.1xxxxx
      //   0.1yyyyy
      //   1.zzzzzz
      //
      // but also
      //   0.1xxxx
      //   0.0001yyyy
      //   0.1zzzz
      //
      // if 1. we easily normalize by shift
      // if 0. we pre
      int mode =
          a.negativeSign == b.negativeSign ? 0 : afrac > bfrac ? 1 : -1;
      bool osign = mode >= 0 ? a.negativeSign : b.negativeSign;
      POSIT_LUTYPE frac = mode == 0
                              ? afrac + bfrac
                              : mode > 0 ? afrac - bfrac : bfrac - afrac;
      return Unpacked(exp, frac, osign).normalized(); // pass denormalized
    }
    case UnpackedDualSel(Regular, Zero):
    case UnpackedDualSel(Zero, Zero):
    case UnpackedDualSel(Infinity, Zero):
    case UnpackedDualSel(Infinity, Regular):
      return a;
    case UnpackedDualSel(Zero, Regular):
    case UnpackedDualSel(Zero, Infinity):
    case UnpackedDualSel(Regular, Infinity):
      return b;
    default: // case UnpackedDualSel(Infinity,Infinity):
      return (a.negativeSign == b.negativeSign) ? a : nan();
    }
  }

  // https://www.edwardrosten.com/code/fp_template.html
  // https://github.com/Melown/half
  // TODO overflow?
  CONSTEXPR14 friend Unpacked operator*(const Unpacked &a, const Unpacked &b)
  {
    if (a.isNaN() || b.isNaN())
      return a;
    switch (UnpackedDualSel(a.type, b.type))
    {
    case UnpackedDualSel(Regular, Regular):
    {
      POSIT_LUTYPE afrac = FT_leftmost_bit | (a.fraction >> 1);
      POSIT_LUTYPE bfrac = FT_leftmost_bit | (b.fraction >> 1);
      auto frac = nextintop<FT>::extramul(afrac,bfrac) >> FT_bits;
#ifdef FPGAHLS
#pragma HLS RESOURCE variable = frac core = Mul_LUT
#endif
      bool q = (frac & FT_leftmost_bit) == 0;
      auto rfrac = q ? (frac << 1) : frac;
      auto exp = a.exponent + b.exponent + (q ? 0 : 1);
#if 0
                if ((frac & FT_leftmost_bit) == 0) {
                    exp--;
                    frac <<= 1;
                }
#endif
      return Unpacked(exp, rfrac << 1, a.negativeSign ^ b.negativeSign);
    }
    case UnpackedDualSel(Regular, Zero):
    case UnpackedDualSel(Zero, Regular):
    case UnpackedDualSel(Zero, Zero):
      return zero();
    case UnpackedDualSel(Infinity, Zero):
    case UnpackedDualSel(Zero, Infinity):
      return nan();
    default: // case UnpackedDualSel(Infinity,Infinity):
      // inf inf or inf reg or reg inf
      return (a.negativeSign ^ b.negativeSign) ? ninfinity() : pinfinity();
    }
  }

  /**
   * Division Truth Table

   */
  // TODO overflow?
  CONSTEXPR14 friend Unpacked operator/(const Unpacked &a, const Unpacked &b)
  {
    if (a.isNaN() || b.isNaN())
      return a;

    // 9 more cases
    switch (UnpackedDualSel(a.type, b.type))
    {
    case UnpackedDualSel(Regular, Regular):
    {
      POSIT_LUTYPE afrac = FT_leftmost_bit | (a.fraction >> 1);
      POSIT_LUTYPE bfrac1 = FT_leftmost_bit | (b.fraction >> 1);
      auto exp = a.exponent - b.exponent + (afrac < bfrac1 ? -1 : 0);
      POSIT_LUTYPE bfrac = afrac < bfrac1 ? (bfrac1 >> 1) : bfrac1;
      /*
        if (afrac < bfrac) {
            exp--;
            bfrac >>= 1;
        }
        */

      return Unpacked(exp, nextintop<FT>::extradiv(((typename nextinttype<FT>::type)afrac) << FT_bits,bfrac),
          a.negativeSign ^ b.negativeSign);
    }
    case UnpackedDualSel(Zero, Zero):
    case UnpackedDualSel(Infinity, Infinity):
      return nan();
    case UnpackedDualSel(Zero, Infinity):
      return zero();
    case UnpackedDualSel(Zero, Regular):
    case UnpackedDualSel(Infinity, Zero):
      return a;
    case UnpackedDualSel(Regular, Zero):
      return Unpacked(Unpacked::Infinity, a.negativeSign);
    default: // case UnpackedDualSel(Infinity,Regular):
      return (a.negativeSign ^ b.negativeSign) ? ninfinity() : pinfinity();
    }
  }

#ifndef FPGAHLS
  friend std::ostream &operator<<(std::ostream &ons, Unpacked const &o)
  {
    switch (o.type)
    {
    case Unpacked::Regular:
      ons << "up(" << (o.negativeSign ? "-" : "+")
          << " exp (dec) = " << std::dec
          << typename printableinttype<const ET>::type(o.exponent)
          << " fraction (hex) = " << std::hex
          << typename printableinttype<const FT>::type(o.fraction)
          << " (bin) = " << std::dec
          << (std::bitset<sizeof(o.fraction) * 8>(o.fraction)) << ")";
      break;
    case Unpacked::Infinity:
      ons << (o.negativeSign ? "up(-infinity)" : "up(+infinity)");
      break;
    case Unpacked::NaN:
      ons << "up(nan)";
      break;
    case Unpacked::Zero:
      ons << "up(0)";
      break;
    }
    return ons;
  }
#endif
};

template <class FT, class ET>
template <class Trait>
CONSTEXPR14 Unpacked<FT, ET> &Unpacked<FT, ET>::unpack_xfixed(
    typename Trait::value_t nx)
{
  // TODO: handle infinity or nan in Trait
  if (nx != 0)
  {
    using UT = typename int_least_bits<sizeof(typename Trait::value_t)*8>::unsigned_type;
    type = Regular;
    negativeSign = nx < 0;
    UT x = pcabs(nx);
    const int p = Trait::totalbits - findbitleftmostC(x) - 1; // 31->0,0->31
    exponent = (p - Trait::fraction_bits);
    UT ux = p == 0 ? 0 : (x << (Trait::totalbits - p));

    // UT x : 0[N-p-1] 1 ?[p]
    // corner cases:
    //  x: 1 ?[N-1]
    //  x: 0[N-1] 1
    // UT ux: ?[p] 0[N-p]
    // FT f:  ?[min(p,size(FT)] 0[size(FT)-min(p,size(FT)]
    // take all p bits rightmost of x and make them leftmost
    fraction = cast_right_to_left<UT, Trait::totalbits, FT, FT_bits>()(ux);
  }
  else
  {
    exponent = 0;
    fraction = 0;
    type = Zero;
    negativeSign = false;
  }
  return *this;
}

// https://www.h-schmidt.net/FloatConverter/IEEE754.html
template <class FT, class ET>
template <class Trait>
CONSTEXPR14 Unpacked<FT, ET> &Unpacked<FT, ET>::unpack_xfloati(
    typename Trait::holder_t value)
{
  ET rawexp = bitset_getT(value, Trait::fraction_bits, Trait::exponent_bits);
  type = Regular;
  negativeSign =
      value & (((typename Trait::holder_t)1) << (Trait::data_bits - 1));
  exponent = rawexp - Trait::exponent_bias; // ((un.u >> Trait::fraction_bits)
                                            // & Trait::exponent_mask)

  // std::cout  << "un.u is " << std::hex <<un.u << " for " << value <<
  // std::endl;  std::cout << std::dec << "float trait: fraction bits " <<
  // Trait::fraction_bits << " exponent bits " << Trait::exponent_bits << " bias
  // " << Trait::exponent_bias << " mask " << std::hex << Trait::exponent_mask<<
  // std::endl;  std::cout << std::hex << "exponent output " << std::hex <<
  // exponent  << " " << std::dec << exponent << " fraction " << std::hex <<
  // fraction << std::endl;

  // fractional part is LSB of the holder_t and of length
  fraction = cast_right_to_left<typename Trait::holder_t, Trait::fraction_bits,
                                FT, FT_bits>()(value);

  // if(FT_bits < Trait::fraction_bits)
  //	fraction = bitset_getT(value,0,Trait::fraction_bits) >>
  //(Trait::fraction_bits-FT_bits);  else 	fraction =
  //((POSIT_LUTYPE)bitset_getT(value,0,Trait::fraction_bits)) <<
  //(FT_bits-Trait::fraction_bits);

  // stored exponent: 0, x, exponent_mask === 0, any, infinity
  // biased: -max, -max+1, ..., max, max+1 === 0, min, ..., max, infinity
  if (rawexp == ((1 << Trait::exponent_bits) - 1)) // AKA 128 for single
  {
    if (fraction == 0)
    {
      type = Infinity;
    }
    else
    {
      type = NaN; // don't care which
    }
  }
  else if (rawexp == 0)
  {
    // normalized
    if (!Trait::with_denorm || fraction == 0)
    {
      type = Zero;
      negativeSign = false; // don't care signed zero
    }
    // denormalized
    else
    {
      int k = findbitleftmostC(fraction);
      exponent -= k;
      fraction <<= (k + 1);
      //std::cout << typeid(Trait).name() << "unpacking: denormalized (rawexp=0,fraction=" <<  (int)tmp << ") unpacked as  (exp=" << exponent << ",fraction=" << (int)fraction << ")"<< std::endl;
    }
  }
  return *this;
}

template <int abits, class AT, int bbits, class BT, bool abits_gt_bbits, AT msb>
struct fraction_bit_extract
{
};

/// specialization when abits >= bbits
template <int abits, class AT, int bbits, class BT, AT msb>
struct fraction_bit_extract<abits, AT, bbits, BT, true, msb>
{
  static constexpr BT packdenorm(AT fraction)
  {
    // expand the fractiona part
    return (msb | (fraction >> 1)) >> (abits - bbits);
  }

  static constexpr BT pack(AT fraction)
  {
    return bitset_getT(fraction, abits - bbits, bbits);
  }
};

/// specialization when bbits >= abits
template <int abits, class AT, int bbits, class BT, AT msb>
struct fraction_bit_extract<abits, AT, bbits, BT, false, msb>
{
  static constexpr BT packdenorm(AT fraction)
  {
    return ((BT)(msb | (fraction >> 1)) << (bbits - abits));
  }

  static constexpr BT pack(AT fraction)
  {
    return ((BT)fraction) << (bbits - abits);
  }
};

/**
 * Convert (s,2**E,F) to int
 */
template <class FT, class ET>
template <class Trait>
CONSTEXPR14 typename Trait::value_t Unpacked<FT, ET>::pack_xfixed() const
{
  switch (type)
  {
  case Infinity:
    return 0;
  case Zero:
    return 0;
  case NaN:
    return 0;
  default:
    break;
  }
  constexpr int intbits = Trait::totalbits - Trait::fraction_bits;
  if (exponent >= intbits)
  {
    return negativeSign ? std::numeric_limits<typename Trait::value_t>::lowest()
                        : std::numeric_limits<typename Trait::value_t>::max();
  }
  else if (exponent < -Trait::fraction_bits)
  {
    return 0;
  }
  else
  {
    using ST = typename Trait::value_t;
    using UT = typename int_least_bits<sizeof(ST)*8>::unsigned_type;
    // fraction 1.xxxxx from left aligned over FT bytes to UT bytes still left
    // aligned over Trait::totalbits
    UT f = fraction_bit_extract<FT_bits, FT, Trait::totalbits, UT,
                                (FT_bits > Trait::totalbits),
                                FT_leftmost_bit>::pack(fraction);

    // add the 1 bit for the current exponent
    // f[totalbits] -> 0[intbits-exponent+1] 1
    // f[exponent+Trait::fraction_bits-1]
    //
    // extrema: e.g. for totalbits=32, whatever fraction
    // - exponent==-Trait::fraction_bits ==> 1 | 0
    // - exponent==intbits-1 ==> 0x8000000 | (F >> 1)
    ST r = (ST(1) << (exponent + Trait::fraction_bits)) |
           (ST)(f >> (intbits - exponent));
    return negativeSign ? -r : r;
  }
}

template <class FT, class ET>
template <class Trait>
CONSTEXPR14 typename Trait::holder_t Unpacked<FT, ET>::pack_xfloati() const
{
  switch (type)
  {
  case Infinity:
    return negativeSign ? Trait::ninfinity_h : Trait::pinfinity_h;
  case Zero:
    return 0;
  case NaN:
    return Trait::nan_h;
    ; // it will cast to double TODO: it will cast to value_t
  default:
    break;
  }

  largest_type<ET, typename int_least_bits<Trait::exponent_bits>::signed_type> fexp =
      exponent;
  fexp += Trait::exponent_bias;

  // left aligned
  typename Trait::holder_t fexpbits = 0;
  typename Trait::holder_t ffracbits = 0;

  if (fexp > Trait::exponent_max) // this is RAW exponent max
  {
    return negativeSign ? Trait::lowest_h : Trait::max_h;
    // overflow, set as MAX
    // fexpbits = ((typename Trait::holder_t)Trait::exponent_max) <<
    // (Trait::fraction_bits); // AKA 254 and 23  ffracbits = -1;
  }
  else if (fexp < 1)
  {
    if (Trait::with_denorm)
      return 0;
    else
    {
      // denormalized numbers are and happen when raw exponent is below 1
      // 0.fractionbits
      //
      // 2^exponent * fraction ==> 0.rawfraction
      //
      // approach:
      //  FT_bits >= Trait::fraction_bits
      //      FT_leftmost_bit| (fraction>>)
      //  FT_bits < Trait::fraction_bits

      ffracbits = fraction_bit_extract<FT_bits, FT, Trait::fraction_bits,
                                       typename Trait::holder_t,
                                       (FT_bits > Trait::fraction_bits),
                                       FT_leftmost_bit>::packdenorm(fraction);
      // use denormalization
      ffracbits >>= -fexp;
      //std::cout << typeid(Trait).name()<< "pack - denormalized (exp=" << exponent << ",fraction=" << (int)fraction << ") means rawexp=" << fexp << " results in fraction=" << (int)ffracbits << "\n";
    }
  }
  else // normal
  {
    fexpbits = ((typename Trait::holder_t)(fexp & Trait::exponent_mask))
               << (Trait::fraction_bits);
    ffracbits = fraction_bit_extract<
        FT_bits, FT, Trait::fraction_bits, typename Trait::holder_t,
        (FT_bits > Trait::fraction_bits), FT_leftmost_bit>::pack(fraction);
  }

  typename Trait::holder_t value =
      ffracbits | fexpbits | (negativeSign ? Trait::signbit : 0);

  // don't underflow to zero?
  if (value != 0 && (value << 1) == 0)
  {
    value++;
  }
  return value;
}

template <class SrcTrait, class DstTrait, class FT>
constexpr typename DstTrait::holder_t convertfloats(
    typename SrcTrait::holder_t src)
{
  return Unpacked<typename largest_type<typename SrcTrait::holder_t,
                                        typename DstTrait::holder_t>::type,
                  int>::template make_floati<SrcTrait>(src)
      .template pack_xfloati<DstTrait>();
}

#if 0
#ifndef FPGAHLS

template <class ET>
struct Unpacked<float,ET>
{

  struct single_tag
  {
  };

  enum Type
  {
    Regular,
    Infinity,
    NaN,
    Zero
  }; /// signed infinity and nan require the extra X bit

  float v;

  Unpacked() : v(0) {}
  Unpacked(float f) : v(f) {}

  Unpacked(double f): v(f) {}

  Unpacked(int f): v(f) {}

  friend Unpacked operator+ (const Unpacked &a, const Unpacked & b) { return Unpacked(a.v+b.v); }
  friend Unpacked operator- (const Unpacked &a, const Unpacked & b) { return Unpacked(a.v-b.v); }
  friend Unpacked operator/ (const Unpacked &a, const Unpacked & b) { return Unpacked(a.v/b.v); }
  friend Unpacked operator* (const Unpacked &a, const Unpacked & b) { return Unpacked(a.v*b.v); }
};
#endif
#endif
