// ObjectTracking.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include "ObjectTracking.h"

using namespace std;
using namespace cv;

#define MIN_CONTOUR_AREA					500
#define MIN_DELTA_THRESHOLD					20
#define MIN_DELTA_AREA_THRESHOLD			0.15
#define MAX_IMGAE_LIMIT						1000
#define IMG_DIRECTORY						"Img/"

S_GLOBAL_HANDLE g_Handle;


int main() 
{
	int iRetVal = 0;
	
	iRetVal = CreateThreads();

	WaitForSingleObject(g_Handle.hThreadAcqFrame, INFINITE);

	WaitForSingleObject(g_Handle.hThreadMotionDetect, INFINITE);

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

int CreateThreads()
{
	g_Handle.isAcqStarted = false;

	// create Frame Acqusition thread
	g_Handle.hThreadAcqFrame = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		ThreadAcqFrame,       // thread function name
		nullptr,          // argument to thread function 
		0,                      // use default creation flags 
		&g_Handle.dwThreadAcqFrame);

	if (g_Handle.hThreadAcqFrame == NULL)
	{
		printf("Thread Acquire Frame creation Failed..!");
	}
	else
	{
		printf("Thread Acquire Frame creation Success");
	}
	
	Sleep(500);

	// create Motion Detection thread
	g_Handle.hThreadMotionDetect = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		ThreadMotionDetect,       // thread function name
		nullptr,          // argument to thread function 
		0,                      // use default creation flags 
		&g_Handle.dwThreadMotionDetect);

	if (g_Handle.hThreadMotionDetect == NULL)
	{
		printf("Thread Motion Detect creation Failed..!");
	}
	else
	{
		printf("Thread Motion Detect creation Success");
	}

	// create Image logging thread
	g_Handle.hThreadLogImage = CreateThread(
		NULL,                   // default security attributes
		0,                      // use default stack size  
		ThreadLogImage,       // thread function name
		nullptr,          // argument to thread function 
		0,                      // use default creation flags 
		&g_Handle.dwThreadLogImage);

	if (g_Handle.hThreadLogImage == NULL)
	{
		printf("Thread Log Image creation Failed..!");
	}
	else
	{
		printf("Thread Log Image creation Success");
	}
	return 0;
}

DWORD WINAPI ThreadMotionDetect(LPVOID lpParam)
{
	char arrcMsg[128] = { 0 };
	Mat gray, frameDelta, thresh, frame;
	vector<vector<Point> > cnts;
	double dContAreaSum = 0;
	double dMaxSum = 0;
	double dContArea = 0;
	float fFPS = 0;
	int iLoopCount = 0;
	TickMeter T;
	TickMeter CamTimer;
	
	while (g_Handle.isAcqStarted == false)
	{
		Sleep(100);
	}

	while (1)
	{
		frame = g_Handle.frame.clone();

		//convert to grayscale
		cvtColor(frame, gray, COLOR_BGR2GRAY);
		GaussianBlur(gray, gray, Size(21, 21), 0);

		//compute difference between first frame and current frame
		absdiff(g_Handle.RefFrame, gray, frameDelta);
		threshold(frameDelta, thresh, MIN_DELTA_THRESHOLD, 255, THRESH_BINARY);

		dilate(thresh, thresh, Mat(), Point(-1, -1), 2);
		findContours(thresh, cnts, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

		dContAreaSum = 0;
		g_Handle.isMotionDetected = false;
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
				g_Handle.isMotionDetected = true;
			}
		}

		if (g_Handle.isMotionDetected == true)
		{
			if ((dContAreaSum / 100) > MIN_DELTA_AREA_THRESHOLD)
			{
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
		if (iLoopCount > 100)
		{
			T.stop();
			fFPS = (iLoopCount / T.getTimeSec());
			printf("\r\nFPS: %2.2f", fFPS);
			printf("\tcontours: %d", cnts.size());
			T.reset();
			T.start();
			iLoopCount = 0;
		}

		Sleep(15);
	}

	return 0;
}

DWORD WINAPI ThreadAcqFrame(LPVOID lpParam)
{
	int iCount = 0;
	Mat frame;
	VideoCapture camera(1);

	Size size(512, 288);
	//set the video size to 512x288 to process faster
	camera.set(CAP_PROP_FRAME_WIDTH, 640);
	camera.set(CAP_PROP_FRAME_HEIGHT, 480);

	while (iCount < 10)
	{
		waitKey(500);
		camera.read(frame);
		iCount++;
	}

	//convert to grayscale and set the first frame
	resize(frame, frame, size);
	cvtColor(frame, frame, COLOR_BGR2GRAY);
	GaussianBlur(frame, g_Handle.RefFrame, Size(21, 21), 0);

	g_Handle.isAcqStarted = true;
	while (camera.read(frame)) 
	{
		resize(frame, g_Handle.frame, size);
		Sleep(15);
	}
	
	return 0;
}

DWORD WINAPI ThreadLogImage(LPVOID lpParam)
{
	int iImageCount = 0;
	char arrcMsg[128] = { 0 };

	while (1)
	{
		if (g_Handle.isMotionDetected == true)
		{
			if (iImageCount >= MAX_IMGAE_LIMIT)
			{
				iImageCount = 0;
			}
			else
			{
				iImageCount++;
			}

			sprintf_s(arrcMsg, "%sImg-%0d.png", IMG_DIRECTORY, iImageCount);
			ImWrite(arrcMsg, g_Handle.frame);
		}

		Sleep(30);
	}

	return 0;
}