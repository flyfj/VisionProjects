//////////////////////////////////////////////////////////////////////////

#include "ClusteringTrees.h"

using namespace tools;


//#define DEBUG_CLUSTERTREE

namespace visualsearch
{

	ClusteringTree::ClusteringTree(void)
	{
		treeDepth = 7;
		canvas_size = Size(256, 125);
		cluster_alg = CLUSTER_KMEANS;
		ifBuilt = false;
	}

	ClusteringTree::~ClusteringTree(void)
	{
	}


	/*void ClusteringTree::BuildTree2Depth(const vector<Feature>& sampleSet)
	{

		if(sampleSet.empty())
			return;

		double stop_th = 0.5;

		//=== start to build clustering trees ===
		cout<<"Start building tree..."<<endl;

		// create root node
		TreeNode root;
		root.tree_node_id = 0;
		root.level = 0;

		// add all data to root node
		for(int i=0; i<sampleSet.size(); i++)
			root.member_ids.push_back(i);
		root.weight = 1;
		nodes[root.tree_node_id] = root;	// add to node collection

		int leaf_id = -1;

		double feat_dim = sampleSet[0].size();

		// start to grow tree
		for(int i=0; i<treeDepth; i++)	// grow tree for level i
		{
			int start_id = (int)pow(2.0,(double)i)-1;
			int node_num = (int)pow(2.0,(double)i);
			for(int j=0; j<node_num; j++)
			{
				// skip non-exist element
				if(nodes.find( start_id+j ) == nodes.end())
					continue;

				TreeNode& cur_node = nodes[start_id+j];
				if( cur_node.type == Leaf )
					continue;

				// create child nodes
				TreeNode lc, rc;
				lc.tree_node_id = 2*cur_node.tree_node_id+1;
				rc.tree_node_id = 2*cur_node.tree_node_id+2;
				cur_node.childIds.push_back( lc.tree_node_id );
				cur_node.childIds.push_back( rc.tree_node_id );
				lc.level = rc.level = i+1;
				if(lc.level == treeDepth)
					lc.type = Leaf;
				if(rc.level == treeDepth)
					rc.type = Leaf;

				// do splitting (clustering)
				Mat samples(cur_node.member_ids.size(), feat_dim, CV_32FC1);
				for(size_t i=0; i<cur_node.member_ids.size(); i++)
					for(size_t j=0; j<feat_dim; j++)
						samples.at<float>(i, j) = sampleSet[cur_node.member_ids[i]][j];

				Mat labels(cur_node.member_ids.size(), 1, CV_32SC1);

				if( cluster_alg == CLUSTER_KMEANS )
				{
					Mat centers(2, feat_dim, CV_32FC1);

					kmeans(samples, 2, labels,
						TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0f), 
						10, KMEANS_PP_CENTERS, centers);
				
					// compute cluster center
					for(int k=0; k<centers.cols; k++)
					{
						lc.center.push_back( centers.at<float>(0,k) );
						rc.center.push_back( centers.at<float>(1,k) );
					}
				}
				else	// GMM
				{
					//EMParams em_params(2, CvEM::COV_MAT_DIAGONAL, CvEM::START_AUTO_STEP,
					//	cvTermCriteria( CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, FLT_EPSILON ) );

					//// train 2 components GMM
					//EM gmm;
					//gmm.train(samples, Mat(), em_params, &labels);

					//// set centers
					//Mat means = gmm.get_means();
					//for(int k=0; k<means.cols; k++)
					//{
					//	lc.mean.push_back( means.at<double>(0,k) );
					//	rc.mean.push_back( means.at<double>(1,k) );
					//}
					//// set cov mat
					//const CvMat** covs = gmm.get_covs();
					//lc.cov = Mat(covs[0], true);
					//rc.cov = Mat(covs[1], true);

				}

				// if trivial split, stop growing
				if( hist_l2_dist(lc.center, rc.center) < stop_th )
				{
					cur_node.type = Leaf;
					continue;
				}

				// assign members
				for(int k=0; k<labels.rows; k++)
					if( labels.at<int>(k,0) == 0 )
						lc.member_ids.push_back(cur_node.member_ids[k]);
					else
						rc.member_ids.push_back(cur_node.member_ids[k]);

				// stopping criterion
				if( lc.member_ids.size() < 2 )
					lc.type = Leaf;
				if( rc.member_ids.size() < 2 )
					rc.type = Leaf;

				lc.weight = (double)lc.member_ids.size() / labels.rows;
				rc.weight = (double)rc.member_ids.size() / labels.rows;

				// compute max mean distance
				lc.max_mean_dist = 0;
				for(int k=0; k<lc.member_ids.size(); k++)
				{
					double dist = 0;
					if( cluster_alg == CLUSTER_KMEANS)
						dist = hist_l2_dist( lc.center, sampleSet[lc.member_ids[k]] );
					else
						dist = hist_l2_dist( lc.mean, sampleSet[lc.member_ids[k]] );

					if(dist > lc.max_mean_dist)
						lc.max_mean_dist = dist;
				}
				lc.max_mean_dist = MAX(0.0005, lc.max_mean_dist);	// avoid divided by 0
				rc.max_mean_dist = 0;
				for(int k=0; k<rc.member_ids.size(); k++)
				{
					double dist = 0;
					if( cluster_alg == CLUSTER_KMEANS )
						dist = hist_l2_dist( rc.center, sampleSet[rc.member_ids[k]] );
					else
						dist = hist_l2_dist( rc.mean, sampleSet[rc.member_ids[k]] );

					if(dist > rc.max_mean_dist)
						rc.max_mean_dist = dist;
				}
				rc.max_mean_dist = MAX(0.0005, rc.max_mean_dist);	// avoid divided by 0

				// add to tree
				if( !lc.member_ids.empty() )	// has member reached
					nodes[lc.tree_node_id] = lc;
				if( !rc.member_ids.empty() )
					nodes[rc.tree_node_id] = rc;

				cout<<"Grown "<<i+1<<" level."<<endl;

				cur_node.ifDone = true;
			}
		}

		for( map<int, TreeNode>::iterator pi=nodes.begin(); pi!=nodes.end(); pi++ )
			if(pi->second.type == Leaf)
			{
				leaf_id++;
				pi->second.word_id = leaf_id;
				leaf_keys.push_back(pi->second.tree_node_id);
			}

		cout<<"Finish building tree."<<endl;

	}*/


