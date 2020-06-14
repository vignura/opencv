#include "pch.h"
#include "main.h"

void create_segfault()
{
	int a = 0xAA;

	/* accessing a null pointer to cause segfault */
	*((int *)0xBB) = 1;
	
	return;
}

int MotionDetect(int iAlgorithm,  Mat &Frame, Mat &RefFrame, Mat &FrameDelta, int *isMotionDetected)
{
	switch (iAlgorithm)
	{
		case MD_IMG_SUBTRACT:
			MDImageSubraction(Frame, RefFrame, FrameDelta, isMotionDetected);
		break;

		case MD_TIME_DIFFERENCE:
			MDTimeDiffernce(Frame, RefFrame, FrameDelta, isMotionDetected);
		break;

		default:
			return -1;
	}

	return 0;
}

int ThreshContour(vector<vector<Point>> &Contours, int iAreaThreshold)
{
	// create_segfault();

	for (int i = 0; i < Contours.size(); i++)
	{
		if (contourArea(Contours[i]) < iAreaThreshold)
		{
			continue;
		}
		else
		{
			return true;
		}
	}

	return false;
}

int MDImageSubraction(Mat &Frame, Mat &RefFrame, Mat &FrameDelta, int *isMotionDetected)
{
	Mat Thresh;
	vector<vector<Point>> cnts;

	if (isMotionDetected == NULL)
	{
		return -1;
	}

	//compute difference between first frame and current frame
	absdiff(RefFrame, Frame, FrameDelta);
	threshold(FrameDelta, Thresh, MIN_DELTA_THRESHOLD, 255, THRESH_BINARY);

	dilate(Thresh, Thresh, Mat(), Point(-1, -1), 2);
	findContours(Thresh, cnts, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	
	// set motion detected if a contour is found with area grater than or equal to MIN_CONTOUR_AREA
	*isMotionDetected = ThreshContour(cnts, MIN_CONTOUR_AREA);

	return 0;
}

int MDTimeDiffernce(Mat &Frame, Mat &RefFrame, Mat &FrameDelta, int *isMotionDetected)
{
	Mat Thresh;
	static Mat PreFrame;
	static int isPreFameset = false;
	vector<vector<Point>> cnts;

	if (isMotionDetected == NULL)
	{
		return -1;
	}

	// set the previous frame to reference for the first time
	if (isPreFameset == false)
	{
		PreFrame = RefFrame.clone();
		isPreFameset = true;
	}

	//compute difference between first frame and current frame
	absdiff(PreFrame, Frame, FrameDelta);
	threshold(FrameDelta, Thresh, MIN_DELTA_THRESHOLD, 255, THRESH_BINARY);

	dilate(Thresh, Thresh, Mat(), Point(-1, -1), 2);
	findContours(Thresh, cnts, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	// set motion detected if a contour is found with area grater than or equal to MIN_CONTOUR_AREA
	*isMotionDetected = ThreshContour(cnts, MIN_CONTOUR_AREA);

	// set the current frame to previous frame
	PreFrame = Frame.clone();

	return 0;
}

int MDThreeFrameDiff(Mat &Frame, Mat &RefFrame, Mat &FrameDelta, int *isMotionDetected)
{
	Mat Thresh;
	static Mat PreFrame[2];
	static int isPreFameset = false;
	vector<vector<Point>> cnts;

	if (isMotionDetected == NULL)
	{
		return -1;
	}

	// set the previous frame to reference for the first time
	if (isPreFameset == false)
	{
		PreFrame[0] = RefFrame.clone();
		PreFrame[1] = RefFrame.clone();
		isPreFameset = true;
	}

	//compute difference between first frame and current frame
	absdiff(PreFrame[0], Frame, FrameDelta);
	threshold(FrameDelta, Thresh, MIN_DELTA_THRESHOLD, 255, THRESH_BINARY);

	dilate(Thresh, Thresh, Mat(), Point(-1, -1), 2);
	findContours(Thresh, cnts, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	// set motion detected if a contour is found with area grater than or equal to MIN_CONTOUR_AREA
	*isMotionDetected = ThreshContour(cnts, MIN_CONTOUR_AREA);

	// set the current frame to previous frame
	PreFrame[1] = PreFrame[0].clone();
	PreFrame[0] = Frame.clone();

	return 0;
}