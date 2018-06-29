#include "softfloat.hpp"
#include <math.h>
#include <fstream>

int main(int argc, char const *argv[])
{
	{
		uint16_t opexp2[65536]; 
		for(int i = 0; i < 65536; i++)
		{
			float16_t x;
			x.v = i;
			softfloat16 u(x);
			float uf = (float)u;
			float ef = exp2(uf);
			softfloat16 efu(ef);
			opexp2[i] = ((float16_t)efu).v;
		}
		std::ofstream onf(argc == 1 ? "float16_tbl.cpp":argv[1]);
		onf << "#include <stdint.h>\nuint16_t float16_opexp2[] = {\n";
		for(int i  = 0; i < 256*256; i++)
			onf << (int)(opexp2[i]) << ",";
		onf << "}; " << std::endl;
	}
}