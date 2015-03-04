/*********************************************************************
 * convert to janus matrix file
 * by: Jie Feng
 *
 ********************************************************************/
#include <matrix.h>
#include <mex.h>
#include <fstream>
#include <iostream>
using namespace std;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    // matrix, fn, type(0: mask, 1: data)
    if(nrhs != 3) {
        mexPrintf("inputs are not right.\n");
        return;
    }
    //declare variables
    char *simptr;
    char *fnptr;
    const mwSize *dims;
    int columns, rows;

    //associate inputs
    simptr = (char*)mxGetData(prhs[0]);
    fnptr = mxArrayToString(prhs[1]);
    int is_mask = (int)mxGetScalar(prhs[2]);
    mexPrintf("%d\n", is_mask);

    //figure out dimensions
    dims = mxGetDimensions(prhs[0]);
    rows = (int)dims[0];
    columns = (int)dims[1];
    
    // write to file
    ofstream stream(fnptr, ios::out | ios::binary);
    stream << "S2\n"
           <<  "./data/gallery_dummy.gal\n"
           << "./data/probe1_dummy.gal\n"
           << 'M' << (is_mask == 1 ? 'B' : 'F') << ' '
           << rows << ' ' << columns << ' ';
    int endian = 0x12345678;
    stream.write((const char*)&endian, 4);
    stream << '\n';
    stream.write((const char*)simptr, rows * columns * (is_mask ? 1 : 4));
    
    mexPrintf("result written to file.\n");
    
    return;
}
