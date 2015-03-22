

#pragma once

#include "CrowdAnalyzer.h"

class VideoEventDemo
{
public:
	VideoEventDemo();

	void Run(string data_src);

	void EvalAccuracy();

private:
	shared_ptr<CrowdAnalyzer> analyzer = nullptr;
	vector<bool> anomaly_signs;
};

