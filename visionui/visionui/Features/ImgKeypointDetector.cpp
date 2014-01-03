//////////////////////////////////////////////////////////////////////////


#include "ImgKeypointDetector.h"


namespace visualsearch
{


	ImgKeypointDetector::ImgKeypointDetector(void)
	{
		InitDetectorTypes();
	}


	ImgKeypointDetector::~ImgKeypointDetector(void)
	{
	}

	void ImgKeypointDetector::InitDetectorTypes()
	{
		/*  
			supported feature detector type
			FAST, SIFT, SURF, ORB, MSER, STAR etc (ref opencv)
			later could split to opencv and self
		*/
		string opencv_feat_types[] = {"FAST", "SIFT", "SURF", "ORB"};
		const StringVector OPENCV_FEATURE_DETECTOR_TYPES(opencv_feat_types, opencv_feat_types+4);
		detector_typeconfig["opencv"] = OPENCV_FEATURE_DETECTOR_TYPES;

		initModule_nonfree();
		// TODO: add self-support or other 3rd party support detectors
	}

	bool ImgKeypointDetector::CheckTypeValidity(const string& keypoint_type, string& support_type)
	{
		support_type = "not_support";

		TypeConfig::const_iterator pi = detector_typeconfig.begin();
		for( ; pi != detector_typeconfig.end(); pi++ )
		{
			if( find(pi->second.begin(), pi->second.end(), keypoint_type) != pi->second.end() )
			{
				support_type = pi->first;
				return true;
			}
		}

		return false;
	}


	void ImgKeypointDetector::ExtractImgKeypoints(
		const cv_Images& imgs, vector<cv_KeyPoints>& keypoints, const string& keypoint_type)
	{
		// check validity first
		string support_type;
		bool if_valid = CheckTypeValidity(keypoint_type, support_type);
		if( !if_valid )
		{
			cerr<<"Not support input type."<<endl;
			return;
		}
		
		cout<<"Keypoint detector support type: "<<support_type<<endl;

		// clear structures
		keypoints.clear();
		keypoints.resize(imgs.size());

		// do detection
		cout<<"Detecting keypoints..."<<endl;
		if( support_type == "opencv" )
		{
			cv_featureDetector = FeatureDetector::create(keypoint_type);
			if(cv_featureDetector == NULL)
				return;

			// do specific parameter settings here
			if(keypoint_type == "FAST")
				cv_featureDetector->set("threshold", 10);
			if(keypoint_type == "SURF")
				cv_featureDetector->set("hessianThreshold", 400);

			cv_featureDetector->detect(imgs, keypoints);
		}
		// TODO: add other detectors


		cout<<"Finish detecting keypoints."<<endl;
	}

}