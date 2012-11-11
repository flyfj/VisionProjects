//////////////////////////////////////////////////////////////////////////
//	conversion function for communicating between imageviewer and segmentor
//	fengjie@MSRA VC
//	2010-7-14
//////////////////////////////////////////////////////////////////////////


#include "ImageAlgorithm.h"
#include "image.h"


image<rgb>* ConvertForward(const CImageRgb& input)
{
	int width = input.Width();
	int height = input.Height();
	image<rgb> *im = new image<rgb>(width, height);
	//copy data from input to im
	rgb val;
	for(int y=0; y<height; y++)
	{
		for(int x=0; x<width; x++)
		{
			PixelRgb t = input.Pixel(x,y);
			val.r = t.R();
			val.b = t.B();
			val.g = t.G();
			imRef(im, x, y) = val;
		}
	}

	return im;
}


CImageRgb ConvertBackward(const image<rgb>* input)
{
	CImageRgb output;
	int width = input->width();
	int height = input->height();
	output.Allocate(width, height);
	for(int y=0; y<height; y++)
	{
		for(int x=0; x<width; x++)
		{
			output.Pixel(x,y) = PixelRgb(imRef(input,x,y).r, \
				imRef(input,x,y).g, imRef(input,x,y).b);
		}
	}

	return output;
}


CImageInt ConvertToIntImg(const image<int>* input)
{
	int width = input->width();
	int height = input->height();
	CImageInt indeximg;
	indeximg.Allocate(width, height);

	for(int y=0; y<height; y++)
	{
		for(int x=0; x<width; x++)
		{
			indeximg.Pixel(x,y) = imRef(input, x, y);
		}
	}

	return indeximg;
}