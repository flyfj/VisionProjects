//使用cvDFT进行傅立叶变换
//使用cvDFT进行傅立逆叶变换
//fft2 fft2shift 参考网路上的代码


/*
src IPL_DEPTH_8U
dst IPL_DEPTH_64F
*/
#include <stdio.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
void fft2(IplImage *src, IplImage *dst)
{
	IplImage *image_Re = 0, *image_Im = 0, *Fourier = 0;
	// int i, j;
	// double Re, Im;
	image_Re = cvCreateImage(cvGetSize(src), IPL_DEPTH_64F, 1);
	//Imaginary part
	image_Im = cvCreateImage(cvGetSize(src), IPL_DEPTH_64F, 1);
	//2 channels (image_Re, image_Im)
	Fourier = cvCreateImage(cvGetSize(src), IPL_DEPTH_64F, 2);

	// Real part conversion from u8 to 64f (double)
	cvConvertScale(src, image_Re, 1, 0);

	// Imaginary part (zeros)
	cvZero(image_Im);

	// Join real and imaginary parts and stock them in Fourier image
	cvMerge(image_Re, image_Im, 0, 0, Fourier);

	// Application of the forward Fourier transform
	cvDFT(Fourier, dst, CV_DXT_FORWARD);

	cvReleaseImage(&image_Re);
	cvReleaseImage(&image_Im);
	cvReleaseImage(&Fourier);
}

/*
src IPL_DEPTH_64F
dst IPL_DEPTH_8U
*/
void fft2shift(IplImage *src, IplImage *dst)
{
	IplImage *image_Re = 0, *image_Im = 0;
	int nRow, nCol, i, j, cy, cx;
	double scale, shift, tmp13, tmp24;
	image_Re = cvCreateImage(cvGetSize(src), IPL_DEPTH_64F, 1);
	//Imaginary part
	image_Im = cvCreateImage(cvGetSize(src), IPL_DEPTH_64F, 1);
	cvSplit( src, image_Re, image_Im, 0, 0 );

	// Compute the magnitude of the spectrum Mag = sqrt(Re^2 + Im^2)
	cvPow( image_Re, image_Re, 2.0);
	cvPow( image_Im, image_Im, 2.0);
	cvAdd( image_Re, image_Im, image_Re);
	cvPow( image_Re, image_Re, 0.5 );

	// Compute log(1 + Mag)
	cvAddS( image_Re, cvScalar(1.0), image_Re ); // 1 + Mag
	cvLog( image_Re, image_Re ); // log(1 + Mag)

	// Rearrange the quadrants of Fourier image so that the origin is at
	// the image center
	nRow = src->height;
	nCol = src->width;
	cy = nRow/2; // image center
	cx = nCol/2;
	for( j = 0; j < cy; j++ ){
		for( i = 0; i < cx; i++ ){
			tmp13 = CV_IMAGE_ELEM( image_Re, double, j, i);
			CV_IMAGE_ELEM( image_Re, double, j, i) = CV_IMAGE_ELEM(
				image_Re, double, j+cy, i+cx);
			CV_IMAGE_ELEM( image_Re, double, j+cy, i+cx) = tmp13;

			tmp24 = CV_IMAGE_ELEM( image_Re, double, j, i+cx);
			CV_IMAGE_ELEM( image_Re, double, j, i+cx) =
				CV_IMAGE_ELEM( image_Re, double, j+cy, i);
			CV_IMAGE_ELEM( image_Re, double, j+cy, i) = tmp24;
		}
	}

	double minVal = 0, maxVal = 0;
	// Localize minimum and maximum values
	cvMinMaxLoc( image_Re, &minVal, &maxVal );

	//printf("\nmin = %g, max = %g\n", minVal, maxVal);

	//printf("\n Image size : %i x %i pixels\n", image_Re->width, image_Re->height);

	// Normalize image (0 - 255) to be observed as an u8 image
	scale = 255/(maxVal - minVal);
	shift = -minVal * scale;
	cvConvertScale( image_Re, dst, scale, shift);
	cvReleaseImage(&image_Re);
	cvReleaseImage(&image_Im);
}

//下面是调用例子：
int main()
{
	IplImage *src;
	IplImage *Fourier;
	IplImage *dst ;
	IplImage *ImageRe;
	IplImage *ImageIm;
	IplImage *Image;
	IplImage *ImageDst;
	double m,M;
	double scale;
	double shift;
	src = cvLoadImage("F:\\denight\\image22.jpg",0);
	Fourier = cvCreateImage(cvGetSize(src),IPL_DEPTH_64F,2);
	dst = cvCreateImage(cvGetSize(src),IPL_DEPTH_64F,2);
	ImageRe = cvCreateImage(cvGetSize(src),IPL_DEPTH_64F,1);
	ImageIm = cvCreateImage(cvGetSize(src),IPL_DEPTH_64F,1);
	Image = cvCreateImage(cvGetSize(src),src->depth,src->nChannels);
	ImageDst = cvCreateImage(cvGetSize(src),src->depth,src->nChannels);
	fft2(src,Fourier);
	fft2shift(Fourier, Image);
	cvDFT(Fourier,dst,CV_DXT_INV_SCALE);//实现傅里叶逆变换
	cvSplit(dst,ImageRe,ImageIm,0,0);
	cvPow(ImageRe,ImageRe,2);
	cvPow(ImageIm,ImageIm,2);
	cvAdd(ImageRe,ImageIm,ImageRe,NULL);
	cvPow(ImageRe,ImageRe,0.5);
	cvMinMaxLoc(ImageRe,&m,&M,NULL,NULL);
	scale = 255/(M - m);
	shift = -m * scale;
	cvConvertScale(ImageRe,ImageDst,scale,shift);
	cvNamedWindow("kk",1);
	cvShowImage("kk",src);
	cvNamedWindow("win",1);
	cvShowImage("win",ImageDst);
	cvNamedWindow("fu",1);
	cvShowImage("fu",Image);
	cvWaitKey(0);
	cvReleaseImage(&src);
	cvReleaseImage(&Image);
	cvReleaseImage(&ImageIm);
	cvReleaseImage(&ImageRe);
	cvReleaseImage(&Fourier);
	cvReleaseImage(&dst);
	cvReleaseImage(&ImageDst);

}