// ObjectTracking.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include "ObjectTracking.h"
#include "MotionDectAlgo.h"


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
		printf("\nThread Acquire Frame creation Failed..!");
	}
	else
	{
		printf("\nThread Acquire Frame creation Success");
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
		printf("\nThread Motion Detect creation Failed..!");
	}
	else
	{
		printf("\nThread Motion Detect creation Success");
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
		printf("\nThread Log Image creation Failed..!");
	}
	else
	{
		printf("\nThread Log Image creation Success");
	}
	return 0;
}

DWORD WINAPI ThreadMotionDetect(LPVOID lpParam)
{
	int iRetVal = 0;
	int isMotionDeltaSet = false;
	int isSceneChanged = false;
	float fMotionFPS = 0;
	float fCamFPS = 0;
	char arrcMsg[128] = { 0 };
	
	TickMeter T;
	Mat gray, thresh, frame;
	Mat frameDelta[2];
	
	g_Handle.fFPSScalingFactor = 1.0f;

	while (g_Handle.isAcqStarted == false)
	{
		Sleep(100);
	}

	T.reset();
	T.start();


	while (1)
	{
		frame = g_Handle.frame.clone();

		// convert to grayscale
		cvtColor(frame, gray, COLOR_BGR2GRAY);
		GaussianBlur(gray, gray, Size(21, 21), 0);

		// detect any changes in scene
		MotionDetect(MD_IMG_SUBTRACT, gray, g_Handle.RefFrame, frameDelta[0], &isSceneChanged);
		
		// detect any movements in scene
		if (isSceneChanged == true)
		{
			MotionDetect(MD_TIME_DIFFERENCE, gray, g_Handle.RefFrame, frameDelta[1], &g_Handle.isMotionDetected);
			isMotionDeltaSet = true;
		}

		if (isSceneChanged == true)
		{
			sprintf_s(arrcMsg, "Scene changed");
			putText(frame, arrcMsg, Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
		}

		if (g_Handle.isMotionDetected == true)
		{
			sprintf_s(arrcMsg, "Motion Detected");
			putText(frame, arrcMsg, Point((FRAME_WIDTH / 2), 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
		}

		imshow("Camera", frame);
		imshow("Scene change Delta", frameDelta[0]);
		
		if (isMotionDeltaSet == true) {
			imshow("Motion Delta", frameDelta[1]);
		}

		if (waitKey(1) == 27) {
			//exit if ESC is pressed
			break;
		}

		g_Handle.iMotionCount++;
		if (g_Handle.iMotionCount > 100)
		{
			T.stop();
			fCamFPS = (float)(g_Handle.iFrameCount / T.getTimeSec());
			fMotionFPS = (float)(g_Handle.iMotionCount / T.getTimeSec());
			g_Handle.fFPSScalingFactor = fMotionFPS / fCamFPS;

			printf("\r\nMotion FPS: %2.2f \tCam FPS: %2.2f \tSF: %2.2f", fMotionFPS, fCamFPS, g_Handle.fFPSScalingFactor);
			T.reset();
			T.start();

			g_Handle.iFrameCount = 0;
			g_Handle.iMotionCount = 0;
		}

		Sleep((30 * g_Handle.fFPSScalingFactor));
	}

	return 0;
}

DWORD WINAPI ThreadAcqFrame(LPVOID lpParam)
{
	int iCount = 0;
	float fFPS = 0;
	VideoCapture camera(1);
	Size size(FRAME_WIDTH, FRAME_HEIGHT);

	//set the video size to 512x288 to process faster
	camera.set(CAP_PROP_FRAME_WIDTH, 640);
	camera.set(CAP_PROP_FRAME_HEIGHT, 480);

	while (iCount < 10)
	{
		waitKey(500);
		camera.read(g_Handle.OrgFrame);
		iCount++;
	}

	//convert to grayscale and set the first frame
	resize(g_Handle.OrgFrame, g_Handle.frame, size);
	cvtColor(g_Handle.frame, g_Handle.frame, COLOR_BGR2GRAY);
	GaussianBlur(g_Handle.frame, g_Handle.RefFrame, Size(21, 21), 0);

	g_Handle.iFrameCount = 0;

	while (camera.read(g_Handle.OrgFrame))
	{
		resize(g_Handle.OrgFrame, g_Handle.frame, size);
		Sleep(15);

		g_Handle.isAcqStarted = true;
		g_Handle.iFrameCount++;
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
				break;
			}
			else
			{
				iImageCount++;
			}

			sprintf_s(arrcMsg, "%sImg-%0d.png", IMG_DIRECTORY, iImageCount);
			ImWrite(arrcMsg, g_Handle.OrgFrame);
			g_Handle.isMotionDetected = false;
			Sleep(25);
		}

		Sleep(1);
	}

	return 0;
}
