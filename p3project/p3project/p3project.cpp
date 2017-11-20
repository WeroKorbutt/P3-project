// p3project.cpp : Defines the entry point for the console application.
// penis

#include "stdafx.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "Statscreen.h"
#include "Settings.h"
#include "Homescreen.h"


using namespace std;
using namespace cv;

string current_screen = "profile";
string winName = "P3 Project";
/*int main(int argc, const char** argv)
{

	// Init background substractor
	Ptr<BackgroundSubtractor> bg_model = createBackgroundSubtractorMOG2().dynamicCast<BackgroundSubtractor>();

	// Create empy input img, foreground and background image and foreground mask.
	Mat img, foregroundMask, backgroundImage, foregroundImg;

	// capture video from source 0, which is web camera, If you want capture video from file just replace //by  VideoCapture cap("videoFile.mov")
	VideoCapture cap(0);

	// main loop to grab sequence of input files
	for (;;) {

		bool ok = cap.grab();

		if (ok == false) {

			std::cout << "Video Capture Fail" << std::endl;


		}
		else {

			// obtain input image from source
			cap.retrieve(img, CV_CAP_OPENNI_BGR_IMAGE);
			// Just resize input image if you want
			resize(img, img, Size(640, 480));

			// create foreground mask of proper size
			if (foregroundMask.empty()) {
				foregroundMask.create(img.size(), img.type());
			}

			// compute foreground mask 8 bit image
			// -1 is parameter that chose automatically your learning rate

			bg_model->apply(img, foregroundMask, true ? -1 : 0);

			// smooth the mask to reduce noise in image
			medianBlur(foregroundMask, foregroundMask, 5);
			//make it binary
			Mat binary;

		//cvtColor(foregroundMask, foregroundMask, CV_BGR2GRAY);
				//GaussianBlur(foregroundMask, foregroundMask, Size(11, 11), 3.5, 3.5);


			//threshold mask to saturate at black and white values
			threshold(foregroundMask, foregroundMask, 10, 255, THRESH_BINARY);
			foregroundImg = Scalar::all(0);
			// Copy source image to foreground image only in area with white mask
			img.copyTo(foregroundImg, foregroundMask);

			//Get background image
			bg_model->getBackgroundImage(backgroundImage);

			// Show the results
			imshow("foreground mask", foregroundMask);
			imshow("foreground image", foregroundImg);

			int key6 = waitKey(40);

			if (!backgroundImage.empty()) {

				imshow("mean background image", backgroundImage);
				int key5 = waitKey(40);

			}


		}

	}

	return EXIT_SUCCESS;
}*/







Mat segmentation(Mat src) {
	for (int x = 0; x < src.rows; x++) {
		for (int y = 0; y < src.cols; y++) {
			if (src.at<Vec3b>(x, y) == Vec3b(255, 255, 255)) {
				src.at<Vec3b>(x, y)[0] = 0;
				src.at<Vec3b>(x, y)[1] = 0;
				src.at<Vec3b>(x, y)[2] = 0;
			}
		}
	}
	// Create a kernel that we will use for accuting/sharpening our image

	Mat blured;
	//GaussianBlur(src, blured, Size(3,3), 0, 0);
	
	


	Mat bw;
	cvtColor(src, bw, CV_BGR2GRAY);
	GaussianBlur(bw, blured, Size(9,9),5);
	//medianBlur(bw, blured, 5); // Applyng median blur
	Mat tresh;
	Mat bn;
	threshold(blured, tresh, 110, 255,  CV_THRESH_BINARY);
	bitwise_not(tresh, bn);
	src = bn;
	return src;


}

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		//Anywhere
		if (x > 106 && x < 190)
		{
			if (y > 71 && y < 167)
			{
				current_screen = "home";
				cout << "home" << endl;
			}
			else if (y > 167 && y < 272)
			{
				current_screen = "profile";
				cout << "profile" << endl;

			}
			else if (y > 167 && y < 372)
			{
				current_screen = "settings";
				cout << "settings" << endl;

			}
			else if (y > 372 && y < 472)
			{
				current_screen = "quit";
				cout << "quit" << endl;

			}
		}
		//Within profile
		if (current_screen == "profile")
		{
			
		}
		//cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_RBUTTONDOWN)
	{
		//cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_MBUTTONDOWN)
	{
		//cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
	else if (event == EVENT_MOUSEMOVE)
	{
		//cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;

	}
}

int main()
{
	/*
	namedWindow(winName, CV_WINDOW_AUTOSIZE);

	Statscreen* test = new Statscreen();
	Settings* settingsscreen = new Settings();
	Homescreen* homescreen = new Homescreen();
	Mat dispImage;
	while (true) {
		if (!dispImage.empty()) { dispImage.release(); }
		setMouseCallback(winName, CallBackFunc, NULL);
		if (current_screen == "home")
		{
			dispImage = homescreen->run();
		} else if (current_screen == "profile")
		{
			dispImage = test->run();
			cout << "Profile: " << dispImage.rows << endl;
		} else if (current_screen == "settings")
		{
			
			dispImage = settingsscreen->run();
			cout << "Settings: " << dispImage.rows << endl;
		} else if (current_screen == "quit")
		{
	Mat img = imread("home.jpg", CV_LOAD_IMAGE_COLOR);   // Read the file
	waitKey(1);
	char* source_image = "Source image";
	namedWindow(source_image, CV_WINDOW_AUTOSIZE);
	imshow(source_image, img);
	resizeWindow(source_image, 800, 600);
	cout << "testing" << endl;
	waitKey(0);
	//stat_screen();*/

	///  VVV is the camera loop VVV 
	Mat testing1;
	Mat testing2;
	Mat image = imread("scr2.png");
	
	VideoCapture stream1(0); //change 0 to 1 to get from the plugged in camera instead of webcam.
	if (!stream1.isOpened())
	{
		cout << "Cannot open camera";
	}
	while (true)
	{
		stream1.read(testing1);

		testing2 = segmentation(image);
		
		imshow("cam", testing2);
		if ((waitKey(30) >= 0))
			break;
	}

	getchar();
    return 0;
	//brain();

}

