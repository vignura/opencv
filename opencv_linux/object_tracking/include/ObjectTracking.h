#ifndef _OBJECT_TRACKING_H_
#define _OBJECT_TRACKING_H_

using namespace cv;
using namespace std;

bool ImWrite(const String& filename, InputArray img);
void* ThreadMotionDetect(void *parm);
void* ThreadAcqFrame(void *parm);
void* ThreadLogImage(void *parm);
void* ThreadAlert(void *parm);

#endif // _OBJECT_TRACKING_H_