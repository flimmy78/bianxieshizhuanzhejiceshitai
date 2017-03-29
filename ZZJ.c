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

#include <NIDAQmx.h>


//#define PORTABLE


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
	RUN_MODEL_RUNING,
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



#define SAMPLE_RATE 2500	 	 // ����Ƶ�� (Hz)
#define SAMPLE_MAX_TIME 20      // ������ʱ�䡣(s)
#define SAMPLE_CHANNEL 4         // ͨ����
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
	double rawData[SAMPLE_CHANNEL][SAMPLE_RATE * SAMPLE_MAX_TIME];// ֱ�ӴӲɼ�����ȡ��ԭʼ���ݡ�ÿͨ������SAMPLE_RATE * SAMPLE_MAX_TIME
	// ͨ����0��������1����ѹ��2����׼��3����
	int    rawDataLen;// ԭʼ���ݵĳ���

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
void Measure(int isStart);
int DisplayCursorData(int panel,int control);  // ��ʾ��괦�����������Ȳ���
int EnableCursor(int panel, int control) ; 	   // ��ʾ/���ع��
void PlotData(CHANNEL_TYPE chanType,int panel,int graphCtrl);  // ����:��ɫ����ѹ����ɫ����ѹ��׼����ɫ��ת��������ɫ
int UpdateUserData(int panel,BOOL isSave);	// panel����Ҫ���µ���塣TRUE:���浽�ڴ����ݣ�FALSE:�ڴ����ݵ�����
int TBDisplayTime( void );                  // ������ʾ��ǰʱ��

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
		default:
			ret = -1;
			ERR1("Channel2Addr ��������channel:%d",channel);
			goto Error;
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
				*x = (*chData.lastAddr - chData.addr -1)*(*chData.increment);
			}
			else
			{
				*x = 0;
			}
			break;
	}
	//*x = (*chData.lastAddr - chData.addr)*(*chData.increment);
	*increment = *chData.increment;
Error:
	return ret;
}

