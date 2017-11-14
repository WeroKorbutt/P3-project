#include "stdafx.h"
#include "MainMenu.h"

using namespace cv;
using namespace std;

void newGame() {

	Mat cat1;
	Mat image;
	cat1 = imread("Home_screen.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	if (cat1.data && !cat1.empty())
		imshow("Grey kat", cat1);
	else if (true)
		cout << "No grey kat" << endl;
}

