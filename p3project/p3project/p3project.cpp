// p3project.cpp : Defines the entry point for the console application.
// penis

#include "stdafx.h"
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgcodecs/imgcodecs_c.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "Statscreen.h"
#include "Settings.h"
#include "Homescreen.h"
#include <opencv2\imgproc\imgproc.hpp>
#include <datetimeapi.h>
#include "Shape.h"
#include <random>
#include <thread>
#include <chrono>
#include <Mmsystem.h>


using namespace std;
using namespace cv;
using namespace this_thread; // sleep_for, sleep_until
using namespace chrono; // nanoseconds, system_clock, seconds


string current_screen = "home";
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




int erosion_elem = 1;
int erosion_size = 5;
int dilation_elem = 0;
int dilation_size = 1;
int const max_elem = 2;
int const max_kernel_size = 21;
Mat src, erosion_dst, dilation_dst, fist_dst;
Mat bn;
Mat blured;
Mat tresh, tresh_bin;
Mat spawningprompt = imread("textspawn.png");
Mat timer0 = imread("Fistbar.png");
Mat timer1 = imread("fistbar2.png");
Mat timer2 = imread("fistbar3.png");
Mat timer3 = imread("fistbar4.png");
Mat timer4 = imread("fistbar5.jpg");
int threshold_value = 17;
int threshold_type = 0;
int const max_value = 255;
int const max_type = 4;
int const max_BINARY_value = 255;
int timer;

int width = 800;
int height = 600;
int stepSize = 78;
int projectorStepSize = 45;
int difficulty = 1;

int maxStepSize = 150;
int basex = 96;
int const maxbase = 100 * 5;
int maxbasex = basex + 5 * projectorStepSize;
int basey = 58;
int maxbasey = basey + 5 * projectorStepSize;
int gridSize = 5;
int const gridSizeMax = 15;
Mat dispImage;
Mat gridimage;
Mat grid;
int t = 0;
clock_t beginClock;
clock_t fistClock;
bool clickSpawn = false;
bool checktime = false;
bool checktimefist = false;
bool fistclicked = false;
bool win = false;
bool lose = false; 
bool showshapeimgs = false;
bool spawnfeedback = false; 
double fist_secs;


//Storage for shapes in projector
vector<Shape*>* shapeinput = new vector<Shape*>(25);

vector<Shape*>* shapeCamera = new vector<Shape*>(25);
// Storage for blobs
vector<KeyPoint> squarekeypoints;
vector<KeyPoint> trianglekeypoints;
vector<KeyPoint> circlekeypoints;
vector<KeyPoint> fistkeypoints;

int comparing[5][5] = { };

/** Function Headers */
void Threshold_Demo(int, void*);
void Grid_Demo(int, void*);
void Erosion(int, void*);
void Dilation(int, void*);
void GridCheckingsAndSuch();
void CameraGrid();
void checkPlacement();
char* trackbar_type = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
char* trackbar_value = "Value";


