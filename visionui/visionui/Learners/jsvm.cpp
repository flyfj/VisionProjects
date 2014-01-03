//////////////////////////////////////////////////////////////////////////

#include "jsvm.h"


namespace learners
{

	OpenCVSVM::OpenCVSVM()
	{
		model_path = "opencvsvm.model";
		ifTrained = false;

		OPENCV_SVM_TYPE.push_back("C_SVC");
	}

	OpenCVSVM::~OpenCVSVM()
	{
		svms.clear();
	}



	bool OpenCVSVM::SVMBinaryTrain(const Mat& train_data, const Mat& train_labels)
	{
		if(train_data.rows == 0 || train_labels.rows == 0)
			return false;


		//Setting up SVM parameters
		CvSVMParams params;
		params.kernel_type = CvSVM::RBF;
		params.svm_type = CvSVM::C_SVC;
		params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 0.000001);

		svms.clear();
		svms.resize(1);
		// train svm
		bool flag = svms[0].train_auto(train_data, train_labels, 
			Mat(), Mat(), params, 10,
			CvSVM::get_default_grid(CvSVM::C), CvSVM::get_default_grid(CvSVM::GAMMA),
			CvSVM::get_default_grid(CvSVM::P), CvSVM::get_default_grid(CvSVM::NU),
			CvSVM::get_default_grid(CvSVM::COEF), CvSVM::get_default_grid(CvSVM::DEGREE));

		if(flag)
		{
			ifTrained = true;
			// save to file
			svms[0].save(model_path.c_str());
		}

