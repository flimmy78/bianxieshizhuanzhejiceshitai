typedef enum{
	TM_START,
	TM_STOP,
	TM_CLR,	
}TIME_MEASURE;
void TimeMeasure(int tmID,TIME_MEASURE tm);
void SaveTimeMeasure(char *fileName);
