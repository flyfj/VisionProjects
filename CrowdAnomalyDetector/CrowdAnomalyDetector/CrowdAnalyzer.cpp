#include "CrowdAnalyzer.h"

//////////////////////////////////////////////////////////////////////////

MotionAnalyzer::MotionAnalyzer(int train_samp_num)
{
	TRAIN_SAMP_NUM = train_samp_num;
}

float MotionAnalyzer::Predict(const Mat& feat) 
{
	if (!hasInit) return 1;
	return analyzer.Predict(feat.at<float>(0));
}

void MotionAnalyzer::Update(const Mat& feat) 
{
	samp_num++;
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
	//samp_num++;
	// predict
	float val = Predict(feat);
	// update
	Update(feat);

	return val;
}

//////////////////////////////////////////////////////////////////////////

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
			analyzers[i][j] = MotionAnalyzer(AnalyzerParams::TRAIN_SAMPLE_NUM);
		}
	}
}

bool CrowdAnalyzer::ValidateFeat(const Mat& feat) {
	if (!feat.empty() && feat.at<float>(0) > MIN_FLOW_MAG)
		return true;
	else
		return false;
}

void CrowdAnalyzer::DrawFlowMap(const Mat& flow, const Mat& color_img, Mat& cflowmap, int step, const Scalar& color, bool toshow) {
	cflowmap = color_img.clone();
	float arrow_length = 10;
	for (int y = 0; y < cflowmap.rows; y += step) {
		for (int x = 0; x < cflowmap.cols; x += step)
		{
			const Point2f& fxy = flow.at<Point2f>(y, x);
			line(cflowmap, Point(x, y), Point(cvRound(x + fxy.x*arrow_length), cvRound(y + fxy.y*arrow_length)),
				color);
			circle(cflowmap, Point(x, y), 2, color, -1);
		}
	}
		
	imshow("flow", cflowmap);
	waitKey(10);
}

void CrowdAnalyzer::ExtractCrowdFeature(Mat& prev_frame_color, Mat& cur_frame_color, vector<vector<Mat>>& feats) {
	
	// convert format
	Mat prev_frame_gray, cur_frame_gray;
	cvtColor(prev_frame_color, prev_frame_gray, CV_BGR2GRAY);
	cvtColor(cur_frame_color, cur_frame_gray, CV_BGR2GRAY);
	// frame differencing to find motion mask
	Mat frame_diff = abs(prev_frame_gray - cur_frame_gray);
	motion_mask = frame_diff >= 20;
	// compute optical flow
	Mat flow;
	vector<Mat> flowxy;
	if (AnalyzerParams::USE_GPU) {
		gpu::GpuMat gpu_flow_x, gpu_flow_y;
		gpu::GpuMat gpu_prev_frame(prev_frame_gray), gpu_cur_frame(cur_frame_gray);
		gpu::FarnebackOpticalFlow gpu_fb_flow;
		gpu_fb_flow(gpu_prev_frame, gpu_cur_frame, gpu_flow_x, gpu_flow_y);
		flowxy.resize(2);
		gpu_flow_x.download(flowxy[0]);
		gpu_flow_y.download(flowxy[1]);
		medianBlur(flowxy[0], flowxy[0], 5);
		medianBlur(flowxy[1], flowxy[1], 5);
		merge(flowxy, flow);
	}
	else
	{
		//calcOpticalFlowSF(prev_frame_color, cur_frame_color, flow, 3, 2, 4, 4.1, 25.5, 18, 55.0, 25.5, 0.35, 18, 55.0, 25.5, 10);
		calcOpticalFlowFarneback(prev_frame_gray, cur_frame_gray, flow, 0.5, 5, 10, 5, 5, 1.2, 0);
		// smooth flow map
		medianBlur(flow, flow, 5);
		split(flow, flowxy);
	}

	if (verbose) {
		Mat cflow, cimg;
		cimg = cur_frame_color.clone();
		DrawFlowMap(flow, cimg, cflow, 16, Scalar(0, 255, 0));
	}

	// use mean flow magnitude as feature
	Mat flow_mag;
	magnitude(flowxy[0], flowxy[1], flow_mag);
	float max_val = 0;
	float min_val = numeric_limits<float>::max();
	feats.resize(grids.size());
	for (size_t r = 0; r < grids.size(); r++) {
		feats[r].resize(grids[r].size());
		for (size_t c = 0; c < grids[r].size(); c++) {
			feats[r][c] = Mat::zeros(1, 1, CV_32F);
			// filter stationary pixels
			motion_mask = flow_mag > MIN_FLOW_MAG;
			float score = static_cast<float>(mean(flow_mag(grids[r][c].grid_box), motion_mask(grids[r][c].grid_box)).val[0]);
			min_val = std::min(min_val, score);
			max_val = std::max(max_val, score);
			feats[r][c].at<float>(0) = score;
		}
	}
	if (verbose) {
		cout << min_val << " " << max_val << endl;
	}
}

bool CrowdAnalyzer::Process(Mat& cur_frame_color) {

	// extract feature
	samp_num++;
	cout << "sample number: " << samp_num << endl;
	if (prev_frame_color.empty()) {
		cur_frame_color.copyTo(prev_frame_color);
		return false;
	}
	vector<vector<Mat>> feats;
	ExtractCrowdFeature(prev_frame_color, cur_frame_color, feats);

	bool ifAbnormal = false;
#pragma omp parallel for
	for (int r = 0; r < analyzers.size(); r++) {
		for (int c = 0; c < analyzers[r].size(); c++) {
			grids[r][c].score = 1;
			grids[r][c].anomaly_sign = false;
			if (ValidateFeat(feats[r][c])) {
				grids[r][c].score = analyzers[r][c].Predict(feats[r][c]);
				if (grids[r][c].score < AnalyzerParams::ANOMALY_TH) {
					grids[r][c].anomaly_sign = true;
					ifAbnormal = true;
				}
				if (!analyzers[r][c].hasInit)
					analyzers[r][c].Update(feats[r][c]);
			}
		}
	}

	cur_frame_color.copyTo(prev_frame_color);

	return ifAbnormal;
}

void CrowdAnalyzer::DrawDetectionFrame(const Mat& color_img, Mat& oimg) {
	oimg = color_img.clone();
	for (size_t r = 0; r < grids.size(); r++) {
		for (size_t c = 0; c < grids[r].size(); c++) {
			if (analyzers[r][c].hasInit) {
				rectangle(oimg, grids[r][c].grid_box, CV_RGB(0, 255, 0), 2);
				if (grids[r][c].anomaly_sign) {
					rectangle(oimg, grids[r][c].grid_box, CV_RGB(255, 0, 0), 2);
					if(verbose) cout << "anomaly score: " << grids[r][c].score << endl;
				}
			}
		}
	}
	if (verbose && !motion_mask.empty()) {
		imshow("motion mask", motion_mask);
		waitKey(10);
	}

}