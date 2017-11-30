#pragma once
#include <opencv2/core/mat.hpp>

class Shape
{
	int gridx;
	int gridy;
	int type; //0 = square, 1 = triangle, 2 = circle
	cv::Mat img;
public:
	Shape(int gridx, int gridy, int type);
	int getGridx();
	int getGridy();
	int getType();

	cv::Mat getImage();
	~Shape();
};


