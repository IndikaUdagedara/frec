//
// CLASS Recognizer
// Recognizer.C
//

//
// Date: 2007-12-31
//


#include "EigenMethod.H"
#include "Loader.H"

////////////////////////////////////////////////////////////////////////////////////////
// Eigen Method
////////////////////////////////////////////////////////////////////////////////////////

/*
 * Let,
 * Fi(1, .. M) = i'th face image N2*1
 * A = 1/M(sum(Fi)) # average image
 * Ii = Fi - A
 * X [N2*M] image matrix
 *
 * we want to find eigen vectors & values of [X*X'] = V(N2*N2)
 * X'*X*vi = ui*vi
 * where vi[M*1] = i'th eigen vector of [X'*X] = W(M*M)
 * ui = i'th eigen value
 *
 * X*X*'X*vi = X*ui*vi 
 * 	=> X*vi are eigen vectors of [X*X']
 * 	=> ui are eigen values
 *
 *
 */
EigenMethod::EigenMethod()
{
	strcpy(z_Name, "Eigen Method");
	i_NumFaces = 0;
	i_NumEigens = 0;

	i_Width = 0;
	i_Height = 0;
	i_MaxSelfTest = 1;

	pp_EigenVects = NULL;
	p_EigenVals = NULL;
	p_AvgImg = NULL;
	pp_Coeffs = NULL;
	pp_CoeffMat = NULL;
	
	pi_Data = NULL;
}

int EigenMethod::Init()
{
	Loader::GetConfig("Scale_Width", i_Width);
        Loader::GetConfig("Scale_Height", i_Height);	

	string sDataDir;
        Loader::GetConfig("EigenMethod_DataDir", sDataDir);	
        Loader::GetConfig("MaxSelfTest_Count", i_MaxSelfTest);	

	strcpy(z_DataDir, sDataDir.c_str());
	sprintf(z_ImgDir, "%s/Imgs", z_DataDir);
	mkdir(z_ImgDir, 0777);
	return 1;
}

int EigenMethod::OnPrimaryLoadUp(FaceDB* pDB)
{
	p_DB = pDB;
	i_NumFaces = p_DB->GetCount();
	i_NumEigens = i_NumFaces - EIGEN_DIFF; 

	LOG(1, "[%s] Faces = %d, Eigens = %d\n", GetName(), i_NumFaces, i_NumEigens);
	if (i_NumEigens <= 0) return 0;

	//
	// everything in eigen method
	//
	CalcEigenMethod();

	
	//
	// save data to use at secondary loadup
	//
	SaveStructures();
	return 1;
}

void EigenMethod::OnPrimaryCleanUp()
{
	CalcEigenMethod_Cleanup();
}


void EigenMethod::CalcEigenMethod()
{
	//
	// setting up memory
	//
       	IplImage** ppImgs = new IplImage*[i_NumFaces];
	pp_EigenVects = new IplImage*[i_NumFaces];
	p_EigenVals = new float[i_NumFaces];

	Face* pFace = NULL;
	p_DB->InitTrv();
        for (int i=0; i<i_NumFaces; i++)
	{
		pFace = p_DB->GetNext();
                ppImgs[i] = pFace->GetPrepImage();
		pp_EigenVects[i] = cvCreateImage(cvSize(i_Width, i_Height), IPL_DEPTH_32F, 1);
		ResetImage(pp_EigenVects[i], 0);
        }

	pp_Coeffs = new float*[i_NumFaces];
	for (int i=0; i<i_NumFaces; i++)
	{
		pp_Coeffs[i] = new float[i_NumFaces];
	}



	//
	// doing eigen calculations
	//
	CvTermCriteria mCriteria;
        mCriteria.type = CV_TERMCRIT_ITER|CV_TERMCRIT_EPS;
        mCriteria.max_iter = 10;
        mCriteria.epsilon = 0.1;

        p_AvgImg = cvCreateImage(cvSize(i_Width, i_Height), IPL_DEPTH_32F, 1);
        cvCalcEigenObjects(i_NumFaces, ppImgs, pp_EigenVects, 0, 0, 0, &mCriteria, p_AvgImg, p_EigenVals);


	//
	// calculate coeffiecients
	// and keep in face->p_UserData
	// 
	p_DB->InitTrv();
	for(int i=0; i<i_NumFaces; i++)
	{
                cvEigenDecomposite(ppImgs[i], i_NumFaces, pp_EigenVects, 0, 0, p_AvgImg,  pp_Coeffs[i]);
		pFace = p_DB->GetNext();
		pFace->p_UserData = (void*)pp_Coeffs[i];

	}

	delete [] ppImgs;


}

