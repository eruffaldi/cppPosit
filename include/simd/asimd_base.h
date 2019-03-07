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
 */
#pragma once
#include <emmintrin.h>
#include <initializer_list>

#if defined(__AVX512F__) && defined(__AVX512VL__)
#define WITH_AVX512F
#endif

#if defined(__AVX512VL__)
#define WITH_AVX512VL
#endif

#ifdef __AVX2__  
#include <smmintrin.h>
#include <immintrin.h>

inline __m128 gethigh(__m256 in)
{   
    return _mm256_castsi256_si128(_mm256_permute2f128_ps(in,in,1));
}

inline __m256 mergelowhigh(__m128 low, __m128 high)
{
    return _mm256_insertf128_ps(_mm256_castps128_ps256(low),high,1); // first expand from 128 to 256 then insert
}

static inline __m256i
_mm256_cmpgt_epu8 (__m256i x, __m256i y)
{
    // Returns 0xFF where x > y:
    return _mm256_andnot_si256(
        _mm256_cmpeq_epi8(x, y),
        _mm256_cmpeq_epi8(_mm256_max_epu8(x, y), x)
    );
}

static inline __m256i
_mm256_cmpgt_epu16 (__m256i x, __m256i y)
{
    // Returns 0xFF where x > y:
    return _mm256_andnot_si256(
        _mm256_cmpeq_epi16(x, y),
        _mm256_cmpeq_epi16(_mm256_max_epu16(x, y), x)
    );
}
#

static inline __m256i
_mm256_cmpgt_epu32(__m256i x, __m256i y)
{
    // Returns 0xFF where x > y:
    return _mm256_andnot_si256(
        _mm256_cmpeq_epi32(x, y),
        _mm256_cmpeq_epi32(_mm256_max_epu32(x, y), x)
    );
}
#endif

// http://www.alfredklomp.com/programming/sse-intrinsics/
static inline __m128i
_mm_cmpgt_epu8 (__m128i x, __m128i y)
{
    // Returns 0xFF where x > y:
    return _mm_andnot_si128(
        _mm_cmpeq_epi8(x, y),
        _mm_cmpeq_epi8(_mm_max_epu8(x, y), x)
    );
}

static inline __m128i
_mm_cmple_epu16 (__m128i x, __m128i y)
{
    // Returns 0xFFFF where x <= y:
    return _mm_cmpeq_epi16(_mm_subs_epu16(x, y), _mm_setzero_si128());
}

static inline __m128i
_mm_cmpgt_epu16 (__m128i x, __m128i y)
{
    // Returns 0xFFFF where x > y:
    return _mm_andnot_si128(_mm_cmpeq_epi16(x, y), _mm_cmple_epu16(y, x));
}



static inline __m128i
_mm_cmplt_epu16 (__m128i x, __m128i y)
{
    // Returns 0xFFFF where x < y:
    return _mm_cmpgt_epu16(y, x);
}


static inline __m128i
_mm_cmplt_epu8 (__m128i x, __m128i y)
{
    // Returns 0xFF where x < y:
    return _mm_cmpgt_epu8(y, x);
}


class WithGather{};
class NoGather {};


/**
 * Classic load
 */
template <class simdwrap>
inline void loadstrided(simdwrap & r,const typename simdwrap::type * p, int stride,typename simdwrap::indextype pregather, NoGather)
{
    if(stride == 1)
        r.load(p);
    else
    {
        typename simdwrap::type data[simdwrap::csize];
        for(int i = 0; i < simdwrap::csize; i++, p += stride)
            data[i] = *p;
        r.load(data);                         
    }
}

/**
 * Use gather function with the pregather object
 */
template <class simdwrap>
inline void loadstrided(simdwrap & r,const typename simdwrap::type * p, int stride, typename simdwrap::indextype pregather, WithGather)
{
    if(stride == 1)
    {
        r.load(p);
    }
    else
    {
        r.gather(p,pregather);
    }
}

// dispatcher
template <class simdwrap>
inline void loadstrided(simdwrap & r,const typename simdwrap::type * p, int stride, typename simdwrap::indextype pregather)
{
    loadstrided(r,p,stride,pregather, typename simdwrap::gathermode());
}

template <class simdwrap>
inline void initgather(int stride, typename simdwrap::indextype  & x, WithGather)
{
    x.initincrement(stride*sizeof(typename simdwrap::type));
}

template <class simdwrap>
inline void initgather(int stride, typename simdwrap::indextype & x, NoGather)
{
}

template <class simdwrap>
inline void initgather(int stride, typename simdwrap::indextype & x)
{
    initgather<simdwrap>(stride,x, typename simdwrap::gathermode());
}


// this is the default storing 1 scalar, but it is better to use the Simd marker to use other implementations
template <class T>
class nonsimd
{
public:
    typedef T type;
    typedef T simdtype;
    typedef int indextype;
    typedef NoGather gathermode;
    typedef nonsimd self;
    enum { csize = 1 };
    
    inline nonsimd() {}
    inline nonsimd(type v1) : x(v1) {}
    inline void load(const type * ptr) { x = *ptr; };
    inline void store(type * ptr) const { *ptr = x; }
    
    inline nonsimd max(nonsimd & y) const { return nonsimd(x.x > y.x? x.x:y.x); }
    inline nonsimd cmplt(nonsimd & y) const { return  nonsimd(x.x < y.x? 1: 0); }
    
    inline unsigned int size() const { return csize; }    
    inline type operator[] (unsigned int idx) const
    {
        return x;
    }    

