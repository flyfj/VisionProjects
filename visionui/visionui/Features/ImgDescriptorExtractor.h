//////////////////////////////////////////////////////////////////////////
//	wrap class for extracting feature from image or object
//////////////////////////////////////////////////////////////////////////


#pragma once

#include "common_libs.h"
#include "ImgKeypointDetector.h"
#include "ImgKeypointDescriptorExtractor.h"
#include "DescriptorDictionaryGenerator.h"


namespace visualsearch
{


	/* 
		wrap class for BOW image descriptor generation
	*/
	class ImgDescriptorExtractor
	{

	private:

		TypeConfig img_descriptor_config;
		Ptr<BOWImgDescriptorExtractor> cv_bow_img_desc_extractor;

		// processors
		ImgKeypointDetector keypoint_detector;
		ImgKeypointDescriptorExtractor keypoint_descriptor;
		DescriptorDictionaryGenerator dict_generator;

	public:

		// init with a dict for bow feature
		ImgDescriptorExtractor(void);
		~ImgDescriptorExtractor(void);

		void Init(const Mat& dict) { dict_generator.SetDictionary(dict); }

		void SetDictionary(const Mat& dict) { dict_generator.SetDictionary(dict); }

		// extract image descriptor from scratch
		void ComputeBOWDescriptors(
			const cv_Images& imgs, const string& keypoint_type, const string& desc_type, 
			const Mat& dictionary, vector<Mat>& descriptors);

		// compute from existing keypoint descriptors
		void ComputeBOWFromKeypointDescs(
			const Mat& point_descs, Mat& descriptor);

		bool SaveImgDescriptor(const string& data_file, const Mat& descriptor);
		bool LoadImgDescriptor(const string& data_file, Mat& descriptor);

	};
}


