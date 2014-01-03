//////////////////////////////////////////////////////////////////////////


#include "ImgKeypointDescriptorExtractor.h"


namespace visualsearch
{


	ImgKeypointDescriptorExtractor::ImgKeypointDescriptorExtractor(void)
	{
		InitKeypointDescriptorTypes();
	}


	ImgKeypointDescriptorExtractor::~ImgKeypointDescriptorExtractor(void)
	{
	}


	void ImgKeypointDescriptorExtractor::InitKeypointDescriptorTypes()
	{
		/*
			supported descriptor type
			SIFT, SURF, ORB, BRIEF, FREAK etc (ref opencv)
		*/
		string opencv_desc_types[] = {"SIFT", "SURF", "ORB", "BRIEF", "FREAK"};
		const StringVector OPENCV_DESCRIPTOR_TYPES(opencv_desc_types, opencv_desc_types+5);
		descriptor_typeconfig["opencv"] = OPENCV_DESCRIPTOR_TYPES;

		initModule_nonfree();

		// TODO: add self-support or other 3rd party support descriptors
	}

	bool ImgKeypointDescriptorExtractor::CheckTypeValidity(const string& feat_type, string& support_type)
	{
		support_type = "not_support";

		TypeConfig::const_iterator pi = descriptor_typeconfig.begin();
		for( ; pi != descriptor_typeconfig.end(); pi++ )
		{
			if( find(pi->second.begin(), pi->second.end(), feat_type) != pi->second.end() )
			{
				support_type = pi->first;
				return true;
			}
		}

		return false;
	}


	void ImgKeypointDescriptorExtractor::ExtractImgKeypointDescriptors(
		const cv_Images& imgs, vector<cv_KeyPoints>& keypoints, vector<Mat>& descriptors, const string& desc_type)
	{
		// check validity first
		string support_type;
		bool if_valid = CheckTypeValidity(desc_type, support_type);
		if( !if_valid )
		{
			cerr<<"Not support input type."<<endl;
			return;
		}

		if( keypoints.size() != imgs.size() )
		{
			cerr<<"Keypoints number not consistent with images size."<<endl;
			return;
		}
		
		cout<<"Keypoint descriptor support type: "<<support_type<<endl;

		// clear structures
		descriptors.clear();
		descriptors.resize(imgs.size());

		// do detection
		cout<<"Extracting descriptors..."<<endl;
		if( support_type == "opencv" )
		{
			cv_descriptorExtractor = DescriptorExtractor::create(desc_type);
			if(cv_descriptorExtractor == NULL)
			{
				cerr<<"Fail to create descriptor extractor."<<endl;
				return;
			}

			cv_descriptorExtractor->compute(imgs, keypoints, descriptors);
		}
		// TODO: add other descriptors


		cout<<"Finish extracting descriptors."<<endl;

	}

	bool ImgKeypointDescriptorExtractor::SaveDescriptors(
		const string& outfile, const cv_KeyPoints& points, const Mat& descs)
	{
		if(points.size() != descs.rows)
		{
			cerr<<"keypoints number not equal to descriptor number."<<endl;
			return false;
		}

		ofstream out(outfile.c_str());
		// descriptor type(number), number and dimension
		out<<descs.type()<<" "<<descs.rows<<" "<<descs.cols<<endl;
		for(int r=0; r<descs.rows; r++)
		{
			// output keypoint position first
			out<<points[r].pt.x<<" "<<points[r].pt.y;
			// here comes the descriptor
			for(int c=0; c<descs.cols; c++)
			{
				// data type conversion
				switch (descs.depth())
				{
				case CV_8U:
					out<<" "<<descs.at<uchar>(r,c);
					break;
				case CV_32F:
					out<<" "<<descs.at<float>(r,c);
					break;
				case CV_64F:
					out<<" "<<descs.at<double>(r,c);
					break;
				default:
					break;
				}

			}

			out<<endl;
		}

		cout<<"Descriptors saved to "<<outfile<<endl;

		return true;
	}


	bool ImgKeypointDescriptorExtractor::LoadDescriptor(const string& datafile, cv_KeyPoints& keypoints, Mat& desc)
	{
		ifstream in(datafile.c_str());
		
		// read type, sample number and dimension
		int data_type, nums, dims;
		in>>data_type>>nums>>dims;
		switch (data_type)
		{
		case CV_8U:
			desc.create(nums, dims, CV_8U);
			break;
		case CV_32F:
			desc.create(nums, dims, CV_32F);
			break;
		case CV_64F:
			desc.create(nums, dims, CV_64F);
			break;
		default:
			break;
		}

		keypoints.clear();
		keypoints.resize(nums);
		for(int r=0; r<nums; r++)
		{
			in>>keypoints[r].pt.x>>keypoints[r].pt.y;
			for(int c=0; c<dims; c++)
			{
				switch (data_type)
				{
				case CV_8U:
					in>>desc.at<uchar>(r, c);
					break;
				case CV_32F:
					in>>desc.at<float>(r, c);
					break;
				case CV_64F:
					in>>desc.at<double>(r, c);
					break;
				default:
					break;
				}
			}	
		}

		cout<<"Load keypoint descriptors from "<<datafile<<endl;

		return true;


	}


	bool ImgKeypointDescriptorExtractor::LoadSurfDescriptor(const string& datafile, cv_KeyPoints& keypoints, Mat& desc)
	{
		desc.release();

		ifstream in(datafile.c_str());
		if(in.fail())
		{
			cout<<"Fail to load surf from "<<datafile<<endl;
			return false;
		}

		string sample;
		vector<vector<float>> val_mat;
		while(getline(in, sample))
		{
			stringstream sstream(sample);
			
			vector<float> vals;
			float val;
			while(sstream>>val)
				vals.push_back(val);

			val_mat.push_back(vals);
		}

		keypoints.clear();
		keypoints.resize(val_mat.size());
		desc.create(val_mat.size(), val_mat[0].size()-4, CV_32FC1);
		for(size_t r=0; r<val_mat.size(); r++)
		{
			// read x,y
			keypoints[r].pt.x = val_mat[r][0];
			keypoints[r].pt.y = val_mat[r][1];
			for(size_t c=4; c<val_mat[r].size(); c++)
				desc.at<float>(r,c-4) = val_mat[r][c];
		}

		cout<<"Load surf from "<<datafile<<endl;

		return true;

	}
}