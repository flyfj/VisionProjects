#pragma once
//////////////////////////////////////////////////////////////////////////


#pragma once

#include "common.h"
#include "Distributions.h"

class FrameGrid
{
public:
	Rect grid_box;
	float score = 1;
};

class CrowdAnalyzer;

class MotionAnalyzer
{
public:
	friend CrowdAnalyzer;

	MotionAnalyzer();

	float Process(const Mat& feat);

	bool hasInit = false;

private:
	void Update(const Mat& feat);

	float Predict(const Mat& feat);

	int samp_num = 0;
	const int TRAIN_SAMP_NUM = 100;
	Mat samps;
	GaussDist1D analyzer;
};


class CrowdAnalyzer
{
public:
	CrowdAnalyzer() {}
	CrowdAnalyzer(int imgw, int imgh, Point frame_grid);

	void ExtractCrowdFeature(Mat& prev_frame_color, Mat& cur_frame_color, vector<vector<Mat>>& feats);

	void Process(Mat& cur_frame_color);

	void DrawFlowMap(const Mat& flow, const Mat& color_img, Mat& cflowmap, int step, const Scalar& color, bool toshow = true);
	void DrawDetectionFrame(const Mat& color_img, Mat& oimg);

	bool verbose = false;

private:
	bool ValidateFeat(const Mat& feat);

	int grid_x = 1;
	int grid_y = 1;
	int samp_num = 0;
	const float ANOMALY_TH = 1e-10;
	const float MIN_FLOW_MAG = 0.5f;

	Mat prev_frame_color;
	Mat motion_mask;
	vector<vector<FrameGrid>> grids;
	vector<vector<MotionAnalyzer>> analyzers;

};


