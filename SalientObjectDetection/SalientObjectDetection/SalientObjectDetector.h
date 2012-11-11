//////////////////////////////////////////////////////////////////////////
//	Salient object detector
//	fengjie@MSRA.VC
//	2010-7-19
//////////////////////////////////////////////////////////////////////////


#pragma once


#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <ml.h>
#include <cvaux.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <cmath>
#include <algorithm>
using namespace std;

typedef struct ScoreRect
{
	CvRect box;
	float score;
}
ScoreRect;

typedef struct TPair
{
	float appdist;
	int id;
	float spadist;
	CvRect rect;
}
TPair;

typedef struct Patch
{
	int id;
	CvRect box;
	vector<float> feat;

	//likelihood list of all patches in this location
	vector<TPair> list;	//distance, id
}
Patch;

//class for superpixel
typedef struct Segment
{
	//data
	IplImage *segmentMask;
	IplImage *segmentIntegral;
	int area;
	CvRect segbox;
	CvPoint centroid;
	vector<float> feat;	//feature vector
	vector<TPair> list;	//relation with other SPs

	CvEM mog;

	////functions
	//Segment()
	//{
	//	segmentMask = NULL;
	//	segmentIntegral = NULL;
	//}
	//~Segment()
	//{
	//	cvReleaseImage(&segmentMask);
	//	cvReleaseImage(&segmentIntegral);
	//}
	//const Segment& operator = (const Segment& a)
	//{
	//	cvReleaseImage(&this->segmentIntegral);
	//	cvReleaseImage(&this->segmentMask);
	//	this->segmentIntegral = cvCloneImage(a.segmentIntegral);
	//	this->segmentMask = cvCloneImage(a.segmentMask);
	//	this->feat = a.feat;
	//	this->list = a.list;

	//	return *this;
	//}

}
Segment;

bool comp(const TPair& a, const TPair& b);

class SalientObjectDetector
{
private:
	int imgWidth;
	int imgHeight;
	int maxIdX;
	int maxIdY;
	float overlapTH;
	CvSize patchsz;
	vector<vector<Patch>> patches;
	CvMat *invmat;
	IplImage *segImg;
	int neighborSize;	//used for knn
	//segment-related
	vector<Segment> segments;	//integral image for each segment
	float sigma;
	float k;
	int minarea;

public:
	SalientObjectDetector(void);
	~SalientObjectDetector(void);

	//initialize
	//segmentation, feature extraction
	void Init(IplImage *img);

	void DecomposeImage(IplImage *img);

	void Release();

	//detection function
	vector<ScoreRect> RunSlidingWindow(const IplImage* img, CvSize startSz, string filename);
	//compute score only for given windows
	void ComputeWindowScores(const IplImage* img, vector<ScoreRect>& wins);

	//compute saliency score for a window
	float ComputeScore(const IplImage* img, CvRect win);	//img no need right now

	//compute feature distance between two units(patches/SPs)
	float ComputeAppDist(const Patch& a, const Patch& b);
	float ComputeAppDist(const Segment& a, const Segment& b);
	//compute spatial distance in image
	float ComputeSpaDist(const Segment& a, const Segment& b);

	//compute knn of patch outside the window
	float ComputeSimilarity(IplImage *img, CvRect patch, CvRect win);

	//if contained
	bool InRect(CvRect a, CvRect win);

	void ColorMoments(IplImage* img, CvScalar *m1, CvScalar *m2, CvScalar *m3);

	//feature extraction for patch
	vector<float> ExtractFeature(IplImage *img, CvRect rect);
	//feature extraction for segment
	void ExtractFeature(const IplImage *img, Segment& seg);

	//compute distance between any two units(patches or SPs)
	void ComputeDistanceMaps();

	void DrawPatches(IplImage *img);

};
