#ifndef _THREAD_SYNC_GCC_H_
#define _THREAD_SYNC_GCC_H_

class ThreadSync
{
public:
	ThreadSync()
	{
		Init();
	}

	~ThreadSync()
	{
		Term();
	}

	inline void Init()
	{
		int nRet = pthread_mutex_init(&m_Mutex, NULL);
		assert(nRet == 0);
	}

	inline void Term()
	{
		pthread_mutex_destroy(&m_Mutex);
	}

	inline void Lock()
	{
		pthread_mutex_lock(&m_Mutex);
	}
	
	inline void TryLock()
	{
		pthread_mutex_trylock(&m_Mutex);
	}	

	inline void Unlock()
	{
		pthread_mutex_unlock(&m_Mutex);
	}
	
	inline pthread_mutex_t* GetSyncObject(){ return &m_Mutex; }

protected:
	pthread_mutex_t m_Mutex;

};

#endif