void EigenMethod::CalcEigenMethod_Cleanup()
{
	//
	// cleanup storage
	//
	for (int i=0; i<i_NumFaces; i++)
	{
		cvReleaseImage(&pp_EigenVects[i]);
	}

	delete [] pp_EigenVects, pp_EigenVects = NULL;
	delete [] p_EigenVals, p_EigenVals = NULL;

	for (int i=0; i<i_NumFaces; i++)
	{
		delete [] pp_Coeffs[i], pp_Coeffs[i] = NULL;
	}

	delete [] pp_Coeffs, pp_Coeffs = NULL;

	p_DB->InitTrv();
	Face* pFace;
	for(int i=0; i<i_NumFaces; i++)
	{
		pFace = p_DB->GetNext();
		pFace->p_UserData = NULL;
	}

	cvReleaseImage(&p_AvgImg);

}

void EigenMethod::SaveStructures()
{
	//
	// saving images and structures
	//
	char zBuf[500];
	IplImage* ppEigen8u = cvCreateImage(cvSize(i_Width, i_Height), IPL_DEPTH_8U, 1);
	ResetImage(ppEigen8u, 0);

	for (int i=0; i<i_NumFaces; i++)
	{
		// save eigens as images	
		NormalizeImg(pp_EigenVects[i], ppEigen8u, 0, 255);

		sprintf(zBuf, "%s/eigen%d.jpeg", z_ImgDir, i);
		cvSaveImage(zBuf, ppEigen8u);

		// save eigens as raw
		sprintf(zBuf, "%s/eigen%d.xml", z_DataDir, i);
		cvSave(zBuf, pp_EigenVects[i]);

	}
	cvReleaseImage(&ppEigen8u);


	// save avg as img
	IplImage* pAvgImg8u = cvCreateImage(cvSize(i_Width, i_Height), IPL_DEPTH_8U, 1); 
	NormalizeImg(p_AvgImg, pAvgImg8u, 0, 255);
	sprintf(zBuf, "%s/avg.jpeg", z_ImgDir);
	cvReleaseImage(&pAvgImg8u);

	// save avg as raw 
	sprintf(zBuf, "%s/avg.xml", z_DataDir);
	cvSave(zBuf, p_AvgImg);
	

	CvMat mMat;	

	// save eigen vals
	cvInitMatHeader(&mMat, 1, i_NumFaces, CV_32FC1, p_EigenVals);
	sprintf(zBuf, "%s/eigenval.xml", z_DataDir);
	cvSave(zBuf, &mMat);

	// save coefficients
	for (int i=0; i<i_NumFaces; i++)
	{
		cvInitMatHeader(&mMat, i_NumFaces, 1, CV_32FC1, pp_Coeffs[i]);
		sprintf(zBuf, "%s/coeffs%d.xml", z_DataDir, i);
		cvSave(zBuf, &mMat);
	}

	// writing other data
	pi_Data = new int[MAX_INDEX];
	pi_Data[INDEX_FACE] = i_NumFaces;
	pi_Data[INDEX_EIG] = i_NumEigens;
	cvInitMatHeader(&mMat, 1, MAX_INDEX, CV_32S, pi_Data);
	sprintf(zBuf, "%s/data.xml", z_DataDir);
	cvSave(zBuf, &mMat);

	delete [] pi_Data, pi_Data = NULL;

}

