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


using namespace std; //using std to save the std:: before each command
using namespace cv; // using namespace cv to save cd:: each time u call a cv-commands
using namespace this_thread; // sleep_for, sleep_until
using namespace chrono; // nanoseconds, system_clock, seconds

int projectorStepSize_i = 45;  // adjusting the projector grid to fit with the camera grid
int difficulty_i = 3; // setting dificulty of the game 

int basex_i = 96;  // x and y  of the point that the grid starts to draw
int basey_i = 58;
int gridSize_i = 5;//  grid size separation 5x5=25 squares 

clock_t beginClock_clock;
clock_t fistClock_clock;
bool clickSpawn_b = false;  // declearing all the booleans 
bool checktime_b = false;
bool checktimefist_b = false;
bool fistclicked_b = false;
bool win_b = false;
bool lose_b = false;
bool startGame_b = false;
bool showshapeimgs_b = false;
bool spawnfeedback_b = false;
double fist_secs_d;

int wincounter_i = 0; // declearing an int wich stores how many times youve put the shapes correct so you can go to the next level
int losecounter_i = 0;

// fist bar mats ..
Mat timer0_mat = imread("Fistbar.png");
Mat timer1_mat = imread("fistbar2.png");
Mat timer2_mat = imread("fistbar3.png");
Mat timer3_mat = imread("fistbar4.png");

//Storage for shapes in projector and camera each with 25spots 
vector<Shape*>* shapeinput = new vector<Shape*>(25);
vector<Shape*>* shapeCamera = new vector<Shape*>(25);

//introducing these three functions so the main function knows about them
Mat GridCheckingsAndSuch(Mat dispImage_mat);
Mat CameraGrid(Mat grid_mat);

void checkPlacement(Mat grid_mat, Mat erosion_dst_mat, vector<KeyPoint> squarekeypoints,
                    vector<KeyPoint> trianglekeypoints, vector<KeyPoint> circlekeypoints);

char* trackbar_type = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted"; //  different tresholds 
char* trackbar_value = "Value";//  containts the value of the trackbars used in segmentation 

Mat takeBackground(Mat input, Mat currentBackground, Mat segmentedImage) // takes a background of the table  
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

