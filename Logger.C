//
// CLASS Logger
// Logger.C
//

//
// Date: 2007-12-28
//


#include "Logger.H"

Logger* Logger::p_Logger = NULL;
Logger* Logger::p_OldLogger = NULL;

Logger::Logger()
{
	p_Out = NULL;
	b_Init = false;
}


Logger::~Logger()
{
	Cleanup();
}

void Logger::Cleanup()
{
	if (p_Out)
	{
		fclose(p_Out);
		p_Out = NULL;
		b_Init = false;
	}
}

Logger*& Logger::GetLogger()
{
	if (p_Logger == NULL)
		p_Logger = new Logger();

	return p_Logger;
}


void Logger::CleanupLogger(Logger*& pLogger)
{
	pLogger->Cleanup();
	delete pLogger;
	pLogger = NULL;
}

int Logger::ChangeLogger(const char* zOutFile)
{
	assert(p_OldLogger==NULL);

	p_OldLogger = p_Logger;
	p_Logger = NULL;
	return GetLogger()->Init(zOutFile);
}

void Logger::RevertLogger(Logger* pOldLogger)
{
	CleanupLogger(p_Logger);
	if (pOldLogger==NULL)
	{
		p_Logger = p_OldLogger;
		p_OldLogger = NULL;

	}
	else
	{
		p_Logger = pOldLogger;
	}
}

int Logger::Init(FILE* pFD)
{
	if (b_Init) return 1;

	p_Out = pFD;

	b_Init = true;
	return 1;
}

int Logger::Init(const char* zOutFile)
{
	printf("Log file %s\n", zOutFile);
	if (b_Init) return 1;

	p_Out = fopen(zOutFile, "w");
	if (p_Out == NULL) 
		return 0;

	b_Init = true;
	return 1;
}


void Logger::Log(int iLevel, char* zFormat, ...)
{
	va_list ap;
        va_start(ap, zFormat);
	
	if (p_Out==NULL || b_Init==false) 
		vprintf(zFormat, ap);
	else
        	vfprintf(p_Out, zFormat, ap);
        va_end(ap);
}
