//////////////////////////////////////////////////////////////////////////
//	wrap class for descriptor codebook generation
//////////////////////////////////////////////////////////////////////////


#pragma once

#include "common_libs.h"


namespace visualsearch
{

	class DescriptorDictionaryGenerator
	{

	private:

		TypeConfig dictionary_typeconfig;
		Ptr<BOWKMeansTrainer> cv_bow_kmeans_trainer;
		flann::Index flannMatcher;
		
	public:

		Mat dictionary;


	public:
		
		DescriptorDictionaryGenerator(void);
		~DescriptorDictionaryGenerator(void);

		void InitDictionaryTypes();
		inline bool CheckTypeValidity(const string& dict_type, string& support_type);
		void BuildDictionary(const Mat& descriptors, int dict_size, const string& dict_type);
		bool BuildDictionaryIndex(const Mat& dictionary);
		void SetDictionary(const Mat& dict) { 
			dictionary = dict; 
			BuildDictionaryIndex(dictionary); 
		}

		// match a set of descriptor to dictionary and return sum of distance
		float Match2Dictionary(const Mat& descs, vector<DMatch>& matches, const string& matcher_type);
		
		// save and load
		bool LoadDictionary(const string& infile);
		bool SaveDictionary(const string& outfile);
		

	};

}
