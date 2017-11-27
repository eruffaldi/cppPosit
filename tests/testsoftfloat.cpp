#include "softfloat.hpp"
#include "exp2t.hpp"


extern uint16_t float16_opexp2[];

template <>
struct exp2t_<softfloat16>
{
	softfloat16 operator() (softfloat16 a)
	{
		float16_t w;
		w.v = float16_opexp2[((float16_t)a).v];
		return softfloat16(w);
	}
};

int main(int argc, char * argv[])
{
	std::cout << softfloat16(3)+softfloat16(2) << std::endl;
	std::cout << ((float)exp2t(softfloat16(5.2))) << " vs " << exp2t(5.2f) << std::endl;
	return 0;
}