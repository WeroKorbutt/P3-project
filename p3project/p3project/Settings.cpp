#include "stdafx.h"
#include "Settings.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include "iostream"

using namespace std;
using namespace cv;

Settings::Settings()
{
}


Settings ::~Settings()
{
}

//string winName = "Profile page";

Mat Settings::run()
{
	Mat settimg = imread("settings.jpg", CV_LOAD_IMAGE_COLOR); // Read the file
	return settimg;
}