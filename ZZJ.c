//==============================================================================
//
// Title:		ZZJ
// Purpose:		A short description of the application.
//
// Created on:	17/03/17 ���� at 9:51:15 by shuaiqi.
// Copyright:	. All Rights Reserved.
//
//==============================================================================

//==============================================================================
// Include files
#include <ansi_c.h>
#include <windows.h>
#include <analysis.h>
#include <cvirte.h>
#include <userint.h>
#include "ZZJ.h"
#include "Portable.h"
#include "toolbox.h"
#include "log.h"
#include "TimeMeasure.h"
#include "UserThread.h"

#if defined(PORTABLE) 
	#include <NIDAQmx.h> 
#else
	typedef void*              TaskHandle;
	typedef signed long        int32;
	typedef double             float64;
#endif

//#define SIMULATE_INPUT  //  ���ģ���������ݣ�ע�͵�������ʹ�òɼ���

//==============================================================================
// Constants

//==============================================================================
// Types
typedef enum
{
	WORK_MODEL_MANUAL,
	WORK_MODEL_AUTO,
} WORK_MODEL;

typedef enum
{
	RUN_MODEL_STOP,
	RUN_MODEL_RUNNING,
	RUN_MODEL_QUIT,
} RUN_MODEL;

typedef struct
{

	TaskHandle	task;
	int32		numSampsPerChan;
	int			startCtrl;
	int			stopCtrl;
	int         chartPanel;
	// ��ͼ�ؼ��Ͳ���
	int			chartCtrl;
	int 		scalingMode[2];
	double 		min[2],max[2];
	// ��ʱ��
	int			timerCtrl;
	RUN_MODEL   running;
	float64		timeout;
	DWORD		startTick;     // ������ʼʱ���tick
	DWORD       processedTick; // �Ѿ��������Tick
	WORK_MODEL  workModel;     // ����ģʽ���Զ��������ֶ�����
} StartReadWfmLoopStopInfo;
StartReadWfmLoopStopInfo g_info;



#define SAMPLE_RATE 2500	 		// ����Ƶ�� (Hz)
#define SAMPLE_BUF_READ_TIME 0.1	// ������������ȡ���ʱ�� (s)
#define SAMPLE_MAX_TIME 20      	// ������ʱ�䡣(s)
#define SAMPLE_CHANNEL 4         	// ͨ����
typedef enum
{
	CHANNEL_I,   		// ����ͨ��
	CHANNEL_V,   		// ��ѹͨ��
	CHANNEL_VREF,		// �ο�ͨ��
	CHANNEL_FORCE		// ��ͨ��
} CHANNEL;

typedef enum
{
	CHANNEL_TYPE_ALL_DATA,   // ͨ������������
	CHANNEL_TYPE_LAST_DATA   // ͨ���������ӵ�����
} CHANNEL_TYPE;

typedef struct
{
	double sampleData[SAMPLE_CHANNEL][SAMPLE_RATE];// ���δӲɼ�����ȡ��ԭʼ���ݡ�ÿͨ������Ϊ1��(SAMPLE_RATE)
	// ͨ����0��������1����ѹ��2����׼��3����
	int    sampleDataLen;// ���ζ�ȡ��ԭʼ���ݵĳ���
	
	double rawData[SAMPLE_CHANNEL][SAMPLE_RATE * SAMPLE_MAX_TIME];; // һ�β����Ĳɼ������ݡ�ʱ��Ӳ�����ʼ����������
	int    rawDataLen;   // һ�β����Ĳɼ������ݵĳ��ȡ����Ų��϶�ȡ�ɼ������ݣ��ó�����������

	double 	force[100 * SAMPLE_MAX_TIME];	// �����飬���ڻ��Ʋ���ͼ��ÿ��100��Ϊ���������仯��Ƶ�����ޡ�
	double 	forceIncrement;		 			// ���ʱ��
	int    	forceLen;  						// ���鳤��
	double* forceLastAddr;
	int    	forceLastLen;

	double 	I[50 * SAMPLE_MAX_TIME];// �������飬���ڻ��Ʋ���ͼ��50Ϊ��ƵƵ��50Hz��Ҳ�ǹ�Ƶ����Чֵ�������ϵ����Ƶ�ʡ�
	double 	IIncrement;
	int    	ILen;
	double* ILastAddr;
	int    	ILastLen;

	double 	V[50 * SAMPLE_MAX_TIME];// ��ѹ���飬���ڻ��Ʋ���ͼ��50Ϊ��ƵƵ��50Hz��Ҳ�ǹ�Ƶ����Чֵ�������ϵ����Ƶ�ʡ�
	double 	VIncrement;
	int    	VLen;
	double*	VLastAddr;
	int    	VLastLen;

	double 	VRef[SAMPLE_RATE * SAMPLE_MAX_TIME];	// ��ѹ��׼�������ɼ�����ԭʼ����
	double 	VRefIncrement;
	int    	VRefLen;
	double*	VRefLastAddr;
	int    	VRefLastLen;
} Waveform;
Waveform g_Waveform;
int g_ConvertedLen;

char g_FileName[MAX_PATHNAME_LEN];// �����ļ���
// ���ò��β���
typedef struct
{
	double *addr;
	double **lastAddr;
	int *len;
	int *lastLen;
	double *increment;

} ChannelDataPoint;


/*
SWITCH_MODEL // ת�޻��ͺ�
FORCE_SENSOR_TYPE // ������������
SWITCH_NUM //ת�޻����
TEST_ADDR  // ���Եص�
TEST_PERSON // ������
TEST_TIME  // ��������
*/
typedef struct
{
	char switchModel[50]; // ת�޻��ͺ�
	char forceSensorType[50]; // ������������
	char switchNum[50]; // ת�޻����
	char TestAddr[50]; // ���Եص�
	char TestPerson[50]; // ������
	time_t TestTime; // ��������
} UserData;
UserData g_UserData;



//==============================================================================
// Static global variables

static int panelMain = 0;   // ������:��������ʾ
static int panelMeasure = 0;// ���ý���:��ʼ��������
static int panelSystem = 0; // ϵͳ����
static int panelGraph  = 0; // ����ͼ����
static int panelPrint = 0;  // ��ӡ����
static int panelUserData = 0; // �����������ݽ���

static double initialX;     // ����ͼ��������ѹ��ʼx��λ�á�
static int initialVRef;     // ����ͼ"��ѹ��׼"��ʼλ��
static int initialForce;    // ����ͼ���ĳ�ʼλ��

//==============================================================================
// Static functions
#define DAQmxErrChk(functionCall) if( (DAQmxError=(functionCall))<0 ) goto Error; else
#define DAQmxNullChk(functionCall) if( (functionCall)==NULL ) { DAQmxError=DAQmxErrorPALMemoryFull; goto Error; } else
#define DAQmxReportErr(statusExpression) \
	{ \
		int32 __DAQmxError = (statusExpression); \
		if (DAQmxFailed(__DAQmxError) && !GetBreakOnLibraryErrors()) \
		{ \
			char gErrBuf[512]; \
			DAQmxGetErrorString(__DAQmxError,gErrBuf,512); \
			MessagePopup("DAQmx Error",gErrBuf); \
		} \
	}
//==============================================================================
// Global variables

//==============================================================================
// Global functions
int CVICALLBACK ThreadAcquire(void *functionData);
ThreadControl g_ThreadControls[]={{ThreadAcquire}};// �̶߳��壬��һ��Ϊ�̵߳Ļص��������������Ϊ0

int Acquisition(BOOL isStart);
void Measure(int isStart);
int DisplayCursorData(int panel,int control);  // ��ʾ��괦�����������Ȳ���
int EnableCursor(int panel, int control) ; 	   // ��ʾ/���ع��
void PlotData(CHANNEL_TYPE chanType,int panel,int graphCtrl);  // ����:��ɫ����ѹ����ɫ����ѹ��׼����ɫ��ת��������ɫ
int UpdateUserData(int panel,BOOL isSave);	// panel����Ҫ���µ���塣TRUE:���浽�ڴ����ݣ�FALSE:�ڴ����ݵ�����
int TBDisplayTime( void );                  // ������ʾ��ǰʱ��
int LoadALLData();

int WaveformClr(CHANNEL channel);

int CreatWaveformName(char *fileName)
{
	char strTime[50];
	time_t testTime = g_UserData.TestTime;
	strftime(strTime,sizeof(strTime),"%Y��%m��%d�� %Hʱ%M��%S��",localtime(&testTime));
	sprintf(fileName,"%s-%s-%s.vif",g_UserData.switchModel,g_UserData.switchNum,strTime);
	return 0;
}


void WaveformInit(void)
{
	WaveformClr(CHANNEL_I);		// ����ͨ��
	WaveformClr(CHANNEL_V);		// ��ѹͨ��
	WaveformClr(CHANNEL_VREF);  // �ο�ͨ��
	WaveformClr(CHANNEL_FORCE); // ��ͨ��

	g_Waveform.rawDataLen = 0;  // �������ݵĳ��ȹ��㡣
	g_ConvertedLen = 0;         // rawData�Ѿ�ת�����ĳ���
	g_Waveform.IIncrement = 0.02;     // 1/50Hz= 0.02s
	g_Waveform.VIncrement = 0.02;
	g_Waveform.VRefIncrement = 1.0/SAMPLE_RATE;
	g_Waveform.forceIncrement= 1.0/100;  // ÿ��100��Ϊ���������仯��Ƶ������
	initialX = 0;
	initialVRef = 0;
	initialForce = 0;
}
static int WaveformChannel2Addr(CHANNEL channel,ChannelDataPoint* pData)
{
	int ret=0;
	switch(channel)
	{
		case CHANNEL_I:
			pData->addr = g_Waveform.I;
			pData->lastAddr = &g_Waveform.ILastAddr;
			pData->len  = &g_Waveform.ILen;
			pData->lastLen = &g_Waveform.ILastLen;
			pData->increment = &g_Waveform.IIncrement;
			break;
		case CHANNEL_V:
			pData->addr = g_Waveform.V;
			pData->lastAddr = &g_Waveform.VLastAddr;
			pData->len  = &g_Waveform.VLen;
			pData->lastLen = &g_Waveform.VLastLen;
			pData->increment = &g_Waveform.VIncrement;
			break;
		case CHANNEL_VREF:
			pData->addr = g_Waveform.VRef;
			pData->lastAddr = &g_Waveform.VRefLastAddr;
			pData->len  = &g_Waveform.VRefLen;
			pData->lastLen = &g_Waveform.VRefLastLen;
			pData->increment = &g_Waveform.VRefIncrement;
			break;
		case CHANNEL_FORCE:
			pData->addr = g_Waveform.force;
			pData->lastAddr = &g_Waveform.forceLastAddr;
			pData->len  = &g_Waveform.forceLen;
			pData->lastLen = &g_Waveform.forceLastLen;
			pData->increment = &g_Waveform.forceIncrement;
			break;
#if 0			
		default:
			ret = -1;
			ERR1("Channel2Addr ��������channel:%d",channel);
			goto Error;
#endif			
			//break;
	}
Error:
	return ret;
}
int WaveformClr(CHANNEL channel)
{
	int ret=0;
	ChannelDataPoint chData;
	if(WaveformChannel2Addr(channel,&chData)<0)
	{
		ret = -1;
		ERR1("WaveformSet ��������channel:%d",channel);
		goto Error;
	}
	*chData.len = 0;
	*chData.increment = 0;
	*chData.lastAddr = 0;
	*chData.lastLen = 0;	
Error:
	return ret;
}


