#include "stdafx.h"
#include "Statscreen.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

Statscreen::Statscreen()
{
}


Statscreen::~Statscreen()
{
}

void Statscreen::run()
{
	const Mat img = imread("profile.jpg", CV_LOAD_IMAGE_COLOR); // Read the file
	waitKey(1);
	char* source_image = "Profile page";
	namedWindow(source_image, CV_WINDOW_AUTOSIZE);
	resizeWindow(source_image, 800, 600);
	imshow(source_image, img);
	waitKey(0);
}