int EigenMethod::LoadStructures()
{
	char zBuf[200];
	sprintf(zBuf, "%s/data.xml", z_DataDir);

	//
	// Loading from files
	CvMat* p = (CvMat*)cvLoad(zBuf);
	if (p == NULL)
	{
		LOG(1, "Data file not avaiable %s\n", zBuf);
		return 0;
	}
	pi_Data = p->data.i;
	i_NumFaces = pi_Data[INDEX_FACE];
	i_NumEigens = pi_Data[INDEX_EIG];
	LOG(1, "[%s] Faces = %d, Eigens = %d\n", GetName(), i_NumFaces, i_NumEigens);
	cvRelease((void**)&p);

	IplImage oImg;
	pp_EigenVects = new IplImage*[i_NumFaces];
	for (int i=0; i<i_NumFaces; i++)
	{
		sprintf(zBuf, "%s/eigen%d.xml", z_DataDir, i);
		p = (CvMat*)cvLoad(zBuf);	
		if (p == NULL)
		{
			LOG(1, "Data file not avaiable %s\n", zBuf);
			return 0;
		}
		pp_EigenVects[i] = cvGetImage(p, &oImg);
	}


	sprintf(zBuf, "%s/avg.xml", z_DataDir);
	p = (CvMat*)cvLoad(zBuf);	
	p_AvgImg = cvGetImage(p, &oImg);

	pp_Coeffs = new float*[i_NumFaces];
	pp_CoeffMat = new CvMat*[i_NumFaces];

	Face* pFace = NULL;
	p_DB->InitTrv();	
	for (int i=0; i<i_NumFaces; i++)
	{
		sprintf(zBuf, "%s/coeffs%d.xml", z_DataDir, i);
		pp_CoeffMat[i] = (CvMat*)cvLoad(zBuf);	
		pp_Coeffs[i] = pp_CoeffMat[i]->data.fl;

		pFace = p_DB->GetNext();
		assert(pFace); // something gone wrong in loadup
		pFace->p_UserData = (void*)pp_Coeffs[i];

	}

	return 1;	

}

void EigenMethod::LoadStructures_Cleanup()
{
	//
	// cleanup
	//
	for (int i=0; i<i_NumFaces; i++)
	{
		cvReleaseImage(&pp_EigenVects[i]);
		cvRelease((void**)&pp_CoeffMat[i]);
	}

	cvReleaseImage(&p_AvgImg);

	delete [] pp_EigenVects, pp_EigenVects = NULL;
	delete [] pp_Coeffs, pp_Coeffs = NULL;
	delete [] pp_CoeffMat, pp_CoeffMat = NULL;
}

void EigenMethod::Cleanup()
{
		
}


//
// it is assumed that after initializing recognizer and database
// the db files are not manually edited
// so they are loaded as they were in normal operation
// we load in severel steps to save memory
//

int EigenMethod::OnSecondaryLoadUp(FaceDB* pDB)
{
	p_DB = pDB;
	LoadStructures();	

	CalcAvgCoeffs();
	SavePlots(z_DataDir);

	return 1;
}

