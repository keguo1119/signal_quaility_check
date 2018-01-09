#include "oss.h"

////////////////////////////////////////////////////////////////////////////////
int MutexInit(HMUTEX *mutex)
{
#ifdef WIN32
	*mutex = CreateMutex( NULL , FALSE , NULL );
	if(NULL == *mutex)
	{
		return RET_FAILED;
	}
#else
	pthread_mutex_init(mutex,NULL);  
#endif		

	return  RET_OK;
}

////////////////////////////////////////////////////////////////////////////////
void MutexLock(HMUTEX *mutex)
{
#ifdef WIN32
	WaitForSingleObject(*mutex, INFINITE );
#else
	pthread_mutex_lock(mutex);  
#endif			
}

////////////////////////////////////////////////////////////////////////////////
void MutexUnLock(HMUTEX *mutex)
{
#ifdef WIN32
	ReleaseMutex(*mutex);
#else
	pthread_mutex_unlock(mutex);  
#endif			
}

