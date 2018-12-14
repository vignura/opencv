// OpenImage.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

using namespace std;
using namespace cv;

int main()
{
	int iKey = 0;
    
	cout << "Open Image\n";
	
	Mat mtImage = imread("lena.png");
	namedWindow("Open Image", WINDOW_NORMAL);
	imshow("Open Image", mtImage);
	
	iKey = waitKey(30 * 1000);
	return 0;
}

