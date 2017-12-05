#include "stdafx.h"
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include "Statscreen.h"
#include "Settings.h"
#include "Homescreen.h"
#include <opencv2\imgproc\imgproc.hpp>
#include "Shape.h"
#include <random>
#include <thread>
#include <chrono>


using namespace std;
using namespace cv;
using namespace this_thread; // sleep_for, sleep_until
using namespace chrono; // nanoseconds, system_clock, seconds


string current_screen_string = "home";
string winName_string = "P3 Project";


int erosion_elem_i = 1;
int erosion_size_i = 5;
int dilation_elem_i = 0;
int dilation_size_i = 1;
int const max_elem_i = 2;
int const max_kernel_size_i = 21;
Mat src_mat, erosion_dst_mat, dilation_dst_mat, fist_dst_mat;
Mat bn_mat;
Mat blured_mat;
Mat tresh_mat, tresh_bin_mat;
Mat spawningprompt_mat = imread("textspawn.png");
int threshold_value_i = 17;
int threshold_type_i = 0;
int const max_value_i = 255;
int const max_type_i = 4;
int const max_BINARY_value_i = 255;
int timer_i;

int width_i = 800;
int height_i = 600;
int stepSize_i = 78;
int projectorStepSize_i = 45;
int difficulty_i = 2;

int maxStepSize_i = 150;
int basex_i = 96;
int const maxbase_i = 100 * 5;
int maxbasex_i = basex_i + 5 * projectorStepSize_i;
int basey_i = 58;
int maxbasey_i = basey_i + 5 * projectorStepSize_i;
int gridSize_i = 5;
int const gridSizeMax_i = 15;
Mat dispImage_mat;
Mat grid_mat;
int t = 0;
clock_t beginClock_clock;
clock_t fistClock_clock;
bool clickSpawn_b = false;
bool checktime_b = false;
bool checktimefist_b = false;
bool fistclicked_b = false;
bool win_b = false;
bool lose_b = false;
bool showshapeimgs_b = false;
bool spawnfeedback_b = false;
bool startGame_b = false;
double fist_secs_d;

Mat timer0_mat = imread("Fistbar.png");
Mat timer1_mat = imread("fistbar2.png");
Mat timer2_mat = imread("fistbar3.png");
Mat timer3_mat = imread("fistbar4.png");
Mat timer4_mat = imread("fistbar5.jpg");


//Storage for shapes in projector
vector<Shape*>* shapeinput = new vector<Shape*>(25);
vector<Shape*>* shapeCamera = new vector<Shape*>(25);
// Storage for blobs
vector<KeyPoint> squarekeypoints;
vector<KeyPoint> trianglekeypoints;
vector<KeyPoint> circlekeypoints;
vector<KeyPoint> fistkeypoints;

int comparing_i[5][5] = {};
// punk bitch
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

Mat takeBackground(Mat input, Mat currentBackground, Mat segmentedImage)
{
	bool blank_b = true;
	int whitecount_i = 0;
	for (int x = 0; x < segmentedImage.rows; x++)
	{
		for (int y = 0; y < segmentedImage.cols; y++)
		{
			if (segmentedImage.at<uchar>(x, y) != 0)
			{
				blank_b = false;
				whitecount_i++;
			}
		}
	}
	if (blank_b == false && whitecount_i > 10)
	{
		cout << "no bg take" << endl;
		return currentBackground;
	}
	cout << "New bg taken" << endl;
	return input;
}

