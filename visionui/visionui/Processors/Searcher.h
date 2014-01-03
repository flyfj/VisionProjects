//////////////////////////////////////////////////////////////////////////
// generic search/indexing processor
// build upon other search structures and algorithms
// 
// 2012.10.27 @ Jie Feng
//////////////////////////////////////////////////////////////////////////


#pragma once

#include "common_libs.h"
#include "ClusteringTrees.h"


namespace visualsearch
{
	using namespace cv;
	using namespace flann;


	struct SearchResults 
	{
		// result images/objects
		vector<VisualObject> res_objs;
		Mat res_obj_ids;
		// distance
		Mat dists;
	};

	class Searcher
	{

	private:
		
		ClusteringTree searchTree;
		Index flannSearcher;
		FlannBasedMatcher flannMatcher;
		
		vector<VisualObject> database_objs;	// object meta data
		Mat database_data;	// descriptors for all database objects

	public:

		Searcher(void);

		bool SaveSearcher(const string& save_file);
		bool LoadSearcher(const string& load_file);
	
		// build search engine
		bool Build(vector<VisualObject>& objs);
		// search for an instance: return distance to best match
		SearchResults Search(const VisualObject& query, int id);
		// search for individual unit: image/object
		SearchResults SearchUnit(const VisualObject& query, int id);
		void ResObj2ResImg(const vector<SearchResults>& res);

	};
}



