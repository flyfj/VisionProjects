//////////////////////////////////////////////////////////////////////////
// Search image based on salient objects
// 2012.11.14 Jie Feng
//////////////////////////////////////////////////////////////////////////


#pragma once

#include "common_libs.h"
#include "Tools.h"
#include "ImgKeypointDetector.h"
#include "ImgKeypointDescriptorExtractor.h"
#include "DescriptorDictionaryGenerator.h"
#include "ImgDescriptorExtractor.h"
#include "ObjCategorizer.h"
#include "Searcher.h"
#include <list>
#include <algorithm>
#include <time.h>
#include <assert.h>

using namespace tools;


namespace visualsearch
{

	enum TaskType
	{
		SAVE, LOAD
	};

	class MainEngine
	{

	public:
		// sign
		bool m_bUseSURF;	// use existing surf data
		TaskType task_TrainingDataInfo;
		bool m_bUseSalientObjects;
		TaskType task_KeyDescriptors;	// load keypoint descriptors (per image)
		TaskType task_BOW;	// load image or objects' bow descriptors
		TaskType task_Dicts;
		bool m_bUseUniversalDict;
		bool m_bUseUniversalSearcher;
		TaskType task_Searcher;

		/*
			params
		*/
		// input
		string dataset_dir;
		string img_dir;
		string txt_dir;
		string product_category_list_file;	// list of training category names
		string training_objects_info_file;	// list of training files
		string salientObj_dir;
		string surfData_dir;
		// results
		string results_dir;
		string res_desc_dir;	// extracted descriptors dir
		string res_bow_dir;
		// model
		string res_model_dir;
		string res_dict_dir;
		string global_dict_file;
		string global_searcher_file;
		int dict_size;

		/*
			data
		*/
		// training data
		ImageObjectCategories training_objects;
		ImageObjectCategories query_objects;
		vector<string> trainingFiles;
		vector<Mat> dicts;
		vector<Searcher> obj_searchers;

		// processors
		ImgKeypointDetector keypoint_detector;
		ImgKeypointDescriptorExtractor desc_extractor;
		vector<DescriptorDictionaryGenerator> dict_generators;
		ImgDescriptorExtractor img_descriptor_generator;
		ObjCategorizer obj_categorizer;
		

	public:
		MainEngine(void);


		// prepare database data
		// have category info
		void PrepareCategoryDatabaseForTraining();
		// no category info
		void PrepareDatabaseForTraining();


		// train engine: get all model data ready - directly generate or load from files
		bool Train();

		// search; return result for each query object/single image
		vector<SearchResults> Search(const ImageObjects& query_image);

		void TestSearch();
		
	private:

		/************************************************************************/
		/* Tool functions                                                       */
		/************************************************************************/
		bool SaveImageObjects(const string& file, const ImageObjects& imgObjs);
		bool LoadImageObjects(const string& file, ImageObjects& imgObjs);

	};

}