void segmentation()
{
	bool takeBackground_bool = true;
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
	clock_t takeBackground_timerStart;

	while (true)
	{
		if (takeBackground_bool)
		{
			takeBackground_timerStart = clock();
			takeBackground_bool = false;
		}

		stream1.read(cameraInput);
		flip(cameraInput, cameraInput, 1);
		if ((char)waitKey(1) == 32)
		{
			background = cameraInput.clone();
			cout << "Space clicked" << endl;
		}

		segmentedCameraInput = background.clone() - cameraInput.clone();


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
			if (t = 0)
			{
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
		if (fistclicked == false)
		{
			cout << "fist is not on table" << endl;
			fistClock = clock();
		}
		fistend = clock();
		fist_secs = double(fistend - fistClock) / CLOCKS_PER_SEC;

		if (fist_secs > 4)
		{
			//fistClock = clock();
			fistclicked = false;
			if (startGame == false)
			{
				startGame = true;
			}
			else if (win == false && lose == false && startGame == true)
			{
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

		if (takeBackground_bool == false)
		{
			clock_t takeBackground_timerEnd = clock();
			double elapsed_secs = double(takeBackground_timerEnd - takeBackground_timerStart) / CLOCKS_PER_SEC;
			if (elapsed_secs > 10)
			{
				background = takeBackground(cameraInput.clone(), background.clone(), dilation_dst_mat.clone());
				takeBackground_bool = true;
			}
		}
	}
}

void spawnFeedback()
{
	clock_t spawning_clock = clock();
	while (spawnfeedback_b == true)
	{
		spawningprompt_mat.copyTo(grid_mat(Rect(grid_mat.cols / 4, grid_mat.rows / 4, spawningprompt_mat.cols, spawningprompt_mat.rows)));
		clock_t spawn_end_clock = clock();
		double elapsed_secs_d = double(spawn_end_clock - spawning_clock) / CLOCKS_PER_SEC;
		imshow("GRID", grid_mat);
		if (elapsed_secs_d > 3)
		{
			spawnfeedback_b = false;
		}
	}
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		if (!clickSpawn_b && !checktime_b)
		{
			clickSpawn_b = true;
			cout << "Spawning" << endl;
		}
	}
}


void GridCheckingsAndSuch()
{
	namedWindow("Grid1");
	int buffersize_i = 12;


	createTrackbar("x",
	               "Grid1", &basex_i,
	               maxbase_i, Grid_Demo);

	createTrackbar("y",
	               "Grid1", &basey_i,
	               maxbase_i, Grid_Demo);
	createTrackbar("Step size",
	               "Grid1", &projectorStepSize_i,
	               maxStepSize_i, Grid_Demo);
	grid_mat = dispImage_mat.clone();
	createTrackbar("Grid size",
	               "Grid1", &gridSize_i,
	               gridSizeMax_i, Grid_Demo);
	grid_mat = dispImage_mat.clone();


	for (int x = 0; x < grid_mat.rows; x++)
	{
		for (int y = 0; y < grid_mat.cols; y++)
		{
			grid_mat.at<uchar>(x, y) = 255;
		}
	}

	Grid_Demo(0, 0);

	Mat triangle_mat = imread("triangle.png");
	Mat square_mat = imread("square.png");
	Mat circle_mat = imread("circle.png");
}


void CameraGrid()
{
	Mat gridimage_mat = dilation_dst_mat.clone()(Rect(dilation_dst_mat.rows - 280, 120, 280, 280));

	int width_i = gridimage_mat.rows;
	stepSize_i = width_i / gridSize_i;


	for (int i = 0; i <= 5; i++)
	{
		line(gridimage_mat, Point(0, i * stepSize_i), Point(5 * stepSize_i, i * stepSize_i), Scalar(255, 255, 255));
	}
	for (int i = 0; i <= 5; i++)
	{
		line(gridimage_mat, Point(i * stepSize_i, 0), Point(i * stepSize_i, 5 * stepSize_i), Scalar(255, 255, 255));
	}

	imshow("cameragrid", gridimage_mat);
}


void checkPlacement()
{
	Mat gridimage_mat = dilation_dst_mat(Rect(dilation_dst_mat.rows - 280, 120, 280, 280));

	int width_i = gridimage_mat.rows;
	int height_i = gridimage_mat.cols;

	for (int i = 0; i < gridSize_i; i += 1)
	{
		for (int j = 0; j < gridSize_i; j += 1)
		{
			int buffersize_i = 12;
			int x_max_i = (((width_i / gridSize_i) * (i + 1)) - buffersize_i);
			int x_min_i = (((width_i / gridSize_i) * i) + buffersize_i);
			int y_max_i = (((height_i / gridSize_i) * (j + 1)) - buffersize_i);
			int y_min_i = (((height_i / gridSize_i) * j) + buffersize_i);
			for (int k = 0; k < squarekeypoints.size(); k += 1)
			{
				if (squarekeypoints.operator[](k).pt.x - (dilation_dst_mat.rows - 280) < x_max_i && squarekeypoints.operator[](k).pt.x
					- (dilation_dst_mat.rows - 280) > x_min_i && squarekeypoints.operator[](k).pt.y - 120 < y_max_i && squarekeypoints.
					operator[](k).pt.y - 120 > y_min_i)
				{
					shapeCamera->operator[](i + j * gridSize_i) = new Shape(squarekeypoints.operator[](k).pt.x,
					                                                      squarekeypoints.operator[](k).pt.y, 0);

					cout << "square in grid " << i << ", " << j << " my man" << endl;
				}
			}
			for (int k = 0; k < trianglekeypoints.size(); k += 1)
			{
				if (trianglekeypoints.operator[](k).pt.x - (dilation_dst_mat.rows - 280) < x_max_i && trianglekeypoints.operator[](k).
				                                                                                                      pt.x - (
						dilation_dst_mat.rows - 280) > x_min_i && trianglekeypoints.operator[](k).pt.y - 120 < y_max_i &&
					trianglekeypoints
					.operator[](k).pt.y - 120 > y_min_i)
				{
					shapeCamera->operator[](i + j * gridSize_i) = new Shape(trianglekeypoints.operator[](k).pt.x,
					                                                      trianglekeypoints.operator[](k).pt.y, 1);


					cout << "triangle in grid " << i << ", " << j << " my man" << endl;
				}
			}
			for (int k = 0; k < circlekeypoints.size(); k += 1)
			{
				if (circlekeypoints.operator[](k).pt.x - (dilation_dst_mat.rows - 280) < x_max_i && circlekeypoints.operator[](k).pt.x
					- (dilation_dst_mat.rows - 280) > x_min_i && circlekeypoints.operator[](k).pt.y - 120 < y_max_i && circlekeypoints.
					operator[](k).pt.y - 120 > y_min_i)
				{
					shapeCamera->operator[](i + j * gridSize_i) = new Shape(circlekeypoints.operator[](k).pt.x,
						circlekeypoints.operator[](k).pt.y, 2);

					cout << "circle in grid " << i << ", " << j << " my man" << endl;
				}
			}
		}
	}
	for (int i = 0; i < gridSize_i; i += 1)
	{
		for (int j = 0; j < gridSize_i; j += 1)
		{
			if (!shapeCamera->operator[](i + j * gridSize_i) == NULL)
			{
				cout << "Test " << shapeCamera->operator[](i + j * gridSize_i)->getType() << endl;
			}
		}
	}

	cout << "works" << endl;
	int counter_i = 0;
	for (int i = 0; i < gridSize_i; i += 1)
	{
		for (int j = 0; j < gridSize_i; j += 1)
		{
			if (shapeinput->operator[](i + j * gridSize_i) == NULL && shapeCamera->operator[](i + j * gridSize_i) == NULL)
			{
				cout << "both are null" << endl;
			}
			else if (shapeinput->operator[](i + j * gridSize_i) == NULL && !shapeCamera->operator[](i + j * gridSize_i) == NULL || !
				shapeinput->operator[](i + j * gridSize_i) == NULL && shapeCamera->operator[](i + j * gridSize_i) == NULL)
			{
				cout << "one is null" << endl;
				counter_i++;
			}
			else
			{
				if (shapeinput->operator[](i + j * gridSize_i)->getType() == shapeCamera->operator[](i + j * gridSize_i)->getType())
				{
					cout << i << " , " << j << " correct" << endl;
				}
				else
				{
					cout << i << " , " << j << " git gud scrub" << endl;
					counter_i++;
				}
			}
		}
	}
	cout << "change difficulty 1: " << difficulty_i << endl;
	cout << "count 1: " << counter_i << endl;
	if (counter_i == 0)
	{
		win_b = true;
		//difficulty++;
	}
	else
	{
		lose_b = true;
		if (difficulty_i != 1)
		{
			//difficulty--;
		}
	}
	cout << "change difficulty 2: " << difficulty_i << endl;
	cout << "count 2: " << counter_i << endl;
}

//vector<Shape*>* 
void calcShapes()
{
	vector<Point*> previouslocations;
	shapeinput->clear();
	shapeinput = new vector<Shape*>(25);
	int shapex_i;
	int shapey_i;
	int shapetype_i;
	bool changed_b = false;
	bool different_b = false;
	for (int i = 0; i < difficulty_i; i++)
	{
		while (different_b == false)
		{
			srand(time(NULL));

			shapex_i = rand() % gridSize_i;
			shapey_i = rand() % gridSize_i;
			shapetype_i = rand() % 3;
			if (previouslocations.size() == 0)
			{
				break;
			}
			for (int j = 0; j < previouslocations.size(); j++)
			{
				if (shapex_i == previouslocations.operator[](j)->x && shapey_i == previouslocations.operator[](j)->y)
				{
					changed_b = true;
				}
				else
				{
					different_b = true;
				}
			}
			if (changed_b)
			{
				different_b = false;
				changed_b = false;
			}
		}
		previouslocations.push_back(new Point(shapex_i, shapey_i));
		different_b = false;
		shapeinput->operator[](shapex_i + shapey_i * gridSize_i) = new Shape(shapex_i, shapey_i, shapetype_i);
	}
	previouslocations.clear();
}


void spawnShape(bool spawn)
{
	//Based on difficulty (spawn number of shapes)
	if (spawn == true)
	{
		calcShapes();
		beginClock_clock = clock();
		checktime_b = true;
		showshapeimgs_b = true;
		spawnfeedback_b = false;
	}
	clickSpawn_b = false;
}

Mat fist_time_bar()
{
	
	if (fist_secs_d > 0 && fist_secs_d < 1.0)
	{
		return timer0_mat;
	}
	else if (1.0 < fist_secs_d && fist_secs_d < 2.0)
	{
		return timer1_mat;
	}
	else if (2.0 < fist_secs_d && fist_secs_d < 3.0)
	{
		return timer2_mat;
	}
	else if (3.0 < fist_secs_d && fist_secs_d < 4.0)
	{
		return timer3_mat;
	}
	else if (4.0 < fist_secs_d)
	{
		return timer4_mat;
		//output_mat.copyTo(grid_mat(Rect(709, 189, timer4_mat.cols, timer4_mat.rows)));
	}
	//return output_mat;
}

int main()
{
	namedWindow(winName_string, CV_WINDOW_AUTOSIZE);

	Statscreen* test = new Statscreen();
	Settings* settingsscreen = new Settings();
	Homescreen* homescreen = new Homescreen();
	Mat winscreen_mat = imread("correct.png");
	Mat losescreen_mat = imread("wrongicon.png");
	Mat intro_mat = imread("introscreen.jpg");

	thread imageprocessing_thread(segmentation);
	cvNamedWindow("GRID", CV_WINDOW_NORMAL);
	moveWindow("GRID", 1981, 0);
	cvSetWindowProperty("GRID", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	setMouseCallback("GRID", CallBackFunc, NULL);
	dispImage_mat = homescreen->run();
	thread* spawn_shapes_thread = new thread();
	startGame_b = false;
	while (true)
	{

		if (startGame_b)
		{

			waitKey(1);

			//Create grid-system on projector
			GridCheckingsAndSuch();
			//Show shapes in grid
			if (clickSpawn_b == true)
			{
				spawnfeedback_b = true;
				spawn_shapes_thread = new thread{ spawnShape, clickSpawn_b };
			}


			if (spawnfeedback_b)
			{
				spawningprompt_mat.copyTo(grid_mat(Rect(grid_mat.cols / 4, grid_mat.rows / 2, spawningprompt_mat.cols, spawningprompt_mat.rows)));
			}
			else
			{
				if (!spawn_shapes_thread == NULL)
				{
					if (spawn_shapes_thread->joinable())
					{
						spawn_shapes_thread->join();
					}
				}
			}
			if (showshapeimgs_b)
			{
				for (int i = 0; i < shapeinput->size(); i++)
				{
					if (!shapeinput->operator[](i) == NULL)
					{
						shapeinput->operator[](i)->getImage().copyTo(grid_mat(Rect(
							(shapeinput->operator[](i)->getGridx() * (projectorStepSize_i * 2)) + basex_i * 2 + 18,
							(shapeinput->operator[](i)->getGridy() * (projectorStepSize_i * 2)) + basey_i * 2 + 18,
							shapeinput->operator[](i)->getImage().cols, shapeinput->operator[](i)->getImage().rows)));
					}
				}
			}

			if (checktime_b)
			{
				clock_t end_clock = clock();
				double elapsed_secs_d = double(end_clock - beginClock_clock) / CLOCKS_PER_SEC;

				if (elapsed_secs_d > 5)
				{
					beginClock_clock = clock();
					showshapeimgs_b = false;
					checktime_b = false;
				}
			}



			if (win_b == true)
			{
				winscreen_mat.copyTo(grid_mat(Rect(grid_mat.cols / 4, grid_mat.rows / 4, winscreen_mat.cols, winscreen_mat.rows)));
			}

			if (lose_b == true)
			{
				losescreen_mat.copyTo(grid_mat(Rect(grid_mat.cols / 4, grid_mat.rows / 4, losescreen_mat.cols, losescreen_mat.rows)));
			}

			Mat fist_time_bar = ::fist_time_bar();
			fist_time_bar.copyTo(grid_mat(Rect(709, 189, timer0_mat.cols, timer0_mat.rows)));
			//thishelps?

			imshow("Grid1", dispImage_mat);
			imshow("GRID", grid_mat);
		}
		else
			//
		{
			waitKey(1);
			grid_mat = intro_mat.clone();

			Mat fist_time_bar = ::fist_time_bar();
			fist_time_bar.copyTo(grid_mat(Rect(709, 189, timer0_mat.cols, timer0_mat.rows)));

			imshow("GRID", grid_mat.clone());
		}
	}
}

void Threshold_Demo(int, void*)
{
	/* 0: Binary
	1: Binary Inverted
	2: Threshold Truncated
	3: Threshold to Zero
	4: Threshold to Zero Inverted
	*/

	threshold(blured_mat, bn_mat, threshold_value_i, max_BINARY_value_i, threshold_type_i);

	imshow("thresholding", bn_mat);
}

void Grid_Demo(int, void*)
{
	for (int i = 0; i <= gridSize_i; i++)
	{
		line(grid_mat, Point(2 * basex_i, 2 * basey_i + 2 * (i * projectorStepSize_i)),
		     Point(2 * basex_i + 2 * (gridSize_i * projectorStepSize_i), 2 * basey_i + 2 * (i * projectorStepSize_i)), Scalar(0, 0, 0));
	}
	for (int i = 0; i <= gridSize_i; i++)
	{
		line(grid_mat, Point(2 * basex_i + 2 * (i * projectorStepSize_i), 2 * basey_i),
		     Point(2 * basex_i + 2 * (i * projectorStepSize_i), 2 * basey_i + 2 * (gridSize_i * projectorStepSize_i)), Scalar(0, 0, 0));
	}
}

/**  @function Erosion  */
void Erosion(int, void*)
{
	int erosion_type;
	if (erosion_elem_i == 0) { erosion_type = MORPH_RECT; }
	else if (erosion_elem_i == 1) { erosion_type = MORPH_CROSS; }
	else if (erosion_elem_i == 2) { erosion_type = MORPH_ELLIPSE; }

	Mat element = getStructuringElement(erosion_type,
	                                    Size(2 * erosion_size_i + 1, 2 * erosion_size_i + 1),
	                                    Point(erosion_size_i, erosion_size_i));

	/// Apply the erosion operation
	erode(bn_mat, erosion_dst_mat, element);
}

/** @function Dilation */
void Dilation(int, void*)
{
	int dilation_type;
	if (dilation_elem_i == 0) { dilation_type = MORPH_RECT; }
	else if (dilation_elem_i == 1) { dilation_type = MORPH_CROSS; }
	else if (dilation_elem_i == 2) { dilation_type = MORPH_ELLIPSE; }

	Mat element = getStructuringElement(dilation_type,
	                                    Size(2 * dilation_size_i + 1, 2 * dilation_size_i + 1),
	                                    Point(dilation_size_i, dilation_size_i));
	/// Apply the dilation operation
	dilate(erosion_dst_mat, dilation_dst_mat, element);
}