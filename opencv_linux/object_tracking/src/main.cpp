#include "pch.h"
#include "main.h"

S_GLOBAL_HANDLE g_Handle;


int main() 
{
	int iRetVal = 0;
	
	printf("\r\nMotion Detect");

	iRetVal = GPIOInit();

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

	// create Image logging thread
	printf("\r\nCreating Log Image thread");
	iRetVal = pthread_create(&g_Handle.iThreadAlertID, NULL, ThreadAlert, NULL);
	if (iRetVal != 0)
	{
		printf("\nThread Alert creation Failed..!");
	}
	else
	{
		printf("\nThread Alert creation Success: ID : %d", g_Handle.iThreadAlertID);
	}

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

	iRetVal = pthread_join(g_Handle.iThreadAlertID, NULL);
	if(iRetVal != 0)
	{
		printf("\r\npthread_join failed for thread ID: %d", g_Handle.iThreadAlertID);
	}

	return iRetVal;
}

int GPIOInit()
{
	int iRetVal = 0;

	int iPin = g_Handle.gpioConst.getGpioByKey(ALERT_PIN);

	// export the pin
	iRetVal = g_Handle.BBgpio.exportPin(iPin);
	if(iRetVal < 0)
	{
		printf("\r\nUnable to export %s", ALERT_PIN);
	}
	else
	{
	
		// set the pin as ouput
		iRetVal = g_Handle.BBgpio.setDirection(iPin, OUTPUT);
		if(iRetVal < 0)
		{
			printf("\r\nUnable to set direction of %s to %d", ALERT_PIN, OUTPUT);
		}

		// test GPIO
		g_Handle.BBgpio.setValue(iPin, HIGH);
		usleep(2000 * 1000);
		g_Handle.BBgpio.setValue(iPin, LOW);

		// assign to global
		g_Handle.iAlertPin = iPin;
	}
	
	return iRetVal;
}