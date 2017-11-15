#include "stdafx.h"
#include "Game.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/highgui.hpp>
#include <opencv2/photo.hpp>
#include "iostream"

using namespace std;
using namespace cv;

Gamescreen::Gamescreen()
{
}


Gamescreen ::~Gamescreen()
{
}

//string winName = "Profile page";

Mat Gamescreen::run()
{
	Mat statimg = imread("game.jpg", CV_LOAD_IMAGE_COLOR); // Read the file
	return statimg;
	// Setup callback function
	/*
	namedWindow(winName);

	//setMouseCallback(winName, CallBackFunc);

	imshow(winName, img);
	//waitKey(0);
	*/
}
