#pragma once
#include <opencv2/core/mat.hpp>

class Homescreen
{
public:
	Homescreen();
	~Homescreen();
	cv::Mat run();
	//void CallBackFunc(int event, int x, int y, int flags, void* userdata);
};

