#include "mex.h"   
//#include "zposit8.hpp"
#include "posit.h"
#include <stdint.h>
using zposit_type = Posit<int8_t,8,0,uint16_t,true>;


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if(nrhs != 2 && nlhs != 1)
    {
        mexPrintf("out = zp8unop(a,op);\n");
        return;
    }
    if(mxGetClassID(prhs[0]) != mxINT8_CLASS)
    {
        mexPrintf("expected signed int 8bit\n");
        return;        
    }
    if(mxGetClassID(prhs[1]) != mxCHAR_CLASS)
    {
        mexPrintf("expected string op\n");
        return;                
    }
    const auto ndims = mxGetNumberOfDimensions(prhs[0]);
    const mwSize * dimi = mxGetDimensions(prhs[0]);
    bool complex1 = mxIsComplex(prhs[0]);
    int n1 = mxGetNumberOfElements(prhs[0])*(complex1?2:1);
     char * opa = mxArrayToString(prhs[1]);
     int n = n1;

	plhs[0] = mxCreateUninitNumericArray(ndims,(mwSize*)dimi,mxINT8_CLASS,!complex1 ? mxREAL: mxCOMPLEX);
    const zposit_type * a = (const zposit_type*)mxGetData(prhs[0]);
    zposit_type * dst = (zposit_type*)mxGetData(plhs[0]);

    if(strcmp(opa,"inv") == 0)
    {
        for(int i= 0; i < n; i++)
        {
            dst[i] = inv(a[i]);
        }
    }
    else
    {
        mexPrintf("unknown operation <%s>\n",opa);
    }
    mxFree((void*)opa);
    

}