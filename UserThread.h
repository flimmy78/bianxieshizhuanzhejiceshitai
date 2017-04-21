typedef enum{
	THREAD_CMD_CREATE_AND_RUN,  			// 创建并运行。一个线程，一个函数。
	THREAD_CMD_CREATE_AND_RUN_PARALLEL,  	// 创建并运行。多个线程一个函数。
	THREAD_CMD_SUSPEND, // 挂起
	THREAD_CMD_RESUME,  // 恢复运行
	THREAD_CMD_QUIT,     // 退出线程，不等待
	THREAD_CMD_QUIT_WAIT// 退出线程,等待线程结束
}THREAD_CMD;

typedef struct{
	CmtThreadFunctionPtr threadFunction; // 线程回调函数	
	int threadFunctionID;   // 线程函数的ID
	
	volatile int quit;		// 控制变量：退出线程。0：不，1：退出
	volatile int suspend;   // 控制变量：挂起线程。0：不，1：挂起
}ThreadControl;

//	接口函数
//	输入参数
//  cmd：创建并运行，挂起，恢复运行，退出
//  ctl：线程数据
void UserThread(THREAD_CMD cmd,ThreadControl *ctl) ;
void UninitUserThread();	   // 退出线程
