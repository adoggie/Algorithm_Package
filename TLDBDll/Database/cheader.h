// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CHEADER_3CF4276801A1_INCLUDED
#define _INC_CHEADER_3CF4276801A1_INCLUDED
#define VER100 100
#define VER110 110
//##ModelId=3CF4276801A1
class CHeader 

{
public:
	CHeader();
	int GetFileVersion();
	
	void SetDataSize(long nDataSize);
	void SetTime(SYSTEMTIME tTime);
	void SetRecordCount( long nCount);
	void SetDataPosition(long nPos);
	void SetFieldCount(long nCount);
	void SetFieldPosition(long nPos);
	void SetHeaderSize(long nHeaderSize);
	//##ModelId=3CF42A070251

	//##ModelId=3D0007FB0263
	long GetFieldPosition();

	//##ModelId=3D00081F0106
	long GetDataPosition();

	//##ModelId=3D00082D03A6
	long GetDataSize();

	//##ModelId=3D0008490306
	int GetFieldCount();
	int GetRecordCount() ;
private:

	int m_nFileVersion;//文件版本.100 为1.0版本,  120 为1.20版本
	int m_nHeaderLength;
	long m_nFieldPosition;
	int m_nFieldCount;
	long m_nRecordCount;
	long m_nDataPosition;
	long m_nDataSize;
	SYSTEMTIME m_tDate;
	int m_nReverse1;
	int m_nReverse2;
	int m_nReverse3;
};

#endif /* _INC_CHEADER_3CF4276801A1_INCLUDED */

