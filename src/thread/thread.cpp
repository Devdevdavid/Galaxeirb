#ifdef THREAD_ENABLE

#include "stdio.h"
#include "stdlib.h"
#include "thread.hpp"

mutex_t * NewMutex() {
	mutex_t * m = (mutex_t*)malloc( sizeof( mutex_t ) );
	if ( m ) {
		Sys_MutexCreate( &m->handle );
	}
	return m;
}

void DeleteMutex( mutex_t * m ) {
	if ( m )  {
		Sys_MutexDestroy( &m->handle );
		free( m );
	}
}

bool LockMutex( mutex_t * m, bool blocking ) {
	return Sys_MutexLock( &m->handle, blocking );
}

void UnlockMutex( mutex_t * m ) { 
	Sys_MutexUnlock( &m->handle ); 
}

signal_t * NewSignal( bool manualReset ) {
	signal_t * s = (signal_t*)malloc( sizeof( signal_t ) );
	if ( s ) {
		Sys_SignalCreate( &s->handle, manualReset ); 
	}
	return s;
}

void DeleteSignal( signal_t * s ) {
	if ( s )  {
		Sys_SignalDestroy( &s->handle ); 
		free( s );
	}
}

void RaiseSignal( signal_t * s ) {
	Sys_SignalRaise( &s->handle );
}

void ClearSignal( signal_t * s ) {
	Sys_SignalClear( &s->handle ); 
}

bool WaitSignal( signal_t * s, int timeout ) { 
	return Sys_SignalWait( &s->handle, timeout );
}

thread_t * NewThread() {
	thread_t * t = (thread_t*)malloc( sizeof( thread_t ) );
	if ( t ) {
		t->threadHandle = 0;
		t->func = NULL;
		t->arg = NULL;
		t->isWorker = false;
		t->isRunning = false;
		t->isTerminating = false;
		t->moreWorkToDo = false;
		t->signalWorkerDone = NewSignal( false );
		t->signalMoreWorkToDo = NewSignal( false );
		t->signalMutex = NewMutex();
	}
	return t;
}

void DeleteThread( thread_t * t ) {
	if ( t ) {
		StopThread( t, true );
		if ( t->threadHandle ) {
			Sys_DestroyThread( t->threadHandle );
		}
		free( t->signalWorkerDone );
		free( t->signalMoreWorkToDo );
		free( t->signalMutex );
		free( t );
	}
}

const char * GetThreadName( thread_t * t ) {
	return t->name;
}

uintptr_t GetThreadHandle( thread_t * t ) {
	return t->threadHandle;
}

bool ThreadIsRunning( thread_t * t ) {
	return t->isRunning;
}

bool ThreadIsTerminating( thread_t * t ) {
	return t->isTerminating;
}
bool ThreadIsFuncRunning( thread_t * t ) {
	return t->isFuncRunning;
}

int RunThread( thread_t * t ) {
	//int i = t->func( t->arg );
	t->func( t->arg );
	t->isFuncRunning = false;
	return 0;
}

int ThreadProc( thread_t * t ) {
	int retVal = 0;
	
	if ( t->isWorker ) {
		for( ; ; ) {
			LockMutex( t->signalMutex, true );
			if ( t->moreWorkToDo ) {
				t->moreWorkToDo = false;
				ClearSignal( t->signalMoreWorkToDo );
				UnlockMutex( t->signalMutex );
			} else {
				RaiseSignal( t->signalWorkerDone );
				UnlockMutex( t->signalMutex );
				WaitSignal( t->signalMoreWorkToDo, WAIT_INFINITE );
				continue;
			}
			if ( t->isTerminating ) {
				break;
			}
			retVal = RunThread( t );
		}
		RaiseSignal( t->signalWorkerDone );
	} else {
		retVal = RunThread( t );
	}

	t->isRunning = false;

	return retVal;
}

bool StartThread( thread_t * t, const char * name_, int (*f)(void*), void * arg, int core, xthreadPriority_t priority,
					int stackSize ) {

	if ( t->isRunning ) {
		return false;
	}

	t->name = name_;
	t->func = f;
	t->arg = arg;

	t->isTerminating = false;
	t->isFuncRunning = false;

	if ( t->threadHandle ) {
		Sys_DestroyThread( t->threadHandle );
	}

	t->threadHandle = Sys_CreateThread( (xthread_t)ThreadProc, t, priority, t->name, core, stackSize, false );

	t->isRunning = true;

	return true;

}

bool StartWorkerThread( thread_t * t, const char * name_, int (*f)(void*), void * arg, int core, xthreadPriority_t priority,
					int stackSize ) {

	bool result;

	if ( t->isRunning ) {
		return false;
	}

	t->isWorker = true;

	result = StartThread( t, name_, f, arg, core, priority, stackSize );

	WaitSignal( t->signalWorkerDone, WAIT_INFINITE );

	return result;
}

void WaitForThread( thread_t * t ) {
	if ( t->isWorker ) {
		WaitSignal( t->signalWorkerDone, WAIT_INFINITE );
	} else if ( t->isRunning ) {
		Sys_DestroyThread( t->threadHandle );
		t->threadHandle = 0;
	}
}

void StopThread( thread_t * t, bool wait ) {
	if ( !t->isRunning ) {
		return;
	}
	if ( t->isWorker ) {
		LockMutex( t->signalMutex, true );
		t->moreWorkToDo = true;
		ClearSignal( t->signalWorkerDone );
		t->isTerminating = true;
		RaiseSignal( t->signalMoreWorkToDo );
		UnlockMutex( t->signalMutex );
	} else {
		t->isTerminating = true;
	}
	if ( wait ) {
		WaitForThread( t );
	}

}

void SignalWork( thread_t * t ) {
	if ( t->isWorker ) {
		t->isFuncRunning = true;
		LockMutex( t->signalMutex, true );
		t->moreWorkToDo = true;
		ClearSignal( t->signalWorkerDone );
		RaiseSignal( t->signalMoreWorkToDo );
		UnlockMutex( t->signalMutex );
	}
}

bool WorkIsDone( thread_t * t ) {
	if ( t->isWorker ) {
		// a timeout of 0 will return immediately with true if signaled
		if ( WaitSignal( t->signalWorkerDone, 0 ) ) {
			return true;
		}
	}
	return false;
}

#endif // THREAD_ENABLE
