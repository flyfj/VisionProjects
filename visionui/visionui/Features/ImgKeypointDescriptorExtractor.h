//////////////////////////////////////////////////////////////////////////
//	wrap class for extracting keypoint descriptor from image or object
//////////////////////////////////////////////////////////////////////////


#pragma once

#include "common_libs.h"


namespace visualsearch
{


	class ImgKeypointDescriptorExtractor
	{

	private:

		TypeConfig descriptor_typeconfig;
		// opencv wrapper
		Ptr<DescriptorExtractor> cv_descriptorExtractor;

	public:

		ImgKeypointDescriptorExtractor(void);
		~ImgKeypointDescriptorExtractor(void);


		// main entry
		void InitKeypointDescriptorTypes();
		inline bool CheckTypeValidity(const string& desc_type, string& support_type);
		void ExtractImgKeypointDescriptors(
			const cv_Images& imgs, vector<cv_KeyPoints>& keypoints, vector<Mat>& descriptors, const string& desc_type);
	
		// general interface for usual processing
		//void ExtractAllPointDescriptors(const cv_Images& imgs, map<int,FeaturePointsPerImg>& all_img_feats);
		bool LoadDescriptor(const string& datafile, cv_KeyPoints& keypoints, Mat& desc);
		// used for old surf descriptors
		bool LoadSurfDescriptor(const string& datafile, cv_KeyPoints& keypoints, Mat& desc);
		bool SaveDescriptors(const string& outfile, const cv_KeyPoints& points, const Mat& descs);
	
	};



}

