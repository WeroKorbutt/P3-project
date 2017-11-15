#pragma once
#include <opencv2/core/mat.hpp>

class Statscreen
{
public:
	Statscreen();
	~Statscreen();
	cv::Mat run();
	//void CallBackFunc(int event, int x, int y, int flags, void* userdata);
};

