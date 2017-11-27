#include "mex.h"   
//#include "zposit8.hpp"
#include "posit.h"
#include <stdint.h>
using zposit_type = Posit<int8_t,8,0,uint16_t,true>;


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    if(nrhs != 3 && nlhs != 1)
    {
        mexPrintf("out = zp8binop(a,b,op);\n");
        return;
    }
    if(mxGetClassID(prhs[0]) != mxGetClassID(prhs[1]) || mxGetClassID(prhs[0]) != mxINT8_CLASS)
    {
        mexPrintf("expected signed int 8bit\n");
        return;        
    }
    if(mxGetClassID(prhs[2]) != mxCHAR_CLASS)
    {
        mexPrintf("expected string op\n");
        return;                
    }
    const auto ndims = mxGetNumberOfDimensions(prhs[0]);
    const mwSize * dimi = mxGetDimensions(prhs[0]);
    bool complex1 = mxIsComplex(prhs[0]);
    int n1 = mxGetNumberOfElements(prhs[0])*(complex1?2:1);



    bool complex2 = mxIsComplex(prhs[1]);
    int n2 = mxGetNumberOfElements(prhs[1])*(complex2?2:1);
    if(n1 != n2)
    {
        mexPrintf("mismatch of items count\n");
        return;                    
    }
     char * opa = mxArrayToString(prhs[2]);
     int n = n1;
    if(opa[1] !=0 )
    {
        mexPrintf("unknown operation %s\n",opa);
        mxFree((void*)opa);
        return;            
    }	

    plhs[0] = mxCreateUninitNumericArray(ndims,(mwSize*)dimi,mxINT8_CLASS,!complex2 ? mxREAL: mxCOMPLEX);
    const zposit_type * a = (const zposit_type*)mxGetData(prhs[0]);
    const zposit_type * b = (const zposit_type*)mxGetData(prhs[1]);
    zposit_type * dst = (zposit_type*)mxGetData(plhs[0]);

    char op = opa[0];
    mxFree((void*)opa);
    switch(op)
    {
        case '+':
            for(int i= 0; i < n; i++)
            {
                dst[i] = a[i] + b[i];
            }
            break;
        case '-':
            for(int i= 0; i < n; i++)
            {
                dst[i] = a[i] - b[i];
            }
            break;
        case '*':
            for(int i= 0; i < n; i++)
            {
                dst[i] = a[i] * b[i];
            }
            break;
        case '/':
            for(int i= 0; i < n; i++)
            {
                dst[i] = a[i] / b[i];
            }
            break;
        default:
            mexPrintf("unknown operation %c\n",op);
            break;
    }

}