int WaveformSet(CHANNEL channel,double *data,int len,int x)// channel.
{
	int ret=0;
	ChannelDataPoint chData;
	if(WaveformChannel2Addr(channel,&chData)<0)
	{
		ret = -1;
		ERR1("WaveformSet ��������channel:%d",channel);
		goto Error;
	}
	memcpy(chData.addr+x,data,len*sizeof(double));
	*chData.len = len + x;
	*chData.lastLen = len;
	*chData.lastAddr = chData.addr+x;
	//*chData.increment = increment;

Error:
	return ret;
}
/*
�����
	data�����ε�ַ
	len	:���γ���
	x��������ʼ��(ʱ��)x����
	increment:����ÿ������֮��ļ��ʱ��
*/
int WaveformGet(CHANNEL channel,CHANNEL_TYPE channelType,double **data,int* len,double* x,double* increment)
{
	int ret=0;
	double xStart=0; // ������ʼ��/�����¿�ʼ��(ʱ��)x����
	ChannelDataPoint chData;

	if(WaveformChannel2Addr(channel,&chData)<0)
	{
		ret = -1;
		ERR1("WaveformGet ��������channel:%d",channel);
		goto Error;
	}
	switch(channelType)
	{
		case CHANNEL_TYPE_ALL_DATA:
			*data = chData.addr;
			*len  = *chData.len;
			break;
		case CHANNEL_TYPE_LAST_DATA:
			*data = *chData.lastAddr;
			*len  = *chData.lastLen;
			if((*len - *chData.len)!=0)
			{
				*data -=1;
				*len +=1;
				xStart = (*chData.lastAddr - chData.addr -1)*(*chData.increment);
			}
			else
			{
				xStart = 0;
			}
			break;
#if 0			
		default:
			ERR1("WaveformGet ��������channelType��%d",channelType);
			goto Error;
			break;
#endif			
	}
	//*x = (*chData.lastAddr - chData.addr)*(*chData.increment);
	if(increment!=NULL)
		*increment = *chData.increment;
	if(x != NULL)
		*x = xStart;
Error:
	return ret;
}
/////////////////////////////////////////////////////////
// UI�û�����
#if 0  // �������Сֵ����������
int UpdatePlot()
{
	// �ػ��ƣ����ղɼ��������ֵ�����������ᡣ
	SetCtrlAttribute(g_info.chartPanel, g_info.chartCtrl,ATTR_REFRESH_GRAPH,0);
	DeleteGraphPlot(panelGraph,GRAPH_GRAPH,-1,VAL_DELAYED_DRAW); // ��ջ�ͼ��
	//SetCtrlAttribute(g_info.chartPanel,g_info.chartCtrl,ATTR_ENABLE_ANTI_ALIASING,TRUE);
	SetAxisScalingMode(g_info.chartPanel,g_info.chartCtrl,VAL_LEFT_YAXIS,VAL_AUTOSCALE,0,0);
	SetAxisScalingMode(g_info.chartPanel,g_info.chartCtrl,VAL_RIGHT_YAXIS,VAL_AUTOSCALE,0,0);

	PlotData(CHANNEL_TYPE_ALL_DATA,g_info.chartPanel, g_info.chartCtrl);

	//SetCtrlAttribute(g_info.chartPanel,g_info.chartCtrl,ATTR_ENABLE_ANTI_ALIASING,FALSE);
	SetCtrlAttribute(g_info.chartPanel, g_info.chartCtrl,ATTR_REFRESH_GRAPH,1);
	return 0;
}
#else // �����ֵ����������
#if 0
int SetWaveformAxies(CHANNEL channel,int axis)
{
	double* waveform;
	double max,min,maxABS;
	int len,imax,imin;	
	WaveformGet(channel,CHANNEL_TYPE_ALL_DATA,&waveform,&len,NULL,NULL);	// ������
	if(len>0){
		MaxMin1D(waveform,len,&max,&imax,&min,&imin);
		max = fabs(max);
		min = fabs(min);
		maxABS = max>min?max:min; // ȡ����ֵ�����ֵ��
		maxABS = (int)maxABS +1;

		SetAxisScalingMode(g_info.chartPanel,g_info.chartCtrl,axis,VAL_MANUAL,-maxABS,maxABS);
	}
	return 0;
}
#else
int GetWavefromAxiesMax(CHANNEL channel,int *axiesMax)
{
	int isErr = 0;
	double* waveform;
	double max,min,maxABS;
	int len,imax,imin;	
	WaveformGet(channel,CHANNEL_TYPE_ALL_DATA,&waveform,&len,NULL,NULL);	// ������
	if(len>0){
		MaxMin1D(waveform,len,&max,&imax,&min,&imin);
		max = fabs(max);
		min = fabs(min);
		maxABS = max>min?max:min; // ȡ����ֵ�����ֵ��
		maxABS = (int)maxABS +1;
		*axiesMax = maxABS;
	}else{
		isErr = -1;
	}

	return isErr;
}

int CalculateWaveformAxies(CHANNEL channel1,CHANNEL channel2,double *Ch1AxiesMax,double *Ch2AxiesMax)
{
	int errno1 = 0;
	int axies[2];
	//int max,min;
	if((errno1 = GetWavefromAxiesMax(channel1,&axies[0]))!= 0)goto Error;
	if((errno1 = GetWavefromAxiesMax(channel2,&axies[1]))!= 0)goto Error;
#if 1
	double val;
	if(axies[0] > axies[1]){
		for(;;axies[1] ++)
		{
			val = axies[0]/axies[1];
			if(val == 1 || val == 2 || val == 5)
				break;
		}// ����������
	}else{
		for(;;axies[0] ++)
		{
			val = axies[1]/axies[0];
			if(val == 1 || val == 2 || val == 5)
				break;
		}// ����������
	}
#endif	
	
	*Ch1AxiesMax = axies[0];
	*Ch2AxiesMax = axies[1];
Error:
	return errno1;
}

void SetWaveformAxies()
{
	double Ch1AxiesMax,Ch2AxiesMax;
	if(CalculateWaveformAxies(CHANNEL_FORCE,CHANNEL_I,&Ch1AxiesMax,&Ch2AxiesMax)==0)
	{
		SetAxisScalingMode(g_info.chartPanel,g_info.chartCtrl,VAL_LEFT_YAXIS,VAL_MANUAL,-Ch1AxiesMax,Ch1AxiesMax);
		SetAxisScalingMode(g_info.chartPanel,g_info.chartCtrl,VAL_RIGHT_YAXIS,VAL_MANUAL,-Ch2AxiesMax,Ch2AxiesMax);
	}
}
#endif

int UpdatePlot()
{
	// �ػ��ƣ����ղɼ��������ֵ�����������ᡣ
	SetCtrlAttribute(g_info.chartPanel, g_info.chartCtrl,ATTR_REFRESH_GRAPH,0); // ��������������ڼ䣬��ֹ�������
	DeleteGraphPlot(panelGraph,GRAPH_GRAPH,-1,VAL_DELAYED_DRAW); // ��ջ�ͼ��
	//SetCtrlAttribute(g_info.chartPanel,g_info.chartCtrl,ATTR_ENABLE_ANTI_ALIASING,TRUE); // �򿪿����

	//SetWaveformAxies(CHANNEL_FORCE,VAL_LEFT_YAXIS);
	//SetWaveformAxies(CHANNEL_I,VAL_RIGHT_YAXIS);
	SetWaveformAxies();

	PlotData(CHANNEL_TYPE_ALL_DATA,g_info.chartPanel, g_info.chartCtrl);

	//SetCtrlAttribute(g_info.chartPanel,g_info.chartCtrl,ATTR_ENABLE_ANTI_ALIASING,FALSE);
	SetCtrlAttribute(g_info.chartPanel, g_info.chartCtrl,ATTR_REFRESH_GRAPH,1); // ���»���
	return 0;
}
#endif

int SetUIChange(int isStart)
{
	if(isStart)   // ��ʼ�ɼ�
	{
		// Example Core: Start the task.
		//DAQmxErrChk(DAQmxStartTask(g_info.task));
		//SetCtrlAttribute (panelMain, g_info.timerCtrl, ATTR_ENABLED, 1);
		DeleteGraphPlot(panelGraph,GRAPH_GRAPH,-1,VAL_DELAYED_DRAW);
		SetAxisScalingMode(g_info.chartPanel,g_info.chartCtrl,VAL_LEFT_YAXIS, g_info.scalingMode[0],g_info.min[0],g_info.max[0]);
		SetAxisScalingMode(g_info.chartPanel,g_info.chartCtrl,VAL_RIGHT_YAXIS,g_info.scalingMode[1],g_info.min[1],g_info.max[1]);
		
		SetCtrlVal(panelMain,MAIN_LED_RUNING,TRUE);
		SetCtrlAttribute(panelMain,MAIN_PIC_MEASURE,ATTR_LABEL_TEXT ,"ֹͣ�ɼ�");
		SetCtrlAttribute(panelMain,MAIN_PIC_SAVE,   ATTR_VISIBLE,FALSE);
		SetCtrlAttribute(panelMain,MAIN_LED_SAVE,   ATTR_VISIBLE,FALSE);
		SetCtrlAttribute(panelMain,MAIN_PIC_OPEN,   ATTR_VISIBLE,FALSE);
		SetCtrlAttribute(panelMain,MAIN_LED_OPEN,   ATTR_VISIBLE,FALSE);
		//SetCtrlAttribute(panelMain,MAIN_PIC_PRINT,  ATTR_VISIBLE,FALSE);
		//SetCtrlAttribute(panelMain,MAIN_LED_PRINT,  ATTR_VISIBLE,FALSE);


		//SetCtrlAttribute(panel,control,ATTR_PICT_BGCOLOR,0x185776);
		WaveformInit();
		UpdateUserData(panelMeasure,TRUE);

	}
	else  // ֹͣ�ɼ�
	{

		//SetCtrlAttribute (panelMain, g_info.timerCtrl, ATTR_ENABLED, 0);
		SetCtrlVal(panelMain,MAIN_LED_RUNING,FALSE);
		SetCtrlAttribute(panelMain,MAIN_PIC_MEASURE,ATTR_LABEL_TEXT ,"��ʼ�ɼ�");
		// �ر���壬ֹͣ����
		SetCtrlAttribute(panelMain,MAIN_PIC_SAVE,   ATTR_VISIBLE,TRUE);
		SetCtrlAttribute(panelMain,MAIN_LED_SAVE,   ATTR_VISIBLE,TRUE);
		SetCtrlAttribute(panelMain,MAIN_PIC_OPEN,   ATTR_VISIBLE,TRUE);
		SetCtrlAttribute(panelMain,MAIN_LED_OPEN,   ATTR_VISIBLE,TRUE);
		//SetCtrlAttribute(panelMain,MAIN_PIC_PRINT,  ATTR_VISIBLE,TRUE);
		//SetCtrlAttribute(panelMain,MAIN_LED_PRINT,  ATTR_VISIBLE,TRUE);

		UpdatePlot(); // �ػ��ƣ����ղɼ��������ֵ�����������ᡣ
		// ����ת�޻��������
		UpdateUserData(panelMeasure,FALSE);
	}
	return 0;
}
/////////////////////////////////////////////////////////
// ״̬��
typedef enum{
	FSM_STATE_IDEL,
	FSM_STATE_MEASURE,
	FSM_STATE_AUTO_MEASURE,
	FSM_STATE_QUIT
}FSM_STATE;

