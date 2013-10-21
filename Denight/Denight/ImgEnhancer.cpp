#include "ImgEnhancer.h"


ImgEnhancer::ImgEnhancer(void)
{
	//scale parameters
	c[0] = 15; //80;
	c[1] = 81; //120;
	c[2] = 251;

	alpha = 125, beta = 46;
	G = 192, b = -30;

	ifInit = false;
}


bool ImgEnhancer::ShiftDFT(Mat& src_arr, Mat& dst_arr)
{
	Mat tmp;
	Mat q1, q2, q3, q4;
	Mat d1, d2, d3, d4;

	Size size(src_arr.cols, src_arr.rows);
	Size dst_size(dst_arr.cols, dst_arr.rows);
	int cx, cy;

	if(dst_size.width != size.width || 
		dst_size.height != size.height){
			//cvError( CV_StsUnmatchedSizes, "cvShiftDFT", "Source and Destination arrays must have equal sizes", __FILE__, __LINE__ ); 
		return false;
	}

	if(src_arr.data == dst_arr.data){
		tmp.create(size.height/2, size.width/2, src_arr.depth());
	}

	//cx = ((double)size.width/2);
	cx = size.width/2;
	//cy = cvFloor((double)size.height/2); // image center
	cy = size.height/2;

	// reference
	q1 = src_arr(Rect(0, 0, cx, cy));
	q2 = src_arr(Rect(cx, 0, cx, cy));
	q3 = src_arr(Rect(cx, cy, cx, cy));
	q4 = src_arr(Rect(0, cy, cx, cy));
	d1 = dst_arr(Rect(0, 0, cx, cy));
	d2 = dst_arr(Rect(cx, 0, cx, cy));
	d3 = dst_arr(Rect(cx, cy, cx, cy));
	d4 = dst_arr(Rect(0, cy, cx, cy));

	if(src_arr.data != dst_arr.data){

		if( q1.depth() != d1.depth()){
			//cvError( CV_StsUnmatchedFormats, "cvShiftDFT", "Source and Destination arrays must have the same format", __FILE__, __LINE__ ); 
			return false;
		}

		q3.copyTo(d1);
		q4.copyTo(d2);
		q1.copyTo(d3);
		q2.copyTo(d4);
	}
	else{
		q3.copyTo(tmp);
		q1.copyTo(q3);
		tmp.copyTo(q1);
		q4.copyTo(tmp);
		q2.copyTo(q4);
		tmp.copyTo(q2);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
//	do convolution in Frequency domain
//	@return		final image after convolution
//////////////////////////////////////////////////////////////////////////
bool ImgEnhancer::cvFFTConvolution(const Mat& channel, const Mat& kernel, Mat& conv)
{
	//create complex image and kernel
	//////////////////////////////////////////////////////////////////////////
	Mat complexImg(channel.rows, channel.cols, CV_32FC2);
	vector<Mat> vec_mat(2);
	vec_mat[0] = channel.clone();
	vec_mat[1] = channel.clone();
	vec_mat[1].setTo(0);
	merge(vec_mat, complexImg);

	//////////////////////////////////////////////////////////////////////////
	Mat complexKernel(kernel.rows, kernel.cols, CV_32FC2);
	kernel.copyTo(vec_mat[0]);
	kernel.copyTo(vec_mat[1]);
	vec_mat[1].setTo(0);
	vec_mat[1] = kernel.clone();
	merge(vec_mat, complexKernel);

	//////////////////////////////////////////////////////////////////////////
	/*int dft_M = getOptimalDFTSize(channel.rows*2-1);
	int dft_N = getOptimalDFTSize(channel.cols*2-1);*/

	Mat dft_A(channel.rows, channel.cols, CV_32FC2);
	dft_A.setTo(0);
	Mat dft_B(kernel.rows, kernel.cols, CV_32FC2);
	dft_B.setTo(0);

	//////////////////////////////////////////////////////////////////////////
	// do dft for image
	//////////////////////////////////////////////////////////////////////////
	Mat tmp = dft_A(Rect(0, 0, channel.cols, channel.rows));
	complexImg.copyTo(tmp);

	dft(dft_A, dft_A, CV_DXT_FORWARD);

	//////////////////////////////////////////////////////////////////////////
	// do dft for kernel
	//////////////////////////////////////////////////////////////////////////
	tmp = dft_B(Rect(0, 0, kernel.cols, kernel.rows));
	complexKernel.copyTo(tmp);

	//do dft
	dft(dft_B, dft_B, CV_DXT_FORWARD);

	//shift kernel to center
	ShiftDFT(dft_B, dft_B);

	//////////////////////////////////////////////////////////////////////////
	//	do convolution
	//////////////////////////////////////////////////////////////////////////
	mulSpectrums(dft_A, dft_B, dft_A, CV_DXT_MUL_CONJ);

	//do inverse dft
	dft(dft_A, dft_A, CV_DXT_INV_SCALE, channel.rows);
	//idft(dft_A, dft_A);

	split(dft_A, vec_mat);
	pow(vec_mat[0], 2, vec_mat[0]);
	pow(vec_mat[1], 2, vec_mat[1]);
	vec_mat[0] += vec_mat[1];
	pow(vec_mat[0], 0.5, conv);

	return true;
}

bool ImgEnhancer::GenerateGaussianKernels(int imgw, int imgh)
{
	gaussKernels.resize(3);
	for(int i=0; i<3; i++)
	{
		gaussKernels[i].create(imgh, imgw, CV_32F);
		Point3d center((double)imgw/2, (double)imgh/2, 0.0f);
		double sum = 0;
		for(int j=0; j<gaussKernels[i].rows; j++)
		{
			double y = j - center.y;
			for(int k=0; k<gaussKernels[i].cols; k++)
			{
				double x = k - center.x;
				double val = exp(-(x*x+y*y)/(c[i]*c[i]));
				gaussKernels[i].at<float>(j, k) = val;
				sum += val;
			}
		}
		
		//normalize
		gaussKernels[i] /= sum;
	}

	return true;
}

bool ImgEnhancer::Init(const Mat& img)
{
	GenerateGaussianKernels(img.cols, img.rows);
	
	ifInit = true;

	return true;
}

bool ImgEnhancer::MSRCR(const Mat& img, Mat& output)
{
	if(img.channels() != 3)
		return false;

	// resize
	Mat new_img = img.clone();
	//resize(img, new_img, Size(img.cols, img.rows));

	// sharpening
	Mat blur = img.clone();
	cv::GaussianBlur(img, blur, cv::Size(0, 0), 3);
	cv::addWeighted(img, 1.5, blur, -0.5, 0, new_img);
	
	// do processing
	Mat dimg(new_img.rows, new_img.cols, CV_32FC3);
	new_img.convertTo(dimg, dimg.type());
	//split to 3 channels
	vector<Mat> channels(3);
	split(dimg, channels);	// bgr

	//////////////////////////////////////////////////////////////////////////
	// MSRCR
	//////////////////////////////////////////////////////////////////////////
	vector<Mat> scales(3);
	for(int i=0; i<3; i++)
		scales[i].create(dimg.rows, dimg.cols, CV_32F);

	Mat logimg1(dimg.rows, dimg.cols, CV_32F);
	Mat logimg2 = logimg1.clone();
	
	Mat channelSum = logimg1.clone();
	channelSum.setTo(0);
	channelSum += channels[0];
	channelSum += channels[1];
	channelSum += channels[2];

	Mat logtemp = logimg1.clone();

	//process each channel
	for(int i=0; i<3; i++)
	{
		//compute C(i,j)
		Mat colorrestore = channels[i] * alpha;
		colorrestore += 1.0;
		log(colorrestore, logimg1);
		log(channelSum+1, logimg2);
		colorrestore = (logimg1 - logimg2) * beta;

		//convert to log
		//avoid being divided by 0
		logimg1 = channels[i] + 1.0;
		//compute R(i,j)
		log(logimg1, logimg1);

		//process each scale
		for(int j=0; j<3; j++)
		{
			// TODO: not have any effect
			// cvFFTConvolution(channels[i], gaussKernels[j], scales[j]);
			//scales[j] += 1.0;

			// do gaussian kernel convolution
			/*GaussianBlur(channels[i], scales[j], cv::Size(c[j], c[j]), 0, 0);
			scales[j] += 1;*/
			
			scales[j].setTo(1);
			log(scales[j], scales[j]);
			scales[j] = logimg1 - scales[j];
		}

		//combine multiscale
		channels[i] = (scales[0] + scales[1] + scales[2]) / 3;
		
		//multiply CRC
		channels[i] = colorrestore.mul(channels[i]);
		channels[i] = (channels[i] + b) * G;

		// linear stretching
		double minval = 0;
		double maxval = 0;
		minMaxLoc(channels[i], &minval, &maxval);
		for(int k=0; k<channels[i].rows; k++)
		{
			for(int kk=0; kk<channels[i].cols; kk++)
			{
				float val = channels[i].at<float>(k, kk);
				if(val < minval)
					channels[i].at<float>(k, kk) = 0;
				else if(val <= maxval)
					channels[i].at<float>(k, kk) = (val-minval)*255 / (maxval-minval);
				else
					channels[i].at<float>(k, kk) = 255;
			}
		}
	}

	//merge into single image
	merge(channels, dimg);
	//cvConvertScale(dimg, dimg, 255.0f);
	Mat result = img.clone();
	dimg.convertTo(result, result.type());

	output = img.clone();
	bilateralFilter ( result, output, 11, 2*11, 11/2 );
	//bilateralFilter(output, result, 11, 2*11, 11/2);
	imshow("bilateral1", output);
	/*Mat back = output.clone();
	bilateralFilter ( output, back, 11, 2*11, 11/2 );
	imshow("bilateral2", back);*/
	
	medianBlur(output, output, 3);
	imshow("Median", output);

	waitKey(0);

	return true;
}