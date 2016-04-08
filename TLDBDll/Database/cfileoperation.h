// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CFILEOPERATION_3CF34B7D010D_INCLUDED
#define _INC_CFILEOPERATION_3CF34B7D010D_INCLUDED
#include "cdataset.h"
//#include "CIFDataSet.h"
#include "cheader.h"
#include "cfields.h"
#include "DriverBase.h"
#include "cfield.h"
#include "cpage.h"
class CFlatFile;


class CFileOperation 
: public CDriverBase
{
	CPages* m_pTempPages;//�����ļ�ʱ��
	TCLFile *m_pFile;
	BYTE*m_IOBuf;
	int m_nIOTotalLen;////////////	
public:
	bool SaveFile(void*pDialogData);
	int GetFileVersion();
	~CFileOperation();

	CFileOperation();
	

	//##ModelId=3CF444BC01B1
	CHeader m_Header;


	//##ModelId=3CF428D001DB
	bool IniReadFile(LPCTSTR lpszFileName, CFields* pFields);

	//##ModelId=3CF598B5018F
	bool IniSaveFile(LPCTSTR lpszFileName,CPages* pPages, CFields* pFields);

	//��ָ���ļ�¼�ſ�ʼ���������ݵ������У����������ݼ�ָ�빩
	//�ڲ���ȡ
	//##ModelId=3D00470A00CB
	int ReadRecords(CPage* pPage, CFromPos Flags, long RecNo, int Count);

	//�������д�������ݼ����浽�ļ���,�����ڸ����ļ�ͷ�Ĳ���?
	//Ϣ
	//##ModelId=3DCB247F01BD
	void SaveRecords(CPages* pPages);


protected:

};
#endif /* _INC_CFILEOPERATION_3CF34B7D010D_INCLUDED */

