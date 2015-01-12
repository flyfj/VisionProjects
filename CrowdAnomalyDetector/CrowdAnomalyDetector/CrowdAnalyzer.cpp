#include "CrowdAnalyzer.h"

//////////////////////////////////////////////////////////////////////////

MotionAnalyzer::MotionAnalyzer()
{

}

float MotionAnalyzer::Predict(const Mat& feat) 
{
	if (!hasInit) return 1;
	return analyzer.Predict(feat.at<float>(0));
}

void MotionAnalyzer::Update(const Mat& feat) 
{
	if (samp_num < TRAIN_SAMP_NUM) {
		// get feature and wait
		samps.push_back(feat);
	}
	else {
		if (samp_num == TRAIN_SAMP_NUM) {
			// init model
			analyzer = GaussDist1D(samps);
			hasInit = true;
			cout << "Finish initializing." << endl;
		}
		analyzer.Update(feat.at<float>(0));
	}
}

float MotionAnalyzer::Process(const Mat& feat) {
	samp_num++;
	// predict
	float val = Predict(feat);
	// update
	Update(feat);

	return val;
}


CrowdAnalyzer::CrowdAnalyzer(int imgw, int imgh, Point frame_grid)
{
	grid_x = frame_grid.x;
	grid_y = frame_grid.y;

	Point grid_sz(imgw / grid_x - 1, imgh / grid_y - 1);
	analyzers.resize(grid_y);
	grids.resize(grid_y);
	for (int i = 0; i < grid_y; i++) {
		analyzers[i].resize(grid_x);
		grids[i].resize(grid_x);
		for (int j = 0; j < grid_x; j++) {
			grids[i][j].grid_box = Rect(grid_sz.x*j, grid_sz.y*i, grid_sz.x, grid_sz.y);
		}
	}
		
}

void CrowdAnalyzer::DrawFlowMap(const Mat& flow, const Mat& color_img, Mat& cflowmap, int step, const Scalar& color, bool toshow) {
	cflowmap = color_img.clone();
	for (int y = 0; y < cflowmap.rows; y += step) {
		for (int x = 0; x < cflowmap.cols; x += step)
		{
			const Point2f& fxy = flow.at<Point2f>(y, x);
			line(cflowmap, Point(x, y), Point(cvRound(x + fxy.x), cvRound(y + fxy.y)),
				color);
			circle(cflowmap, Point(x, y), 2, color, -1);
		}
	}
		
	imshow("flow", cflowmap);
	waitKey(10);
}

void CrowdAnalyzer::ExtractCrowdFeature(const Mat& prev_frame_gray, const Mat& cur_frame_gray, vector<vector<Mat>>& feats) {

	// compute optical flow
	Mat flow;
	calcOpticalFlowFarneback(prev_frame_gray, cur_frame_gray, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
	if (verbose) {
		Mat cflow, cimg;
		cvtColor(cur_frame_gray, cimg, CV_GRAY2BGR);
		DrawFlowMap(flow, cimg, cflow, 16, Scalar(0, 255, 0));
	}

	// use mean flow magnitude as feat
	vector<Mat> flowxy;
	split(flow, flowxy);
	Mat flow_mag;
	magnitude(flowxy[0], flowxy[1], flow_mag);
	feats.resize(grids.size());
	for (size_t r = 0; r < grids.size(); r++) {
		feats[r].resize(grids[r].size());
		for (size_t c = 0; c < grids[r].size(); c++) {
			feats[r][c] = Mat::zeros(1, 1, CV_32F);
			Mat valid_pixels;
			compare(flow_mag, MIN_FLOW_MAG, valid_pixels, CMP_GT);
			feats[r][c].at<float>(0) = 
				static_cast<float>(mean(flow_mag(grids[r][c].grid_box), valid_pixels(grids[r][c].grid_box)).val[0]);
		}
	}
}

void CrowdAnalyzer::Process(const Mat& cur_frame_gray) {

	// extract feature
	samp_num++;
	if (prev_frame_gray.empty()) {
		cur_frame_gray.copyTo(prev_frame_gray);
		return;
	}
	vector<vector<Mat>> feats;
	ExtractCrowdFeature(prev_frame_gray, cur_frame_gray, feats);

	for (int r = 0; r < analyzers.size(); r++) {
		for (int c = 0; c < analyzers[r].size(); c++) {
			grids[r][c].score = analyzers[r][c].Process(feats[r][c]);
		}
	}
	
	cur_frame_gray.copyTo(prev_frame_gray);
}

void CrowdAnalyzer::DrawDetectionFrame(const Mat& color_img, Mat& oimg) {
	oimg = color_img.clone();
	for (size_t r = 0; r < grids.size(); r++) {
		for (size_t c = 0; c < grids[r].size(); c++) {
			if (grids[r][c].score < ANOMALY_TH) {
				rectangle(oimg, grids[r][c].grid_box, CV_RGB(255, 0, 0), 2);
			}
		}
	}
}