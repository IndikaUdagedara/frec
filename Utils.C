
#include "Utils.H"
#include "_cxcore.h"



int ShowImage(const char* zFile, const char* zWnd)
{
	IplImage* pImg = cvLoadImage(zFile);
	int iRet = ShowImage(pImg, zWnd);
	cvReleaseImage(&pImg);
	return iRet;
}

int ShowImage(IplImage* pImg, const char* zWnd)
{
	cvNamedWindow(zWnd, 0);
        cvShowImage(zWnd, pImg);
        int iRet = cvWaitKey(); // after a key pressed, release data
        cvDestroyWindow(zWnd);
	return iRet;
}


/*
 * safely assume zBuf is 0 terminated
 * zOut has at least zBuf
 */
void RemoveBlanks(char* zBuf, char* zOut)
{
	int iLen = 0;
	char* zStart, *zEnd;
	zStart = zBuf;

	while(*zStart == ' ' || *zStart == '\t' )
	{
		zStart++;
	}
	
	zEnd = zBuf+strlen(zBuf)-1;
	while(*zEnd == ' ' || *zEnd == '\t' )
	{
		zEnd--;
	}

	iLen = zEnd - zStart + 1;
	strncpy(zOut, zStart, iLen);
	zOut[iLen] = 0;
		
}


void NormalizeImg(IplImage* pSrc, IplImage* pDest, float _fMin, float _fMax)
{
	float p=0.0, q=0.0, fMin = 1000000.0 , fMax = 0.0; 

	for(int y=0; y<pSrc->height; y++)
	{
	        for(int x=0;x<pSrc->width; x++)
		{
	                p = *((float*)(pSrc->imageData + pSrc->widthStep*y) + x);
	                if(p <= fMin) fMin = p;
	                if(p >= fMax) fMax = p;
	        }
	}
	
	// y = mx + c
	float m,c;
	m = (_fMax - _fMin)/(fMax - fMin);
	c = _fMin - (m*fMin);


	for(int y=0; y<pDest->height; y++)
	{
        	for(int x=0; x<pDest->width; x++)
		{
        	        p = *((float *)(pSrc->imageData + pSrc->widthStep*y) + x);
        	        q = m*p + c;
        	        ((uchar *)(pDest->imageData + pDest->widthStep*y))[x] = (uchar)nearbyintf(q);
        	}
	}

}
                   


