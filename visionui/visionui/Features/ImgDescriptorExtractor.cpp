//////////////////////////////////////////////////////////////////////////

#include "ImgDescriptorExtractor.h"



namespace visualsearch
{

	ImgDescriptorExtractor::ImgDescriptorExtractor()
	{
		cv_bow_img_desc_extractor = NULL;
		initModule_nonfree();
	}


	ImgDescriptorExtractor::~ImgDescriptorExtractor(void)
	{
	}


	void ImgDescriptorExtractor::ComputeBOWDescriptors(
		const cv_Images& imgs, const string& keypoint_type, const string& desc_type, 
		const Mat& dictionary, vector<Mat>& descriptors)
	{
		
		// check validity
		string keypoint_support_type, descriptor_support_type;
		if( !keypoint_detector.CheckTypeValidity(keypoint_type, keypoint_support_type) 
			|| !keypoint_descriptor.CheckTypeValidity(desc_type, descriptor_support_type) )
		{
			cerr<<"Keypoint or descriptor type not supported."<<endl;
			return;
		}

		if( descriptor_support_type == "opencv" )
		{
			// use opencv extractor
			Ptr<DescriptorExtractor> cv_descriptor = DescriptorExtractor::create(desc_type);
			Ptr<DescriptorMatcher> cv_descriptorMatcher = DescriptorMatcher::create("BruteForce");
			cv_bow_img_desc_extractor = new BOWImgDescriptorExtractor(cv_descriptor, \
				cv_descriptorMatcher);
			cv_bow_img_desc_extractor->setVocabulary(dictionary);


			descriptors.clear();
			descriptors.resize(imgs.size());
			// detect keypoints first
			vector<cv_KeyPoints> keypoints;
			keypoint_detector.ExtractImgKeypoints(imgs, keypoints, keypoint_type);
			for(size_t i=0; i<imgs.size(); i++)
			{
				cv_bow_img_desc_extractor->compute(imgs[i], keypoints[i], descriptors[i]);
			}
		}

		// TODO: implement self-support extractor

	}


	void ImgDescriptorExtractor::ComputeBOWFromKeypointDescs(
		const Mat& point_descs, Mat& descriptor)
	{
		if(point_descs.empty())
			return;

		// convert to float type
		Mat convert_descs;
		point_descs.convertTo(convert_descs, CV_32FC1);


		vector<DMatch> matches;
		dict_generator.Match2Dictionary(convert_descs, matches, "FlannBased");
		descriptor.create(1, dict_generator.dictionary.cols, CV_32FC1);
		descriptor.setTo(0);
		for(size_t i=0; i<matches.size(); i++)
			descriptor.at<float>(0, matches[i].trainIdx)++;
	
		for(size_t i=0; i<descriptor.cols; i++)
		{
			descriptor.at<float>(0, i) /= point_descs.rows;
		}
	}


	bool ImgDescriptorExtractor::SaveImgDescriptor(const string& data_file, const Mat& descriptor)
	{
		ofstream out(data_file.c_str());
		if(out.fail())
		{
			cerr<<"Can't open file to save image descriptor."<<endl;
			return false;
		}
		if(descriptor.rows != 1)
		{
			cerr<<"Descriptor not a 1D row vector."<<endl;
			return false;
		}

		out<<descriptor.cols<<endl;	// output dimension
		for(int r=0; r<descriptor.rows; r++)
		{
			for(int c=0; c<descriptor.cols; c++)
				out<<(c==0? "": " ")<<descriptor.at<float>(r,c);
			out<<endl;
		}

		cout<<"Save image descriptor to "<<data_file<<endl;

		return true;
	}


	bool ImgDescriptorExtractor::LoadImgDescriptor(const string& data_file, Mat& descriptor)
	{
		ifstream in(data_file.c_str());
		if(in.fail())
		{
			cerr<<"Can't open file "<<data_file<<" to load image descriptor."<<endl;
			return false;
		}

		int dim;
		in>>dim;	// output dimension
		descriptor.release();
		descriptor.create(1, dim, CV_32FC1);
		for(int c=0; c<descriptor.cols; c++)
			in>>descriptor.at<float>(0,c);

		cout<<"Load image descriptor from "<<data_file<<endl;

		return true;
	}


}

