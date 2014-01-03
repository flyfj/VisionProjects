//////////////////////////////////////////////////////////////////////////
//	wrap class for extracting keypoint from image or object
//////////////////////////////////////////////////////////////////////////


#pragma once

#include "common_libs.h"


namespace visualsearch
{

	// maybe need to define a generic keypoint structure
	// to be compatible with structs from other library


	class ImgKeypointDetector
	{

	private:

		TypeConfig detector_typeconfig;
		// opencv wrapper
		Ptr<FeatureDetector> cv_featureDetector;

	public:

		ImgKeypointDetector(void);
		~ImgKeypointDetector(void);
	
	
		// main entry
		void InitDetectorTypes();
		inline bool CheckTypeValidity(const string& keypoint_type, string& support_type);
		void ExtractImgKeypoints(
			const cv_Images& imgs, vector<cv_KeyPoints>& keypoints, const string& keypoint_type);
	};



}
