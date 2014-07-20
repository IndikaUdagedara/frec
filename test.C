#include <stdio.h>
#include "Utils.H"
#include "common.H"
#include <string>

using namespace std;

//#define IS_MUL_CHANNEL(i) ((i)>>CV_CN_SHIFT)

// load a saved image
//CvMat* p = (CvMat*)cvLoad("/home/indika/programming/frec_data/EigenData/avg_img.xml");
//IplImage oImg, *pImg, *pImg2;	
//pImg = cvGetImage(p, & oImg);
//pImg2 = cvCreateImage(cvSize(p->rows, p->cols), 8, 1);
//NormalizeImg(pImg, pImg2, 0.0, 255.0);
//ShowImage(pImg2, "w");
//template<class T>
//T* Avg(T** ppArray, int iCount, int iDim)
//{
//        T t;
//        float f = static_cast<float>(t);
//        double d = static_cast<double>(t);
//	return &t;
//}

class B
{};
int main(int arg, char** argv)
{
	//CvMat* p = (CvMat*)cvLoad("./avg.xml");
	//IplImage oImg, *pImg, *pImg2;	
	//pImg = cvGetImage(p, &oImg);
	//pImg2 = cvCreateImage(cvSize(p->rows, p->cols), 8, 1);
	//NormalizeImg(pImg, pImg2, 0.0, 255.0);
	//const char* zWnd ="wnd";

	//for (int i=0; i<5; i++)
	//{
	//	//zWnd = new char[10];
	//	//sprintf(zWnd, "wnd");
	//	cvNamedWindow(zWnd,  1);
        //	cvShowImage(zWnd, p);
        //	//int iRet = cvWaitKey(-1000); // after a key pressed, release data
	//	cvDestroyWindow(zWnd);


	//}
	//cvReleaseImage(&pImg2);
	//cvReleaseMat(&p);

	IplImage* pImg1 = cvCreateImage(cvSize(4, 1), 8, 1);
	IplImage* pImg2 = cvCreateImage(cvSize(4, 1), 8, 1);
	IplImage* pAvg = cvCreateImage(cvSize(4, 1), 8, 1);

	int i1[] = {2, 3, 4, 5};
	int i2[] = {4, 6, 8, 10};

	int iData = 0;
	for (int i=0; i<4; i++)
	{
		*(char*)(pImg1->imageData + i) = (char)i1[i];
		*(char*)(pImg2->imageData + i) = (char)i2[i];
	}
	//memcpy(pImg2->imageData, &i2, sizeof(int)*4);
	//for (int y=0; y<1; y++)
	//{
	//	for (int x=0; x<4; x++)
	//	{
	//		iData = (int)*(char*)(pImg1->imageData + pImg1->widthStep*y + x);
	//		printf("%d\n", iData);
	//		iData = 0;
	//	}
	//}
			
	IplImage* ppImgs[2];
	ppImgs[0] = pImg1;
	ppImgs[1] = pImg2;
//	int iRet = ImgAvg(pAvg, ppImgs, 2);

	for (int y=0; y<1; y++)
	{
		for (int x=0; x<4; x++)
		{
			iData = (int)*(char*)(pAvg->imageData + pAvg->widthStep*y + x);
			//printf("%d\n", iData);
			iData = 0;
		}
	}
	float p1[] = {1.0, 2.0, 3.0};
	float p2[] = {1.0, 1.5, 3.0};
	float p3[] = {1.0, 1.6, 3.0};
	float p4[] = {1.0, 4.0, 3.0};
	float p5[] = {2.0, 4.0, 6.0};
	float* pp[]  = {p1, p2};
	float* p = new float[3];
	//Avg<float>(pp, p, 2, 3);

	double dDist;
	dDist = CalcMahalanobis(p1, p1, 3);
	dDist = CalcMahalanobis(p1, p2, 3);
	dDist = CalcMahalanobis(p1, p3, 3);
	dDist = CalcMahalanobis(p1, p4, 3);
	dDist = CalcMahalanobis(p1, p5, 3);

	//WritePlot(pp, 2, 3, "./a.dat", "./a.p");
	//for (int i=0; i<3; i++)
	//	printf("%f, ", p[i]);

	return 0;
}
