

/// HIFN The main entry-point function.
int main (int argc, char *argv[])
{
	int DAQmxError = 0;
	TaskHandle task;
	/* initialize and load resources */
	if(InitCVIRTE (0, argv, 0)==0)
		return -1;

	RunGUI(task);

Error:
	return 0;
}
