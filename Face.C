//
// CLASS Face
// Face.cpp
//

//
// Date: 2007-09-23
//


#include "Face.H"
#include "Loader.H"


Face::Face(Person* pPerson)
{
	p_Person = pPerson;

	i_PrepLevel = -1;
	for(int i=0; i<PROCESSING_LEVELS; i++)
		pp_Processed[i] = NULL;

	p_UserData = NULL;
	b_Protected = false;
}

Face::Face()
{
	i_PrepLevel = -1;
	for(int i=0; i<PROCESSING_LEVELS; i++)
		pp_Processed[i] = NULL;

	p_UserData = NULL;
	b_Protected = false;

}

void Face::Reset()
{
	i_PrepLevel = -1;
	for(int i=0; i<PROCESSING_LEVELS; i++)
	{
		cvReleaseImage(&pp_Processed[i]);
		pp_Processed[i] = NULL;
	}
	p_UserData = NULL;
	b_Protected = false;
}


CvMat* Face::GetMat(int iMode)
{
	CvMat* pMat = NULL;
	IplImage* pImg = GetPrepImage();	
	if (pImg)
		pMat = ConvertToMat(pImg, iMode);
	
	return pMat;
}

Face::~Face()
{
	Reset();
}


void Face::Load(IplImage* pImg)
{
	/* loaded already */
	if(i_PrepLevel != -1) return;

	i_PrepLevel++;
	pp_Processed[i_PrepLevel] = pImg;
	pImg->imageId = (void*)this;

}

void Face::Load(IplImage* pImg, const char* zFileName)
{
	Load(pImg);
	strncpy(z_FileName, zFileName, FILE_NAME_LEN);
}


IplImage* Face::GetPrepImage(int iLevel)
{
	IplImage* pImg = NULL;

	if(i_PrepLevel < 0)
		return NULL; 

	if(iLevel>=0 && iLevel<PROCESSING_LEVELS)
		pImg = pp_Processed[iLevel];
	else
		pImg = pp_Processed[i_PrepLevel];
			

	return pImg;
			
}

void Face::PreProcess(PreProcessor* pProcessor)
{
	if(i_PrepLevel<0 || i_PrepLevel >= (PROCESSING_LEVELS))
	{
		printf("Preprocess Level=%d(%d)\n", i_PrepLevel, PROCESSING_LEVELS);
		return;
	}
			
	IplImage *pImg = GetPrepImage();
	IplImage *pNewImg = pProcessor->Process(pImg);
	if(pNewImg)
	{
		i_PrepLevel++;	
		pp_Processed[i_PrepLevel] = pNewImg;
		pNewImg->imageId = (void*)this;
	}

}


////////////////////////////////////////////////////////////////////////////////////////
// Person
////////////////////////////////////////////////////////////////////////////////////////
Person::Person(int iID)
{
	p_UserData = NULL;
	o_Data.i_ID = iID;
}

Person::Person()
{
	p_UserData = NULL;
}

Person::~Person()
{
	Reset();
}

void Person::Reset()
{
	Face* p = NULL;
	
	while(!lst_Faces.empty())
	{
		p = lst_Faces.front();
		lst_Faces.pop_front();
		delete p;
	}

	p_UserData = NULL;
}

void Person::AddFace(Face* pFace)
{
	lst_Faces.push_back(pFace);
}


void Person::InitTrv()
{
	ite_Faces = lst_Faces.begin();
}

Face* Person::GetNext(bool bIgnoreProtected)
{
	Face* p = NULL;
	while (ite_Faces != lst_Faces.end())	
	{
		p = *ite_Faces;
		if (bIgnoreProtected && p->IsProtected())	
		{
			ite_Faces++;
			p = NULL;
		}
		else
		{
			ite_Faces++;
			break;
		}
	}

	return p;
}

int Person::GetCount(bool bIgnoreProtected)
{
	int iCount = 0;
	if (bIgnoreProtected)
	{
		InitTrv();
		while(GetNext(true))
		{
			iCount++;	
		}
	}
	else
	{
		iCount = lst_Faces.size(); 
	}	
	return iCount; 


}

////////////////////////////////////////////////////////////////////////////////////////
// FaceDB
////////////////////////////////////////////////////////////////////////////////////////
FaceDB::FaceDB()
{
}