// save data for gnuplot script
void EigenMethod::SavePlots(const char* zPath)
{
	//
	// saving all coefficients
	//
	char zBuf[500];

	FILE* pData = NULL;
	FILE* pPlot = NULL;
	FILE* pAvg = NULL;
	FILE* pAvgPlot = NULL;

	Person* pPerson = NULL;
	Face* pFace = NULL;
	float* pCoeffs = NULL;
	float* pAvgCoeff = NULL;


	sprintf(zBuf, "%s/person_avg.p", zPath);
	pAvgPlot = fopen(zBuf, "w");
	if (pAvgPlot == NULL)	
		LOG(1, "fopen() failed on %s\n", zBuf);

	if (pAvgPlot)
		fprintf(pAvgPlot, "plot \\\n");

	map<int, Person*>::iterator it = p_DB->map_Persons.begin();
	while(it != p_DB->map_Persons.end() )
	{
		sprintf(zBuf, "%s/person_%d_coeff.dat", zPath, it->first);
		pData = fopen(zBuf, "w");
		if (pData == NULL)	
		{
			LOG(1, "fopen() failed on %s\n", zBuf);
			continue;
		}


		sprintf(zBuf, "%s/person_%d_plot.p", zPath, it->first);
		pPlot = fopen(zBuf, "w");
		if (pPlot == NULL)	
		{
			LOG(1, "fopen() failed on %s\n", zBuf);
			fclose(pData);
			continue;

		}

		sprintf(zBuf, "%s/person_%d_avg.dat", zPath, it->first);
		pAvg = fopen(zBuf, "w");
		if (pAvg == NULL)	
		{
			LOG(1, "fopen() failed on %s\n", zBuf);
			fclose(pData);
			fclose(pPlot);
			continue;

		}

		pPerson = it->second;
		fprintf(pPlot, "plot\t\\\n");
		bool bPlot = false;
		for (int i=0; i<i_NumFaces; i++)
		{
			//
			// output coeffs - person_XX_coeff.dat
			// plot script - person_XX_plot.p
			//
			int iCount=0;
			pPerson->InitTrv();
			while((pFace = pPerson->GetNext()) != NULL)
			{
				if (!bPlot)
				{
					iCount++;
					if (iCount>1) fprintf(pPlot, ",\\\n");
					fprintf(pPlot, "\"person_%d_coeff.dat\" using 0:%d with linespoints", it->first, iCount);
				}
				pCoeffs = (float*)(pFace->p_UserData);
				fprintf(pData, "\t%f", *(pCoeffs+i));
			}


			//
			// output average to all data file as last column - person_XX_coeff.data
			// and to a new file - person_XX_avg.data
			// plot script - person_avg.p
			//
			if (!bPlot)
			{
				iCount++;
				fprintf(pPlot, ",\\\n");
				fprintf(pPlot, "\"person_%d_coeff.dat\" using 0:%d with linespoints lw 4 lt 1", it->first, iCount);
			}
			pAvgCoeff = (float*)(pPerson->p_UserData);
			fprintf(pData, "\t%f", *(pAvgCoeff+i));
			fprintf(pData, "\n");
			fprintf(pAvg, "\t%f\n", *(pAvgCoeff+i));
			bPlot = true;
		}

		fclose(pData);
		fclose(pPlot);
		fclose(pAvg);

		
		if (pAvgPlot)
			fprintf(pAvgPlot, "\"person_%d_avg.dat\" using 0:1 with linespoints lw 2 title \"Person = %d\"", it->first, it->first);

		it++;

		if (it != p_DB->map_Persons.end())
			fprintf(pAvgPlot, ",\\\n");
	}

	if (pAvgPlot)
		fclose(pAvgPlot);
}


void EigenMethod::OnSecondaryCleanUp()
{
	CalcAvgCoeffs_Cleanup();
	LoadStructures_Cleanup();
}

Person* EigenMethod::Recognize(Face* pFace)
{
	return Recognize(pFace, z_DataDir);
}

Person* EigenMethod::Recognize(Face* pFace, const char* zOutputPath)
{
	static int iCount = 0;
	iCount++;

	IplImage* pImg = pFace->GetPrepImage();
	float* pCoeffs = new float[i_NumFaces];
	cvEigenDecomposite(pImg, i_NumFaces, pp_EigenVects, 0, 0, p_AvgImg,  pCoeffs);
	char zBuf[200];
	sprintf(zBuf, "%s/input_%d.dat", zOutputPath, iCount);
	FILE *fp = fopen(zBuf, "w");
	if (fp == NULL) LOG(1, "fopen() failed on %s\n", zBuf);
	LOG(1, "Input Img: Data file '%s'\n", zBuf);

	for (int i=0; i<i_NumFaces; i++)		
	{
		if (fp)
			fprintf(fp, "%f\n", *(pCoeffs+i));
			
	}	

	if (fp) fclose(fp);
	delete [] pCoeffs;

	sprintf(zBuf, "%s/avg_with_input_%d.p", zOutputPath, iCount);
	System("cp %s/person_avg.p %s", zOutputPath, zBuf);
	LOG(1, "Input Img: Plot script file '%s'\n", zBuf);

	fp = fopen(zBuf, "a");
	if (fp == NULL) LOG(1, "fopen() failed on %s\n", zBuf);
	fprintf(fp, ",\\\n\"input_%d.dat\" using 0:1 with linespoints lw 6", iCount);
	if (fp) fclose(fp);

	//float* f;
	//Person* p;
	//map<int, Person*>::iterator it = p_DB->map_Persons.begin();			
	//double dDist = 0.0;

	//LOG(1, "Mahalanobis distance with each Person's average coeff\n");
	//while(it != p_DB->map_Persons.end())
	//{
	//	p = it->second;
	//	f = (float*)(p->p_UserData);
	//	dDist = CalcMahalanobis(pCoeffs, f, 5);
	//	LOG(1, "dist %d = %.10f\n", it->first, dDist);
	//	it++;	
	//}
	return NULL;

}

