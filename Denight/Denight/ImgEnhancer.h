//////////////////////////////////////////////////////////////////////////
//  Image enhancer
//	based on paper
//	'A multiscale retinex for bridging the gap between color images and human observation of scenes'
//	developed by fengjie
//	2009-11-3
//	update: 2013-3-25
//////////////////////////////////////////////////////////////////////////


#pragma once

#include <opencv2\opencv.hpp>
#include <cmath>
using namespace std;
using namespace cv;


class ImgEnhancer
{
private:
	
	// params
	int c[3];	// Guassian kernel scale
	float alpha;
	float beta;
	float G;
	float b;

	vector<Mat> gaussKernels;
	
	// exchange 1st quad with 3rd, and 2nd with 4th
	bool ShiftDFT(Mat& src_arr, Mat& dst_arr);

	//////////////////////////////////////////////////////////////////////////
	//	do convolution in Frequency domain
	//	@return		final image after convolution
	//////////////////////////////////////////////////////////////////////////
	bool cvFFTConvolution(const Mat& channel, const Mat& kernel, Mat& conv);

	bool GenerateGaussianKernels(int imgw, int imgh);

	

public:
	ImgEnhancer(void);
	~ImgEnhancer(void);

	bool ifInit;

	bool Init(const Mat& img);

	bool MSRCR(const Mat& img, Mat& output);
};

