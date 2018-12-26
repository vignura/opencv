#ifndef _MUTEX_H_
#define _MUTEX_H_

#define MUTEX_SUCCESS				0x00
#define MUTEX_FAILURE				0x01

#define MUTEX_STATE_FREE			0x00
#define MUTEX_STATE_LOCKED			0x01

class MyMutex
{
	int m_iState;
	int m_iOwnerID;
	
	void MutexSleep(int iSleepTime_ms);

public:
	MyMutex();
	int Lock(int iOwnerID);
	int Lock(int iOwnerID, int iTimeout_ms);
	int Free(int iOwnerID);
};

#endif // _MUTEX_H_