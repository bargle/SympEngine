#ifndef _THREAD_SYNC_H_
#define _THREAD_SYNC_H_

#ifdef _MSC_VER
	#include "threadsync_win.h"
#endif //WIN32

#ifdef LINUX
	#include "threadsync_linux.h"
#endif //LINUX

class AutoThreadSync
{
public:
	AutoThreadSync():m_pThreadSync(NULL){}
	AutoThreadSync(ThreadSync* pThreadSync):
	m_pThreadSync(pThreadSync)
	{
		m_pThreadSync->Lock();
	}
	~AutoThreadSync()
	{
		assert(m_pThreadSync);
		m_pThreadSync->Unlock();
	}

protected:
	ThreadSync* m_pThreadSync;
};

#endif