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

 * 128bit for f: 4  => int32
 * 256bit for f: 8  => int32
 * 512bit for f: 16 => int32 
 */
#pragma once
#include "asimd_32.h"

// DUE TO _mm_i32gather_epi32
#ifdef __AVX2__

class simd_f_4
{
public:
    typedef float type;
    typedef __m128 simdtype;
    typedef simd_i32_4 indextype; // for indices
    typedef WithGather gathermode;
    typedef simd_f_4 self;
    typedef self cmpresult;
    enum { csize = 4 };
    
    inline simd_f_4() {}
    inline simd_f_4(type v) : x(_mm_set1_ps(v)) {}
    inline simd_f_4(simdtype y): x(y) {}
    inline simd_f_4(type v1, type v2, type v3, type v4) : x(_mm_setr_ps(v1,v2,v3,v4)) {}
        inline void load(const type * ptr) { x = _mm_loadu_ps((const type*)ptr); }
    inline void store(type * ptr) const { _mm_storeu_ps((type*)ptr,x); }
    
    inline self max(self & y) const { return self(_mm_max_ps(x,y.x)); }
    inline cmpresult cmplt(self & y) const { return  cmpresult(_mm_cmplt_ps(x,y.x)); }
    inline void gather(const type * ptr, indextype idx) { x = _mm_i32gather_ps(ptr, idx.x, 1); }

    inline unsigned int size() const { return csize; }    

    /*
    inline type operator[] (unsigned int idx) const
    {
        type temp[csize];
        store(temp);
        return temp[idx];
    } 
    */   

    void initincrement(type x)
    {
        type a[csize];
        for(int i = 0; i < csize; i++)
            a[i] = i*x;
        load(a);
    }

    inline void blend(self & other, self mask)
    {
        x = _mm_blendv_ps(x,other.x,mask.x);
    }

    static inline void blendindex(indextype & oindex, indextype other, self mask);

    simdtype x;
};


inline void simd_f_4::blendindex(indextype & oindex, indextype other, self mask)
{
    oindex.x = _mm_blendv_ps(oindex.x,other.x,mask.x);
}
#endif

#ifdef __AVX2__

class simd_f_8
{
public:
    typedef float type;
    typedef __m256 simdtype;
    typedef simd_i32_8 indextype;
    typedef WithGather gathermode;
    typedef simd_f_8 self;
    typedef self cmpresult;
    enum { csize = 8 };
    
    inline simd_f_8() {}
    inline simd_f_8(type v) : x(_mm256_set1_ps(v)) {}
    inline simd_f_8(simdtype y): x(y) {}
    //inline simd_f_8(type v1, type v2, type v3, type v4) : x(_mm256_setr_ps(v1,v2,v3,v4)) {}
    inline void load(const type * ptr) { x = _mm256_loadu_ps((const type*)ptr); }
    inline void store(type * ptr) const { _mm256_storeu_ps((type*)ptr,x); }
    
    inline self max(simd_f_8 & y) const { return self(_mm256_max_ps(x,y.x)); }
    inline cmpresult cmplt(self & y) const { return  cmpresult(_mm256_cmp_ps(x,y.x, 1 /*_CMP_LT_OS*/)); }
    inline void gather(const type * ptr, indextype idx) { x = _mm256_i32gather_ps(ptr, idx.x, 1); }

    inline unsigned int size() const { return csize; }    
    inline type operator[] (unsigned int idx) const
    {
        type temp[csize];
        store(temp);
        return temp[idx];
    }    

    void initincrement(type x)
    {
        type a[csize];
        for(int i = 0; i < csize; i++)
            a[i] = i*x;
        load(a);
    }

    inline void blend(self & other, self mask)
    {
        x = _mm256_blendv_ps(x,other.x,mask.x);
    }

    static inline void blendindex(indextype & oindex, indextype other, self mask);

    simdtype x;
};


inline void simd_f_8::blendindex(indextype & oindex, indextype other, self mask)
{
    oindex.x = _mm256_blendv_ps(oindex.x,other.x,mask.x);
}


#endif

#ifdef WITH_AVX512F
class simd_f_16
{
public:
    typedef float type;
    typedef __m512 simdtype;
    typedef simd_i32_16 indextype;
    typedef WithGather gathermode;
    typedef simd_f_16 self;
    typedef __mmask16 cmpresult;
    enum { csize = 16 };


    inline simd_f_16() {}
    inline simd_f_16(type v) : x(_mm512_set1_ps(v)) {}
    inline simd_f_16(simdtype y): x(y) {}
    //inline simd_f_8(type v1, type v2, type v3, type v4) : x(_mm256_setr_ps(v1,v2,v3,v4)) {}
    inline void load(const type * ptr) { x = _mm512_loadu_ps((const type*)ptr); }
    inline void store(type * ptr) const { _mm512_storeu_ps((type*)ptr,x); }
    
    inline self max(self & y) const { return self(_mm512_max_ps(x,y.x)); }
    inline cmpresult cmplt(self & y) const { return  (_mm512_cmp_ps_mask(x,y.x, 1 /*_CMP_LT_OS*/)); }
    inline void gather(const type * ptr, indextype idx) { x = _mm512_i32gather_ps(idx.x,ptr, 1); }

    inline unsigned int size() const { return csize; }    
    inline type operator[] (unsigned int idx) const
    {
        type temp[csize];
        store(temp);
        return temp[idx];
    }    

    void initincrement(type x)
    {
        type a[csize];
        for(int i = 0; i < csize; i++)
            a[i] = i*x;
        load(a);
    }

    inline void blend(self & other, cmpresult mask)
    {
        x = _mm512_mask_blend_ps(mask,x,other.x);
    }

    static inline void blendindex(indextype & oindex, indextype other, cmpresult mask)
    {
        oindex.x = _mm512_mask_blend_epi32(mask, oindex.x,other.x);
    }

    simdtype x;
};
#endif

#ifdef __AVX2__
DECLAREOSTREAM(simd_f_4,"f_4")
DECLAREOSTREAM(simd_f_8,"f_8")
#endif

#ifdef WITH_AVX512F
DECLAREOSTREAM(simd_f_16,"f_16")
#endif


#ifdef WITH_AVX512F

template <int n>
struct simdgenn<float,n>
{
    using type = typename std::conditional<n <= 4, simd_f_4,typename std::conditional<n <= 8, simd_f_8, simd_f_16>::type >::type;
    typedef SimdMode simdmarker;
};


template <>
struct simdgen<float>
{
    typedef simd_f_16 type;
    typedef SimdMode simdmarker;
};
#else
#ifdef __AVX2__
template <int n>
struct simdgenn<float,n>
{
    using type = typename std::conditional<n <= 4, simd_f_4, simd_f_8>::type;
    typedef SimdMode simdmarker;
};

template <>
struct simdgen<float>
{
    typedef simd_f_8 type;
    typedef SimdMode simdmarker;
};
#endif
#endif



