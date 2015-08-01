#ifndef _THREAD_EVENT_GCC_H
#define _THREAD_EVENT_GCC_H

#include "threadsync_linux.h"

class ThreadEvent
{
public:
	ThreadEvent():m_bSet(false)
	{
		int nRet = pthread_cond_init(&m_Cond, NULL);
		assert(nRet == 0);
		nRet = pthread_mutex_init(&m_Mutex, NULL);
		assert(nRet == 0);
	}
	~ThreadEvent()
	{
		pthread_cond_destroy(&m_Cond);	
		pthread_mutex_destroy(&m_Mutex);
	}
	
	inline void Set()
	{
		pthread_mutex_lock(&m_Mutex);
		m_bSet = true;
		pthread_cond_signal(&m_Cond);
		pthread_mutex_unlock(&m_Mutex);
	}
	
	inline void SetAll()
	{
		pthread_mutex_lock(&m_Mutex);
		m_bSet = true;
		pthread_cond_broadcast(&m_Cond);
		pthread_mutex_unlock(&m_Mutex);
	}	
	
	inline void Reset()
	{
		pthread_mutex_lock(&m_Mutex);
		m_bSet = false;
		pthread_mutex_unlock(&m_Mutex);
	}
	
	inline void Wait()
	{
		pthread_mutex_lock(&m_Mutex);
		if(!m_bSet)
		{
			pthread_cond_wait(&m_Cond, &m_Mutex);
		}
		pthread_mutex_unlock(&m_Mutex);
	}
	
protected:
	pthread_cond_t		m_Cond;
	pthread_mutex_t 	m_Mutex;
	bool				m_bSet;
};

#endif
