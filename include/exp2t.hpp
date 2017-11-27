#pragma once

template <class T>
struct exp2t_
{
	T operator() (T a)
	{
		return (T)(exp2((double)a));
	}
};



template <class T>
T exp2t(T a)
{
	return exp2t_<T>()(a);
}