// �ź�
typedef enum{
	FSM_SIG_UI_MEASURE,// �û�����ɼ���ť	
	FSM_SIG_AUTO_MEASURE,
	FSM_SIG_START_MEASURE,
	FSM_SIG_STOP_MEASURE,
	FSM_SIG_SAVE,
	FSM_SIG_LOAD,
	FSM_SIG_PRINT,
	FSM_SIG_QUIT,
	FSM_SIG_LEN
}FSM_SIG_ID;

typedef struct{
	FSM_SIG_ID id;
	char describe[50];
}FSM_SIG;

typedef struct{
	volatile FSM_STATE	state;
}FSM_ID;


FSM_ID g_fsmID;
FSM_SIG g_fsmSig[] = {
	{FSM_SIG_UI_MEASURE,"�û�����ɼ���ť"},
	{FSM_SIG_AUTO_MEASURE,	"�Զ�����"},
	{FSM_SIG_START_MEASURE, "��ʼ����"},
	{FSM_SIG_STOP_MEASURE,	"ֹͣ����"},
	{FSM_SIG_SAVE,			"����"},
	{FSM_SIG_LOAD,			"��ȡ"},
	{FSM_SIG_PRINT,			"��ӡ"},
	{FSM_SIG_QUIT,			"�˳�"},
};
#define SetState(newState) fsmId->state = newState;

// SendFSMSig(g_fsmID,FSM_SIG_AUTO_MEASURE);
int GetFSMState(FSM_ID *fsmId,FSM_ID *fsm)
{
	*fsm = *fsmId;
	return 0;
}


void OnFSMStateQuitInFun()
{
	Measure(FALSE); // ֹͣ�ɼ�
	QuitUserInterface (0);
}

void OnFSMSig(FSM_ID *fsmId,FSM_SIG *sig)
{
	//FSM_ID fsm;
	//GetFSMState(fsmId,&fsm);
	switch(fsmId->state){
		case FSM_STATE_IDEL:
			if(sig->id == FSM_SIG_UI_MEASURE){
				DisplayPanel(panelMeasure);	//�û������л����ɼ��������
			}else if(sig->id == FSM_SIG_AUTO_MEASURE){  // �Զ�����
				SetUIChange(1);
				Acquisition(TRUE);
				SetCtrlAttribute(panelMain,MAIN_TIMER_MEASURE,ATTR_ENABLED,TRUE);
				SetState(FSM_STATE_AUTO_MEASURE);				
			}else if(sig->id == FSM_SIG_START_MEASURE){ // �ֶ�����
				SetUIChange(1);
				Acquisition(TRUE);
				Measure(TRUE);
				SetState(FSM_STATE_MEASURE);				
			}else if(sig->id == FSM_SIG_SAVE){  		// ����
				UpdateUserData(panelUserData,FALSE);
				DisplayPanel(panelUserData);				// ��ʾ�������
			}else if(sig->id == FSM_SIG_LOAD){  		// ��ѯ
				LoadALLData();
			}else if(sig->id == FSM_SIG_PRINT){  		// 
			}else if(sig->id == FSM_SIG_QUIT){  		// �˳�
				OnFSMStateQuitInFun();				
				SetState(FSM_STATE_QUIT);
			}			
			break;
		case FSM_STATE_MEASURE:	// ������		
			if(sig->id == FSM_SIG_UI_MEASURE){			// �û�ֹͣ����
				Measure(FALSE);				
				Acquisition(FALSE);
				SetUIChange(0);	
				SaveTimeMeasure("OnTime.txt");
				SetState(FSM_STATE_IDEL);
			}else if(sig->id == FSM_SIG_STOP_MEASURE){	// ����ʱ�䵽��ֹͣ
				Measure(FALSE);
				Acquisition(FALSE);
				SetUIChange(0);					
				SaveTimeMeasure("OnTime.txt");
				SetState(FSM_STATE_IDEL);
			}else if(sig->id == FSM_SIG_QUIT){			// �˳�����
				OnFSMStateQuitInFun();
				SetState(FSM_STATE_QUIT);				
			}
			break;			
		case FSM_STATE_AUTO_MEASURE:  // �Զ�����
			if(sig->id == FSM_SIG_UI_MEASURE){
				SetUIChange(0);
				Acquisition(FALSE);
				SetState(FSM_STATE_IDEL);				
			}else if(sig->id == FSM_SIG_START_MEASURE){
				Acquisition(TRUE);
				Measure(TRUE);
				SetState(FSM_STATE_MEASURE);				
			}else if(sig->id == FSM_SIG_QUIT){
				OnFSMStateQuitInFun();				
				SetState(FSM_STATE_QUIT);				
			}			
			break;			
		case FSM_STATE_QUIT:			
			// OnFSMStateQuitOutFun();
			break;
	}
}

int SendFSMSig(FSM_ID *fsmId,FSM_SIG_ID sigId)
{   
	FSM_SIG *sig = &g_fsmSig[sigId];
// lock
	OnFSMSig(fsmId,sig);
// unlock
	return 0;
}
int  ChartInit(TaskHandle task)
{
	int error = 0;
	//uInt32 numChannels;

	g_info.task = task;
	g_info.timerCtrl = MAIN_TIMER_MEASURE;
	g_info.numSampsPerChan = SAMPLE_RATE * SAMPLE_BUF_READ_TIME; // �ɼ�������ÿ�ζ�ȡ�ĳ��ȣ�����Ƶ��*��ȡ���ʱ��
	g_info.timeout = 10;
	g_info.chartPanel = panelGraph;
	g_info.chartCtrl = GRAPH_GRAPH;
	//errChk(DAQmxGetTaskAttribute (task, DAQmx_Task_NumChans, &numChannels));
	//SetCtrlAttribute (panelMain, MAIN_STRIPCHART, ATTR_NUM_TRACES, numChannels);
	GetAxisScalingMode(g_info.chartPanel,g_info.chartCtrl,VAL_LEFT_YAXIS ,&g_info.scalingMode[0],&g_info.min[0],&g_info.max[0]);
	GetAxisScalingMode(g_info.chartPanel,g_info.chartCtrl,VAL_RIGHT_YAXIS,&g_info.scalingMode[1],&g_info.min[1],&g_info.max[1]);

Error:
	return error;
}

void RunGUI(TaskHandle task)
{
	int error = 0;

	//errChk (panelHandle = LoadPanel (0, "ZZJ.uir", PANEL));
#if defined(PORTABLE)
	errChk (panelMain    = LoadPanel (0, "Portable.uir", MAIN));
#else	
	errChk (panelMain    = LoadPanel (0, "Portable.uir", MAIN_DESK));
    SetPanelAttribute (panelMain, ATTR_WINDOW_ZOOM, VAL_MAXIMIZE);	
	//SetPanelPos(panelMain,0,0);
#endif	
	errChk (panelMeasure = LoadPanel (panelMain	, "Portable.uir", MEASURE));
	errChk (panelSystem  = LoadPanel (panelMain	, "Portable.uir", SYSTEM));
	errChk (panelGraph   = LoadPanel (panelMain , "Portable.uir", GRAPH));
#if defined(PORTABLE)	
#else
	int attr;
	GetCtrlAttribute(panelMain,MAIN_DESK_DECORATION,ATTR_TOP,&attr);
	SetPanelAttribute(panelGraph,ATTR_TOP,attr);
	GetCtrlAttribute(panelMain,MAIN_DESK_DECORATION,ATTR_LEFT,&attr);
	SetPanelAttribute(panelGraph,ATTR_WIDTH,attr);
	GetCtrlAttribute(panelMain,MAIN_DESK_DECORATION,ATTR_HEIGHT,&attr);
	SetPanelAttribute(panelGraph,ATTR_HEIGHT,attr);
	
#endif	
	
	errChk (panelPrint   = LoadPanel (panelMain , "Portable.uir", P_PRINT));
	errChk (panelUserData = LoadPanel(panelMain , "Portable.uir", USER_DATA));

	errChk (ChartInit(task));

	/* display the panel and run the user interface */
	TBDisplayTime(); // ���½��棺��ǰʱ��

	errChk(EnableCursor(panelGraph, GRAPH_RB_FORCE));	 // ���½��棬�ù������汣��һ�¡����������
	errChk(EnableCursor(panelGraph, GRAPH_RB_CURRENT));  // ���½��棬�ù������汣��һ�¡����ص������
	errChk (DisplayPanel (panelMain));
	errChk (DisplayPanel (panelGraph));
	//QueueUserEvent(EVENT_COMMIT,panelGraph,GRAPH_RB_FORCE);
	errChk (RunUserInterface ());

Error:
	/* clean up */
	if (panelMain > 0)
		DiscardPanel (panelMain);
}


/// HIFN The main entry-point function.
#if defined(PORTABLE)
int main (int argc, char *argv[])
{
	int DAQmxError = 0;
	TaskHandle task;
	/* initialize and load resources */
	if(InitCVIRTE (0, argv, 0)==0)
		return -1;

	DAQmxErrChk(DAQmxLoadTask("RailwaySwitch",&task));

	RunGUI(task);

Error:
	if(DAQmxError!=0){
		MessagePopup("����","δ����Ӳ���������˳���");
	}
	DAQmxClearTask(task);		
	DeInitUserThread(g_ThreadControls,sizeof(g_ThreadControls)/sizeof(ThreadControl));	   // �˳��߳�
	return 0;
}
#else

int main (int argc, char *argv[])
{
	/* initialize and load resources */
	if(InitCVIRTE (0, argv, 0)==0)
		return -1;

	RunGUI(NULL);

Error:
	return 0;
}
#endif

//==============================================================================
// UI callback function prototypes

int CVICALLBACK OnExitPrograme (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_LEFT_CLICK:
		case EVENT_LEFT_DOUBLE_CLICK:
#if defined(PORTABLE) 			
			//g_info.running = RUN_MODEL_QUIT;
			SendFSMSig(&g_fsmID,FSM_SIG_QUIT);
#else
			QuitUserInterface (0);			
#endif
			
	}
	return 0;
}




int CVICALLBACK OnStartMeasure (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_LEFT_CLICK)
	{
		SendFSMSig(&g_fsmID,FSM_SIG_UI_MEASURE);
	}
	return 0;
}


#if defined(PORTABLE)


int Acquisition(BOOL isStart)
{
	int errNumber = 0;
	static BOOL isRunning = FALSE; // ��ֹ�������/ֹͣ

	if(isStart){ // ��ʼ�ɼ�
		if(isRunning != TRUE){
			isRunning = TRUE;
			//errNumber = DAQmxStartTask(g_info.task);		
			UserThread(THREAD_CMD_CREATE_AND_RUN,&g_ThreadControls[0]);
		}
	}else{		 // ֹͣ�ɼ�
		if(isRunning != FALSE){
			isRunning = FALSE;
			//errNumber = DAQmxStopTask(g_info.task);
			UserThread(THREAD_CMD_QUIT,&g_ThreadControls[0]);
		}
	}
	return errNumber;
}

