#include "stdafx.h"
#include "Statscreen.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include "iostream"

using namespace std;
using namespace cv;

Statscreen::Statscreen()
{
}


Statscreen::~Statscreen()
{
}

Rect button;
Mat3b canvas;
//string winName = "Profile page";

Mat Statscreen::run()
{
	const Mat img = imread("profile.jpg", CV_LOAD_IMAGE_COLOR); // Read the file
	return img;
	// Setup callback function
	/*
	namedWindow(winName);

	//setMouseCallback(winName, CallBackFunc);

	imshow(winName, img);
	//waitKey(0);
	*/
}
