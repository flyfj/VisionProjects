//////////////////////////////////////////////////////////////////////////

#include "ObjCategorizer.h"


namespace visualsearch
{

	ObjCategorizer::ObjCategorizer(void)
	{
		ifTrained = false;
		objCategorizer_type = "not init";
		InitCategorizerTypes();
	}


	ObjCategorizer::~ObjCategorizer(void)
	{
	}


	void ObjCategorizer::InitCategorizerTypes()
	{
		/*  
			supported categorizer/classifier type
		*/
		string categorizer_types[] = {"DTREE", "RTREES", "SVM"};
		CATEGORIZER_TYPES = StringVector(categorizer_types, categorizer_types+3);

		// TODO: add self-support or other 3rd party support detectors
	}

	bool ObjCategorizer::CheckTypeValidity(const string& type)
	{
		for(size_t i=0; i<CATEGORIZER_TYPES.size(); i++)
			if(type == CATEGORIZER_TYPES[i])
			{
				objCategorizer_type = type;
				return true;
			}

		return false;
	}


	void ObjCategorizer::Train(const Mat& samps, const Mat& labels, const string& categorizer_type)
	{
		
		bool if_valid = CheckTypeValidity(categorizer_type);
		if( !if_valid )
		{
			cerr<<"Not support input type."<<endl;
			return;
		}

		cout<<"Start to train "<<categorizer_type<<endl;
		if(categorizer_type == "DTREE")
		{

			// set params
			CvDTreeParams params(
				8, // max depth
				10, // min sample count
				0, // regression accuracy: N/A here
				false, // no missing data
				30, // max number of categories (use sub-optimal algorithm for larger numbers)
				10, // the number of cross-validation folds
				true, // use 1SE rule => smaller tree
				true, // throw away the pruned tree branches
				NULL
				);

			Mat var_type(samps.cols+1, 1, CV_8U);
			var_type.setTo(Scalar(CV_VAR_CATEGORICAL)); // all the variables are categorical

			bool flag = cv_dtree.train(samps, CV_ROW_SAMPLE, labels, Mat(), Mat(), var_type, Mat(), params);
			if( !flag )
				cerr<<"Train dtree fails."<<endl;
			else
				ifTrained = true;

		}
		if(categorizer_type == "RTREES")
		{
			CvRTParams params(
				10, // max depth
				10, // min_sample_count
				0, // regression_accurary
				false, // use_surrogates
				15, // max_categories
				0, // priors
				true, // calc_var_importance
				4, // nactive_vars
				100, // max_num_of_trees_in_the_forest
				0.01f, // forest_accuracy
				CV_TERMCRIT_ITER);


			Mat var_type(1, samps.cols+1, CV_8U);
			var_type.setTo(Scalar(CV_VAR_ORDERED)); // all the variables are categorical

			bool flag = cv_forest.train(
				samps, CV_ROW_SAMPLE, labels);
			if(!flag)
				cerr<<"Fail to train random forest."<<endl;
			else
			{
				ifTrained = true;
				cout<<"Finish training random forest classifier."<<endl;
			}

		}
		if(categorizer_type == "SVM")
		{
			if( cv_svm.SVMMultiTrain_OnevsAll(samps, labels) )
				ifTrained = true;
		}

		cout<<"Finish training categorizer."<<endl;
		
	}


	int ObjCategorizer::Predict(const Mat& samp, FloatVector& allscores)
	{
		if( !ifTrained )
		{
			cerr<<"Train categorizer first."<<endl;
			return -1;
		}

		if(objCategorizer_type == "DTREE")
		{
			Ptr<CvDTreeNode> leafnode = cv_dtree.predict(samp);
			return (int)leafnode->value;
		}
		if(objCategorizer_type == "RTREES")
		{
			return (int)cv_forest.predict(samp);
		}
		if(objCategorizer_type == "SVM")
		{
			if( cv_svm.SVMMultiPredict_AllScores(samp, allscores) )
			{
				double minval, maxval;
				int minIdx, maxIdx;
				minMaxIdx(allscores, &minval, &maxval, &minIdx, &maxIdx);
				return maxval;
			}
			else
				return -1;
		}

	}


	bool ObjCategorizer::SaveCategorizer(const string& model_info_file)
	{

		string model_dir;
		int id = model_info_file.find_last_of('\\');
		model_dir = model_info_file.substr(0, id);

		// write a model information file
		ofstream out(model_info_file.c_str());
		
		// type
		out<<objCategorizer_type<<endl;
		if(objCategorizer_type == "RTREES")
		{
			// model number
			out<<1<<endl;
			// model data file name, save in the same directory as model_info_files
			string modelfile = model_dir + "\\rtrees.model";
			out<<modelfile<<endl;
			cv_forest.save(modelfile.c_str());

			cout<<"Model has been saved to "<<modelfile<<endl;
		}
		if(objCategorizer_type == "SVM")
		{
			// model number
			out<<cv_svm.svms.size()<<endl;
			char str[20];
			for(size_t i=0; i<cv_svm.svms.size(); i++)
			{
				sprintf(str, "svm%d.model", i);
				string modelfile = model_dir + "\\" + string(str);
				out<<modelfile<<endl;
				cv_svm.svms[i].save(modelfile.c_str());
				cout<<"Model has been saved to "<<modelfile<<endl;
			}
		}

		return true;
	}

	bool ObjCategorizer::LoadCategorizer(const string& model_info_file)
	{

		ifstream in(model_info_file.c_str());
		if(in.fail())
		{
			cerr<<"Fail to load categorizer from "<<model_info_file<<endl;
			return false;
		}
		in>>objCategorizer_type;
		int model_num;
		in>>model_num;
		if(objCategorizer_type == "RTREES")
		{
			string model_file;
			in>>model_file;
			cv_forest.load(model_file.c_str());
			ifTrained = true;
		}
		if(objCategorizer_type == "SVM")
		{
			cv_svm.svms.resize(model_num);
			for(size_t i=0; i<model_num; i++)
			{
				string model_file;
				in>>model_file;
				cv_svm.svms[i].load(model_file.c_str());
			}
			ifTrained = true;
		}

		cout<<"Object categorizer loaded."<<endl;

		return true;
	}

}

