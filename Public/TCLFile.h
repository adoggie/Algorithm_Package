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
����˵����
	����64λ�ļ���.����һ�����߶���ļ����
������ //#_S 2005-4-19 zlq
************************************************************************************/

typedef unsigned int  TPosType;

class TCLFile  
{
	//int m_hFileHandle ;//�ļ����
	FILE *m_hFile;
	CTString m_sFileName;

	void Init();
public:
	static const unsigned int modeCreate;//û���򴴽��ļ�
	static const unsigned int modeNoTruncate;//ԭ����,�����
	static const unsigned int modeRead ;//ֻ��
	static const unsigned int modeReadWrite;//��д
	static const unsigned int modeWrite;//ֻд


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
	int WriteString(std::string strText); //����д-add by xds 20060816
	int ReadString(std::string &strText); //���ж�-add by xds 20060816
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
#ifdef WINPLAT //  Windowsƽ̨ר��
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
#ifdef WINPLAT //  Windowsƽ̨ר��
	TCLFile &  operator >> (__int64 &nlpData);
	TCLFile &  operator >> (unsigned __int64 &unlpData);
#endif	
	TCLFile &  operator >> (float &fData);
	TCLFile &  operator >> (double &dData);

	static const TPosType tBegin ;    //��ʼ��
	static const TPosType tCurrent  ;      //��ǰλ��
	static const TPosType tEnd ;  //����λ��
private:
	bool CheckMode(UINT nOpenFlags,CTString &strMode);
};

#endif // !defined(AFX_TCLFILE_H__DEB22ADC_D5F3_42C5_9DBC_377ACBD355C5__INCLUDED_)
