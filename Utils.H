#ifndef _UTILS_H_
#define _UTILS_H_

#include "common.H"

#define PRINT_VAR(var, t) printf(#var " = " t "\n", var)

int ShowImage(const char* zFile, const char* zWnd);
int ShowImage(IplImage* pImg, const char* zWnd);

void NormalizeImg(IplImage* pSrc, IplImage* pDest, float fMin, float fMax);

void RemoveBlanks(char* zBuf, char* zOut);
void System(char* zFormat, ...);

#define CONV_MODE_DFL	0
#define CONV_MODE_ROW	1
#define CONV_MODE_COL	2

CvMat* ConvertToMat(IplImage* pImg, int iMode=0);

void PrintMat(CvMat* pMat, const char* zFileName);
int GetType(IplImage* pImg);
void FillMat(CvMat* pDstMat, int iRowNum, CvMat* pSrcMat);

template<class T>
void Avg(T** ppArray, T* pAvg, int iCount, int iDim)
{ 
	T t;
	for (int i=0; i<iDim; i++)
	{
		t = 0.0;
		for (int j=0; j<iCount; j++)
		{
			t += ppArray[j][i];
		}
		t = (T)t/iCount;	
		pAvg[i] = t;
	}
}

int WritePlot(float** ppArray , int iCount, int iDim, const char* zDataFile, const char* zPlotFile);
double CalcMahalanobis(float* pArr1, float* pArr2, int iSize);

void ResetImage(IplImage* pImg, int iVal);

#define IS_MUL_CHANNEL(t) ((t)>>CV_CN_SHIFT)

#endif //_UTILS_H_
