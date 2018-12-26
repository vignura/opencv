#include "pch.h"
#include "main.h"

S_GLOBAL_HANDLE g_Handle;


int main() 
{
	int iRetVal = 0;
	
	printf("\r\nMotion Detect");
	iRetVal = CreateThreads();

	// join threads
	iRetVal = JoinThreads();

	return 0;
}

int CreateThreads()
{
	int iRetVal = 0;

	g_Handle.isAcqStarted = false;

#if 1
	printf("\r\nCreating Acquire Frame thread");
	// create Frame Acqusition thread
	iRetVal = pthread_create(&g_Handle.iThreadAcqFrameID, NULL, ThreadAcqFrame, NULL);
	if (iRetVal != 0)
	{
		printf("\nThread Acquire Frame creation Failed..!");
	}
	else
	{
		printf("\nThread Acquire Frame creation Success: ID : %d", g_Handle.iThreadAcqFrameID);
	}
#endif

	usleep(500 * 1000);
	printf("\r\nCreating Motion Detect thread");
	iRetVal = pthread_create(&g_Handle.iThreadMotionDetectID, NULL, ThreadMotionDetect, NULL);
	if (iRetVal != 0)
	{
		printf("\nThread Motion Detect creation Failed..!");
	}
	else
	{
		printf("\nThread Motion Detect creation Success: ID : %d", g_Handle.iThreadMotionDetectID);
	}

#if 1
	// create Image logging thread
	printf("\r\nCreating Log Image thread");
	iRetVal = pthread_create(&g_Handle.iThreadLogImageID, NULL, ThreadLogImage, NULL);
	if (iRetVal != 0)
	{
		printf("\nThread Log Image creation Failed..!");
	}
	else
	{
		printf("\nThread Log Image creation Success: ID : %d", g_Handle.iThreadLogImageID);
	}
#endif
	return 0;
}

int JoinThreads()
{
	int iRetVal = 0;

	iRetVal = pthread_join(g_Handle.iThreadAcqFrameID, NULL);
	if(iRetVal != 0)
	{
		printf("\r\npthread_join failed for thread ID: %d", g_Handle.iThreadAcqFrameID);
	}

	iRetVal = pthread_join(g_Handle.iThreadMotionDetectID, NULL);
	if(iRetVal != 0)
	{
		printf("\r\npthread_join failed for thread ID: %d", g_Handle.iThreadMotionDetectID);
	}

	iRetVal = pthread_join(g_Handle.iThreadLogImageID, NULL);
	if(iRetVal != 0)
	{
		printf("\r\npthread_join failed for thread ID: %d", g_Handle.iThreadLogImageID);
	}

	return iRetVal;
}