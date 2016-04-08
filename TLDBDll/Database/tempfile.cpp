// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "tempfile.h"
#include "cfields.h"
//#include "recordnode.h"
#include "cfakerecno.h"
//modeNoTruncate || modeReadWrite


/************************************************************************************
函数名称：TempFile:: TempFile(LPCTSTR lpszFileName, UINT nOpenFlags)
功能说明：构造函数,构造一个tempfile实例，分配一个缓冲区，大小为1016个字节

详细解释：

出入参数：
    [in]:文件名，打开标志
	[out]:
返回类型：
    
制作：

修改: 
TempFile:: TempFile(LPCTSTR lpszFileName, UINT nOpenFlags)
:TCLFile( lpszFileName, nOpenFlags)
{
        m_lpBuf = new BYTE[m_nBufSize];
        m_lpBuf = new BYTE[m_nBufSize];
        Write(m_lpBuf, 20);
}************************************************************************************/
TempFile:: TempFile(LPCTSTR lpszFileName, UINT nOpenFlags)
//:TCLFile( lpszFileName, nOpenFlags)
{

	Open(lpszFileName);

    //Write(m_lpBuf, 20);    // 给文件头上写20个字节
	
	//☆☆☆☆☆☆☆☆ #_S 2005-5-10 zlq  
	m_nIOTotalLen = 1024*1024*2;

	m_IOBuf = new BYTE[m_nIOTotalLen];

}
/************************************************************************************
函数名称：TempFile:: ~TempFile()
功能说明：析构函数，释放缓冲区资源
详细解释：
出入参数：
    [in]:
	[out]:
返回类型：
制作：

修改: 
************************************************************************************/
TempFile:: ~TempFile()
{
	//☆☆☆☆☆☆☆☆ #_S 2005-5-10 zlq  
	delete []m_IOBuf;


}



/*
*
* nStartPos 起始记录号,相对位置0开始
*count:保存记录个数
*nFilePos 文件偏移位置
*Offset,偏移记录个数,
*/
long TempFile::_SavePage(CPage *pPage,int nStartPos, int count,INT64 nFilePos, int Offset)
{
	int nOldPhysicalNo = pPage->m_nCurNo;//###zlq 2002-08-08 Add 保存原来的数据位置

    //获取当前记录存储用的字节数 
	int nRecordLength = (pPage->m_pFields)->QueryLength();//+sizeof(int);
	int nCurNewVersion = pPage->GetNewestVersion();//当前最新版本，此时不可能进行字段修改操作
	if (nFilePos ==-1)
		SeekToEnd();      //把文件指针调整到临时文件当前的结尾
	else
		//Seek(nFilePos + nRecordLength*Offset,TCLFile::begin   );
		Seek(nFilePos + nRecordLength*Offset);
	long nCurFilePos = GetPosition( );
	//保存将要返回的第一个结点的位置指针
	long  result = nCurFilePos;

    LPBYTE lpNodeBuf;
	//判断是否需要重新生成缓冲区
	if( (nRecordLength)* 50 > m_nIOTotalLen)//1000
	{
		delete []m_IOBuf;
		m_nIOTotalLen = (nRecordLength)* 50;
		m_IOBuf = new BYTE[m_nIOTotalLen];
		lpNodeBuf = m_IOBuf;
	}
	else
		lpNodeBuf = m_IOBuf;
	
	//pPage->First();
	bool rst = pPage->_SetRec(nStartPos);
	//assert(rst);
	for (int i=0;i < count ;i++)
	{
		if (m_nIOTotalLen+ m_IOBuf - lpNodeBuf <nRecordLength )
		{
			Write(m_IOBuf, lpNodeBuf-m_IOBuf);
			lpNodeBuf = m_IOBuf;
		}
		pPage->_SetRecord(lpNodeBuf );//===temp yh
		lpNodeBuf = lpNodeBuf + nRecordLength;
		//pPage->PrintCurLine();
		pPage->Next();
	}

	if (lpNodeBuf  > m_IOBuf )
	{//残余
		Write(m_IOBuf, lpNodeBuf-m_IOBuf);
		lpNodeBuf = m_IOBuf;
	}
	//pPage->MoveBy(nOldPhysicalNo, FromBegin);//恢复原来的数据位置
	return result;
}
/*
*
*Offset,偏移记录个数,
*count:读取记录个数
*/
void TempFile::_LoadPage(CPage *pPage, INT64 FilePosition, int Offset,int nCurVersion, int count)
{
	pPage->m_pFields->SetCurVersion(nCurVersion);
	int nRecordLength = (pPage->m_pFields)->QueryLength();

	//Seek(FilePosition + nRecordLength*Offset, TCLFile::begin);
	Seek(FilePosition + nRecordLength*Offset);
	LPBYTE lpNodeBuf;
	int nLoadLen = m_nIOTotalLen;
	if (nLoadLen > nRecordLength* count)
		nLoadLen = nRecordLength* count;
	Read(m_IOBuf, nLoadLen);
	lpNodeBuf = m_IOBuf;

	int nNextCur = pPage->m_nCurNo + 1;
	pPage->_AppendInBatch(count);
    //开始读记录
	for(int i = 0; i < count; i++)
	{
		if (nLoadLen+ m_IOBuf - lpNodeBuf <nRecordLength  )
		{
			Read(m_IOBuf,nLoadLen);
			lpNodeBuf = m_IOBuf;
		}
		
		//pPage->_AppendIn(storeTempFile,0);//0 无用 ===temp yh
		pPage->_SetRecLast(nNextCur+i);

		int nTemp;
		pPage->GetRecord(lpNodeBuf,nTemp);//===temp yh
		lpNodeBuf = lpNodeBuf+nRecordLength;
		//pPage->PrintCurLine();
	}
	//===temp 版本恢复问题?
}

CTString TempFile::GetFilePath()
{
	return GetFileName();
}
