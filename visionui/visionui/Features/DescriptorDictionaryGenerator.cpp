//////////////////////////////////////////////////////////////////////////


#include "DescriptorDictionaryGenerator.h"


namespace visualsearch
{
	DescriptorDictionaryGenerator::DescriptorDictionaryGenerator(void)
	{
		InitDictionaryTypes();
	}


	DescriptorDictionaryGenerator::~DescriptorDictionaryGenerator(void)
	{
	}

	void DescriptorDictionaryGenerator::InitDictionaryTypes()
	{
		/*
			supported codebook type
			kmeans, hierarchical kmeans (ref opencv)
		*/
		string opencv_dic_types[] = {"KMEANS", "HKMEANS"};
		const StringVector OPENCV_DICTIONARY_TYPES(opencv_dic_types, opencv_dic_types+2);
		dictionary_typeconfig["opencv"] = OPENCV_DICTIONARY_TYPES;
		
		// TODO: other dictionaries
	}

	bool DescriptorDictionaryGenerator::CheckTypeValidity(const string& dict_type, string& support_type)
	{
		support_type = "not_support";

		TypeConfig::const_iterator pi = dictionary_typeconfig.begin();
		for( ; pi != dictionary_typeconfig.end(); pi++ )
		{
			if( find(pi->second.begin(), pi->second.end(), dict_type) != pi->second.end() )
			{
				support_type = pi->first;
				return true;
			}
		}

		return false;
	}


	void DescriptorDictionaryGenerator::BuildDictionary(
		const Mat& descriptors, int dict_size, const string& dict_type)
	{
		// check validity first
		string support_type;
		bool if_valid = CheckTypeValidity(dict_type, support_type);
		if( !if_valid )
		{
			cerr<<"Not support input type."<<endl;
			return;
		}

		if(dict_size > descriptors.rows)
		{
			cerr<<"Dictionary size bigger than descriptor numbers."<<endl;
			return;
		}

		cout<<"Dictionary support type: "<<support_type<<endl;
		cout<<"Dictionary build type: "<<dict_type<<endl;

		// clear structures
		dictionary.release();
		// convert descriptors type
		Mat convert_descs;
		descriptors.convertTo(convert_descs, CV_32F);
		
		// do computation
		cout<<"Building dictionary..."<<endl;
		if( support_type == "opencv" )
		{
			if(dict_type == "KMEANS")
			{
				cv_bow_kmeans_trainer = new BOWKMeansTrainer(dict_size);
				dictionary = cv_bow_kmeans_trainer->cluster( convert_descs );
			}

			// TODO: other opencv type
			
		}
		
		// TODO: other self-support dictionary learning algorithm

		cout<<"Finish building dictionary."<<endl;

		BuildDictionaryIndex(dictionary);

	}


	float DescriptorDictionaryGenerator::Match2Dictionary(
		const Mat& descs, vector<DMatch>& matches, const string& matcher_type)
	{

		if(dictionary.empty())
		{
			cerr<<"Empty dictionary."<<endl;
			return false;
		}

		if(matcher_type != "BruteForce" && matcher_type != "FlannBased")
		{
			cerr<<"Wrong matcher type."<<endl;
			return false;
		}

		Ptr<DescriptorMatcher> cv_descriptorMatcher = DescriptorMatcher::create(matcher_type);
		cv_descriptorMatcher->match(descs, dictionary, matches);
		
		float sumd = 0;
		for(int i=0; i<matches.size(); i++)
			sumd += matches[i].distance;

		return sumd /= descs.rows;

	}


	bool DescriptorDictionaryGenerator::SaveDictionary(const string& outfile)
	{
		ofstream out(outfile.c_str());

		// save dictionary size
		out<<dictionary.rows<<endl;
		// save descriptor length
		out<<dictionary.cols<<endl;
		for(int i=0; i<dictionary.rows; i++)
		{
			for(int j=0; j<dictionary.cols; j++)
				out<<(j==0? "":" ")<<dictionary.at<float>(i,j);

			out<<endl;
		}

		cout<<"Save dictionary to "<<outfile<<endl;

		return true;

	}


	bool DescriptorDictionaryGenerator::LoadDictionary(const string& infile)
	{
		ifstream in(infile.c_str());
		if(in.fail())
		{
			cerr<<"Fail to load dictionary from "<<infile<<endl;
			return false;
		}

		int dict_size, dict_dim;
		in>>dict_size>>dict_dim;
		float val;

		// start from 5 for surf dict only
		dictionary.create(dict_size, dict_dim, CV_32F);
		for(int r=0; r<dict_size; r++)	
		{
			for(int c=0; c<dict_dim; c++)
				in>>dictionary.at<float>(r,c);
		}

		cout<<"Load dictionary from "<<infile<<endl;

		
		BuildDictionaryIndex(dictionary);

		return true;

	}


	bool DescriptorDictionaryGenerator::BuildDictionaryIndex(const Mat& dictionary)
	{
		// build dictionary matcher
		flann::KMeansIndexParams indexParam;
		flannMatcher.build(dictionary, indexParam);

		cout<<"Dictionary indexer built."<<endl;

		return true;
	}


}
