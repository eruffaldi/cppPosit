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

 * 128bit for f: 2  => int32 simd_i32_4 low => probably better using regular SSE
 * 256bit for f: 4  => int32 simd_i32_4 
 * 512bit for f: 8  => int32 simd_i32_8
 */
#pragma once
#include "asimd_base.h"

// TODO: replace AVX2 _mm_i32gather_pd with something when AVX2 is missing
#ifdef __AVX2__
class simd_d_2
{
public:
    typedef double type;
    typedef __m128d simdtype;
    typedef simd_i32_4 indextype;
    typedef WithGather gathermode;
    typedef simd_d_2 self;
    typedef self cmpresult;
    enum { csize = 2 };
    
    inline simd_d_2() {}
    inline simd_d_2(type v) : x(_mm_set1_pd(v)) {}
    inline simd_d_2(simdtype d) : x(d) {}
    inline simd_d_2(type v1, type v2) : x(_mm_setr_pd(v1,v2)) {}
    
    inline void load(const type * ptr) { x = _mm_loadu_pd((const type*)ptr); }
    inline void store(type * ptr) const { _mm_storeu_pd((type*)ptr,x); }
    
    inline self max(self & y) const { return self(_mm_max_pd(x,y.x)); }
    inline cmpresult cmplt(self & y) const { return  cmpresult(_mm_cmplt_pd(x,y.x)); }
    inline void gather(const type * ptr, indextype idx) { x = _mm_i32gather_pd(ptr, idx.x, 1); }
    
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
        x = _mm_blendv_pd(x,other.x,mask.x);
    }

    static inline void blendindex(indextype & oindex, indextype other, self mask)
    {
        // compare generates: AABB with each letter 32bit
        // shuffle: AABB => ABAB
        // use low part as in simd_i32_4
        //
        // mm_shuffle_epi32: (0)|(2)<<2|(0)<<4|(2)<<6
        oindex.x = _mm_blendv_ps(oindex.x,other.x,_mm_shuffle_epi32(mask.x,0x44));
    }


    simdtype x;
};

#endif


#ifdef __AVX2__

class simd_d_4
{
public:
    typedef double type;
    typedef __m256d simdtype;
    typedef simd_i32_4 indextype;
    typedef WithGather gathermode;
    typedef simd_d_4 self;
    typedef self cmpresult;
    enum { csize = 4 };
    
    inline simd_d_4() {}
    inline simd_d_4(type v) : x(_mm256_set1_pd(v)) {}
    inline simd_d_4(simdtype d) : x(d) {}
    inline void load(const type * ptr) { x = _mm256_loadu_pd((const type*)ptr); }
    inline void store(type * ptr) const { _mm256_storeu_pd((type*)ptr,x); }
    
    inline self max(self & y) const { return self(_mm256_max_pd(x,y.x)); }
    inline cmpresult cmplt(simd_d_4 & y) const { return  cmpresult(_mm256_cmp_pd(x,y.x, 1 /* LT_OS */)); }
    
    inline void gather(const type * ptr, indextype idx) { x = _mm256_i32gather_pd(ptr, idx.x, 1); }
    inline unsigned int size() const { return csize; }    

    inline void blend(self & other, self mask)
    {
        x = _mm256_blendv_pd(x,other.x,mask.x);
    }

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

    static inline void blendindex(indextype & oindex, indextype other, self mask)
    {
        // input: AA BB CC DD with each letter 32bit as of cmp
        // AA BB CC DD => ABCD ABCD (0 2 4 6 0 2 4 6) work in mm128
        // permute and shuffle work by 128bit lane =>  _mm256_permutevar8x32_epi32 is generic and uses indices as bits
        // then _mm256_castsi256_si128 to take lower part
        __m128i newmask = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(mask.x,_mm256_setr_epi32(0,2,4,6,0,2,4,6)));
        oindex.x = _mm_blendv_ps(oindex.x,other.x,newmask);
    }

    simdtype x;
};

#endif


#ifdef WITH_AVX512F

class simd_d_8
{
public:
    typedef double type;
    typedef __m512d simdtype;
    typedef simd_i32_8 indextype;
    typedef WithGather gathermode;
    typedef simd_d_8 self;
    typedef __mmask8 cmpresult;
    enum { csize = 8 };
    
    inline simd_d_8() {}
    inline simd_d_8(type v) : x(_mm512_set1_pd(v)) {}
    inline simd_d_8(simdtype d) : x(d) {}
    inline void load(const type * ptr) { x = _mm512_loadu_pd((const type*)ptr); }
    inline void store(type * ptr) const { _mm512_storeu_pd((type*)ptr,x); }
    
