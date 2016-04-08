// MemoStream.cpp: implementation of the CMemoStream class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "MemoStream.h"
#include "../Public/TCLFile.h"
#include <string>

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemoStream::CMemoStream(int nGrowSize )
{
	m_nGrowSize = nGrowSize;
	m_nBufLen =  1024*4;
	m_pBuf = new unsigned char[m_nBufLen];

	m_nCurPos = 0;
	m_nDataLength = 0;

}

CMemoStream::~CMemoStream()
{
	delete []m_pBuf;
}

unsigned int CMemoStream::Read(void *lpBuf, unsigned int nCount)
{
	if (GetPosition()+nCount > GetLength() )
	{
		nCount = GetLength() - GetPosition();
	}
	memcpy(lpBuf,m_pBuf+GetPosition(),nCount);
	m_nCurPos = m_nCurPos + nCount;
	return nCount;
}

unsigned int CMemoStream::Write(const void *lpBuf, unsigned int nCount)
{
	if (GetPosition()+nCount < m_nBufLen )
	{
		memcpy(m_pBuf+GetPosition(),lpBuf,nCount);
		m_nCurPos = m_nCurPos + nCount;
		if (m_nDataLength <= m_nCurPos)//调整最大长度
			m_nDataLength = m_nCurPos+1;
	}
	else
	{
		ReAlloc(m_nGrowSize);
		Write(lpBuf, nCount);
	}
	return nCount;
}

int CMemoStream::Seek(int lOff)
{
	if (lOff>=m_nDataLength)
		lOff = m_nDataLength-1;
	m_nCurPos = lOff;
	return lOff;
}
//得到数据长度
int CMemoStream::GetLength()
{
	return m_nDataLength;
}
//得到当前位置
DWORD CMemoStream::GetPosition()
{
	return m_nCurPos;
}

bool CMemoStream::ReAlloc(int nAddBufLen)
{
	unsigned char *pBuf = new unsigned char[m_nBufLen + nAddBufLen];
	memcpy(pBuf,m_pBuf,m_nBufLen);
	m_nBufLen +=  nAddBufLen;
	delete []m_pBuf;
	m_pBuf = pBuf;
	return true;
}

bool CMemoStream::WriteString(CTString strValue)
{
	int nLen = strValue.GetLength();
	if (Write(&nLen,sizeof(int)) != sizeof(int))
		return false;
	if (Write((const char*)strValue,nLen) != nLen)
		return false;
	return true;
}

bool CMemoStream::ReadString(CTString &strValue)
{
	char *pBuf = new char[256];
	int nLen = 0;
	Read(&nLen,sizeof(int));
	memset(pBuf,0,nLen+2);
	if (Read(pBuf,nLen) != nLen)
	{
		strValue = pBuf;
		delete [] pBuf;
		return false;
	}
	strValue = pBuf;
	delete [] pBuf;
	return true;
}

void CMemoStream::Reset()
{
	m_nCurPos = 0;
	m_nDataLength = 0;
}

bool CMemoStream::SaveFile(CTString strFileName)
{
	try
	{
		TCLFile cf;
		cf.Open(strFileName,TCLFile::modeCreate|TCLFile::modeReadWrite);
		cf.Write(m_pBuf,m_nDataLength);
	}
	catch(...)
	{
		return false;
	}
	return true;
}

bool CMemoStream::LoadFile(CTString strFileName)
{
	try
	{
		Reset();
		TCLFile cf;
		cf.Open(strFileName,TCLFile::modeCreate|TCLFile::modeReadWrite);
		if (m_nBufLen<cf.GetLength())
		{
			m_nBufLen = cf.GetLength()+1024;
			delete []m_pBuf;
			m_pBuf = new BYTE[m_nBufLen];
		}
		cf.Read(m_pBuf,cf.GetLength());
		SetLength(cf.GetLength());//设置数据长度
		
	}
	catch(...)
	{
		return false;
	}
	return true;
}

bool CMemoStream::Load(const char *pSource)
{
	Reset();
	if (m_nBufLen< strlen(pSource))
	{
		m_nBufLen =  strlen(pSource)+1024;
		delete []m_pBuf;
		m_pBuf = new BYTE[m_nBufLen];
		memset(m_pBuf,0,m_nBufLen);
	}
	memcpy(m_pBuf,pSource,strlen(pSource));
	SetLength(strlen(pSource));//设置数据长度
	return true;
}

std::string CMemoStream::Export()
{
	std::string strOut;
	strOut = (char *)m_pBuf;
	return strOut;
}

void CMemoStream::SetLength(int nLen)
{
	if (m_nBufLen <nLen)
	{
		m_nBufLen = nLen;
		delete []m_pBuf;
		m_pBuf = new unsigned char[m_nBufLen];
	}
	m_nDataLength = nLen;
}

bool CMemoStream::WriteDouble(double value)
{
	if (Write(&value,sizeof(double)) == sizeof(double))
		return true;
	return false;
}

bool CMemoStream::WriteInt(int value)
{
	if (Write(&value,sizeof(int)) == sizeof(int))
		return true;
	return false;
}

bool CMemoStream::ReadDouble(double &value)
{
	if (Read(&value,sizeof(double)) == sizeof(double))
		return true;
	return false;
}

bool CMemoStream::ReadInt(int &value)
{
	if (Read(&value,sizeof(int)) == sizeof(int))
		return true;
	return false;
}