		return flag;
	}


	float OpenCVSVM::SVMBinaryPredict( const Mat& samp, bool returnScore )
	{
		if(!ifTrained || svms.size() != 1)
		{
			cout<<"Train svm first."<<endl;
			return -2;
		}

		return svms[0].predict(samp, returnScore);	// return 2-class result

	}

	bool OpenCVSVM::SVMMultiTrain_OnevsAll(const Mat& train_data, const Mat& train_labels)
	{
		
		if(train_data.rows != train_labels.cols)	// assume labels are row vector
		{
			cerr<<"Training samples and labels are not consistent."<<endl;
			return false;
		}

		/* to train for each class, only need to manipulate labels 
			labels are 0,1...,N for N+1 classes
		*/

		double minval, maxval;
		minMaxIdx(train_labels, &minval, &maxval);

		svms.clear();
		svms.resize(maxval+1);
		CvSVMParams params;
		params.kernel_type = CvSVM::LINEAR;
		params.svm_type = CvSVM::C_SVC;
		params.C = 1;
		params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 100, 0.0001);

		// train for each class a one-versus-all svm
		Mat binaryLabels(train_labels.rows, train_labels.cols, train_labels.depth());
		for(int i=0; i<=maxval; i++)
		{

			int pos_num = 0;
			int neg_num = 0;
			binaryLabels.setTo(-1);
			for(int j=0; j<train_labels.cols; j++)
				if(train_labels.at<int>(0,j) == i)
				{
					binaryLabels.at<int>(0,j) = 1;
					pos_num++;
				}
			neg_num = train_labels.cols - pos_num;

			// train svm for ith object class
			cout<<"Training svm for "<<i<<"th class..."<<endl;
			cout<<"Positive samples: "<<pos_num<<" negative samples: "<<neg_num<<endl;

			bool flag = svms[i].train_auto(train_data, binaryLabels, 
				Mat(), Mat(), params, 10,
				CvSVM::get_default_grid(CvSVM::C), CvSVM::get_default_grid(CvSVM::GAMMA),
				CvSVM::get_default_grid(CvSVM::P), CvSVM::get_default_grid(CvSVM::NU),
				CvSVM::get_default_grid(CvSVM::COEF), CvSVM::get_default_grid(CvSVM::DEGREE));
			cout<<"Finish training."<<endl;

		}

		cout<<"SVM trained."<<endl;

		return true;

	}


	bool OpenCVSVM::SVMMultiPredict_AllScores(const Mat& samp, FloatVector& allClassScores)
	{

		if(samp.rows == 0)
		{
			cerr<<"No sample input."<<endl;
			return false;
		}
		if( svms.size() < 1)
		{
			cerr<<"SVM not trained."<<endl;
			return false;
		}

		allClassScores.clear();
		allClassScores.resize(svms.size(), -1);
		for(size_t i=0; i<svms.size(); i++)
		{
			allClassScores[i] = svms[i].predict(samp, true);
			cout<<allClassScores[i]<<endl;
		}

		return true;

	}


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	//LibSVM::LibSVM()
	//{
	//	model_path = "";
	//	origin_train_data_path = "";
	//	scaled_train_data_path = "";
	//	program_path = "";
	//}

	//LibSVM::~LibSVM()
	//{
	//	if(model!=NULL)
	//		svm_free_and_destroy_model(&model);
	//}

	//void LibSVM::Init()
	//{
	//	// set default paths
	//	origin_train_data_path = "train_data";
	//	scaled_train_data_path = origin_train_data_path + ".scale";
	//	model_path = "svm.model";
	//	program_path = "libs\\libsvm\\binary\\";

	//	// set data
	//	model = NULL;
	//}

	//bool LibSVM::OutputTrainData(const FeatureSet &samps, const Feature &labels)
	//{
	//	if(samps.size() != labels.size())
	//	{
	//		cout<<"training sample size not match label size."<<endl;
	//		return false;
	//	}

	//	// set training data file path
	//	origin_train_data_path = "train_data";
	//	ofstream out(origin_train_data_path.c_str());
	//	// data format: label index1:value1 index2:value2 ... (index starts from 1)
	//	for(size_t y=0; y<samps.size(); y++)
	//	{
	//		// loop each sample
	//		out<<labels[y];
	//		for(size_t x=0; x<samps[y].size(); x++)
	//			out<<" "<<x+1<<":"<<samps[y][x];
	//		out<<endl;
	//	}

	//	cout<<"Written training data to file: "<<origin_train_data_path<<endl;
	//	return true;

	//}

	//bool LibSVM::Train(string train_file)
	//{
	//	origin_train_data_path = train_file;
	//	scaled_train_data_path = origin_train_data_path + ".scale";

	//	// scale features
	//	string command = program_path + \
	//		"svm-scale -l -1 -u 1 -s range " + origin_train_data_path + " > " + scaled_train_data_path;
	//	int ret = system( command.c_str() );
	//	if( ret != 0 )
	//		cout<<"Error running svm-scale."<<endl;

	//	// train svm
	//	command = program_path + \
	//		"svm-train -s 0 -t 2 -c 100 -g 0.1 " + scaled_train_data_path + " " + model_path;
	//	ret = system( command.c_str() );
	//	if( ret != 0 )
	//		cout<<"Error running svm-train."<<endl;

	//	return true;
	//}

	//bool LibSVM::Train(const FeatureSet& samps, const Feature& labels)
	//{
	//	if(samps.empty() || labels.empty())
	//		return false;

	//	// train svm using binary
	//	// output training data to file
	//	bool flag = false;
	//	flag = OutputTrainData(samps, labels);
	//	if(!flag)
	//		return false;

	//	// scale features
	//	scaled_train_data_path = origin_train_data_path + ".scale";
	//	string command = program_path + \
	//		"svm-scale -l -1 -u 1 -s range " + origin_train_data_path + " > " + scaled_train_data_path;
	//	int ret = system( command.c_str() );
	//	if( ret != 0 )
	//		cout<<"Error running svm-scale."<<endl;

	//	// train svm
	//	command = program_path + \
	//		"svm-train -s 0 -t 2 -c 100 -g 0.1 " + scaled_train_data_path + " " + model_path;
	//	ret = system( command.c_str() );
	//	if( ret != 0 )
	//		cout<<"Error running svm-train."<<endl;

	//	return true;
	//}

	//bool LibSVM::LoadModelFromFile()
	//{
	//	model = svm_load_model(model_path.c_str());
	//	if(model == NULL)
	//	{
	//		cout<<"Failed to load model."<<endl;
	//		return false;
	//	}

	//	return true;
	//}

	//void LibSVM::Predict(string test_file)
	//{
	//	if(model == NULL)
	//	{
	//		// load model
	//		bool flag = LoadModelFromFile();
	//		if(!flag)
	//			return;
	//	}

	//	string command = program_path + \
	//		"svm-predict " + test_file + " " + model_path + " test_res";
	//	int ret = system( command.c_str() );
	//	if( ret != 0 )
	//		cout<<"Error running svm-predict."<<endl;
	//	else
	//		cout<<"Prediction results saved to: test_res"<<endl;

	//}

	//double LibSVM::Predict(const Feature &samp)
	//{
	//	if(samp.empty())
	//	{
	//		cout<<"Invalid input test sample (empty)."<<endl;
	//		return -2;
	//	}
	//	if(model == NULL)
	//	{
	//		bool flag = LoadModelFromFile();
	//		if(!flag)
	//			return -2;
	//	}

	//	Feature tsamp = samp;
	//	// scale
	//	if(scaled_train_data_path != "")
	//	{
	//		ofstream out("test_data");
	//		out<<1;
	//		for(size_t i=0; i<samp.size(); i++)
	//			out<<" "<<i+1<<":"<<samp[i];

	//		string command = program_path + \
	//			"svm-scale -r range test_data > test.scale";
	//		int ret = system( command.c_str() );
	//		if(ret != 0)
	//		{
	//			cout<<"Error running svm-scale."<<endl;
	//			return -2;
	//		}

	//		ifstream in("test.scale");
	//		float idx, val;
	//		char comma;
	//		in>>idx;
	//		while(in>>idx>>comma>>val)
	//			tsamp[idx-1] = val;
	//	}

	//	// convert format
	//	svm_node *test_samp = new svm_node[tsamp.size()+1];
	//	for(int i=0; i<tsamp.size(); i++)
	//	{
	//		test_samp[i].index = i+1;
	//		test_samp[i].value = tsamp[i];
	//	}
	//	test_samp[tsamp.size()].index = -1;

	//	// do prediction
	//	int nclass = svm_get_nr_class(model);
	//	double *dec_values = new double[nclass*(nclass-1)/2];
	//	double val = svm_predict_values(model, test_samp, dec_values);

	//	// clean
	//	delete []test_samp;
	//	test_samp = NULL;
	//	delete []dec_values;
	//	dec_values = NULL;

	//	return val;
	//}

}
