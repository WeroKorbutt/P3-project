#pragma once
#include <opencv2/core/mat.hpp>

class Settings
{
public:
	Settings();
	~Settings();
	cv::Mat run();
	//void CallBackFunc(int event, int x, int y, int flags, void* userdata);
};

