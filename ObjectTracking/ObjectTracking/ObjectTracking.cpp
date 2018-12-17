// ObjectTracking.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

using namespace std;
using namespace cv;

#define MIN_CONTOUR_AREA					500
#define MIN_DELTA_THRESHOLD					20
#define MIN_DELTA_AREA_THRESHOLD			0.15
#define MAX_IMGAE_LIMIT						10000
#define IMG_DIRECTORY						"Img/"

bool ImWrite(const String& filename, InputArray img);

int main() 
{
	char arrcMsg[128] = {0};
	Mat frame, gray, frameDelta, thresh, RefFrame;
	vector<vector<Point> > cnts;
	VideoCapture camera(0); //open camera
	int iCount = 0;
	int iImageCount = 0;
	double dContAreaSum = 0;
	double dMaxSum = 0;
	double dContArea = 0;
	bool isMotionDetected = false;
	float fFPS = 0;
	int iLoopCount = 0;
	TickMeter T;
	TickMeter CamTimer;


	//set the video size to 512x288 to process faster
	camera.set(CAP_PROP_FRAME_WIDTH, 640);
	camera.set(CAP_PROP_FRAME_HEIGHT, 480);
	
	while (iCount < 10)
	{
		waitKey(500);
		camera.read(frame); 
		imshow("Camera", frame);
		iCount++;
	}

	//convert to grayscale and set the first frame
	cvtColor(frame, RefFrame, COLOR_BGR2GRAY);
	GaussianBlur(RefFrame, RefFrame, Size(21, 21), 0);

	while (camera.read(frame)) {

		//convert to grayscale
		cvtColor(frame, gray, COLOR_BGR2GRAY);
		GaussianBlur(gray, gray, Size(21, 21), 0);

		//compute difference between first frame and current frame
		absdiff(RefFrame, gray, frameDelta);
		threshold(frameDelta, thresh, MIN_DELTA_THRESHOLD, 255, THRESH_BINARY);

		dilate(thresh, thresh, Mat(), Point(-1, -1), 2);
		findContours(thresh, cnts, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		dContAreaSum = 0;
		isMotionDetected = false;
		for (int i = 0; i < cnts.size(); i++) 
		{
			dContArea = contourArea(cnts[i]);
			dContAreaSum += dContArea;

			if (dMaxSum < dContAreaSum)
			{
				dMaxSum = dContAreaSum;
				
				// store the image
				sprintf_s(arrcMsg, "%sMaxSum.png", IMG_DIRECTORY);
				//ImWrite(arrcMsg, frame);
			}

			if (contourArea(cnts[i]) < MIN_CONTOUR_AREA) 
			{
				continue;
			}
			else
			{
				isMotionDetected = true;
			}
		}

		if (isMotionDetected == true)
		{
			if (iImageCount >= MAX_IMGAE_LIMIT)
			{
				iImageCount = 0;
			}
			else
			{
				iImageCount++;
			}
			
			// store to file
			if ((dContAreaSum / 100)> MIN_DELTA_AREA_THRESHOLD)
			{
				sprintf_s(arrcMsg, "%sImg-%0d.png", IMG_DIRECTORY, iImageCount);
				ImWrite(arrcMsg, frame);

				sprintf_s(arrcMsg, "Motion Detected: [%0.0f]", (dContAreaSum / 100));
				putText(frame, arrcMsg, Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
			}
		}

		imshow("Camera", frame);
		imshow("Delta", frameDelta);

		if (waitKey(1) == 27) {
			//exit if ESC is pressed
			break;
		}

		iLoopCount++;
		if (iLoopCount > 10)
		{
			T.stop();
			fFPS = (iLoopCount / T.getTimeSec());
			printf("\r\nFPS: %2.2f", fFPS);
			T.reset();
			T.start();
			iLoopCount = 0;
		}
	}

	return 0;
}


bool ImWrite(const String& filename, InputArray img)
{
	vector<int> compression_params;
	compression_params.push_back(IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(4);

	bool result = false;
	try
	{
		result = imwrite(filename, img, compression_params);
	}
	catch (const cv::Exception& ex)
	{
		fprintf(stderr, "Exception in ImWrite: %s\n", ex.what());
	}

	return result ? 0 : 1;
}