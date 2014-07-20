//
// HEADER Recognizer 
// Recognizer.H
//

//
// Date: 2007-12-31
//


#ifndef EIGEN_METHOD_HEADER 
#define EIGEN_METHOD_HEADER 

#include <stdio.h>
#include "Recognizer.H"
#include "common.H"
#include "Face.H"



////////////////////////////////////////////////////////////////////////////////////////
// Eigen Method
////////////////////////////////////////////////////////////////////////////////////////

#define EIGEN_DIFF	0
enum Indices
{
	INDEX_FACE=0, 
	INDEX_EIG, 
	MAX_INDEX
};

class EigenMethod: public Recognizer
{

public:
	EigenMethod();
	virtual ~EigenMethod(){}

	int Init();
	void Cleanup();
	void CleanupData();

	int OnPrimaryLoadUp(FaceDB* pDB);
	void OnPrimaryCleanUp();

	int OnSecondaryLoadUp(FaceDB* pDB);
	void OnSecondaryCleanUp();

	Person* Recognize(Face* pFace); 

	void RunSelfTest(FaceDB* pDB);
	void RunSelfTest(Person* pPerson);

private:
	Person* Recognize(Face* pFace, const char* zOutputPath); 

	// Primary Loadup
	void CalcEigenMethod();
	void CalcEigenMethod_Cleanup();
	void SaveStructures();


	// Secondary Loadup
	int LoadStructures();
	void LoadStructures_Cleanup();

	void CalcAvgCoeffs();
	void CalcAvgCoeffs_Cleanup();
	void SavePlots(const char* zPath);

	// check if Face matches Person
	// self test
	void RunNormalTest(Person* pPerson, Face* pProtectedFace, const char* zOutputPath);

	int i_Width;
	int i_Height;

	int i_NumFaces;
	int i_NumEigens;

	IplImage** 	pp_EigenVects;
	float*		p_EigenVals;
	float**		pp_Coeffs;
	CvMat**		pp_AvgCoeffs;

	IplImage*  	p_AvgImg;
	CvMat**		pp_CoeffMat;

	int* pi_Data;
	char z_ImgDir[1000];

	int i_MaxSelfTest;
};

#endif // EIGEN_METHOD_HEADER 


