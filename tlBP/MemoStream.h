// MemoStream.h: interface for the CMemoStream class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMOSTREAM_H__A4672DD5_85AD_4B6C_B6F2_7974F1F47200__INCLUDED_)
#define AFX_MEMOSTREAM_H__A4672DD5_85AD_4B6C_B6F2_7974F1F47200__INCLUDED_
#include "../Public/TString.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/*
内存流模拟类
//#_S 2004-12-8 zlq
*/
class CMemoStream  
{
	unsigned char*m_pBuf;
	int m_nBufLen;
	int m_nGrowSize;

	int m_nCurPos;
	int m_nDataLength;
	
public:
	bool Load(const char *pSource);
	std::string Export();
	bool ReadInt(int &value);//扩展函数
	bool ReadDouble(double &value);//扩展函数
	bool WriteInt(int value);//扩展函数
	bool WriteDouble(double value);//扩展函数
	void SetLength(int nLen);
	bool LoadFile(CTString strFileName);
	bool SaveFile(CTString strFileName);
	void Reset();
	bool ReadString(CTString &strValue);//扩展函数
	bool WriteString(CTString strValue);//扩展函数
	DWORD GetPosition( );
	int GetLength();
	int Seek( int lOff );
	unsigned int Write( const void* lpBuf, unsigned int nCount );
	unsigned int Read( void* lpBuf, unsigned int nCount );
	CMemoStream(int nGrowSize = 1024);
	virtual ~CMemoStream();

protected:
	bool ReAlloc(int nAddBufLen);
};

#endif // !defined(AFX_MEMOSTREAM_H__A4672DD5_85AD_4B6C_B6F2_7974F1F47200__INCLUDED_)
