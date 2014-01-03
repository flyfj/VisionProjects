//////////////////////////////////////////////////////////////////////////
// Define commonly used stuff
//	Jie Feng @ 2012.10
//////////////////////////////////////////////////////////////////////////

#pragma once

#include <math.h>
#include <vector>
#include <string>
#include <list>
#include <algorithm>
#include <map>
#include <fstream>
// added for .net platform; remove managed for regular win app
#pragma managed(push, off)
#include <opencv2/opencv.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#pragma managed(pop)
using namespace std;
using namespace cv;


namespace visualsearch
{
	//////////////////////////////////////////////////////////////////////////
	//	structures
	typedef vector<double> Feature;	// single multivariate feature vector
	typedef vector<Feature> FeatureSet;	// a set of features
	typedef vector<string> StringVector;
	typedef vector<float> FloatVector;
	typedef vector<int> IntVector;
	typedef list<string> StringList;
	typedef vector<Mat> cv_Images;
	typedef KeyPoint cv_KeyPoint;
	typedef vector<KeyPoint> cv_KeyPoints;


	/*
		image visual feature related information
		salient feature point and descriptor
	*/
	struct ObjVisualDescription
	{
		Rect bbox;	// bounding box
		cv_KeyPoints pts;
		Mat descs;	// keypoint descriptors
		Mat img_desc;	// total feature

		ObjVisualDescription()
		{
			bbox.x = bbox.y = bbox.width = bbox.height = -1;
		}
	};


	/*
		every object is associated with an individual image file
	*/
	struct VisualObject 
	{
		// common information
		string imgname;			// pure image file name
		string imgpath;			// image file path from category directory, used as id to retrieve image
		string imgfile;			// absolute image file path
		string title;			// product name
		string categoryName;
		int category_id;
		string text_desc;		// detailed descriptions
		ObjVisualDescription visual_desc;	// visual features
	};

	struct ObjectCategory
	{
		string name;
		vector<VisualObject> objects;
	};

	// support image itself and objects in images
	struct ImageObjects
	{
		VisualObject imgObj;	// whole image as an object: used to store all local features
		vector<VisualObject> objects;
	};

	// assume each image has single type objects
	// if provide class info, multiple categories; otherwise, sinlge category containng all images
	struct ImageObjectCategory
	{
		string category_name;
		vector<ImageObjects> imgObjs;
	};

	typedef vector<ObjectCategory> ObjectCategories;
	typedef vector<ImageObjectCategory> ImageObjectCategories;


	/* 
		manage type configuration
		which library supports and what types are supported
	*/
	typedef map<string, StringVector> TypeConfig;


	/*
	CV_8U - 8-bit unsigned integers ( 0..255 )
	CV_8S - 8-bit signed integers ( -128..127 )
	CV_16U - 16-bit unsigned integers ( 0..65535 )
	CV_16S - 16-bit signed integers ( -32768..32767 )
	CV_32S - 32-bit signed integers ( -2147483648..2147483647 )
	CV_32F - 32-bit floating-point numbers ( -FLT_MAX..FLT_MAX, INF, NAN )
	CV_64F - 64-bit floating-point numbers ( -DBL_MAX..DBL_MAX, INF, NAN )
	*/

	enum DistanceType
	{
		Dist_L1,
		Dist_L2
	};

	enum NormType
	{
		Norm_L2,
		Norm_L1, 
		Norm_L_INF
	};

	

	
}
