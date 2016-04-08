// System.cpp: implementation of the TCSystem class.
//
//////////////////////////////////////////////////////////////////////


#include "System.h"
#include <string>
#ifdef _UNIX //   Linux平台专用
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
//得到执行模块所在的路径
bool TCSystem::GetModulePath(CTString &strPath)
{
#ifdef WINPLAT
	//取 路径文件名
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
	//取 最后"/"之前的路径, 去文件名
#endif
	return true;
}

/************************************************************************************
功能说明：
        数据库到矩阵的转换。
		前提是数据库已经没有缺失值了。
出入参数：
    [in] tb         : 输入的表格类，即数据库
	[in] sFieldsList:要转换到矩阵的字段列表，用字符表示，中间分割号可以是 空格，逗号
					如 "年龄  身高  体重"
	[out] mt         :输出的矩阵，原有信息会清空
返回：是否转换成功，如果没有字段名，或者有空值，则返回false
制作： //#_S 2005-4-15 zlq
************************************************************************************/
/*
bool TCSystem::TableToMatrix(TCTable &tb,CTString sFieldsList, TCMatrix<double> &mt)
{
	TCWordAnaly wa;
	wa.Analysis(sFieldsList);
	vector<int> nList;
	for (int i=0;i<wa.GetWordCount();i++)
	{//查找是否所有的字段都在其中，如果有没有其中，则返回失败
		int nPos = tb.FindPosByName(wa.GetWord(i));
		if (nPos == -1)
			return false;
		nList.push_back(nPos);
	}
	
	return TableToMatrix(tb,nList, mt);
}
*/
/************************************************************************************
功能说明：
        数据库到矩阵的转换。
		前提是数据库已经没有缺失值了。
出入参数：
    [in] tb         : 输入的表格类，即数据库
	[in] nFieldsList:要转换到矩阵的字段列表，存放为索引号列表，0开始
	[out] mt         :输出的矩阵，原有信息会清空
返回：是否转换成功，如果没有字段名，或者有空值，则返回false
制作： //#_S 2005-4-18 zlq
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
				return false;//缺失值没有处理，应该先处理
			mt.SetData(nRow,nCol,value);
		}
	}
	//nFieldsList.size();
	
	return true;
}
*/
/*
末尾包含分隔符号
*/
bool TCSystem::GetTempPath(CTString &strPath)
{
	if (m_sTempPath.GetLength())
	{
		strPath = m_sTempPath;
		return true;
	}
	//取得系统临时路径.
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
	//取得系统临时路径.
	char szTempFile[MAX_PATH];

#ifdef WINPLAT

    //取得唯一的文件名
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
得到markway.ini文件的配置信息
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
	//===uc 未完成
	GetLocalTime(st);
}
/*
根据过程名字得到，过程所在的dll模块名，以及过程地址，函数过程的类型

 过程类型nProcType
1 数据处理模型 
typedef int (*pTrain)(CDataInterface *pData, CEnvParam *pEnv, Interaction *itt, string VO, string &ppOutStruct,CResult **ppRst);
2 为训练和预测模型
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

//路径转换
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































































