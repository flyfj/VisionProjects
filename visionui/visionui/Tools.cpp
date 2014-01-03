

#include "Tools.h"

namespace tools
{
	//////////////////////////////////////////////////////////////////////////
	double hist_l2_dist(const Feature& a, const Feature& b)
	{
		double sum = 0;
		for(size_t i=0; i<a.size(); i++)
		{
			double min_dist = (a[i]-b[i])*(a[i]-b[i]);
			sum += min_dist;
		}

		return sqrt(sum);
	}

	double hist_intersection_dist(const Feature& a, const Feature& b)
	{
		double sum = 0;
		for(size_t i=0; i<a.size(); i++)
			sum += min(a[i], b[i]);

		return 1 - sum;
	}


	//////////////////////////////////////////////////////////////////////////
	void GetFilesFromDir(const string& dir, const string& type, FileInfos& fileInfos)
	{
		fileInfos.clear();
		FileInfo fileinfo;

		struct _finddata_t ffblk;
		int done = 1;
		string filelist = dir + type;	// TODO: is there a way to find multiple ext files without doing several times?
		int handle = _findfirst(filelist.c_str(), &ffblk);
		if(handle != -1)
		{
			while( done != -1 )
			{
				string fname(ffblk.name);
				//fname = fname.substr(0, fname.length()-4);	// image name without extension
				string path = dir + fname;
				fileinfo.filename = fname;
				fileinfo.filepath = path;
				fileInfos.push_back(fileinfo);

				done = _findnext(handle, &ffblk);
			}
		}

	}

	void GetDirsFromDir(const string& dir, DirInfos& dirInfos, const string& outputFile)
	{
		dirInfos.clear();
		DirInfo dirInfo;

		struct _finddata_t ffblk;
		int done = 1;
		string files = dir + "*.*";	// TODO: is there a way to find multiple ext files without doing several times?
		int handle = _findfirst(files.c_str(), &ffblk);
		if(handle != -1)
		{
			while( done != -1 )
			{
				if(ffblk.attrib == _A_SUBDIR)
				{
					string fname(ffblk.name);
					dirInfo.dirname = fname;
					dirInfo.dirpath = dir + fname + "\\";
					dirInfos.push_back(dirInfo);
				}
				
				done = _findnext(handle, &ffblk);
			}
		}

		if(!dirInfos.empty())
		{
			ofstream out(outputFile.c_str());
			for(size_t i=0; i<dirInfos.size(); i++)
				out<<dirInfos[i].dirname<<endl;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	bool compFileNums(const DirInfo& a, const DirInfo& b)
	{
		return a.filenum > b.filenum;
	}


	bool compValuePairsAsce(const Point2f& a, const Point2f& b)
	{
		if(a.y < b.y)
			return true;
		else if(a.y == b.y)
			return a.x < b.x;
		else
			return false;
	}

	bool compValuePairsDesc(const Point2f& a, const Point2f& b)
	{
		if(a.y > b.y)
			return true;
		else if(a.y == b.y)
			return a.x < b.x;
		else
			return false;
	}

	//////////////////////////////////////////////////////////////////////////
	bool DrawHist(Mat& canvas, Size canvas_size, int max_val, const Feature& hist)
	{
		if( hist.empty() )
			return false;

		/* show histogram */
		// Get scale so the histogram fit the canvas height
		double maxv = max_val;	// upper bound of bin value

		canvas.create(canvas_size.height, canvas_size.width, CV_8UC3);
		canvas.setTo(255);
		double binWidth = canvas.cols / (hist.size() - 1);
		double scale = maxv > canvas.rows ? (double)canvas.rows/maxv : 1.;  

		// Draw histogram
		for ( size_t i = 0; i < hist.size(); i++)  
		{    
			Point pt1(i*binWidth, canvas.rows - (hist[i]/maxv * canvas.rows));
			Point pt2((i+1)*binWidth, canvas.rows);
			rectangle(canvas, pt1, pt2, CV_RGB(0, 255, 0), CV_FILLED);
		}

		return true;

	}

}