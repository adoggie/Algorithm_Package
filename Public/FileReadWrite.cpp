// FileReadWrite.cpp: implementation of the CFileReadWrite class.
//
//////////////////////////////////////////////////////////////////////

//#include "StdAfx.h"
#include "FileReadWrite.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileReadWrite::CFileReadWrite()
{

}

CFileReadWrite::~CFileReadWrite()
{

}

BOOL CFileReadWrite::WriteStrToFile(CTString WriteFileName, std::string WriteStr)
{
	FILE *pSaveModel = fopen(WriteFileName, "w");
	if (pSaveModel == NULL)
	{
		return FALSE;
	}
	else
	{
		fprintf(pSaveModel, WriteStr.c_str());
		fclose(pSaveModel);
	}			
	return TRUE;
}

BOOL CFileReadWrite::ReadFileToStr(CTString ReadFileName, std::string &ReadStr)
{
	FILE *pReadModel = fopen(ReadFileName, "r");
	ReadStr = "0";
	if (pReadModel)
	{
		fseek(pReadModel,0,SEEK_END);
		int n = ftell(pReadModel);
		fseek(pReadModel,0,SEEK_SET);
		char *pch = new char[n];
		fread(pch,sizeof(char),n,pReadModel);
		fclose(pReadModel);
		ReadStr = std::string(pch);
		delete []pch;
		return TRUE;
	}
	else
		return FALSE;
}

//�Զ�������Ҫ������ļ���
CTString CFileReadWrite::GetSaveFileName(CTString OrgFileName, CTString &PlusStr)
{
	CTString strDataFile,strSaveFile,strTemp;
	strDataFile=OrgFileName;//pData->m_DataAccess.GetFileName();
	strTemp=strDataFile;
	
	int nIndex=strDataFile.ReverseFind('.');
	if(nIndex<0)  //����չ��������
	{
		strTemp+=PlusStr;
		strTemp+=".mkw"; 		//--scott 060922
		if(strTemp.ReverseFind('\\')<0) 
		{//��·���������򱣴�����ǰ·�������ڷ���������������½���δ�����ļ���
			strTemp=".\\"+strTemp;
		}
	}
	else
	{
		strTemp.Insert(nIndex,PlusStr.GetData());
	}
	
	strSaveFile=strTemp;
	//CFileFind FF;
	int nSuffix=1;
	char buffer[20];
	while(1)
	{
		FILE *hFile = fopen( strSaveFile.GetData(),"r" );

		if(hFile  != NULL )
		{	
			strSaveFile=strTemp;
			 _itoa(nSuffix,buffer,10);
			CTString strSuffix =buffer;			//--scott 060922
			strSaveFile.Insert(strSaveFile.ReverseFind('.'),
					strSuffix.GetData());
			nSuffix++;
			fclose(hFile);
		}
		else
			break;
	}							
	
	return strSaveFile;
}