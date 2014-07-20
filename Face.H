//
// HEADER Face 
// Face.h
//

//
// Date: 2007-09-23
//


#ifndef FACE_HEADER
#define FACE_HEADER



#include "common.H"
#include "PreProcessor.H"

/* allow only no of processing levels */
#define PROCESSING_LEVELS	10

#define IGNR_PROTECTED	true
#define ACPT_PROTECTED	false

using namespace std;

struct PersonData
{
	string s_Name;
	int i_ID; /* should be unique */
};

class Face;

class Person
{

public:
	Person(int iID);
	Person();
	void Reset();
	~Person();
	
	inline PersonData* GetData(){return &o_Data;}
	friend class Face;
	friend class FaceDB;
	void AddFace(Face* pFace);

	void *p_UserData;

	void InitTrv();
	Face* GetNext(bool bIgnoreProtected=true);
	int GetCount(bool bIgnoreProtected=true);

private:
	PersonData o_Data;
	list<Face*>		lst_Faces;
	list<Face*>::iterator	ite_Faces;
};


class FaceDB
{
public:
	FaceDB();
	virtual ~FaceDB();
	void Reset();
	void Save(const char* zPrefix);

	Person* FindOrCreatePerson(int iID);
	Person* GetPerson(int iID);
	void LogSummary();

	int AddFace(const char* zFileName, int iID, IplImage* pImg);
	
	void ShowImages();

	void InitTrv();	
	Face* GetNext(bool bIgnoreProtected=true);
	int GetCount(bool bIgnoreProtected=true);

	map<int, Person*> map_Persons;

private:
	void AddToQueue(Face* pFace);
	vector<Face*>		vec_Face;
	vector<Face*>::iterator	ite_Face;

};

class Face
{

public:
	Face(Person* pPerson);
	Face();
	~Face();

	void Reset();
	void Load(IplImage* pImg);
	void Load(IplImage* pImg, const char* zFileName);
	void PreProcess(PreProcessor* pProcessor);

	CvMat* GetMat(int iMode);  /* get Mat from last preprocessed image */

	IplImage* GetPrepImage(int iLevel=-1);
	inline int GetPrepLevel(){ return i_PrepLevel; }
	Person*	p_Person;
	Face* p_Next;
	const char* GetFileName(){return (const char*)z_FileName; }
	void* p_UserData;

	bool IsProtected(){ return b_Protected; }
	void SetProtected(bool bProtected){ b_Protected = bProtected; }
	friend class Person;

private:
	char z_FileName[FILE_NAME_LEN];  /* base file name */
	IplImage* pp_Processed[PROCESSING_LEVELS];
	int i_PrepLevel;

	bool b_Protected;


};

#endif // FACE_HEADER


