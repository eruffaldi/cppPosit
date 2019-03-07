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

 * 128bit for ui32: 4  => self
 * 256bit for ui32: 8  => self
 * 512bit for ui32: 16 => self
 */
#pragma once
#include "asimd_base.h"

#ifdef __AVX2__

// DUE TO _mm_i32gather_epi32
class simd_i32_4
{
public:
    typedef int32_t type;
    typedef __m128i simdtype;
    typedef simd_i32_4 indextype;
    typedef WithGather gathermode;
    typedef simd_i32_4 self;
    typedef self cmpresult;
    enum { csize = 4 };
    
    inline simd_i32_4() {}
    inline simd_i32_4(type v) : x(_mm_set1_epi32(v)) {}
    inline simd_i32_4(type v1, type v2, type v3, type v4) : x(_mm_setr_epi32(v1,v2,v3,v4)) {}
    inline simd_i32_4(simdtype v) : x(v) {}
    inline void load(const type ptr[csize]) { x = _mm_loadu_si128((const simdtype*)ptr); }
    inline void store(type ptr[csize]) const { _mm_storeu_si128((simdtype*)ptr,x); }
    //inline void store(type * ptr) const { _mm_storeu_si128((simdtype*)ptr,x); }
    
    inline self max(self & y) const { return self(_mm_max_epi32(x,y.x)); }
    inline cmpresult cmplt(self & y) const { return  cmpresult(_mm_cmplt_epi32(x,y.x)); }
    inline void gather(const type * ptr, indextype idx) { x = _mm_i32gather_epi32(ptr, idx.x, 1); }
    
    inline unsigned int size() const { return csize; } 
    /*   
    inline type operator[] (unsigned int idx) const
    {
        // SLOPPY
        type temp[csize];
        store(temp);
        return temp[idx];
    }
    */

    self & operator += (const self & o)
    {
        x = _mm_add_epi32(x,o.x);
        return *this;
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
        x = _mm_blendv_ps(x,other.x,mask.x);
    }

   static inline void blendindex(indextype & oindex, indextype other, self mask);

    simdtype x;
};

class simd_i32_8
{
public:
    typedef int32_t type;
    typedef __m256i simdtype;
    typedef simd_i32_8 indextype;
    typedef WithGather gathermode;
    typedef simd_i32_8 self;
    typedef self cmpresult;
    enum { csize = 8 };
    
    inline simd_i32_8() {}
    inline simd_i32_8(type v) : x(_mm256_set1_epi32(v)) {}
    inline simd_i32_8(simdtype v) : x(v) {}
    inline void load(const type * ptr) { x = _mm256_loadu_si256((const simdtype*)ptr); }
    inline void store(type * ptr) const { _mm256_storeu_si256((simdtype*)ptr,x); }
    
    inline self max(self & y) const { return self(_mm256_max_epi32(x,y.x)); }
    inline cmpresult cmplt(self & y) const { return  cmpresult(_mm256_cmpgt_epi32(y.x,x)); }
    inline void gather(const type * ptr, indextype idx) { x = _mm256_i32gather_epi32(ptr, idx.x, 1); }
    
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

    self & operator += (const self & o)
    {
        x = _mm256_add_epi32(x,o.x);
        return *this;
    }    

    inline void blend(self & other, self mask)
    {
        x = _mm256_blendv_ps(x,other.x,mask.x);
    }

   static inline void blendindex(indextype & oindex, indextype other, self mask);

    simdtype x;
};


class simd_u32_8
{
public:
    typedef uint32_t type;
    typedef __m256i simdtype;
    typedef simd_i32_8 indextype;
    typedef WithGather gathermode;
    typedef simd_u32_8 self;
    typedef self cmpresult;
    enum { csize = 8 };
    
    inline simd_u32_8() {}
    inline simd_u32_8(type v) : x(_mm256_set1_epi32(v)) {}
    inline simd_u32_8(simdtype v) : x(v) {}
    inline void load(const type * ptr) { x = _mm256_loadu_si256((const simdtype*)ptr); }
    inline void store(type * ptr) const { _mm256_storeu_si256((simdtype*)ptr,x); }
    
    inline self max(self & y) const { return self(_mm256_max_epu32(x,y.x)); }
    inline cmpresult cmplt(self & y) const { return  cmpresult(_mm256_cmpgt_epu32(y.x,x)); }
    inline void gather(const type * ptr, indextype idx) { x = _mm256_i32gather_epi32(ptr, idx.x, 1); }
    
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

    self & operator += (const self & o)
    {
        x = _mm256_add_epi32(x,o.x);
        return *this;
    }    

    inline void blend(self & other, self mask)
    {
        x = _mm256_blendv_ps(x,other.x,mask.x);
    }

   static inline void blendindex(indextype & oindex, indextype other, self mask);

    simdtype x;
};

#ifdef WITH_AVX512VL

class simd_i32_16
{
public:
    typedef int32_t type;
    typedef __m512i simdtype;
    typedef simd_i32_16 indextype; // itself
    typedef WithGather gathermode;
    typedef simd_i32_16 self;
    typedef __mmask16 cmpresult;

    enum { csize = 16 };
    
