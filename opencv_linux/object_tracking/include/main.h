#ifndef _MAIN_H_
#define _MAIN_H_

// comment the below link for desktop compile
#define BEAGLE_COMPILE

// project headers
#include "ObjectTracking.h"
#include "MotionDectAlgo.h"
#include "Mutex.h"
#include "GPIOConst.h"
#include "GPIOManager.h"

using namespace cv;
using namespace std;
using namespace GPIO;

#ifdef BEAGLE_COMPILE
	#define CAMID								0
#else
	#define CAMID								1
#endif

#define	FRAMES_TO_PROCESS					50
#define MAX_CAM_FPS							30
#define FRAME_WIDTH							512
#define FRAME_HEIGHT						288

#define MAX_IMGAE_LIMIT						10000
#define IMG_DIRECTORY						"Img/"

#define ALERT_PIN 							"P8_7"

typedef struct GlobalHandle{
	
	Mat OrgFrame;
	Mat frame;
	Mat RefFrame;

	pthread_t iThreadAcqFrameID;
	pthread_t iThreadMotionDetectID;
	pthread_t iThreadLogImageID;
	pthread_t iThreadAlertID;

	int isAcqStarted;
	int isMotionDetected;

	int iFrameCount;
	int iMotionCount;

	float fFPSScalingFactor;
	MyMutex FrameMutex;
	MyMutex LogMutex;

	// GPIO 
	GPIOConst 		gpioConst;
	GPIOManager		BBgpio;

	int iAlertPin;

}S_GLOBAL_HANDLE;

extern S_GLOBAL_HANDLE g_Handle;

int CreateThreads();
int JoinThreads();
int GPIOInit();

#endif // _MAIN_H_