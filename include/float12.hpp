/**
 * Copyright (C) 2017-2019 Emanuele Ruffaldi
 * Distributed under the terms of the BSD 3-Clause License.  
 * 
 * (See accompanying file LICENSE)
 * 
 * --
 */
#include <stdbool.h>
#include <stdint.h>
#include "platform.h"
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

struct ui12_f12
{

};
float12_t

#define packToF12UI( sign, exp, sig ) (((uint12_t) (sign)<<15) + ((uint12_t) (exp)<<10) + (sig))
#define softfloat_commonNaNToF12UI(aPtr) (uint_fast12_t) ((aPtr)->sign<<15 | 0x7E00 | aPtr->v64>>54)


int_float12_t
uint_float12_t

// f8  is typically 1 4 3
// f12 is typically 1 5 6 or 1 4 7
// f16 is           1 6 9
template <int exponentbits, int mantissabits, int mantissamask>
float12_t f32_to_f12( float32_t a )
{
    union ui32_f32 uA;
    uint_fast32_t uiA;
    bool sign;
    int_fast12_t exp;
    uint_fast32_t frac;
    struct commonNaN commonNaN;
    uint_fast12_t uiZ, frac16;
    union ui12_f12 uZ;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uA.f = a;
    uiA = uA.ui;
    sign = signF32UI( uiA );
    exp  = expF32UI( uiA );
    frac = fracF32UI( uiA );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( exp == 0xFF ) {
        if ( frac ) {
            softfloat_f32UIToCommonNaN( uiA, &commonNaN );
            uiZ = softfloat_commonNaNToF12UI( &commonNaN );
        } else {
            uiZ = packToF12UI( sign, 0x1F, 0 );
        }
        goto uiZ;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    frac16 = frac>> mantissabits| ((frac & mantissamask) != 0);
    if ( ! (exp | frac16) ) {
        uiZ = packToF12UI( sign, 0, 0 );
        goto uiZ;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    return softfloat_roundPackToF12( sign, exp - 0x71, frac16 | 0x4000 );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}



float12_t
 softfloat_roundPackToF12( bool sign, int_fast12_t exp, uint_fast12_t sig )
{
    uint_fast8_t roundingMode;
    bool roundNearEven;
    uint_fast8_t roundIncrement, roundBits;
    bool isTiny;
    uint_fast12_t uiZ;
    union ui12_f12 uZ;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    roundingMode = softfloat_roundingMode;
    roundNearEven = (roundingMode == softfloat_round_near_even);
    roundIncrement = 0x8;
    if ( ! roundNearEven && (roundingMode != softfloat_round_near_maxMag) ) {
        roundIncrement =
            (roundingMode
                 == (sign ? softfloat_round_min : softfloat_round_max))
                ? 0xF
                : 0;
    }
    roundBits = sig & 0xF;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( 0x1D <= (unsigned int) exp ) {
        if ( exp < 0 ) {
            /*----------------------------------------------------------------
            *----------------------------------------------------------------*/
            isTiny =
                (softfloat_detectTininess == softfloat_tininess_beforeRounding)
                    || (exp < -1) || (sig + roundIncrement < 0x8000);
            sig = softfloat_shiftRightJam32( sig, -exp );
            exp = 0;
            roundBits = sig & 0xF;
            if ( isTiny && roundBits ) {
                softfloat_raiseFlags( softfloat_flag_underflow );
            }
        } else if ( (0x1D < exp) || (0x8000 <= sig + roundIncrement) ) {
            /*----------------------------------------------------------------
            *----------------------------------------------------------------*/
            softfloat_raiseFlags(
                softfloat_flag_overflow | softfloat_flag_inexact );
            uiZ = packToF12UI( sign, 0x1F, 0 ) - ! roundIncrement;
            goto uiZ;
        }
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    sig = (sig + roundIncrement)>>4;
    if ( roundBits ) {
        softfloat_exceptionFlags |= softfloat_flag_inexact;
#ifdef SOFTFLOAT_ROUND_ODD
        if ( roundingMode == softfloat_round_odd ) {
            sig |= 1;
            goto packReturn;
        }
#endif
    }
    sig &= ~(uint_fast12_t) (! (roundBits ^ 8) & roundNearEven);
    if ( ! sig ) exp = 0;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
 packReturn:
    uiZ = packToF12UI( sign, exp, sig );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}

struct exp8_sig16 softfloat_normSubnormalF12Sig( uint_fast12_t sig )
{
    int_fast8_t shiftDist;
    struct exp8_sig16 z;

    shiftDist = softfloat_countLeadingZeros16( sig ) - 5; // TODO
    z.exp = 1 - shiftDist;
    z.sig = sig<<shiftDist;
    return z;

}


void softfloat_f12UIToCommonNaN( uint_fast12_t uiA, struct commonNaN *zPtr )
{
    if ( softfloat_isSigNaNF12UI( uiA ) ) {
        softfloat_raiseFlags( softfloat_flag_invalid );
    }
    zPtr->sign = uiA>>15;
    zPtr->v64  = (uint_fast64_t) uiA<<54;
    zPtr->v0   = 0;
}


float32_t f12_to_f32( float12_t a )
{
    union ui12_f12 uA;
    uint_fast12_t uiA;
    bool sign;
    int_fast8_t exp;
    uint_fast12_t frac;
    struct commonNaN commonNaN;
    uint_fast32_t uiZ;
    struct exp8_sig16 normExpSig;
    union ui32_f32 uZ;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uA.f = a;
    uiA = uA.ui;
    sign = signF12UI( uiA );
    exp  = expF12UI( uiA );
    frac = fracF12UI( uiA );
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( exp == 0x1F ) {
        if ( frac ) {
            softfloat_f12UIToCommonNaN( uiA, &commonNaN );
            uiZ = softfloat_commonNaNToF32UI( &commonNaN );
        } else {
            uiZ = packToF32UI( sign, 0xFF, 0 );
        }
        goto uiZ;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ( ! exp ) {
        if ( ! frac ) {
            uiZ = packToF32UI( sign, 0, 0 );
            goto uiZ;
        }
        normExpSig = softfloat_normSubnormalF12Sig( frac );
        exp = normExpSig.exp - 1;
        frac = normExpSig.sig;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uiZ = packToF32UI( sign, exp + 0x70, (uint_fast32_t) frac<<13 );
 uiZ:
    uZ.ui = uiZ;
    return uZ.f;

}


