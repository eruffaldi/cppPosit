/**
 * Copyright (C) 2017-2019 Emanuele Ruffaldi
 * Distributed under the terms of the BSD 3-Clause License.  
 * 
 * (See accompanying file LICENSE)
 * 
 * --
 */
/**
 * Requires: AVX2, allows for AVX512F
 *
 - missing: 64bit integer, unsigned integers except uint8
 - AVX2: double 4, float/int32 8, int16 16, int8 32

 *
 * 128bit for ui8: 16 => requires simd_i32_16 (AVX512 512bit or array for AVX2)
 * 256bit for ui8: 32 => requires simd_i32_32 (array)
 * 512bit for ui8: 64 => requires simd_i32_64 (array)

 */
#pragma once
#include "asimd_32.h"

#ifdef WITH_AVX512VLBW

#if 0

/**
 64 x uint8_t
 */
class simd_i8_64
{
public:
    typedef int8_t type;
    typedef __m512i simdtype;
    typedef simd_x_a<simd_i32_16,4> indextype; 
    typedef NoGather gathermode;
    typedef simd_i8_64 self;
    typedef __mmask64 cmpresult;

    enum { csize = 64 };
    
    inline simd_i8_64() {}
    inline simd_i8_64(type v) : x(_mm512_set1_epi8(v)) {}
    inline simd_i8_64(simdtype v) : x(v) {}
    inline void load(const type * ptr) { x = _mm512_loadu_ps((const float*)ptr); }
    inline void store(type * ptr) const { _mm512_storeu_ps((float*)ptr,x); }
    inline self max(self & y) const { return self(_mm512_max_epi8(x,y.x)); }
    inline cmpresult cmplt(self & y) const { return _mm512_cmp_epi8_mask(x,y.x,1); }
    inline unsigned int size() const { return csize; }    

    void initincrement(type x)
    {
        type a[csize];
        for(int i = 0; i < csize; i++)
            a[i] = i*x;
        load(a);
    }

    /*
    inline type operator[] (unsigned int idx) const
    {
        type temp[csize];
        store(temp);
        return temp[idx];
    } 
    */   

    inline void blend(self & other, cmpresult mask)
    {
        x = _mm512_mask_blendv_epi8(mask,x,other.x);
    }

    inline void blendindex(indextype & oindex, indextype other, cmpresult mask)
    {
        // __mmask64 => four integers vectors
        blendindex(oindex[0],other[0],(__mmask16)(mask >> 0));
        blendindex(oindex[1],other[1],(__mmask16)(mask >> 16));
        blendindex(oindex[2],other[2],(__mmask16)(mask >> 32));
        blendindex(oindex[3],other[3],(__mmask16)(mask >> 48));
    }

    simdtype x;
};


/**
 64 x uint8_t
 */
class simd_u8_64
{
public:
    typedef uint8_t type;
    typedef __m512i simdtype;
    typedef simd_x_a<simd_i32_16,2> indextype; 
    typedef NoGather gathermode;
    typedef simd_u8_64 self;
    typedef __mmask64 cmpresult;

    enum { csize = 64 };
    
    inline simd_u8_64() {}
    inline simd_u8_64(type v) : x(_mm512_set1_epi8(v)) {}
    inline simd_u8_64(simdtype v) : x(v) {}
    inline void load(const type * ptr) { x = _mm512_loadu_ps((const float*)ptr); }
    inline void store(type * ptr) const { _mm512_storeu_ps((float*)ptr,x); }
    inline self max(self & y) const { return self(_mm512_max_epu8(x,y.x)); }
    inline cmpresult cmplt(self & y) const { return _mm512_cmp_epu8_mask(x,y.x,1); }
    inline unsigned int size() const { return csize; }    

    void initincrement(type x)
    {
        type a[csize];
        for(int i = 0; i < csize; i++)
            a[i] = i*x;
        load(a);
    }

    /*
    inline type operator[] (unsigned int idx) const
    {
        type temp[csize];
        store(temp);
        return temp[idx];
    } 
    */   

