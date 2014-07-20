//
// HEADER PreProcessorEx
// PreProcessorEx.H
//

//
// Date: 2007-09-28
//


#ifndef PRE_PROCESSOREX_HEADER
#define PRE_PROCESSOREX_HEADER

#include <stdio.h>
#include "common.H"
#include "PreProcessor.H"

class ImgScaler: public PreProcessor
{
public:
	ImgScaler();
	virtual ~ImgScaler(){}
	int Init();
	void Cleanup();
	virtual IplImage* Process(const IplImage*);

private:
	int i_Width;
	int i_Height;
	int i_Depth;
	int i_Channels;
};


class Equalizer: public PreProcessor
{

public:
	Equalizer()
	{
		strcpy(z_Name, "Equalizer");
	}

	virtual ~Equalizer(){}
	int Init();
	void Cleanup();
	virtual IplImage* Process(const IplImage*);

};
#endif // PRE_PROCESSOREX_HEADER


