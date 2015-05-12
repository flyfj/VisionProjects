#include "VideoEventDemo.h"


VideoEventDemo::VideoEventDemo()
{
}

void VideoEventDemo::Run(string data_src) {

	grid_scores.clear();

	ofstream logger(AnalyzerParams::LOG_FILE);
	Logger::log("input data source: " + data_src, logger);
	VideoCapture capture(data_src);
	if (!capture.isOpened()) {
		Logger::log("Error opening data source.", logger);
		return;
	}

	int frame_id = 0;
	double total_start_t = cv::getTickCount();
	anomaly_signs.clear();
	while (true) {
		Mat frame;
		capture >> frame;
		if (frame.empty())
			break;

		frame_id++;
		if (frame_id == 1) {
			analyzer = make_shared<CrowdAnalyzer>(frame.cols, frame.rows, Point(AnalyzerParams::grid_x, AnalyzerParams::grid_y));
			analyzer->verbose = AnalyzerParams::DO_DEBUG;
		}

		// perform detection
		double start_t = cv::getTickCount();
		bool det_sign = analyzer->Process(frame);
		anomaly_signs.push_back(!det_sign);
		Logger::log("Frame time cost: " + std::to_string((double)(cv::getTickCount() - start_t) / cv::getTickFrequency()) + "s.", logger);

		// store detection scores
		const vector<vector<FrameGrid>> grids = analyzer->GetGrids();
		vector<vector<float>> cur_grid_scores(grids.size(), vector<float>(grids[0].size()));
		for (size_t i = 0; i < grids.size(); i++) {
			for (size_t j = 0; j < grids[i].size(); j++) {
				cur_grid_scores[i][j] = grids[i][j].score;
			}
		}
		grid_scores.push_back(cur_grid_scores);

		// show result
		Mat oimg;
		analyzer->DrawDetectionFrame(frame, oimg);
		imshow("frame", oimg);
		if (waitKey(10) == 'q')
			break;
	}
	Logger::log("total processing time: " + std::to_string((double)(cv::getTickCount() - total_start_t) / cv::getTickFrequency()) + "s.", logger);

	OutputResults("outputs.txt");

	EvalAccuracy();
}

void VideoEventDemo::OutputResults(const string& savefn) 
{
	ofstream out(savefn);
	out << grid_scores.size() << endl;
	for (size_t id = 0; id < grid_scores.size(); id++) {
		float max_score = 0;
		for (size_t i = 0; i < grid_scores[id].size(); i++) {
			for (size_t j = 0; j < grid_scores[id][i].size(); j++) {
				max_score = std::max(grid_scores[id][i][j], max_score);
			}
		}
		out << max_score << endl;
	}
}

void VideoEventDemo::EvalAccuracy() {
	try
	{
		// read in labels
		ifstream in(AnalyzerParams::LABEL_FILE);
		if (!in.is_open()) {
			cerr << "label file not found." << endl;
			return;
		}

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