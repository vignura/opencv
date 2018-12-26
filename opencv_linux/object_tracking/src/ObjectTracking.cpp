#include "pch.h"
#include "main.h"

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



void* ThreadMotionDetect(void *parm)
{
	int iRetVal = 0;
	int isMotionDeltaSet = false;
	int isSceneChanged = false;
	int iSceneChangeCount = 0;
	int iMotionDectectCount = 0;

	float fMotionFPS = 0;
	float fCamFPS = 0;
	float fExcTime = 0;
	float fSleepTime = 30.0f;
	char arrcMsg[128] = { 0 };
	
	TickMeter T;
	TickMeter excT;
	Mat gray, thresh, frame;
	Mat frameDelta[2];
	
	g_Handle.fFPSScalingFactor = 1.0f;

	while (g_Handle.isAcqStarted == false)
	{
		usleep(100 * 1000);
	}

	T.reset();
	T.start();

	while (1)
	{
		excT.reset();
		excT.start();

		g_Handle.FrameMutex.Lock(g_Handle.iThreadMotionDetectID);

		frame = g_Handle.frame.clone();

		g_Handle.FrameMutex.Free(g_Handle.iThreadMotionDetectID);

		// convert to grayscale
		cvtColor(frame, gray, COLOR_BGR2GRAY);
		GaussianBlur(gray, gray, Size(21, 21), 0);

		// detect any changes in scene
		MotionDetect(MD_IMG_SUBTRACT, gray, g_Handle.RefFrame, frameDelta[0], &isSceneChanged);
		
		// detect any movements in scene
		if (isSceneChanged == true)
		{
			iMotionDectectCount++;
			MotionDetect(MD_TIME_DIFFERENCE, gray, g_Handle.RefFrame, frameDelta[1], &g_Handle.isMotionDetected);
			isMotionDeltaSet = true;

			if(g_Handle.isMotionDetected == true)
			{
				iSceneChangeCount++;
			}
		}

		#ifndef BEAGLE_COMPILE

			if (isSceneChanged == true)
			{
				sprintf(arrcMsg, "Scene changed");
				putText(frame, arrcMsg, Point(10, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
			}

			if (g_Handle.isMotionDetected == true)
			{
				sprintf(arrcMsg, "Motion Detected");
				putText(frame, arrcMsg, Point((FRAME_WIDTH / 2), 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
			}
			
			imshow("Camera", frame);
			imshow("Scene change Delta", frameDelta[0]);
		
			if (isMotionDeltaSet == true) {
				imshow("Motion Delta", frameDelta[1]);
			}
			
		#endif 
		
		excT.stop();
		fExcTime += excT.getTimeMilli();

		g_Handle.iMotionCount++;
		if (g_Handle.iMotionCount >= FRAMES_TO_PROCESS)
		{
			T.stop();
			fCamFPS = (float)(g_Handle.iFrameCount / T.getTimeSec());
			fMotionFPS = (float)(g_Handle.iMotionCount / T.getTimeSec());
			fSleepTime = ((1000.0f/ fCamFPS) - (fExcTime / FRAMES_TO_PROCESS));

			if(fCamFPS > fMotionFPS)
			{
				fSleepTime -= 5;	
			}
			else if(fCamFPS < fMotionFPS)
			{
				fSleepTime += 5;
			}
			else
			{
				// do nothing
			}

			if(fSleepTime < 5.0f)
			{
				fSleepTime = 5.0f;
			}
			else if(fSleepTime > 150)
			{
				fSleepTime = 150;	
			}

			printf("\r\nFPS: [Motion: %2.2f [%d]\tCam: %2.2f [%d]]\tSleep: %2.2f \tExc: %2.2f" 
						"\tSC Count: %d \tMD Count: %d", 
						fMotionFPS, g_Handle.iMotionCount, fCamFPS, g_Handle.iFrameCount, 
						fSleepTime, (fExcTime / FRAMES_TO_PROCESS),
						iSceneChangeCount, iMotionDectectCount);
			T.reset();
			T.start();

			iSceneChangeCount = 0;
			iMotionDectectCount = 0;
			g_Handle.iFrameCount = 0;
			g_Handle.iMotionCount = 0;
			fExcTime = 0;
		}

		usleep(fSleepTime * 1000);
		//usleep(30 * 1000);
	}

	return 0;
}

void* ThreadAcqFrame(void *parm)
{
	int iCount = 0;
	float fFPS = 0;
	VideoCapture camera(CAMID);
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
		g_Handle.LogMutex.Free(g_Handle.iThreadAcqFrameID);

		g_Handle.FrameMutex.Lock(g_Handle.iThreadAcqFrameID);
		resize(g_Handle.OrgFrame, g_Handle.frame, size);
		g_Handle.FrameMutex.Free(g_Handle.iThreadAcqFrameID);

		usleep(33 * 1000);

		// lock the mutex
		g_Handle.LogMutex.Lock(g_Handle.iThreadAcqFrameID);

		g_Handle.isAcqStarted = true;
		g_Handle.iFrameCount++;
	}

	return 0;
}

void* ThreadLogImage(void *parm)
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

			sprintf(arrcMsg, "%sImg-%0d.png", IMG_DIRECTORY, iImageCount);

			g_Handle.LogMutex.Lock(g_Handle.iThreadLogImageID);

			ImWrite(arrcMsg, g_Handle.OrgFrame);

			g_Handle.LogMutex.Free(g_Handle.iThreadLogImageID);

			g_Handle.isMotionDetected = false;
			usleep(25 * 1000);
		}

		usleep(1 * 1000);
	}

	return 0;
}