void EigenMethod::CleanupData()
{
	System("/bin/rm -rf %s/*", z_DataDir);
}




void EigenMethod::CalcAvgCoeffs()
{
	Person* pPerson;
	Face* pFace;
	int iCount = 0;
	float** ppCoeffs;
	float* pAvg;
	map<int, Person*>::iterator it = p_DB->map_Persons.begin();

	while(it != p_DB->map_Persons.end())
	{
		pAvg = new float[i_NumFaces];
		pPerson = it->second;
		iCount = pPerson->GetCount();
		ppCoeffs = new float*[iCount];
		pPerson->InitTrv();
		int i=0;
		while((pFace = pPerson->GetNext()) != NULL)
		{
			ppCoeffs[i] = (float*)(pFace->p_UserData);
			i++;
		}
		
		Avg<float>(ppCoeffs, pAvg, iCount, i_NumFaces);
		pPerson->p_UserData = (void*)pAvg;
		
		it++;
		delete [] ppCoeffs;
	}
}

void EigenMethod::CalcAvgCoeffs_Cleanup()
{
	map<int, Person*>::iterator it = p_DB->map_Persons.begin();

	Person* pPerson = NULL;
	while(it != p_DB->map_Persons.end())
	{
		pPerson = it->second;
		delete [] (float*)(pPerson->p_UserData);
		pPerson->p_UserData = NULL;
		it++;
	}
}


//
// Run self test per DB
//
void EigenMethod::RunSelfTest(FaceDB* pDB)
{
	p_DB = pDB;

	map<int, Person*>::iterator it = p_DB->map_Persons.begin();
	int i=0; 
	while(it != p_DB->map_Persons.end())
	{
		RunSelfTest(it->second);
		it++;	
		//if (++i >= i_MaxSelfTest)
		//	break;
	}
}


//
// Run self test per person
//
void EigenMethod::RunSelfTest(Person* pPerson)
{
	int iID = pPerson->GetData()->i_ID;
	char zOutputPath1[500];
	char zOutputPath2[500];
	char zLog[500];

	sprintf(zOutputPath1, "%s/SelfTest_%d", z_DataDir, iID);
	System("/bin/mkdir %s", zOutputPath1, iID);
	sprintf(zLog, "%s/SelfTest.%d.log", zOutputPath1, iID);

	Logger::ChangeLogger(zLog);
	LOG(1, "Self Test: Person [%d]\n", iID);

	Face* pFace = NULL;
	Face* pProtectedFace = NULL;

	int iCount = pPerson->GetCount();

	LOG(1, "Person [%d] count=%d\n", pPerson->GetData()->i_ID, iCount);
	for (int i=1; i<=iCount; i++)
	{
		pPerson->InitTrv();
		int j = i;
		while(j--)
		{
			pFace = pPerson->GetNext();
			pProtectedFace = pFace;
		}
		// Run self test happily		
		// we have i'th face protected

		pProtectedFace->SetProtected(true);

		sprintf(zOutputPath2, "%s/Face_%d", zOutputPath1, i);
		System("/bin/mkdir %s", zOutputPath2);

		LOG(1, "======================================================================================\n");
		LOG(1, "Protected: %s : %d \n", pProtectedFace->GetFileName(), i);
		LOG(1, "Output: %s\n", zOutputPath2);
		LOG(1, "======================================================================================\n");

		RunNormalTest(pPerson, pProtectedFace, zOutputPath2);
		pProtectedFace->SetProtected(false);
	}

	Logger::RevertLogger();

}

void EigenMethod::RunNormalTest(Person* pPerson, Face* pProtectedFace, const char* zOutputPath)
{
	i_NumFaces = p_DB->GetCount();
	i_NumEigens = i_NumFaces - EIGEN_DIFF; 
	LOG(1, "Self Test: [%s] Faces = %d, Eigens = %d\n", GetName(), i_NumFaces, i_NumEigens);
	p_DB->LogSummary();
	CalcEigenMethod();
	CalcAvgCoeffs();
	SavePlots(zOutputPath);

	Recognize(pProtectedFace, zOutputPath);
	
	CalcAvgCoeffs_Cleanup();
	CalcEigenMethod_Cleanup();
}

