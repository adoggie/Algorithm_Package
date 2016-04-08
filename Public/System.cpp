// System.cpp: implementation of the TCSystem class.
//
//////////////////////////////////////////////////////////////////////


#include "System.h"
#include <string>
#ifdef _UNIX //   Linuxƽ̨ר��
#include <stdlib.h>//linux 
#include <unistd.h>//linux 
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#endif
#include "Util.h"
// #include "..\Common\WordAnaly.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTString TCSystem::m_sIniPath = "";
CTString TCSystem::m_sTempPath = "";
TCSystem::TCSystem()
{

}

TCSystem::~TCSystem()
{

}

bool TCSystem::GetWorkPath(CTString &strPath)
{
	CTString sPath;
	GetModulePath(sPath);
	CTString str;
	sPath += "workpath.txt";
	/*
	ifstream inf(sPath.c_str());
	inf >> strPath;
	*/
	return true;
}
//�õ�ִ��ģ�����ڵ�·��
bool TCSystem::GetModulePath(CTString &strPath)
{
#ifdef WINPLAT
	//ȡ ·���ļ���
	char buffer[_MAX_PATH];
	HMODULE hHandle = NULL;
	if (strPath.GetLength() > 1)
	{
		hHandle = GetModuleHandle(strPath.GetData());
		if (hHandle == INVALID_HANDLE_VALUE)
			return false;
	}
	GetModuleFileName(hHandle, buffer,_MAX_PATH);
	std::string str = buffer;
	str = ConvertPath(str.c_str());
	str = str.substr(0,str.find_last_of("/"));
	strPath = str;
	//ȡ ���"/"֮ǰ��·��, ȥ�ļ���
#endif
	return true;
}

/************************************************************************************
����˵����
        ���ݿ⵽�����ת����
		ǰ�������ݿ��Ѿ�û��ȱʧֵ�ˡ�
���������
    [in] tb         : ����ı���࣬�����ݿ�
	[in] sFieldsList:Ҫת����������ֶ��б����ַ���ʾ���м�ָ�ſ����� �ո񣬶���
					�� "����  ���  ����"
	[out] mt         :����ľ���ԭ����Ϣ�����
���أ��Ƿ�ת���ɹ������û���ֶ����������п�ֵ���򷵻�false
������ //#_S 2005-4-15 zlq
************************************************************************************/
/*
bool TCSystem::TableToMatrix(TCTable &tb,CTString sFieldsList, TCMatrix<double> &mt)
{
	TCWordAnaly wa;
	wa.Analysis(sFieldsList);
	vector<int> nList;
	for (int i=0;i<wa.GetWordCount();i++)
	{//�����Ƿ����е��ֶζ������У������û�����У��򷵻�ʧ��
		int nPos = tb.FindPosByName(wa.GetWord(i));
		if (nPos == -1)
			return false;
		nList.push_back(nPos);
	}
	
	return TableToMatrix(tb,nList, mt);
}
*/
/************************************************************************************
����˵����
        ���ݿ⵽�����ת����
		ǰ�������ݿ��Ѿ�û��ȱʧֵ�ˡ�
���������
    [in] tb         : ����ı���࣬�����ݿ�
	[in] nFieldsList:Ҫת����������ֶ��б����Ϊ�������б�0��ʼ
	[out] mt         :����ľ���ԭ����Ϣ�����
���أ��Ƿ�ת���ɹ������û���ֶ����������п�ֵ���򷵻�false
������ //#_S 2005-4-18 zlq
************************************************************************************/
/*
bool TCSystem::TableToMatrix(TCTable &tb,vector<int> nFieldsList, TCMatrix<double> &mt)
{
	int nRecords = tb.GetRowNum();
	int nFields = tb.GetColNum();
	mt.Create(nRecords,nFields);
	for (int nRow = 0 ; nRow<nRecords; nRow++ )
	{
		for (int nCol = 0;nCol <nFields;nCol++)
		{
			double value;
			if (tb.GetValue(nRow,nCol,value)==false)
				return false;//ȱʧֵû�д���Ӧ���ȴ���
			mt.SetData(nRow,nCol,value);
		}
	}
	//nFieldsList.size();
	
	return true;
}
*/
/*
ĩβ�����ָ�����
*/
bool TCSystem::GetTempPath(CTString &strPath)
{
	if (m_sTempPath.GetLength())
	{
		strPath = m_sTempPath;
		return true;
	}
	//ȡ��ϵͳ��ʱ·��.
    char szTempPath[MAX_PATH];

#ifdef WINPLAT

    DWORD dwResult = ::GetTempPath(MAX_PATH, szTempPath);
	strPath = szTempPath;
	strPath = ConvertPath(strPath.GetData());
	return true;
    //assert(nResult);
#else
	strPath = "/tmp/";//===uc
#endif
	return false;
}