SimpleBlobDetector::Params find_params(string type) //  creating a simple blob detector + shape assignment 
{
	//auto specifies that the type of the variable that is being declared 
	//will be automatically deduced from its initializer
	const auto squarecurrentmin_area = 238;
	const auto squarecurrentmax_area = 500;
	const auto trianglecurrentmin_area = 0;
	const auto trianglecurrentmax_area = 79;
	const auto circlecurrentmin_area = 79;
	const auto circlecurrentmax_area = 237;

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

	//  filtering the blobs by min and max area of the blob to decide what type of shape the blob is. 
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

	VideoCapture stream(1); //change 0 to 1 to get from the plugged in camera instead of webcam.
	if (!stream.isOpened()) cout << "Cannot open camera"; // error check 

// the different objects detected
	SimpleBlobDetector::Params square_params = find_params("square");
	SimpleBlobDetector::Params circle_params = find_params("circle");
	SimpleBlobDetector::Params triangle_params = find_params("triangle");
	SimpleBlobDetector::Params fist_params = find_params("fist");

	// an array for each object containing x,y direction 
	vector<KeyPoint> squarekeypoints;
	vector<KeyPoint> trianglekeypoints;
	vector<KeyPoint> circlekeypoints;
	vector<KeyPoint> fistkeypoints;

	while (true)
	{
		if (take_background_b)
		{
			take_background_timer_start_clock = clock(); // a timer for automatic background taking
			take_background_b = false;
		}

		stream.read(camera_input_mat); //  reads the stream output assigning it to a mat
		flip(camera_input_mat, camera_input_mat, 1); // tourning around the picture of the camera to fit the gameplay 
		if ((char)waitKey(1) == 32) // on space clicked creates a new background  
		{
			background_mat = camera_input_mat.clone(); 
			cout << "Space clicked" << endl;
		}

		Mat segmented_camera_input_mat = background_mat.clone() - camera_input_mat.clone(); // the original input form the background 

		for (int x = 0; x < segmented_camera_input_mat.rows; x++) // two nested for loops going thru each pixel of the image 
			for (int y = 0; y < segmented_camera_input_mat.cols; y++)
				if (segmented_camera_input_mat.at<Vec3b>(x, y) == Vec3b(255, 255, 255))
				{
					segmented_camera_input_mat.at<Vec3b>(x, y)[0] = 0;
					segmented_camera_input_mat.at<Vec3b>(x, y)[1] = 0;
					segmented_camera_input_mat.at<Vec3b>(x, y)[2] = 0;
				}

		cvtColor(segmented_camera_input_mat, greyscale_mat, CV_BGR2GRAY); //  converting the segment camera input mat to a gray scale mat

		medianBlur(greyscale_mat, blured_mat, 15); // Applyng median blur

		// Call the function to initialize
		threshold(blured_mat, bn_mat, threshold_value_i, 255, threshold_type_i); //  thresholding  


		//What erosion does is to compute a local minimum over the area of the kernel.
		Mat erosion_element_mat = getStructuringElement(MORPH_CROSS,
		                                                Size(2 * erosion_size_i + 1, 2 * erosion_size_i + 1), //  setting rectangular structuring element of size 
		                                                Point(erosion_size_i, erosion_size_i));
		erode(bn_mat, erosion_dst_mat, erosion_element_mat);

		Rect camera_region_of_interest = Rect(489, 329, 150, 150); // declearing the particular area of interest from the overall camera size 
		Mat fist_dst_mat(camera_region_of_interest.size(), erosion_dst_mat.type());
		erosion_dst_mat(camera_region_of_interest).copyTo(fist_dst_mat); // erosion on the fist element-mat
		imshow("Fist window", fist_dst_mat);


		// declearing detectors for each selement 
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

		clock_t fist_end_clock;// creating a counter for the fist ( how long is detected)
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

		Mat gridimage_mat = erosion_dst_mat.clone()(Rect(erosion_dst_mat.rows - 280, 120, 280, 280)); // erodes 

		if (fist_secs_d > 3) // 3 seconds timecheck 
		{
			fistclicked_b = false;
			if (startGame_b == false)
			{
				startGame_b = true;
				clickSpawn_b = true;
			}
			else if (win_b == false && lose_b == false && startGame_b == true) // if statement that calls the shapecheck function 
			{
				checkPlacement(gridimage_mat, erosion_dst_mat, squarekeypoints, trianglekeypoints, circlekeypoints); // calling check shapes fucnction to check if the placement of the shapes  is correct 
			}
			else if (win_b == true) // if the game is lost or won clears the arrays of shapes both camera and 
			{
				win_b = false; //  set back both booleans to false 
				lose_b = false;
				shapeinput->clear();
				shapeCamera->clear();
				shapeinput = new vector<Shape*>(25);
				shapeCamera = new vector<Shape*>(25);
				clickSpawn_b = true;// turns clickSpawn to true so it can spawn new shapes 
				if(wincounter_i >=2 )
					wincounter_i = 0;

			}
			else if (lose_b == true)
			{
				win_b = false; //  set back both booleans to false 
				lose_b = false;
				shapeinput->clear();
				shapeCamera->clear();
				shapeinput = new vector<Shape*>(25);
				shapeCamera = new vector<Shape*>(25);
				clickSpawn_b = true; // turns clickSpawn to true so it can spawn new shapes 
				if (losecounter_i >= 2) 
					losecounter_i = 0;
			}
			fistClock_clock = clock();
			state_b = false;
		}


		// mat images of the different types of objects 
		Mat im_with_squarekeypoints;
		Mat im_with_trianglekeypoints;
		Mat im_with_circlekeypoints;
		Mat im_with_fistkeypoints;

			// drawing circles around each blob whenever is defined to be an particular shape 

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

		// spawning  the different widows and assigning them mats
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
			double elapsed_secs = double(take_background_timer_end_clock - take_background_timer_start_clock) / CLOCKS_PER_SEC; // calculate the time in computer seconds between the first timer and the second one that shows how much time you have hold the fist
			if (elapsed_secs > 5)
			{
				background_mat = takeBackground(camera_input_mat.clone(), background_mat.clone(), erosion_dst_mat.clone()); // takes a new background 
				take_background_b = true;
			}
		}
	}
}


// Creating a method which will create the grid on the projected picture 
Mat GridCheckingsAndSuch(Mat dispImage_mat,int dificulty)
{
	int buffersize_i = 12; // distance between the lanes of the grid
	

	
	Mat grid_mat = dispImage_mat.clone(); // assinging a existing mat to a mat


	for (int x = 0; x < dispImage_mat.rows; x++)//  nested loops for detecting the pixels in the mat
		for (int y = 0; y < dispImage_mat.cols; y++)
			grid_mat.at<uchar>(x, y) = 255;

	//Grid build 
	for (int i = 0; i <= gridSize_i; i++)
		line(grid_mat, Point(2 * basex_i, 2 * basey_i + 2 * (i * projectorStepSize_i)), //horizontal lines drawn 
		     Point(2 * basex_i + 2 * (gridSize_i * projectorStepSize_i), 2 * basey_i + 2 * (i * projectorStepSize_i)),
		     Scalar(0, 0, 0));

	for (int i = 0; i <= gridSize_i; i++) //  vertical lines drawn 
		line(grid_mat, Point(2 * basex_i + 2 * (i * projectorStepSize_i), 2 * basey_i),
		     Point(2 * basex_i + 2 * (i * projectorStepSize_i), 2 * basey_i + 2 * (gridSize_i * projectorStepSize_i)),
		     Scalar(0, 0, 0));

	return grid_mat; // returnt the grided mat 
}


