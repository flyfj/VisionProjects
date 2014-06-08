#include "KinectDataMan.h"


KinectDataMan::KinectDataMan(void)
{
	color_reso = NUI_IMAGE_RESOLUTION_640x480;
	depth_reso = NUI_IMAGE_RESOLUTION_640x480;
}


void KinectDataMan::ShowColorDepth()
{
	// init kinect and connect
	if( m_cvhelper.IsInitialized() )
		return;

	m_cvhelper.SetColorFrameResolution(color_reso);
	m_cvhelper.SetDepthFrameResolution(depth_reso);

	HRESULT hr;
	// Get number of Kinect sensors
	int sensorCount = 0;
	hr = NuiGetSensorCount(&sensorCount);
	if (FAILED(hr)) 
	{
		return;
	}

	// If no sensors, update status bar to report failure and return
	if (sensorCount == 0)
	{
		cerr<<"No kinect"<<endl;
	}

	// Iterate through Kinect sensors until one is successfully initialized
	for (int i = 0; i < sensorCount; ++i) 
	{
		INuiSensor* sensor = NULL;
		hr = NuiCreateSensorByIndex(i, &sensor);
		if (SUCCEEDED(hr))
		{
			hr = m_cvhelper.Initialize(sensor);
			if (SUCCEEDED(hr)) 
			{
				// Report success
				cerr<<"Kinect initialized"<<endl;
				break;
			}
			else
			{
				// Uninitialize KinectHelper to show that Kinect is not ready
				m_cvhelper.UnInitialize();
				return;
			}
		}
	}

	DWORD width, height;
	m_cvhelper.GetColorFrameSize(&width, &height);
	Size colorsz(width, height);
	Mat cimg(colorsz, m_cvhelper.COLOR_TYPE);
	m_cvhelper.GetDepthFrameSize(&width, &height);
	Size depthsz(width, height);
	Mat dimg(depthsz, m_cvhelper.DEPTH_RGB_TYPE);

	// start processing
	while(true)
	{
		// get color frame
		if( SUCCEEDED(m_cvhelper.UpdateColorFrame()) )
		{
			HRESULT hr = m_cvhelper.GetColorImage(&cimg);
			if(FAILED(hr))
				break;

			imshow("color", cimg);
			if( waitKey(10) == 'q' )
				break;
		}

		if( SUCCEEDED(m_cvhelper.UpdateDepthFrame()) )
		{
			HRESULT hr = m_cvhelper.GetDepthImageAsArgb(&dimg);
			if(FAILED(hr))
				break;

			imshow("depth", dimg);
			if( waitKey(10) == 'q' )
				break;
		}
	}

	destroyAllWindows();

}