void segmentation()
{
	VideoCapture stream1(1); //change 0 to 1 to get from the plugged in camera instead of webcam.
	if (!stream1.isOpened())
	{
		cout << "Cannot open camera";
	}
	Mat cameraInput;
	Mat testing1;
	Mat segmentedCameraInput;
	Mat image = imread("scary2.png");
	//Mat test = imread("testing.jpg");
	Mat background = imread("background.jpg");
	Mat gridOutput;
	Mat bw;

	while (true)
	{
		stream1.read(cameraInput);
		flip(cameraInput, cameraInput, 1);
		if ((char)waitKey(1) == 32)
		{
			background = cameraInput.clone();
			cout << "Space clicked" << endl;
		}

		segmentedCameraInput = background - cameraInput;


		/// Create Erosion Trackbar
		createTrackbar("Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "erosion",
		               &erosion_elem, max_elem,
		               Erosion);

		createTrackbar("Kernel size:\n 2n +1", "erosion",
		               &erosion_size, max_kernel_size,
		               Erosion);

		/// Create Dilation Trackbar
		createTrackbar("Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "dilation",
		               &dilation_elem, max_elem,
		               Dilation);

		createTrackbar("Kernel size:\n 2n +1", "dilation",
		               &dilation_size, max_kernel_size,
		               Dilation);


		for (int x = 0; x < segmentedCameraInput.rows; x++)
		{
			for (int y = 0; y < segmentedCameraInput.cols; y++)
			{
				if (segmentedCameraInput.at<Vec3b>(x, y) == Vec3b(255, 255, 255))
				{
					segmentedCameraInput.at<Vec3b>(x, y)[0] = 0;
					segmentedCameraInput.at<Vec3b>(x, y)[1] = 0;
					segmentedCameraInput.at<Vec3b>(x, y)[2] = 0;
				}
			}
		}
		// Create a kernel that we will use for accuting/sharpening our image


		//GaussianBlur(src, blured, Size(3,3), 0, 0);




		cvtColor(segmentedCameraInput, bw, CV_BGR2GRAY);
		//GaussianBlur(bw, blured, Size(9,9),5);
		medianBlur(bw, blured, 15); // Applyng median blur

		/// Create Trackbar to choose type of Threshold
		createTrackbar(trackbar_type,
		               "thresholding", &threshold_type,
		               max_type, Threshold_Demo);

		createTrackbar(trackbar_value,
		               "thresholding", &threshold_value,
		               max_value, Threshold_Demo);

		/// Call the function to initialize
		Threshold_Demo(0, 0);
		//threshold(blured, tresh, 109, 255,  CV_THRESH_TRUNC);
		//threshold(tresh, tresh_bin, 108, 255, THRESH_BINARY);

		//bitwise_not(tresh_bin, bn); 
		//bn to erosion_dst
		Erosion(0, 0);
		imshow("erosion", erosion_dst);
		//erosion_dst to dilation_dst
		Dilation(0, 0);
		imshow("dilation", dilation_dst);
		
		//fist_dst = dispImage(Rect(dilation_dst.rows - 152, dilation_dst.cols - 152, 150, 150));
		Rect region_of_interest = Rect(489, 329, 150, 150);
		Mat fist(region_of_interest.size(), dilation_dst.type());
		dilation_dst(region_of_interest).copyTo(fist);
		imshow("fist window", fist);

		fist_dst = fist;

		src = bn;
		// Setup SimpleBlobDetector parameters.
		SimpleBlobDetector::Params params;
		// Filter by Area.
		params.filterByArea = false;
		params.minArea = 50;
		params.maxArea = 700;

		// filter my min distance
		//params.minDistBetweenBlobs=100;


		// Filter by Convexity
		params.filterByConvexity = true;
		params.minConvexity = 0.9;

		// Filter by Inertia
		params.filterByInertia = false;
		params.minInertiaRatio = 0.01;

		//filter by colour
		params.filterByColor = true;
		params.blobColor = 255;


		// Filter by circularity

		//Square
		params.filterByCircularity = true;
		params.minCircularity = 0.80;
		params.maxCircularity = 0.85;

		// Set up detector with params
		Ptr<SimpleBlobDetector> squaredetector = SimpleBlobDetector::create(params);

		//Circle
		params.filterByCircularity = true;
		params.minCircularity = 0.86;
		params.maxCircularity = 1;

		Ptr<SimpleBlobDetector> circledetector = SimpleBlobDetector::create(params);

		//Triangle
		params.filterByCircularity = true;
		params.minCircularity = 0.05;
		params.maxCircularity = 0.79;

		Ptr<SimpleBlobDetector> triangledetector = SimpleBlobDetector::create(params);

		//fist
		params.filterByArea = false;
		params.filterByCircularity = true;
		params.minCircularity = 0.05;
		params.maxCircularity = 0.78;

		Ptr<SimpleBlobDetector> fistdetector = SimpleBlobDetector::create(params);


		// Detect blobs
		squaredetector->detect(dilation_dst, squarekeypoints);
		triangledetector->detect(dilation_dst, trianglekeypoints);
		circledetector->detect(dilation_dst, circlekeypoints);
		fistdetector->detect(fist_dst, fistkeypoints);

		//the total no of blobs detected are:
		size_t squareamount = squarekeypoints.size();
		size_t triangleamount = trianglekeypoints.size();
		size_t circleamount = circlekeypoints.size();
		size_t fistamount = fistkeypoints.size();
		//cout << "total no of squares detected are:" << squareamount << endl << "total no of triangles detected are: " << triangleamount << endl;


		//location of first blob
		if (squareamount != 0)
		{
			Point2f point1 = squarekeypoints.at(0).pt;
			float x1 = point1.x;
			float y1 = point1.y;
			//cout << "location of the square blob is " << x1 << "," << y1;
		}

		if (triangleamount != 0)
		{
			Point2f point1 = trianglekeypoints.at(0).pt;
			float x1 = point1.x;
			float y1 = point1.y;
			//cout << "location of the triangle blob is " << x1 << "," << y1 << endl;
		}

		if (circleamount != 0)
		{
			Point2f point1 = circlekeypoints.at(0).pt;
			float x1 = point1.x;
			float y1 = point1.y;
		}

		clock_t fistend;
		if (fistamount != 0)
		{
			//if (!fistclicked)
			fistclicked = true; 
			if (t = 0) {
				fistClock = clock();
				t = 1;
			}
			//}
		}
		else
		{
			t = 0;
			fistclicked = false;
			fistend = clock();

		}
		if (!fistclicked)
		{
			fistClock = clock();
		}
		fistend = clock();
			fist_secs = double(fistend - fistClock) / CLOCKS_PER_SEC;
			cout << fist_secs << endl;
			if (fist_secs > 4)
			{
				//fistClock = clock();
				fistclicked = false;
				if (win == false && lose == false) {
					checkPlacement();
				}

				else if (win == true || lose == true)
				{
					//spawningprompt.copyTo(grid(Rect(grid.cols / 4, grid.rows / 4, spawningprompt.cols, spawningprompt.rows)));
					win = false;
					lose = false; 
					shapeinput->clear();
					shapeCamera->clear();
					shapeinput = new vector<Shape*>(25);
					shapeCamera = new vector<Shape*>(25);
					clickSpawn = true;
					//showshapeimgs = true;
				}
					fistClock = clock();
					t = 0;
				

			}

		//}

		// Draw detected blobs as red circles.
		// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
		// the size of the circle corresponds to the size of blob

		Mat im_with_squarekeypoints;
		Mat im_with_trianlgekeypoints;
		Mat im_with_circlekeypoints;
		Mat im_with_fistkeypoints;

		drawKeypoints(dilation_dst, squarekeypoints, im_with_squarekeypoints, Scalar(0.0, 0.0, 255),
		              DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		drawKeypoints(dilation_dst, trianglekeypoints, im_with_trianlgekeypoints, Scalar(0.0, 0.0, 255),
		              DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		drawKeypoints(dilation_dst, circlekeypoints, im_with_circlekeypoints, Scalar(0.0, 0.0, 255),
		              DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		drawKeypoints(fist_dst, fistkeypoints, im_with_fistkeypoints, Scalar(0.0, 0.0, 255),
		              DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

		//Show blobs
		Mat c = im_with_squarekeypoints + im_with_trianlgekeypoints;

		//Make camera grid
		CameraGrid();

		imshow("Squares", im_with_squarekeypoints);
		imshow("Triangle", im_with_trianlgekeypoints);
		imshow("Circle", im_with_circlekeypoints);
		imshow("original", cameraInput);
		imshow("edited", segmentedCameraInput);
		//imshow("cam", testing3);
		imshow("background", background);
		imshow("Fisting", im_with_fistkeypoints);
	}
	//return src;
}

void spawnFeedback()
{
	clock_t spawning = clock();
	while (spawnfeedback == true)
	{
		spawningprompt.copyTo(grid(Rect(grid.cols / 4, grid.rows / 4, spawningprompt.cols, spawningprompt.rows)));
		clock_t spawnEnd = clock();
		double elapsed_secs = double(spawnEnd - spawning) / CLOCKS_PER_SEC;
		imshow("GRID", grid);
		if (elapsed_secs > 3)
		{
			spawnfeedback = false;
		}

	}
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{

	if (event == EVENT_LBUTTONDOWN)
	{
		//showloadingimage = true;
		if (!clickSpawn && !checktime)
		{
			clickSpawn = true; cout << "Spawning" << endl;
			//showshapeimgs = true;

		}
		////Anywhere



		//if (x > 106 && x < 190)
		//{
		//	if (y > 71 && y < 167)
		//	{
		//		
		//		current_screen= "home";
		//		cout << "home" << endl;
		//	}
		//	else if (y > 167 && y < 272)
		//	{
		//		current_screen = "profile";
		//		cout << "profile" << endl;

		//	}
		//	else if (y > 167 && y < 372)
		//	{
		//		current_screen = "settings";
		//		cout << "settings" << endl;

		//	}
		//	else if (y > 372 && y < 472)
		//	{
		//		current_screen = "quit";
		//		cout << "quit" << endl;

		//	}
		//}
		////Within profile
		//if (current_screen == "profile")
		//{
		//	
		//}
		////cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
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


void GridCheckingsAndSuch()
{
	namedWindow("Grid1");
	/*
	int width = 800;
	int height = 600;
	int stepSize = 70;
	int buffersize = 12;
	int gridS = 1000;
	int base = 100;
	int maxbase = width;
	int basey = 100;
	int maxbasey = height;
	int end = base + 5*stepSize;
	*/
	int buffersize = 12;
	

	createTrackbar("x",
		"Grid1", &basex,
		maxbase, Grid_Demo);

	createTrackbar("y",
		"Grid1", &basey,
		maxbase, Grid_Demo);
	createTrackbar("Step size",
		"Grid1", &projectorStepSize,
		maxStepSize, Grid_Demo);
	grid = dispImage.clone();
	createTrackbar("Grid size",
		"Grid1", &gridSize,
		gridSizeMax, Grid_Demo);
	grid = dispImage.clone();
	


		

	for (int x = 0; x < grid.rows; x++)
	{
		for (int y = 0; y < grid.cols; y++)
		{
			//mat.at<uchar>(row, column, channel) = val;
			grid.at<uchar>(x, y) = 255;
			//grid.at<Vec3b>(Point(x,y)) = (255, 255, 255);
		}
	}

	Grid_Demo(0, 0);



	

	//imshow("GRID", gridimage);

	
	/*
	for (int i = 0; i < 5; i += 1) {
		for (int j = 0; j < 5; j += 1) {
			int VALUEXMAX = (((width / 5) * (i + 1)) - buffersize);
			int VALUEXMIN = (((width / 5) * i) + buffersize);
			int VALUEYMAX = (((height / 5) * (j + 1)) - buffersize);
			int VALUEYMIN = (((height / 5) * j) + buffersize);
			for (int k = 0; k < squarekeypoints.size(); k += 1) {
				if (squarekeypoints.operator[](k).pt.x - 100 < VALUEXMAX && squarekeypoints.operator[](k).pt.x - 100 > VALUEXMIN && squarekeypoints.operator[](k).pt.y - 144 < VALUEYMAX && squarekeypoints.operator[](k).pt.y - 144 > VALUEYMIN)
				{
					cout << "square in grid " << i << ", " << j << " my man" << endl;
				}
			}
		}
	}
	*/

	Mat triangle = imread("triangle.png");
	Mat square = imread("square.png");
	Mat circle = imread("circle.png");

	/*
	int difficulty = 4;
	//Based on difficulty (spawn number of shapes)
	vector<Shape*>* shapeinput = spawnShapes(difficulty);
	cout << "Amount of shapes shown " << shapeinput->size() << endl;
	//calculate random int(s) for the x and y axis
	
	for (int i = 0; i < shapeinput->size(); i++)
	{
		cout << "X: " << shapeinput->operator[](i)->getGridx() << ", Y: " << shapeinput->operator[](i)->getGridy() << ", Type: " << shapeinput->operator[](i)->getType() << endl;
	}


	for (int i = 0; i < shapeinput->size(); i++)
	{
		shapeinput->operator[](i)->getImage().copyTo(a(Rect((shapeinput->operator[](i)->getGridx()*50)+257, (shapeinput->operator[](i)->getGridy()*50) + 210, shapeinput->operator[](i)->getImage().cols, shapeinput->operator[](i)->getImage().rows)));
	}



	//imshow("grid", gridimage);
	*/
}


void CameraGrid()
{
	//Mat gridimage = dilation_dst(Rect(dilation_dst.rows-280, 120, 280, 280));
	Mat gridimage = dilation_dst.clone()(Rect(dilation_dst.rows - 280, 120, 280, 280));

	int width = gridimage.rows;
	int height = gridimage.cols;
	stepSize = width / gridSize;


	for (int i = 0; i <= 5; i++)
	{
		line(gridimage, Point(0, i*stepSize), Point(5 * stepSize, i*stepSize), Scalar(255, 255, 255));
	}
	for (int i = 0; i <= 5; i++)
	{
		line(gridimage, Point(i*stepSize, 0), Point(i*stepSize, 5 * stepSize), Scalar(255, 255, 255));
	}

	//for (int i = 0; i < height; i += stepSize)
	//	line(gridimage, Point(0, i), Point(width, i), Scalar(255, 255, 255));
	//for (int i = 0; i < width; i += stepSize)
	//	line(gridimage, Point(i, 0), Point(i, height), Scalar(255, 255, 255));


	imshow("cameragrid", gridimage);

}


void checkPlacement()
{
	Mat gridimage = dilation_dst(Rect(dilation_dst.rows - 280, 120, 280, 280));

	int width = gridimage.rows;
	int height = gridimage.cols;

	for (int i = 0; i < gridSize; i += 1) {
		for (int j = 0; j < gridSize; j += 1) {
			int buffersize = 12;
			int VALUEXMAX = (((width / gridSize) * (i + 1)) - buffersize);
			int VALUEXMIN = (((width / gridSize) * i) + buffersize);
			int VALUEYMAX = (((height / gridSize) * (j + 1)) - buffersize);
			int VALUEYMIN = (((height / gridSize) * j) + buffersize);
			for (int k = 0; k < squarekeypoints.size(); k += 1) {
				if (squarekeypoints.operator[](k).pt.x - (dilation_dst.rows - 280) < VALUEXMAX && squarekeypoints.operator[](k).pt.x - (dilation_dst.rows - 280) > VALUEXMIN && squarekeypoints.operator[](k).pt.y - 120 < VALUEYMAX && squarekeypoints.operator[](k).pt.y - 120 > VALUEYMIN)
				{
					//if (shapeCamera->operator[](i + j*gridSize) != NULL)
					//	shapeCamera->erase(shapeCamera->begin() + (i + j*gridSize));
					shapeCamera->operator[](i + j*gridSize) = new Shape(squarekeypoints.operator[](k).pt.x, squarekeypoints.operator[](k).pt.y, 0);

					//comparing[i][j] = 1;

					cout << "square in grid " << i << ", " << j << " my man" << endl;
				}
			}
			for (int k = 0; k < trianglekeypoints.size(); k += 1) {
				if (trianglekeypoints.operator[](k).pt.x - (dilation_dst.rows - 280) < VALUEXMAX && trianglekeypoints.operator[](k).pt.x - (dilation_dst.rows - 280) > VALUEXMIN && trianglekeypoints.operator[](k).pt.y - 120 < VALUEYMAX && trianglekeypoints.operator[](k).pt.y - 120 > VALUEYMIN)
				{
					//if (shapeCamera->operator[](i + j*gridSize) != NULL)
					//	shapeCamera->erase(shapeCamera->begin() + (i + j*gridSize));
					shapeCamera->operator[](i + j*gridSize) = new Shape(trianglekeypoints.operator[](k).pt.x, trianglekeypoints.operator[](k).pt.y, 1);

					//comparing[i][j] = 2;


					cout << "triangle in grid " << i << ", " << j << " my man" << endl;
				}
			}
			for (int k = 0; k < circlekeypoints.size(); k += 1) {
				if (circlekeypoints.operator[](k).pt.x - (dilation_dst.rows - 280) < VALUEXMAX && circlekeypoints.operator[](k).pt.x - (dilation_dst.rows - 280) > VALUEXMIN && circlekeypoints.operator[](k).pt.y - 120 < VALUEYMAX && circlekeypoints.operator[](k).pt.y - 120 > VALUEYMIN)
				{
					//if (shapeCamera->operator[](i + j*gridSize) != NULL) {
						//shapeCamera->erase(shapeCamera->begin() + (i + j*gridSize));
						shapeCamera->operator[](i + j*gridSize) = new Shape(circlekeypoints.operator[](k).pt.x, circlekeypoints.operator[](k).pt.y, 2);
					//}
					//comparing[i][j] = 3;


					cout << "circle in grid " << i << ", " << j << " my man" << endl;
				}
			}
		}
	}
	for (int i = 0; i < gridSize; i += 1) {
		for (int j = 0; j < gridSize; j += 1) {
			if (!shapeCamera->operator[](i + j*gridSize) == NULL) {
				cout << "Test " << shapeCamera->operator[](i + j*gridSize)->getType() << endl;
			}
		}
	}

	
	//for (int i = 0; i < 4; i += 1) {
	//	for (int j = 0; j < 4; j += 1) {
	//		cout << comparing[i][j] << endl;
	//	}
	//}
	cout << "works" << endl;
	int counter = 0;
	for (int i = 0; i < gridSize; i += 1) {
		for (int j = 0; j < gridSize; j += 1) {
			if (shapeinput->operator[](i + j*gridSize) == NULL && shapeCamera->operator[](i + j*gridSize) == NULL) {
				cout << "both are null" << endl;
			}
			else if (shapeinput->operator[](i + j*gridSize) == NULL && !shapeCamera->operator[](i + j*gridSize) == NULL || !shapeinput->operator[](i + j*gridSize) == NULL && shapeCamera->operator[](i + j*gridSize) == NULL) {
				cout << "one is null" << endl;
				counter++;
			}
			else {	
				
				if (shapeinput->operator[](i + j*gridSize)->getType() == shapeCamera->operator[](i + j*gridSize)->getType())
				{
					cout << i << " , " << j << " correct" << endl; 

				}
				else
				{
					cout << i << " , " << j << " git gud scrub" << endl;
					counter++;
				}
			}
		}
	}
	cout << "change difficulty 1: " << difficulty << endl;
		cout << "count 1: " << counter << endl;
	if (counter == 0)
	{
		win = true;
		difficulty++;
	}
	else
	{
		lose = true;
		if (difficulty != 1) {
			difficulty--;
		}
	}
	cout << "change difficulty 2: " << difficulty << endl;
	cout << "count 2: " << counter << endl;
	for (int i = 0; i < shapeCamera->size(); i++)
	{
//		cout << "Element: " << i << ", X: " << shapeCamera->operator[](i)->getGridx() << ", Y: " << shapeCamera->operator[](i)->getGridy() << ", Type: " << shapeCamera->operator[](i)->getType() << endl;
	}
}

//vector<Shape*>* 
void calcShapes()
{
	vector<Point*> previouslocations;
	shapeinput->clear();
	shapeinput = new vector<Shape*>(25);
	int shapex;
	int shapey;
	int shapetype;
	bool changed = false;
	bool different = false;
	for (int i = 0; i < difficulty; i++)
	{
		while (different == false)
		{
			srand(time(NULL));

			shapex = rand() % gridSize;
			shapey = rand() % gridSize;
			shapetype = rand() % 3;
			if (previouslocations.size() == 0)
			{
				break;
			}
			//cout << different << endl;
			for (int j = 0; j < previouslocations.size(); j++)
			{
				if (shapex == previouslocations.operator[](j)->x && shapey == previouslocations.operator[](j)->y)
				{
					changed = true;
				}
				else
				{
					different = true;
					//cout << "set to true" << endl;
				}
			}
			if (changed)
			{
				different = false;
				changed = false;
			}
		}
		previouslocations.push_back(new Point(shapex, shapey));
		different = false;
		shapeinput->operator[](shapex + shapey *gridSize) = new Shape(shapex, shapey, shapetype);
	}
	previouslocations.clear();

}


void spawnShape(bool spawn)
{
	//Based on difficulty (spawn number of shapes)
	if (spawn == true)
	{
		//shapeinput->empty();
		//shapeinput = 
		calcShapes();
		beginClock = clock();
		checktime = true;
		showshapeimgs = true;
		spawnfeedback = false;
	}
	//cout << "Amount of shapes shown " << shapeinput->size() << endl;
	//calculate random int(s) for the x and y axis

	/*for (int i = 0; i < shapeinput->size(); i++)
	{
		cout << "X: " << shapeinput->operator[](i)->getGridx() << ", Y: " << shapeinput->operator[](i)->getGridy() << ", Type: " << shapeinput->operator[](i)->getType() << endl;
	}*/
	clickSpawn = false;

}

/*void Comparison()
{
	for (int i = 0; i < shapeinput->size(); i++)
		if (== (trianglekeypoints.operator[](i).pt.x, trianglekeypoints.operator[](i).pt.y, 1);
}
*/





int main()
{
	namedWindow(winName, CV_WINDOW_AUTOSIZE);

	Statscreen* test = new Statscreen();
	Settings* settingsscreen = new Settings();
	Homescreen* homescreen = new Homescreen();
	Mat winscreen = imread("correct.png");
	Mat losescreen = imread("wrongicon.png");
	
	dispImage = homescreen->run();

	//namedWindow("GRID", 1);
	cvNamedWindow("GRID", CV_WINDOW_NORMAL);
	moveWindow("GRID", 1981, 0);
	cvSetWindowProperty("GRID", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	setMouseCallback("GRID", CallBackFunc, NULL);

	//Creates a thread that runs the image processing part of the prototype
	thread imageprocessing(segmentation);
	//thread spawnimagethread;
	thread* spawnShapesThread = new thread();
	//bool checktime = false;
	while (true) {

		waitKey(1);
		gridimage = dispImage;
		//testing


		//GridCheckingsAndSuch();
		//cout << basex << " " << basey << endl;

		//Create grid-system on projector
		GridCheckingsAndSuch();
		//Spawn shapes in grid-system
		
		//Show shapes in grid
		if (clickSpawn == true)
		{
			spawnfeedback = true;
			//spawnimagethread = thread{ spawnFeedback };
			spawnShapesThread = new thread{ spawnShape, clickSpawn };

			//if (spawnShapesThread.joinable()) spawnShapesThread.join();

			//spawnimagethread.join();

			//if (spawn == true) spawn = false;
		}


		if (spawnfeedback)
		{
			spawningprompt.copyTo(grid(Rect(grid.cols / 4, grid.rows / 2, spawningprompt.cols, spawningprompt.rows)));
		} else
		{
			if (!spawnShapesThread == NULL) {
				if (spawnShapesThread->joinable())
				{
					spawnShapesThread->join();
				}
			}
		}
		if (showshapeimgs) {

			for (int i = 0; i < shapeinput->size(); i++)
			{
				if (!shapeinput->operator[](i) == NULL) {
					shapeinput->operator[](i)->getImage().copyTo(grid(Rect((shapeinput->operator[](i)->getGridx()*(projectorStepSize * 2)) + basex * 2 + 18, (shapeinput->operator[](i)->getGridy()*(projectorStepSize * 2)) + basey * 2 + 18, shapeinput->operator[](i)->getImage().cols, shapeinput->operator[](i)->getImage().rows)));
				}
			}
		}

		



		//Mat gridOutput;
		//gridOutput = grid;

		//sleep_for(seconds(25));
		//shapeinput->erase(shapeinput->begin(), shapeinput->begin() + shapeinput->size());
		//GridCheckingsAndSuch();




		if (checktime)
		{
			clock_t end = clock();
			double elapsed_secs = double(end - beginClock) / CLOCKS_PER_SEC;
			//cout << elapsed_secs << endl;
			
			if (elapsed_secs > 5)
			{
				beginClock = clock();
				showshapeimgs = false;
				//shapeinput->erase(shapeinput->begin(), shapeinput->begin() + shapeinput->size());
				checktime = false;
			}

		}

		if (win == true)
		{
			winscreen.copyTo(grid(Rect(grid.cols/4, grid.rows/4, winscreen.cols, winscreen.rows)));
			
		}

		if (lose == true)
		{
			losescreen.copyTo(grid(Rect(grid.cols / 4, grid.rows / 4, losescreen.cols, losescreen.rows)));
		}

		if (fist_secs > 0 && fist_secs < 1.0)
		{
			timer0.copyTo(grid(Rect(669, 189, timer0.cols, timer0.rows)));
		}
		else if (1.0 < fist_secs && fist_secs < 2.0)
		{
			timer1.copyTo(grid(Rect(669, 189, timer1.cols, timer1.rows)));
		}
		else if (2.0 < fist_secs && fist_secs < 3.0)
		{
			timer2.copyTo(grid(Rect(669, 189, timer2.cols, timer2.rows)));
		}
		else if (3.0 < fist_secs && fist_secs < 4.0)
		{
			timer3.copyTo(grid(Rect(669, 189, timer3.cols, timer3.rows)));
		}
		else if (4.0 < fist_secs)
		{
			timer4.copyTo(grid(Rect(669, 189, timer4.cols, timer4.rows)));
		}
		//if (showloadingimage == true)
		//{
		//	spawningprompt.copyTo(grid(Rect(grid.cols / 4, grid.rows / 4, spawningprompt.cols, spawningprompt.rows)));
		//hey
		//}

		imshow("Grid1", dispImage);
		//if (spawnimagethread.joinable()) spawnimagethread.join();
		imshow("GRID", grid);
	}


	//this is a test comment

	//waitKey(0);
	//testing
}




/*
while (true) {
if (!dispImage.empty()) {
dispImage.release();
setMouseCallback(winName, CallBackFunc, NULL);

if (current_screen == "home")
{
dispImage = homescreen->run();
}
else if (current_screen == "profile")
{
dispImage = test->run();
cout << "Profile: " << dispImage.rows << endl;
}
else if (current_screen == "settings")
{

dispImage = settingsscreen->run();
cout << "Settings: " << dispImage.rows << endl;
}
else if (current_screen == "quit")
{
Mat img = imread("home.jpg", CV_LOAD_IMAGE_COLOR);   // Read the file
waitKey(1);
char* source_image = "Source image";
namedWindow(source_image, CV_WINDOW_AUTOSIZE);
imshow(source_image, img);
resizeWindow(source_image, 800, 600);
cout << "testing" << endl;
waitKey(0);
//stat_screen();
}
imshow("window", dispImage);
waitKey();
///  VVV is the camera loop VVV
Mat testing1;
Mat testing2;
Mat image = imread("scary2.png");
Mat test = imread("testing.jpg");
Mat background = imread("background.jpg");

/*
VideoCapture stream1(1); //change 0 to 1 to get from the plugged in camera instead of webcam.
if (!stream1.isOpened())
{
cout << "Cannot open camera";
}

*/

/*
while (true)
{
stream1.read(testing1);
if ((char)waitKey(1) == 32)
{
background = testing1.clone();
cout << "Space clicked" << endl;
}

testing2 = background - testing1;


/// Create Erosion Trackbar
createTrackbar("Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "erosion",
&erosion_elem, max_elem,
Erosion);

createTrackbar("Kernel size:\n 2n +1", "erosion",
&erosion_size, max_kernel_size,
Erosion);

/// Create Dilation Trackbar
createTrackbar("Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", "dilation",
&dilation_elem, max_elem,
Dilation);

createTrackbar("Kernel size:\n 2n +1", "dilation",
&dilation_size, max_kernel_size,
Dilation);


Mat testing3;
//testing3 = segmentation(testing2);
//GridCheckingsAndSuch();
//imshow("original", testing1);
//imshow("edited", testing2);
//imshow("cam", testing3);
//imshow("background", background);

}

}
*/
	//	}


		//getchar();
		//return 0;
		//brain();


//	}
	//*/



void Threshold_Demo(int, void*)
{
	/* 0: Binary
	1: Binary Inverted
	2: Threshold Truncated
	3: Threshold to Zero
	4: Threshold to Zero Inverted
	*/

	threshold(blured, bn, threshold_value, max_BINARY_value, threshold_type);

	imshow("thresholding", bn);
}

void Grid_Demo(int, void*)
{
	for (int i = 0; i <= gridSize; i++)
	{
		line(grid, Point(2*basex, 2*basey + 2*(i*projectorStepSize)), Point(2*basex + 2*(gridSize * projectorStepSize), 2*basey + 2*(i*projectorStepSize)), Scalar(0, 0, 0));
	}
	for (int i = 0; i <= gridSize; i++)
	{
		line(grid, Point(2*basex + 2*(i*projectorStepSize), 2*basey), Point(2*basex + 2*(i*projectorStepSize), 2*basey + 2*(gridSize * projectorStepSize)), Scalar(0, 0, 0));
	}
}

/**  @function Erosion  */
void Erosion(int, void*)
{
	int erosion_type;
	if (erosion_elem == 0) { erosion_type = MORPH_RECT; }
	else if (erosion_elem == 1) { erosion_type = MORPH_CROSS; }
	else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

	Mat element = getStructuringElement(erosion_type,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));

	/// Apply the erosion operation
	erode(bn, erosion_dst, element);
}

/** @function Dilation */
void Dilation(int, void*)
{
	int dilation_type;
	if (dilation_elem == 0) { dilation_type = MORPH_RECT; }
	else if (dilation_elem == 1) { dilation_type = MORPH_CROSS; }
	else if (dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }

	Mat element = getStructuringElement(dilation_type,
		Size(2 * dilation_size + 1, 2 * dilation_size + 1),
		Point(dilation_size, dilation_size));
	/// Apply the dilation operation
	dilate(erosion_dst, dilation_dst, element);
}