    inline void blend(self & other, cmpresult mask)
    {
        x = _mm512_mask_blendv_epi8(mask,x,other.x);
    }

    static void blendindex(indextype & oindex, indextype other, cmpresult mask)
    {
        // __mmask64 => two integers vectors
        blendindex(oindex[0],other[0],(__mmask16)(mask >> 0));
        blendindex(oindex[1],other[1],(__mmask16)(mask >> 16));
        blendindex(oindex[2],other[2],(__mmask16)(mask >> 32));
        blendindex(oindex[3],other[3],(__mmask16)(mask >> 48));
    }

    simdtype x;
};

#endif


#endif

#ifdef __AVX2__

/**
 16 x uint8_t
 */
class simd_i8_32
{
public:
    typedef int8_t type;
    typedef __m256i simdtype;
    typedef simd_i32_8_a<4> indextype; 
    typedef NoGather gathermode;
    typedef simd_i8_32 self;
    typedef self cmpresult;

    enum { csize = 32 };
    
    inline simd_i8_32() {}
    inline simd_i8_32(type v) : x(_mm256_set1_epi8(v)) {}
    inline simd_i8_32(simdtype v) : x(v) {}
    inline void load(const type * ptr) { x = _mm256_loadu_ps((const float*)ptr); }
    inline void store(type * ptr) const { _mm256_storeu_ps((float*)ptr,x); }
    inline self max(self & y) const { return self(_mm256_max_epi8(x,y.x)); }
    inline cmpresult cmplt(self & y) const { return  cmpresult(_mm256_cmpgt_epi8(y.x,x)); }
    inline unsigned int size() const { return csize; }    

    void initincrement(type x)
    {
        type a[csize];
        for(int i = 0; i < csize; i++)
            a[i] = i*x;
        load(a);
    }

    /*
    inline type operator[] (unsigned int idx) const
    {
        type temp[csize];
        store(temp);
        return temp[idx];
    } 
    */   

    inline void blend(self & other, self mask)
    {
        x = _mm256_blendv_epi8(x,other.x,mask.x);
    }

    static inline void blendindex(indextype & oindex, indextype other, self mask);

    simdtype x;
};



/**
 32 x uint8_t
 */
class simd_u8_32
{
public:
    typedef uint8_t type;
    typedef __m256i simdtype;
    typedef simd_i32_8_a<4> indextype; 
    typedef NoGather gathermode;
    typedef simd_u8_32 self;
    typedef self cmpresult;
    
    enum { csize = 32 };
    
    inline simd_u8_32() {}
    inline simd_u8_32(type v) : x(_mm256_set1_epi8(v)) {}
    inline simd_u8_32(simdtype v) : x(v) {}
    inline void load(const type * ptr) { x = _mm256_loadu_ps((const float*)ptr); }
    inline void store(type * ptr) const { _mm256_storeu_ps((float*)ptr,x); }
    inline self max(self & y) const { return self(_mm256_max_epu8(x,y.x)); }
    inline cmpresult cmplt(self & y) const { return  cmpresult(_mm256_cmpgt_epu8(y.x,x)); }
    inline unsigned int size() const { return csize; }    

    void initincrement(type x)
    {
        type a[csize];
        for(int i = 0; i < csize; i++)
            a[i] = i*x;
        load(a);
    }

    /*
    inline type operator[] (unsigned int idx) const
    {
        type temp[csize];
        store(temp);
        return temp[idx];
    } 
    */   

    inline void blend(self & other, self mask)
    {
        x = _mm256_blendv_epi8(x,other.x,mask.x);
    }

   static inline void blendindex(indextype & oindex, indextype other, self mask);

    simdtype x;
};



