#include "mex.h"   
//#include "zposit8.hpp"
#include "posit.h"
#include <stdint.h>
using zposit_type = Posit<int8_t,8,0,uint16_t,true>;


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if(nrhs != 1 && nlhs != 1)
    {
        mexPrintf("floatTozp8(input) -> output\n");
        return;
    }
    bool complex = mxIsComplex(prhs[0]);
	const auto ndims = mxGetNumberOfDimensions(prhs[0]);
	const mwSize * dimi = mxGetDimensions(prhs[0]);
	plhs[0] = mxCreateUninitNumericArray(ndims,(mwSize*)dimi,mxINT8_CLASS,!complex ? mxREAL: mxCOMPLEX);
    zposit_type * dst = (zposit_type*)mxGetData(plhs[0]);
    int n = mxGetNumberOfElements(prhs[0])*(complex?2:1);
    switch(mxGetClassID(prhs[0]))
    {
    	case mxDOUBLE_CLASS: 
        	// double precision
            {
                double * src = (double*)mxGetData(prhs[0]);
                for(int i = 0;i < n; i++)
                {
                  dst[i] = zposit_type(src[i]);
                }
            }
    		break;
        case mxSINGLE_CLASS: 
            {
                float * src = (float*)mxGetData(prhs[0]);
                for(int i = 0;i < n; i++)
                {
                  dst[i] = zposit_type(src[i]);
                }
            }
    		break;
        case mxINT16_CLASS: 
        case mxUINT16_CLASS:
            {
                halffloat * src = (halffloat*)mxGetData(prhs[0]);
                for(int i = 0;i < n; i++)
                {
                  dst[i] = zposit_type(typename zposit_type::UnpackedT(halffloat(src[i]))); // wrap cast to unpacked then to posit
                }
            }
    		break;
        default:
            mexErrMsgTxt("Unsupported type of output: only double, single or 16-bit has half float\n");
            break;
    }

}