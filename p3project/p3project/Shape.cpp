#include "stdafx.h"
#include "Shape.h"
#include <opencv2/imgcodecs.hpp>

using namespace cv;

Shape::Shape(int x, int y, int type)
{
	this->gridx = x;
	this->gridy = y;
	this->type = type;

	if (this->type == 0)
	{
		this->img = imread("square.jpg");
	} else if (this->type == 1)
	{
		this->img = imread("triangle.jpg");
	} else
	{
		this->img = imread("circle.jpg");
	}

	//switch (type)
	//{
	//case 0:
	//	this->img = cv::imread("square.png");
	//	break;

	//case 1:
	//	this->img = cv::imread("triangle.png");
	//	break;

	//case 2:
	//	this->img = cv::imread("circle.png");
	//	break;
	//}

}


Shape::~Shape()
{
}

int Shape::getGridx()
{
	return this->gridx;
}
int Shape::getGridy()
{
	return this->gridy;
}
int Shape::getType()
{
	return this->type;
}

cv::Mat Shape::getImage()
{
	return this->img;
}
