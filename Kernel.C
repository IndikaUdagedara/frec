//
// CLASS Kernel
// Kernel.C
//

//
// Date: 2007-12-28
//


#include "Kernel.H"
#include "FaceDetector.H"
#include "Face.H"
#include "Loader.H"
#include "EigenMethod.H"
#include "App.H"



FaceDetector 		gDetector;
Kernel::Kernel()
{
	i_PreProcessors = 0;
	for(int i=0; i<PROCESSING_LEVELS; i++)
		p_PreProcessor[i] = NULL;

	p_Recognizer = NULL;

}


bool Kernel::Register(PreProcessor* p)
{
	if (i_PreProcessors < PROCESSING_LEVELS)
		p_PreProcessor[i_PreProcessors++] = p;
	else
		return false;
	
	return true; 
}

bool Kernel::Register(Recognizer* p)
{
	if (p_Recognizer)
		return false;	

	p_Recognizer = p;
	return true;
}


Kernel::~Kernel()
{
}

int Kernel::Run(int iMode)
{
	if 	(iMode == MODE_INIT_DB) 	InitDB();
	else if (iMode == MODE_INIT_RECOGNIZER) InitRecognizer();
	else if (iMode == MODE_NORMAL)		NormalRun();
	else if (iMode == MODE_SELF_TEST)	SelfTest();
	else if (iMode == MODE_CLEANUP)		CleanData();
	else 	return 0; 


	return 1;
}




void Kernel::Cleanup()
{
	PreProcessor *pPreProcessor = NULL;
	for(int i=0; i<PROCESSING_LEVELS; i++)
	{
		pPreProcessor = p_PreProcessor[i];
		if (pPreProcessor)	
		{
			pPreProcessor->Cleanup();
			delete p_PreProcessor[i], p_PreProcessor[i] = NULL;
		}
	}

	gDetector.Cleanup();
	p_Recognizer->Cleanup();
	delete p_Recognizer, p_Recognizer = NULL;

	p_App->OnCleanup();

}

int Kernel::Init(App* pApp)
{
	p_App = pApp;
	p_App->OnInit(this);

	PreProcessor *pPreProcessor = NULL;

	if (! Loader::LoadConfigs("frec.conf") )
		return 0;

	gDetector.Init();
	for(int i=0; i<PROCESSING_LEVELS; i++)
	{
		pPreProcessor = p_PreProcessor[i];
		if (pPreProcessor)	
		{
			LOG(1, "-------- Initializing PreProcessor : [%s] --------\n", pPreProcessor->GetName());
			pPreProcessor->Init();
		}
	}

	LOG(1, "-------- Initializing Recognizer : [%s] --------\n", p_Recognizer->GetName());
	p_Recognizer->Init();	

	return 1;
}

int Kernel::InitDB()
{
	LOG(1, "-------- Loading Raw Images --------\n");
	Loader::LoadImages("RawImgDir", true, &o_FaceDB);

	LOG(1, "-------- PreProcessing Images --------\n");
	PreProcessDB(&o_FaceDB);

	//o_FaceDB.ShowImages();
	LOG(1, "-------- Saving Processed Images --------\n");
	o_FaceDB.Save("OutImgDir");
	o_FaceDB.LogSummary();
	return 1;
}

int Kernel::InitRecognizer()
{
	LOG(1, "-------- PreProcessing Images --------\n");
	Loader::LoadImages("ProcessedImgDir", false, &o_FaceDB);
	o_FaceDB.LogSummary();

	int iRet = p_Recognizer->OnPrimaryLoadUp(&o_FaceDB);
	LOG(1, "-------- Loading(I) Recognizer : [%s] -------- : %s\n", p_Recognizer->GetName(), iRet? "OK": "FAIL");
	p_Recognizer->OnPrimaryCleanUp();
	
	return iRet;
}

int Kernel::NormalRun()
{
	LOG(1, "-------- Loading Processed Images --------\n");
	Loader::LoadImages("ProcessedImgDir", false, &o_FaceDB);
	o_FaceDB.LogSummary();
	//o_FaceDB.ShowImages();

	LOG(1, "-------- Loading(II) Recognizer : [%s] --------\n", p_Recognizer->GetName());
	p_Recognizer->OnSecondaryLoadUp(&o_FaceDB);

	// go to run loop
	RunLoop(); // TODO
	p_Recognizer->OnSecondaryCleanUp();

	return 1;
}


void Kernel::PreProcessDB(FaceDB* pFaceDB)
{
	pFaceDB->InitTrv();
	Face* p = NULL;
	while( (p = pFaceDB->GetNext()))
	{
		PreProcess(p);
	}

}

void Kernel::PreProcess(Face* pFace)
{
	PreProcessor* pPreProcessor = NULL;
	
	for(int i=0; i<PROCESSING_LEVELS; i++)
	{
		pPreProcessor = p_PreProcessor[i];
		if (pPreProcessor == NULL) continue;
		
		pFace->PreProcess(pPreProcessor);
	}
}


// cleanup temp files, saved data structs etc
void Kernel::CleanData()
{
	string sOutDir;
	Loader::GetConfig("OutImgDir", sOutDir);
	System("/bin/rm -rf %s/*", sOutDir.c_str());
	p_Recognizer->CleanupData();
}

void Kernel::RunLoop()
{
	Person *p; 
	Face f;

	char zBuf[1000];
	IplImage *pImg;

	System("/usr/bin/clear");
	while(1)
	{
		printf("Enter file name or 'q' to quit, 'c' to clear\n");
		scanf("%s", zBuf);
		if (zBuf[0] == 'q')
		{
			break;
		}
		else if ( zBuf[0] == 'c')
		{
			System("/usr/bin/clear");
			continue;
		}

		pImg = cvLoadImage(zBuf, 0);
		if (pImg == NULL)
		{
			printf("Can't open %s\n", zBuf);
			continue;
		}
		FaceDetectResult e = gDetector.Process(pImg);
		if (e != ONE_FACE)
		{
			cvReleaseImage(&pImg);
			printf("Image contains many faces\n");
			continue;
		}	

		f.Load(pImg);
		PreProcess(&f);
		p = p_Recognizer->Recognize(&f);
		//cvReleaseImage(&pImg);
	}
}

int Kernel::SelfTest()
{
	LOG(1, "-------- Loading Processed Images --------\n");
	Loader::LoadImages("ProcessedImgDir", false, &o_FaceDB);
	o_FaceDB.LogSummary();

	LOG(1, "-------- Running Self Test of Recognizer : [%s] --------\n", p_Recognizer->GetName());
	p_Recognizer->RunSelfTest(&o_FaceDB);

	return 1;
}


void PrintRunModes()
{
	printf("\nMode\n");
	printf("%d = %s\n", MODE_INIT_DB, "Initialize Face Database");
	printf("%d = %s\n", MODE_INIT_RECOGNIZER, "Initialize Recognizer");
	printf("%d = %s\n", MODE_NORMAL, "Interactive mode");
	printf("%d = %s\n", MODE_SELF_TEST, "Recognizer Self Test");
	printf("%d = %s\n", MODE_CLEANUP, "Cleanup disk files");

}