Mat CameraGrid(Mat gridinput_mat)
{ 
	// creating the grid on the camera  in the region of interest 
	int width_i = gridinput_mat.rows; // the width of the grid matches the rows(all pixels in horizontal direction )
	int stepSize_i = width_i / gridSize_i; // calculating how many squares the grid will have 


	for (int i = 0; i <= 5; i++) // drawing horizontal lines 
		line(gridinput_mat, Point(0, i * stepSize_i), Point(5 * stepSize_i, i * stepSize_i), Scalar(255, 255, 255));

	for (int i = 0; i <= 5; i++)// drawing vertical lines 
		line(gridinput_mat, Point(i * stepSize_i, 0), Point(i * stepSize_i, 5 * stepSize_i), Scalar(255, 255, 255));

	return gridinput_mat;
}

// creating a method that check the shapes of the camera imput with the spawned shapes 
void checkPlacement(Mat gridinput_mat, Mat erosion_dst_mat, vector<KeyPoint> squarekeypoints,
                    vector<KeyPoint> trianglekeypoints, vector<KeyPoint> circlekeypoints)
{
	int width_i = gridinput_mat.rows;
	int height_i = gridinput_mat.cols;

	// twon neste loops going thru the grid squares 
	for (int i = 0; i < gridSize_i; i += 1)
	{
		for (int j = 0; j < gridSize_i; j += 1)
		{
			int buffersize_i = 2;// distance betweene the positions 
			int x_max_i = (((width_i / gridSize_i) * (i + 1)) - buffersize_i); // declearing four ints for the limit of the bunding box ratio
			int x_min_i = (((width_i / gridSize_i) * i) + buffersize_i);
			int y_max_i = (((height_i / gridSize_i) * (j + 1)) - buffersize_i);
			int y_min_i = (((height_i / gridSize_i) * j) + buffersize_i);
			for (int k = 0; k < squarekeypoints.size(); k += 1) // for loop going thru the  squarekeypoints vector array - going thru all the blobs in the array
			{
				if (squarekeypoints.operator[](k).pt.x - (erosion_dst_mat.rows - 280) < x_max_i && squarekeypoints.operator[](k).pt.
				                                                                                                   x
					- (erosion_dst_mat.rows - 280) > x_min_i && squarekeypoints.operator[](k).pt.y - 120 < y_max_i && squarekeypoints.
					operator[](k).pt.y - 120 > y_min_i)  // checks if there is a square shape blob in the square 
				{
					shapeCamera->operator[](i + j * gridSize_i) = new Shape(squarekeypoints.operator[](k).pt.x,
					                                                        squarekeypoints.operator[](k).pt.y, 0);  // puting an object of class shape into square points array x and y +  type 

					cout << "square in grid " << i << ", " << j << " my man" << endl;
				}
			}
			for (int k = 0; k < trianglekeypoints.size(); k += 1) // for loop going thru the  trianglekeypoints vector array - going thru all the blobs in the array
			{
				if (trianglekeypoints.operator[](k).pt.x - (erosion_dst_mat.rows - 280) < x_max_i && trianglekeypoints.
					operator[](k).
					pt.x - (
						erosion_dst_mat.rows - 280) > x_min_i && trianglekeypoints.operator[](k).pt.y - 120 < y_max_i &&
					trianglekeypoints
					.operator[](k).pt.y - 120 > y_min_i) // checks if there is a triangle shape in the square 
				{
					shapeCamera->operator[](i + j * gridSize_i) = new Shape(trianglekeypoints.operator[](k).pt.x,
					                                                        trianglekeypoints.operator[](k).pt.y, 1);// puting an object of class shape into trianglekeypoints array x and y + type


					cout << "triangle in grid " << i << ", " << j << " my man" << endl;
				}
			}
			for (int k = 0; k < circlekeypoints.size(); k += 1) // for loop going thru the  circle keypoints vector array - going thru all the blobs in the array
			{
				if (circlekeypoints.operator[](k).pt.x - (erosion_dst_mat.rows - 280) < x_max_i && circlekeypoints.operator[](k).pt.
				                                                                                                   x
					- (erosion_dst_mat.rows - 280) > x_min_i && circlekeypoints.operator[](k).pt.y - 120 < y_max_i && circlekeypoints.
					operator[](k).pt.y - 120 > y_min_i) // checks if there is a circle shape blob in the square 
				{
					shapeCamera->operator[](i + j * gridSize_i) = new Shape(circlekeypoints.operator[](k).pt.x,
					                                                        circlekeypoints.operator[](k).pt.y, 2);// puting an object of class shape into circlekeypoints array x and y + type

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
				|| !shapeinput->operator[](i + j * gridSize_i) == NULL && shapeCamera->operator[](i + j * gridSize_i) == NULL)
				// compares the shape required with the shape put
			{
				cout << "one is null" << endl;
				counter_i++; // how many mistakes there are and adds a mistake
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
					counter_i++;// how many mistakes there are and adds a mistake
				}
			}
		}
	}
	if (counter_i == 0) //  if there is no mistake 
	{
		win_b = true;
		if (difficulty_i < 6)
		{
			wincounter_i++; // wins +1
			losecounter_i = 0; // losses set to 0
			if (wincounter_i >= 2) // if you put correctly the shapes 2 times you go to next difficulty
				difficulty_i++; //  difficulty increase
		}
		else wincounter_i = 0;
	}
	else
	{
		lose_b = true;
		if (difficulty_i != 1) {
			losecounter_i++;
			wincounter_i = 0; // set wincounter to 0
			if (losecounter_i >= 2)
				difficulty_i--; // difficulty down if the shape pattern is wrong 
		}
		else losecounter_i = 0;
	}
	cout << "New difficulty: " << difficulty_i << endl;
	cout << "Wrongs : " << counter_i << endl;
}

