//////////////////////////////////////////////////////////////////////////
//	wrap class for object categorizer, general interface
//////////////////////////////////////////////////////////////////////////


#pragma once

#include "common_libs.h"
#include "ImgDescriptorExtractor.h"
#include "jsvm.h"


namespace visualsearch
{
	
	using namespace learners;

	/* actually a classifier (multi-class) */
	class ObjCategorizer
	{

	private:

		StringVector CATEGORIZER_TYPES;
		string objCategorizer_type;
		bool ifTrained;
		/* classifiers */
		// decision tree
		CvDTree cv_dtree;
		// random trees
		CvRTrees cv_forest;
		// svm
		OpenCVSVM cv_svm;

	public:

		ObjCategorizer(void);
		~ObjCategorizer(void);

		
		const string GetCategorizerType() { return objCategorizer_type; }
		void SetupImageCategorizer(const string& clf_type);
		void InitCategorizerTypes();
		bool CheckTypeValidity(const string& type);
		void Train(const Mat& samps, const Mat& labels, const string& categorizer_type);
		// compute classification score for all categories and return best class label
		int Predict(const Mat& samp, FloatVector& allscores);
		bool SaveCategorizer(const string& model_info_file);
		bool LoadCategorizer(const string& model_info_file);
		
	};

}