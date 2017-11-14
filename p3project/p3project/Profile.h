#pragma once
#include "stdafx.h"
#include <opencv2\opencv.hpp>

using namespace std;
using namespace cv;

class Profile
{
	float stats;
	Mat avatar;
	String username;
	String password;
	int Trophy[];

public:
	void newAccount();
	void Login();
	void ForgetPassword();
};
