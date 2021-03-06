// CamTesTimer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "CamTest.h"

using namespace std;
using namespace cv;

int main()
{
    cout << "Cam Test\n"; 
	
	CamTest();

	return 0;
}

void CamTest()
{
	int iCamCount = 0;
	int iRetVal = 0;
	Resolution arrRes[MAX_TEST_RESOLUTIONS] = TEST_RESOLUTIONS;
	
	// find cameras
	iRetVal = FindCams(&iCamCount);

	cout << "Calculating Camera FPS...\n";
	for (int i = 0; i < iCamCount; i++)
	{
		for (int j = 0; j < MAX_TEST_RESOLUTIONS; j++)
		{
			cout << "FPS: " << getCamFPS(i, arrRes[j].iWidth, arrRes[j].iHeight) << endl;
		}
	}
}

int FindCams(int *piCamCount)
{
	int iCamCount = 0;
	int iRetVal = 0;

	if (piCamCount == nullptr)
	{
		return -1;
	}

	// find the number of cameras available
	cout << "Detecting connected cameras...\n";
	iCamCount = 0;
	for (int i = 0; i < MAX_CAM_COUNT; i++)
	{
		VideoCapture Camera;
		iRetVal = Camera.open(iCamCount);
		if (iRetVal == true)
		{
			cout << "Cam " << iCamCount << " found\n";
			iCamCount++;

			//Camera.release();
		}
		else
		{
			if (iCamCount == 0)
			{
				cout << "No cameras found\n";
			}
			else
			{
				cout << iCamCount << " cameras found\n";
			}
			break;
		}

	}

	*piCamCount = iCamCount;

	return 0;
}

float getCamFPS(int iCamera, int iWidth, int iHeight)
{
	int iFrameCount = 0;
	float fFPS = 0;
	int iFPS = 30;
	
	Mat frame;
	TickMeter Timer;
	VideoCapture Camera;
	
	cout << "Opening Cam " << iCamera;
	if (Camera.open(iCamera))
	{
		cout << "\t opened\n";
		cout << "Setting Cam resolution to " << iWidth << "x" << iHeight;

		if ((Camera.set(CAP_PROP_FRAME_WIDTH, iWidth)) && (Camera.set(CAP_PROP_FRAME_HEIGHT, iHeight)))
		{
			cout << "\t OK\n";

			cout << "Setting Camera FPS to " << iFPS;
			if (Camera.set(CAP_PROP_FPS, iFPS))
			{
				cout << "\t OK\n";
			}
			else
			{
				cout << "\t failed.!\n";
			}

			Timer.reset();
			Timer.start();

			for (int i = 0; i < MAX_TEST_FRAMES; i++)
			{
				if (Camera.read(frame) == false)
				{
					cout << "unable to read\n";
				}
				else
				{
					iFrameCount++;
				}

				waitKey(1);
			}

			Timer.stop();
			fFPS = (float)((iFrameCount / Timer.getTimeMilli()) * 1000);
		}
		else
		{
			cout << "\t resolution not supported.!\n";
		}
	}
	else
	{
		cout << "\t failed\n";
	}

	return fFPS;
}