#include "VideoEventDemo.h"


VideoEventDemo::VideoEventDemo()
{
}

void VideoEventDemo::RunVideo(string video_fn) {

	ofstream logger(AnalyzerParams::LOG_FILE);
	
	Logger::log("input video: " + video_fn, logger);
	VideoCapture capture(video_fn);
	if (!capture.isOpened()) {
		Logger::log("Error opening video.", logger);
		return;
	}

	int frame_id = 0;
	anomaly_signs.clear();
	while (true) {
		Mat frame;
		capture >> frame;
		if (frame.empty())
			break;

		frame_id++;
		if (frame_id == 1) {
			analyzer = make_shared<CrowdAnalyzer>(frame.cols, frame.rows, Point(AnalyzerParams::grid_x, AnalyzerParams::grid_y));
			analyzer->verbose = false;
		}

		double start_t = cv::getTickCount();
		bool det_sign = analyzer->Process(frame);
		anomaly_signs.push_back(det_sign);
		Logger::log("Frame cost: " + std::to_string((double)(cv::getTickCount() - start_t) / cv::getTickFrequency()) + "s.", logger);

		Mat oimg;
		analyzer->DrawDetectionFrame(frame, oimg);
		imshow("frame", oimg);
		if (waitKey(0) == 'q')
			break;
	}

	EvalAccuracy();

}

void VideoEventDemo::EvalAccuracy() {
	try
	{
		// read in labels
		ifstream in(AnalyzerParams::LABEL_FILE);

		int frame_num = 0;
		in >> frame_num;
		vector<bool> gt_labels;
		gt_labels.reserve(frame_num);
		bool sign = false;	// false: normal; true: abnormal
		for (int i = 0; i < frame_num; i++) {
			in >> sign;
			gt_labels.push_back(sign);
		}

		float corr_num = 0;
		float total_num = std::min(anomaly_signs.size(), gt_labels.size());
		for (auto i = 0; i < total_num; i++) {
			corr_num += (anomaly_signs[i] == gt_labels[i] ? 1 : 0);
		}
		float accu = corr_num / total_num;
		cout << "detection accuracy: " << accu * 100 << "%" << endl;
	}
	catch (std::exception ex)
	{
		cerr <<"error during accuracy evaluation: "<< ex.what() << endl;
	}
	
}