// ��ʼ����
void Measure(int isStart)
{
	int DAQmxError = 0;
	if(isStart)   // ��ʼ�ɼ�
	{
		//g_info.running = RUN_MODEL_RUNNING;
		//DAQmxErrChk(DAQmxStopTask(g_info.task));
		//DAQmxErrChk(DAQmxStartTask(g_info.task));
		g_info.startTick = GetTickCount();
		// ������ʱ����������ʾ/�ɼ���
		SetCtrlAttribute(panelMain,MAIN_TIMER_MEASURE,ATTR_ENABLED,TRUE);
	}
	else  // ֹͣ�ɼ�
	{
		//g_info.running = RUN_MODEL_STOP;
		//g_info.workModel = WORK_MODEL_MANUAL;
		g_info.processedTick = 0;
		//DAQmxErrChk(DAQmxStopTask(g_info.task));
		// ֹͣ��ʱ��(������ʾ/�ɼ�)
		SetCtrlAttribute(panelMain,MAIN_TIMER_MEASURE,ATTR_ENABLED,FALSE);		
	}
Error:
	DAQmxReportErr(DAQmxError);
	return;
}
#else
// ��ʼ����
void Measure(int isStart)
{
	return;
}
#endif


int UpdateUserData(int panel,BOOL isSave)
{
	char str[255];
	time_t testTime;

	if(panel == panelMeasure)
	{
		if(isSave)
		{
			// ת�޻��ͺ�
			GetCtrlVal(panel,MEASURE_SWITCH_MODEL,str);
			strcpy(g_UserData.switchModel,str);
			// ������������
#if 0
			GetCtrlVal(panel,USER_DATA_FORCE_SENSOR_TYPE,str);
#else
			if(strcmp(g_UserData.switchModel,"S700K-C"))
			{
				strcpy(str,"��25mm");
			}
			else
			{
				strcpy(str,"��22mm");
			}
#endif
			strcpy(g_UserData.forceSensorType,str);
			// ��������
			time(&testTime);
			g_UserData.TestTime	= testTime;
		}
		else
		{
			// ת�޻��ͺ�
			strcpy(str,g_UserData.switchModel);
			SetCtrlVal(panel,MEASURE_SWITCH_MODEL,str);
		}
	}
	else if(panel == panelUserData)
	{
		if(isSave)
		{
			// �ļ���
			GetCtrlVal(panel,USER_DATA_FILENAME,str);
			strcpy(g_FileName,str);
			
			// ת�޻��ͺ�
			GetCtrlVal(panel,USER_DATA_SWITCH_MODEL,str);
			strcpy(g_UserData.switchModel,str);

			// ת�޻����
			GetCtrlVal(panel,USER_DATA_SWITCH_NUM,str);
			strcpy(g_UserData.switchNum,str);
			// ���Եص�
			GetCtrlVal(panel,USER_DATA_TEST_ADDR,str);
			strcpy(g_UserData.TestAddr,str);
			// ������
			GetCtrlVal(panel,USER_DATA_TEST_PERSON,str);
			strcpy(g_UserData.TestPerson,str);

		}
		else    // ��ȡ���ݲ���ʾ�������
		{
			// �ļ���
			//strcpy(str,g_FileName);
			CreatWaveformName(str);
			SetCtrlVal(panel,USER_DATA_FILENAME,str);
			// ת�޻��ͺ�
			strcpy(str,g_UserData.switchModel);
			SetCtrlVal(panel,USER_DATA_SWITCH_MODEL,str);

			// ������������
			strcpy(str,g_UserData.forceSensorType);
			SetCtrlVal(panel,USER_DATA_FORCE_SENSOR_TYPE,str);
			// ת�޻����
			strcpy(str,g_UserData.switchNum);
			SetCtrlVal(panel,USER_DATA_SWITCH_NUM,str);
			// ���Եص�
			strcpy(str,g_UserData.TestAddr);
			SetCtrlVal(panel,USER_DATA_TEST_ADDR,str);
			// ������
			strcpy(str,g_UserData.TestPerson);
			SetCtrlVal(panel,USER_DATA_TEST_PERSON,str);
			// ��������
			testTime = g_UserData.TestTime;
			strftime(str,sizeof(str),"%Y��%m��%d�� %H:%M:%S",localtime(&testTime));
			SetCtrlVal (panel, USER_DATA_TEST_TIME, str);
		}
	}
	else
	{
		ERR1("UpdateUserData����panel����panel:%d.",panel);
	}
	return 0;
}

// �����������ã�ȷ�Ͽ�ʼ������ť
int CVICALLBACK OnMeasureStart (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_LEFT_CLICK)
	{
		// ת�޻��û�����
		if(control == MEASURE_PIC_AUTO_MEASURE)  // �Զ�����
		{
			//g_info.workModel = WORK_MODEL_AUTO;
			SendFSMSig(&g_fsmID,FSM_SIG_AUTO_MEASURE);
		}
		else    // �ֶ�����
		{
			//Measure(TRUE);
			SendFSMSig(&g_fsmID,FSM_SIG_START_MEASURE);
		}
		HidePanel(panel);
	}

	return 0;
}

int CVICALLBACK OnSystemSet (int panel, int control, int event,
							 void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_LEFT_CLICK)
	{
		HidePanel(panel);
	}
	return 0;
}

int CVICALLBACK OnSystem (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	if(event == EVENT_LEFT_CLICK)
	{
		DisplayPanel(panelSystem);
	}
	return 0;
}

int RMSWin(double sampleData[],ssize_t len,double *val)
{
	double rms = 0;
	double data[len];
	
	memcpy(data,sampleData,len * sizeof(double));
	HanWin(data,len);
	RMS(data,len,&rms);// ��Чֵ
	*val = rms/sqrt(3.0/8.0);
	return 0;
}

int MedianWin(double sampleData[],ssize_t len,double *val)
{
	double median = 0;
	double data[len];
	
	memcpy(data,sampleData,len * sizeof(double));
	HanWin(data,len);
	Median(data,len,&median);// ��ֵ
	*val = 2*median;
	return 0;
}

int MeterDisplay()
{
	double *sampleData;
	double val;
	
	// ��ʾ��ѹ(��Чֵ)������(��Чֵ)����
	ssize_t dataLen = g_info.numSampsPerChan;
	// ����	
	sampleData = g_Waveform.sampleData[CHANNEL_I];
	RMSWin(sampleData,dataLen,&val);
	//SetCtrlVal(panelMain,MAIN_NUM_I,val);
	SetCtrlAttribute(panelMain,MAIN_NUM_I,ATTR_CTRL_VAL,val);
	
	// ��ѹ
	sampleData = g_Waveform.sampleData[CHANNEL_V];
	RMSWin(sampleData,dataLen,&val);// ��ѹ
	//SetCtrlVal(panelMain,MAIN_NUM_V,val);
	SetCtrlAttribute(panelMain,MAIN_NUM_V,ATTR_CTRL_VAL,val);

	// ��ѹ��׼
	sampleData = g_Waveform.sampleData[CHANNEL_VREF];
	MedianWin(sampleData,dataLen,&val);// ��ѹ��׼�ľ�ֵ��(��ѹ��׼Ϊֱ����ѹ����ֵ��������׼ȷ��)
	//SetCtrlVal(panelMain,MAIN_NUM_VREF,val);
	SetCtrlAttribute(panelMain,MAIN_NUM_VREF,ATTR_CTRL_VAL,val);

	// ��
	sampleData = g_Waveform.sampleData[CHANNEL_FORCE];
	MedianWin(sampleData,dataLen,&val);
	//SetCtrlVal(panelMain,MAIN_NUM_FORCE,val);
	SetCtrlAttribute(panelMain,MAIN_NUM_FORCE,ATTR_CTRL_VAL,val);

	return 0;
}


double MaxDoubleArray(double *array,int len)
{
	double max;
	max = array[0];
	for(int i=0; i<len; i++)
	{
		if(max<array[i])
		{
			max = array[i];
		}
	}
	return max;
}

int isAutoStart(double sampleData[SAMPLE_CHANNEL][SAMPLE_RATE],int len)
{
	int ret = 0;
	double *waveform;

	//WaveformGet(CHANNEL_I,CHANNEL_TYPE_LAST_DATA,&waveform,&len,&x,&increment);	// ����
	waveform = &sampleData[CHANNEL_I][0];
	if(len!=0)  // �в���
	{
		if(MaxDoubleArray(waveform,len) > 1.0 ) // �ɼ�����ѹ�����Ǵ������������ѹ��
		{
			ret = 1;
		}
		else
		{
			//WaveformGet(CHANNEL_V,CHANNEL_TYPE_LAST_DATA,&waveform,&len,&x,&increment);	// ��ѹ
			waveform = &sampleData[CHANNEL_V][0];
			if(MaxDoubleArray(waveform,len) > 1.0 )  // �ɼ�����ѹ�������Ǵ������������ѹ��
			{
				ret = 1;
			}
		}
	}

	return ret;
}
///////////////////////////////////////////////////////////////////////////////
// ���ܣ���������ת��Ϊ�������ݣ������벨�����顣��ѹ����ͨ����������Чֵ�任��
// ���룺sampleData
// �����WaveformSet
///////////////////////////////////////////////////////////////////////////////

