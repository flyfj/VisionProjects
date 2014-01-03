//////////////////////////////////////////////////////////////////////////

#include "Searcher.h"


namespace visualsearch
{

	Searcher::Searcher(void)
	{
	}


	bool Searcher::Build(vector<VisualObject>& objs)
	{
		if(objs.empty())
		{
			cerr<<"Empty objects input for building searcher."<<endl;
			return false;
		}

		// accumulate samples
		// set up object meta info
		database_objs.resize(objs.size());
		for(size_t i=0; i<objs.size(); i++)
		{
			database_objs[i] = objs[i];
			database_objs[i].visual_desc.img_desc.release();
			database_objs[i].visual_desc.descs.release();
			database_data.push_back( objs[i].visual_desc.img_desc );
		}

		// accumulate samples
		/*ClusteringStopCriterion crit;
		searchTree.BuildFullTree(objs, crit);*/

		// build index
		KMeansIndexParams index_params;
		flannSearcher.build(database_data, index_params);
		
		/*flannMatcher.add(vector<Mat>(1, database_data));
		flannMatcher.train();*/

		return true;
	}


	SearchResults Searcher::Search(const VisualObject& query, int id)
	{

		SearchResults res;
		/*string leaf_id;
		float dist = searchTree.TravelTree(query.visual_desc.img_desc, leaf_id);
		cout<<leaf_id<<endl;
		res.dists.push_back(dist);
		res.res_objs = searchTree.nodes[leaf_id].leaf_objs;*/

		if(database_data.empty())
			return res;

		int knn = MIN(5, database_data.rows);
		cout<<knn<<endl;
		res.res_obj_ids.create(1, knn, CV_32S);
		res.dists.create(1, knn, CV_32F);

		vector<vector<DMatch>> matches;
		flannMatcher.knnMatch(query.visual_desc.img_desc, database_data, matches, knn);
		cout<<matches.size()<<endl;
		//flannSearcher.knnSearch(query.visual_desc.img_desc, res.res_obj_ids, res.dists, knn);
		
		res.res_objs.clear();
		res.res_objs.resize(knn);
		for(size_t i=0; i<matches[0].size(); i++)
		{
			res.res_objs[i] = database_objs[matches[0][i].trainIdx];
		}

		return res;
	}


	void Searcher::ResObj2ResImg(const vector<SearchResults>& res)
	{

	}

	//////////////////////////////////////////////////////////////////////////

	bool Searcher::SaveSearcher(const string& save_file)
	{
		// save index data
		//flannSearcher.save(save_file);
		// save obj info
		string sfile = save_file + ".objs";
		ofstream out(sfile.c_str());
		cout<<database_objs.size()<<endl;
		for(size_t i=0; i<database_objs.size(); i++)
		{
			out<<database_objs[i].imgfile<<endl;
			out<<database_objs[i].visual_desc.bbox.x<<" "<<database_objs[i].visual_desc.bbox.y<<" "
				<<database_objs[i].visual_desc.bbox.width<<" "<<database_objs[i].visual_desc.bbox.height<<endl;
		}

		return true;
		//return searchTree.SaveTree(save_file);
	}

	bool Searcher::LoadSearcher(const string& load_file)
	{
		//return searchTree.LoadTree(load_file);

		// load indexing data
		flannSearcher.load(database_data, load_file);

		// save obj info
		string sfile = load_file + ".objs";
		ifstream in(sfile.c_str());
		int sz;
		in>>sz;
		database_objs.clear();
		database_objs.resize(sz);
		for(size_t i=0; i<database_objs.size(); i++)
		{
			in>>database_objs[i].imgfile;
			in>>database_objs[i].visual_desc.bbox.x>>database_objs[i].visual_desc.bbox.y
				>>database_objs[i].visual_desc.bbox.width>>database_objs[i].visual_desc.bbox.height;
		}

		return true;
	}
	

}

