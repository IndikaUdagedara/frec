//
// HEADER PreProcessor 
// PreProcessor.h
//

//
// Date: 2007-09-28
//


#ifndef FACE_PRE_PROCESSOR_HEADER
#define FACE_PRE_PROCESSOR_HEADER

#include <stdio.h>
#include "common.H"

class PreProcessor
{

public:
	PreProcessor(){}
	virtual ~PreProcessor(){}
	virtual int Init(){ return -1; };
	virtual void Cleanup(){};
	virtual IplImage* Process(const IplImage*) = 0;
	const char* GetName(){ return (const char*)z_Name; }

protected:
	char z_Name[30];

};


#endif // FACE_PRE_PROCESSOR_HEADER