ssize_t g_ConvertedLen = 0; // �Ѿ�ת�����ĳ��ȡ�
int RawData2Waveform(double rawData[SAMPLE_CHANNEL][SAMPLE_RATE * SAMPLE_MAX_TIME],ssize_t len)
{

	int sampNumPerCycle = SAMPLE_RATE/50;  		// ��ѹ������ÿ���ڲ������� 2500Hz(����Ƶ��)/50Hz(��ƵƵ��)  = 50��
	int sampNumPerForceCycle = SAMPLE_RATE/100; // ת����ÿ���ڲ���������	2500Hz(����Ƶ��)/100Hz(����������ӦƵ��) = 25��

#if 1	
	if(len - g_ConvertedLen >= 5*sampNumPerCycle){ // ����5�����ڲ���ת����Чֵ
		ssize_t ToConvertLen = len - g_ConvertedLen-4*sampNumPerCycle;// ��Ҫת���Ĳ���������ԭʼ����-�Ѿ�ת���ĳ���-4������
		double waveformI [ToConvertLen/sampNumPerCycle]; // ת���Ĳ�����������Ϊ��������ÿ�������һ����Чֵ��
		double waveformV [ToConvertLen/sampNumPerCycle];
		double *waveformVref = NULL;
		double waveformFORCE [ToConvertLen/sampNumPerForceCycle];// ת���Ĳ�����������Ϊ��ֵ��
		
		double *waveform[SAMPLE_CHANNEL]={waveformI,waveformV,waveformVref,waveformFORCE};
		double *data[SAMPLE_CHANNEL];		
		
		data[CHANNEL_I] = &rawData[CHANNEL_I][0];// ����
		data[CHANNEL_V] = &rawData[CHANNEL_V][0];// ��ѹ
		data[CHANNEL_VREF] = &rawData[CHANNEL_VREF][0];// ��ѹ��׼
		data[CHANNEL_FORCE] = &rawData[CHANNEL_FORCE][0];// ��
		int index = g_ConvertedLen;
		int i=0;
		for(; index<len-4*sampNumPerCycle; index+=sampNumPerCycle,++i)
		{
			RMSWin(data[CHANNEL_I]+index,g_info.numSampsPerChan,&waveform[CHANNEL_I][i]);
			RMSWin(data[CHANNEL_V]+index,g_info.numSampsPerChan,&waveform[CHANNEL_V][i]);
#if 0			
			//Median(data[CHANNEL_FORCE]+    2*i*25,25,&waveform[CHANNEL_FORCE][2*i]  );	// ת����.ǰ25�㡣ÿ��100�Σ�ÿ��2500Hz/100Hz = 25��ƽ��ֵ
			//Median(data[CHANNEL_FORCE]+(2*i+1)*25,25,&waveform[CHANNEL_FORCE][2*i+1]);	// ת����.��25�㡣
#else
			Median(data[CHANNEL_FORCE]+index	,25,&waveform[CHANNEL_FORCE][2*i]  );	// ת����.ǰ25�㡣ÿ��100�Σ�ÿ��2500Hz/100Hz = 25��ƽ��ֵ
			Median(data[CHANNEL_FORCE]+index+25	,25,&waveform[CHANNEL_FORCE][2*i+1]);	// ת����.��25�㡣						
#endif
		}
		initialX = g_ConvertedLen/50;
		initialForce = g_ConvertedLen/25;
		initialVRef = g_ConvertedLen;
		WaveformSet(CHANNEL_I,&waveform[CHANNEL_I][0],i,initialX);
		WaveformSet(CHANNEL_V,&waveform[CHANNEL_V][0],i,initialX);
		WaveformSet(CHANNEL_VREF,&data[CHANNEL_VREF][0],ToConvertLen,initialVRef);
		WaveformSet(CHANNEL_FORCE,&waveform[CHANNEL_FORCE][0],2*i,initialForce);
		g_ConvertedLen = index;
	}
#endif
#if 0
	// ��ѹ��׼
	data[CHANNEL_VREF] = rawData[CHANNEL_VREF];
	WaveformSet(CHANNEL_VREF,data[CHANNEL_VREF],g_info.numSampsPerChan,initialVRef);
	initialVRef +=g_info.numSampsPerChan;

	// ת����
	
	data[CHANNEL_FORCE] = rawData[CHANNEL_FORCE];
	int numMedian = 10;//0.1s/(1/100Hz)
	for(int i=0; i<numMedian; ++i)
	{
		Median(data[CHANNEL_FORCE]+i*25,25,&waveform[CHANNEL_FORCE][i]);//ÿ��25���м�ֵ
	}
	WaveformSet(CHANNEL_FORCE,waveform[CHANNEL_FORCE],numMedian,initialForce);
	initialForce +=numMedian;
#endif
	return 0;
}



///////////////////////////////////////////////////////////////////////////////
// ���ܣ����β�������ת��Ϊ�������ݣ������벨�����顣��ѹ����ͨ����������Чֵ�任��
// ���룺sampleData
// �����WaveformSet
///////////////////////////////////////////////////////////////////////////////
int SampleData2Waveform(int isRuning,double sampleData[SAMPLE_CHANNEL][SAMPLE_RATE])
{

	int sampNumPerCycle =SAMPLE_RATE/50;  // ÿ���ڲ������� 2500Hz/50Hz=50��
	int numRMS = g_info.numSampsPerChan/sampNumPerCycle;	//5�Ρ�250/50 = 5
	static double waveform[SAMPLE_RATE/10];
	double* data;

	if(isRuning)  // ������
	{
	}
	else 		   // ֻ��ʾ��δ����������������
	{
		initialX = 0;
		initialVRef = 0;
		initialForce = 0;
	}

	// ����
#if 0	
	data = sampleData[CHANNEL_I];
	for(int i=0; i<numRMS; ++i)
	{
		RMS(data+i*sampNumPerCycle,sampNumPerCycle,&waveform[i]);
	}
	WaveformSet(CHANNEL_I,waveform,numRMS,initialX);
#else
	data = g_Waveform.sampleData[CHANNEL_I];
	RMSWin(data,g_info.numSampsPerChan,&waveform[0]);
	WaveformSet(CHANNEL_I,waveform,1,initialX/5.0);
#endif


	// ��ѹ
#if 1	
	data = sampleData[CHANNEL_V];
	for(int i=0; i<numRMS; ++i)
	{
		RMS(data+i*sampNumPerCycle,sampNumPerCycle,&waveform[i]);
	}

	WaveformSet(CHANNEL_V,waveform,numRMS,initialX);
	initialX +=  numRMS;
#else
	data = sampleData[CHANNEL_V]; 
	RMSWin(data,g_info.numSampsPerChan,&waveform[0]);
	WaveformSet(CHANNEL_V,waveform,1,initialX/5.0);	
#endif


	// ��ѹ��׼
	data = sampleData[CHANNEL_VREF];
	WaveformSet(CHANNEL_VREF,data,g_info.numSampsPerChan,initialVRef);
	initialVRef +=g_info.numSampsPerChan;

	// ת����
	data = sampleData[CHANNEL_FORCE];
	int numMedian = 10;//0.1s/(1/100Hz)
	for(int i=0; i<numMedian; ++i)
	{
		Median(data+i*25,25,&waveform[i]);//ÿ��25���м�ֵ
	}
	WaveformSet(CHANNEL_FORCE,waveform,numMedian,initialForce);
	initialForce +=numMedian;// ��ѹ�ο�

	return 0;
}
int SampleData2RawData(double sampleData[SAMPLE_CHANNEL][SAMPLE_RATE],int len)
{
	double *dataStart;
	for(int i=0;i<SAMPLE_CHANNEL;i++){
		dataStart = &g_Waveform.rawData[i][g_Waveform.rawDataLen];
		memcpy(dataStart,sampleData[i],len*sizeof(double));
	}
	g_Waveform.rawDataLen += len;
	return 0;
}

#if defined(PORTABLE) 
#if defined(SIMULATE_INPUT) // ���ģ����������
#define SIMULATE_FREQUENCY 20.0/2500 // ��һ��Ƶ�ʡ�����Ƶ��/����Ƶ�ʡ�50Hz/2500Hz
#define SIMULATE_PHASE 2.0  // ��ʼ��λ����λ�㡣360Ϊһ�ܡ�
int ReadMeasure(double sampleData[SAMPLE_CHANNEL][SAMPLE_RATE],int *length)
{
	int32 DAQmxError = DAQmxSuccess; 	
	float64 *data = NULL;
	uInt32 numChannels = 4;//4ͨ��
	uInt32 dataSize;
	double phase = SIMULATE_PHASE;


	dataSize = numChannels * g_info.numSampsPerChan;
	DAQmxNullChk(data = (float64 *)malloc (dataSize * sizeof(float64)));

	int len = g_info.numSampsPerChan;
	int channel=0;
	// ����
	SineWave(len,3.0*sqrt(2),SIMULATE_FREQUENCY	,&phase,data+len*channel);
	channel ++;
	// ��ѹ
	SineWave(len,1.0*sqrt(2),SIMULATE_FREQUENCY	,&phase,data+len*channel);
	channel ++;	
	// ��ѹ��׼ 2.5V
	Set1D(data+len*channel,len,2.5);
	channel ++;	
	// ��
	//SineWave(len,10,1.0/2500	,&phase,data+len*channel);
	Set1D(data+len*channel,len,5.0);

	for(channel=0; channel<numChannels; channel++)
	{
		memcpy(sampleData[channel],data+len*channel,len*sizeof(double));
	}

	*length = len;
Error:
	if (data)
		free (data);
	return 0;
}
#else   // ʹ��Ӳ���ɼ���



int ReadMeasure(double sampleData[SAMPLE_CHANNEL][SAMPLE_RATE],int *length)
{
	int32 DAQmxError = DAQmxSuccess;
	float64 *data = NULL;
	uInt32 numChannels, dataSize;



	// Example Core: Allocate a buffer of appropriate size.  If you know
	// the necessary buffer size at compile time you can declare an array
	// of fixed size on the stack instead of allocating one.

	DAQmxErrChk(DAQmxGetTaskAttribute (g_info.task, DAQmx_Task_NumChans, &numChannels));
	dataSize = numChannels * g_info.numSampsPerChan;
	DAQmxNullChk(data = (float64 *)malloc (dataSize * sizeof(float64)));

	// Example Core: Read the specified number of samples from each channel.

	DAQmxErrChk(DAQmxReadAnalogF64 (g_info.task, g_info.numSampsPerChan, g_info.timeout,
									DAQmx_Val_GroupByChannel, data, dataSize, NULL, 0));
#if 0
	for(int channel=0; channel<numChannels; channel++)
	{
		memcpy(g_Waveform.sampleData[channel],data,g_info.numSampsPerChan*sizeof(double));
	}
#else
	int len = g_info.numSampsPerChan;
	for(int channel=0; channel<numChannels; channel++)
	{
		memcpy(sampleData[channel],data+len*channel,len*sizeof(double));
	}
	*length = len;
	
#endif

Error:
	if (data)
		free (data);
	if (DAQmxFailed(DAQmxError))
	{
		DAQmxReportErr(DAQmxError);
		//StopCB_StartReadWfmLoopStop (g_info.chartPanel, info->stopCtrl, EVENT_COMMIT, callbackData, 0, 0);
	}
	return DAQmxError;
}
#endif
#endif


