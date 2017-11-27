#include "mex.h"   
//#include "zposit8.hpp"
#include "posit.h"


using zposit_type = Posit<int8_t,8,0,uint16_t,true>;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if(nrhs != 2 && nlhs != 1)
    {
        mexPrintf("zp8Tofloat(input,output template) -> output. Use (unsigned) int 16 for half precision. \n");
        return;
    }
    if(mxGetClassID(prhs[0]) != mxINT8_CLASS)
    {
        mexPrintf("zp8Tofloat(input,output template) -> output. Use (unsigned) int 16 for half precision. \n");
        return;
    }
    switch(mxGetClassID(prhs[1]))
    {
        case mxDOUBLE_CLASS:
        case mxSINGLE_CLASS:
        case mxINT16_CLASS:
        case mxUINT16_CLASS:
            break;
        default:
            mexPrintf("zp8Tofloat unsupported output type: float double half(as int16)\n");
            return;
    }
    bool complex = mxIsComplex(prhs[0]);
	const auto ndims = mxGetNumberOfDimensions(prhs[0]);
	const mwSize * dimi = mxGetDimensions(prhs[0]);
	plhs[0] = mxCreateUninitNumericArray(ndims,(mwSize*)dimi,mxGetClassID(prhs[1]),!complex ? mxREAL: mxCOMPLEX);
    zposit_type * src = (zposit_type*)mxGetData(prhs[0]);
    int n = mxGetNumberOfElements(prhs[0])*(complex?2:1);
    switch(mxGetClassID(prhs[1]))
    {
    	case mxDOUBLE_CLASS: 
            {
                double * dst = (double*)mxGetData(plhs[0]);
                for(int i = 0;i < n; i++)
                {
                    dst[i] = (double)src[i];   
                }                
            }
        	// double precision
    		break;
        case mxSINGLE_CLASS: 
            {
                float * dst = (float*)mxGetData(plhs[0]);
                for(int i = 0;i < n; i++)
                {
                    dst[i] = (float)src[i];                       
                }                                
            }
    		break;
        case mxINT16_CLASS: 
        case mxUINT16_CLASS:
            {
                halffloat * dst = (halffloat*)mxGetData(plhs[0]);
                for(int i = 0;i < n; i++)
                {
                    dst[i] = (halffloat)src[i].unpack(); 
                }                                
            }
    		break;
        default:
            break;
    }

}