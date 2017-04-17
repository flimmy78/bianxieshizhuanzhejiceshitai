#include <windows.h>
#include "utility.h"
#include "UserThread.h"

int CVICALLBACK ThreadAcquire(void *functionData);
void UserThreadCreate(CmtThreadFunctionPtr threadFunction,ThreadControl *ctlData);
void UserThreadResume(int functionId);
void UserThreadWaitForALLQuit(ThreadControl ctl[],int len);

//ThreadControl g_ThreadControls[]={{ThreadAcquire},{ThreadAcquire}};// �̶߳��壬��һ��Ϊ�̵߳Ļص��������������Ϊ0
//	�ӿں���
//	�������
//  cmd�����������У����𣬻ָ����У��˳�
//  ctl���߳�����

void UserThread(THREAD_CMD cmd,ThreadControl *ctl)
{
	if(THREAD_CMD_CREATE_AND_RUN == cmd){ // ���������� 
		ctl->quit = 0;
		if(ctl->threadFunctionID ==0){ // �ú���û�б�����
			UserThreadCreate(ctl->threadFunction,ctl);
		}
	}else if(THREAD_CMD_SUSPEND == cmd){  // �����߳�
		ctl->suspend = 1;
	}else if(THREAD_CMD_RESUME == cmd){   // �ָ�����
		UserThreadResume(ctl->threadFunctionID);	
	}else if(THREAD_CMD_QUIT == cmd){	  // �˳��߳�
		ctl->quit = 1;
		CmtWaitForThreadPoolFunctionCompletion(DEFAULT_THREAD_POOL_HANDLE,ctl->threadFunctionID,0);
		CmtReleaseThreadPoolFunctionID(DEFAULT_THREAD_POOL_HANDLE,ctl->threadFunctionID);
		ctl->threadFunctionID = 0;
	}
}

void UserThread_Init()
{
	//UserThread(THREAD_CMD_CREATE_AND_RUN,&g_ThreadControls[0]);
}

void DeInitUserThread(ThreadControl ctl[],int len)	   // �˳�
{
	UserThreadWaitForALLQuit(ctl,len);
}

void UserThreadWaitForALLQuit(ThreadControl ctl[],int len)
{
#define id ctl[i].threadFunctionID		
	for(int i = 0;i<len;i++){
		if(id != 0){
			UserThread(THREAD_CMD_QUIT,&ctl[i]);
			//CmtWaitForThreadPoolFunctionCompletion (DEFAULT_THREAD_POOL_HANDLE, id, 0);
			//CmtReleaseThreadPoolFunctionID (DEFAULT_THREAD_POOL_HANDLE,id);
			//id = 0;
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

