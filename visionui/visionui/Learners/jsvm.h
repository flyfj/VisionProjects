//////////////////////////////////////////////////////////////////////////
// svm interface
//	2012/6/13
//////////////////////////////////////////////////////////////////////////

#pragma once


#include "common_libs.h"
#include "libsvm\svm.h"
using namespace std;
using namespace visualsearch;



namespace learners
{

	class OpenCVSVM
	{
	private:

		StringVector OPENCV_SVM_TYPE;
		string model_path;
		bool ifTrained;

	public:

		vector<CvSVM> svms;

		OpenCVSVM();
		~OpenCVSVM();

		bool CheckTypeValidity(const string& svm_type);
		bool SVMBinaryTrain(const Mat& train_data, const Mat& train_labels);
		float SVMBinaryPredict(const Mat& samp, bool returnScore);
		bool SVMMultiTrain_OnevsAll(const Mat& train_data, const Mat& train_labels);
		bool SVMMultiPredict_AllScores(const Mat& samp, FloatVector& allClassScores);

	};

	//////////////////////////////////////////////////////////////////////////
	// interface to libsvm
	//////////////////////////////////////////////////////////////////////////
	// usage: model saved to a pre-defined file
	//class LibSVM
	//{
	//private:

	//	// structures
	//	svm_model *model;
	//	// file paths
	//	string program_path;
	//	string model_path;
	//	string origin_train_data_path;
	//	string scaled_train_data_path;


	//public:
	//	LibSVM();
	//	~LibSVM();

	//	void Init();

	//	bool Train(const FeatureSet& samps, const Feature& labels);
	//	bool Train(string train_file);

	//	double Predict(const Feature& samp);
	//	void Predict(string test_file);

	//	// helper function to save data to a file to be read by methods
	//	bool OutputTrainData(const FeatureSet& samps, const Feature& labels);

	//	// load model directly from file to be used for predicting
	//	bool LoadModelFromFile();

	//};

}
