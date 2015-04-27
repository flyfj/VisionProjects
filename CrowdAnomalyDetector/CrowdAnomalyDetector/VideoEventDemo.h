

#pragma once

#include "CrowdAnalyzer.h"

class VideoEventDemo
{
public:
	VideoEventDemo();

	void Run(string data_src);

	void OutputResults(const string& savefn);
	void EvalAccuracy();

private:
	shared_ptr<CrowdAnalyzer> analyzer = nullptr;
	vector<bool> anomaly_signs;
	// save scores for all grids for the whole video
	vector<vector<vector<float>>> grid_scores;
};

