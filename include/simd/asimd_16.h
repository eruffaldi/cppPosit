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

 * Copyright Emanuele Ruffaldi (2017) SSSA
 *
 * 128bit for ui16: 8  => requires simd_i32_8 (AVX 256bit)
 * 256bit for ui16: 16 => requires simd_i32_16 (AVX512 512bit or array for AVX2)
 * 512bit for ui16: 32 => requires simd_i32_32 (array)
 */
#pragma once
#include "asimd_32.h"

// NOT implemented simd_u16_4 for < AVX2 using simd
#ifdef __AVX2__ 
class simd_u16_8
{
public:
    typedef uint16_t type;
    typedef __m128i simdtype;    
    typedef simd_i32_8 indextype; // in general simdgenn<int32,8>
    typedef NoGather gathermode;
    typedef simd_u16_8 self;
    typedef self cmpresult;
    enum { csize = 8 };
    
    inline simd_u16_8() {}
    inline simd_u16_8(type v) : x(_mm_set1_epi16(v)) {}
    inline simd_u16_8(simdtype v) : x(v) {}
    inline void load(const type * ptr) { x = _mm_loadu_si128((const simdtype*)ptr); }
    inline void store(type * ptr) const { _mm_storeu_si128((simdtype*)ptr,x); }
    inline self max(self & y) const { return self(_mm_max_epu16(x,y.x)); }
    inline cmpresult cmplt(self & y) const { return  cmpresult(_mm_cmplt_epu16(x,y.x)); }
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

    static inline void blendindex(indextype & oindex, indextype other, self mask);

    simdtype x;
};



class simd_i16_8
{
public:
    typedef int16_t type;
    typedef __m128i simdtype;
    typedef simd_i32_8 indextype;
    typedef NoGather gathermode;
    typedef simd_i16_8 self;
    typedef self cmpresult;
    enum { csize = 8 };
    
    inline simd_i16_8() {}
    inline simd_i16_8(type v) : x(_mm_set1_epi16(v)) {}
    inline simd_i16_8(simdtype v) : x(v) {}
    inline void load(const type * ptr) { x = _mm_loadu_si128((const simdtype*)ptr); }
    inline void store(type * ptr) const { _mm_storeu_si128((simdtype*)ptr,x); }
    inline self max(self & y) const { return self(_mm_max_epi16(x,y.x)); }
    inline cmpresult cmplt(self & y) const { return  cmpresult(_mm_cmplt_epi16(x,y.x)); }
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

    static inline void blendindex(indextype & oindex, indextype other, self mask);

    simdtype x;
};

#endif

#ifdef __AVX2__

class simd_i16_16
{
public:
    typedef int16_t type;
    typedef __m256i simdtype;
#ifdef WITH_AVX512VLBW
    typedef simd_i32_16 indextype;
#else
    typedef simd_i32_8_a<2> indextype;
#endif
    typedef NoGather gathermode;
    typedef simd_i16_16 self;
#ifdef WITH_AVX512VLBW
    typedef __mask16 cmpresult;
#else
    typedef self cmpresult;
#endif
    enum { csize = 16 };
    
    inline simd_i16_16() {}
    inline simd_i16_16(type v) : x(_mm256_set1_epi16(v)) {}
    inline simd_i16_16(simdtype v) : x(v) {}
    inline void load(const type * ptr) { x = _mm256_loadu_si256((const simdtype*)ptr); }
    inline void store(type * ptr) const { _mm256_storeu_si256((simdtype*)ptr,x); }
    inline self max(self & y) const { return self(_mm256_max_epi16(x,y.x)); }
    
#ifdef WITH_AVX512VLBW
    inline cmpresult cmplt(self & y) const { return _mm256_cmp_epi16_mask(x,y.x,1); }
#else    
    inline cmpresult cmplt(self & y) const { return  cmpresult(_mm256_cmpgt_epi16(y.x,x)); }
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
#ifdef WITH_AVX512VLBW        
        x = _mm256_mask_blend_epi16(x,other.x,mask);
#else        
        x = _mm256_blendv_epi8(x,other.x,mask.x);
#endif   
    }

    static inline void blendindex(indextype & oindex, indextype other, cmpresult mask);

    simdtype x;

};



