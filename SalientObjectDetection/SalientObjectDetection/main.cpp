//////////////////////////////////////////////////////////////////////////
//	main test program
//	fengjie@MSRA.VC
//	2010-7-23
//////////////////////////////////////////////////////////////////////////


#include "SalientObjectDetector.h"
#include <io.h>
#include <fstream>
#include <iostream>
using namespace std;

int main()
{

	//SalientObjectDetector detector;

	//int count = 0;
	//struct _finddata_t ffblk; 
	//int done = 1;
	//string pascaldir = "D:\\research_data\\PASCAL VOC 07\\VOCtest_06-Nov-2007\\VOCdevkit\\VOC2007\\";
	//string savedir = "E:\\selfres\\new\\";
	//string gtdir = pascaldir;
	//gtdir += "AnnotationsTXT_test\\";
	//string imgdir = pascaldir;
	//imgdir += "salienttest\\";
	//string gtfile = imgdir + "*.jpg";
	//int handle = _findfirst(gtfile.c_str(), &ffblk);
	////cvNamedWindow("win", 1);
	//while(done!=-1)
	//{
	//	count++;
	//	if(count>2)
	//	{
	//		string fname(ffblk.name);
	//		fname = fname.substr(0, fname.length()-4);	//remove extension
	//		string gtfilename = gtdir + fname + ".txt";
	//		string imgfilename = imgdir + fname + ".jpg";
	//		//read ground truth
	//		vector<CvRect> objs;
	//		ifstream in(gtfilename.c_str());
	//		char ch;
	//		string tstr, str;
	//		CvRect obj;
	//		int right, bottom;
	//		int w,h;
	//		while(getline(in, tstr))
	//		{
	//			stringstream sin(tstr);
	//			sin>>ch;	//sign
	//			if(ch!='B')
	//			{
	//				//read width and height
	//				sin>>ch>>w>>ch>>h;
	//				continue;
	//			}

	//			sin>>ch;	//comma
	//			sin>>str;	//name
	//			sin>>ch;	//left bracket
	//			sin>>obj.x;
	//			sin>>ch;
	//			sin>>obj.y;
	//			sin>>ch>>ch>>ch;
	//			sin>>right;
	//			sin>>ch;
	//			sin>>bottom;
	//			sin>>ch;

	//			obj.width = right-obj.x+1;
	//			obj.height = bottom-obj.y+1;

	//			//add to set
	//			objs.push_back(obj);
	//		}

	//		//run on each object scale
	//		char numstr[10];
	//		cvNamedWindow("win", 1);
	//		for(size_t i=0; i<1; i++)
	//		{
	//			IplImage *img = cvLoadImage(imgfilename.c_str());
	//			cvShowImage("win", img);
	//			cvWaitKey(10); 
	//			CvRect gtrect = objs[i];
	//			vector<ScoreRect> rects = detector.RunSlidingWindow(img, cvSize(gtrect.width, gtrect.height), fname);
	//			//detector.DrawPatches(img);

	//			for(int j=0; j<rects.size(); j++)
	//			{
	//				//draw
	//				cvRectangle(img, cvPoint(rects[j].box.x, rects[j].box.y), \
	//					cvPoint(rects[j].box.x+rects[j].box.width-1, rects[j].box.y+rects[j].box.height-1), \
	//					CV_RGB(255.0f, 0.0f, 0.0f), 2);
	//			}
	//			
	//			cvRectangle(img, cvPoint(gtrect.x, gtrect.y), \
	//				cvPoint(gtrect.x+gtrect.width-1, gtrect.y+gtrect.height-1), \
	//				CV_RGB(0.0f, 255.0f, 0.0f), 2);

	//			//save image
	//			string savepath = savedir + fname;
	//			sprintf(numstr, "%d", i);
	//			savepath = savepath + string(numstr) + ".jpg";
	//			cvSaveImage(savepath.c_str(), img);

	//			cvReleaseImage(&img);
	//		}

	//	}

	//	//get next
	//	done = _findnext(handle, &ffblk);
	//}

	IplImage *img = cvLoadImage("D:\\research_data\\PASCAL_VOC_07\\VOCtest_06-Nov-2007\\VOCdevkit\\VOC2007\\JPEGImages\\008827.jpg");
	SalientObjectDetector detector;
	detector.Init(img);
	

	//detector.Init(img);
	/*int xmin=211;
	int ymin=168;
	int xmax=403;
	int ymax=272;
	CvRect gtrect = cvRect(xmin,ymin,xmax-xmin+1,ymax-ymin+1);
	CvRect bestrect = detector.Run(img, cvSize(gtrect.width, gtrect.height));*/
	//detector.DrawPatches(img);

	////draw
	//cvRectangle(img, cvPoint(bestrect.x, bestrect.y), \
	//	cvPoint(bestrect.x+bestrect.width-1, bestrect.y+bestrect.height-1), \
	//	CV_RGB(255.0f, 0.0f, 0.0f));
	//cvRectangle(img, cvPoint(gtrect.x, gtrect.y), \
	//	cvPoint(gtrect.x+gtrect.width-1, gtrect.y+gtrect.height-1), \
	//	CV_RGB(0.0f, 255.0f, 0.0f), 2);
	//cvNamedWindow("win", 1);
	//cvShowImage("win", img);
	//cvWaitKey(0);

	//cvReleaseImage(&img);
	//cvDestroyAllWindows();

	return 0;
}