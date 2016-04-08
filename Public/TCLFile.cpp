// TCLFile.cpp: implementation of the TCLFile class.
//
//////////////////////////////////////////////////////////////////////


#include "TCLFile.h"
//#include <io.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const TPosType TCLFile::tBegin  = SEEK_SET;    //开始处
const TPosType TCLFile::tCurrent  = SEEK_CUR;      //当前位置
const TPosType TCLFile::tEnd =SEEK_END;  //后面位置


const  unsigned int TCLFile::modeCreate = 1;//没有则创建文件
const  unsigned int TCLFile::modeNoTruncate = 2;//原来有,则不清空
const  unsigned int TCLFile::modeRead  = 4;//只读
const  unsigned int TCLFile::modeReadWrite = 8;//读写
const  unsigned int TCLFile::modeWrite = 16;//只写

TCLFile::TCLFile()
{
	Init();
}


TCLFile::~TCLFile()
{
	Close();
}

TCLFile::TCLFile( LPCTSTR lpszFileName, UINT nOpenFlags )
{
	Init();
	Open(lpszFileName, nOpenFlags);
}

/************************************************************************************
功能说明：
        打开文件
出入参数：
    [in] lpszFileName         : 文件名
	[in] nOpenFlags:读写标志，默认可读写
返回：是否读写成功
************************************************************************************/
bool TCLFile::Open(LPCTSTR lpszFileName, UINT nOpenFlags)
{
#ifdef WINPLAT //  Windows平台专用
	_fmode = _O_BINARY;
#else          //  Linux平台专用
	//===uc 未完成
#endif
	
	Close();
	CTString strMode = "";
	if (CheckMode(nOpenFlags,strMode)==false)
		return false;
	const char *lpszMode =  strMode.GetData();
	m_hFile = fopen( lpszFileName,lpszMode);
	
	if(m_hFile  == NULL )
		return false;

	m_sFileName = lpszFileName;

	return true;
}
//关闭文件操作
void TCLFile::Close()
{
	if (m_hFile)
	{
		fclose(m_hFile );
		Init();
	}
	
}

//按行写字符串到文件－add by xds 20060816
int TCLFile::WriteString(std::string strText)
{	
	int nLen = strText.length();
	
	if (nLen < 1)//空串
		return 0;
	//最后非回车
	std::string szReturn = "\n";
	int nPos = strText.rfind(szReturn);
	if (nPos != nLen-szReturn.length())
	{
		strText += szReturn;
	}
	if (fputs(strText.c_str(), m_hFile) == EOF)
		return 0;
	nLen = strText.length();
	return nLen;
}
//按行从文件中读字符串－add by xds 20060816
int TCLFile::ReadString(std::string &strText)
{
	int nLen = 0, nTemp=0;
	const int nMaxSize = 128;
	//一个保存
	char *pSave = NULL;
	//一个读
	char *pBuf = new char[nMaxSize+1];
	for (;;)
	{
		const char *lpszRes = fgets(pBuf, nMaxSize+1, m_hFile);
		int n = feof(m_hFile);
		if (lpszRes == NULL || feof(m_hFile))
		{
			delete []pBuf;
			return 0;
		}
		
		nTemp = lstrlen(pBuf);
		nLen += nTemp;
		if (pSave != NULL)
		{
			char *pTemp = new char [nLen-nTemp+1];
			memcpy(pTemp,pSave,(nLen-nTemp)*sizeof(char));
			pTemp[nLen-nTemp] = 0;
			delete []pSave;
			pSave = new char [nLen+1];
			memcpy(pSave,pTemp,(nLen-nTemp+1)*sizeof(char));
			strcat(pSave,pBuf);
			pSave[nLen] = 0;
			delete []pTemp;
		}
		else
		{
			pSave = new char [nLen+1];
			memcpy(pSave,pBuf,nLen*sizeof(char));
			pSave[nLen] = 0;
		}
		// if string is read completely or EOF
		if (lpszRes == NULL ||
			(nTemp = lstrlen(pBuf)) < nMaxSize ||
			pBuf[nTemp-1] == '\n')
			break;
	}
	strText = pSave;
	delete []pSave;
	delete []pBuf;
	return nLen;
}

