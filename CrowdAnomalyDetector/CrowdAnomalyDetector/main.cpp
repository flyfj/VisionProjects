

#include "VideoEventDemo.h"
#include "Tools/IniParser.hpp"

#ifdef _DEBUG
#pragma comment(lib, "opencv_core249d.lib")
#pragma comment(lib, "opencv_highgui249d.lib")
#pragma comment(lib, "opencv_imgproc249d.lib")
#pragma comment(lib, "opencv_video249d.lib")
#pragma comment(lib, "opencv_gpu249d.lib")
//#pragma comment(lib, "libconfig-x86-v120-mt-sgd-1_4_9_4.lib")

#else
#pragma comment(lib, "opencv_core249.lib")
#pragma comment(lib, "opencv_highgui249.lib")
#pragma comment(lib, "opencv_imgproc249.lib")
#pragma comment(lib, "opencv_video249.lib")
#pragma comment(lib, "opencv_gpu249.lib")
#endif

// init parameters
int AnalyzerParams::TRAIN_SAMPLE_NUM = 100;
float AnalyzerParams::ANOMALY_TH = 1e-10;
int AnalyzerParams::grid_x = 1;
int AnalyzerParams::grid_y = 1;
bool AnalyzerParams::USE_GPU = false;
string AnalyzerParams::LOG_FILE = "log.txt";
string AnalyzerParams::LABEL_FILE = "labels.txt";
bool AnalyzerParams::USE_IP_CAM = false;
string AnalyzerParams::data_input = "";
bool AnalyzerParams::DO_DEBUG = false;

int main(int argc, char* argv[])
{
	// parse params
	if (argc != 1) {
		cerr << "No parameters needed. Use app.cfg to change settings." << endl;
		return -1;
	}

	// load params
	try
	{
		// NOTE: must pass path like this due to win32 api
		IniParser parser(".\\app.ini");
		AnalyzerParams::DO_DEBUG = static_cast<bool>(parser.ReadInt("system", "debug"));
		AnalyzerParams::TRAIN_SAMPLE_NUM = parser.ReadInt("system", "train_sample_num");
		AnalyzerParams::grid_x = parser.ReadInt("system", "scene_grid_x");
		AnalyzerParams::grid_y = parser.ReadInt("system", "scene_grid_y");	
		AnalyzerParams::ANOMALY_TH = parser.ReadFloat("system", "anomaly_th");
		AnalyzerParams::USE_GPU = static_cast<bool>(parser.ReadInt("system", "use_gpu"));
		AnalyzerParams::LOG_FILE = parser.ReadString("system", "log_file");
		AnalyzerParams::LABEL_FILE = parser.ReadString("system", "label_file");
		AnalyzerParams::USE_IP_CAM = static_cast<bool>(parser.ReadInt("data source", "use_cam"));
		AnalyzerParams::data_input = parser.ReadString("data source", "input");
	}
	catch (std::exception e)
	{
		cerr << "Fail to load configuration file, will use default values instead. Make sure the format of ini file is correct." << endl;
	}

	if (AnalyzerParams::USE_GPU) {
		try {
			cout << "Using GPU device: " << cv::gpu::getDevice() << endl;
			gpu::DeviceInfo gpu_info(gpu::getDevice());
			cout << "GPU name: " << gpu_info.name() << endl;
			cout << "GPU multiprocessor count: " << gpu_info.multiProcessorCount() << endl;
			cout << "GPU total memory: " << gpu_info.totalMemory() << " , free memory: " << gpu_info.freeMemory() << endl;
		}
		catch (cv::Exception ex) {
			cout << ex.msg << endl;
			cout << "switch to cpu" << endl;
			AnalyzerParams::USE_GPU = false;
		}
	}
	else
		cout << "Using CPU" << endl;

	VideoEventDemo demo;
	demo.Run(AnalyzerParams::data_input);
	
	cout << endl << "Input anything to close the window: " << endl;
	getchar();
	return 0;
}