    inline self max(self & y) const { return self(_mm512_max_pd(x,y.x)); }
    inline cmpresult cmplt(self & y) const { return _mm512_cmp_pd_mask(x,y.x, 1 /* LT_OS */); }
    
    inline void gather(const type * ptr, indextype idx) { x = _mm512_i32gather_pd(idx.x,ptr, 1); }
    inline unsigned int size() const { return csize; }    

    inline void blend(self & other, cmpresult mask)
    {        
        x = _mm512_mask_blend_pd(mask, x, other.x);
    }
    
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

    static inline void blendindex(indextype & oindex, indextype other, cmpresult mask)
    {       
        // much much easier because __mmask8 is trivial and we can work as in simd_i32_8
        oindex.x = _mm256_mask_blend_epi32(mask, oindex.x,other.x);
    }

    simdtype x;
};

#endif


#ifdef __AVX2__

/**
 * Array
 */
template <int N>
class simd_d_4_a
{
public:
    typedef double type;
    typedef __m256d basesimdtype;
    typedef __m256d simdtype[N];
    typedef simd_i32_8_a<N> indextype; // could be N/2
    typedef simd_d_4_a self;
    enum { csize = 4*N };
    enum { isize = 4 };
    typedef WithGather gathermode;

    simd_d_4_a()
    {}

    simd_d_4_a(int K)
    {
        x[0] = _mm256_set1_pd(K);
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
    inline unsigned int size() const { return csize; }    

    self & operator += (const self & o)
    {
        for(int i = 0; i < N; i++)
            x[i] = _mm256_add_pd(x[i],o.x[i]);
        return *this;
    }    

    void initincrement(type x)
    {
        type a[csize];
        for(int i = 0; i < csize; i++)
            a[i] = i*x;
        load(a);
    }

    inline self max(self & y) const 
    { 
        simd_d_4_a r;
        for(int i = 0; i < N; i++)  
            r.x[i] = simd_d_4(_mm256_max_pd(x[i],y.x[i]));
    }
    
    inline self cmplt(self & y) const { 
        simd_d_4_a r;
        for(int i = 0; i < N; i++)  
            r.x[i] = simd_d_4(_mm256_cmp_pd(x[i],y.x[i], 1 /* LT_OS */));
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
        for(int i = 0; i < N; i++)
            x[i] = _mm256_blendv_pd(x[i],other.x[i],mask.x[i]);
    }

    static void blendindex(indextype & oindex, indextype other, self mask)
    {
        __m256 mm = _mm256_setr_epi32(0,2,4,6,0,2,4,6);
        
        for(int i = 0; i < csize; i++)
        {
            __m128i newmask = _mm256_castsi256_si128(_mm256_permutevar8x32_epi32(mask.x[i],mm));
            oindex.x[i] = _mm_blendv_ps(oindex.x[i],other.x[i],newmask);
        }
    }
    
    
    inline void gather(const type * ptr, indextype idx) 
    { 
        simd_d_4 tmp;
        for(int i = 0; i < N; i++, ptr += isize)
        {
            tmp.gather(ptr,idx.x[i]);
            x[i] = tmp.x;
        }
    }

    simdtype x;
};
#endif



#ifdef __AVX2__
DECLAREOSTREAM(simd_d_2,"d_2")
DECLAREOSTREAM(simd_d_4,"d_4")
#endif

#ifdef WITH_AVX512F
DECLAREOSTREAM(simd_d_8,"d_8")
#endif


#ifdef __AVX2__
#ifdef WITH_AVX512F

template <int n>
struct simdgenn<double,n>
{
    using type = typename std::conditional<n <= 2, simd_d_2, typename std::conditional<n <= 4, simd_d_4, simd_d_8>::type >::type;
    typedef SimdMode simdmarker;
};

template <>
struct simdgen<double>
{
    //typedef simd_d_4_a<2> type;
    typedef simd_d_8 type;
    typedef SimdMode simdmarker;
};
#else

// picks using n: favor larger
template <int n>
struct simdgenn<double,n>
{
    using type = typename std::conditional<n <= 2, simd_d_2, simd_d_4>::type;
    typedef SimdMode simdmarker;
};


template <>
struct simdgen<double>
{
    //typedef simd_d_4_a<2> type;
    typedef simd_d_4 type;
    typedef SimdMode simdmarker;
};
 

#endif

#endif

