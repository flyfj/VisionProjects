//////////////////////////////////////////////////////////////////////////
//	compare with CVPR10 results
//	fengjie@MSRA.VC
//	2010-7-30
//////////////////////////////////////////////////////////////////////////


#include "SalientObjectDetector.h"
#include <io.h>
#include <fstream>
#include <iostream>
using namespace std;

bool compt(const ScoreRect a, const ScoreRect b)
{
	if(a.score>b.score)
		return true;
	else
		return false;
}

int main()
{
	
	SalientObjectDetector detector;

	//read windows from author data
	int count = 0;
	struct _finddata_t ffblk; 
	int done = 1;
	string pascaldir = "D:\\research_data\\PASCAL VOC 07\\VOCtest_06-Nov-2007\\VOCdevkit\\VOC2007\\";
	string savedir = "E:\\selfres\\score\\";
	string authordir = "E:\\research_data\\objectness_sampled_windows_pascal07\\VOCtest\\";
	string gtdir = pascaldir;
	gtdir += "AnnotationsTXT_test\\";
	string imgdir = pascaldir;
	imgdir += "JPEGImages\\";
	string gtfile = authordir + "*.txt";
	int handle = _findfirst(gtfile.c_str(), &ffblk);
	//cvNamedWindow("win", 1);
	while(done!=-1)
	{
		count++;
		if(count>1)
		{
			string fname(ffblk.name);
			fname = fname.substr(0, fname.length()-4);	//remove extension
			string authorfilename = authordir + fname + ".txt";
			string imgfilename = imgdir + fname + ".jpg";
			//read author data
			ScoreRect rect;
			float score;
			int xmin,xmax,ymin,ymax;
			vector<ScoreRect> wins;
			vector<float> authorScores;
			ifstream in(authorfilename.c_str());
			for(int i=0; i<500; i++)
			{
				in>>xmin>>ymin>>xmax>>ymax>>score;
				rect.box.x = xmin-1;
				rect.box.y = ymin-1;
				rect.box.width = xmax-xmin+1;
				rect.box.height = ymax-ymin+1;
				rect.score = 0;
				wins.push_back(rect);
				authorScores.push_back(score);
			}

			//read image
			cvNamedWindow("win", 1);
			IplImage *img = cvLoadImage(imgfilename.c_str());
			cvShowImage("win", img);
			cvWaitKey(10);
			detector.ComputeWindowScores(img, wins);

			//sort by score
			sort(wins.begin(), wins.end(), compt);
			for(size_t i=0; i<20; i++)
			{
				//draw on image
				cvRectangle(img, cvPoint(wins[i].box.x, wins[i].box.y),\
					cvPoint(wins[i].box.x+wins[i].box.width-1, wins[i].box.y+wins[i].box.height-1), \
					CV_RGB(rand()%255, rand()%255, rand()%255), 2);
				cvShowImage("win", img);
				cvWaitKey(0);
			}


			//output to file
			/*string savepath = savedir + fname + ".txt";
			ofstream out(savepath.c_str());
			for(size_t i=0; i<wins.size(); i++)
			{
				out<<wins[i].box.x<<" "<<wins[i].box.y<<" " \
					<<wins[i].box.x+wins[i].box.width-1<<" " \
					<<wins[i].box.y+wins[i].box.height-1<<" " \
					<<wins[i].score<<" "<<authorScores[i]<<endl;
			}*/
			
			cvReleaseImage(&img);

		}
		

		//get next
		done = _findnext(handle, &ffblk);

	}

	cvDestroyAllWindows();
	return 0;
}