void TCSystem::GetTempFileName(CTString szTempPath, CTString PrefixString, CTString &sName)
{
	//ȡ��ϵͳ��ʱ·��.
	char szTempFile[MAX_PATH];

#ifdef WINPLAT

    //ȡ��Ψһ���ļ���
    UINT nResult = ::GetTempFileName(szTempPath.GetData(), _T("~TLTemp"),0,szTempFile);
	sName = szTempFile;
	sName = ConvertPath(sName.GetData());
    //assert(nResult);
#else
	clock_t clk = clock();
	time_t curt = time(0);
	sprintf(szTempFile,"/tmp/~TLTemp_%u_%u",curt,clk);
	sName = szTempFile;
#endif

	return ;
}
/***********************************************
�õ�markway.ini�ļ���������Ϣ
zlq
***********************************************/
bool TCSystem::GetProfileIntValue(CTString strSection, CTString strSectionKey, int &value)
{

#ifdef WINPLAT
	CTString strPath;
	if (m_sIniPath.GetLength())
		strPath = m_sIniPath + "Markway.ini";
	else
		strPath = "c:/Markway.ini";//===tempddsds 

	CTString strValue       = _T("");
	char inBuf[80];
	int rst =GetPrivateProfileString (strSection,strSectionKey, NULL, inBuf, 80, strPath); 
	value = atoi(inBuf);
	//strValue=inBuf;
	return (bool)rst;

#else
	CTString strPath;
	if (m_sIniPath.GetLength())
		strPath = m_sIniPath + "Markway.ini";
	else
		strPath = "/etc/Markway.ini";
	Properties ps = Properties::FromeIniFile(strPath.GetData());
	value = ps.intValue( strSection.GetData(),strSectionKey.GetData());
	return true;
#endif
	return false;
}
bool TCSystem::GetProfileStrValue(CTString strSection, CTString strSectionKey, CTString &value)
{
#ifdef WINPLAT
	CTString strPath;
	//GetModulePath(strPath);
	//strPath = strPath+"Markway.ini";
	if (m_sIniPath.GetLength())
		strPath = m_sIniPath + "Markway.ini";
	else
		strPath = "c:/Markway.ini";

	CTString strValue       = _T("");
	char inBuf[80];
	int rst =GetPrivateProfileString (strSection,strSectionKey, NULL, inBuf, 80, strPath); 
	value = inBuf;
	//strValue=inBuf;
	return (bool)rst;

#else //--UNIX PORT
	CTString strPath;
	if (m_sIniPath.GetLength())
		strPath = m_sIniPath + "Markway.ini";
	else
		strPath = "/etc/Markway.ini";
	Properties ps = Properties::FromeIniFile(strPath.GetData());
	value = ps.value( strSection.GetData(),strSectionKey.GetData());
	return true;
#endif
	return false;
}

void TCSystem::GetSystemTime(SYSTEMTIME *st)
{
	//===uc δ���
	GetLocalTime(st);
}
/*
���ݹ������ֵõ����������ڵ�dllģ�������Լ����̵�ַ���������̵�����

 ��������nProcType
1 ���ݴ���ģ�� 
typedef int (*pTrain)(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt, string VO, string &ppOutStruct,CResult **ppRst);
2 Ϊѵ����Ԥ��ģ��
typedef int (*pTrain)(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt, string VO, string &ppOutStruct,CResult **ppRst);
 */
//
bool TCSystem::GetModuleInfo(CTString strName, CTString &sDllName, CTString &sProcName, int &nProcType)
{
	CTString szPath("");
	//===uc
	if (GetProfileStrValue("DLLName",strName,sDllName)==false)
		return false;
	if (GetProfileStrValue("ProcName",strName,sProcName)==false)
		return false;
	if (GetProfileIntValue("ProcType",strName,nProcType)==false)
		return false;
#ifdef WINPLAT
	szPath = "tlBase.dll";
	TCSystem::GetModulePath(szPath);
	sDllName = szPath + "/" + sDllName;		
#else

#endif
	

	return true;
}

//·��ת��
CTString TCSystem::ConvertPath(const char *src)
{
	char *pDest = NULL;
	char *pRepl = "/";
	char sPath[_MAX_PATH];
	
	strcpy(sPath,src);
	while (pDest = strstr(sPath,"\\"))
	{
		*pDest = *pRepl;
	}
	return CTString(sPath);
}

void TCSystem::SetIniPath(CTString sFilePath)
{
	m_sIniPath = sFilePath;
}

void TCSystem::SetTempPath(CTString sFilePath)
{
	m_sTempPath = sFilePath;
}































































