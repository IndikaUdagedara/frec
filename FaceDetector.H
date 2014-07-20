//
// HEADER FaceDetector 
// FaceDetector.h
//

//
// Date: 2007-09-26
//


#ifndef FACE_DETECTOR_HEADER
#define FACE_DETECTOR_HEADER

#include <stdio.h>
#include "common.H"

#define SCALE_PARAM	1.2 	/* 20% scale */

enum FaceDetectResult
{
	NO_FACES,
	MANY_FACES,
	ONE_FACE,
	ERROR_FACE
};

class FaceDetector
{

public:
	FaceDetector();
	~FaceDetector();
	int Init();	
	FaceDetectResult Process(IplImage* pImg);
	void Cleanup();

private:
	CvMemStorage* p_Storage;
	CvHaarClassifierCascade* p_Classifier;
	bool b_Init;

};

#endif // FACE_DETECTOR_HEADER


