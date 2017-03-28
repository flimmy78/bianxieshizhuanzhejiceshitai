/*
 * Name:    log.h
 *
 * Purpose: general debug system
 *
 */
#ifndef __LOG_H__1234__
#define __LOG_H__1234__

#ifdef __cplusplus
	extern "C"{
#endif

#include "stdio.h"
#include "stdarg.h"

typedef enum _debug_level {
	DEBUG_LEVEL_DISABLE = 0,
	DEBUG_LEVEL_ERR,
	DEBUG_LEVEL_INFO,
	DEBUG_LEVEL_DEBUG,
}DebugLevel;

int ToLog (char file_name[],char error_type[],char function[]);
int LogErr2File(char file_name[],char * fmt,va_list args);
int Log2StrAndPrint(DebugLevel debuglevel,char * fmt,va_list args);
int Str2FileWithTime(char file_name[],char *str);

void INFO1(char fmt[],...);
void DEBUG1(char fmt[],...);
void ERR1(char fmt[],...);

//#define PRINT(fmt,args)               LogErr2File("log.txt",fmt,args)
#define PRINT(debuglevel,fmt,args)		Log2StrAndPrint(debuglevel,fmt,args)
#define PRINT_LOG(str)	Str2FileWithTime(".\\log\\log.txt",str);OutputDebugString(str)


/////////////////////////
// 测量参数写入Log文件。版本1。
void TestPara2Log(double dfDistance,  // 动程
		 double dfCurrentForce,  // 当前力
		 double dfRatedForce,	 // 目标力
		 double fAOAForce,       // 比例阀A电压
		 double fAOBForce);      // 比例阀B电压
// 测量参数写入Log文件。版本2，写入更多数据。
void TestParaV2ToLog(
	DWORD Ticks,	//本次测量时间(tick)(mm)
	char* logbuf,	//步骤文本
	double dfDistance,	//动程(mm)
	double dfSpeed,						//速度(mm/s)
	BOOL   bOk,							//卡阻
	double dfCurrentForce,
	double dfRatedForce,	//当前力(KN),目标力
	double fAOAForce,	//比例阀A电压
	double fAOBForce,	//比例阀B电压
	double dfIa,//A相电流(A)
	double dfIb,//B相电流(A)
	double dfIc,//C相电流(A)
	int    dwCount,		//累计次数
	double dfForceCount,//累计力(KN)
	double dfIaCount,	//累计A相电流(A)
	double dfIbCount,	//累计B相电流(A)
	double dfIcCount 	//累计C相电流(A)
);
#ifdef __cplusplus
	}
#endif
#endif
