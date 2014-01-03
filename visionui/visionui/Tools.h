//////////////////////////////////////////////////////////////////////////
// Define commonly used functions
//////////////////////////////////////////////////////////////////////////

#pragma once


#include "common_libs.h"
#include <io.h>
#include <direct.h>



namespace tools
{

	using namespace visualsearch;
	using namespace cv;

	struct FileInfo 
	{
		string filename;	// no directory name
		string filepath;	// full path
	};

	struct DirInfo 
	{
		string dirname;
		string dirpath;
		float filenum;
	};

	typedef vector<FileInfo> FileInfos;
	typedef vector<DirInfo> DirInfos;

	/*	tool functions	*/

	//////////////////////////////////////////////////////////////////////////
	// distance functions
	double hist_l2_dist(const Feature& a, const Feature& b);
	double hist_intersection_dist(const Feature& a, const Feature& b);


	//////////////////////////////////////////////////////////////////////////
	// file io functions
	void GetFilesFromDir(const string& dir, const string& type, FileInfos& fileInfos);
	void GetDirsFromDir(const string& dir, DirInfos& dirInfos, const string& outputFile="");

	//////////////////////////////////////////////////////////////////////////
	// sorting functions
	bool compFileNums(const DirInfo& a, const DirInfo& b);
	bool compValuePairsAsce(const Point2f& a, const Point2f& b);
	bool compValuePairsDesc(const Point2f& a, const Point2f& b);

	//////////////////////////////////////////////////////////////////////////
	// drawing functions
	// draw a histogram on canvas
	bool DrawHist(Mat& canvas, Size canvas_size, int max_val, const Feature& hist);

}