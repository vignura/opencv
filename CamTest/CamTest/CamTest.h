#pragma once

#define MAX_CAM_COUNT			5
#define MAX_TEST_FRAMES			25

#define MAX_TEST_RESOLUTIONS	4
#define TEST_RESOLUTIONS	{{512, 288},\
							 {640, 480},\
							 {800, 600},\
							 {1280, 720}\
							}

typedef struct Resolution
{
	int iWidth;
	int iHeight;
}Resolution;

void CamTest();
int FindCams(int *piCamCount);
float getCamFPS(int iCamera, int iWidth, int iHeight);
