#ifndef __SYS_THREADING_H__
#define __SYS_THREADING_H__

#ifdef THREAD_ENABLE

#ifndef __TYPEINFOGEN__

#if defined(_WIN32)
#include "windows.h"
#include <intrin.h>
typedef CRITICAL_SECTION		mutexHandle_t;
typedef HANDLE					signalHandle_t;
typedef LONG					interlockedInt_t;
#else

#define _GNU_SOURCE

#include "stdint.h"
#include <time.h>
#include <stdbool.h>
//#include <errno.h>
#include <pthread.h>

typedef struct signalHandle {
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	int waiting;
	bool manualReset;
	bool signaled;
} signalHandle_t;

typedef pthread_mutex_t 	mutexHandle_t;
typedef int 				interlockedInt_t;

#endif

#if defined(_MSC_VER)
// _ReadWriteBarrier() does not translate to any instructions but keeps the compiler
// from reordering read and write instructions across the barrier.
// MemoryBarrier() inserts and CPU instruction that keeps the CPU from reordering reads and writes.
#pragma intrinsic(_ReadWriteBarrier)
#define SYS_MEMORYBARRIER		_ReadWriteBarrier(); MemoryBarrier()
#elif defined( __GNUC__)
#define SYS_MEMORYBARRIER 		asm volatile("" ::: "memory");__sync_synchronize()
#endif

#endif // __TYPEINFOGEN__

#define CORE_ANY	0xFF

#define CORE_0		0x01
#define CORE_1		0x02
#define CORE_2		0x04
#define CORE_3		0x08
#define CORE_4		0x10
#define CORE_5		0x20
#define CORE_6		0x40
#define CORE_7		0x80

typedef unsigned int (*xthread_t)( void * );

typedef enum {
	THREAD_LOWEST,
	THREAD_BELOW_NORMAL,
	THREAD_NORMAL,
	THREAD_ABOVE_NORMAL,
	THREAD_HIGHEST
}xthreadPriority_t;

#define DEFAULT_THREAD_STACK_SIZE		( 4 * 1024 )

// on win32, the threadID is NOT the same as the threadHandle
uintptr_t			Sys_GetCurrentThreadID();

// returns a threadHandle
uintptr_t			Sys_CreateThread( xthread_t function, void * parms, xthreadPriority_t priority, 
									  const char *name, int core, int stackSize, 
									  bool suspended );

void				Sys_WaitForThread( uintptr_t threadHandle );
void				Sys_DestroyThread( uintptr_t threadHandle );
void				Sys_SetCurrentThreadName( const char * name );

uintptr_t			Sys_SetThreadAffinity( signalHandle_t h, int core );

void				Sys_SignalCreate( signalHandle_t * handle, bool manualReset );
void				Sys_SignalDestroy( signalHandle_t * handle );
void				Sys_SignalRaise( signalHandle_t * handle );
void				Sys_SignalClear( signalHandle_t * handle );
bool				Sys_SignalWait( signalHandle_t * handle, int timeout );

void				Sys_MutexCreate( mutexHandle_t * handle );
void				Sys_MutexDestroy( mutexHandle_t * handle );
bool				Sys_MutexLock( mutexHandle_t * handle, bool blocking );
void				Sys_MutexUnlock( mutexHandle_t * handle );

void				Sys_Yield();

#define MAX_CRITICAL_SECTIONS	4
#define WAIT_INFINITE			-1

#define false 0
#define true 1

enum {
	CRITICAL_SECTION_ZERO = 0,
	CRITICAL_SECTION_ONE,
	CRITICAL_SECTION_TWO,
	CRITICAL_SECTION_THREE
};

#endif // THREAD_ENABLE

#endif	// !__SYS_THREADING_H__