	bool ClusteringTree::BuildFullTree(
		const vector<VisualObject>& sampleSet, const ClusteringStopCriterion& stopCrit)
	{

		if(sampleSet.empty())
		{
			cerr<<"No input objects for building clustering tree."<<endl;
			return false;
		}


		//=== start to build clustering trees ===
		cout<<"Start building tree for category: "<<sampleSet[0].categoryName<<endl;

		// create root node
		TreeNode root;
		root.tree_node_id = "0";
		root.level = 0;

		// add all data to root node
		for(int i=0; i<sampleSet.size(); i++)
			root.member_ids.push_back(i);
		root.weight = 1;
		nodes[root.tree_node_id] = root;	// add to node collection

		int leaf_id = -1;
		cout<<sampleSet[0].visual_desc.img_desc.cols<<endl;
		double feat_dim = 1000;	// todo: modify feature dim

		// tree growing queue
		list<string> toBuildNodes;
		toBuildNodes.push_back(root.tree_node_id);

		vector<string> ids;
		char str[100];

		// start to grow tree
		int level_count = 0;
		double tree_id = 0;
		while( !toBuildNodes.empty() )
		{

			string start_id = toBuildNodes.front();
			toBuildNodes.pop_front();

			TreeNode& cur_node = nodes[start_id];
			if(cur_node.type == Leaf)
				continue;

			// create child nodes
			level_count++;

			TreeNode lc, rc;
			tree_id++;
			sprintf(str, "%f", tree_id);
			lc.tree_node_id = string(str);
			ids.push_back(lc.tree_node_id);
			tree_id++;
			sprintf(str, "%f", tree_id);
			rc.tree_node_id = string(str);
			ids.push_back(rc.tree_node_id);

			cur_node.childIds.push_back( lc.tree_node_id );
			cur_node.childIds.push_back( rc.tree_node_id );
			lc.level = rc.level = level_count;

			// do splitting (clustering)
			if(cur_node.member_ids.size() == 2)
			{
				// no need for clustering, directly assign
				// must assign center matrix too!
				lc.member_ids.push_back(cur_node.member_ids[0]);
				sampleSet[cur_node.member_ids[0]].visual_desc.img_desc.copyTo(lc.centerMat);
				rc.member_ids.push_back(cur_node.member_ids[1]);
				sampleSet[cur_node.member_ids[1]].visual_desc.img_desc.copyTo(rc.centerMat);
			}
			else
			{
				Mat samples;

				// copy image feature to samples
				// loop each sample in current node
				// in case empty feature
				vector<int> validMemberIds;
				for(size_t i=0; i<cur_node.member_ids.size(); i++)
				{
					int cur_samp_id = cur_node.member_ids[i];
					if(sampleSet[cur_samp_id].visual_desc.img_desc.empty())
						continue;

					samples.push_back( sampleSet[cur_samp_id].visual_desc.img_desc );
					validMemberIds.push_back(cur_samp_id);
				}

				Mat labels;

				if( cluster_alg == CLUSTER_KMEANS )
				{
					Mat centers(2, feat_dim, CV_32FC1);

					kmeans(samples, 2, labels,
						TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0f), 
						2, KMEANS_RANDOM_CENTERS, centers);

					// !ERR: resulting assignment may not be consistent in distance measure!
					// assign cluster center
					/*for(int k=0; k<centers.cols; k++)
					{
						lc.center.push_back( centers.at<float>(0,k) );
						rc.center.push_back( centers.at<float>(1,k) );
					}*/

					centers.row(0).copyTo(lc.centerMat);
					centers.row(1).copyTo(rc.centerMat);
				}
				else	// GMM
				{

				}


				// assign members based on distance directly not label (not consistent with our distance measure, don't know why)
				/*for(int k=0; k<samples.rows; k++)
				{

					float ldist = norm(samples.row(k), lc.centerMat);
					float rdist = norm(samples.row(k), rc.centerMat);
					if(ldist < rdist)
					{
						lc.member_ids.push_back(cur_node.member_ids[k]);
					}
					else
						rc.member_ids.push_back(cur_node.member_ids[k]);
				}*/
				
				for(int k=0; k<labels.rows; k++)
				{
					if( labels.at<int>(k) == 0 )
					{
						lc.member_ids.push_back(validMemberIds[k]);
					}
					else
					{
						rc.member_ids.push_back(validMemberIds[k]);
					}
				}


				// compute relative weights
				lc.weight = (double)lc.member_ids.size() / labels.rows;
				rc.weight = (double)rc.member_ids.size() / labels.rows;

				
#ifdef DEBUG_CLUSTERTREE

				// display clustered object images for visual look
				cout<<"Showing 1st cluster objects of node - level: "<<lc.level<<" id: "<<lc.tree_node_id<<endl;
				for(size_t k=0; k<lc.member_ids.size(); k++)
				{
					Mat img = imread(sampleSet[lc.member_ids[k]].imgfile.c_str());
					imshow("obj", img);
					waitKey(0);
				}

				cout<<"Showing 2nd cluster objects of node - level: "<<rc.level<<" id: "<<rc.tree_node_id<<endl;
				for(size_t k=0; k<rc.member_ids.size(); k++)
				{
					Mat img = imread(sampleSet[rc.member_ids[k]].imgfile.c_str());
					imshow("obj", img);
					waitKey(0);
				}

#endif

			}


			// stopping criterion and save leaf objects
			if( lc.member_ids.size() <= stopCrit.min_sample_num )
			{
				lc.type = Leaf;
				lc.ifDone = true;
				for(size_t k=0; k<lc.member_ids.size(); k++)
					lc.leaf_objs.push_back( sampleSet[lc.member_ids[k]] );
			}
			if( rc.member_ids.size() <= stopCrit.min_sample_num )
			{
				rc.type = Leaf;
				rc.ifDone = true;
				for(size_t k=0; k<rc.member_ids.size(); k++)
					rc.leaf_objs.push_back( sampleSet[rc.member_ids[k]] );
			}

			// add to tree
			nodes[lc.tree_node_id] = lc;
			nodes[rc.tree_node_id] = rc;

			// add to queue for growing if not leaf
			if( !lc.ifDone )
			{
				toBuildNodes.push_back(lc.tree_node_id);
			}
			if( !rc.ifDone )
			{
				toBuildNodes.push_back(rc.tree_node_id);
			}
			

			// finish, remove current node
			cur_node.ifDone = true;

		}

