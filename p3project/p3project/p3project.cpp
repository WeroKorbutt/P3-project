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

int projectorStepSize_i = 45;
int difficulty_i = 2;

int basex_i = 96;
int basey_i = 58;
int gridSize_i = 5;

clock_t beginClock_clock;
clock_t fistClock_clock;
bool clickSpawn_b = false;
bool checktime_b = false;
bool checktimefist_b = false;
bool fistclicked_b = false;
bool win_b = false;
bool lose_b = false;
bool startGame_b = false;
bool showshapeimgs_b = false;
bool spawnfeedback_b = false;
double fist_secs_d;

int wincounter_i = 0;


Mat timer0_mat = imread("Fistbar.png");
Mat timer1_mat = imread("fistbar2.png");
Mat timer2_mat = imread("fistbar3.png");
Mat timer3_mat = imread("fistbar4.png");
Mat timer4_mat = imread("fistbar5.png");


//Storage for shapes in projector
vector<Shape*>* shapeinput = new vector<Shape*>(25);
vector<Shape*>* shapeCamera = new vector<Shape*>(25);

Mat GridCheckingsAndSuch(Mat dispImage_mat);
Mat CameraGrid(Mat grid_mat);
void checkPlacement(Mat grid_mat, Mat erosion_dst_mat, vector<KeyPoint> squarekeypoints,
                    vector<KeyPoint> trianglekeypoints, vector<KeyPoint> circlekeypoints);
char* trackbar_type = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
char* trackbar_value = "Value";

Mat takeBackground(Mat input, Mat currentBackground, Mat segmentedImage)
{
	bool blank_b = true;
	int whitecount_i = 0;
	for (int x = 0; x < segmentedImage.rows; x++)
		for (int y = 0; y < segmentedImage.cols; y++)
			if (segmentedImage.at<uchar>(x, y) != 0)
			{
				blank_b = false;
				whitecount_i++;
			}
	if (blank_b == false && whitecount_i > 10)
		return currentBackground;
	cout << "New bg image taken" << endl;
	return input;
}

SimpleBlobDetector::Params find_params(string type)
{
	//auto specifies that the type of the variable that is being declared 
	//will be automatically deduced from its initializer
	const auto squarecurrentmin_area = 215;
	const auto squarecurrentmax_area = 500;
	const auto trianglecurrentmin_area = 0;
	const auto trianglecurrentmax_area = 79;
	const auto circlecurrentmin_area = 79;
	const auto circlecurrentmax_area = 214;

	SimpleBlobDetector::Params output;


	//Set up default parameters
	// Filter by Convexity
	output.filterByConvexity = false;

	// Filter by Inertia
	output.filterByInertia = false;

	// Filter by colour
	output.filterByColor = true;
	output.blobColor = 255;
	output.filterByArea = true;


	if (type == "square")
	{
		output.minArea = squarecurrentmin_area;
		output.maxArea = squarecurrentmax_area;
		output.filterByCircularity = false;
	}
	else if (type == "triangle")
	{
		output.minArea = trianglecurrentmin_area;
		output.maxArea = trianglecurrentmax_area;
	}
	else if (type == "circle")
	{
		output.minArea = circlecurrentmin_area;
		output.maxArea = circlecurrentmax_area;
	}
	else if (type == "fist")
	{
		output.filterByArea = false;
		output.filterByCircularity = true;
		output.minCircularity = 0.05;
		output.maxCircularity = 0.78;
	}
	return output;
}

