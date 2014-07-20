//
// HEADER Loader 
// Loader.h
//

//
// Date: 2007-09-26
//


#ifndef LOADER_HEADER
#define LOADER_HEADER

#include <stdio.h>
#include "common.H"
#include "Face.H"
#include "FaceDetector.H"
#include "Utils.H"

using namespace std;
struct ImgDetail
{
	char z_File[FILE_NAME_LEN];
	int i_ID;
	// etc
};


class Loader
{

public:
	Loader();
	~Loader();

	/* configs */
	static int LoadConfigs(const char* zConfigFile);
	static int GetConfig(const char* zKey, int &riValue);
	static int GetConfig(const char* zKey, string &rsValue);

	/* Face DB */
	static int LoadImages(const char* zPrefix, bool bValidate, FaceDB* pDB); 

	static void LoadFromFile(const char* zFile, bool bValidate, FaceDB* pDB);

private:
	static int ExtractDetails(char* zBuf, ImgDetail* pDetail);
	static int ValidateFace(IplImage* pImg);
	static map<string, string>map_Configs;

};

#endif // LOADER_HEADER