void PrintMat(CvMat* pMat, const char* zFile)
{
	if (pMat == NULL) return; 

	int iType = cvGetElemType(pMat);
	if ( IS_MUL_CHANNEL(iType))
	{
		/* no intension to use multi channel images !! */
		LOG(1, "Dont print multi channel mat %d\n", iType);
		return;
	}

	FILE *fp = fopen(zFile, "w");
	if (fp == NULL)	
	{	
		LOG(1, "Error opening %s\n", zFile);
		return;
	}

	int iRows, iCols;
	iRows = pMat->rows;
	iCols = pMat->cols;

	unsigned char* c;
	short* s;
	int* i;
	float *f;
	double *d;
	for (int k=0; k<iRows; k++)
	{
		for (int j=0; j<iCols; j++)
		{
			/* ugly ??*/	
			if ( iType == CV_8U || iType == CV_8S )
			{
				c = pMat->data.ptr;
				fprintf(fp, "%d, ",  *(c + k*iCols + j));
			}
			else if (iType == CV_16U || iType == CV_16S ) 
			{
				s = pMat->data.s;
				fprintf(fp, "%d, ",  *(s + k*iCols + j));
			}
			else if ( iType == CV_32S )
			{
				i = pMat->data.i;
				fprintf(fp, "%d, ",  *(i + k*iCols + j));
			
			}
			else if ( iType == CV_32F )
			{
				f = pMat->data.fl;
				fprintf(fp, "%f, ",  *(f + k*iCols + j));
			}
			else if ( iType == CV_64F )
			{
				d = pMat->data.db;
				fprintf(fp, "%f, ",  *(d + k*iCols + j));
			}
			else
			{
				LOG(1, "Unknown mat type in PrintMat() %d\n", iType);
				return;
			}
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

//
// Convert Image to Mat
//
CvMat* ConvertToMat(IplImage* pImg, int iMode)
{
	if (pImg->nChannels != 1)
	{
		LOG(1, "Image channels=%d, not converting to mat\n", pImg->nChannels);
		return NULL;
	}

	CvMat *pMat = NULL;
	int iType = GetType(pImg);	

	int iHeight, iWidth, iWidthStep;
	if (iMode == CONV_MODE_ROW)
	{
		iHeight = 1;	
		iWidth = pImg->height * pImg->width;
		iWidthStep = pImg->widthStep * pImg->height;
	}
	else if ( iMode == CONV_MODE_COL)
	{
		iHeight = pImg->height * pImg->width;
		iWidth = 1;	
		iWidthStep = pImg->widthStep * pImg->height;
	}
	else
	{
		iHeight = pImg->height;
		iWidth = pImg->width;	
		iWidthStep = pImg->widthStep;
		
	}

	pMat = cvCreateMatHeader(iHeight, iWidth, iType);
        cvSetData(pMat, pImg->imageData, iWidthStep);
	return pMat;

}


//CvMat* GetMat(IplImage* pImg, int iMode)
//{
//	if (pImg->nChannels != 1)
//	{
//		LOG(1, "Image channels=%d, not converting to mat\n", pImg->nChannels);
//		return NULL;
//	}
//
//
//	//if (iMode == CONV_MODE_ROW)
//	//	iRows = 1;
//	//else if ( iMode == CONV_MODE_COL)
//	//	iRows = pImg->height * pImg->width;
//	//else
//	//	iRows = 0;
//
//	//cvReshape(&oMat, pMat, 0, iRows);
//	PrintMat(&oMat, "./temp.mat");
//	return pMat;
//}

int GetType(IplImage* pImg)
{
	int iDepth = icvIplToCvDepth( pImg->depth );	
	return CV_MAKETYPE( iDepth, pImg->nChannels );
}

// fill iRowNum of pDest with pSrc
// mats should match of course
void FillMat(CvMat* pDstMat, int iRowNum, CvMat* pSrcMat)
{
        unsigned char *pSrc, *pDst;
        float *pPos;
        int iSrcStep, iDstStep;
        CvSize oSrcSize, oDstSize;
        cvGetRawData(pSrcMat, &pSrc, &iSrcStep, &oSrcSize );
        cvGetRawData(pDstMat, &pDst, &iDstStep, &oDstSize );


        pPos = (float*)(pDst + iDstStep*iRowNum);
        for( int y=0; y<oSrcSize.height; y++ )
        {
                for( int x=0; x<oSrcSize.width; x++ )
                {
                        *pPos = (float)(pSrc + y*iSrcStep)[x];
                        pPos++;
                }
        }
}       


void System(char* zFormat, ...)
{
	char zBuf[1000];
	va_list ap;
        va_start(ap, zFormat);
	vsnprintf(zBuf, 1000, zFormat, ap);
        va_end(ap);

	LOG(1, "Executing \"%s\"\n", zBuf);
	system(zBuf);
}



int WritePlot(float** ppArray , int iCount, int iDim, const char* zDataFile, const char* zPlotFile)
{

	FILE *pData = fopen(zDataFile, "w");
	if (!pData) 
	{
		LOG(1, "fopen() failed on %s\n", zDataFile);
		return 0;
	}

	FILE *pPlot = fopen(zPlotFile, "w");
	if (!pPlot)
	{
		LOG(1, "fopen() failed on %s\n", zPlotFile);
		fclose(pData);
		return 0;
	}


	fprintf(pPlot, "plot \\\n");
	bool bPlot = false;
	for (int i=0; i<iDim; i++)
	{
		for (int j=0; j<iCount; j++)
		{
			if (!bPlot)
			{
				if (j>0) fprintf(pPlot, ", \\\n");
				fprintf(pPlot, "\"%s\" using 0:%d with linespoints", zDataFile, j+1);
			}
			float f = *(*(ppArray+j) + i);
			fprintf(pData, "%f\t", f);
		}
		bPlot = true;
		fprintf(pData, "\n");

	}

	fclose(pData);
	fclose(pPlot);
	return 1;
}



double CalcMahalanobis(float* pArr1, float* pArr2, int iSize)
{
	CvMat* pMat1 = cvCreateMatHeader(1, iSize, CV_32FC1);
	cvSetData(pMat1, pArr1, sizeof(float)*iSize);

	CvMat* pMat2 = cvCreateMatHeader(1, iSize, CV_32FC1); 
	cvSetData(pMat2, pArr2, sizeof(float)*iSize);

	CvMat* pCovMat, *pInvCovMat, *pAvgMat;
	CvMat** ppMat = new CvMat*[2];
	ppMat[0] = pMat1;
	ppMat[1] = pMat2;

	pAvgMat = cvCreateMat(1, iSize, CV_32FC1);
	pCovMat = cvCreateMat(iSize, iSize, CV_32FC1);
	pInvCovMat = cvCreateMat(iSize, iSize, CV_32FC1);

	cvCalcCovarMatrix((const CvArr**)ppMat, 2, pCovMat, pAvgMat, CV_COVAR_NORMAL);
	cvInvert(pCovMat, pInvCovMat, CV_SVD);
	double dDist = cvMahalonobis(pMat1, pMat2, pInvCovMat);

	cvReleaseMat(&pMat1);
	cvReleaseMat(&pMat2);
	cvReleaseMat(&pAvgMat);
	cvReleaseMat(&pAvgMat);
	cvReleaseMat(&pCovMat);
	cvReleaseMat(&pInvCovMat);
	delete [] ppMat;

	return dDist;
}

void ResetImage(IplImage* pImg, int iVal)
{
	memset(pImg->imageData, iVal, pImg->imageSize);
}
