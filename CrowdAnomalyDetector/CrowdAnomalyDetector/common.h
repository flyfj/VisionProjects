

#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>
using namespace std;

#include <opencv2/opencv.hpp>
#include <opencv2/gpu/gpu.hpp>
using namespace cv;

//#include <libconfig.hh>


class AnalyzerParams
{
public:
	static int TRAIN_SAMPLE_NUM;
	static float ANOMALY_TH;
	static int grid_x;
	static int grid_y;
	static bool USE_GPU;
};