void segmentation()
{
	//Declaring variables
	bool take_background_b = true;
	bool state_b = false;

	Mat camera_input_mat;
	Mat background_mat = imread("background.jpg");
	Mat grid_output_mat;
	Mat greyscale_mat;
	clock_t take_background_timer_start_clock;


	int threshold_value_i = 17;
	int threshold_type_i = 0;
	int erosion_size_i = 7;

	Mat bn_mat, blured_mat;
	Mat erosion_dst_mat;

	VideoCapture stream(0); //change 0 to 1 to get from the plugged in camera instead of webcam.
	if (!stream.isOpened()) cout << "Cannot open camera";


	SimpleBlobDetector::Params square_params = find_params("square");
	SimpleBlobDetector::Params circle_params = find_params("circle");
	SimpleBlobDetector::Params triangle_params = find_params("triangle");
	SimpleBlobDetector::Params fist_params = find_params("fist");

	vector<KeyPoint> squarekeypoints;
	vector<KeyPoint> trianglekeypoints;
	vector<KeyPoint> circlekeypoints;
	vector<KeyPoint> fistkeypoints;

	while (true)
	{
		if (take_background_b)
		{
			take_background_timer_start_clock = clock();
			take_background_b = false;
		}

		stream.read(camera_input_mat);
		flip(camera_input_mat, camera_input_mat, 1);
		if ((char)waitKey(1) == 32)
		{
			background_mat = camera_input_mat.clone();
			cout << "Space clicked" << endl;
		}

		Mat segmented_camera_input_mat = background_mat.clone() - camera_input_mat.clone();

		for (int x = 0; x < segmented_camera_input_mat.rows; x++)
			for (int y = 0; y < segmented_camera_input_mat.cols; y++)
				if (segmented_camera_input_mat.at<Vec3b>(x, y) == Vec3b(255, 255, 255))
				{
					segmented_camera_input_mat.at<Vec3b>(x, y)[0] = 0;
					segmented_camera_input_mat.at<Vec3b>(x, y)[1] = 0;
					segmented_camera_input_mat.at<Vec3b>(x, y)[2] = 0;
				}

		cvtColor(segmented_camera_input_mat, greyscale_mat, CV_BGR2GRAY);

		medianBlur(greyscale_mat, blured_mat, 15); // Applyng median blur

		// Call the function to initialize
		threshold(blured_mat, bn_mat, threshold_value_i, 255, threshold_type_i);

		Mat erosion_element_mat = getStructuringElement(MORPH_CROSS,
		                                                Size(2 * erosion_size_i + 1, 2 * erosion_size_i + 1),
		                                                Point(erosion_size_i, erosion_size_i));
		erode(bn_mat, erosion_dst_mat, erosion_element_mat);

		Rect camera_region_of_interest = Rect(489, 329, 150, 150);
		Mat fist_dst_mat(camera_region_of_interest.size(), erosion_dst_mat.type());
		erosion_dst_mat(camera_region_of_interest).copyTo(fist_dst_mat);
		imshow("Fist window", fist_dst_mat);

		Ptr<SimpleBlobDetector> squaredetector;
		squaredetector = SimpleBlobDetector::create(square_params);
		Ptr<SimpleBlobDetector> circledetector;
		circledetector = SimpleBlobDetector::create(circle_params);
		Ptr<SimpleBlobDetector> triangledetector;
		triangledetector = SimpleBlobDetector::create(triangle_params);

		Ptr<SimpleBlobDetector> fistdetector = SimpleBlobDetector::create(fist_params);


		// Detect blobs
		squaredetector->detect(erosion_dst_mat, squarekeypoints);
		triangledetector->detect(erosion_dst_mat, trianglekeypoints);
		circledetector->detect(erosion_dst_mat, circlekeypoints);
		fistdetector->detect(fist_dst_mat, fistkeypoints);

		//the total no of blobs detected are:
		size_t fistamount_size = fistkeypoints.size();

		clock_t fist_end_clock;
		if (fistamount_size != 0)
		{
			fistclicked_b = true;
			if (!state_b)
			{
				fistClock_clock = clock();
				state_b = true;
			}
		}
		else
		{
			state_b = false;
			fistclicked_b = false;
		}
		if (fistclicked_b == false)
			fistClock_clock = clock();


		fist_end_clock = clock();
		fist_secs_d = double(fist_end_clock - fistClock_clock) / CLOCKS_PER_SEC;
		Mat gridimage_mat = erosion_dst_mat.clone()(Rect(erosion_dst_mat.rows - 280, 120, 280, 280));
		if (fist_secs_d > 4)
		{
			fistclicked_b = false;
			if (startGame_b == false)
			{
				startGame_b = true;
				clickSpawn_b = true;
			}
			else if (win_b == false && lose_b == false && startGame_b == true)
			{
				checkPlacement(gridimage_mat, erosion_dst_mat, squarekeypoints, trianglekeypoints, circlekeypoints);
			}
			else if (win_b == true || lose_b == true)
			{
				win_b = false;
				lose_b = false;
				shapeinput->clear();
				shapeCamera->clear();
				shapeinput = new vector<Shape*>(25);
				shapeCamera = new vector<Shape*>(25);
				clickSpawn_b = true;
			}
			fistClock_clock = clock();
			state_b = false;
		}

		Mat im_with_squarekeypoints;
		Mat im_with_trianglekeypoints;
		Mat im_with_circlekeypoints;
		Mat im_with_fistkeypoints;

		drawKeypoints(erosion_dst_mat, squarekeypoints, im_with_squarekeypoints, Scalar(0.0, 0.0, 255),
		              DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		drawKeypoints(erosion_dst_mat, trianglekeypoints, im_with_trianglekeypoints, Scalar(0.0, 0.0, 255),
		              DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		drawKeypoints(erosion_dst_mat, circlekeypoints, im_with_circlekeypoints, Scalar(0.0, 0.0, 255),
		              DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		drawKeypoints(fist_dst_mat, fistkeypoints, im_with_fistkeypoints, Scalar(0.0, 0.0, 255),
		              DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

		//Make camera grid
		Mat cameragrid_mat = CameraGrid(gridimage_mat);
		imshow("cameragrid", cameragrid_mat);
		imshow("edited", segmented_camera_input_mat);
		imshow("Squares", im_with_squarekeypoints);
		imshow("Triangle", im_with_trianglekeypoints);
		imshow("Circle", im_with_circlekeypoints);
		imshow("original", camera_input_mat);

		imshow("background", background_mat);
		imshow("Fisting", im_with_fistkeypoints);

		if (take_background_b == false)
		{
			clock_t take_background_timer_end_clock = clock();
			double elapsed_secs = double(take_background_timer_end_clock - take_background_timer_start_clock) / CLOCKS_PER_SEC;
			if (elapsed_secs > 5)
			{
				background_mat = takeBackground(camera_input_mat.clone(), background_mat.clone(), erosion_dst_mat.clone());
				take_background_b = true;
			}
		}
	}
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	if (event == EVENT_LBUTTONDOWN)
		if (!clickSpawn_b && !checktime_b)
		{
			clickSpawn_b = true;
			cout << "Spawning" << endl;
		}
}


Mat GridCheckingsAndSuch(Mat dispImage_mat)
{
	int buffersize_i = 12;

	Mat grid_mat = dispImage_mat.clone();


	for (int x = 0; x < dispImage_mat.rows; x++)
		for (int y = 0; y < dispImage_mat.cols; y++)
			grid_mat.at<uchar>(x, y) = 255;

	//Grid
	for (int i = 0; i <= gridSize_i; i++)
		line(grid_mat, Point(2 * basex_i, 2 * basey_i + 2 * (i * projectorStepSize_i)),
		     Point(2 * basex_i + 2 * (gridSize_i * projectorStepSize_i), 2 * basey_i + 2 * (i * projectorStepSize_i)),
		     Scalar(0, 0, 0));

	for (int i = 0; i <= gridSize_i; i++)
		line(grid_mat, Point(2 * basex_i + 2 * (i * projectorStepSize_i), 2 * basey_i),
		     Point(2 * basex_i + 2 * (i * projectorStepSize_i), 2 * basey_i + 2 * (gridSize_i * projectorStepSize_i)),
		     Scalar(0, 0, 0));

	return grid_mat;
}


Mat CameraGrid(Mat gridinput_mat)
{
	int width_i = gridinput_mat.rows;
	int stepSize_i = width_i / gridSize_i;


	for (int i = 0; i <= 5; i++)
		line(gridinput_mat, Point(0, i * stepSize_i), Point(5 * stepSize_i, i * stepSize_i), Scalar(255, 255, 255));

	for (int i = 0; i <= 5; i++)
		line(gridinput_mat, Point(i * stepSize_i, 0), Point(i * stepSize_i, 5 * stepSize_i), Scalar(255, 255, 255));

	return gridinput_mat;
}


void checkPlacement(Mat gridinput_mat, Mat erosion_dst_mat, vector<KeyPoint> squarekeypoints,
                    vector<KeyPoint> trianglekeypoints, vector<KeyPoint> circlekeypoints)
{
	int width_i = gridinput_mat.rows;
	int height_i = gridinput_mat.cols;


	for (int i = 0; i < gridSize_i; i += 1)
	{
		for (int j = 0; j < gridSize_i; j += 1)
		{
			int buffersize_i = 2;
			int x_max_i = (((width_i / gridSize_i) * (i + 1)) - buffersize_i);
			int x_min_i = (((width_i / gridSize_i) * i) + buffersize_i);
			int y_max_i = (((height_i / gridSize_i) * (j + 1)) - buffersize_i);
			int y_min_i = (((height_i / gridSize_i) * j) + buffersize_i);
			for (int k = 0; k < squarekeypoints.size(); k += 1)
			{
				if (squarekeypoints.operator[](k).pt.x - (erosion_dst_mat.rows - 280) < x_max_i && squarekeypoints.operator[](k).pt.
				                                                                                                   x
					- (erosion_dst_mat.rows - 280) > x_min_i && squarekeypoints.operator[](k).pt.y - 120 < y_max_i && squarekeypoints.
					operator[](k).pt.y - 120 > y_min_i)
				{
					shapeCamera->operator[](i + j * gridSize_i) = new Shape(squarekeypoints.operator[](k).pt.x,
					                                                        squarekeypoints.operator[](k).pt.y, 0);

					cout << "square in grid " << i << ", " << j << " my man" << endl;
				}
			}
			for (int k = 0; k < trianglekeypoints.size(); k += 1)
			{
				if (trianglekeypoints.operator[](k).pt.x - (erosion_dst_mat.rows - 280) < x_max_i && trianglekeypoints.
					operator[](k).
					pt.x - (
						erosion_dst_mat.rows - 280) > x_min_i && trianglekeypoints.operator[](k).pt.y - 120 < y_max_i &&
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
				if (circlekeypoints.operator[](k).pt.x - (erosion_dst_mat.rows - 280) < x_max_i && circlekeypoints.operator[](k).pt.
				                                                                                                   x
					- (erosion_dst_mat.rows - 280) > x_min_i && circlekeypoints.operator[](k).pt.y - 120 < y_max_i && circlekeypoints.
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
			else if (shapeinput->operator[](i + j * gridSize_i) == NULL && !shapeCamera->operator[](i + j * gridSize_i) == NULL
				|| !
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
		wincounter_i++;
		if (difficulty_i < 6 && wincounter_i >= 2)
		{
			difficulty_i++;
			wincounter_i = 0;
		}
	}
	else
	{
		lose_b = true;
		if (difficulty_i != 1)
		{
			wincounter_i = 0;
			difficulty_i--;
		}
	}
	cout << "change difficulty 2: " << difficulty_i << endl;
	cout << "count 2: " << counter_i << endl;
}

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
	vector<int>* previousShapes;
	previousShapes = new vector<int>(3);
	for (int i = 0; i < previousShapes->capacity(); i++)
	{
		previousShapes->operator[](i) = 0;
	}

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
					if (previousShapes->operator[](shapetype_i) != NULL)
					{
						if (previousShapes->operator[](shapetype_i) <= 1)
						{
							previousShapes->operator[](shapetype_i)++;
							different_b = true;
						}
						else
						{
							changed_b = true;
						}
					}
					else
					{
						changed_b = true;
					}
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
	clickSpawn_b = false;
	if (spawn == true)
	{
		calcShapes();
		beginClock_clock = clock();
		checktime_b = true;
		showshapeimgs_b = true;
		spawnfeedback_b = false;
		cout << "this is ran" << endl;
	}
}

Mat call_fist_time_bar()
{
	if (fist_secs_d >= 0 && fist_secs_d <= 1.0)
	{
		return timer0_mat;
	}
	else if (1.0 < fist_secs_d && fist_secs_d <= 2.0)
	{
		return timer1_mat;
	}
	else if (2.0 < fist_secs_d && fist_secs_d <= 3.0)
	{
		return timer2_mat;
	}
	else if (3.0 < fist_secs_d && fist_secs_d <= 4.0)
	{
		return timer3_mat;
	}
	else if (4.0 < fist_secs_d)
	{
		return timer4_mat;
	}
}

int main()
{
	Statscreen* test = new Statscreen();
	Settings* settingsscreen = new Settings();
	Homescreen* homescreen = new Homescreen();
	Mat winscreen_mat = imread("correct.png");
	Mat losescreen_mat = imread("wrongicon.png");
	Mat intro_mat = imread("introscreen.jpg");
	Mat spawningprompt_mat = imread("textspawn.png");

	thread imageprocessing_thread(segmentation);
	cvNamedWindow("GRID", CV_WINDOW_NORMAL);
	moveWindow("GRID", 1981, 0);
	cvSetWindowProperty("GRID", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	setMouseCallback("GRID", CallBackFunc, NULL);
	Mat dispImage_mat = homescreen->run();
	thread* spawn_shapes_thread = new thread();
	Mat grid_mat;

	while (true)
	{
		if (startGame_b)
		{
			waitKey(1);
			//Create grid-system on projector
			grid_mat = GridCheckingsAndSuch(dispImage_mat);
			//Show shapes in grid
			if (clickSpawn_b == true)
			{
				spawnfeedback_b = true;
				spawn_shapes_thread = new thread{spawnShape, clickSpawn_b};
			}
			if (spawnfeedback_b)
			{
				spawningprompt_mat.copyTo(grid_mat(Rect(grid_mat.cols / 4, grid_mat.rows / 2, spawningprompt_mat.cols,
				                                        spawningprompt_mat.rows)));
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
						cout << "shows shape " << endl;
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
				losescreen_mat.copyTo(
					grid_mat(Rect(grid_mat.cols / 4, grid_mat.rows / 4, losescreen_mat.cols, losescreen_mat.rows)));
			}

			Mat fist_time_bar = call_fist_time_bar();
			fist_time_bar.copyTo(grid_mat(Rect(709, 189, timer0_mat.cols, timer0_mat.rows)));
			imshow("GRID", grid_mat);
		}
		else
		{
			waitKey(1);
			grid_mat = intro_mat.clone();

			Mat fist_time_bar = call_fist_time_bar();
			fist_time_bar.copyTo(grid_mat(Rect(709, 189, timer0_mat.cols, timer0_mat.rows)));

			imshow("GRID", grid_mat.clone());
		}
	}
}
