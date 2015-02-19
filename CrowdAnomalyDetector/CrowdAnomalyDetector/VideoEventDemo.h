

#pragma once

#include "CrowdAnalyzer.h"

class VideoEventDemo
{
public:
	VideoEventDemo();

	void RunVideo(string video_fn);

	void EvalAccuracy();

private:
	shared_ptr<CrowdAnalyzer> analyzer = nullptr;
	vector<bool> anomaly_signs;
};

