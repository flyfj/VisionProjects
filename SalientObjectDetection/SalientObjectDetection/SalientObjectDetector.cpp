//////////////////////////////////////////////////////////////////////////
//	Salient object detector
//	fengjie@MSRA.VC
//	2010-7-19
//	modified to process superpixels
//////////////////////////////////////////////////////////////////////////


#include "SalientObjectDetector.h"
#include "Segment/image.h"
#include "Segment/misc.h"
#include "Segment/pnmfile.h"
#include "Segment/segment-image.h"
#include "nms.h"
//////////////////////////////////////////////////////////////////////////
bool comp(const TPair& a, const TPair& b)
{
	if(a.appdist < b.appdist)
		return true;
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////

SalientObjectDetector::SalientObjectDetector(void)
{
	patchsz = cvSize(20,20);
	invmat = NULL;
	segImg = NULL;

	sigma = 0.5f;
	k = 200.f;
	minarea = 50;
	overlapTH = 0.9f;
	neighborSize = 10;
}

SalientObjectDetector::~SalientObjectDetector(void)
{
	Release();
}

void SalientObjectDetector::Release()
{
	for(size_t i=0; i<segments.size(); i++)
	{
		cvReleaseImage(&segments[i].segmentIntegral);
		cvReleaseImage(&segments[i].segmentMask);
	}
	segments.clear();

	cvReleaseMat(&invmat);

	cvReleaseImage(&segImg);
}

void SalientObjectDetector::DecomposeImage(IplImage *img)
{
	patches.clear();

	int maxx = img->width/patchsz.width;
	int maxy = img->height/patchsz.height;
	maxIdX = maxx;
	maxIdY = maxy;
	cout<<"Max x: "<<maxx<<endl;
	cout<<"Max y: "<<maxy<<endl;

	//CvMat **inputmat = new CvMat*[maxx*maxy];
	CvRect rect;
	Patch pa;
	rect.width = patchsz.width;
	rect.height = patchsz.height;
	int featDim;
	for(int y=0; y<maxy; y++)
	{
		vector<Patch> row;
		for(int x=0; x<maxx; x++)
		{
			rect.x = x*patchsz.width;
			rect.y = y*patchsz.height;
			pa.id = y*maxx+x;
			pa.box = rect;
			pa.feat = ExtractFeature(img, pa.box);
			featDim = pa.feat.size();
			//add to mat
			/*inputmat[y*maxx+x] = cvCreateMat(1, pa.feat.size(), CV_32FC1);
			for(size_t id=0; id<pa.feat.size(); id++)
			{
				inputmat[y*maxx+x]->data.fl[id] = pa.feat[id];
			}*/
			row.push_back(pa);
		}
		patches.push_back(row);
	}

	//compute covariance matrix for distance computation
	//CvMat *avg = cvCreateMat(1, featDim, CV_32FC1);
	//CvMat *cov = cvCreateMat(featDim, featDim, CV_32FC1);
	//invmat = cvCloneMat(cov);
	//cvCalcCovarMatrix((const void**)inputmat, maxx*maxy, cov, avg, CV_COVAR_SCALE | CV_COVAR_ROWS);
	//cvInvert(cov, invmat, CV_SVD);
	//

	////clean
	//cvReleaseMat(&cov);
	//cvReleaseMat(&avg);
	//for(int i=0; i<maxx*maxy; i++)
	//	cvReleaseMat(&inputmat[i]);
	//delete []inputmat;
}



void SalientObjectDetector::Init(IplImage *img)
{

	//do segmentation
	//create input image
	image<rgb> *input = new image<rgb>(imgWidth, imgHeight);
	rgb val;
	for(int y=0; y<imgHeight; y++)
	{
		for(int x=0; x<imgWidth; x++)
		{
			int pos = y*imgWidth*3 + x*3;
			val.b = img->imageData[pos];
			val.g = img->imageData[pos+1];
			val.r = img->imageData[pos+2];
			imRef(input, x, y) = val;
		}
	}
	//segment
	int num_ccs = 0;
	image<int> *index = new image<int>(imgWidth, imgHeight);
	image<rgb> *seg = segment_image(input, sigma, k, minarea, &num_ccs, index);
	segImg = cvCreateImage(cvSize(imgWidth, imgHeight), IPL_DEPTH_8U, 3);
	for(int y=0; y<imgHeight; y++)
	{
		for(int x=0; x<imgWidth; x++)
		{
			val = imRef(seg, x, y);
			int pos = y*imgWidth*3 + x*3;
			segImg->imageData[pos] = val.b;
			segImg->imageData[pos+1] = val.g;
			segImg->imageData[pos+2] = val.r;
		}
	}
	//show segment image
	cvNamedWindow("seg", 1);
	cvShowImage("seg", segImg);
	cvWaitKey(10);

	printf("Segmentation is done. Total component: %d.\n", num_ccs);

	//extract components
	segments.clear();
	for(int i=0; i<num_ccs; i++)
	{
		Segment segt;
		segt.centroid = cvPoint(0,0);
		segt.segbox = cvRect(-1,-1,0,0);
		segt.area = 0;
		//create mask image
		segt.segmentMask = cvCreateImage(cvSize(imgWidth,imgHeight), IPL_DEPTH_8U, 1);
		cvSetZero(segt.segmentMask);
		//create integral image
		/*segt.segmentIntegral = cvCreateImage(cvSize(imgWidth+1, imgHeight+1), IPL_DEPTH_32F, 1);
		cvSetZero(segt.segmentIntegral);*/
		//add to set
		segments.push_back(segt);
	}

	//set masks
	for(int y=0; y<imgHeight; y++)
	{
		for(int x=0; x<imgWidth; x++)
		{
			int id = imRef(index, x, y);	//segment id
			segments[id].segmentMask->imageData[y*imgWidth+x] = 1;
			segments[id].centroid.x += x;
			segments[id].centroid.y += y;
			segments[id].area++;
			if(segments[id].segbox.x==-1)
			{
				//first point
				segments[id].segbox.x = x;
				segments[id].segbox.y = y;
			}
			else
			{
				segments[id].segbox.width = x-segments[id].segbox.x+1;
				segments[id].segbox.height = y-segments[id].segbox.y+1;
			}
		}
	}


	for(int i=0; i<num_ccs; i++)
	{
		segments[i].centroid.x /= segments[i].area;
		segments[i].centroid.y /= segments[i].area;
		//extract feature
		ExtractFeature(img, segments[i]);
		//compute integral image
		//cvIntegral(segments[i].segmentMask, segments[i].segmentIntegral);
	}


	//DecomposeImage(img);

}


//////////////////////////////////////////////////////////////////////////
void SalientObjectDetector::ComputeDistanceMaps()
{
	TPair pair;
	for(size_t i=0; i<segments.size(); i++)
	{
		Segment& seg = segments[i];
		//add distance to self (may ignore)
		pair.id = i;
		pair.appdist = 0;
		pair.spadist = 0;
		seg.list.push_back(pair);
		//only need to compute with patches behind
		for (size_t ti=i+1; ti<segments.size(); ti++)
		{
			Segment &b = segments[ti];
			pair.appdist = ComputeAppDist(seg, b);
			pair.id = ti;
			pair.spadist = ComputeSpaDist(seg, b);
			seg.list.push_back(pair);

			pair.id = i;
			b.list.push_back(pair);
		}

		//sort
		sort(seg.list.begin(), seg.list.end(), comp);

	}
}


float SalientObjectDetector::ComputeAppDist(const Patch& a, const Patch& b)
{
	float dist = 0;
	CvMat *feata = cvCreateMat(1, a.feat.size(), CV_32FC1);
	CvMat *featb = cvCreateMat(1, b.feat.size(), CV_32FC1);
	for(int i=0; i<a.feat.size(); i++)
	{
		//Euclidean distance of feature vectors
		dist += (a.feat[i]-b.feat[i])*(a.feat[i]-b.feat[i]);
		feata->data.fl[i] = a.feat[i];
		featb->data.fl[i] = b.feat[i];
	}
	dist = sqrt(dist);
	//dist = cvMahalanobis(feata, featb, invmat);

	cvReleaseMat(&feata);
	cvReleaseMat(&featb);
	return dist;
}


float SalientObjectDetector::ComputeAppDist(const Segment& a, const Segment& b)
{
	float dist = 0;
	for(size_t i=0; i<a.feat.size(); i++)
	{
		dist += (a.feat[i]-b.feat[i])*(a.feat[i]-b.feat[i]);
	}
	dist = sqrt(dist);
	//do normalization
	float dominator = sqrt(100.f*100.f+2*255.f*255);
	dist /= dominator;

	return dist;
}


float SalientObjectDetector::ComputeSpaDist(const Segment& a, const Segment& b)
{
	//compute bounding box centers
	CvPoint c1 = cvPoint(a.segbox.x+a.segbox.width/2, a.segbox.y+a.segbox.height/2);
	CvPoint c2 = cvPoint(b.segbox.x+b.segbox.width/2, b.segbox.y+b.segbox.height/2);
	float cdist = (c1.x-c2.x)*(c1.x-c2.x)+(c1.y-c2.y)*(c1.y-c2.y);
	cdist = sqrt(cdist);
	cdist = cdist - a.segbox.height/2 - b.segbox.height/2;
	cdist = MAX(cdist, 0);

	return cdist;
}


float SalientObjectDetector::ComputeScore(const IplImage *img, CvRect win)
{
	float winscore = 0;
	
	//find valid segment
	//union mask of valid segments
	IplImage *segmentUnion = cvCreateImage(cvSize(imgWidth, imgHeight), IPL_DEPTH_8U, 1);
	cvSetZero(segmentUnion);
	short *signs = new short[segments.size()];	//whether contained in the window
	memset(signs, 0, sizeof(short)*segments.size());
	vector<float> overlapArea;	//	overlap_area/window_area
	for(size_t i=0; i<segments.size(); i++)
	{
		//compute pixel number
		float sum = CV_IMAGE_ELEM(segments[i].segmentIntegral, float, win.y+win.height, win.x+win.width) + \
			CV_IMAGE_ELEM(segments[i].segmentIntegral, float, win.y, win.x) - \
			CV_IMAGE_ELEM(segments[i].segmentIntegral, float, win.y, win.x+win.width) - \
			CV_IMAGE_ELEM(segments[i].segmentIntegral, float, win.y+win.height, win.x);
		float area = CV_IMAGE_ELEM(segments[i].segmentIntegral, float, imgHeight, imgWidth);
		float ratio = sum/area;
		if(ratio > 0)	//intersect
		{
			if(ratio>=overlapTH)
			{
				//put on union image
				cvOr(segments[i].segmentMask, segmentUnion, segmentUnion);
				signs[i] = 2;
			}
			else
				signs[i] = 1;
		}

		overlapArea.push_back(sum);
	}

	//for display use
	IplImage *display = cvCloneImage(segmentUnion);
	cvConvertScale(segmentUnion, display, 255);
	cvRectangle(display, cvPoint(win.x, win.y), \
		cvPoint(win.x+win.width-1, win.y+win.height-1), \
		CV_RGB(255.0f, 255.0f, 255.0f), 2);
	cvNamedWindow("mask", 1);
	cvShowImage("mask", display);
	cvWaitKey(10);
	cvReleaseImage(&display);
	//display on segmentation image
	IplImage *segmentBackup = cvCloneImage(segImg);
	cvRectangle(segmentBackup, cvPoint(win.x, win.y), \
		cvPoint(win.x+win.width-1, win.y+win.height-1), \
		CV_RGB(255.0f, 255.0f, 255.0f), 2);


	/*IplImage *unionIntegral = cvCreateImage(cvSize(imgWidth+1, imgHeight+1), IPL_DEPTH_32F, 1);
	cvIntegral(segmentUnion, unionIntegral);
	float validsum = CV_IMAGE_ELEM(unionIntegral, float, win.y+win.height, win.x+win.width) + \
		CV_IMAGE_ELEM(unionIntegral, float, win.y, win.x) - \
		CV_IMAGE_ELEM(unionIntegral, float, win.y, win.x+win.width) - \
		CV_IMAGE_ELEM(unionIntegral, float, win.y+win.height, win.x);
	float rate = validsum/(win.width*win.height);*/
	float winarea = win.height*win.width;

	//float sums = CV_IMAGE_ELEM(unionIntegral, float, imgHeight, imgWidth);
	char str[255];

	//process all SPs within the window
	for(size_t i=0; i<segments.size(); i++)
	{
		if(signs[i]>0)
		{
			Segment &seg = segments[i];
			int nnNum = 0;
			float weightSum = 0;
			float weightDistSum = 0;
			float spaDistSum = 0;
			//find all SPs outside the window
			for(size_t j=0; j<seg.list.size(); j++)
			{
				if(signs[seg.list[j].id]<2)
				{
					//add its distance
					spaDistSum += seg.list[j].spadist;
				}
			}
			for(size_t j=0; j<seg.list.size(); j++)
			{
				if(signs[seg.list[j].id]<2)
				{
					nnNum++;
					float weight = 1-seg.list[j].spadist/spaDistSum;
					weightSum += weight;
					weightDistSum += weight*seg.list[j].appdist;
				}
			}

			float score = weightDistSum/weightSum;

			//contribute to window
			float subscore = score*overlapArea[i]*overlapArea[i]/winarea/seg.area;
			sprintf(str, "%.2f", subscore);
			CvFont font = cvFont(1,1);
			//draw on image
			cvPutText(segmentBackup, str, seg.centroid, &font, CV_RGB(0.0f,0.0f,0.0f));


			winscore += subscore;
		}
	}

	//show result
	cvShowImage("seg", segmentBackup);
	cvWaitKey(0);


	//clean
	delete []signs;
	signs = NULL;
	cvReleaseImage(&segmentBackup);
	cvReleaseImage(&segmentUnion);
	//cvReleaseImage(&unionIntegral);

	//convert to likelihood
	//printf("%f\n", winscore*rate*rate/winarea);
	return winscore;;
}


void SalientObjectDetector::ComputeWindowScores(const IplImage *img, std::vector<ScoreRect> &wins)
{
	imgWidth = img->width;
	imgHeight = img->height;
	IplImage *resized = cvCreateImage(cvSize(imgWidth, imgHeight), IPL_DEPTH_8U, 3);
	cvResize(img, resized);

	//do initialization
	Init(resized);

	//compute mutual distance between segments
	ComputeDistanceMaps();

	//evaluate certain windows
	for(size_t i=0; i<wins.size(); i++)
	{
		printf("Processing window %d.\n", i);
		wins[i].score = ComputeScore(img, wins[i].box);
	}

	cvReleaseImage(&resized);
	Release();

}


vector<ScoreRect> SalientObjectDetector::RunSlidingWindow(const IplImage* img, CvSize startSz, string filename)
{
	//resize (make maximum dimension=200)
	if(img->width > img->height)
	{
		imgWidth = 250;
		imgHeight = 250*img->height/img->width;
	}
	else
	{
		imgHeight = 250;
		imgWidth = 250*img->width/img->height;
	}
	imgWidth = img->width;
	imgHeight = img->height;
	IplImage *resized = cvCreateImage(cvSize(imgWidth, imgHeight), IPL_DEPTH_8U, 3);
	cvResize(img, resized);

	//do initialization
	Init(resized);

	//compute mutual distance between segments
	ComputeDistanceMaps();

	IplImage *scoremap = cvCreateImage(cvSize(imgWidth, imgHeight), IPL_DEPTH_32F, 1);
	cvSetZero(scoremap);

	float maxv = -1;
	CvPoint bestloc = cvPoint(0,0);
	vector<ScoreRect> wins;
	CvSize newsz = cvSize(startSz.width*imgWidth/img->width, startSz.height*imgHeight/img->height);
	//sliding window
	for(int y=0; y<imgHeight-newsz.height; y+=2)
	{
		for(int x=0; x<imgWidth-newsz.width; x+=2)
		{
			cout<<"Compute window :"<<y<<" "<<x<<endl;
			ScoreRect win;
			win.box = cvRect(x, y, newsz.width, newsz.height);
			win.score = ComputeScore(img, win.box);
			/*if(score > maxv)
			{	
				maxv = score;
				bestloc = cvPoint(x,y);
			}*/
			CV_IMAGE_ELEM(scoremap, float, \
				(int)(y+newsz.height/2), (int)(x+newsz.width/2)) = win.score;

			wins.push_back(win);
		}
	}

	//do nms
	vector<ScoreRect> reswins = nms(wins, 0.5);
	printf("%d\n", reswins.size());

	maxv = -1;
	CvPoint minloc;
	double min_val, max_val;
	//normalize
	cvNormalize(scoremap, scoremap, 1, 0, CV_C);
	//find maximum and minimum
	cvMinMaxLoc(scoremap, &min_val, &max_val, &minloc, NULL);
	printf("Max value:%f. Min value:%f.\n", max_val, min_val);
	cvConvertScale(scoremap, scoremap, 255);
	
	CvRect res = cvRect(bestloc.x, bestloc.y,\
		newsz.width, newsz.height);

	//display scoremap
	IplImage *map = cvCreateImage(cvGetSize(scoremap), IPL_DEPTH_8U, 1);
	cvConvert(scoremap, map);
	for(int i=0; i<reswins.size(); i++)
	{
		cvDrawRect(map, cvPoint(reswins[i].box.x, reswins[i].box.y),\
			cvPoint(reswins[i].box.x+reswins[i].box.width-1, reswins[i].box.y+reswins[i].box.height-1), \
			CV_RGB(255.0f,255.0f,255.0f));
	}
	
	cvNamedWindow("map", 1);
	cvShowImage("map", map);
	cvWaitKey(0);
	string savename = filename+".jpg";
	cvSaveImage(savename.c_str(), map);


	//clean
	cvReleaseImage(&map);
	//cvDestroyAllWindows();
	cvReleaseImage(&resized);
	cvReleaseImage(&scoremap);

	//release resources
	Release();

	//map result rect to original image size
	CvRect tres;
	tres.width = startSz.width;
	tres.height = startSz.height;
	tres.x = img->width*res.x/imgWidth;
	tres.y = img->height*res.y/imgHeight;

	return reswins;
}


//////////////////////////////////////////////////////////////////////////
float SalientObjectDetector::ComputeSimilarity(IplImage *img, CvRect patch, CvRect win)
{
	IplImage *back = cvCloneImage(img);
	
	float score = 0;
	vector<float> dists;
	//process all patches outside the window
	for(size_t y=0; y<patches.size(); y++)
	{
		for(size_t x=0; x<patches[y].size(); x++)
		{
			if(!InRect(patches[y][x].box, win))
			{
				cvSetImageROI(img, patch);
				cvSetImageROI(back, patches[y][x].box);
				float dist = cvNorm(img, back, CV_L2);
				dists.push_back(dist);

				//reset
				cvResetImageROI(img);
				cvResetImageROI(back);
			}
		}
	}

	cvReleaseImage(&back);

	//sort to find knn
	int neighsz = dists.size()/4;
	sort(dists.begin(), dists.end());
	for(size_t k=0; k<neighsz; k++)
		score += dists[k];
	score /= neighsz;
	
	
	return score;

}

bool SalientObjectDetector::InRect(CvRect a, CvRect win)
{
	//compute intersection
	int xmin = MAX(a.x, win.x);
	int xmax = MIN(a.x+a.width-1, win.x+win.width-1);
	int ymin = MAX(a.y, win.y);
	int ymax = MIN(a.y+a.height-1, win.y+win.height-1);
	if(xmax < xmin || ymax < ymin)
		return 0;

	float intersect = (float)((xmax-xmin+1)*(ymax-ymin+1));
	if(intersect/(a.width*a.height) > 0.7f)
		return true;
	else
		return false;
}


vector<float> SalientObjectDetector::ExtractFeature(IplImage *img, CvRect rect)
{
	cvSetImageROI(img, rect);
	CvScalar mean;
	CvScalar std;
	cvAvgSdv(img, &mean, &std);
	cvResetImageROI(img);
	vector<float> feat;
	for(int i=0; i<3; i++)
	{
		feat.push_back(mean.val[i]);
		feat.push_back(std.val[i]);
	}

	return feat;
}

void SalientObjectDetector::ExtractFeature(const IplImage *img, Segment &seg)
{
	//convert color space for feature extraction
	IplImage *backup = cvCloneImage(img);
	cvCvtColor(img, backup, CV_BGR2Lab);

	//seg.feat.clear();
	//CvScalar mean;
	//CvScalar std;
	//cvAvgSdv(backup, &mean, &std, seg.segmentMask);
	//for(int i=0; i<3; i++)
	//{
	//	seg.feat.push_back(mean.val[i]);
	//	//seg.feat.push_back(std.val[i]);
	//}

	// extract GMM appearance feature
	int nSamples = seg.area;
	CvMat* samples = cvCreateMat( nSamples, 3, CV_32FC1 );
	int count = 0;
	for(int y=seg.segbox.y; y<seg.segbox.y+seg.segbox.height; y++)
	{
		for(int x=seg.segbox.x; x<seg.segbox.x+seg.segbox.width; x++)
		{
			if(CV_IMAGE_ELEM(seg.segmentMask, uchar, y, x) > 0)
			{
				CvScalar val = cvGet2D(img, y, x);
				// add to samples
				samples->data.fl[count*3] = val.val[0];
				samples->data.fl[count*3+1] = val.val[1];
				samples->data.fl[count*3+2] = val.val[2];
				count++;
			}
		}
	}

	CvEMParams params;
	params.covs      = NULL;
	params.means     = NULL;
	params.weights   = NULL;
	params.probs     = NULL;
	params.nclusters = 3;
	params.cov_mat_type       = CvEM::COV_MAT_SPHERICAL;
	params.start_step         = CvEM::START_AUTO_STEP;
	params.term_crit.max_iter = 10;
	params.term_crit.epsilon  = 0.1;
	params.term_crit.type     = CV_TERMCRIT_ITER|CV_TERMCRIT_EPS;
	
	// train
	seg.mog.train(samples, 0, params);


	cvReleaseImage(&backup);

}

void SalientObjectDetector::DrawPatches(IplImage *img)
{
	for(size_t y=0; y<patches.size(); y++)
	{
		for(size_t x=0; x<patches[y].size(); x++)
		{
			CvRect rect = patches[y][x].box;
			cvRectangle(img, cvPoint(rect.x, rect.y), \
				cvPoint(rect.x+rect.width-1, rect.y+rect.height-1), CV_RGB(255.f,255.f,255.f));
		}
	}
}