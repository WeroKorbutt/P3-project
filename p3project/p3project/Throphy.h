#pragma once
#include <opencv2\opencv.hpp>
#include <string>

using namespace std;
using namespace cv;
class Throphy
{
	cv::Mat image_mat_;
	std::string description_string_;

public:
	Throphy();
	~Throphy();
};

