// p3project.cpp : Defines the entry point for the console application.
// penis

#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <opencv2\core.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include<iostream>
#include<cmath>

using namespace std;
using namespace cv;

void newGame() {

	Mat cat1;
	Mat image;
	cat1 = imread("Home_screen.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (cat1.data && !cat1.empty())
		imshow("Grey kat", cat1);
	else if (true)
		cout << "No grey kat" << endl;
}


int main()
{
	newGame();
	Mat testing;

	getchar();
    return 0;
}

