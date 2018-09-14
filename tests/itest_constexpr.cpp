#include "posit.h"

template <class P>
void pp(P po)
{
	std::cout << std::hex << po.v << " " << po << " == " << (float)po << std::endl;
}

int main()
{
	using X1=Posit<int32_t,32,1,uint64_t,true>;
	static_assert(X1::one() == X1::one()*X1::one(),"same 1==1*1");
	static_assert(X1::zero() == X1::one()-X1::one(),"same 0==1-1");
	static_assert(X1::one() == X1::one()/X1::one(),"same 1==1/1");
	static_assert(X1::one() == X1::one()+X1::zero(),"same 1==1+0");

	static_assert(X1::two() == X1::one()+X1::one(),"same 2==1+1");
	static_assert(X1::one() == X1::two().half(),"same 1==half(2)");
	static_assert(X1::onehalf() == X1::one()/X1::two(),"same 0.5==1/2");
	static_assert(X1::one().twice() == X1::two(),"same 2==1*1+1 using fma");
	static_assert(X1::two() == X1::one()*X1::one()+X1::one(),"same 2==1*1+1 using fma");

	static_assert(X1::PT::minexponent() < X1::PT::maxexponent() ,"exponent ordering");

	static_assert(X1(1) == X1::one(),"same 1==1");

	pp(X1(2));
	pp(X1::two());
	pp(X1::one()+X1::one());
	return 1;
}

void itest_constexpr()
{
	return main();
}