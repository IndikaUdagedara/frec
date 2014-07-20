#include <stdio.h>
#include "Logger.H"
#include "Kernel.H"
#include "App.H"


void PrintUsage(int argc, char** argv)
{
	printf("\nFREC %s: a little Face RECognition program\n", FREC_VERSION);
	printf("\nusage: %s Mode [logfile]\n", argv[0]);
	PrintRunModes();
}

int main(int argc, char** argv)
{

	if (argc != 2 && argc != 3)
	{
		PrintUsage(argc, argv);
		return 0;
	}

	Kernel oKernel;

	if (argc == 2)
		LOGGER()->Init(stdout);
	else
		LOGGER()->Init(argv[2]);


	App oApp;
	oKernel.Init(&oApp);

	int iMode = atoi(argv[1]);
	if (oKernel.Run(iMode))
	{
		printf("Normal exit. mode=%d\n", iMode);
	}
	else
	{
		printf("Abnormal exit. mode=%d", iMode);
		PrintRunModes();
	}
		

	oKernel.Cleanup();
	Logger::CleanupLogger(LOGGER());

	return 1;
}



