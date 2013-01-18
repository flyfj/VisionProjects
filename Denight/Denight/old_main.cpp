//////////////////////////////////////////////////////////////////////////
//  Image denight
//	'A multiscale retinex for bridging the gap between color images and human observation of scenes'
//	fengjie@pku.cis
//	2009-11-3
//////////////////////////////////////////////////////////////////////////


#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <cmath>
#include <iostream>
#include <fstream>
using namespace std;


//////////////////////////////////////////////////////////////////////////
//	shift
//////////////////////////////////////////////////////////////////////////
void cvShiftDFT(CvArr * src_arr, CvArr * dst_arr )
{
	CvMat * tmp;
	CvMat q1stub, q2stub;
	CvMat q3stub, q4stub;
	CvMat d1stub, d2stub;
	CvMat d3stub, d4stub;
	CvMat * q1, * q2, * q3, * q4;
	CvMat * d1, * d2, * d3, * d4;

	CvSize size = cvGetSize(src_arr);
	CvSize dst_size = cvGetSize(dst_arr);
	int cx, cy;

	if(dst_size.width != size.width || 
		dst_size.height != size.height){
			cvError( CV_StsUnmatchedSizes, "cvShiftDFT", "Source and Destination arrays must have equal sizes", __FILE__, __LINE__ ); 
	}

	if(src_arr==dst_arr){
		tmp = cvCreateMat(size.height/2, size.width/2, cvGetElemType(src_arr));
	}

	cx = cvFloor((double)size.width/2);
	cy = cvFloor((double)size.height/2); // image center

	q1 = cvGetSubRect( src_arr, &q1stub, cvRect(0, 0, cx, cy) );
	q2 = cvGetSubRect( src_arr, &q2stub, cvRect(cx, 0, cx, cy) );
	q3 = cvGetSubRect( src_arr, &q3stub, cvRect(cx, cy, cx, cy) );
	q4 = cvGetSubRect( src_arr, &q4stub, cvRect(0, cy, cx, cy) );
	d1 = cvGetSubRect( dst_arr, &d1stub, cvRect(0, 0, cx, cy) );
	d2 = cvGetSubRect( dst_arr, &d2stub, cvRect(cx, 0, cx, cy) );
	d3 = cvGetSubRect( dst_arr, &d3stub, cvRect(cx, cy, cx, cy) );
	d4 = cvGetSubRect( dst_arr, &d4stub, cvRect(0, cy, cx, cy) );

	if(src_arr!=dst_arr){
		if( !CV_ARE_TYPES_EQ( q1, d1 )){
			cvError( CV_StsUnmatchedFormats, "cvShiftDFT", "Source and Destination arrays must have the same format", __FILE__, __LINE__ ); 
		}
		cvCopy(q3, d1, 0);
		cvCopy(q4, d2, 0);
		cvCopy(q1, d3, 0);
		cvCopy(q2, d4, 0);
	}
	else{
		cvCopy(q3, tmp, 0);
		cvCopy(q1, q3, 0);
		cvCopy(tmp, q1, 0);
		cvCopy(q4, tmp, 0);
		cvCopy(q2, q4, 0);
		cvCopy(tmp, q2, 0);
	}
}



