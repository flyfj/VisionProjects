#pragma once

//#include <Windows.h>
//#include <GdiPlus.h>
#include <opencv2\opencv.hpp>
using namespace cv;
//using namespace Gdiplus;

#include <ImageSimple.h>

// segmentedImg: 3*width*height bgr image buffer
// bmp: input bgr image; segmentedImg: output segmentation image for display
int graph_based_segment(const Mat& img, float sigma, float c, int min_size, ImageUIntSimple& indexImg, unsigned char* segmentedImg);