#if defined(PORTABLE)
int ReadMeasureAndToWaveform()
{

	int32 DAQmxError = DAQmxSuccess;
	float64 *data = NULL;
	uInt32 numChannels, dataSize;

	//double xIncrement = 0.02;
	double val;

	//static float64 values[10*1000*1000];

	// Example Core: Allocate a buffer of appropriate size.  If you know
	// the necessary buffer size at compile time you can declare an array
	// of fixed size on the stack instead of allocating one.

	DAQmxErrChk(DAQmxGetTaskAttribute (g_info.task, DAQmx_Task_NumChans, &numChannels));
	dataSize = numChannels * g_info.numSampsPerChan;
	DAQmxNullChk(data = (float64 *)malloc (dataSize * sizeof(float64)));

	// Example Core: Read the specified number of samples from each channel.
#if 0
	DAQmxErrChk(DAQmxReadAnalogF64 (g_info.task, g_info.numSampsPerChan, g_info.timeout,
									DAQmx_Val_GroupByScanNumber, data, dataSize, NULL, 0));
#else
	DAQmxErrChk(DAQmxReadAnalogF64 (g_info.task, g_info.numSampsPerChan, g_info.timeout,
									DAQmx_Val_GroupByChannel, data, dataSize, NULL, 0));
#endif
	//PlotStripChart (g_info.chartPanel, g_info.chartCtrl, data, dataSize, 0, 0, VAL_DOUBLE);
//int PlotWaveform (int panelHandle, int controlID, void *yArray, size_t numberOfPoints, int yDataType, double yGain, double yOffset, double initialX, double xIncrement, int plotStyle, int pointStyle, int lineStyle, int pointFrequency, int color);

	// ��ʾ��ѹ(��Чֵ)������(��Чֵ)����
	RMS(data,g_info.numSampsPerChan,&val);// ����
	SetCtrlVal(panelMain,MAIN_NUM_I,val);

	RMS(data+g_info.numSampsPerChan,g_info.numSampsPerChan,&val);// ��ѹ
	SetCtrlVal(panelMain,MAIN_NUM_V,val);

	RMS(data+g_info.numSampsPerChan*2,g_info.numSampsPerChan,&val);// ��ѹ��׼
	SetCtrlVal(panelMain,MAIN_NUM_VREF,val);

	RMS(data+g_info.numSampsPerChan*3,g_info.numSampsPerChan,&val);// ������
	SetCtrlVal(panelMain,MAIN_NUM_FORCE,val);

#if 0
	xIncrement =  = 1.0/3000;
	PlotWaveform (g_info.chartPanel, g_info.chartCtrl, data, g_info.numSampsPerChan, VAL_DOUBLE, 1.0f, 0.0f, initialX, xIncrement, VAL_THIN_LINE, VAL_SOLID_SQUARE, VAL_SOLID, 1, VAL_BLUE);
	PlotWaveform (g_info.chartPanel, g_info.chartCtrl, data+g_info.numSampsPerChan, g_info.numSampsPerChan, VAL_DOUBLE, 1.0f, 0.0f, initialX, xIncrement, VAL_THIN_LINE, VAL_SOLID_SQUARE, VAL_SOLID, 1, VAL_RED);
	//PlotWaveform (g_info.chartPanel, g_info.chartCtrl, data+g_info.numSampsPerChan*2, g_info.numSampsPerChan, VAL_DOUBLE, 1.0f, 0.0f, initialX, xIncrement, VAL_THIN_LINE, VAL_SOLID_SQUARE, VAL_SOLID, 1, VAL_WHITE);
	initialX +=  xIncrement*g_info.numSampsPerChan;
#else
	int sampNumPerCycle =SAMPLE_RATE/50;  // ÿ���ڲ������� 2500Hz/50Hz=50��
	int numRMS = g_info.numSampsPerChan/sampNumPerCycle;	//5�Ρ�250/50 = 5
	double waveform[SAMPLE_RATE/10];

	// ����
	for(int i=0; i<numRMS; ++i)
	{
		RMS(data+i*sampNumPerCycle,sampNumPerCycle,&waveform[i]);
	}

#if 0
	PlotWaveform (g_info.chartPanel, g_info.chartCtrl, waveform, numRMS, VAL_DOUBLE, 1.0f, 0.0f,
				  initialX, xIncrement, VAL_THIN_LINE, VAL_SOLID_SQUARE, VAL_SOLID, 1, VAL_WHITE);
#else
	WaveformSet(CHANNEL_I,waveform,numRMS,initialX);
#endif
	// ��ѹ
	float64 *pdata = data + g_info.numSampsPerChan;
	for(int i=0; i<numRMS; ++i)
	{
		RMS(pdata+i*sampNumPerCycle,sampNumPerCycle,&waveform[i]);
	}
#if 0
	PlotWaveform (g_info.chartPanel, g_info.chartCtrl, waveform, numRMS, VAL_DOUBLE, 1.0f, 0.0f,
				  initialX, xIncrement, VAL_THIN_LINE, VAL_SOLID_SQUARE, VAL_SOLID, 1, VAL_RED);
#else
	WaveformSet(CHANNEL_V,waveform,numRMS,initialX);

#endif

	// ��ѹ��׼
	pdata = data + g_info.numSampsPerChan*2;
	WaveformSet(CHANNEL_VREF,pdata,g_info.numSampsPerChan,initialVRef);
	initialVRef +=g_info.numSampsPerChan;// ��ѹ�ο�

	// ת����
	pdata = data + g_info.numSampsPerChan*3;//
	int numMedian = 10;//0.1s/(1/100Hz)
	for(int i=0; i<numMedian; ++i)
	{
		Median(pdata+i*25,25,&waveform[i]);//ÿ��25���м�ֵ
	}
	WaveformSet(CHANNEL_FORCE,waveform,numMedian,initialForce);
	initialForce +=numMedian;// ��ѹ�ο�


	initialX +=  numRMS;

	//initialX +=  xIncrement*numRMS;

#endif
Error:
	if (data)
		free (data);
	if (DAQmxFailed(DAQmxError))
	{
		DAQmxReportErr(DAQmxError);
		//StopCB_StartReadWfmLoopStop (g_info.chartPanel, info->stopCtrl, EVENT_COMMIT, callbackData, 0, 0);
	}
	return 0;
}
#endif 

void PlotData(CHANNEL_TYPE chanType,int panel,int graphCtrl)  // ����:��ɫ����ѹ����ɫ����ѹ��׼����ɫ��ת��������ɫ
{
	double* waveform;
	double x;
	int len;
	double increment;
	int plotHandle;
	
	TimeMeasure(4,TM_START);
	SetCtrlAttribute(panel, graphCtrl,ATTR_REFRESH_GRAPH,0);TimeMeasure(4,TM_STOP);TimeMeasure(5,TM_START);
	WaveformGet(CHANNEL_I,chanType,&waveform,&len,&x,&increment);	TimeMeasure(5,TM_STOP);// ���Ƶ�������ͼ
	if(len)
	{
				TimeMeasure(6,TM_START);
		plotHandle = PlotWaveform (panel, graphCtrl, waveform, len, VAL_DOUBLE, 1.0f, 0.0f,
								   x, increment, VAL_THIN_LINE, VAL_SOLID_SQUARE, VAL_SOLID, 1, VAL_RED);
				TimeMeasure(6,TM_STOP);  TimeMeasure(7,TM_START);
		if(chanType == CHANNEL_TYPE_ALL_DATA){
			SetPlotAttribute (panel, graphCtrl, plotHandle, ATTR_PLOT_YAXIS, VAL_RIGHT_YAXIS);
		}
		TimeMeasure(7,TM_STOP);
	}
#if 0
	WaveformGet(CHANNEL_V,chanType,&waveform,&len,&x,&increment);	// ���Ƶ�ѹ����ͼ
	if(len)
	{
		plotHandle = PlotWaveform (panel, graphCtrl, waveform, len, VAL_DOUBLE, 1.0f, 0.0f,
								   x, increment, VAL_THIN_LINE, VAL_SOLID_SQUARE, VAL_SOLID, 1, VAL_WHITE);
		SetPlotAttribute (panel, graphCtrl, plotHandle, ATTR_PLOT_YAXIS, VAL_RIGHT_YAXIS);
	}
#endif
	
#if 0
	WaveformGet(CHANNEL_VREF,chanType,&waveform,&len,&x,&increment);// ���Ƶ�ѹ��׼����ͼ
	if(len)
	{
		plotHandle = PlotWaveform (panel, graphCtrl, waveform, len, VAL_DOUBLE, 1.0f, 0.0f,
								   x, increment, VAL_THIN_LINE, VAL_SOLID_SQUARE, VAL_SOLID, 1, VAL_BLUE);
		SetPlotAttribute (panel, graphCtrl, plotHandle, ATTR_PLOT_YAXIS, VAL_RIGHT_YAXIS);
	}
#endif
	TimeMeasure(8,TM_START);
	WaveformGet(CHANNEL_FORCE,chanType,&waveform,&len,&x,&increment);// ����ת��������ͼ
	if(len)
	{
		PlotWaveform (panel, graphCtrl, waveform, len, VAL_DOUBLE, 1.0f, 0.0f,
					  x, increment, VAL_THIN_LINE, VAL_SOLID_SQUARE, VAL_SOLID, 1, VAL_GREEN);
	}
	TimeMeasure(8,TM_STOP);TimeMeasure(9,TM_START);
	SetCtrlAttribute(panel, graphCtrl,ATTR_REFRESH_GRAPH,1); TimeMeasure(9,TM_STOP);

}

#if defined(PORTABLE)
#define MDAASVer 3
#if MDAASVer == 0
int MeasureDisplayAndAutoStart()
{
	if(g_info.running == RUN_MODEL_STOP) // û���ڲ�����ֻ��ʾ��ǰֵ������������
	{
		if(g_info.workModel == WORK_MODEL_AUTO)
		{
			ReadMeasure(g_Waveform.sampleData,&g_Waveform.sampleDataLen);
			//SampleData2Waveform(FALSE,g_Waveform.sampleData);
			MeterDisplay();  // ���Ǳ������ʾ����ֵ
			if(isAutoStart(g_Waveform.sampleData,g_Waveform.sampleDataLen))
			{
				Measure(TRUE);
				SampleData2RawData(g_Waveform.sampleData,g_Waveform.sampleDataLen);
			}
			else
			{
			}
		}
	}
	else if(g_info.running == RUN_MODEL_RUNNING)     // ���ڲ���
	{
	
		int maxTime_ms = 15.1*1000;	   // ֻ����15���ڵ�����
		int curTick = GetTickCount();
		if(curTick > g_info.startTick + maxTime_ms)
			curTick = g_info.startTick + maxTime_ms;
		int n = (curTick - g_info.startTick - g_info.processedTick)/100;
		g_info.processedTick = ((curTick - g_info.startTick)/100)*100;

		for(int i=0; i<n; i++)
		{
			ReadMeasure(g_Waveform.sampleData,&g_Waveform.sampleDataLen);
			SampleData2RawData(g_Waveform.sampleData,g_Waveform.sampleDataLen);
		}
		RawData2Waveform(g_Waveform.rawData,g_Waveform.rawDataLen);
		MeterDisplay();
		PlotData(CHANNEL_TYPE_LAST_DATA,g_info.chartPanel, g_info.chartCtrl);		
		if(g_info.processedTick >= maxTime_ms) // ֻ����maxTime �����ڵ�����
		{
			Measure(FALSE); // ֹͣ�ɼ�
		}
		else
		{
		}
	}
	else if(g_info.running == RUN_MODEL_QUIT)    //quit
	{
		Measure(FALSE); // ֹͣ�ɼ�
		QuitUserInterface (0);
	}
	return 0;
}
#elif MDAASVer == 1
// ������ʱ���ۺϲ���/��ʾ������
int ElapseTimeToRunN(double maxTime,int *n)
{
	int maxTime_ms = 15.1*1000;	   // ֻ����15���ڵ�����
	int curTick = GetTickCount();
	if(curTick > g_info.startTick + maxTime_ms)
		curTick = g_info.startTick + maxTime_ms;
	g_info.processedTick = ((curTick - g_info.startTick)/100)*100;		
	*n = (curTick - g_info.startTick - g_info.processedTick)/100;
	return 0;
}

