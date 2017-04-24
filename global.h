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
#define SAMPLE_RATE 2500	 		// ����Ƶ�� (Hz)
#define SAMPLE_BUF_READ_TIME 0.1	// ������������ȡ���ʱ�� (s)
#define SAMPLE_MAX_TIME 25      	// ������ʱ�䡣(s)
#define SAMPLE_CHANNEL 4         	// ͨ����
		
/////////////////////////////////////////////////////////////////////		
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
extern Waveform g_Waveform;
extern int g_ConvertedLen;

int SampleData2RawData(double sampleData[SAMPLE_CHANNEL][SAMPLE_RATE],int len);

#ifdef __cplusplus
	}
#endif
#endif
