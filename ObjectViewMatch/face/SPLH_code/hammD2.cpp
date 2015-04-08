#include <mex.h>
#include <stdio.h>
#include <math.h>

// Compile with: 
//     mex -largeArrayDims hamming.cpp

// Usage:
//    dist_mat = hamming( probabs, bin_threshold , max_distance );
//
// Takes in a matrix (nCodeBits by nVectors) of PROBABILITIES. Compares them to a binarization threshold and turns each vector
// into a single uint32 number. Then quickly, but exhaustively, computes hamming distance between all pairs of uint32 numbers. 
// It gives up if the distance exceeds max_distance. 
// Output is matrix of size nVectors x nVectors of biary hamming distances. Dynamically allocates more memory for array as needed.
// If initial ammount is too large, turn down INITIAL_PERCENT_SPARSE.
// WARNING: Current version only handles 32bits.

 
/* Input Arguments */
#define	VEC   	prhs[0] // Uint8 vector of size n x 1
#define MAT     prhs[1] // Uint8 matrix of size n x m

/* Output Arguments */
#define	OUTPUT	plhs[0] // Double vector 1 x m binary hamming distance

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] )
     
{ 
  int nBits, nVecs, nBits_Vecs,  i, j;
  unsigned int dist;
  double* outputp;
  unsigned char tmp, *pMat, *pVec, *pV;

  const int lookup [] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8};

  /* Check for proper number of arguments */
    
  if (nrhs != 2) { 
    mexErrMsgTxt("Two input arguments required."); 
  } else if (nlhs != 1) {
    mexErrMsgTxt("One output argument rqeuired."); 
  } 

  if (!mxIsUint8(MAT))
    mexErrMsgTxt("Matrix must be uInt8");

  if (!mxIsUint8(VEC))
    mexErrMsgTxt("Vector must be uInt8");


  /* Get dimensions of image and kernel */
  nBits = (int)  mxGetM(MAT); 
  nVecs = (int)  mxGetN(MAT); 
  nBits_Vecs = (int)  mxGetM(VEC); 
  //mexPrintf("Bits: %d  Vectors: %d %d\n",nBits,nVecs,nBits_Vecs);

  if (nBits!=nBits_Vecs)
    mexErrMsgTxt("Dimension mismatch btw. matrix and vector");
    

  //mexPrintf("Bits: %d  Vectors: %d\n",nBits,nVecs);

  // Create sparse array
  OUTPUT = mxCreateNumericMatrix(1,nVecs,mxDOUBLE_CLASS,mxREAL);
  outputp = (double*) mxGetPr(OUTPUT);
  
  pMat = (unsigned char*) mxGetPr(MAT);
  pVec = (unsigned char*) mxGetPr(VEC);

  for (i=0;i<nVecs;i++,outputp++){
    pV=pVec;
    for (j=0;j<nBits;j++,pMat++,pV++){
      *outputp += lookup[*pV^*pMat];
    }

  }
  return;
    
}