//////////////////////////////////////////////////////////////////////////
//	do convolution in Frequency domain
//	@return		final image after convolution
//////////////////////////////////////////////////////////////////////////
IplImage* cvFFTConvolution(const IplImage *channel, const CvMat *kernel)
{
	//create complex image and kernel
	//////////////////////////////////////////////////////////////////////////
	IplImage *realImg = cvCloneImage(channel);
	IplImage *imaginaryImg = cvCloneImage(channel);
	cvSetZero(imaginaryImg);
	IplImage *complexImg = cvCreateImage(cvGetSize(channel), IPL_DEPTH_32F, 2);
	cvMerge(realImg, imaginaryImg, NULL, NULL, complexImg);
	//////////////////////////////////////////////////////////////////////////
	CvMat *realKernel = cvCloneMat(kernel);
	CvMat *imaginaryKernel = cvCloneMat(kernel);
	cvSetZero(imaginaryKernel);
	CvMat *complexKernel = cvCreateMat(realKernel->rows, realKernel->cols, CV_32FC2);
	cvMerge(realKernel, imaginaryKernel, NULL, NULL, complexKernel);

	//////////////////////////////////////////////////////////////////////////
	int dft_M = cvGetOptimalDFTSize(channel->height*2-1);
	int dft_N = cvGetOptimalDFTSize(channel->width*2-1);

	CvMat *dft_A = cvCreateMat(channel->height, channel->width, CV_32FC2);
	cvSetZero(dft_A);
	CvMat *dft_B = cvCreateMat(kernel->height, kernel->width, CV_32FC2);
	cvSetZero(dft_B);
	IplImage *conv = cvCloneImage(channel);
	CvMat tmp;

	//////////////////////////////////////////////////////////////////////////
	// do dft for image
	//////////////////////////////////////////////////////////////////////////
	cvGetSubRect(dft_A, &tmp, cvRect(0, 0, channel->width, channel->height));
	cvCopy(complexImg, &tmp, NULL);

	cvDFT(dft_A, dft_A, CV_DXT_FORWARD);


	//////////////////////////////////////////////////////////////////////////
	// do dft for kernel
	//////////////////////////////////////////////////////////////////////////
	cvGetSubRect(dft_B, &tmp, cvRect(0, 0, kernel->cols, kernel->rows));
	cvCopy(complexKernel, &tmp, NULL);

	//do dft
	cvDFT(dft_B, dft_B, CV_DXT_FORWARD);

	//shift kernel to center
	CvMat *dft_BB = cvCloneMat(dft_B);
	cvShiftDFT(dft_B, dft_BB);
	cvCopy(dft_BB, dft_B);
	cvReleaseMat(&dft_BB);

	//////////////////////////////////////////////////////////////////////////
	//	do convolution
	//////////////////////////////////////////////////////////////////////////
	cvMulSpectrums(dft_A, dft_B, dft_A, CV_DXT_MUL_CONJ);

	//do inverse dft
	cvDFT(dft_A, dft_A, CV_DXT_INV_SCALE, channel->height);
	CvMat *realRes = cvCreateMat(dft_A->rows, dft_A->cols, CV_32F);
	CvMat *imageRes = cvCloneMat(realRes);
	cvSplit(dft_A, realRes, imageRes, NULL, NULL);
	cvPow(realRes, realRes, 2);
	cvPow(imageRes, imageRes, 2);
	cvAdd(realRes, imageRes, realRes, NULL);
	cvPow(realRes, realRes, 0.5);
	
	//copy back
	cvCopy(realRes, conv);

	//clean
	cvReleaseImage(&realImg);
	cvReleaseImage(&imaginaryImg);
	cvReleaseImage(&complexImg);
	cvReleaseMat(&realKernel);
	cvReleaseMat(&imaginaryKernel);
	cvReleaseMat(&complexKernel);
	cvReleaseMat(&dft_A);
	cvReleaseMat(&dft_B);
	cvReleaseMat(&realRes);
	cvReleaseMat(&imageRes);

	return conv;
}


