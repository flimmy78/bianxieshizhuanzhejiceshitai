#include <ansi_c.h>
#include <windows.h>
#include <analysis.h>
#include "TimeMeasure.h"


#define MAX_TM_ID 10
#define MAX_TM_COUNT 20*10   //20s,10次/s   
static __int64 m_time[MAX_TM_ID][MAX_TM_COUNT];
static int m_index[MAX_TM_ID];
static int m_overflow[MAX_TM_ID];

int MaxInt(int array[],ssize_t len)
{
	int max = array[0];
	for(int i=1; i<len; i++)
	{
		if(max<array[i])
			max = array[i];
	}
	return max;
}

double MaxDouble(double array[],ssize_t len)
{
	double max = array[0];
	for(int i=1; i<len; i++)
	{
		if(max < array[i])
			max = array[i];
	}
	return max;
}


double MinDouble(double array[],ssize_t len)
{
	double min = array[0];
	for(int i=1; i<len; i++)
	{
		if(min > array[i])
			min = array[i];
	}
	return min;
}

void TimeMeasure(int tmID,TIME_MEASURE tm)
{
	static __int64 startCounter[MAX_TM_ID],stopCounter[MAX_TM_ID];

	if(tmID < MAX_TM_ID)
	{
		if(tm == TM_START)
		{
			QueryPerformanceCounter((LARGE_INTEGER*)&startCounter[tmID]);
		}
		else if(tm == TM_STOP)
		{
			QueryPerformanceCounter((LARGE_INTEGER*)&stopCounter[tmID]);
			if(m_index[tmID] < MAX_TM_COUNT) 	// 没有溢出。
			{
				m_time[tmID][m_index[tmID]] = stopCounter[tmID] - startCounter[tmID];
				m_index[tmID] ++;
			}
			else
			{
				m_overflow[tmID] = 1;//溢出
			}
		}
		else if(tm == TM_CLR)
		{
			m_index[tmID] = 0;
			m_overflow[tmID] = 0;
		}
	}
}

void MaxMin2FileHandle(FILE *fp)
{
	double maxTime[MAX_TM_ID]={0},minTime[MAX_TM_ID]={0},medianTime[MAX_TM_ID]={0};// 中间位置的值(不是平均值)
	double times[MAX_TM_COUNT];
	__int64 freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	fprintf(fp,"\n--------------------------------------------------------\n");	
	int row = MaxInt(m_index,MAX_TM_ID);
	for(int tmID = 0; tmID<MAX_TM_ID && m_index[tmID]!=0; tmID++) // 每一列
	{	
		for(int i = 0; i < row; i++)// 每一行
		{
			//fprintf(fp,"	%.3f,",(double)m_time[tmID][i]/(double)freq*1000);
			times[i] = (double)m_time[tmID][i]/(double)freq*1000;
		}
		maxTime[tmID] = MaxDouble(times,row);
		Median(times,row,&medianTime[tmID]);
		minTime[tmID] = MinDouble(times,row);
	} 
	// 输出
	fprintf(fp,"最小值    ");	
	for(int tmID = 0; tmID<MAX_TM_ID && m_index[tmID]!=0; tmID++)
	{
		fprintf(fp,"	%.3f,",minTime[tmID]);
	}
	/////////////////
	fprintf(fp,"\n");
	fprintf(fp,"中位值    ");	
	for(int tmID = 0; tmID<MAX_TM_ID && m_index[tmID]!=0; tmID++)
	{
		fprintf(fp,"	%.3f,",medianTime[tmID]);
	}
	fprintf(fp,"\n");	
	/////////////////	
	fprintf(fp,"最大值    ");	
	for(int tmID = 0; tmID<MAX_TM_ID && m_index[tmID]!=0; tmID++)
	{
		fprintf(fp,"	%.3f,",maxTime[tmID]);
	}
	fprintf(fp,"\n"); 
}

void SaveTimeMeasure(char *fileName)
{
	__int64 freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	FILE *fp = fopen(fileName,"a");

	fprintf(fp,"\n\n----------------------------开始------------------------\n");
	//fprintf(fp,"		0(ms)		1(ms)		2(ms)		3(ms)		4(ms)		5(ms)		6(ms)		7(ms)		8(ms)		9(ms)		\n");
	for(int tmID = 0; tmID<MAX_TM_ID && m_index[tmID]!=0; tmID++)
	{
		fprintf(fp,"		%d(ms)",tmID);
	}
	fprintf(fp,"\n");

	int row = MaxInt(m_index,MAX_TM_ID);
	for(int i = 0; i < row; i++)
	{
		fprintf(fp,"%3d:    ",i);
		for(int tmID = 0; tmID<MAX_TM_ID && m_index[tmID]!=0; tmID++)
		{
			fprintf(fp,"	%.3f,",(double)m_time[tmID][i]/(double)freq*1000);
		}
		fprintf(fp,"\n");
	}
	//if(maxInt(m_overflow,MAX_TM_ID)!=0)
	MaxMin2FileHandle(fp);
	{
		fprintf(fp,"\n----------------------------溢出------------------------\n\n\n");
		for(int tmID = 0; tmID<MAX_TM_ID && m_overflow[tmID]!=0; tmID++)
		{
			fprintf(fp,"秒表%d溢出",tmID);
		}
	}

	fprintf(fp,"\n----------------------------结束------------------------\n\n\n");
Error:
	for(int tmID = 0; tmID<MAX_TM_ID; tmID++)
	{
		TimeMeasure(tmID,TM_CLR);
	}	
	if(fp)
		fclose(fp);
}