		// save leaf node ids
		for( map<string, TreeNode>::iterator pi=nodes.begin(); pi!=nodes.end(); pi++ )
			if(pi->second.type == Leaf)
			{
				leaf_id++;
				pi->second.word_id = leaf_id;
				leaf_keys.push_back(pi->second.tree_node_id);
			}

		cout<<"Finish building clustering tree."<<endl;
		ifBuilt = true;

		return true;

	}


	float ClusteringTree::TravelTree(const Mat& matfeat, string& id)
	{	

		if(matfeat.empty() || matfeat.rows != 1)
		{
			cerr<<"Wrong input query feature input."<<endl;
			return -1;
		}

		if(!ifBuilt)
		{
			cerr<<"Build clustering tree first before doing query."<<endl;
			return -1;
		}

		// feed the data into the tree and travel
		string next_id = "0";
		int count = 0;
		double score = 0;
		double tweight = 0;
		double ddist = 0;

		while(1)
		{
			TreeNode& cur_node = nodes[next_id];

			// stop testing
			if( cur_node.type == Leaf || cur_node.childIds.empty() )
				break;

			// find closer child
			TreeNode& lc = nodes[cur_node.childIds[0]];
			TreeNode& rc = nodes[cur_node.childIds[1]];
			double dist_l, dist_r;
			if( cluster_alg == CLUSTER_KMEANS )
			{
				dist_l = norm(lc.centerMat, matfeat, NORM_L2);
				dist_r = norm(rc.centerMat, matfeat, NORM_L2);
			}
			else
			{
				// gmm
			}
		
			if( dist_l < dist_r )	// assign to left child
			{
				ddist = dist_l;
				next_id = lc.tree_node_id;
			}
			else
			{
				ddist = dist_r;
				next_id = rc.tree_node_id;
			}

		}

		id = next_id;
		return ddist;
	}


	bool ClusteringTree::DrawLeafCenters(cv::Mat &canvas, int draw_num)
	{
		if(draw_num == -1)	// display all clusters
			draw_num = leaf_keys.size();

		if( leaf_keys.empty() || leaf_keys.size() < draw_num )
			return false;

		int total_num = draw_num;
		int cols = 5;	// restrict to 3 rows
		int rows = total_num / cols;

		char str[30];
		Mat subplot;
		canvas.create(rows*canvas_size.height, cols*canvas_size.width, CV_8UC3);
		int count = -1;
		Rect box;
		box.width = canvas_size.width;
		box.height = canvas_size.height;
		for(int y=0; y<rows; y++)
		{
			for(int x=0; x<cols; x++)
			{
				box.x = x*canvas_size.width;
				box.y = y*canvas_size.height;
				count++;
				/*if( cluster_alg == CLUSTER_KMEANS )
					DrawHist(subplot, canvas_size, 5, nodes[leaf_keys[count]].center);
				else
					DrawHist(subplot, canvas_size, 5, nodes[leaf_keys[count]].mean);*/

				// draw weight
				/*sprintf(str, "%d: %.5f", count, clusters[count].weight);
				putText(subplot, string(str), Point(20, 20), \
					FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 0, 0), 1.5);*/

				// add to whole image
				subplot.copyTo( canvas(box) );
			}
		}

		return true;

	}


	bool ClusteringTree::SaveTree(const string& file)
	{
		ofstream out(file.c_str());
		if(out.fail())
		{
			cerr<<"Error saving clustering tree."<<endl;
			return false;
		}

		// output model type
		out<<"CLUSTERINGTREE"<<endl;


		//////////////////////////////////////////////////////////////////////////
		// only need to save information useful for searching
		//////////////////////////////////////////////////////////////////////////
		// save nodes information
		// size
		out<<nodes.size()<<endl;
		for(map<string, TreeNode>::iterator pi=nodes.begin(); pi!=nodes.end(); pi++)
		{
			const TreeNode& curNode = pi->second;
			// id type
			out<<curNode.tree_node_id<<" "<<curNode.type<<endl;
			// child ids
			out<<curNode.childIds.size();
			for(size_t i=0; i<curNode.childIds.size(); i++)
				out<<" "<<curNode.childIds[i];
			out<<endl;
			// center mat
			out<<curNode.centerMat.cols;
			for(size_t i=0; i<curNode.centerMat.cols; i++)
				out<<" "<<curNode.centerMat.at<float>(0, i);
			out<<endl;
			// if leaf, leaf objects per line: imgname, bbox, keypoints
			if(curNode.type == Leaf)
			{
				out<<curNode.leaf_objs.size()<<endl;
				for(size_t i=0; i<curNode.leaf_objs.size(); i++)
				{
					out<<curNode.leaf_objs[i].imgfile<<endl;
					out<<curNode.leaf_objs[i].visual_desc.bbox.x<<" "
						<<curNode.leaf_objs[i].visual_desc.bbox.y<<" "
						<<curNode.leaf_objs[i].visual_desc.bbox.width<<" "
						<<curNode.leaf_objs[i].visual_desc.bbox.height<<endl;
					out<<curNode.leaf_objs[i].visual_desc.pts.size();
					for(size_t j=0; j<curNode.leaf_objs[i].visual_desc.pts.size(); j++)
					{
						out<<" "<<curNode.leaf_objs[i].visual_desc.pts[j].pt.x<<" "
							<<curNode.leaf_objs[i].visual_desc.pts[j].pt.y;
					}
					out<<endl;
				}
			}
		}


		cout<<"Clustering tree saved to "<<file<<endl;

		return true;
	}

	bool ClusteringTree::LoadTree(const string& file)
	{

		ifstream in(file.c_str());
		if(in.fail())
		{
			cerr<<"Error open file "<<file<<endl;
			return false;
		}

		string name;
		in>>name;

		// load nodes information
		nodes.clear();
		int sz, num, type;
		// size
		in>>sz;
		for(int pi=0; pi<sz; pi++)
		{
			// id type
			in>>name>>type;
			nodes[name].tree_node_id = name;
			nodes[name].type = (type==0? Internal: Leaf);
			// child ids
			in>>num;
			nodes[name].childIds.resize(num);
			for(size_t i=0; i<num; i++)
				in>>nodes[name].childIds[i];
			// center mat
			in>>num;
			if(num==0)
				cout<<""<<endl;
			nodes[name].centerMat.create(1, num, CV_32F);
			for(size_t i=0; i<num; i++)
				in>>nodes[name].centerMat.at<float>(0, i);
			// if leaf, leaf objects per line: imgname, bbox, keypoints
			if(nodes[name].type == Leaf)
			{
				in>>num;
				nodes[name].leaf_objs.resize(num);
				for(size_t i=0; i<num; i++)
				{
					in>>nodes[name].leaf_objs[i].imgfile;
					in>>nodes[name].leaf_objs[i].visual_desc.bbox.x
						>>nodes[name].leaf_objs[i].visual_desc.bbox.y
						>>nodes[name].leaf_objs[i].visual_desc.bbox.width
						>>nodes[name].leaf_objs[i].visual_desc.bbox.height;
					int ptsnum;
					in>>ptsnum;
					nodes[name].leaf_objs[i].visual_desc.pts.resize(ptsnum);
					for(size_t j=0; j<ptsnum; j++)
					{
						in>>nodes[name].leaf_objs[i].visual_desc.pts[j].pt.x
							>>nodes[name].leaf_objs[i].visual_desc.pts[j].pt.y;
					}
				}
			}
		}

		ifBuilt = true;

		cout<<"Clustering tree load from "<<file<<endl;

		return true;
	}


}