    inline simd_i32_16() {}
    inline simd_i32_16(type v) : x(_mm512_set1_epi32(v)) {}
    inline simd_i32_16(simdtype v) : x(v) {}
    inline void load(const type * ptr) { x = _mm512_loadu_si512((const simdtype*)ptr); }
    inline void store(type * ptr) const { _mm512_storeu_si512((simdtype*)ptr,x); }
    
    inline self max(self & y) const { return self(_mm512_max_epi32(x,y.x)); }

    // note: _mm512_cmpgt_epi32_mask exists == ? == _mm512_cmp_epi32_mask
    inline cmpresult cmplt(self & y) const { return  _mm512_cmp_epi32_mask(x,y.x,1); }
    inline void gather(const type * ptr, indextype idx) { x = _mm512_i32gather_epi32(idx.x, ptr, 1); }
    
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

    self & operator += (const self & o)
    {
        x = _mm512_add_epi32(x,o.x);
        return *this;
    }    

    inline void blend(self & other, cmpresult mask)
    {
        x = _mm512_mask_blend_epi32(mask,x,other.x);
    }

    static inline void blendindex(indextype & oindex, indextype other, cmpresult mask)
    {
        oindex.x = _mm512_mask_blend_epi32(mask, oindex.x,other.x);        
    }

    simdtype x;
};


#endif



// like simd_x_a but directl
template <int N>
class simd_i32_8_a
{
public:
    typedef int32_t type;
    typedef __m256i basesimdtype;
    typedef __m256i simdtype[N];
    typedef simd_i32_8_a self;
    typedef self indextype;
    typedef self cmpresult;
    enum { csize = 8*N };
    enum { isize = 8 };
    
    simd_i32_8_a()
    {}

    simd_i32_8_a(int K)
    {
        x[0] = _mm256_set1_epi32(K);
        for(int i = 1; i < N; i++)
            x[i] = x[0];
    }

    inline void load(const type * ptr) 
    {
        for(int i = 0; i < N; i++, ptr += isize)
            x[i] = _mm256_loadu_si256((const __m256i*)ptr);
    }

    inline void store(type * ptr) const 
    {
        for(int i = 0; i < N; i++, ptr += isize)
            _mm256_storeu_si256((basesimdtype*)ptr,x[i]);
    }

    simd_i32_8_a & operator += (const simd_i32_8_a & o)
    {
        for(int i = 0; i < N; i++)
            x[i] = _mm256_add_epi32(x[i],o.x[i]);
        return *this;
    }    

    void initincrement(type x)
    {
        type a[csize];
        for(int i = 0; i < csize; i++)
            a[i] = i*x;
        load(a);
    }


    simdtype x;
};

inline void simd_i32_8::blendindex(indextype & oindex, indextype other, self mask)
{
    oindex.x = _mm256_blendv_ps(oindex.x,other.x,mask.x);
}

inline void simd_u32_8::blendindex(indextype & oindex, indextype other, self mask)
{
    oindex.x = _mm256_blendv_ps(oindex.x,other.x,mask.x);
}


inline void simd_i32_4::blendindex(indextype & oindex, indextype other, self mask)
{
    oindex.x = _mm_blendv_ps(oindex.x,other.x,mask.x);
}


DECLAREOSTREAM(simd_i32_4,"i32_4")

#ifdef __AVX2__

DECLAREOSTREAM(simd_i32_8,"i32_8")

#ifdef NOMATLAB
template <int N>
std::ostream &operator << (std::ostream & ons, const simd_i32_8_a<N> & x)\
{\
    return printout< simd_i32_8_a<N>, int32_t>(ons,x,"array");\
}
#endif

#endif

#ifdef WITH_AVX512VL
DECLAREOSTREAM(simd_i32_16,"i32_16")
#endif

#endif


#ifdef __AVX2__

#ifdef WITH_AVX512VL

// picks using n: favor larger
template <int n>
struct simdgenn<int32_t,n>
{
    using type = typename std::conditional<n <= 4, simd_i32_4,typename std::conditional<n <= 8, simd_i32_8, simd_i32_16>::type >::type;
    typedef SimdMode simdmarker;
};

template <int n>
struct simdgenn<uint32_t,n>
{
    using type = typename std::conditional<n <= 4, simd_u32_4,typename std::conditional<n <= 8, simd_u32_8, simd_u32_16>::type >::type;
    typedef SimdMode simdmarker;
};

template <>
struct simdgen<int32_t>
{   
    typedef simd_i32_16 type;
    typedef SimdMode simdmarker;
};
#else

// picks using n: favor larger
template <int n>
struct simdgenn<int32_t,n>
{
    using type = typename std::conditional<n <= 4, simd_i32_4, simd_i32_8>::type;
    typedef SimdMode simdmarker;
};

template <int n>
struct simdgenn<uint32_t,n>
{
    using type = simd_u32_8; // u32_4 not yet implemented
    typedef SimdMode simdmarker;
};

template <>
struct simdgen<int32_t>
{
    typedef simd_i32_8 type;
    typedef SimdMode simdmarker;
};

template <>
struct simdgen<uint32_t>
{
    typedef simd_u32_8 type;
    typedef SimdMode simdmarker;
};
#endif

#endif

