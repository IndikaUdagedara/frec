//
// HEADER Logger 
// Logger.H
//

//
// Date: 2007-12-28
//


#ifndef LOGGER_HEADER
#define LOGGER_HEADER

#include <stdio.h>
#include "common.H"

class Logger
{

public:
	static Logger*& GetLogger();
	static int ChangeLogger(const char* zOutFile);
	static void RevertLogger(Logger* pOldLogger=NULL);
	static void CleanupLogger(Logger*& pLogger);

	int Init(FILE* pFD);
	int Init(const char* zOutFile);

	void Cleanup();
	void Log(int iLevel, char* zFormat, ...);

	static Logger* p_OldLogger;

private:
	Logger();
	~Logger();
	static Logger* p_Logger;

	FILE *p_Out;
	bool b_Init;
	
};

#endif // LOGGER_HEADER


