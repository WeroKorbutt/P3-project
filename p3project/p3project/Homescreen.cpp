#include "stdafx.h"
#include "Homescreen.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include "iostream"

using namespace std;
using namespace cv;

Homescreen::Homescreen()
{
}


Homescreen::~Homescreen()
{
}

//string winName = "Home page";

Mat Homescreen::run()
{
	Mat hometest2 = imread("home3.png", CV_LOAD_IMAGE_COLOR); // Read the file
	return hometest2;
	// Setup callback function
	/*
	namedWindow(winName);

	//setMouseCallback(winName, CallBackFunc);

	imshow(winName, img);
	//waitKey(0);
	*/
}
