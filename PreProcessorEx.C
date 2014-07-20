//
// CLASS PreProcessorEx
// PreProcessorEx.C
//

//
// Date: 2007-09-28
//



#include "Loader.H"
#include "PreProcessorEx.H"

////////////////////////////////////////////////////////////////////////////////////////
// Image Scaler
////////////////////////////////////////////////////////////////////////////////////////

ImgScaler::ImgScaler()
{
	strcpy(z_Name, "Image Scaler");
	i_Width = 0;
	i_Height = 0;
	i_Depth = 0;
	i_Channels = 0;
}

int ImgScaler::Init()
{
	Loader::GetConfig("Scale_Width", i_Width);
	Loader::GetConfig("Scale_Height", i_Height);
	Loader::GetConfig("Scale_Depth", i_Depth);
	Loader::GetConfig("Scale_Channels", i_Channels);

	return 1;
}

IplImage* ImgScaler::Process(const IplImage* pImg)
{
	IplImage* pScaledImg = cvCreateImage(cvSize(i_Width, i_Height), i_Depth, i_Channels);
	cvResize(pImg, pScaledImg, CV_INTER_LINEAR);
	
	return pScaledImg;
}

void ImgScaler::Cleanup()
{}


////////////////////////////////////////////////////////////////////////////////////////
// Equalizer
////////////////////////////////////////////////////////////////////////////////////////

int Equalizer::Init()
{
	return 1;
}

/*
 * input image should be 8bit greyscale
 */
IplImage* Equalizer::Process(const IplImage* pImg)
{
	IplImage* pEqImg = cvCreateImage(cvGetSize(pImg), 8, 1);	
	cvEqualizeHist(pImg, pEqImg);	
	return pEqImg;
}

void Equalizer::Cleanup()
{

}
