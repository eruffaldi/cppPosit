#pragma once

template <class T, int N, int F>
struct fixedtrait
{
    static_assert(sizeof(T)*8 <= N,"fixedtrait holding type is too small");
    static_assert(N > 0,"fixedtrait total bits should be positive");
    static_assert(F <= N && F >= 0,"fraction bits should be less than N and not negative");
    static_assert(std::is_integral<T>::value && std::is_signed<T>::value,"only for signed integrals");
	using value_t = T;
	static constexpr int totalbits = N;
	static constexpr int fraction_bits = F;
};

template <class T>
struct fixedtrait<T,sizeof(T)*8,0>
{
    static_assert(std::is_integral<T>::value && std::is_signed<T>::value,"only for signed integrals");
	using value_t = T;
	static constexpr int totalbits = sizeof(T)*8;
	static constexpr int fraction_bits = 0;
};