//读当前文件数据
UINT TCLFile::Read(void *lpBuf, UINT nCount)
{
	return fread(lpBuf,1,nCount,m_hFile);
}
//写当前文件数据 
int TCLFile::Write(const void *lpBuf, UINT nCount)
{
	return fwrite(lpBuf,1, nCount, m_hFile);
}
//得到文件长度
INT64 TCLFile::GetLength()
{
	long curpos = ftell(m_hFile);
	fseek(m_hFile,0,SEEK_END);
	long nLen = ftell(m_hFile);
	fseek(m_hFile,curpos,SEEK_SET);
	return nLen;
}
//设置文件长度
void TCLFile::SetLength(INT64 dwNewLen)
{
	//_chsize(m_hFile,dwNewLen);
	//===uc 位数不对
}

//得到当前文件位置
INT64 TCLFile::GetPosition()
{
	return ftell(m_hFile);
}
//定位文件操作,返回定位后的位置
INT64 TCLFile::Seek(INT64 lOff, TPosType nFrom /*= tBegin*/)
{
	fseek(m_hFile,lOff,nFrom);
	return GetPosition();
}

CTString TCLFile::GetFileName()
{
	return m_sFileName;
}


void TCLFile::Init()
{
	m_hFile = NULL;
	m_sFileName = "";

}

bool TCLFile::IsOpen()
{
	return m_hFile != NULL;
}

void TCLFile::SeekToEnd()
{
	Seek(GetLength());
}
void TCLFile::SeekToBegin()
{
	Seek(0);
}


TCLFile &  TCLFile::operator << (const CTString &strData_)
{
	CTString& strData=(CTString&)strData_;
	if (IsOpen())
	{
		int nLen = strData.GetLength();
		Write(&nLen,sizeof(int));
		Write(strData.GetData(),strData.GetLength());
	}
	return *this;
}

TCLFile &  TCLFile::operator << (char *pData)
{
	if (IsOpen())
	{
		Write(pData,strlen(pData)+1);
	}
	return *this;
}

TCLFile &  TCLFile::operator << (unsigned char *pData)
{
	if (IsOpen())
	{
		Write(pData,strlen((char *)pData)+1);
	}
	return *this;
}

TCLFile &  TCLFile::operator << (char &cData)
{
	if (IsOpen())
	{
		Write(&cData,sizeof(char));
	}
	return *this;
}

TCLFile &  TCLFile::operator << (unsigned char &cData)
{
	if (IsOpen())
	{
		Write(&cData,sizeof(char));
	}
	return *this;
}

TCLFile &  TCLFile::operator << (bool &bData)
{
	if (IsOpen())
	{
		Write(&bData,sizeof(bool));
	}
	return *this;
}

TCLFile &  TCLFile::operator << (short int &nData)
{
	if (IsOpen())
	{
		Write(&nData,sizeof(short int));
	}
	return *this;
}

TCLFile &  TCLFile::operator << (unsigned short int &nData)
{
	if (IsOpen())
	{
		Write(&nData,sizeof(unsigned short int));
	}
	return *this;
}

TCLFile &  TCLFile::operator << (int &nData)
{
	if (IsOpen())
	{
		Write(&nData,sizeof(int));
	}
	return *this;
}

TCLFile &  TCLFile::operator << (unsigned int &nData)
{
	if (IsOpen())
	{
		Write(&nData,sizeof(unsigned int));
	}
	return *this;
}

TCLFile &  TCLFile::operator << (long &lData)
{
	if (IsOpen())
	{
		Write(&lData,sizeof(long));
	}
	return *this;
}

TCLFile &  TCLFile::operator << (unsigned long &lData)
{
	if (IsOpen())
	{
		Write(&lData,sizeof(unsigned long));
	}
	return *this;
}
#ifdef WINPLAT //  Windows平台专用
TCLFile &  TCLFile::operator << (__int64 &nlpData)
{
	if (IsOpen())
	{
		Write(&nlpData,sizeof(__int64));
	}
	return *this;
}

TCLFile &  TCLFile::operator << (unsigned __int64 &unlpData)
{
	if (IsOpen())
	{
		Write(&unlpData,sizeof(unsigned __int64));
	}
	return *this;
}
#endif

TCLFile &  TCLFile::operator << (float &fData)
{
	if (IsOpen())
	{
		Write(&fData,sizeof(float));
	}
	return *this;
}

TCLFile &  TCLFile::operator << (double &dData)
{
	if (IsOpen())
	{
		Write(&dData,sizeof(double));
	}
	return *this;
}


TCLFile &  TCLFile::operator >> (CTString &strData)
{
	if (IsOpen())
	{		
		//Read((char*)strData,strData.GetLength()+1);
		int nLen=0,ret = 0;
		ret = Read(&nLen,sizeof(int));
		char * buf= new char[nLen+1];
		memset(buf,0,nLen+1);
		Read(buf,nLen);
		strData = buf;
		delete []buf;
	}
	return *this;
}

