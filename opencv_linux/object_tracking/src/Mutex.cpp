#include "pch.h"
#include "Mutex.h"

MyMutex::MyMutex()
{
	m_iState = MUTEX_STATE_FREE;
	m_iOwnerID = 0;
}

int MyMutex:: Lock(int iOwnerID)
{
	int iRetVal = 0;

	while(1)
	{
		if(m_iState == MUTEX_STATE_FREE)
		{
			m_iState = MUTEX_STATE_LOCKED;
			m_iOwnerID = iOwnerID;
			break;
		}
		else
		{
			MutexSleep(1);
		}
	}

	return iRetVal;
}

int MyMutex:: Lock(int iOwnerID, int iTimeout_ms)
{
	int iRetVal = 0;
	int iTimeoutCount = 0;

	while(iTimeoutCount <= iTimeout_ms)
	{
		if(m_iState == MUTEX_STATE_FREE)
		{
			m_iState = MUTEX_STATE_LOCKED;
			m_iOwnerID = iOwnerID;
			break;
		}
		else
		{
			MutexSleep(1);
		}
	}
	
	iRetVal = MUTEX_FAILURE;

	return iRetVal;
}

int MyMutex::Free(int iOwnerID)
{
	int iRetVal = 0;

	if(m_iOwnerID == iOwnerID)
	{
		m_iState = MUTEX_STATE_FREE;
	}
	else
	{
		iRetVal = MUTEX_FAILURE;
	}

	return iRetVal;
}

void MyMutex::MutexSleep(int iSleepTime_ms)
{
	#ifdef linux
		
		usleep(iSleepTime_ms * 1000);

	#else
	
	#endif
}