#ifndef _FREC_COMMON_
#define _FREC_COMMON_

#define FREC_VERSION	"0.1.0"


#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <list>
#include <sys/stat.h>
#include <sys/types.h>


#include <math.h>
#include <string>
#include <time.h>
#include <signal.h>

#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include <cvaux.h>

#include "Logger.H"

////////////////////////////////////////////////////////////////////////////////////////
// macros
#define QUIT(expr, txt, status)\
	if(!expr)\
	{\
		fprintf(stderr, txt);\
		exit(status);\
	}while(0)
	

#define LOG(i, ...)\
        Logger::GetLogger()->Log(i, __VA_ARGS__)

#define LOGGER() Logger::GetLogger()



////////////////////////////////////////////////////////////////////////////////////////
// defs

#define MAX_IMG_DIR     200
#define FILE_DATA_LEN	500
#define FILE_NAME_LEN   300

////////////////////////////////////////////////////////////////////////////////////////

#endif //_FREC_COMMON_