    void initincrement(type x)
    {
        type a[csize];
        for(int i = 0; i < csize; i++)
            a[i] = i*x;
        load(a);
    }

   static inline void blendindex(indextype & oindex, indextype other, self mask);

   
    simdtype x;
};

// array of wrapped types. 
template <class WT, int N>
class simd_x_a
{
public:
    typedef WT basetype;
    typedef typename WT::type type;
    typedef typename WT::simdtype simdtype;
    typedef std::array<typename WT::cmpresult,N> cmpresult; // combination of outputs
    typedef std::array<WT,N> contenttype; // flattened items
    typedef simd_x_a<typename WT::indextype,N> indextype; // so we can perform ops over that
    typedef simd_x_a self;
    enum { csize = WT::csize*N };
    enum { isize = WT::csize };  

    simd_x_a()
    {

    }

    simd_x_a(type K)
    {
        x.fill(K);
    }

    simd_x_a(std::initializer_list<type> values) : x(values)
    {
    }

    inline unsigned int size() const { return csize; } 

    inline self max(self & y) const 
    { 
        self r;
        for(int i = 0; i < N; i++)
            r.x[i] = x[i].max(y[i]);
        return r;
    }

    inline cmpresult cmplt(self & y) const 
    { 
        cmpresult r;
        for(int i = 0; i < N; i++)
            r[i] = x[i].cmplt(y[i]);
        return r;
    }

    // current max, and received index
    // returns this updated and new index
    inline indextype argmax_along_fit(indextype & curio, const indextype & curi, const type *p0) const 
    { 
        // unroll the loop by 2 for filling the ALUs
        const int Nn = N/2*2;
        for(int i = 0; i < Nn; i+=2)
        {
            basetype cur0,cur1;
            _mm_prefetch(p0+csize,1);
            cur0.load(p0);     
            p0 += csize;
            _mm_prefetch(p0+csize,1);
            cur1.load(p0);            
            auto cmp0 = x[i].cmplt(cur0); // curmax >= cur 
            auto cmp1 = x[i+1].cmplt(cur); // curmax >= cur 
           	x[i].blend(cur0,cmp0); // pick other if 1 at bit
           	x[i+1].blend(cur1,cmp1); // pick other if 1 at bit
            WT::blendindex(curio[i],curi[i],cmp0);    
            WT::blendindex(curio[i+1],curi[i+1],cmp1);               
        }
        if(N & 1)
        {
            basetype cur0;
            cur0.load(p0);     
            auto cmp0 = x[N-1].cmplt(cur0); // curmax >= cur 
           	x[i].blend(cur0,cmp0); // pick other if 1 at bit
           	WT::blendindex(curio[N-1],curi[N-1],cmp0);
        }        
    }

        
    /// load N pieces of T objects separated by stride
    inline void gather(const type * ptr, indextype idx, int stride) 
    {
        for(int i = 0; i < N; i++, ptr += stride)
            x[i].gather(ptr,idx[i]);
    }
    
    /// 
    inline void load(const type * ptr, int stride = isize) 
    {
        for(int i = 0; i < N; i++, ptr += stride)
            x[i].load(ptr);
    }

    inline void store(type * ptr) const 
    {
        for(int i = 0; i < N; i++, ptr += isize)
            x[i].store(ptr);
    }

    self & operator += (const self & o)
    {
        for(int i = 0; i < N; i++)
            x[i] += o[i];
        return *this;
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
        for(int i = 0; i < N; i++)
            WT::blendindex(oindex[i],other[i],mask[i]);    
    }

    inline void blend(self & other, cmpresult mask)
    {
        for(int i = 0; i < N; i++)
            x[i].blend(other[i],mask[i]);
    }

    inline basetype& operator [] (int i) { return x[i]; }

    inline const basetype& operator [] (int i) const { return x[i]; }


    contenttype x;
};

#ifdef NOMATLAB

template <class T, class OT>
std::ostream & printout(std::ostream & ons, const T & x, const char * prefix)
{
    typename T::type q[T::csize];
    x.store(q);
    ons << prefix << "[";
    for(int i = 0; i < T::csize; i++)
        ons << " " << (OT)q[i];
    ons << "]";
    return ons;

}

#define DECLAREOSTREAM(xtype,name) \
    std::ostream &operator << (std::ostream & ons, const xtype & x)\
    {\
        return printout<xtype,typename xtype::type>(ons,x,name);\
    }
#define DECLAREOSTREAMT(type,name,T) \
    std::ostream &operator << (std::ostream & ons, const type & x)\
    {\
        return printout<type,T>(ons,x,name);\
    }


#else

#define DECLAREOSTREAM(a,b)
#define DECLAREOSTREAMT(a,b,x)

#endif

DECLAREOSTREAM(nonsimd<double>,"nonsimd")
DECLAREOSTREAM(nonsimd<float>,"nonsimd")
DECLAREOSTREAM(nonsimd<int8_t>,"nonsimd")
DECLAREOSTREAM(nonsimd<int16_t>,"nonsimd")
DECLAREOSTREAM(nonsimd<int32_t>,"nonsimd")




/*
 * Below we setup the mapping between the type T and the corresponding wrapped SIMD object. We also expose a Simd tag for notifying the algorithms that for that type we are
   not using SIMD
 */


class SimdMode{};
class NoSimdMode{};

template <class T, int n>
struct simdgenn
{
    typedef nonsimd<T> type;
    typedef NoSimdMode simdmarker;
};

template <class T>
struct simdgen
{
    typedef nonsimd<T> type;
    typedef NoSimdMode simdmarker;
};

