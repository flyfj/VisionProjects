

#include "VideoEventDemo.h"

#ifdef _DEBUG
#pragma comment(lib, "opencv_core249d.lib")
#pragma comment(lib, "opencv_highgui249d.lib")
#pragma comment(lib, "opencv_imgproc249d.lib")
#pragma comment(lib, "opencv_video249d.lib")
#else
#pragma comment(lib, "opencv_core249.lib")
#pragma comment(lib, "opencv_highgui249.lib")
#pragma comment(lib, "opencv_imgproc249.lib")
#pragma comment(lib, "opencv_video249.lib")
#endif

int main()
{
	string video_fn = "F:\\Crowd\\scene3.avi";
	
	VideoEventDemo demo;
	demo.RunVideo(video_fn);

	return 0;
}