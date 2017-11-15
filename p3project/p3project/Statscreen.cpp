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

//string winName = "Profile page";

Mat Statscreen::run()
{
	Mat statimg = imread("profile.jpg", CV_LOAD_IMAGE_COLOR); // Read the file
	return statimg;
	// Setup callback function
	/*
	namedWindow(winName);

	//setMouseCallback(winName, CallBackFunc);

	imshow(winName, img);
	//waitKey(0);
	*/
}
