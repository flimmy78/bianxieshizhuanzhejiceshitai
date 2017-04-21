#include <windows.h>
#include "utility.h"
#include "UserThread.h"

int CVICALLBACK ThreadAcquire(void *functionData);
void UserThreadCreate(CmtThreadFunctionPtr threadFunction,ThreadControl *ctlData);
void UserThreadResume(int functionId);
void UserThreadWaitForALLQuit(ThreadControl ctl[],int len);

//ThreadControl g_ThreadControls[]={{ThreadAcquire},{ThreadAcquire}};// 线程定义，第一项为线程的回掉函数，其他项均为0
//	接口函数
//	输入参数
//  cmd：创建并运行，挂起，恢复运行，退出
//  ctl：线程数据

void UserThread(THREAD_CMD cmd,ThreadControl *ctl)
{
	if(THREAD_CMD_CREATE_AND_RUN == cmd){ // 创建并运行 
		ctl->quit = 0;
		if(ctl->threadFunctionID != 0 ){ // 有使用同一个回掉函数的线程在运行。
			CmtReleaseThreadPoolFunctionID(DEFAULT_THREAD_POOL_HANDLE,ctl->threadFunctionID);
			ctl->threadFunctionID = 0;
		}			
		UserThreadCreate(ctl->threadFunction,ctl);
	}else if(THREAD_CMD_CREATE_AND_RUN_PARALLEL == cmd){ // 创建并运行。多个线程使用同一个回调函数。未测试，数据结构ctl->quit可能有问题	
		ctl->quit = 0;		
		UserThreadCreate(ctl->threadFunction,ctl);
	}else if(THREAD_CMD_SUSPEND == cmd){  // 挂起线程
		ctl->suspend = 1;
	}else if(THREAD_CMD_RESUME == cmd){   // 恢复运行
		UserThreadResume(ctl->threadFunctionID);	
	}else if(THREAD_CMD_QUIT == cmd){	  // 退出线程
		ctl->quit = 1;
	}else if(THREAD_CMD_QUIT_WAIT == cmd){	// 退出线程,并等待线程结束 
		CmtWaitForThreadPoolFunctionCompletion(DEFAULT_THREAD_POOL_HANDLE,ctl->threadFunctionID,0);
		CmtReleaseThreadPoolFunctionID(DEFAULT_THREAD_POOL_HANDLE,ctl->threadFunctionID);
		ctl->threadFunctionID = 0;		
	}
}

void UserThread_Init()
{
	//UserThread(THREAD_CMD_CREATE_AND_RUN,&g_ThreadControls[0]);
}

void UninitUserThread(ThreadControl ctl[],int len)	   // 退出
{
	UserThreadWaitForALLQuit(ctl,len);
	//CmtDiscardThreadPool(DEFAULT_THREAD_POOL_HANDLE);
}

void UserThreadWaitForALLQuit(ThreadControl ctl[],int len)
{
#define id ctl[i].threadFunctionID		
	for(int i = 0;i<len;i++){
		if(id != 0){
			UserThread(THREAD_CMD_QUIT,&ctl[i]);
			CmtWaitForThreadPoolFunctionCompletion (DEFAULT_THREAD_POOL_HANDLE, id, 0);
			CmtReleaseThreadPoolFunctionID (DEFAULT_THREAD_POOL_HANDLE,id);
			id = 0;
		}
	}
#undef id	
}

void UserThreadCreate(CmtThreadFunctionPtr threadFunction,ThreadControl *ctlData)
{
	CmtScheduleThreadPoolFunction (DEFAULT_THREAD_POOL_HANDLE, 
							   threadFunction, ctlData, &ctlData->threadFunctionID);
}

void UserThreadResume(int functionId)
{
	HANDLE threadHandle; 	
	CmtGetThreadPoolFunctionAttribute (DEFAULT_THREAD_POOL_HANDLE, functionId, ATTR_TP_FUNCTION_THREAD_HANDLE, 
									   &threadHandle);
	ResumeThread (threadHandle);			
}

