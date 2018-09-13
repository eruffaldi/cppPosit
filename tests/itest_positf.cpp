#include "positf.h"


int main()
{
	using XF=PositF<int32_t,8,1,double,true>;
	using X=XF::PPT;

	XF af0(XF::DeepInit(),50);
	X ai0(X::DeepInit(),50);
	for(int32_t i = -128; i < 127; i++)
	{
		XF af(XF::DeepInit(),i);
		X ai(X::DeepInit(),i);
		auto rf = af+af0;
		auto ri = ai+ai0;
		if(rf.v != ri.v)
		{
			std::cout << rf << " != " << ri << " from " << af << " " << ai << " + " << (double)ai0 << std::endl;
		}
	}
}

void itest_positf()
{
	main();
}