// from 1 _mm256 8bit to 4 _mm256 32bit
inline void simd_u8_32::blendindex(indextype & oindex, indextype other, self mask)
{
    // mask = 8 items 32bit = A B C D E F G H each expresses the status of 4 variables
    // Objective: blendv: most sigificant bit: 31 63 .. 255
    //
    // ABCD can be taken from 128bit of the input, and in particular the A is just what needed
    __m128i cur = _mm256_castsi256_si128(mask.x);
    __m256i mask0 = mergelowhigh(
            _mm_cvtepi8_epi32(cur), // just the first 32bits
            _mm_cvtepi8_epi32(_mm_srli_si128(cur,4)) // the next 32 bits
            );
    __m256i mask1 = mergelowhigh(
            _mm_cvtepi8_epi32(_mm_srli_si128(cur,8)),
            _mm_cvtepi8_epi32(_mm_srli_si128(cur,12))
            );
    cur = gethigh(mask.x);

    __m256i mask2 = mergelowhigh(
            _mm_cvtepi8_epi32(cur), // just the first 32bits
            _mm_cvtepi8_epi32(_mm_srli_si128(cur,4))
            );
    __m256i mask3 = mergelowhigh(
            _mm_cvtepi8_epi32(_mm_srli_si128(cur,8)),
            _mm_cvtepi8_epi32(_mm_srli_si128(cur,12))
            );

    oindex.x[0] = _mm256_blendv_ps(oindex.x[0],other.x[0],mask0);
    oindex.x[1] = _mm256_blendv_ps(oindex.x[1],other.x[1],mask1);
    oindex.x[2] = _mm256_blendv_ps(oindex.x[2],other.x[2],mask2);
    oindex.x[3] = _mm256_blendv_ps(oindex.x[3],other.x[3],mask3);
}

// from 1 _mm256 8bit to 4 _mm256 32bit
inline void simd_i8_32::blendindex(indextype & oindex, indextype other, self mask)
{
    // mask = 8 items 32bit = A B C D E F G H each expresses the status of 4 variables
    // Objective: blendv: most sigificant bit: 31 63 .. 255
    //
    // ABCD can be taken from 128bit of the input, and in particular the A is just what needed
    __m128i cur = _mm256_castsi256_si128(mask.x);
    __m256i mask0 = mergelowhigh(
            _mm_cvtepi8_epi32(cur), // just the first 32bits
            _mm_cvtepi8_epi32(_mm_srli_si128(cur,4)) // the next 32 bits
            );
    __m256i mask1 = mergelowhigh(
            _mm_cvtepi8_epi32(_mm_srli_si128(cur,8)),
            _mm_cvtepi8_epi32(_mm_srli_si128(cur,12))
            );
    cur = gethigh(mask.x);

    __m256i mask2 = mergelowhigh(
            _mm_cvtepi8_epi32(cur), // just the first 32bits
            _mm_cvtepi8_epi32(_mm_srli_si128(cur,4))
            );
    __m256i mask3 = mergelowhigh(
            _mm_cvtepi8_epi32(_mm_srli_si128(cur,8)),
            _mm_cvtepi8_epi32(_mm_srli_si128(cur,12))
            );

    oindex.x[0] = _mm256_blendv_ps(oindex.x[0],other.x[0],mask0);
    oindex.x[1] = _mm256_blendv_ps(oindex.x[1],other.x[1],mask1);
    oindex.x[2] = _mm256_blendv_ps(oindex.x[2],other.x[2],mask2);
    oindex.x[3] = _mm256_blendv_ps(oindex.x[3],other.x[3],mask3);
}




#endif

// due to _mm_blendv_epi8
#ifdef __AVX2__

/**
 16 x uint8_t
 */
class simd_u8_16
{
public:
    typedef uint8_t type;
    typedef __m128i simdtype;
#ifdef WITH_AVX512VLBW
    typedef simd_i32_16 indextype;
#else
    typedef simd_i32_8_a<2> indextype;
#endif
    typedef NoGather gathermode;
    typedef simd_u8_16 self;
#ifdef WITH_AVX512VLBW
    typedef __mmask16 cmpresult;
#else
    typedef self cmpresult;
#endif
    enum { csize = 16 };
    
