//
// HEADER Recognizer 
// Recognizer.H
//

//
// Date: 2007-12-31
//


#ifndef RECOGNIZER_HEADER
#define RECOGNIZER_HEADER

#include <stdio.h>
#include "common.H"
#include "Face.H"

enum Mode
{
	M_PRIMARY, 
	M_SECONDARY, 
};

class Recognizer
{

public:
	Recognizer()
	{
		p_DB = NULL;
		z_Name[0] = 0;
		z_DataDir[0] = 0;
	}
	virtual ~Recognizer(){}

	/*
	 * initialization
	 */
	virtual int Init(){ return -1;}
	virtual int OnPrimaryLoadUp(FaceDB* pDB){ return -1;}
	virtual void OnPrimaryCleanUp(){}


	/*
	 * general cleanup
	 */
	virtual void Cleanup(){}

	/*
	 * cleanup disk usage
	 */
	virtual void CleanupData(){}

	/*
	 * load internal structures during normal operation
	 */
	virtual int OnSecondaryLoadUp(FaceDB* pDB){ return -1;}
	virtual void OnSecondaryCleanUp(){}
		
	/*
	 * Recognize Face from FaceDB
	 */
	virtual Person* Recognize(Face* pFace) { return NULL; } 

	/*
	 * Run a self test
	 * full control is given
	 * system will exit after running this
	 */
	virtual void RunSelfTest(FaceDB* pDB){}

	/*
	 * I have a name too
	 */
	virtual const char* GetName(){ return (const char*)z_Name; }

	friend class Kernel;

protected:
	FaceDB *p_DB;
	char z_Name[30];
	char z_DataDir[1000];
	int i_Mode;
};

#endif // RECOGNIZER_HEADER


