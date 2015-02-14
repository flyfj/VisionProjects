

#include "VideoEventDemo.h"

#ifdef _DEBUG
#pragma comment(lib, "opencv_core249d.lib")
#pragma comment(lib, "opencv_highgui249d.lib")
#pragma comment(lib, "opencv_imgproc249d.lib")
#pragma comment(lib, "opencv_video249d.lib")
#pragma comment(lib, "opencv_gpu249d.lib")
#pragma comment(lib, "libconfig-x86-v120-mt-sgd-1_4_9_4.lib")

#else
#pragma comment(lib, "opencv_core249.lib")
#pragma comment(lib, "opencv_highgui249.lib")
#pragma comment(lib, "opencv_imgproc249.lib")
#pragma comment(lib, "opencv_video249.lib")
#pragma comment(lib, "opencv_gpu249.lib")
#endif

int AnalyzerParams::TRAIN_SAMPLE_NUM = 100;
float AnalyzerParams::ANOMALY_TH = 1e-10;
int AnalyzerParams::grid_x = 1;
int AnalyzerParams::grid_y = 1;
bool AnalyzerParams::USE_GPU = false;

int main(int argc, char* argv[])
{
	if (argc != 2) {
		cerr << "Usage: program.exe video_path. Use app.cfg to change settings." << endl;
		return -1;
	}

	// load params
	try
	{
		libconfig::Config cfg;
		cfg.readFile("app.cfg");
		cfg.lookupValue("detector.train_sample_num", AnalyzerParams::TRAIN_SAMPLE_NUM);
		cfg.lookupValue("detector.anomaly_th", AnalyzerParams::ANOMALY_TH);
		cfg.lookupValue("detector.use_gpu", AnalyzerParams::USE_GPU);
		cfg.lookupValue("detector.scene_grid.[0]", AnalyzerParams::grid_x);
		cfg.lookupValue("detector.scene_grid.[1]", AnalyzerParams::grid_y);
	}
	catch (std::exception e)
	{
		cerr << "Fail to load configuration file, will use default values instead." << endl;
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

	string video_fn(argv[1]);
	
	VideoEventDemo demo;
	demo.RunVideo(video_fn);

	cout << endl << "Input anything to close the window: " << endl;
	getchar();
	return 0;
}