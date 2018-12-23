#ifndef _MAIN_H_
#define _MAIN_H_

// project headers
#include "ObjectTracking.h"
#include "MotionDectAlgo.h"

using namespace cv;
using namespace std;

#define MAX_CAM_FPS							30
#define FRAME_WIDTH							512
#define FRAME_HEIGHT						288

#define MAX_IMGAE_LIMIT						10000
#define IMG_DIRECTORY						"Img/"

typedef struct GlobalHandle{
	
	Mat OrgFrame;
	Mat frame;
	Mat RefFrame;

	pthread_t iThreadAcqFrameID;
	pthread_t iThreadMotionDetectID;
	pthread_t iThreadLogImageID;

	int isAcqStarted;
	int isMotionDetected;

	int iFrameCount;
	int iMotionCount;

	float fFPSScalingFactor;

}S_GLOBAL_HANDLE;

extern S_GLOBAL_HANDLE g_Handle;

int CreateThreads();
int JoinThreads();
#endif // _MAIN_H_