int  ChartInit(TaskHandle task)
{
	int error = 0;
	//uInt32 numChannels;

	g_info.task = task;
	g_info.timerCtrl = MAIN_TIMER_MEASURE;
	g_info.numSampsPerChan = SAMPLE_RATE/10; // 0.1s�ɼ�������
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
	DAQmxClearTask(task);
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
			g_info.running = RUN_MODEL_QUIT;
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
		if(g_info.running == RUN_MODEL_RUNING)  // ��ǰ���ڲɼ�������ֹͣ�ɼ����ݡ�
		{
			Measure(FALSE); // ֹͣ�ɼ�
		}
		else if(g_info.running == RUN_MODEL_STOP)// ��ǰ����ֹͣ�����濪ʼ�ɼ����ݡ�
		{
			DisplayPanel(panelMeasure);//�л����ɼ��������
		}
	}
	return 0;
}
int UpdatePlot()
{
	// �򿪿���ݣ����»���һ��ͼ��
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
// ��ʼ����
void Measure(int isStart)
{
	int DAQmxError = 0;
	if(isStart)   // ��ʼ�ɼ�
	{
		// Example Core: Start the task.
		//DAQmxErrChk(DAQmxStartTask(g_info.task));
		//SetCtrlAttribute (panelMain, g_info.timerCtrl, ATTR_ENABLED, 1);
		DeleteGraphPlot(panelGraph,GRAPH_GRAPH,-1,VAL_DELAYED_DRAW);

		SetCtrlVal(panelMain,MAIN_LED_RUNING,TRUE);
		SetCtrlAttribute(panelMain,MAIN_PIC_MEASURE,ATTR_LABEL_TEXT ,"ֹͣ�ɼ�");
		SetCtrlAttribute(panelMain,MAIN_PIC_SAVE,   ATTR_VISIBLE,FALSE);
		SetCtrlAttribute(panelMain,MAIN_LED_SAVE,   ATTR_VISIBLE,FALSE);
		SetCtrlAttribute(panelMain,MAIN_PIC_OPEN,   ATTR_VISIBLE,FALSE);
		SetCtrlAttribute(panelMain,MAIN_LED_OPEN,   ATTR_VISIBLE,FALSE);
		//SetCtrlAttribute(panelMain,MAIN_PIC_PRINT,  ATTR_VISIBLE,FALSE);
		//SetCtrlAttribute(panelMain,MAIN_LED_PRINT,  ATTR_VISIBLE,FALSE);

		SetAxisScalingMode(g_info.chartPanel,g_info.chartCtrl,VAL_LEFT_YAXIS, g_info.scalingMode[0],g_info.min[0],g_info.max[0]);
		SetAxisScalingMode(g_info.chartPanel,g_info.chartCtrl,VAL_RIGHT_YAXIS,g_info.scalingMode[1],g_info.min[1],g_info.max[1]);
		//SetCtrlAttribute(panel,control,ATTR_PICT_BGCOLOR,0x185776);
		g_info.running = RUN_MODEL_RUNING;
		g_info.startTick = GetTickCount();
		WaveformInit();
		UpdateUserData(panelMeasure,TRUE);

	}
	else  // ֹͣ�ɼ�
	{
		g_info.running = RUN_MODEL_STOP;
		g_info.workModel = WORK_MODEL_MANUAL;
		g_info.processedTick = 0;
		DAQmxErrChk(DAQmxStopTask(g_info.task));

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

#if 1
		UpdatePlot();
#endif
		// ����ת�޻��������
		UpdateUserData(panelMeasure,FALSE);

		//OnStartMeasure(panelMain,MAIN_PIC_MEASURE,EVENT_LEFT_CLICK,NULL,0,0);

	}
Error:
	DAQmxReportErr(DAQmxError);
	return;
}


int UpdateUserData(int panel,BOOL isSave)
{
	char str[50];
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
			g_info.workModel = WORK_MODEL_AUTO;
		}
		else    // �ֶ�����
		{
			Measure(TRUE);
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


int DisplayData()
{
	double* data;
	double val;
	// ��ʾ��ѹ(��Чֵ)������(��Чֵ)����
	data = g_Waveform.rawData[CHANNEL_I];
	RMS(data,g_info.numSampsPerChan,&val);// ����
	SetCtrlVal(panelMain,MAIN_NUM_I,val);

	data = g_Waveform.rawData[CHANNEL_V];
	RMS(data,g_info.numSampsPerChan,&val);// ��ѹ
	SetCtrlVal(panelMain,MAIN_NUM_V,val);

	data = g_Waveform.rawData[CHANNEL_VREF];
	RMS(data,g_info.numSampsPerChan,&val);// ��ѹ��׼
	SetCtrlVal(panelMain,MAIN_NUM_VREF,val);

	data = g_Waveform.rawData[CHANNEL_FORCE];
	RMS(data,g_info.numSampsPerChan,&val);// ������
	SetCtrlVal(panelMain,MAIN_NUM_FORCE,val);

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

int isAutoStart()
{
	int ret = 0;
	double *waveform;
	double x;
	int len;
	double increment;

	WaveformGet(CHANNEL_I,CHANNEL_TYPE_LAST_DATA,&waveform,&len,&x,&increment);	// ����
	if(len!=0)  // �в���
	{
		if(MaxDoubleArray(waveform,len) > 1.0 )
		{
			ret = 1;
		}
		else
		{
			WaveformGet(CHANNEL_V,CHANNEL_TYPE_LAST_DATA,&waveform,&len,&x,&increment);	// ��ѹ
			if(len!=0)
			{
				if(MaxDoubleArray(waveform,len) > 1.0 )
				{
					ret = 1;
				}
			}
		}
	}

	return ret;
}

int RawData2Waveform(int isRuning,double rawData[SAMPLE_CHANNEL][SAMPLE_RATE * SAMPLE_MAX_TIME])
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
	data = rawData[CHANNEL_I];
	for(int i=0; i<numRMS; ++i)
	{
		RMS(data+i*sampNumPerCycle,sampNumPerCycle,&waveform[i]);
	}
	WaveformSet(CHANNEL_I,waveform,numRMS,initialX);


	// ��ѹ
	data = rawData[CHANNEL_V];
	for(int i=0; i<numRMS; ++i)
	{
		RMS(data+i*sampNumPerCycle,sampNumPerCycle,&waveform[i]);
	}

	WaveformSet(CHANNEL_V,waveform,numRMS,initialX);
	initialX +=  numRMS;


	// ��ѹ��׼
	data = rawData[CHANNEL_VREF];
	WaveformSet(CHANNEL_VREF,data,g_info.numSampsPerChan,initialVRef);
	initialVRef +=g_info.numSampsPerChan;

	// ת����
	data = rawData[CHANNEL_FORCE];
	int numMedian = 10;//0.1s/(1/100Hz)
	for(int i=0; i<numMedian; ++i)
	{
		Median(data+i*25,25,&waveform[i]);//ÿ��25���м�ֵ
	}
	WaveformSet(CHANNEL_FORCE,waveform,numMedian,initialForce);
	initialForce +=numMedian;// ��ѹ�ο�

	return 0;
}
int ReadMeasure(double rawData[SAMPLE_CHANNEL][SAMPLE_RATE * SAMPLE_MAX_TIME])
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
		memcpy(g_Waveform.rawData[channel],data,g_info.numSampsPerChan*sizeof(double));
	}
#else
	int len = g_info.numSampsPerChan;
	for(int channel=0; channel<numChannels; channel++)
	{
		memcpy(rawData[channel],data+len*channel,len*sizeof(double));
	}
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
void PlotData(CHANNEL_TYPE chanType,int panel,int graphCtrl)  // ����:��ɫ����ѹ����ɫ����ѹ��׼����ɫ��ת��������ɫ
{
	double* waveform;
	double x;
	int len;
	double increment;
	int plotHandle;

	SetCtrlAttribute(panel, graphCtrl,ATTR_REFRESH_GRAPH,0);
	WaveformGet(CHANNEL_I,chanType,&waveform,&len,&x,&increment);	// ���Ƶ�������ͼ
	if(len)
	{
		plotHandle = PlotWaveform (panel, graphCtrl, waveform, len, VAL_DOUBLE, 1.0f, 0.0f,
								   x, increment, VAL_THIN_LINE, VAL_SOLID_SQUARE, VAL_SOLID, 1, VAL_RED);
		SetPlotAttribute (panel, graphCtrl, plotHandle, ATTR_PLOT_YAXIS, VAL_RIGHT_YAXIS);
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


	WaveformGet(CHANNEL_FORCE,chanType,&waveform,&len,&x,&increment);// ����ת��������ͼ
	if(len)
	{
		PlotWaveform (panel, graphCtrl, waveform, len, VAL_DOUBLE, 1.0f, 0.0f,
					  x, increment, VAL_THIN_LINE, VAL_SOLID_SQUARE, VAL_SOLID, 1, VAL_GREEN);
	}

	SetCtrlAttribute(panel, graphCtrl,ATTR_REFRESH_GRAPH,1);

}


int MeasureDisplayAndAutoStart()
{
	if(g_info.running == RUN_MODEL_STOP) // û���ڲ�����ֻ��ʾ��ǰֵ������������
	{
		if(g_info.workModel == WORK_MODEL_AUTO)
		{
			ReadMeasure(g_Waveform.rawData);
			RawData2Waveform(FALSE,g_Waveform.rawData);
			DisplayData();
			if(isAutoStart())
			{
				Measure(TRUE);
			}
			else
			{
			}
		}
	}
	else if(g_info.running == RUN_MODEL_RUNING)     // ���ڲ���
	{
		int maxTime_ms = 15*1000;	   // ֻ����15���ڵ�����
		int curTick = GetTickCount();
		if(curTick > g_info.startTick + maxTime_ms)
			curTick = g_info.startTick + maxTime_ms;
		int n = (curTick - g_info.startTick - g_info.processedTick)/100;
		g_info.processedTick = ((curTick - g_info.startTick)/100)*100;
		for(int i=0; i<n; i++)
		{
			ReadMeasure(g_Waveform.rawData);
			RawData2Waveform(TRUE,g_Waveform.rawData);
			DisplayData();
			//ReadMeasureAndToWaveform();
			PlotData(CHANNEL_TYPE_LAST_DATA,g_info.chartPanel, g_info.chartCtrl);
		}
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
#if 0
int CVICALLBACK OnTimer_Measure (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	static int n;
	static int curTick;
	switch (event)
	{
		case EVENT_TIMER_TICK:
			if(g_info.running == RUN_MODEL_RUNING)
			{
				curTick = GetTickCount();
				n = (curTick - g_info.startTick - g_info.processedTick)/100;
				g_info.processedTick = ((curTick - g_info.startTick)/100)*100;
				for(int i=0; i<n; i++)
				{
					MeasureDisplayAndAutoStart();
				}
			}
			else
			{
				MeasureDisplayAndAutoStart();
			}
			break;
	}
	return 0;
}
#else
int CVICALLBACK OnTimer_Measure (int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_TIMER_TICK:
			MeasureDisplayAndAutoStart();
			break;
	}
	return 0;
}
#endif

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
		UpdateUserData(panelUserData,FALSE);
		DisplayPanel(panelUserData);
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
		LoadALLData();
	}
	return 0;
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