int main()
{
	//parameters
	int c[3] = {80, 120, 250};	//scale parameters
	float alpha = 125, beta = 46;
	float G = 192, b = -30;

	cvNamedWindow("win",1);
	cvNamedWindow("res",1);

	//obtain image
	IplImage *img = cvLoadImage("F:\\denight\\image21.jpg", 1);
	IplImage *dimg = cvCreateImage(cvGetSize(img), IPL_DEPTH_32F, 3);
	cvConvert(img, dimg);
	//split to 3 channels
	IplImage* channels[3];
	for(int i=0; i<3; i++)
		channels[i] = cvCreateImage(cvGetSize(img), IPL_DEPTH_32F, 1);
	cvSplit(dimg, channels[0], channels[1], channels[2], NULL);	//bgr

	//////////////////////////////////////////////////////////////////////////
	// generate surround function for 3 scales corresponds different 
	// smooth kernel: std = sqrt(2)/2*c (OK)
	//////////////////////////////////////////////////////////////////////////
	CvMat *gaussKernels[3];
	for(int i=0; i<3; i++)
	{
		gaussKernels[i] = cvCreateMat(dimg->height, dimg->width, CV_32FC1);
		CvPoint3D32f center = cvPoint3D32f((double)dimg->width/2, (double)dimg->height/2, 0.0f);
		double sum = 0;
		for(int j=0; j<gaussKernels[i]->rows; j++)
		{
			double y = j-center.y;
			for(int k=0; k<gaussKernels[i]->cols; k++)
			{
				double x = k-center.x;
				double val = exp(-(x*x+y*y)/(2*c[i]*c[i]));
				gaussKernels[i]->data.fl[j*gaussKernels[i]->width+k] = val;
				sum += val;
			}
		}
		sum = 1.0f/sum;
		//normalize
		cvConvertScale(gaussKernels[i], gaussKernels[i], sum);
	}

	//////////////////////////////////////////////////////////////////////////
	// MSRCR
	//////////////////////////////////////////////////////////////////////////
	IplImage *scales[3];
	for(int i=0; i<3; i++)
		scales[i] = cvCreateImage(cvGetSize(img), IPL_DEPTH_32F, 1);
	IplImage *logimg1 = cvCreateImage(cvGetSize(img), IPL_DEPTH_32F, 1);
	IplImage *logimg2 = cvCloneImage(logimg1);
	
	IplImage *channelSum = cvCloneImage(logimg1);
	cvSetZero(channelSum);
	cvAdd(channels[0], channels[1], channelSum);
	cvAdd(channels[2], channelSum, channelSum);

	IplImage *logtemp = cvCloneImage(logimg1);

	//process each channel
	for(int i=0; i<3; i++)
	{
		//compute C(i,j)
		IplImage *colorrestore = cvCloneImage(channelSum);
		cvConvertScale(channels[i], colorrestore, alpha);
		cvAddS(colorrestore, cvRealScalar(1.0), logtemp);
		cvCopyImage(logtemp, colorrestore);
		cvLog(colorrestore, logimg1);
		cvAddS(channelSum, cvRealScalar(1.0), logtemp);
		cvCopyImage(logtemp, channelSum);
		cvLog(channelSum, logimg2);
		cvSub(logimg1, logimg2, colorrestore);
		cvConvertScale(colorrestore, colorrestore, beta);

		//convert to log
		//avoid being divided by 0
		cvAddS(channels[i], cvRealScalar(1.0), logtemp);
		//compute R(i,j)
		cvLog(logtemp, logimg1);

		//process each scale
		for(int j=0; j<3; j++)
		{
			IplImage *res = cvFFTConvolution(channels[i], gaussKernels[j]);
			/*double minv = 0;
			cvMinMaxLoc(res, &minv, 0, 0, 0, 0);
			cvSubS(res, cvRealScalar(minv), scales[j]);*/
			cvCopyImage(res, scales[j]);
			cvReleaseImage(&res);
			cvAddS(scales[j], cvRealScalar(1.0), logtemp);
			cvLog(logtemp, logimg2);
			cvSub(logimg1, logimg2, scales[j]);
		}

		//combine multiscale
		cvSetZero(channels[i]);
		cvAdd(scales[0], scales[1], channels[i]);
		cvAdd(channels[i], scales[2], channels[i]);
		cvConvertScale(channels[i], channels[i], 1.0/3);
		
		//multiply CRC
		cvMul(colorrestore, channels[i], channels[i]);
		cvReleaseImage(&colorrestore);
		cvAddS(channels[i], cvRealScalar(b), channels[i]);
		cvConvertScale(channels[i], channels[i], G);

		
		//ÏßÐÔÀ­Éì
		double minval = 0;
		double maxval = 0;
		cvMinMaxLoc(channels[i], &minval, &maxval, 0, 0, 0);
		for(int k=0; k<channels[i]->height; k++)
		{
			for(int kk=0; kk<channels[i]->width; kk++)
			{
				float val = CV_IMAGE_ELEM(channels[i], float, k, kk);
				if(val < minval)
					CV_IMAGE_ELEM(channels[i], float, k, kk) = 0;
				else if(val <= maxval)
				{
					CV_IMAGE_ELEM(channels[i], float, k, kk) = (val-minval)*255/(maxval-minval);
				}
				else
					CV_IMAGE_ELEM(channels[i], float, k, kk) = 255;
			}
		}
		
	}

	//merge into single image
	cvMerge(channels[0], channels[1], channels[2], NULL, dimg);
	//cvConvertScale(dimg, dimg, 255.0f);
	IplImage* result = cvCloneImage(img);
	cvConvert(dimg, result);

	
	cvShowImage("win", img);
	cvShowImage("res", result);
	cvWaitKey(0);

	//clean
	cvReleaseImage(&img);
	cvReleaseImage(&dimg);
	cvReleaseImage(&logimg1);
	cvReleaseImage(&logimg2);
	cvReleaseImage(&result);
	cvReleaseImage(&channelSum);
	cvReleaseImage(&logtemp);
	for(int i=0; i<3; i++)
	{
		cvReleaseImage(&channels[i]);
		cvReleaseImage(&scales[i]);
	}

	cvDestroyAllWindows();

	return 0;
}