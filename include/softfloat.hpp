/**
 * C++ Wrapper of SoftFloat 
 * Emanueel Ruffaldi 2017
 */
#pragma once
#include <cmath>
extern "C"
{
#include "softfloat.h"
}
#include <iostream>

union double_union
{
	float64_t i;
	double   f;
};

union float_union
{
	float32_t i;
	float     f;
};

inline float32_t float_to_f32(float f)
{
	float_union u;
	u.f = f;
	return u.i;
}

inline float64_t double_to_f64(double f)
{
	double_union u;
	u.f = f;
	return u.i;
}

inline float f32_to_float(float32_t i)
{
	float_union u;
	u.i = i;
	return u.f;
}

inline double f64_to_double(float64_t i)
{
	double_union u;
	u.i = i;
	return u.f;
}


struct softfloat16
{
	explicit softfloat16() {}
	explicit softfloat16(uint32_t f): value(ui32_to_f16(f)) {}
	explicit softfloat16(int32_t f): value(i32_to_f16(f)) {}
	explicit softfloat16(float f): value(f32_to_f16(float_to_f32(f))) {}
	explicit softfloat16(double f) : value(f64_to_f16(double_to_f64(f))) {}
	explicit softfloat16(float16_t f): value(f) {}
	explicit softfloat16(float32_t f): value(f32_to_f16(f)) {}

	operator float () const { return f32_to_float(f16_to_f32(value)); }
	operator double () const { return f64_to_double(f16_to_f64(value)); }
	operator float16_t() const { return value; }

	/// replace them with constants
	static softfloat16 two() { return softfloat16(2); };
	static softfloat16 one() { return softfloat16(1); };

	friend softfloat16 operator+ (softfloat16 a, softfloat16 b)  { return softfloat16(f16_add(a.value,b.value)); }
	friend softfloat16 operator- (softfloat16 a, softfloat16 b)  { return softfloat16(f16_sub(a.value,b.value)); }
	friend softfloat16 operator* (softfloat16 a, softfloat16 b)  { return softfloat16(f16_mul(a.value,b.value)); }
	friend softfloat16 operator/ (softfloat16 a, softfloat16 b)  { return softfloat16(f16_div(a.value,b.value)); }
	softfloat16 & operator+=(const softfloat16 &a) { softfloat16 x = *this + a; value = x.value; return *this; }
	softfloat16 & operator*=(const softfloat16 &a) { softfloat16 x = *this * a; value = x.value; return *this; }

	softfloat16 operator-() const {
		float16_t a = value;
		a.v ^= 0x8000;
		return softfloat16(a);
	}

	friend softfloat16 rem(softfloat16 a,softfloat16 b)  { return softfloat16(f16_rem(a.value,b.value)); }
	friend softfloat16 sqrt(softfloat16 a) { return softfloat16(f16_sqrt(a.value)); }
	friend softfloat16 mulAdd(softfloat16 a, softfloat16 b, softfloat16 c) { return softfloat16(f16_mulAdd(a.value,b.value,c.value)); }

	friend softfloat16 half(softfloat16 a) { return a/two(); }
	friend softfloat16 square(softfloat16 a) { return a*a; }
	friend softfloat16 inv(softfloat16 a) { return one()/a; }

	friend bool operator == (softfloat16 a, softfloat16 b)  { return (f16_eq(a.value,b.value)); }
	friend bool operator < (softfloat16 a, softfloat16 b)  { return (f16_lt(a.value,b.value)); }
	friend bool operator > (softfloat16 a, softfloat16 b)  { return (f16_le(b.value,a.value)); }
	friend bool operator >= (softfloat16 a, softfloat16 b)  { return (!f16_lt(b.value,a.value)); }
	friend bool operator <= (softfloat16 a, softfloat16 b)  { return (f16_le(a.value,b.value)); }

	friend std::ostream & operator << (std::ostream & ons, const softfloat16 & v)
	{
		ons << "f16(" << ((float)v) << ")";
		return ons;
	}

private:
	float16_t value;
};

struct softfloat32
{
	explicit softfloat32() {}
	explicit softfloat32(uint32_t f): value(ui32_to_f32(f)) {}
	explicit softfloat32(int32_t f): value(i32_to_f32(f)) {}
	explicit softfloat32(float f): value((float_to_f32(f))) {}
	explicit softfloat32(double f) : value(f64_to_f32(double_to_f64(f))) {}
	explicit softfloat32(float16_t f): value(f16_to_f32(f)) {}
	explicit softfloat32(float32_t f): value(f) {}

	operator float () const { return f32_to_float(value); }
	operator double () const { return f64_to_double(f32_to_f64(value)); }
	operator float32_t() const { return value; }
	static softfloat32 two() { return softfloat32(2); };
	static softfloat32 one() { return softfloat32(1); };

	friend softfloat32 operator+ (softfloat32 a, softfloat32 b)  { return softfloat32(f32_add(a.value,b.value)); }
	friend softfloat32 operator- (softfloat32 a, softfloat32 b)  { return softfloat32(f32_sub(a.value,b.value)); }
	friend softfloat32 operator* (softfloat32 a, softfloat32 b)  { return softfloat32(f32_mul(a.value,b.value)); }
	friend softfloat32 operator/ (softfloat32 a, softfloat32 b)  { return softfloat32(f32_div(a.value,b.value)); }

	softfloat32 & operator+=(const softfloat32 &a) { softfloat32 x = *this + a; value = x.value; return *this; }

	softfloat32 operator-() const {
		float32_t a = value;
		a.v ^= 0x80000000;
		return softfloat32(a);
	}

	friend softfloat32 half(softfloat32 a) { return a/two(); }
	friend softfloat32 square(softfloat32 a) { return a*a; }
	friend softfloat32 inv(softfloat32 a) { return one()/a; }

	friend softfloat32 rem(softfloat32 a,softfloat32 b) { return softfloat32(f32_rem(a.value,b.value)); }
	friend softfloat32 sqrt(softfloat32 a) { return softfloat32(f32_sqrt(a.value)); }
	friend softfloat32 mulAdd(softfloat32 a, softfloat32 b, softfloat32 c) { return softfloat32(f32_mulAdd(a.value,b.value,c.value)); }

	friend bool operator == (softfloat32 a, softfloat32 b)  { return (f32_eq(a.value,b.value)); }
	friend bool operator < (softfloat32 a, softfloat32 b)  { return (f32_lt(a.value,b.value)); }
	friend bool operator > (softfloat32 a, softfloat32 b)  { return (f32_le(b.value,a.value)); }
	friend bool operator >= (softfloat32 a, softfloat32 b)  { return (!f32_lt(b.value,a.value)); }
	friend bool operator <= (softfloat32 a, softfloat32 b)  { return (f32_le(a.value,b.value)); }

	friend std::ostream &operator << (std::ostream & ons, const softfloat32 & v)
	{
		ons << "f32(" << ((float)v) << ")";
		return ons;
	}

private:
	float32_t value;

};