int MeasureDisplayAndAutoStart()
{
	if(g_info.running == RUN_MODEL_STOP) // û���ڲ�����ֻ��ʾ��ǰֵ������������
	{
		if(g_info.workModel == WORK_MODEL_AUTO)
		{
			ReadMeasure(g_Waveform.sampleData,&g_Waveform.sampleDataLen);
			//SampleData2Waveform(FALSE,g_Waveform.sampleData);
			MeterDisplay();  // ���Ǳ������ʾ����ֵ
			if(isAutoStart(g_Waveform.sampleData,g_Waveform.sampleDataLen))
			{
				Measure(TRUE);
				SampleData2RawData(g_Waveform.sampleData,g_Waveform.sampleDataLen);
			}
			else
			{
			}
		}
	}
	else if(g_info.running == RUN_MODEL_RUNNING)     // ���ڲ���
	{
#if 1	
		int maxTime_ms = 15.1*1000;	   // ֻ����15���ڵ�����
		int curTick = GetTickCount();
		if(curTick > g_info.startTick + maxTime_ms)
			curTick = g_info.startTick + maxTime_ms;
		int n = (curTick - g_info.startTick - g_info.processedTick)/100;
		g_info.processedTick = ((curTick - g_info.startTick)/100)*100;
#else
		int n;
		ElapseTimeToRunN(15.1,&n);
#endif
		for(int i=0; i<n; i++)
		{
			ReadMeasure(g_Waveform.sampleData,&g_Waveform.sampleDataLen);
			SampleData2RawData(g_Waveform.sampleData,g_Waveform.sampleDataLen);
		}
		RawData2Waveform(g_Waveform.rawData,g_Waveform.rawDataLen);
		MeterDisplay();
		PlotData(CHANNEL_TYPE_LAST_DATA,g_info.chartPanel, g_info.chartCtrl);		
		if(g_info.processedTick >= maxTime_ms) // ֻ����maxTime �����ڵ�����
		{
			Measure(FALSE); // ֹͣ�ɼ�
		}
		else
		{
		}
	}
	else if(g_info.running == RUN_MODEL_QUIT)    //quit
	{
		Measure(FALSE); // ֹͣ�ɼ�
		QuitUserInterface (0);
	}
	return 0;
}
#elif MDAASVer == 2
// ������ʱ���ۺϲ���/��ʾ������
int ElapseTimeToRunN(double maxTime_s,int *n)
{
	int maxTime_ms = maxTime_s*1000;	   // ��ת��Ϊ���롣ֻ����maxTime_s���ڵ�����
	int curTick = GetTickCount();
	if(curTick > g_info.startTick + maxTime_ms)
		curTick = g_info.startTick + maxTime_ms;
	*n = (curTick - g_info.startTick - g_info.processedTick)/100;
	g_info.processedTick = ((curTick - g_info.startTick)/100)*100;		
	return 0;
}
int time_start_ms;
int time_stop_ms;
int time_elapse_ms;
int MeasureDisplayAndAutoStart()
{
	FSM_ID fsm;
	GetFSMState(&g_fsmID,&fsm);	
	switch(fsm.state){
		case FSM_STATE_MEASURE:	// ������		
			double maxTime_s = 15.1;	   // ֻ����15���ڵ�����
			int n;
			ElapseTimeToRunN(maxTime_s,&n);
			if(n>0){ 
				TimeMeasure(0,TM_START);
				for(int i=0; i<n; i++)
				{
					ReadMeasure(g_Waveform.sampleData,&g_Waveform.sampleDataLen);
					SampleData2RawData(g_Waveform.sampleData,g_Waveform.sampleDataLen);
				}
				TimeMeasure(0,TM_STOP);TimeMeasure(1,TM_START);
				MeterDisplay();TimeMeasure(1,TM_STOP);TimeMeasure(2,TM_START);
				RawData2Waveform(g_Waveform.rawData,g_Waveform.rawDataLen);TimeMeasure(2,TM_STOP);TimeMeasure(3,TM_START);
				PlotData(CHANNEL_TYPE_LAST_DATA,g_info.chartPanel, g_info.chartCtrl);TimeMeasure(3,TM_STOP);
			}
			if(g_info.processedTick >= maxTime_s*1000) // ֻ����maxTime ���ڵ�����
			{
				SendFSMSig(&g_fsmID,FSM_SIG_STOP_MEASURE); // ֹͣ�ɼ�  
			}			
			break;			
		case FSM_STATE_AUTO_MEASURE:  // �ȴ������źţ��Զ�������ֻ��ʾ��ǰֵ������������
			ReadMeasure(g_Waveform.sampleData,&g_Waveform.sampleDataLen);
			MeterDisplay();  // ���Ǳ������ʾ����ֵ
			if(isAutoStart(g_Waveform.sampleData,g_Waveform.sampleDataLen))
			{
				//Measure(TRUE);
				SampleData2RawData(g_Waveform.sampleData,g_Waveform.sampleDataLen);
				SendFSMSig(&g_fsmID,FSM_SIG_START_MEASURE);
			}
			break;			
		default :
			break;
	}	
	return 0;
}
#elif MDAASVer == 3
// ������ʱ���ۺϲ���/��ʾ������
int ElapseTimeToRunN(double maxTime_s,int *n)
{
	int maxTime_ms = maxTime_s*1000;	   // ��ת��Ϊ���롣ֻ����maxTime_s���ڵ�����
	int curTick = GetTickCount();
	if(curTick > g_info.startTick + maxTime_ms)
		curTick = g_info.startTick + maxTime_ms;
	*n = (curTick - g_info.startTick - g_info.processedTick)/100;
	g_info.processedTick = ((curTick - g_info.startTick)/100)*100;		
	return 0;
}
int time_start_ms;
int time_stop_ms;
int time_elapse_ms;
int MeasureDisplayAndAutoStart()
{
	FSM_ID fsm;
	GetFSMState(&g_fsmID,&fsm);	
	switch(fsm.state){
		case FSM_STATE_MEASURE:	// ������		
			double maxTime_s = 15.1;	   // ֻ����15���ڵ�����
			int n;
			ElapseTimeToRunN(maxTime_s,&n);
			if(n>0){ 
				TimeMeasure(0,TM_START);
#if 0				
				for(int i=0; i<n; i++)
				{
					ReadMeasure(g_Waveform.sampleData,&g_Waveform.sampleDataLen);
					SampleData2RawData(g_Waveform.sampleData,g_Waveform.sampleDataLen);
				}
#endif				
				TimeMeasure(0,TM_STOP);TimeMeasure(1,TM_START);
				MeterDisplay();TimeMeasure(1,TM_STOP);TimeMeasure(2,TM_START);
				RawData2Waveform(g_Waveform.rawData,g_Waveform.rawDataLen);TimeMeasure(2,TM_STOP);TimeMeasure(3,TM_START);
				PlotData(CHANNEL_TYPE_LAST_DATA,g_info.chartPanel, g_info.chartCtrl);TimeMeasure(3,TM_STOP);
			}
			if(g_info.processedTick >= maxTime_s*1000) // ֻ����maxTime ���ڵ�����
			{
				SendFSMSig(&g_fsmID,FSM_SIG_STOP_MEASURE); // ֹͣ�ɼ�  
			}			
			break;			
		case FSM_STATE_AUTO_MEASURE:  // �ȴ������źţ��Զ�������ֻ��ʾ��ǰֵ������������
#if 0			
			ReadMeasure(g_Waveform.sampleData,&g_Waveform.sampleDataLen);
			MeterDisplay();  // ���Ǳ������ʾ����ֵ
			if(isAutoStart(g_Waveform.sampleData,g_Waveform.sampleDataLen))
			{
				//Measure(TRUE);
				SampleData2RawData(g_Waveform.sampleData,g_Waveform.sampleDataLen);
				SendFSMSig(&g_fsmID,FSM_SIG_START_MEASURE);
			}
#else
			MeterDisplay();  // ���Ǳ������ʾ����ֵ
#endif
			break;			
		default :
			break;
	}	
	return 0;
}
#endif
#endif 



int CVICALLBACK OnTimer_Measure (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_TIMER_TICK:
			#if defined(PORTABLE)  
			MeasureDisplayAndAutoStart();
			#endif 
			break;
	}
	return 0;
}

#define TIME_DISPLAY_VER   3
#if TIME_DISPLAY_VER == 1
int TBDisplayTime( void )
{
	SYSTEMTIME st;
	char  acData[ 32 ];
	GetLocalTime (&st);
	//sprintf (acData, "%04d.%02d.%02d", st.wYear, st.wMonth, st.wDay);
	//SetCtrlVal (g_plMain, MAIN_TM_DATE, acData);

	sprintf (acData, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond );
	SetCtrlVal (panelMain, MAIN_TM_TIME, acData);
	return 0;
}
#elif TIME_DISPLAY_VER == 2
int TBDisplayTime( void )
{
	time_t rawTime;
	struct tm *timeinfo;
	char  acData[ 100 ];
	time(&rawTime);
	timeinfo = localtime(&rawTime);
#if 0
	sprintf (acData, "%02d:%02d:%02d", timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec );
#else
	strftime(acData,sizeof(acData),"%Y-%m-%d %H:%M:%S",timeinfo);
#endif
	SetCtrlVal (panelMain, MAIN_TM_TIME, acData);
	return 0;
}
#else

int TBDisplayTime( void )
{
	time_t rawTime;
	struct tm *timeinfo;
	char  acData[ 50 ];
	time(&rawTime);
	timeinfo = localtime(&rawTime);

	strftime(acData,sizeof(acData),"%H:%M:%S",timeinfo);
#ifdef PORTABLE
	SetCtrlVal (panelMain, MAIN_TM_TIME, acData);
#else
	SetCtrlVal (panelMain, MAIN_DESK_TM_TIME, acData);
#endif	
	return 0;
}
#endif


int CVICALLBACK OnTimer_Watch (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_TIMER_TICK:
			TBDisplayTime();
			break;
	}
	return 0;
}
#if 0

int DisplayCursorData(int panel,int control)
{
	int error;
	int activeCursor;
	double cursorX,cursorY;
	char str[20];

	errChk(GetActiveGraphCursor (panel,control,&activeCursor));
	errChk(GetGraphCursor(panel,control,activeCursor,&cursorX,&cursorY));
	if(activeCursor==1)
	{
		sprintf(str,"%.1f(KN)",cursorY);
		SetCtrlVal(panel,GRAPH_FORCE,str);
		sprintf(str,"%.1f(s)",cursorX);
		SetCtrlVal(panel,GRAPH_TIME_FORCE ,str);
	}
	else
	{
		sprintf(str,"%.1f(A)",cursorY);
		SetCtrlVal(panel,GRAPH_CURRENT,str);
		sprintf(str,"%.1f(s)",cursorX);
		SetCtrlVal(panel,GRAPH_TIME_CURRENT ,str);
	}
Error:
	return error;
}
#else
int DisplayCursorData(int panel,int control)
{
	int error;
	int activeCursor;
	double cursorX,cursorY;
	char str[20];

	errChk(GetActiveGraphCursor (panel,control,&activeCursor));
	errChk(GetGraphCursor(panel,control,activeCursor,&cursorX,&cursorY));
	if(activeCursor==1)
	{
		sprintf(str,"%.1f",cursorY);
		SetCtrlVal(panel,GRAPH_FORCE,str);
		sprintf(str,"%.1f",cursorX);
		SetCtrlVal(panel,GRAPH_TIME_FORCE ,str);
	}
	else
	{
		sprintf(str,"%.1f",cursorY);
		SetCtrlVal(panel,GRAPH_CURRENT,str);
		sprintf(str,"%.1f",cursorX);
		SetCtrlVal(panel,GRAPH_TIME_CURRENT ,str);
	}
Error:
	return error;
}
#endif
int CVICALLBACK OnGraph (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		case EVENT_VAL_CHANGED:
			DisplayCursorData(panel,control);
			break;
	}
	return 0;
}

