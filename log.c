/*
log数据。有如下功能
1.ERR(),INFO(),DEBUG()函数向外输出调试信息。
	函数的参数与printf完全一样.
2.可输出到文件和debugview软件监视软件。
	PRINT_LOG宏定义输出流向。
	默认为.\log\log.txt和被debugview软件监视。
3.可以调整输出信息级别。方便调试过程中和调试结束后，而无需删除调试信息。
分为完全关闭/打开 CONFIG_ENABLE_DEBUG
	DEBUG_LEVEL_ERR,	只输出ERR信息
	DEBUG_LEVEL_INFO,   只输出ERR和INFO信息
	DEBUG_LEVEL_DEBUG,    输出ERR和INFO和DEBUG信息。
*/

//#include <ansi_c.h>
//#include <utility.h>


#include <WINDOWS.H>
#include <ansi_c.h>

#include "log.h"
#include <TIME.H>

#ifdef __cplusplus
extern "C" {
#endif

//#define	LOG_APPEND     // 追加记录到log.txt文件
#define	LOG_ONLY_ONCE  // 清除以前的数据，只保存本次运行的数据。


#define CONFIG_ENABLE_DEBUG
#define DEBUG_LEVEL DEBUG_LEVEL_DEBUG

	static char* TitleInfo[]=
	{
		"SKQ-DISABLE:","SKQ-ERR:","SKQ-INFO:","SKQ-DEBUG:"
	};

	void ERR1(char fmt[],...)
	{
#if defined CONFIG_ENABLE_DEBUG && (DEBUG_LEVEL >= DEBUG_LEVEL_ERR)
		va_list args;

		va_start(args,fmt);
		PRINT(DEBUG_LEVEL_ERR,fmt,args);
		va_end(args);
#endif
	}


	void INFO1(char fmt[],...)
	{
#if defined CONFIG_ENABLE_DEBUG && (DEBUG_LEVEL >= DEBUG_LEVEL_INFO)
		va_list args;

		va_start(args,fmt);
		PRINT(DEBUG_LEVEL_INFO,fmt,args);
		va_end(args);
#endif
	}

	void DEBUG1(char fmt[],...)
	{
#if defined CONFIG_ENABLE_DEBUG && (DEBUG_LEVEL >= DEBUG_LEVEL_DEBUG)
		va_list args;

		va_start(args,fmt);
		PRINT(DEBUG_LEVEL_DEBUG,fmt,args);
		va_end(args);
#endif
	}







# if 0
	void ASSERT()
	{
		//PRINT("ASSERT: %s %s %d",__FILE__, __FUNCTION__, __LINE__);
		while (1);
	}
	void INFO(x)
	{
		if (debug >= DEBUG_LEVEL_INFO)
		{
			PRINT(x);
		}
	}

	void DEBUG(x)
	{
		if (debug >= DEBUG_LEVEL_DEBUG)
		{
			PRINT(x);
		}
	}
#endif




//int ToLog (char file_name[],char error_type[],char function[])
#define LogErr(x)								\
	{												\
		LogErr2File("log.txt",x);					\
	}

	int Log2StrAndPrint(DebugLevel debuglevel,char * fmt,va_list args)
	{
		char buffer[1024];
		char *pbuf = buffer;

		sprintf(pbuf,"%s",TitleInfo[debuglevel]);
		pbuf += strlen(pbuf);

		vsprintf(pbuf,fmt,args);
		pbuf += strlen(pbuf);

		PRINT_LOG (buffer);

		return 0;
	}


	int Str2FileWithTime(char file_name[],char *str)
	{
		/*
			strftime( timebuf, sizeof(timebuf), " %Y/%m/%d %X %A 本年第%j天 %z" , localtime(&t) );
			%Y：年		%m：月		%d：日		%A：星期
			%X：时分秒	%j：本年第几天
		*/

		FILE *fileHandle;
		char timebuf[100];
		static isFirst=1;

		time_t t =time(0);

		strftime( timebuf, sizeof(timebuf), "%Y/%m/%d %X --- " , localtime(&t) );

		//
#ifdef LOG_SAVE_APPEND
		fileHandle = fopen (file_name, "a");
#elif defined  LOG_ONLY_ONCE
		if(isFirst)
		{
			isFirst = 0;
			if((fileHandle = fopen (file_name, "w"))==NULL){
				system("cmd.exe /c mkdir Log");
				fileHandle = fopen (file_name, "w");
			};
		}
		else
		{
			if((fileHandle = fopen (file_name, "a"))==NULL){
				system("cmd.exe /c mkdir Log");
				fileHandle = fopen (file_name, "a");
			};
		}
#else
		{
			return 0;
		}
#endif
		//fileHandle = fopen (file_name, "a");
		//

		fputs(timebuf,fileHandle);
		fputs (str, fileHandle);
		fputs("\n",fileHandle);
		fclose (fileHandle);
		return 0;
	}


	int Str2File(char file_name[],char *str)
	{

		FILE *fileHandle;

		fileHandle = fopen (file_name, "a");
		fputs (str, fileHandle);
		fclose (fileHandle);
		return 0;
	}


	int LogErr2File(char file_name[],char * fmt,va_list args)
	{

		//char *datee;
		//char *timee;
		FILE *fileHandle;
		//int sec,min,hour,year,day,mon;

		char buffer[200];
		char report[200];

		//sprintf(report_name,"%s\\",Result_folder);
		//sprintf(report,"Log\\%s","Note.txt");
		sprintf(report,"%s",file_name);

		fileHandle = fopen (report, "a");

		//GetSystemDate (&mon, &day, &year);
		//GetSystemTime (&hour, &min, &sec);

		//strcpy(buffer,"");
		//sprintf(buffer,"\t%d/%.2d/%.2d%  .2d:%.2d:%.2d\n",year,mon,day,hour,min,sec);
		//fputs (buffer, fileHandle);

		//va_start(args,fmt);
		vsprintf(buffer,fmt,args);
		//va_end(args);
		fputs (buffer, fileHandle);

		sprintf(buffer,"\n");
		fputs (buffer, fileHandle);
		fclose (fileHandle);

		return 0;
	}


# if 0
#define DEBUGERROR   DebugError(__FILE__,__LINE__

	UINT8 DebugError(UINT8 *file, UINT32 line, UINT8 * fmt,...)
	{
		char    buf[PARAM_LEN] = {0};
		va_list    args;

		va_start(args,fmt);
		vsprintf(buf,fmt,args);
		va_end(args);

		return  DEBUG_LOG(DEBUG_ERROR, DEBUG_ERROR,  file, line, buf);
	}

	UINT8 DEBUG_LOG(UINT8 logLevel, UINT8 printLevel, UINT8 *file, UINT32 line, UINT8 *fmt, ...)
	{
		UINT8 pname[PNAME_LEN] = {'\0'};
		MODOPT *ptDebugOpt = NULL;
		UINT8 buffer[PARAM_LEN] = {'\0'};
		va_list    args;

		va_start(args, fmt);
		vsnprintf(buffer, PARAM_LEN-1, fmt, args);
		va_end(args);

		strncpy(pname, ptDebugOpt->; pname, PNAME_LEN);
		write_log(pname, buffer, file, line);

		return ((INT8)-1);
	}
#endif

#if 0
	int ToLog (char file_name[],char error_type[],char function[])
	{
		char *datee;
		char *timee;
		FILE *fileHandle;
		int sec,min,hour,year,day,mon;
		char buffer[200];
		char report[200];

		//sprintf(report_name,"%s\\",Result_folder);
		//sprintf(report,"Log\\%s","Note.txt");
		sprintf(report,"%s",file_name);

		//LOG_SAVE_APPEND, // 以追加方式保存到log.txt文件
		//LOG_SAVE_WRITE,  // 清除以前的数据，只保存本次运行的数据。

		fileHandle = fopen (report, "a");
		//GetSystemDate (&mon, &day, &year);
		//GetSystemTime (&hour, &min, &sec);

		strcpy(buffer,"");
		sprintf(buffer,"[%s]\n",error_type);
		fputs (buffer, fileHandle);

		strcpy(buffer,"");
		sprintf(buffer,"\t%d/%.2d/%.2d%  .2d:%.2d:%.2d\n",year,mon,day,hour,min,sec);
		fputs (buffer, fileHandle);

		strcpy(buffer,"");
		sprintf(buffer,"\t%s\n",function);
		fputs (buffer, fileHandle);

		sprintf(buffer,"\n");
		fputs (buffer, fileHandle);
		fclose (fileHandle);

		return 0;
	}
#endif

// 测量参数写入Log文件。
	void TestPara2Log(double dfDistance,  // 动程
					  double dfCurrentForce,  // 当前力
					  double dfRatedForce,	 // 目标力
					  double fAOAForce,       // 比例阀A电压
					  double fAOBForce)       // 比例阀B电压
	{
		INFO1("    动程:%f(mm)，当前力:%f(KN)，目标力:%f(KN)，比例阀A电压:%f(V)，比例阀B电压:%f(V)",
			  dfDistance,
			  dfCurrentForce,dfRatedForce,
			  fAOAForce,
			  fAOBForce
			 );
	}
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
)
{
	INFO1("    时间:%u(ms)，步骤:%s，动程:%f(mm)，速度:%f(mm/s)，"
		  "卡阻:%d，当前力:%f(KN)，目标力:%f(KN)，"
		  "比例阀A电压:%f(V)，比例阀B电压:%f(V)，"
		  "A相电流:%f(A)，B相电流:%f(A)，C相电流:%f(A)，"
		  "累计次数:%d次，累计力:%f(KN)，累计A相电流:%f(A)，"
		  "累计B相电流:%f(A)，累计C相电流:%f(A)",
		  Ticks,	//本次测量时间(tick)(mm)
		  logbuf,	//步骤文本
		  dfDistance,	//动程(mm)
		  dfSpeed,		//速度(mm/s)
		  
		  bOk,			 //卡阻
		  dfCurrentForce,//当前力(KN)
		  dfRatedForce,	 //目标力
		  fAOAForce,	 //比例阀A电压
		  fAOBForce,	 //比例阀B电压
		  
		  dfIa,//A相电流(A)
		  dfIb,//B相电流(A)
		  dfIc,//C相电流(A)
		  dwCount,		//累计次数
		  dfForceCount,	//累计力(KN)
		  dfIaCount,		//累计A相电流(A)
		  dfIbCount,		//累计B相电流(A)
		  dfIcCount 		//累计C相电流(A)
		 );

}


#ifdef __cplusplus
}
#endif
