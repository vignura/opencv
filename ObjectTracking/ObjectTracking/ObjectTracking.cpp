// ObjectTracking.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

using namespace std;
using namespace cv;

int main()
{
	Mat mtFrame;
	VideoCapture camera(0);

    cout << "Object Detection\n"; 
	namedWindow("Object Detect", WINDOW_GUI_NORMAL);

	while (camera.read(mtFrame))
	{
		imshow("Object Detect", mtFrame);
		
		if (waitKey(35) == 27) {
			//exit if ESC is pressed
			break;
		}
	}
}

