

#pragma once

#include "CrowdAnalyzer.h"

class VideoEventDemo
{
public:
	VideoEventDemo();

	void RunVideo(string video_fn);

private:
	shared_ptr<CrowdAnalyzer> analyzer = nullptr;
};