int EnableCursor(int panel, int control)
{
	int CursorNumber;
	int val;
	GetCtrlVal(panel,control,&val);
	if(control == GRAPH_RB_FORCE)
	{
		CursorNumber = 1;
	}
	else
	{
		CursorNumber = 2;
	}
	if(val) // �򿪹��
	{
		SetCursorAttribute(panel,GRAPH_GRAPH,CursorNumber,ATTR_CURSOR_ENABLED,val);
		SetCursorAttribute(panel,GRAPH_GRAPH,CursorNumber,ATTR_CROSS_HAIR_STYLE,VAL_LONG_CROSS);
		SetCursorAttribute(panel,GRAPH_GRAPH,CursorNumber,ATTR_CURSOR_POINT_STYLE,VAL_SOLID_SQUARE);
	}
	else    // �رչ��
	{
		SetCursorAttribute(panel,GRAPH_GRAPH,CursorNumber,ATTR_CURSOR_ENABLED,val);
		SetCursorAttribute(panel,GRAPH_GRAPH,CursorNumber,ATTR_CROSS_HAIR_STYLE,VAL_NO_CROSS);
		SetCursorAttribute(panel,GRAPH_GRAPH,CursorNumber,ATTR_CURSOR_POINT_STYLE,VAL_NO_POINT);
	}

	//������ʾ
	//DisplayCursorData(panel,GRAPH_GRAPH);
	return 0;
}
int CVICALLBACK OnCursor (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			EnableCursor(panel,control);
			break;
	}
	return 0;
}


#if 1
// ��ȡ���ݲ���ʾ�ڴ�ӡ��������
// ���룺
// 		data:�û�����
//		panel�������
// �����
// 		��
// ���أ�
// 		0���ɹ�
int PrintUserData(UserData *data,int panel)
{
	char str[50];
	time_t testTime;


	// ȡ�����ؼ��ĵ�ǰѡ��
	//SetTableSelection (panel, P_PRINT_TABLE, VAL_EMPTY_RECT );
	// ȡ�����ؼ�����ɫ��
	//SetActiveTableCell (panel, P_PRINT_TABLE, MakePoint (0, 0));

	SetCtrlAttribute(panel,P_PRINT_TABLE,ATTR_HIDE_HILITE,FALSE);
	// ת�޻��ͺ�
	sprintf(str," %s",data->switchModel);
	//SetCtrlVal(panel,MEASURE_SWITCH_MODEL,str);
	SetTableCellVal (panel, P_PRINT_TABLE, MakePoint (2, 1), str);

	// ������������
	sprintf(str," %s",data->forceSensorType);
	//SetCtrlVal(panel,MEASURE_FORCE_SENSOR_TYPE,str);
	SetTableCellVal (panel, P_PRINT_TABLE, MakePoint (2, 3), str);
	// ת�޻����
	sprintf(str," %s",data->switchNum);
	//SetCtrlVal(panel,MEASURE_SWITCH_NUM,str);
	SetTableCellVal (panel, P_PRINT_TABLE, MakePoint (2, 2), str);
	// ���Եص�
	sprintf(str," %s",data->TestAddr);
	//SetCtrlVal(panel,MEASURE_TEST_ADDR,str);
	SetTableCellVal (panel, P_PRINT_TABLE, MakePoint (2, 5), str);
	// ������
	sprintf(str," %s",data->TestPerson);
	//SetCtrlVal(panel,MEASURE_TEST_PERSON,str);
	SetTableCellVal (panel, P_PRINT_TABLE, MakePoint (2, 6), str);
	// ��������
	testTime = data->TestTime;
	strftime(str,sizeof(str)," %Y��%m��%d�� %H:%M:%S",localtime(&testTime));
	//SetCtrlVal (panel, MEASURE_TEST_TIME, str);
	SetTableCellVal (panel, P_PRINT_TABLE, MakePoint (2, 4), str);
	return 0;
}

#endif

// �û������ӡ��ť
int CVICALLBACK OnPrint (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_LEFT_CLICK)
	{
		PrintUserData(&g_UserData,panelPrint);
		PlotData(CHANNEL_TYPE_ALL_DATA,panelPrint, P_PRINT_GRAPH);

		SetPrintAttribute(ATTR_ORIENTATION,VAL_PORTRAIT);
		//SetPrintAttribute(ATTR_SYSTEM_PRINT_DIALOG_ONLY,TRUE);
		int ret = PrintPanel(panelPrint,NULL,1,VAL_VISIBLE_AREA ,TRUE);
		INFO1("PrintPanel return:%d",ret);
	}
	return 0;
}

int SaveData(char *fileName)
{
	FILE *fp;

	if((fp = fopen(fileName,"wb"))==NULL)
	{
		goto Error;
	}
#if 1
	fwrite((void*)&g_UserData,sizeof(g_UserData),1,fp);// ����ת�޻��û����������
#endif
	fwrite((void*)&g_Waveform,sizeof(g_Waveform),1,fp);// ����ת�޻��ɼ����Ĳ���

Error:
	if(fp)fclose(fp);
	return 0;
}

int LoadWaveform(char *fileName)
{
	FILE *fp;
	if((fp = fopen(fileName,"rb"))==NULL)
	{
		goto Error;
	}
#if 1
	fread((void*)&g_UserData,sizeof(g_UserData),1,fp);// ��ȡת�޻��û����������
#endif
	fread((void*)&g_Waveform,sizeof(g_Waveform),1,fp);// ��ȡת�޻��ɼ����Ĳ���
Error:
	if(fp)fclose(fp);
	return 0;
}

int CVICALLBACK OnSaveData (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	if(event == EVENT_LEFT_CLICK)
	{
		SendFSMSig(&g_fsmID,FSM_SIG_SAVE);
		//UpdateUserData(panelUserData,FALSE);
		//DisplayPanel(panelUserData);
	}
	return 0;
}


char* GetWaveFormPath()
{
	static char path[MAX_PATHNAME_LEN];
	GetProjectDir(path);
	sprintf(path,"%s\\waveform",path);
	return path;
}

int LoadALLData()
{
	char fileName[MAX_PATHNAME_LEN];
	// ȡ���û�Ҫ������ļ���

	if(FileSelectPopupEx(GetWaveFormPath(),"*.vif","","�򿪲����ļ�",VAL_LOAD_BUTTON ,0,1,fileName) != VAL_NO_FILE_SELECTED)
	{
		// �򿪸��ļ���������
		LoadWaveform(fileName);
		// �Ը����ݻ�������
		UpdatePlot();
	}
	else   // �û�����
	{
	}
	return 0;
}
int CVICALLBACK OnOpenData (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	if(event == EVENT_LEFT_CLICK)
	{
		//LoadALLData();
		SendFSMSig(&g_fsmID,FSM_SIG_LOAD);
	}
	return 0;
}


int SaveForceDataAsCSV(char *fileName)
{
	FILE *fp;
	int retVal = 0;

	if((fp = fopen(fileName,"w"))==NULL)
	{
		goto Error;
	}

	for(int i = 0; i < g_Waveform.rawDataLen; i++)
	{
		fprintf(fp,"%f,",g_Waveform.rawData[CHANNEL_FORCE][i]); 	// ����ת�޻��ɼ�����ԭʼ����
	}	
	
	fprintf(fp,"\n");
			
	for(int i = 0; i < g_Waveform.forceLen; i++)
	{
		fprintf(fp,"%f,",g_Waveform.force[i]); 		// ����ת�޻��ɼ���ת���Ĳ���
	}
	
	
Error:
	if(fp){
		fclose(fp);
	}else{
		retVal = 1;	 // ����
	}
	return retVal;
}

int SaveALLData()
{
	char fullFileName[MAX_PATHNAME_LEN];
	
#if 0
	strcpy(fullFileName,GetWaveFormPath());

	//CreatWaveformName(fullFileName);
	if(FileSelectPopupEx(GetWaveFormPath(),"*.dat","","���沨��",VAL_SAVE_BUTTON,1,1,fullFileName)
			!= VAL_NO_FILE_SELECTED)
	{
		
		SaveData(fullFileName);  // �����ļ����ɼ�����ת�޻����ݺ��û�����
	}
#else	
	sprintf(fullFileName,"%s\\%s",GetWaveFormPath(),g_FileName);
	SaveData(fullFileName);  // �����ļ����ɼ�����ת�޻����ݺ��û�����
	
	SaveForceDataAsCSV("force.csv");
#endif	
	return 0;
}

int CVICALLBACK OnSaveUserData (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	if(event == EVENT_LEFT_CLICK)
	{
		HidePanel(panel);
		UpdateUserData(panel,TRUE);
		SaveALLData();
	}
	return 0;
}

int CVICALLBACK OnCancelSaveUserData (int panel, int control, int event,
									  void *callbackData, int eventData1, int eventData2)
{
	if(event == EVENT_LEFT_CLICK)
	{
		HidePanel(panel);
	}
	return 0;
}

int CVICALLBACK OnMeasureCancel (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	if (event == EVENT_LEFT_CLICK)
	{
		HidePanel(panel);
	}
	return 0;
}

int CVICALLBACK ONSwitchNumChange (int panel, int control, int event,
								   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			char fileName[MAX_PATHNAME_LEN];
			UpdateUserData(panel,TRUE);
			CreatWaveformName(fileName);
			SetCtrlVal(panel,USER_DATA_FILENAME,fileName);
			break;
	}
	return 0;
}

///////////////////////////////////////////////
// �̻߳ص�����.�ɼ��߳�
void Acquire()
{
	FSM_ID fsm;
	GetFSMState(&g_fsmID,&fsm);	
	switch(fsm.state){
		case FSM_STATE_MEASURE:	// ������	
			ReadMeasure(g_Waveform.sampleData,&g_Waveform.sampleDataLen);
			SampleData2RawData(g_Waveform.sampleData,g_Waveform.sampleDataLen);			
			break;			
		case FSM_STATE_AUTO_MEASURE:  // �ȴ������źţ��Զ�������ֻ��ʾ��ǰֵ������������	
			ReadMeasure(g_Waveform.sampleData,&g_Waveform.sampleDataLen);
			if(isAutoStart(g_Waveform.sampleData,g_Waveform.sampleDataLen))
			{
				SampleData2RawData(g_Waveform.sampleData,g_Waveform.sampleDataLen);
				SendFSMSig(&g_fsmID,FSM_SIG_START_MEASURE);
			}
			break;
	}
}
int CVICALLBACK ThreadAcquire(void *functionData)
{
	ThreadControl *ctl = (ThreadControl*)functionData;
	
	DAQmxStartTask(g_info.task);
	while (!ctl->quit) {
		if (ctl->suspend) { // ��ͣ��ǰ�̵߳�����
			SuspendThread (GetCurrentThread ());
			ctl->suspend = 0;
		}
		Acquire();// �ɼ�����
		//Analyze(. . .);// ����ɼ���������		
	}
	DAQmxStopTask(g_info.task);
	DAQmxWaitUntilTaskDone(g_info.task,DAQmx_Val_WaitInfinitely);//�ȴ���ʱ����Ϊ1��
	return 0;
}

