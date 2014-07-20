//
// CLASS FaceDetector
// FaceDetector.cpp
//

//
// Date: 2007-09-26
//


#include "FaceDetector.H"
#include "Loader.H"

using namespace std;

FaceDetector::FaceDetector()
{
	b_Init = false;
	p_Storage = NULL;
	p_Classifier = NULL;
}


FaceDetector::~FaceDetector()
{
	Cleanup();
}


int FaceDetector::Init()
{
	if (b_Init) return 1;

	string sClassifier;
	if (! Loader::GetConfig("Classifier", sClassifier) ) return 0;
	p_Storage = cvCreateMemStorage(0);
	p_Classifier = (CvHaarClassifierCascade*)cvLoad(sClassifier.c_str(), NULL, NULL, NULL);

	if(p_Classifier == NULL)
		return 0; 
	
	b_Init = true;
	return 1;
}

FaceDetectResult FaceDetector::Process(IplImage* pImg)
{
	FaceDetectResult eRes;
	CvSeq* pFaces = NULL;

        pFaces = cvHaarDetectObjects( 	pImg, 
					p_Classifier, 
					p_Storage, 
					SCALE_PARAM, // scale the cascade by 20% after each pass
        				2, // groups of 3 (2+1) or more neighbor face rectangles are joined into a single "face", smaller groups are rejected
        				CV_HAAR_DO_CANNY_PRUNING, // use Canny edge detector to reduce number of false alarms
        				cvSize(0, 0) // start from the minimum face size allowed by the particular classifier
        				);

	for(int i=0;i<(pFaces ? pFaces->total:0); i++ )
	{
		CvRect* r	= (CvRect*)cvGetSeqElem(pFaces, i);
		CvPoint pt1 	= { r->x, r->y };
		CvPoint pt2 	= { r->x + r->width, r->y + r->height };

		if (pFaces->total == 1) 
			cvSetImageROI(pImg, *r);
		else	
			cvRectangle(pImg, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
		
	}


	if(pFaces)
		eRes = (pFaces->total>1)?MANY_FACES:(pFaces->total==1)?ONE_FACE:NO_FACES;
	else
		eRes = ERROR_FACE;


	return eRes;
}


void FaceDetector::Cleanup()
{
	cvReleaseMemStorage(&p_Storage);
	cvReleaseHaarClassifierCascade(&p_Classifier);
}
