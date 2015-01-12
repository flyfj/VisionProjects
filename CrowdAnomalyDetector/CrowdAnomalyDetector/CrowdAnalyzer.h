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

private:
	void Update(const Mat& feat);

	float Predict(const Mat& feat);

	int samp_num = 0;
	bool hasInit = false;
	const int TRAIN_SAMP_NUM = 50;

	Mat samps;
	GaussDist1D analyzer;
};


class CrowdAnalyzer
{
public:
	CrowdAnalyzer() {}
	CrowdAnalyzer(int imgw, int imgh, Point frame_grid);

	void ExtractCrowdFeature(const Mat& prev_frame_gray, const Mat& cur_frame_gray, vector<vector<Mat>>& feats);

	void Process(const Mat& cur_frame_gray);

	void DrawFlowMap(const Mat& flow, const Mat& color_img, Mat& cflowmap, int step, const Scalar& color, bool toshow = true);
	void DrawDetectionFrame(const Mat& color_img, Mat& oimg);

	bool verbose = false;

private:
	int grid_x = 1;
	int grid_y = 1;
	int samp_num = 0;
	const float ANOMALY_TH = 0.00001f;
	const float MIN_FLOW_MAG = 0.01f;

	Mat prev_frame_gray;
	vector<vector<FrameGrid>> grids;
	vector<vector<MotionAnalyzer>> analyzers;

};


