#include <utility.h>
#include <ansi_c.h>
#include "global.h"
#include "UserThreadQueue.h"
//UserThreadQueue.c
#define SAMPLE_LEN SAMPLE_RATE*SAMPLE_BUF_READ_TIME  //250


#define DATA_QUEUE_SIZE 10	// 队列长度，1s的采样数据
static CmtTSQHandle	g_dataQueueHdl		   = 0;
////////////////////////////////////////////////////////////
static int WriteQueueItem (uint8_t *dataValue);
int ReadQueueItem (uint8_t *outDataValue,ssize_t lenByte);
int ItemsInQueue(CmtTSQHandle);
////////////////////////////////////////////////////////////
typedef struct{
	uint8_t data[SAMPLE_CHANNEL*SAMPLE_RATE*sizeof(double)]; // 占位
	int len;
}ItemData_Sample;
#define EACH_DATA_SIZE_SAMPLE sizeof(ItemData_Sample)	// 每单位数据长度

////////////////////////////////////////////////////////////
/*---------------------------------------------------------------------------*/
/* 接口函数                                                                  */
/*---------------------------------------------------------------------------*/

int InitThreadQueue()
{
	int retVal = 0;
	if (CmtNewTSQ (DATA_QUEUE_SIZE, EACH_DATA_SIZE_SAMPLE, OPT_TSQ_DYNAMIC_SIZE,
				   &g_dataQueueHdl) < 0)
		retVal = -1;
	return retVal;
}

int UninitThreadQueue()
{
	int retVal = 0;
	retVal = CmtDiscardTSQ (g_dataQueueHdl);
	return retVal;
}

int ReadQueueAllData_Sample()
{
	int retVal = 0;
	while(ReadQueue_Sample(g_Waveform.sampleData,&g_Waveform.sampleDataLen))
	{
		//g_Waveform.sampleDataLen = SAMPLE_LEN;
		SampleData2RawData(g_Waveform.sampleData,g_Waveform.sampleDataLen);
		retVal = 1;
	}	
	return retVal;
}

int ReadQueue_Sample(double outData[SAMPLE_CHANNEL][SAMPLE_RATE],int len[1])
{
	int retVal = 0;
	ItemData_Sample itemData;
	if(ReadQueueItem((uint8_t*)&itemData,sizeof(ItemData_Sample))){
		len[0] = itemData.len;		
		memcpy(outData,itemData.data,sizeof(itemData.data));	
		retVal = 1;
	}
	return retVal;
}

int WriteQueue_Sample(double inData[SAMPLE_CHANNEL][SAMPLE_RATE],int len)
{
	ItemData_Sample itemData;
	itemData.len = len;
	memcpy(itemData.data,inData,sizeof(itemData.data));
	WriteQueueItem((uint8_t*)&itemData);
	return 0;
}
// 清空队列，删除所有数据
int FlushQueue_Sample()
{
	/* Empty the queue so that we don't have data in next time. */
	CmtFlushTSQ (g_dataQueueHdl, TSQ_FLUSH_ALL, NULL);
	return 0;
}
/*---------------------------------------------------------------------------*/
/* 内部函数                                                                  */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/* This function will do most of the work in response to an async. timer tick*/
/*---------------------------------------------------------------------------*/
//static int WriteQueueItem (double dataValue[SAMPLE_CHANNEL][SAMPLE_RATE],int len)
static int WriteQueueItem (uint8_t *dataValue)
{
	CmtWriteTSQData (g_dataQueueHdl, dataValue, 1, TSQ_INFINITE_TIMEOUT,
					 NULL);
	return 0;
}
/*---------------------------------------------------------------------------*/
/* This function will be called by the monitor CVI timer callback to update  */
/* the UIR with data information on the other timers.                        */
//---------------------------------------------------------------------------*/
int ItemsInQueue()
{
	size_t 	   nItems; 
	CmtGetTSQAttribute (g_dataQueueHdl, ATTR_TSQ_ITEMS_IN_QUEUE, &nItems);
	return nItems;
}
#define ReadQueueItem_Ver 2
#if ReadQueueItem_Ver == 2
int ReadQueueItem (uint8_t *outDataValue,ssize_t lenByte)
{
	size_t 	   nItems;
	int        nItemsRead;
	//double 	   *dataValues;
	int retVal = 0;

	CmtGetTSQAttribute (g_dataQueueHdl, ATTR_TSQ_ITEMS_IN_QUEUE, &nItems);
	if (nItems > 0)
	{
		uint8_t dataValues[lenByte];
		nItemsRead = CmtReadTSQData (g_dataQueueHdl, dataValues, 1,
									 TSQ_INFINITE_TIMEOUT, 0);
		if (nItemsRead > 0)
		{
			memcpy(outDataValue,dataValues,lenByte);
			retVal = 1;
		}
	}
	return retVal;	
}
#elif ReadQueueItem_Ver == 1
int ReadQueueItem (double outDataValue[SAMPLE_CHANNEL][SAMPLE_RATE],int len[1])
{
	size_t 	   nItems;
	int        nItemsRead;
	//double 	   *dataValues;
	double (*dataValues)[SAMPLE_RATE];
	int retVal = 0;

	CmtGetTSQAttribute (g_dataQueueHdl, ATTR_TSQ_ITEMS_IN_QUEUE, &nItems);
	if (nItems > 0)
	{
		if ((dataValues = (double (*)[SAMPLE_RATE]) malloc (nItems *EACH_DATA_SIZE)) != NULL)
		{
			nItemsRead = CmtReadTSQData (g_dataQueueHdl, dataValues, nItems,
										 TSQ_INFINITE_TIMEOUT, 0);
			if (nItemsRead > 0)
			{
				memcpy(outDataValue,dataValues,EACH_DATA_SIZE);
				//len[0] = nItemsRead;
				retVal = 1;
				if (len != NULL)
					len[0] = SAMPLE_LEN;
			}
		}
		free (dataValues);
	}
	return retVal;	
}
#elif ReadQueueItem_Ver == 0
static int ReadQueueItem (double outDataValue[SAMPLE_CHANNEL][SAMPLE_RATE],int len)
{
	size_t 	   nItems;
	int        nItemsRead;
	//double 	   *dataValues;
	double (*dataValues)[SAMPLE_RATE];
	int retVal = 0;

	CmtGetTSQAttribute (g_dataQueueHdl, ATTR_TSQ_ITEMS_IN_QUEUE, &nItems);
	if (nItems > len)
	{
		if ((dataValues = (double *) malloc (nItems *EACH_DATA_SIZE)) != NULL)
		{
			nItemsRead = CmtReadTSQData (g_dataQueueHdl, dataValues, nItems,
										 TSQ_INFINITE_TIMEOUT, 0);
			if (nItemsRead > 0)
			{
#if 0				
				PlotStripChart (panelHandle,  PANEL_DATASTRIPCHART,
								(double*)dataValues, nItemsRead, 0,
								0, VAL_DOUBLE);
				SetCtrlVal(panelHandle, PANEL_DELTAWRITTEN, nItemsRead);
#else
				memcpy(outDataValue,dataValues,EACH_DATA_SIZE);
				//len[0] = nItemsRead;
				retVal = 1;
#endif
			}
		}
		free (dataValues);
	}
	return retVal;
}
#endif
