//////////////////////////////////////////////////////////////////////////
//  Image denight
//	'A multiscale retinex for bridging the gap between color images and human observation of scenes'
//	fengjie@pku.cis
//	2009-11-3
//////////////////////////////////////////////////////////////////////////

#include <opencv2\opencv.hpp>
#include "ImgEnhancer.h"
#include <cmath>
#include <iostream>
#include <fstream>
using namespace std;
using namespace cv;




int main()
{
	//parameters
	int c[3] = {80, 120, 250};	//scale parameters
	float alpha = 125, beta = 46;
	float G = 192, b = -30;

	//obtain image
	Mat old_img = imread("normal.jpeg");
	Mat img;
	resize(old_img, img, Size(old_img.cols, old_img.rows));

	imshow("old", img);

	cout<<img.cols<<" "<<img.rows<<endl;

	double start_t = getTickCount();

	ImgEnhancer enhancer;
	enhancer.Init(img);
	Mat output;
	enhancer.MSRCR(img, output);

	/*cv::GaussianBlur(output, frame, cv::Size(0, 0), 3);
	cv::addWeighted(output, 1.5, frame, -0.5, 0, output);

	imshow("sharp", output);
	waitKey(0);*/

	cout<<(getTickCount() - start_t) / getTickFrequency()<<"s"<<endl;

	cvDestroyAllWindows();

	return 0;
}