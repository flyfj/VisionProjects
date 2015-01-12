//////////////////////////////////////////////////////////////////////////
// a collection of distribution data
// jiefeng©2014-12-17
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "common.h"

/*
	basic gaussian distribution
*/
class GaussDist1D
{
public:
	GaussDist1D() {
		// init as normal
		mean_ = 0; std_ = 1; samp_num = 0;
	}

	GaussDist1D(float mean, float std) {
		mean_ = mean; std_ = std; samp_num = 0;
	}

	GaussDist1D(const Mat& samps) {
		Scalar mean_mat, std_mat;
		meanStdDev(samps, mean_mat, std_mat);
		mean_ = static_cast<float>(mean_mat.val[0]);
		std_ = static_cast<float>(std_mat.val[0]);
		samp_num = samps.rows;
	}

	void Update(float val) {
		mean_ = (mean_*samp_num + val) / (samp_num + 1);
		std_ = sqrt((std_*std_*samp_num + (val - mean_)*(val - mean_)) / (samp_num + 1));
		samp_num++;
	}

	float Predict(float val) {
		// compute probability
		float prob = exp(-(mean_ - val)*(mean_ - val) / (2 * std_*std_));
		prob /= (2 * 3.14159f);
		return prob;
	}

private:
	float mean_;
	float std_;
	int samp_num;
};



//////////////////////////////////////////////////////////////////////////

/*
	mixture of gaussian
*/
template<typename Gauss>
class GMM 
{
public:
	GMM(int cls_num, const Mat& samps);

	void Update(const Mat& samp);

	float Predict(const Mat& samp);

private:
	vector<Gauss> components;
};

template<typename Gauss>
GMM<Gauss>::GMM(int cls_num, const Mat& samps) {
	// init with kmeans
	Mat centers, labels;
	kmeans(samps, cls_num, labels, TermCriteria(CV_TERMCRIT_NUMBER | CV_TERMCRIT_EPS, 4, 0.01f), 3, KMEANS_PP_CENTERS, centers);
	// assign samples
	vector<Mat> cls_samps(cls_num);
	for (int i = 0; i < labels.rows; i++) {
		cls_samps[labels.at<int>(i)].push_back(samps.row(i));
	}
	// init components
	components.resize(cls_num);
	for (size_t i = 0; i < cls_samps.size(); i++) {
		Scalar mean, std;
		meanStdDev(cls_samps[i], mean, std);
		components[i] = Gauss(mean.val[0], std.val[0]);
	}
}

template<typename Gauss>
void GMM<Gauss>::Update(const Mat& samp) {
	
}