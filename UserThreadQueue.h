// UserThreadQueue.h

/*
 * Name:    UserThreadQueue.h
 *
 * Purpose: Thread val queue .
 *
 */
#ifndef __USER_THREAD_QUEUE_H__12342534__
#define __USER_THREAD_QUEUE_H__12342534__

#ifdef __cplusplus
	extern "C"{
#endif

#include "global.h"		
		
int InitThreadQueue();
int UninitThreadQueue();
int ReadQueue_Sample(double outData[SAMPLE_CHANNEL][SAMPLE_RATE],int len[1]);
int ReadQueueAllData_Sample();
int WriteQueue_Sample();
int FlushQueue_Sample();

#ifdef __cplusplus
	}
#endif
#endif
