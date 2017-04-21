typedef enum{
	THREAD_CMD_CREATE_AND_RUN,  			// ���������С�һ���̣߳�һ��������
	THREAD_CMD_CREATE_AND_RUN_PARALLEL,  	// ���������С�����߳�һ��������
	THREAD_CMD_SUSPEND, // ����
	THREAD_CMD_RESUME,  // �ָ�����
	THREAD_CMD_QUIT,     // �˳��̣߳����ȴ�
	THREAD_CMD_QUIT_WAIT// �˳��߳�,�ȴ��߳̽���
}THREAD_CMD;

typedef struct{
	CmtThreadFunctionPtr threadFunction; // �̻߳ص�����	
	int threadFunctionID;   // �̺߳�����ID
	
	volatile int quit;		// ���Ʊ������˳��̡߳�0������1���˳�
	volatile int suspend;   // ���Ʊ����������̡߳�0������1������
}ThreadControl;

//	�ӿں���
//	�������
//  cmd�����������У����𣬻ָ����У��˳�
//  ctl���߳�����
void UserThread(THREAD_CMD cmd,ThreadControl *ctl) ;
void UninitUserThread();	   // �˳��߳�
