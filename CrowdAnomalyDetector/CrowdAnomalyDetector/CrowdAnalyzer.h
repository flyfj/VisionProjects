//////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"
#include "Distributions.h"

class FrameGrid
{
public:
	Rect grid_box;
	float score = 1;
	bool anomaly_sign = false;

};


class CrowdAnalyzer;

class MotionAnalyzer
{
public:
	friend CrowdAnalyzer;

	MotionAnalyzer() {}
	MotionAnalyzer(int train_samp_num);

	float Process(const Mat& feat);

	bool hasInit = false;

private:
	void Update(const Mat& feat);

	float Predict(const Mat& feat);

	int samp_num = 0;
	int TRAIN_SAMP_NUM = 100;
	Mat samps;
	GaussDist1D analyzer;
};



class CrowdAnalyzer
{
public:
	CrowdAnalyzer() {}
	CrowdAnalyzer(int imgw, int imgh, Point frame_grid);

	void ExtractCrowdFeature(Mat& prev_frame_color, Mat& cur_frame_color, vector<vector<Mat>>& feats);

	bool Process(Mat& cur_frame_color);

	void DrawFlowMap(const Mat& flow, const Mat& color_img, Mat& cflowmap, int step, const Scalar& color, bool toshow = true);
	void DrawDetectionFrame(const Mat& color_img, Mat& oimg);

	const vector<vector<FrameGrid>> GetGrids() const { return grids; }

	bool verbose = false;

private:
	bool ValidateFeat(const Mat& feat);

	// sequence of detection result, used for eliminating noisy detection
	bitset<3> det_res_seq;
	int grid_x = 1;
	int grid_y = 1;
	int samp_num = 0;
	const float MIN_FLOW_MAG = 0.5f;

	Mat prev_frame_color;
	Mat motion_mask;
	vector<vector<FrameGrid>> grids;
	vector<vector<MotionAnalyzer>> analyzers;
};


