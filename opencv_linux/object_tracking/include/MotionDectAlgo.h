#ifndef _MOTION_DETECT_H_
#define _MOTION_DETECT_H_

#define MIN_CONTOUR_AREA					500
#define MIN_DELTA_THRESHOLD					25

// Algorithms
#define MD_IMG_SUBTRACT						0
#define MD_TIME_DIFFERENCE					1
#define MD_THREE_FRAME_DIFF					2

int MotionDetect(int iAlgorithm, Mat &Frame, Mat &RefFrame, Mat &FrameDelta, int *isMotionDetected);
int ThreshContour(vector<vector<Point>> &Contours, int iAreaThreshold);

// motion dections algorithms
int MDImageSubraction(Mat &Frame, Mat &RefFrame, Mat &FrameDelta, int *isMotionDetected);
int MDTimeDiffernce(Mat &Frame, Mat &RefFrame, Mat &FrameDelta, int *isMotionDetected);
int MDThreeFrameDiff(Mat &Frame, Mat &RefFrame, Mat &FrameDelta, int *isMotionDetected);

#endif // _MOTION_DETECT_H_