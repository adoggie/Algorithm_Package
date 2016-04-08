// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "tempfile.h"
#include "cfields.h"
//#include "recordnode.h"
#include "cfakerecno.h"
//modeNoTruncate || modeReadWrite


/************************************************************************************
�������ƣ�TempFile:: TempFile(LPCTSTR lpszFileName, UINT nOpenFlags)
����˵�������캯��,����һ��tempfileʵ��������һ������������СΪ1016���ֽ�

��ϸ���ͣ�

���������
    [in]:�ļ������򿪱�־
	[out]:
�������ͣ�
    
������

�޸�: 
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

    //Write(m_lpBuf, 20);    // ���ļ�ͷ��д20���ֽ�
	
	//��������� #_S 2005-5-10 zlq  
	m_nIOTotalLen = 1024*1024*2;

	m_IOBuf = new BYTE[m_nIOTotalLen];

}
/************************************************************************************
�������ƣ�TempFile:: ~TempFile()
����˵���������������ͷŻ�������Դ
��ϸ���ͣ�
���������
    [in]:
	[out]:
�������ͣ�
������

�޸�: 
************************************************************************************/
TempFile:: ~TempFile()
{
	//��������� #_S 2005-5-10 zlq  
	delete []m_IOBuf;


}



/*
*
* nStartPos ��ʼ��¼��,���λ��0��ʼ
*count:�����¼����
*nFilePos �ļ�ƫ��λ��
*Offset,ƫ�Ƽ�¼����,
*/
long TempFile::_SavePage(CPage *pPage,int nStartPos, int count,INT64 nFilePos, int Offset)
{
	int nOldPhysicalNo = pPage->m_nCurNo;//###zlq 2002-08-08 Add ����ԭ��������λ��

    //��ȡ��ǰ��¼�洢�õ��ֽ��� 
	int nRecordLength = (pPage->m_pFields)->QueryLength();//+sizeof(int);
	int nCurNewVersion = pPage->GetNewestVersion();//��ǰ���°汾����ʱ�����ܽ����ֶ��޸Ĳ���
	if (nFilePos ==-1)
		SeekToEnd();      //���ļ�ָ���������ʱ�ļ���ǰ�Ľ�β
	else
		//Seek(nFilePos + nRecordLength*Offset,TCLFile::begin   );
		Seek(nFilePos + nRecordLength*Offset);
	long nCurFilePos = GetPosition( );
	//���潫Ҫ���صĵ�һ������λ��ָ��
	long  result = nCurFilePos;

    LPBYTE lpNodeBuf;
	//�ж��Ƿ���Ҫ�������ɻ�����
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
	{//����
		Write(m_IOBuf, lpNodeBuf-m_IOBuf);
		lpNodeBuf = m_IOBuf;
	}
	//pPage->MoveBy(nOldPhysicalNo, FromBegin);//�ָ�ԭ��������λ��
	return result;
}
/*
*
*Offset,ƫ�Ƽ�¼����,
*count:��ȡ��¼����
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
    //��ʼ����¼
	for(int i = 0; i < count; i++)
	{
		if (nLoadLen+ m_IOBuf - lpNodeBuf <nRecordLength  )
		{
			Read(m_IOBuf,nLoadLen);
			lpNodeBuf = m_IOBuf;
		}
		
		//pPage->_AppendIn(storeTempFile,0);//0 ���� ===temp yh
		pPage->_SetRecLast(nNextCur+i);

		int nTemp;
		pPage->GetRecord(lpNodeBuf,nTemp);//===temp yh
		lpNodeBuf = lpNodeBuf+nRecordLength;
		//pPage->PrintCurLine();
	}
	//===temp �汾�ָ�����?
}

CTString TempFile::GetFilePath()
{
	return GetFileName();
}
