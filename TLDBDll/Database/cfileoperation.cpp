// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"

#include "cfileoperation.h"
#include "cpages.h"
#include "cfakerecno.h"
#include "../../Public/System.h"




CFileOperation::CFileOperation()
{
	m_pFile = 0;
	m_Header.SetHeaderSize(64);

	m_Header.SetFieldPosition(64);
	m_Header.SetFieldCount(0);

	m_Header.SetDataPosition(64);
	m_Header.SetDataSize(0);
	m_Header.SetRecordCount(0);

	
	m_nIOTotalLen = 1024*1024*2;
	//m_nIOTotalLen = 1024*8;
	m_IOBuf = new BYTE[m_nIOTotalLen];
}


CFileOperation::~CFileOperation()
{
	delete []m_IOBuf;
	if(m_pFile)
	{
		delete m_pFile;
		m_pFile =0;
	}

}

int CFileOperation::GetFileVersion()
{
	return m_Header.GetFileVersion();
}

bool CFileOperation::IniReadFile(LPCTSTR lpszFileName, CFields* pFields)
{
    if (m_pFile)
            delete m_pFile;
	
	//���ȸ��ݴ�����ļ��������ļ���ʵ��
	try
	{//�ļ��򿪷�ʽ��
		//m_pFile = new CFlatFile(lpszFileName,TCLFile::modeRead|TCLFile::shareDenyWrite );
		m_pFile = new TCLFile(lpszFileName,TCLFile::modeRead);//===uc
		if (m_pFile->IsOpen()==false)
			return false;
	}
	catch(...)
	{
		m_pFile = NULL;
		return false;
	}
	
	//*
	//װ���ļ�ͷ
	int nn = sizeof(CHeader);
	m_pFile->Read(&m_Header,sizeof(CHeader));
	if (m_Header.GetFileVersion() != 120 )
		return false;

	m_nRecordCount = m_Header.GetRecordCount();

	//���ֶ���Ϣװ���ֶ�����
	int nFieldCount = m_Header.GetFieldCount();
	if (!nFieldCount)
		return false;

	BYTE *pOldTemp = new BYTE[m_Header.GetDataPosition()];
	m_pFile->Read(pOldTemp , m_Header.GetDataPosition());
	BYTE *pTemp = pOldTemp;
	pFields ->LoadFromBuffer(pTemp, nFieldCount);
	delete []pOldTemp ;
    return true;
}


bool CFileOperation::IniSaveFile(LPCTSTR lpszFileName,CPages* pPages, CFields* pFields)
{
    if (m_pFile)
            delete m_pFile;
    //m_pFile = new TCLFile(lpszFileName,TCLFile::modeCreate  |TCLFile::modeWrite);
	m_pFile = new TCLFile(lpszFileName,TCLFile::modeCreate  |TCLFile::modeWrite);

	m_pFile->Write(&m_Header,sizeof(CHeader));

	//�����ļ�ͷ��Ϣ��������
	long nHeaderSize = sizeof(CHeader);
	m_Header.SetHeaderSize(nHeaderSize);
	//�����ֶ���������ʼλ��
	m_Header.SetFieldPosition(nHeaderSize);
	
	BYTE*pTemp = m_IOBuf;
    //�����ֶ���Ϣ

	int n1 = m_pFile->GetPosition();
	pFields->SaveToBuffer(m_pFile);//===uc  Ҫ��
	int n2 = m_pFile->GetPosition();
	long nFieldSize =n2 -n1;
	//m_pFile->Write(m_IOBuf,nFieldSize);
	
	//�������ݴ洢����ʼλ��
	long nDataPosition = nHeaderSize + nFieldSize;  //�ļ���ʼλ������0����ģ����Բ���+1
	m_Header.SetDataPosition(nDataPosition);
	//�����ֶθ���
    m_Header.SetFieldCount(pFields->GetFieldCount());
	

	return true;
}


