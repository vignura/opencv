#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

#define FRAME_WIDTH							512
#define FRAME_HEIGHT						288

#define MAX_IMGAE_LIMIT						10000
#define IMG_DIRECTORY						"Img/"

typedef struct GlobalHandle{
	
	Mat frame;
	Mat RefFrame;

	HANDLE hThreadAcqFrame;
	DWORD dwThreadAcqFrame;

	HANDLE hThreadMotionDetect;
	DWORD dwThreadMotionDetect;

	HANDLE hThreadLogImage;
	DWORD dwThreadLogImage;

	int isAcqStarted;
	int isMotionDetected;

}S_GLOBAL_HANDLE;


bool ImWrite(const String& filename, InputArray img);
DWORD WINAPI ThreadMotionDetect(LPVOID lpParam);
DWORD WINAPI ThreadAcqFrame(LPVOID lpParam);
DWORD WINAPI ThreadLogImage(LPVOID lpParam);
int CreateThreads();