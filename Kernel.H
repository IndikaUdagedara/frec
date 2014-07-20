//
// HEADER Kernel 
// Kernel.H
//

//
// Date: 2007-12-28
//


#ifndef KERNEL_HEADER
#define KERNEL_HEADER

#include <stdio.h>
#include "common.H"
#include "PreProcessor.H" 
#include "Recognizer.H"
#include "Face.H"
#include "FaceDetector.H"

class App;


#define MODE_INIT_DB			0
#define MODE_INIT_RECOGNIZER		1
#define MODE_NORMAL			2
#define MODE_SELF_TEST			3
#define MODE_CLEANUP			5

void PrintRunModes();

class Kernel
{

public:
	Kernel();
	~Kernel();

	//
	// common to all modes	
	//
	int Init(App* pApp);
	void Cleanup();	

	int Run(int iMode);
		
	//
	// utilities preprocess
	// kernel contains preprocessors
	void PreProcess(Face* pFace);
	void PreProcessDB(FaceDB* pFaceDB);

	bool Register(PreProcessor* p);
	bool Register(Recognizer* p);

private:

	//
	// initialize database, do preprocessing images
	// allow for manual intervention to change db after exit
	//
	int InitDB();

	//
	// training mode for recognizer
	// train and save required data(if required) for normal mode
	//
	int InitRecognizer();

	//
	// normal run mode
	//
	int NormalRun();

	//
	// Recognizer Self Test
	// pass Recognizer with db
	// it must not modify saved DB, but its own data
	// after running will have to 
	// -> CleanupData
	// -> InitDB
	// -> InitRecognizer
	// in order to go to normal run mode
	int SelfTest();


	//
	// cleanup mode
	//
	void CleanData();

	// use for normal mode
	void RunLoop();

	PreProcessor*		p_PreProcessor[PROCESSING_LEVELS];
	Recognizer*		p_Recognizer;		
	FaceDB 			o_FaceDB;
	int 			i_PreProcessors;
	App			*p_App;
};

#endif // KERNEL_HEADER