int CFileOperation::ReadRecords(CPage* pPage, CFromPos Flags, long RecNo, int Count) 
{//*
	//assert(RecNo >= 1);
	//assert(Count >= 1);
	pPage->m_pFields->SetCurVersion(0);//###_Mod 2002-9-18 zlq ���
	int nRecordLength = pPage->m_pFields->QueryLength();
	if (nRecordLength > m_nIOTotalLen )
	{//��¼���ȴ��ڻ�����,�����������
		delete []m_IOBuf;
		m_nIOTotalLen = nRecordLength+20;
		m_IOBuf = new BYTE[m_nIOTotalLen];
	}
	m_pFile->Seek(m_Header.GetDataPosition() + nRecordLength*(RecNo-1), TCLFile::tBegin);
	LPBYTE lpNodeBuf;
	int nLoadLen = m_nIOTotalLen;
	if (nLoadLen > nRecordLength* Count)
		nLoadLen = nRecordLength* Count;
	m_pFile->Read(m_IOBuf, nLoadLen);
	lpNodeBuf = m_IOBuf;
	
	int nNextCur = pPage->m_nCurNo + 1;
	pPage->_AppendInBatch(Count);
	for(int k = 0; k < Count; k++)
	{
		if (nLoadLen+ m_IOBuf - lpNodeBuf <nRecordLength  )
		{
			m_pFile->Read(m_IOBuf,nLoadLen);
			lpNodeBuf = m_IOBuf;
		}
		
		//FromEnd
		//pPage->_AppendIn(storeTlFile,0);//0 ���� ===temp yh
		pPage->_SetRecLast(nNextCur+k);

		int nTemp;
		pPage->GetRecord(lpNodeBuf,nTemp);//===temp yh
		lpNodeBuf = lpNodeBuf+nRecordLength;
	}
	return RecNo + Count - 1;//*/  ===temp yh
	return 0;
}


bool Com(void *pDataA,void*pDialogData)
{
	CFileOperation*pData = (CFileOperation*)pDataA;
	return pData->SaveFile(pDialogData);

}
void CFileOperation::SaveRecords(CPages* pPages)
{
	m_pTempPages = pPages;

	
	typedef bool  (*pAddr)(void*pDlg,void*pDialogData);
	typedef  bool (*pShowDelay) (pAddr pAddr,void *pData);
	//***************************
		SaveFile(NULL);
		return;



}

bool CFileOperation::SaveFile(void*pDialogData)
{

	{//ֱ�ӱ����ļ�**********************************
		int nRecordLength = (m_pTempPages->m_pFields)->QueryLength();
		if (nRecordLength > m_nIOTotalLen )
		{//��¼���ȴ��ڻ�����,�����������
			delete []m_IOBuf;
			m_nIOTotalLen = nRecordLength+20;
			m_IOBuf = new BYTE[m_nIOTotalLen];
		}
		
		LPBYTE lpNodeBuf;
		//�ж��Ƿ���Ҫ�������ɻ�����
		if( (nRecordLength)* 50 > m_nIOTotalLen)//1000
		{
			delete []m_IOBuf;
			m_nIOTotalLen = (nRecordLength)* 50;
			m_IOBuf = new BYTE[m_nIOTotalLen];
		}		
		lpNodeBuf = m_IOBuf;
		
		m_pTempPages->First();
		while (!m_pTempPages->Eof())
		{
			if (m_nIOTotalLen+ m_IOBuf - lpNodeBuf <nRecordLength )
			{
				m_pFile->Write(m_IOBuf, lpNodeBuf-m_IOBuf);
				lpNodeBuf = m_IOBuf;
			}
			m_pTempPages->SetRecord(lpNodeBuf);
			lpNodeBuf += nRecordLength;
			
			m_pTempPages->Next();
		}
		if (lpNodeBuf  > m_IOBuf )
		{//����
			m_pFile->Write(m_IOBuf, lpNodeBuf-m_IOBuf);
			lpNodeBuf = m_IOBuf;
		}
		
		m_Header.SetRecordCount(m_pTempPages->GetRecordCount());
		//������������С
		//m_Header.SetDataSize(m_lpBufData - lpBuf + 1); //���Ҫ��1:2002-7-24-XC
		//����浵ʱ��
		SYSTEMTIME tTime;		
		TCSystem::GetSystemTime(&tTime);
		m_Header.SetTime(tTime);
		
		m_pFile->SeekToBegin();
		m_pFile->Write(&m_Header,sizeof(CHeader));
		m_pFile->SeekToEnd();
	}
	return true;
}
