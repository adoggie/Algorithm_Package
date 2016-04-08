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
	CPages* m_pTempPages;//保存文件时用
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

	//从指定的记录号开始，读入数据到缓存中，并返回数据集指针供
	//内部读取
	//##ModelId=3D00470A00CB
	int ReadRecords(CPage* pPage, CFromPos Flags, long RecNo, int Count);

	//根据所有传入的数据集保存到文件里,并在内更新文件头的部分?
	//息
	//##ModelId=3DCB247F01BD
	void SaveRecords(CPages* pPages);


protected:

};
#endif /* _INC_CFILEOPERATION_3CF34B7D010D_INCLUDED */