void calcShapes()
{
	// declearing variables and arrays 
	vector<Point*> previouslocations;
	shapeinput->clear(); // clear the shapeinput array
	shapeinput = new vector<Shape*>(25); // creates a new cector array of shapes 25-positions 
	int shapex_i;
	int shapey_i;
	int shapetype_i;
	bool changed_b = false;
	bool different_b = false;
	vector<int>* previousShapes;  // stores the x,y  and type of the shapes put on the grid

	previousShapes = new vector<int>(3); // default size of the array 
	for (int i = 0; i < previousShapes->capacity(); i++)
	{
		previousShapes->operator[](i) = 0;
	}

	for (int i = 0; i < difficulty_i; i++)
	{
		while (different_b == false)
		{
			srand(time(NULL)); // it depending of what time you run the program it randomize the position of the shapes

			shapex_i = rand() % gridSize_i;// random x
			shapey_i = rand() % gridSize_i;// random y
			shapetype_i = rand() % 3; // randoms the type of the shape 
			if (previouslocations.size() == 0) 
			{
				break;// brakes out of the loop if the game is runed for first time to skip the rest of the code bellow 
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

// crate a spawnShape method  to spawn the shapes on the screen
void spawnShape(bool spawn)
{
	clickSpawn_b = false;
	if (spawn == true)
	{
		calcShapes();// calls the calcluating x,y position function for the shapes 
		beginClock_clock = clock();
		checktime_b = true;
		showshapeimgs_b = true;
		spawnfeedback_b = false;
		cout << "this is ran" << endl;
	}
}
// a time bar mathod which determines how many second you had your fist on the table 
Mat call_fist_time_bar()
{// in dependence of the amount of time the fist was on the table different pictures of the background change to create a visualisation of the time 
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
	else if (3.0 < fist_secs_d)
	{
		return timer3_mat;
	}
}
// create a main method cuz we had to 
int main()
{// creating several screens out of constructors
	Statscreen* test = new Statscreen();
	Settings* settingsscreen = new Settings();
	Homescreen* homescreen = new Homescreen();
	// crating mats for different popups and screens 
	Mat winscreen_mat = imread("correct.png");
	Mat losescreen_mat = imread("wrongicon.png");
	Mat intro_mat = imread("introscreen.jpg");
	Mat spawningprompt_mat = imread("textspawn.png");
	Mat new_game_mat = imread("emptyicon.png");
	Mat one_win_mat = imread("onecorrect.png");
	Mat two_win_mat = imread("twocorrect.png");
	Mat one_wrong_mat = imread("onewrong.png");
	Mat two_wrong_mat = imread("twowrong.png");
	stringstream ss_stringstream;
	ss_stringstream << difficulty_i;
	string difficulty_text_string = "Level: " + ss_stringstream.str();

	thread imageprocessing_thread(segmentation); // running the whole segmentation method on a different thread so it works parallel to the main method

	cvNamedWindow("GRID", CV_WINDOW_NORMAL);

	moveWindow("GRID", 1981, 0);

	cvSetWindowProperty("GRID", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);


	
	Mat dispImage_mat = homescreen->run();// runs the main screen
	thread* spawn_shapes_thread = new thread();  // declearing a new thread 
	Mat grid_mat;

	while (true)
	{
		if (startGame_b)
		{
			waitKey(1);
			//Create grid-system on projector
			grid_mat = GridCheckingsAndSuch(dispImage_mat,difficulty_i);
			//Show shapes in grid
			if (clickSpawn_b == true)
			{
				spawnfeedback_b = true; // boolean that decides if the text "shape spawning" is on the screen 
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
					if (spawn_shapes_thread->joinable()) // it checks if the method is joinable 
					{
						spawn_shapes_thread->join();// wait for the thread to finish before it continues reading the code below 
					}
				}
			}
			if (showshapeimgs_b) // if showsshapeimgs_b == true 
			{
				for (int i = 0; i < shapeinput->size(); i++) // for loop goes thru the shapinput size 
				{
					if (!shapeinput->operator[](i) == NULL)
					{ 
						// using the operator to acces each object of shapeInput and his x an y 
						shapeinput->operator[](i)->getImage().copyTo(grid_mat(Rect(
							(shapeinput->operator[](i)->getGridx() * (projectorStepSize_i * 2)) + basex_i * 2 + 18,
							(shapeinput->operator[](i)->getGridy() * (projectorStepSize_i * 2)) + basey_i * 2 + 18,
							shapeinput->operator[](i)->getImage().cols, shapeinput->operator[](i)->getImage().rows)));
						cout << "shows shape " << endl;
					}
				}
			}
			if (checktime_b)// if checktime ==  true 
			{
				clock_t end_clock = clock();	// new clock declared 
				double elapsed_secs_d = double(end_clock - beginClock_clock) / CLOCKS_PER_SEC; 

				if (elapsed_secs_d > 5)
				{
					beginClock_clock = clock();
					showshapeimgs_b = false;
					checktime_b = false;
				}
			}
			if (win_b == true)
			{ // if win == true spawns win screen 
				winscreen_mat.copyTo(grid_mat(Rect(grid_mat.cols / 4, grid_mat.rows / 4, winscreen_mat.cols, winscreen_mat.rows)));
			}

			if (lose_b == true)
			{// if lose == true spawns lose screen 
				losescreen_mat.copyTo(grid_mat(Rect(grid_mat.cols / 4, grid_mat.rows / 4, losescreen_mat.cols, losescreen_mat.rows)));
			}

			Mat fist_time_bar = call_fist_time_bar(); // fist time bar appear 
			fist_time_bar.copyTo(grid_mat(Rect(709, 189, timer0_mat.cols, timer0_mat.rows))); // position of the fist time bar

			if (wincounter_i == 0 && losecounter_i == 0)
				new_game_mat.copyTo(grid_mat(Rect(600, 10, new_game_mat.cols, new_game_mat.rows)));
			if (wincounter_i == 1)
				one_win_mat.copyTo(grid_mat(Rect(600, 10, new_game_mat.cols, new_game_mat.rows)));
			if (wincounter_i == 2)
				two_win_mat.copyTo(grid_mat(Rect(600, 10, new_game_mat.cols, new_game_mat.rows)));
			if (losecounter_i == 1)
				one_wrong_mat.copyTo(grid_mat(Rect(600, 10, new_game_mat.cols, new_game_mat.rows)));
			if (losecounter_i == 2)
				two_wrong_mat.copyTo(grid_mat(Rect(600, 10, new_game_mat.cols, new_game_mat.rows)));

			putText(grid_mat,
				difficulty_text_string,
				Point(400, 50), // Coordinates
				FONT_HERSHEY_COMPLEX_SMALL, // Font
				2.0, // Scale. 2.0 = 2x bigger
				Scalar(0, 0, 0), // Color
				3, // Thickness
				CV_AA); // Anti-alias

			imshow("GRID", grid_mat);
		}
		else
		{
			waitKey(1);
			grid_mat = intro_mat.clone();

			Mat fist_time_bar = call_fist_time_bar();
			fist_time_bar.copyTo(grid_mat(Rect(700, 180, timer0_mat.cols, timer0_mat.rows)));

			imshow("GRID", grid_mat.clone()); // spawn the main screen 
		}
	}
}