TCLFile &  TCLFile::operator >> (char *pData)
{
	if (IsOpen())
	{
		Read(pData,strlen(pData)+1);
	}
	return *this;
}

TCLFile &  TCLFile::operator >> (unsigned char *pData)
{
	if (IsOpen())
	{
		Read(pData,strlen((char *)pData)+1);
	}
	return *this;
}

TCLFile &  TCLFile::operator >> (char &cData)
{
	if (IsOpen())
	{
		Read(&cData,sizeof(char));
	}
	return *this;
}

TCLFile &  TCLFile::operator >> (unsigned char &cData)
{
	if (IsOpen())
	{
		Read(&cData,sizeof(char));
	}
	return *this;
}

TCLFile &  TCLFile::operator >> (bool &bData)
{
	if (IsOpen())
	{
		Read(&bData,sizeof(bool));
	}
	return *this;
}

TCLFile &  TCLFile::operator >> (short int &nData)
{
	if (IsOpen())
	{
		Read(&nData,sizeof(short int));
	}
	return *this;
}

TCLFile &  TCLFile::operator >> (unsigned short int &nData)
{
	if (IsOpen())
	{
		Read(&nData,sizeof(unsigned short int));
	}
	return *this;
}

TCLFile &  TCLFile::operator >> (int &nData)
{
	if (IsOpen())
	{
		Read(&nData,sizeof(int));
	}
	return *this;
}

TCLFile &  TCLFile::operator >> (unsigned int &nData)
{
	if (IsOpen())
	{
		Read(&nData,sizeof(unsigned int));
	}
	return *this;
}

TCLFile &  TCLFile::operator >> (long &lData)
{
	if (IsOpen())
	{
		Read(&lData,sizeof(long));
	}
	return *this;
}

TCLFile &  TCLFile::operator >> (unsigned long &lData)
{
	if (IsOpen())
	{
		Read(&lData,sizeof(unsigned long));
	}
	return *this;
}

#ifdef WINPLAT //  Windows平台专用
TCLFile &  TCLFile::operator >> (__int64 &nlpData)
{
	if (IsOpen())
	{
		Read(&nlpData,sizeof(__int64));
	}
	return *this;
}

TCLFile &  TCLFile::operator >> (unsigned __int64 &unlpData)
{
	if (IsOpen())
	{
		Read(&unlpData,sizeof(unsigned __int64));
	}
	return *this;
}
#endif

TCLFile &  TCLFile::operator >> (float &fData)
{
	if (IsOpen())
	{
		Read(&fData,sizeof(float));
	}
	return *this;
}

TCLFile &  TCLFile::operator >> (double &dData)
{
	if (IsOpen())
	{
		Read(&dData,sizeof(double));
	}
	return *this;
}

/*
a  _O_WRONLY | _O_APPEND (usually _O_WRONLY | _O_CREAT | _O_APPEND) 
a+ _O_RDWR | _O_APPEND (usually _O_RDWR | _O_APPEND | _O_CREAT ) 
r  _O_RDONLY 
r+ _O_RDWR 
w  _O_WRONLY (usually _O_WRONLY | _O_CREAT | _O_TRUNC) 
w+ _O_RDWR (usually _O_RDWR | _O_CREAT | _O_TRUNC) 
 
	modeWrite	modeRead	modeReadWrite	modeNoTruncate	modeCreate
a	1										1				.
a+							1				1				.
r				1											
r+							1				0				0
w	1										0				1
w+							1				0				1
*/
bool TCLFile::CheckMode(UINT nOpenFlags, CTString &strMode)
{
	strMode = "";
	int nStat = 0;
	if  (nOpenFlags & modeRead) 
		nStat++;
	if  (nOpenFlags & modeWrite) 
		nStat++;
	if  (nOpenFlags & modeReadWrite) 
		nStat++;
	if (nStat != 1)//只能有一种状态
		return false;
	
	if  (nOpenFlags & modeRead) //只读
		strMode = "r";
	else if  (nOpenFlags & modeWrite) //只写
	{
		if  (nOpenFlags & modeNoTruncate) 
			strMode = "a";
		else
			strMode = "w";		
	}
	else
	{
		if  (nOpenFlags & modeNoTruncate) 
			strMode = "a";
		else
		{
			if  (nOpenFlags & modeCreate) 
				strMode = "w";	
			else
				strMode = "r";	
		}
	}

	if  (nOpenFlags & modeReadWrite) 
	{
		strMode = strMode+"+";
	}	
	//strMode = strMode + "b";
	return true;
}
