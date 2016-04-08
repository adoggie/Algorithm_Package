// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "cheader.h"


//03 FF FF FF   100 ban
//03 FE FF 01   110 ban
//03 FE FF 02   120 ban


//##ModelId=3D0007FB0263
long CHeader::GetFieldPosition()
{
	return m_nFieldPosition;
}

//##ModelId=3D00081F0106
long CHeader::GetDataPosition()
{
	return m_nDataPosition;
}

//##ModelId=3D00082D03A6
long CHeader::GetDataSize()
{
	return m_nDataSize;
}

//##ModelId=3D0008490306
int CHeader::GetFieldCount()
{
	return m_nFieldCount;
}

int CHeader::GetRecordCount()
{
	return m_nRecordCount;
}


void CHeader::SetHeaderSize(long nHeaderSize)
{
	m_nHeaderLength = nHeaderSize;
}

void CHeader::SetFieldPosition(long nPos)
{
	m_nFieldPosition = nPos;
}

void CHeader::SetFieldCount(long nCount)
{
	m_nFieldCount = nCount;
}

void CHeader::SetDataPosition(long nPos)
{
    m_nDataPosition = nPos;
}

void CHeader::SetRecordCount(long nCount)
{
	m_nRecordCount = nCount;
}

void CHeader::SetTime(SYSTEMTIME tTime)
{
	m_tDate = tTime;
}

void CHeader::SetDataSize(long nDataSize)
{
	m_nDataSize = nDataSize;
}


//得到文件版本 100 110
int CHeader::GetFileVersion()
{
	/*
				if (m_nFileVersion == 0xFFFFFF03	)
					nVersion = 100;
				if (m_nFileVersion == 0x01FFFE03 )
					nVersion = 110;
				if (m_nFileVersion == 0x02FFFE03)
					nVersion = 120;
	*/
	if (m_nFileVersion == 0xFFFFFF03	)
		return 100;
	if (m_nFileVersion == 0x01FFFE03 )
		return 110;
	if (m_nFileVersion == 0x02FFFE03)
		return 120;
	return -1;
}

CHeader::CHeader()
{
//03 FF FF FF   100 ban
//03 FE FF 01   110 ban
//03 FE FF 02   120 ban
	m_nFileVersion = 0x02FFFE03;
	m_nReverse1 = 0;
	m_nReverse2 = 0;
	m_nReverse3 = 0;
}
