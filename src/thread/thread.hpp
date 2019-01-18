#ifdef THREAD_ENABLE

#ifndef __THREAD_H__
#define __THREAD_H__

#include "sys_threading.hpp"

typedef struct mutex {
	mutexHandle_t handle;
} mutex_t;

typedef struct signal {
	signalHandle_t handle;
} signal_t;

typedef struct thread {
	const char * name;
	int (*func)(void*);
	void * arg;
	uintptr_t threadHandle;
	bool isFuncRunning;
	bool isWorker;
	bool isRunning;
	bool isTerminating;
	bool moreWorkToDo;
	signal_t * signalWorkerDone;
	signal_t * signalMoreWorkToDo;
	mutex_t	* signalMutex;
} thread_t;

mutex_t		*	NewMutex();
void			DeleteMutex( mutex_t * m );
bool			LockMutex( mutex_t * m, bool blocking );
void			UnlockMutex( mutex_t * m );

signal_t	*	NewSignal( bool manualReset );
void			DeleteSignal( signal_t * s );
void			RaiseSignal( signal_t * s );
void			ClearSignal( signal_t * s );
bool			WaitSignal( signal_t * s, int timeout );

thread_t	*	NewThread();
void			DeleteThread( thread_t * t );
const char	*	GetThreadName( thread_t * t );
uintptr_t		GetThreadHandle( thread_t * t );
bool			ThreadIsRunning( thread_t * t );
bool			ThreadIsTerminating( thread_t * t );
bool 			ThreadIsFuncRunning( thread_t * t );
int				RunThread( thread_t * t );
int				ThreadProc( thread_t * t );
bool			StartThread( thread_t * t, const char * name_, int (*f)(void*), void * arg, int core, xthreadPriority_t priority,
					int stackSize );
bool			StartWorkerThread( thread_t * t, const char * name_, int (*f)(void*), void * arg, int core, xthreadPriority_t priority,
					int stackSize );
void			WaitForThread( thread_t * t );

void			StopThread( thread_t * t, bool wait );
void			SignalWork( thread_t * t );
bool			WorkIsDone( thread_t * t );

#endif	// !__THREAD_H__

#endif // THREAD_ENABLE
