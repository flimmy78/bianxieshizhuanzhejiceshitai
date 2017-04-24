/*
 * Name:    global.h
 *
 * Purpose: general global 
 *
 */
#ifndef __GLOBAL_H__12342534__
#define __GLOBAL_H__12342534__

#ifdef __cplusplus
	extern "C"{
#endif
/////////////////////////////////////////////////////////////////////		
#define SAMPLE_RATE 2500	 		// 采样频率 (Hz)
#define SAMPLE_BUF_READ_TIME 0.1	// 采样缓冲区读取间隔时间 (s)
#define SAMPLE_MAX_TIME 25      	// 最大采样时间。(s)
#define SAMPLE_CHANNEL 4         	// 通道数
		
/////////////////////////////////////////////////////////////////////		
typedef struct
{
	double sampleData[SAMPLE_CHANNEL][SAMPLE_RATE];// 单次从采集卡读取的原始数据。每通道容量为1秒(SAMPLE_RATE)
	// 通道：0：电流，1：电压，2：基准，3：力
	int    sampleDataLen;// 单次读取的原始数据的长度
	
	double rawData[SAMPLE_CHANNEL][SAMPLE_RATE * SAMPLE_MAX_TIME];; // 一次测量的采集卡数据。时间从测量开始到测量结束
	int    rawDataLen;   // 一次测量的采集卡数据的长度。随着不断读取采集卡数据，该长度数据增大。

	double 	force[100 * SAMPLE_MAX_TIME];	// 力数组，用于绘制波形图。每秒100次为力传感器变化的频率上限。
	double 	forceIncrement;		 			// 间隔时间
	int    	forceLen;  						// 数组长度
	double* forceLastAddr;
	int    	forceLastLen;

	double 	I[50 * SAMPLE_MAX_TIME];// 电流数组，用于绘制波形图。50为工频频率50Hz，也是工频的有效值在理论上的最高频率。
	double 	IIncrement;
	int    	ILen;
	double* ILastAddr;
	int    	ILastLen;

	double 	V[50 * SAMPLE_MAX_TIME];// 电压数组，用于绘制波形图。50为工频频率50Hz，也是工频的有效值在理论上的最高频率。
	double 	VIncrement;
	int    	VLen;
	double*	VLastAddr;
	int    	VLastLen;

	double 	VRef[SAMPLE_RATE * SAMPLE_MAX_TIME];	// 电压基准。保留采集到的原始数据
	double 	VRefIncrement;
	int    	VRefLen;
	double*	VRefLastAddr;
	int    	VRefLastLen;
} Waveform;
extern Waveform g_Waveform;
extern int g_ConvertedLen;

int SampleData2RawData(double sampleData[SAMPLE_CHANNEL][SAMPLE_RATE],int len);

#ifdef __cplusplus
	}
#endif
#endif
