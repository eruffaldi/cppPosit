#include "posit.h"

void fx()
{
	using X1=Posit<int32_t,32,2,uint32_t,false>;
	static_assert(X1::one() == X1::one()*X1::one(),"same 1==1*1");
	static_assert(X1::zero() == X1::one()-X1::one(),"same 0==1-1");
	static_assert(X1::one() == X1::one()/X1::one(),"same 1==1/1");

	static_assert(X1::one() == X1::two().half(),"same 1==half(2)");

	static_assert(X1::two() == X1::one()+X1::one(),"same 2==1+1");
	static_assert(X1::two() == X1::one()*X1::one()+X1::one(),"same 2==1*1+1 using fma");
}