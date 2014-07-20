//
// CLASS Loader
// Loader.cpp
//

//
// Date: 2007-09-26
//


#include "Loader.H"
#include "YConfigFile.h"

using namespace std;

map<string, string>Loader::map_Configs;

extern FaceDetector gDetector;

Loader::Loader()
{}


Loader::~Loader()
{}

int Loader::LoadConfigs(const char* zConfigFile)
{
	YConfigFile oConfigs;
	QUIT(oConfigs.Load(zConfigFile), "LoadConfigs failed", 0);
	oConfigs.GetMap(map_Configs);
	return 1;
}

int Loader::GetConfig(const char* zKey, int &riValue)
{
	string sVal;
	if(!GetConfig(zKey, sVal))
		return 0;

	riValue = atoi(sVal.c_str());
	return 1; 
}

int Loader::GetConfig(const char* zKey, string &rsValue)
{
	map<string, string>::iterator iter = map_Configs.find(string(zKey));
	if(iter == map_Configs.end())
		return 0;

	rsValue = iter->second;
	return 1;
	
}


/*
 * img db is in 
 * ImgDir_n/files.list
 * 'file_name, id\n' format
 */
int Loader::LoadImages(const char* zPrefix, bool bValidate, FaceDB* pDB)
{
	string sImgDir, sImgName;
	char zBuf[20];
		
	for (int i=0; i<MAX_IMG_DIR; i++)
	{
		sprintf(zBuf, "%s_%d", zPrefix, i);
		if (GetConfig(zBuf, sImgDir))
		{
			LoadFromFile(sImgDir.c_str(), bValidate, pDB);
		}
		
	}
	return 1;
}

void Loader::LoadFromFile(const char* zDir, bool bValidate, FaceDB* pDB)
{
	IplImage* pImg = NULL;

	string sList, sImg, sFile = zDir;
	GetConfig("ListFile", sList);
	sFile += string("/") + sList;

	ifstream fList;
	fList.open(sFile.c_str());
	if (!fList)
	{
		LOG(1, "fopen failed on [%s]\n", sFile.c_str());
		return;
	}

	bool bSuccess = false;
	ImgDetail oDetail;	
	char zBuf[FILE_DATA_LEN];  
	LOG(1, "Loading from \"%s\"\n", sFile.c_str());
	while(!fList.eof())
	{
		fList.getline(zBuf, FILE_DATA_LEN);
		zBuf[FILE_DATA_LEN-1] = 0;
		if (ExtractDetails(zBuf, &oDetail))
		{

			sImg = string(zDir) + string("/") + string(oDetail.z_File);
			pImg = cvLoadImage(sImg.c_str(), 0);
			if (pImg)
			{
				if (bValidate && ValidateFace(pImg)==0)
				{
					bSuccess = false;
					cvReleaseImage(&pImg);
				}
				else
				{
					bSuccess = true;
				}
			}
			else
			{
				bSuccess = false;
			}
				
			if (bSuccess)	
				pDB->AddFace(oDetail.z_File, oDetail.i_ID, pImg);

			LOG(1, "Load [%d] [%s] ... %s\n", oDetail.i_ID, oDetail.z_File, bSuccess?"OK":"FAIL");
		}
	}
	
}

int Loader::ExtractDetails(char* zBuf, ImgDetail* pDetail)
{
	char *zToken, *zStr;
	char zClean[FILE_DATA_LEN];
	int iTokens = 0;
	zStr = zBuf;
        for(iTokens=0 ;; zStr=NULL, iTokens++)
        {
                zToken = strtok(zStr, ",");
                if (zToken == NULL)
			break;

		RemoveBlanks(zToken, zClean);
		if (strlen(zClean) == 0)
			return 0;	

		switch(iTokens)
		{
			case 0:
				strcpy(pDetail->z_File, zClean);
			break;

			case 1:
				pDetail->i_ID = atoi(zClean);
			break;

			default:	
				return 0; 
			break;
		}
        }

	if (iTokens == 0)
		return 0;

	return 1;
	
}

int Loader::ValidateFace(IplImage* pImg)
{
	FaceDetectResult e = gDetector.Process(pImg);
	//int i = ShowImage(pImg, "Faces");
	if (e != ONE_FACE )
		return 0;

	//if (i == 'y' || i == 'Y')
	//	return 1;

	return 1;
}