// from 1 _mm256 8bit to 4 _mm256 32bit
inline void simd_i16_16::blendindex(indextype & oindex, indextype other, cmpresult mask)
{
#ifdef WITH_AVX512VLBW
    simd_i32_16::blendindex(oindex,other,mask);
#else
    // mask = 8 items 32bit = A B C D E F G H each expresses the status of 2 input variables
    //
    __m128i cur = _mm256_castsi256_si128(mask.x);
    __m256i mask0 = mergelowhigh(
            _mm_cvtepi16_epi32(cur), // takes the first 4 16bit (64bit) and expands them into 4 32bit (128bit)
            _mm_cvtepi16_epi32(_mm_srli_si128(cur,6)) // the next 64bit
            );
    cur = gethigh(mask.x); // high part

    __m256i mask1 = mergelowhigh(
            _mm_cvtepi16_epi32(cur), // takes the first 4 16bit (64bit) and expands them into 4 32bit (128bit)
            _mm_cvtepi16_epi32(_mm_srli_si128(cur,6)) // the next 64bit
            );

    oindex.x[0] = _mm256_blendv_ps(oindex.x[0],other.x[0],mask0);
    oindex.x[1] = _mm256_blendv_ps(oindex.x[1],other.x[1],mask1);
#endif    
}


class simd_u16_16
{
public:
    typedef uint16_t type;
    typedef __m256i simdtype;
#ifdef WITH_AVX512VLBW
    typedef simd_i32_16 indextype;
#else
    typedef simd_i32_8_a<2> indextype;
#endif
    typedef NoGather gathermode;
    typedef simd_u16_16 self;
#ifdef WITH_AVX512VLBW
    typedef __mask16 cmpresult;
#else
    typedef self cmpresult;
#endif
    enum { csize = 16 };
    
    inline simd_u16_16() {}
    inline simd_u16_16(type v) : x(_mm256_set1_epi16(v)) {}
    inline simd_u16_16(simdtype v) : x(v) {}
    inline void load(const type * ptr) { x = _mm256_loadu_si256((const simdtype*)ptr); }
    inline void store(type * ptr) const { _mm256_storeu_si256((simdtype*)ptr,x); }
    inline self max(self & y) const { return self(_mm256_max_epu16(x,y.x)); }

#ifdef WITH_AVX512VLBW
    inline cmpresult cmplt(self & y) const { return _mm256_cmpgt_epu16_mask(x,y.x,1); }
#else    
    inline cmpresult cmplt(self & y) const { return  self(_mm256_cmpgt_epu16(y.x,x)); }
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
#ifdef WITH_AVX512VLBW        
        x = _mm256_mask_blend_epi16(x,other.x,mask);
#else        
        x = _mm256_blendv_epi8(x,other.x,mask.x);
#endif        
    }

    static inline void blendindex(indextype & oindex, indextype other, cmpresult mask);

    simdtype x;

};



// from 1 _mm256 8bit to 4 _mm256 32bit
inline void simd_u16_16::blendindex(indextype & oindex, indextype other, cmpresult mask)
{
#ifdef WITH_AVX512VLBW
    simd_i32_16::blendindex(oindex,other,mask);
#else
    // mask = 8 items 32bit = A B C D E F G H each expresses the status of 2 input variables
    //
    __m128i cur = _mm256_castsi256_si128(mask.x);
    __m256i mask0 = mergelowhigh(
            _mm_cvtepi16_epi32(cur), // takes the first 4 16bit (64bit) and expands them into 4 32bit (128bit)
            _mm_cvtepi16_epi32(_mm_srli_si128(cur,6)) // the next 64bit
            );
    cur = gethigh(mask.x); // high part

    __m256i mask1 = mergelowhigh(
            _mm_cvtepi16_epi32(cur), // takes the first 4 16bit (64bit) and expands them into 4 32bit (128bit)
            _mm_cvtepi16_epi32(_mm_srli_si128(cur,6)) // the next 64bit
            );

    oindex.x[0] = _mm256_blendv_ps(oindex.x[0],other.x[0],mask0);
    oindex.x[1] = _mm256_blendv_ps(oindex.x[1],other.x[1],mask1);
#endif
}

#endif



#ifdef __AVX2__
DECLAREOSTREAM(simd_i16_8,"i16_8")
DECLAREOSTREAM(simd_i16_16,"i16_16")

template <int n>
struct simdgenn<int16_t,n>
{
    using type = typename std::conditional<n <= 8, simd_i16_8, simd_i16_16>::type;
    typedef SimdMode simdmarker;
};

template <int n>
struct simdgenn<uint16_t,n>
{
    using type = typename std::conditional<n <= 8, simd_u16_8, simd_u16_16>::type;
    typedef SimdMode simdmarker;
};


template <>
struct simdgen<int16_t>
{
    typedef simd_i16_16 type;
    typedef SimdMode simdmarker;
};

template <>
struct simdgen<uint16_t>
{
    typedef simd_u16_16 type;
    typedef SimdMode simdmarker;
};

#else


#endif



