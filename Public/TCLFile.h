// TCLFile.h: interface for the TCLFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCLFILE_H__DEB22ADC_D5F3_42C5_9DBC_377ACBD355C5__INCLUDED_)
#define AFX_TCLFILE_H__DEB22ADC_D5F3_42C5_9DBC_377ACBD355C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)
#include <string>
#include <vector>
//#include <iostream>
#include <algorithm>
//#include <fstream>
#include <fcntl.h>
#include "TString.h"
/************************************************************************************
功能说明：
	处理64位文件类.它由一个或者多个文件组成
制作： //#_S 2005-4-19 zlq
************************************************************************************/

typedef unsigned int  TPosType;

class TCLFile  
{
	//int m_hFileHandle ;//文件句柄
	FILE *m_hFile;
	CTString m_sFileName;

	void Init();
public:
	static const unsigned int modeCreate;//没有则创建文件
	static const unsigned int modeNoTruncate;//原来有,则不清空
	static const unsigned int modeRead ;//只读
	static const unsigned int modeReadWrite;//读写
	static const unsigned int modeWrite;//只写


	void SeekToBegin();
	void SeekToEnd();
	bool IsOpen();
	
	CTString GetFileName();
	INT64 Seek( INT64 lOff, TPosType nFrom = tBegin);
	INT64 GetPosition();
	void SetLength( INT64 dwNewLen );
	INT64 GetLength();
	int Write( const void* lpBuf, UINT nCount );
	UINT Read( void* lpBuf, UINT nCount );
	int WriteString(std::string strText); //按行写-add by xds 20060816
	int ReadString(std::string &strText); //按行读-add by xds 20060816
	void Close( );//|_O_TRUNC _O_CREAT| _O_RDWR
	bool Open( LPCTSTR lpszFileName, UINT nOpenFlags =TCLFile::modeCreate|TCLFile::modeReadWrite);
	TCLFile();
	TCLFile( LPCTSTR lpszFileName, UINT nOpenFlags );
	virtual ~TCLFile();

//	TCLFile &  operator << (CTStringArray &strData);
	TCLFile &  operator << (const CTString &strData);
	TCLFile &  operator << (char *pData);
	TCLFile &  operator << (unsigned char *pData);
	TCLFile &  operator << (char &cData);
	TCLFile &  operator << (unsigned char &cData);
	TCLFile &  operator << (bool &bData);
	TCLFile &  operator << (short int &nData);
	TCLFile &  operator << (unsigned short int &nData);
	TCLFile &  operator << (int &nData);
	TCLFile &  operator << (unsigned int &nData);
	TCLFile &  operator << (long &lData);
	TCLFile &  operator << (unsigned long &lData);
#ifdef WINPLAT //  Windows平台专用
	TCLFile &  operator << (__int64 &nlpData);
	TCLFile &  operator << (unsigned __int64 &unlpData);//===temp
#endif	
	TCLFile &  operator << (float &fData);
	TCLFile &  operator << (double &dData);

//	TCLFile &  operator >> (CTStringArray &strData);
	TCLFile &  operator >> (CTString &strData);
	TCLFile &  operator >> (char *pData);
	TCLFile &  operator >> (unsigned char *pData);
	TCLFile &  operator >> (char &cData);
	TCLFile &  operator >> (unsigned char &cData);
	TCLFile &  operator >> (bool &bData);
	TCLFile &  operator >> (short int &nData);
	TCLFile &  operator >> (unsigned short int &nData);
	TCLFile &  operator >> (int &nData);
	TCLFile &  operator >> (unsigned int &nData);
	TCLFile &  operator >> (long &lData);
	TCLFile &  operator >> (unsigned long &lData);
#ifdef WINPLAT //  Windows平台专用
	TCLFile &  operator >> (__int64 &nlpData);
	TCLFile &  operator >> (unsigned __int64 &unlpData);
#endif	
	TCLFile &  operator >> (float &fData);
	TCLFile &  operator >> (double &dData);

	static const TPosType tBegin ;    //开始处
	static const TPosType tCurrent  ;      //当前位置
	static const TPosType tEnd ;  //后面位置
private:
	bool CheckMode(UINT nOpenFlags,CTString &strMode);
};

#endif // !defined(AFX_TCLFILE_H__DEB22ADC_D5F3_42C5_9DBC_377ACBD355C5__INCLUDED_)
