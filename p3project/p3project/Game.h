#pragma once
#include <opencv2/core/mat.hpp>

class Gamescreen
{
public:
	Gamescreen();
	~Gamescreen();
	cv::Mat run();
	//void CallBackFunc(int event, int x, int y, int flags, void* userdata);
};
#pragma once
