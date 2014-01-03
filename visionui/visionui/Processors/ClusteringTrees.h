//////////////////////////////////////////////////////////////////////////
// clustering tree tool
// fengjie@pku
// 2012/3/14

#pragma once


#include "common_libs.h"
#include "Tools.h"
#include <list>
#include <queue>
using namespace std;


namespace visualsearch
{

	// type definitions
	enum ClusterMethod
	{
		CLUSTER_KMEANS, CLUSTER_EM
	};

	enum TreeNodeType
	{
		Internal, Leaf
	};


	struct ClusteringStopCriterion
	{
		int min_sample_num;

		ClusteringStopCriterion(): min_sample_num(1) {}
	};

	struct TreeNode
	{

		// data
		string tree_node_id;	// node id in the tree
		int word_id;	// event word id
		int level;	// tree level, root as level 0
		double weight;
		Feature center;
		Mat centerMat;	// mat format of center feature
		vector<int> member_ids;	// sample id in sample set
		TreeNodeType type;
		vector<VisualObject> leaf_objs;
		bool ifDone;
		double max_mean_dist;


		// GMM params
		Feature mean;
		Mat cov;

		// child ids
		vector<string> childIds;


		TreeNode(): level(0), tree_node_id(""), weight(0), ifDone(false), word_id(-1) 
		{  type = Internal; }


		double ComputeLikelihood(const Feature& sample);

	};


	class ClusteringTree
	{

	public:

		// tree data: a tree is started from index 0
		map<string, TreeNode> nodes;	// used to store all tree nodes indexed by identical id (time string)
		int treeDepth;
		vector<string> leaf_keys;	// tree node id for i_th word (leaf)
		bool ifBuilt;

		// params
		Size canvas_size;
		ClusterMethod cluster_alg;

	public:

		ClusteringTree(void);
		~ClusteringTree(void);

		// build until reach defined depth
		void BuildTree2Depth(const vector<Feature>& sampleSet);
		// build full tree
		bool BuildFullTree(const vector<VisualObject>& sampleSet, const ClusteringStopCriterion& stopCrit);
		// find corresponding leaf of input feature, return leaf id in the tree
		float TravelTree(const Mat& matfeat, string& id);
		bool DrawLeafCenters(Mat& canvas, int draw_num);

		// interface
		bool SaveTree(const string& file);
		bool LoadTree(const string& file);

	};

}