    inline simd_u8_16() {}
    inline simd_u8_16(type v) : x(_mm_set1_epi8(v)) {}
    inline simd_u8_16(simdtype v) : x(v) {}
    inline void load(const type * ptr) { x = _mm_loadu_si128((const simdtype*)ptr); }
    inline void store(type * ptr) const { _mm_storeu_si128((simdtype*)ptr,x); }
    inline self max(self & y) const { return self(_mm_max_epu8(x,y.x)); }
    
#ifdef WITH_AVX512VLBW
    inline cmpresult cmplt(self & y) const { return  _mm_mask_cmp_epi8_mask(x,y.x,1); }
#else
    inline cmpresult cmplt(self & y) const { return  cmpresult(_mm_cmplt_epu8(x,y.x)); }
#endif
    inline unsigned int size() const { return csize; }    

    void initincrement(type x)
    {
        type a[csize];
        for(int i = 0; i < csize; i++)
            a[i] = i*x;
        load(a);
    }

    /*
    inline type operator[] (unsigned int idx) const
    {
        type temp[csize];
        store(temp);
        return temp[idx];
    } 
    */   

    inline void blend(self & other, cmpresult mask)
    {
#if WITH_AVX512VLBW
        x = _mm_mask_blend_epi8(mask,x,other.x);
#else
        x = _mm_blendv_epi8(x,other.x,mask.x);        
#endif
    }

    inline void blendindex(indextype & oindex, indextype other, cmpresult mask);

    simdtype x;
};


/**
 16 x uint8_t
 */
class simd_i8_16
{
public:
    typedef int8_t type;
    typedef __m128i simdtype;
    typedef simd_i32_8_a<2> indextype;
    typedef NoGather gathermode;
    typedef simd_i8_16 self;
    typedef __m128i cmpresult;
    enum { csize = 16 };
    
    inline simd_i8_16() {}
    inline simd_i8_16(type v) : x(_mm_set1_epi8(v)) {}
    inline simd_i8_16(simdtype v) : x(v) {}
    inline void load(const type * ptr) { x = _mm_loadu_si128((const simdtype*)ptr); }
    inline void store(type * ptr) const { _mm_storeu_si128((simdtype*)ptr,x); }
    inline self max(self & y) const { return self(_mm_max_epi8(x,y.x)); }
    inline self cmplt(self & y) const { return  self(_mm_cmplt_epi8(x,y.x)); }
    inline unsigned int size() const { return csize; }    

    void initincrement(type x)
    {
        type a[csize];
        for(int i = 0; i < csize; i++)
            a[i] = i*x;
        load(a);
    }

    /*
    inline type operator[] (unsigned int idx) const
    {
        type temp[csize];
        store(temp);
        return temp[idx];
    } 
    */   

    inline void blend(self & other, self mask)
    {
        x = _mm_blendv_epi8(x,other.x,mask.x);
    }

    inline void blendindex(indextype & oindex, indextype other, self mask);

    simdtype x;
};
#endif




#ifdef __AVX2__
DECLAREOSTREAMT(simd_i8_16,"i8_16",int32_t)
DECLAREOSTREAMT(simd_u8_16,"u8_16",uint32_t)
DECLAREOSTREAMT(simd_i8_32,"i8_32",int32_t)
DECLAREOSTREAMT(simd_u8_32,"u8_32",uint32_t)

template <>
struct simdgen<int8_t>
{
    typedef simd_i8_32 type;
    typedef SimdMode simdmarker;
};

template <>
struct simdgen<uint8_t>
{
    typedef simd_u8_32 type;
    typedef SimdMode simdmarker;
};

template <int n>
struct simdgenn<int8_t,n>
{
    using type = typename std::conditional<n <= 16, simd_i8_16, simd_i8_32>::type;
    typedef SimdMode simdmarker;
};

template <int n>
struct simdgenn<uint8_t,n>
{
    using type = typename std::conditional<n <= 16, simd_u8_16, simd_u8_32>::type;
    typedef SimdMode simdmarker;
};


#else

/*
template <>
struct simdgen<int8_t>
{
    typedef simd_i8_16 type;
    typedef SimdMode simdmarker;
};

template <>
struct simdgen<uint8_t>
{
    typedef simd_u8_16 type;
    typedef SimdMode simdmarker;
};
*/

#endif
