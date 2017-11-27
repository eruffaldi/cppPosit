#include "posit12.hpp"
#include <iostream>

template <class T>
struct pof_
{
public:
	pof_(T p): posit(p) {}

	friend std::ostream & operator << (std::ostream & ons, const pof_ & x)
	{
		ons << "(" << " unsigned " << x.posit.uu() << " signed " << (int)x.posit.v << " float " << (float)x.posit<<")";
		return ons;
	}

	T posit;
};

template<class T>
pof_<T> pof(T x)
{
	return pof_<T>(x);
}

int main(int argc, char const *argv[])
{
	posit12 a(20.0);
	posit12 b(-10.0);
	std::cout << "a:   " << pof(a) <<   "\nb:   " <<  pof(b) << std::endl;
	std::cout << "a+b: " << pof(a+b) << "\na-b: " << pof(a-b) << "\na*b: " << pof(a*b)  << "\na/b: " << pof(a/b) << "\ninv a:"  << pof(a.inv()) << "\ninv b:" << pof(b.inv()) << std::endl;
	return 0;
}