FaceDB::~FaceDB()
{
	Reset();
}

Person* FaceDB::GetPerson(int iID)
{
	map<int, Person*>::iterator iter = map_Persons.find(iID);
	if ( iter == map_Persons.end())
		return NULL;
	
	return iter->second;
}

Person* FaceDB::FindOrCreatePerson(int iID)
{
	Person* p = GetPerson(iID);
	if (p == NULL)
	{
		p = new Person(iID);
		map_Persons[iID] = p;
	}

	return p;
}

int FaceDB::AddFace(const char* zFileName, int iID, IplImage* pImg)
{
	Person* pPerson = FindOrCreatePerson(iID);
	Face* pFace = new Face(pPerson);
	pFace->Load(pImg, zFileName);
	pPerson->AddFace(pFace);
	AddToQueue(pFace);
	return 1;	
}

void FaceDB::AddToQueue(Face* pFace)
{
	//i_Count++;
	//if (p_Head == NULL)
	//	p_Tail = p_Head = pFace;
	//else
	//{
	//	p_Tail->p_Next = pFace;
	//	p_Tail = pFace;
	//}

	//p_Tail->p_Next = NULL;

	vec_Face.push_back(pFace);
		
}

void FaceDB::Save(const char* zPrefix)
{
	string sOutDir, sList, sListFile, sImg;	

	Loader::GetConfig(zPrefix, sOutDir);
	Loader::GetConfig("ListFile", sList);
	sListFile = sOutDir + string("/") + sList;
	FILE *pOut = fopen(sListFile.c_str(), "w");
	if (pOut == NULL)
	{
		LOG(1, "fopen failed on %s\n", sListFile.c_str());
		return;
	}

	char zImg[FILE_NAME_LEN];
	Face* p = NULL;
	InitTrv();
	while((p = GetNext()))	
	{
		snprintf(zImg, FILE_NAME_LEN, "%d_%d__%s", 
				p->p_Person->GetData()->i_ID, p->GetPrepLevel(), p->GetFileName());
		sImg = sOutDir + string("/") + string(zImg);
		cvSaveImage(sImg.c_str(), p->GetPrepImage());
		fprintf(pOut, "%s , %d\n", zImg, p->p_Person->GetData()->i_ID);
		LOG(1, "Saved to [%s]\n", sImg.c_str());
	}
	fclose(pOut);
}


void FaceDB::InitTrv()
{
	ite_Face = vec_Face.begin();
}

Face* FaceDB::GetNext(bool bIgnoreProtected)
{
	Face* p = NULL;
	while (ite_Face != vec_Face.end())	
	{
		p = *ite_Face;
		if (bIgnoreProtected && p->IsProtected() )	
		{
			ite_Face++;
			p = NULL;
		}
		else
		{
			ite_Face++;
			break;
		}
	}
	
	return p;
}

int FaceDB::GetCount(bool bIgnoreProtected)
{ 
	int iCount = 0;
	if (bIgnoreProtected)
	{
		InitTrv();
		while(GetNext(true))
		{
			iCount++;	
		}
	}
	else
	{
		iCount = vec_Face.size(); 
	}	
	return iCount; 

}
void FaceDB::Reset()
{
	Person* p = NULL;
	map<int, Person*>::iterator iter = map_Persons.begin();
	while(iter != map_Persons.end())
	{
		p = iter->second;
		delete p;
		iter++;
	}
	map_Persons.clear();
}

void FaceDB::LogSummary()
{
	LOG(1, "----- FaceDB Summary -----\n");
	LOG(1, "Persons = %d\n", map_Persons.size());
	LOG(1, "Faces = %d(Total) %d(Usable)\n", GetCount(false), GetCount(true));
	
	Person* p = NULL;
	map<int, Person*>::iterator iter = map_Persons.begin();
	while ( iter != map_Persons.end())
	{
		p = iter->second;
		LOG(1, "Person [%d] : Faces = %d(Total) %d(Usable)\n", p->GetData()->i_ID, p->GetCount(false), p->GetCount(true));
		iter++;
	}
	LOG(1, "----- FaceDB Summary -----\n");

}


void FaceDB::ShowImages()
{
	InitTrv();
	Face* p = NULL;
	while ((p = GetNext()))
	{
		IplImage* f = p->GetPrepImage();	
		ShowImage(f, "w");
	}

}

