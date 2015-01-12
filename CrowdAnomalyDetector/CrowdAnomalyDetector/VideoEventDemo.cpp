#include "VideoEventDemo.h"


VideoEventDemo::VideoEventDemo()
{
}

void VideoEventDemo::RunVideo(string video_fn) {

	VideoCapture capture(video_fn);
	if (!capture.isOpened()) {
		cerr << "Error opening video." << endl;
		return;
	}

	int frame_id = 0;
	while (true) {
		Mat frame;
		capture >> frame;
		if (frame.empty())
			break;

		frame_id++;
		if (frame_id == 1) {
			analyzer = make_shared<CrowdAnalyzer>(frame.cols, frame.rows, Point(1, 1));
			analyzer->verbose = true;
		}

		analyzer->Process(frame);
		//putText(frame, string(str), Point(20, 20), CV_FONT_HERSHEY_PLAIN, 1.5, CV_RGB(0, 0, 255));
		//cout << frame_id << " : " << prob << endl << endl;
		Mat oimg;
		analyzer->DrawDetectionFrame(frame, oimg);
		imshow("frame", oimg);
		if (waitKey(10) == 'q')
			break;
	}

}
