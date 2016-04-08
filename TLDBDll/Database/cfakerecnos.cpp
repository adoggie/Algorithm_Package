// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "tempfile.h"
#include "cfakerecnos.h"
#include "cfileoperation.h"
#include "DriverManager.h"
#include "../../Public/System.h"
#include "../../Public/common.h"

extern BYTE  tlcfgBuf[] ;
extern BYTE  CurFileBuf[] ;

/************************************************************************************
�������ƣ�
    CFakeRecNos::~CFakeRecNos
����˵����
    ��������
��ϸ���ͣ�
       1.ɾ��CFileOperation,TempFile������.
	   2.���m_FakeRecNoList��m_QueueItemList����.
���������
�������ͣ�
������
    Eric2002-07-08   //### Add by YTLI at  2002/08/12
�޸�: 
************************************************************************************/
CFakeRecNos::~CFakeRecNos()
{	
	Clear();//###_�޸� 2002-8-27 zlq


}



/************************************************************************************
�������ƣ�
void CFakeRecNos::InQueueLoad(CPage* pPage, long FakeRecNo, int count, bool IsNeedFeedback)
����˵���������в���.

��ϸ���ͣ�1.���ݴ�����������ɽṹ,���ӵ��Ŷӵȴ��Ĵ���Ķ���m_pQueueItemList��ȥ.
         2.Ȼ����m_bThreadStart ,����߳��Ѿ�ֹͣ,�����������߳�,
		 3.����m_bThreadStart����Ϊ����״̬(true).

���������
[in]: 1.pPage             //��ҳ��ָ��.
      2.FakeRecNo         //
	  3.count             //      
	  4.IsNeedFeedback    //
  
[out]:�ޡ�

�������ͣ�void

������zlq   //### Add Document by YTLI at  2002/08/12

�޸�: 
************************************************************************************/
void CFakeRecNos::InQueueLoad(CPage* pPage, long FakeRecNo, int count, bool IsNeedFeedback)
{
	m_pIOFields_->GetBookmark();
	//int nVersion = m_pIOFields_->GetCurVersion();

	Load(pPage,FakeRecNo,count);

	pPage->m_bIsModify = false;

	m_pIOFields_->SetCurVersion();
	m_pIOFields_->GotoBookmark();
	

}


/************************************************************************************
�������ƣ�
CFakeRecNos::Load(CPage* pPage, long FakeRecNo, int count, bool IsNeedFeedback)
����˵����ҳ���װ�빤��.

��ϸ���ͣ�1.�����ҳ����ʵ��Ϊ��,��ô�Ƚ���������ղ���.
         2.���������α��¼����ȷ��Ҫ��ȡ��朱�ĽY�c��Χ.

���������
[in]:    1.pPage            //��ҳ��ָ��
		 2.FakeRecNo        //��ҳ����Ҫ��ȡ����ʼα��¼��
		 3.count            //��ҳ����Ҫ��ȡ�ļ�¼����
		 4.IsNeedFeedback   //
[out]:�ޡ�
************************************************************************************/
void CFakeRecNos::Load(CPage* pPage, long FakeRecNo, int count)
{
	////===temp TRACE( "Load enter ҳ��:%d ��ʼ�߼���¼��=%d, ��ʼα��¼��=%d, OldFakeRecNo = %d count=%d\n", pPage->m_nPageNumber, pPage->m_lngLStartRecNo, pPage->m_lngFakeStartRecNo, FakeRecNo ,count);
	pPage->HangFields(m_pIOFields_);
	//assert(pPage->m_nLogicCount == pPage->m_nOriginLoadCount);
	pPage->m_pFields->m_nLastPosVersion = pPage->m_pFields->GetCurVersion();//###_�޸� 2002-8-27 zlq
	m_pPages->m_pFields->m_nLastPosVersion = pPage->m_pFields->m_nLastPosVersion;//###_�޸� 2002-8-27 zlq
#ifdef _DEBUG
	Print();
#endif
	//�����ҳ����ʵ��Ϊ��,��ô�Ƚ���������ղ���
	if(!pPage->IsPhysicalEmpty())	
		pPage->EmptyTable();

	long lStartFakeRecNo   =  0;
	long lEndFakeRecNo     =  0;
	long lPageMaxFakeRecNo =  ( FakeRecNo + count -1 );	

	POSITION pos = m_FakeRecNoList.GetHeadPosition();
	if(!pos)
		return;
	do
	{
		CFakeRecNo* pFakeRecNo = (CFakeRecNo*)m_FakeRecNoList.GetNext(pos);

		lStartFakeRecNo = (lEndFakeRecNo + 1);
		lEndFakeRecNo  += pFakeRecNo->m_lngRecCount;

		int iOffset  = FakeRecNo - lStartFakeRecNo;			

		if( (  FakeRecNo >= lStartFakeRecNo) && 
			(  FakeRecNo <= lEndFakeRecNo  )   )
		{
			int iLoadCount = 0;

			//�������Ҫ��������
			if ( lPageMaxFakeRecNo <= lEndFakeRecNo )
				iLoadCount = count;
			else
				iLoadCount = lEndFakeRecNo - FakeRecNo + 1;//Ӧ�ü�1

			//����m_nStoreType�����ݶ�����
			if( pFakeRecNo->m_nStoreType == storeTlFile )
			{//����������Դ��ȡ				
				pPage->SetCurVersion(0);// �汾���ò���
				int nStartRecNo = (int)pFakeRecNo->m_lngPosition + iOffset;
				int nRealLoadCount = m_pOriginData->ReadRecords(pPage,FromEnd,nStartRecNo, iLoadCount );
				//����δ֪��¼��������Դ
				if ( nRealLoadCount != nStartRecNo + iLoadCount - 1 )
				{//û�ж�����Ӧ�ļ�¼����
					AdjustOriginCount(m_pPages->GetLogicCount() - nRealLoadCount);
					pPage->m_bReachEndError = true;//���ó�����
				}
			}
			else
			{//��ʱ�ļ���ȡ                                        
				//m_pTempFile->ReadRecords(pPage,FromEnd,pFakeRecNo->m_lngPosition,iOffset ,iLoadCount);
				m_pTempFile->_LoadPage(pPage,pFakeRecNo->m_lngPosition,iOffset ,pFakeRecNo->m_nVersion,iLoadCount);
			}
			count        -= iLoadCount;
			FakeRecNo    += iLoadCount;
		}
	}while( lEndFakeRecNo < lPageMaxFakeRecNo && pos);
	

	//assert(pPage->m_nLogicCount == pPage->m_nOriginLoadCount);
	//pPage->PrintCurLine();

}
/************************************************************************************
�������ƣ�
    CFakeRecNos::Clear
����˵����
    ������е���Ϣ,�ر��ļ�
��ϸ���ͣ�
    
���������
    [in,out]��
�������ͣ�
    
������
    //Eric2002-07-05
�޸�: 
************************************************************************************/
bool CFakeRecNos::Clear()
{
	//ɾ��CFileOperation,TempFile������.
	if(m_pOriginData)
	{
		delete m_pOriginData;
		m_pOriginData = 0;
	}
	if(m_pTempFile)
	{
		CTString filename = m_pTempFile->GetFilePath();
		delete 	m_pTempFile;
		unlink(filename);

		m_pTempFile=0;
	}

	//�������
	while(!m_FakeRecNoList.IsEmpty())
	{
		CFakeRecNo* pFakeRecNo = (CFakeRecNo*)m_FakeRecNoList.RemoveHead();
		delete pFakeRecNo;
	}
	while(!m_QueueItemList.IsEmpty())
	{
		CQueueItem* pQueueItem = (CQueueItem*)m_QueueItemList.RemoveHead();
		delete pQueueItem;
	}
	return true;
}



/************************************************************************************
�������ƣ�
����˵������治�Õr,��ҳ��ı������.

��ϸ���ͣ�1.���x�R�r朱�localFakeRecNoList�����Ҫ����ă���.
		 2.�{��,ԓ������퓃�ӛ䛰�Load�r�ā�Դ�ļ���ͬ���֠�С�K(Block).
		   Ȼ�ጦÿС�K(Block)�{��CFakeRecNos::UnLoadBlock()�����朱�localFakeRecNoList.
		 3.�{��CFakeRecNos::DecomposeList()�ֽ���ʱ����.
		 4.�{��CFakeRecNos::ReplaceList()�滻����.
		 5.�{��CFakeRecNos::MergeList()�ϲ�����.

���������
[in]: 1.pPage            //��ҳ��ָ��
      2.FakeRecNo        //��ҳ����ʼ��α��¼��
      3.IsNeedFeedback   //����
�޸�: 
***********************************************************************************/
void CFakeRecNos::UnLoad(CPage* pPage )
{
	m_pIOFields_->GetBookmark();
	//===temp   Ӧ��ɾ�� ������
	POSITION p1       = m_FakeRecNoList.GetHeadPosition();
	long lAllFake      =  0;//��ǰ������α��¼��� ����α��¼��
	while( p1)
	{	
		CFakeRecNo* pFakeNode        = (CFakeRecNo*)m_FakeRecNoList.GetAt(p1);	
		lAllFake     += pFakeNode->m_lngRecCount;
		m_FakeRecNoList.GetNext(p1);
	}

	//***temp end;
	////===temp TRACE( "��ʼ�߼���= %d,����= %d\t��ʼα��¼�� = %d,����:%d totalFake:%d\n",  
	//pPage->m_lngLStartRecNo,pPage->m_nLogicCount,pPage->m_lngFakeStart,pPage->m_nOriginLoadCount ,lAllFake);
	pPage->HangFields(m_pIOFields_);

	Print();


	m_pIOFields_->SetCurVersion();//### 08-23
	pPage->m_pFields->m_nLastPosVersion = pPage->m_pFields->GetCurVersion();
	m_pPages->m_pFields->m_nLastPosVersion = pPage->m_pFields->m_nLastPosVersion;
	int nLastMemoryVersion = pPage->m_pFields->GetLastVersion();
	int nRecordLen = pPage->m_pFields->QueryLength();
	
	//��lOriPageRecCnt  End************************************
	long lMaxOriginFakeNo   =  pPage->m_nOriginLoadCount + pPage->m_lngFakeStart - 1;//���Load�r������ă^ӛ�̖
	if ( lMaxOriginFakeNo <  pPage->m_lngFakeStart)//ȷ��lMaxOriginFakeNo��=FakeRecNo �������ҳ��ȫ����ӵģ�
		 lMaxOriginFakeNo =  pPage->m_lngFakeStart;
	long lMinFakeNo = pPage->m_lngFakeStart;//start

	POSITION posDeleteStart = NULL;      //��ʼɾ����λ��
	int      iDeleteCnt     = 0;         //�����Ҫɾ���Ľ������
	long     InLocallistPos =0  ;         //

	CPtrList localFakeRecNoList;
	long lPhyCnt    = pPage->GetLogicCount(); //��ҳ�е�ȫ����¼����.


	POSITION posRecNo       = m_FakeRecNoList.GetHeadPosition();
	long lBlockStart    =  0;//��ǰ������α��¼��� ����α��¼��
	long lBlockEnd      =  0;//��ǰ������α��¼��� ����α��¼��
	bool bIsVersionEqual = true;//������ؽڵ�İ汾�͵�ǰ���µ��Ƿ�һ��,���Ҷ�����ʱ�ļ���

	
	int nLastNodeReamin = 0;//���һ���ڵ�
	POSITION LastNodePos = NULL;//���һ���ڵ�����λ��
	
	//do//����һ����¼��		
	while( lMaxOriginFakeNo > lBlockEnd && posRecNo)
	{	
		CFakeRecNo* pFakeNode        = (CFakeRecNo*)m_FakeRecNoList.GetAt(posRecNo);	
		//ȷ���ü�¼�����β��¼
		lBlockStart    = lBlockEnd + 1;        
		lBlockEnd     += pFakeNode->m_lngRecCount;
		
		if (lMaxOriginFakeNo < lBlockStart )
			break;//����
		
		if (lMinFakeNo <= lBlockEnd )//���ڸÿ�
		{		
			nLastNodeReamin = lBlockEnd-(pPage->m_nOriginLoadCount + pPage->m_lngFakeStart)+1;
			LastNodePos = posRecNo;
				
			if(posDeleteStart == NULL)
			{
				posDeleteStart = posRecNo; //����Deleteʱ����ʼ���λ�ã������ã�
				InLocallistPos = lMinFakeNo - lBlockStart;
			}
			iDeleteCnt++;//�������ã�
			if (nLastMemoryVersion != pFakeNode->m_nVersion 
				|| pFakeNode->m_nStoreType != storeTempFile)
				bIsVersionEqual = false;
		}

		m_FakeRecNoList.GetNext(posRecNo);
	}


	//**********   ����������  ************************
	if (posDeleteStart == NULL)
	{//��ĩβ���
		POSITION posRecNo   = m_FakeRecNoList.GetTailPosition();
		CFakeRecNo* pFakeNode   = NULL;
		if (posRecNo)
			pFakeNode  = (CFakeRecNo*)m_FakeRecNoList.GetAt(posRecNo);	
		int nFilePos = m_pTempFile->_SavePage(pPage,0,pPage->m_nLogicCount);
		//( nVersion, nStoreType, lngPosition,  lngRecCount)
		if (pFakeNode && pFakeNode->m_nVersion == nLastMemoryVersion //�汾��ͬ
			//&& 0 //===temp
			&& lAllFake +1 ==pPage->m_lngFakeStart  //===uc
			&& pFakeNode->m_lngPosition + pFakeNode->m_lngRecCount*nRecordLen == nFilePos)
		{
			pFakeNode->m_lngRecCount = pFakeNode->m_lngRecCount+pPage->m_nLogicCount;
			return;//�������滻
		}
		else
		{
			CFakeRecNo* pFakeRecNo = new CFakeRecNo(nLastMemoryVersion,storeTempFile
				,nFilePos,pPage->m_nLogicCount);
			localFakeRecNoList.AddTail(pFakeRecNo);	
		}
	}
	else
	{

		Print();

		if (bIsVersionEqual ==true && pPage->m_nLogicCount == pPage->m_nOriginLoadCount)
		{//���� �޸�, �ֶ�Ҳ���仯,��������ʱ�ļ���
			POSITION curPos = posDeleteStart;
			int nStartOffset = InLocallistPos;
			int nRemain  = pPage->m_nLogicCount ;
			int nPagePos = 0;
			for (int i=0; i<iDeleteCnt ;i++)
			{
				CFakeRecNo* pNode = (CFakeRecNo*)m_FakeRecNoList.GetAt(curPos);
				int nc =0;//��ǰ�ڵ�Ҫ����ĸ���
				if (pNode->m_lngRecCount - nStartOffset> nRemain)
					nc = nRemain;
				else
					nc = pNode->m_lngRecCount - nStartOffset;
				nRemain = nRemain -nc;
				m_pTempFile->_SavePage(pPage,nPagePos,nc,pNode->m_lngPosition,nStartOffset);

				nPagePos = nPagePos+nc;
				nStartOffset = 0;//��һ����Ϊ��,����Ϊ��
				m_FakeRecNoList.GetNext(curPos);
			}
			//�������滻
			m_pIOFields_->SetCurVersion();
			m_pIOFields_->GotoBookmark();
			return;
		}
		else
		{//�޸�,�ҷ����汾�仯����������¼
			if (pPage->m_nLogicCount)
			{//�ǿ�ҳ
				int nFilePos = m_pTempFile->_SavePage(pPage,0,pPage->m_nLogicCount);
				CFakeRecNo* pFakeRecNo = new CFakeRecNo(nLastMemoryVersion,
					storeTempFile,nFilePos,pPage->m_nLogicCount);			
				localFakeRecNoList.AddTail(pFakeRecNo);	
			}

			if (InLocallistPos)
			{//ͷ�ڵ�
				CFakeRecNo* pNodeOld = (CFakeRecNo*)m_FakeRecNoList.GetAt(posDeleteStart);
				CFakeRecNo* pNodeNew = new CFakeRecNo(pNodeOld);
				pNodeNew->m_lngRecCount = InLocallistPos;
				localFakeRecNoList.AddHead(pNodeNew);
			}

			if (nLastNodeReamin )
			{//��β�ڵ�
				CFakeRecNo* pNodeOld = (CFakeRecNo*)m_FakeRecNoList.GetAt(LastNodePos );
				CFakeRecNo* pNodeNew = new CFakeRecNo(pNodeOld);
				m_pIOFields_->SetCurVersion(pNodeNew->m_nVersion);//### 08-23
				if (pNodeNew->m_nStoreType == storeTempFile)
					pNodeNew->m_lngPosition =  pNodeNew->m_lngPosition 
						+ m_pIOFields_->QueryLength()*(pNodeNew->m_lngRecCount - nLastNodeReamin );
				else
					pNodeNew->m_lngPosition =  pNodeNew->m_lngPosition + 
						(pNodeNew->m_lngRecCount - nLastNodeReamin) ; 

				pNodeNew->m_lngRecCount = nLastNodeReamin ;
				
				localFakeRecNoList.AddTail(pNodeNew);
				m_pIOFields_->SetCurVersion();//�ָ��汾
			}
		}
		
	}
	
	//*****************������滻**************
	ReplaceList(localFakeRecNoList,posDeleteStart,iDeleteCnt);	

	Print();



	m_pIOFields_->SetCurVersion();
	m_pIOFields_->GotoBookmark();
}

/************************************************************************************
�������ƣ�
����˵������m_FakeRecNoList����Ҫ����Ľڵ�����ʱ����Ľڵ����,˽�к���,�����ڲ�����.

��ϸ���ͣ�1.����ʱ����Ľڵ���뵽m_FakeRecNoList�е���Ӧ��λ��.
         2.ɾ��m_FakeRecNoList�оɵĽڵ�.
    
���������
[in]: 1.localFakeRecNoList    //��ʱ����.
      2.posDeleteStart        //��ʼɾ����λ��(����Ϊ��,���ں������)
      3.iDeleteCnt            //ɾ���ĸ���	  

[out]:1.posNewStart           //����ֵ,��һ�����滻�Ľڵ�,��������ϲ�.
	  2.posNewEnd             //����ֵ,���һ�����滻�Ľڵ�,��������ϲ�.

	  ע��:���localFakeRecNoListΪ��,�򷵻�posNewStart=posNewEnd,
	  posNewEnd����posDeleteStart ǰ��һ��(������Ϊ��)
�������ͣ�void

������YTLI   2002/07/12

�޸�: 
***********************************************************************************/
void CFakeRecNos::ReplaceList(CPtrList &localFakeRecNoList,
							  POSITION posDeleteStart,int iDeleteCnt 
							  )
{ 
	POSITION posRecNo = localFakeRecNoList.GetHeadPosition();//����Ϊ��,��Ϊȫ��ɾ�������.

	//assert(posRecNo || posDeleteStart);//����������ͬʱΪ��.
	POSITION posNewStart =NULL,posNewEnd=NULL;
	if(!posDeleteStart)
	{//ȫ������.�Ҿֲ�����Ϊ��.
		while(posRecNo != NULL)
		{
			m_FakeRecNoList.AddTail( localFakeRecNoList.GetNext(posRecNo) );
		}		
		posNewStart = m_FakeRecNoList.GetHeadPosition();
		posNewEnd = m_FakeRecNoList.GetTailPosition();
	}
	else
	{//��ɾ����ʼλ��
		if (posRecNo)
		{//�ֲ�����Ϊ��,�������posNewStart,posNewEnd������Ϊ��
			//���в������.
			posNewStart = 0;
			while(posRecNo != NULL)
			{
				if (!posNewStart )//���µ��׽ڵ㸳ֵ.
					posNewStart = m_FakeRecNoList.InsertBefore(posDeleteStart,localFakeRecNoList.GetNext(posRecNo));
				else
					m_FakeRecNoList.InsertBefore(posDeleteStart,localFakeRecNoList.GetNext(posRecNo));
			}
			//���µ�β�ڵ㸳ֵ.
			posNewEnd = posDeleteStart;
			m_FakeRecNoList.GetPrev (posNewEnd);//���ô���,�õ�posNewEnd,������Ϊ��
		}
		else
		{//�ֲ�����Ϊ��,�������posNewStart Ϊ��,posNewEnd ����Ϊ��
			posNewStart = 0;
			posNewEnd = posDeleteStart;
			m_FakeRecNoList.GetPrev (posNewEnd);//���ô���,�õ�posNewEnd,����Ϊ��,����ͷһ���ڵ�
		}

		//�����ɾ��	
		for(int i = 0; i<iDeleteCnt; i++)
		{
			POSITION posTemp     = posDeleteStart;
			CFakeRecNo*   pCFakeRecNo = (CFakeRecNo* )m_FakeRecNoList.GetNext(posDeleteStart);
			m_FakeRecNoList.RemoveAt(posTemp);
			delete pCFakeRecNo;
		}
		if (!posNewEnd)//���posDeleteStartǰ��û�нڵ�(����ԭ��),��ȡɾ������Ľڵ�
			posNewEnd = posDeleteStart;
		if (posNewStart==0)
			posNewStart = posNewEnd;//����ֲ�����Ϊ��.�򷵻��������.
	}
}







/************************************************************************************
�������ƣ�
    CFakeRecNos::NewFile
����˵����
    ���ԭ��������ͬʱ������ʱ�ļ������ݴ�����ֶ���Ϣ���Ƶ�m_pIOFields��
��ϸ���ͣ�
    
���������
    [in,out]clone��
�������ͣ�
    bool :�Ƿ�ɹ�
������
    Eric2002-07-05
�޸�: 
************************************************************************************/
bool CFakeRecNos::NewFile( CFields* clone )
{
	Clear();
	//������ʱ�ļ�
    char szTempFile[MAX_PATH];
	GetTempFile(szTempFile);
	//m_pTempFile = new TempFile(szTempFile,TCLFile::modeReadWrite|TCLFile::modeCreate);
	m_pTempFile = new TempFile(szTempFile,0);
	m_pTempFile->m_pFakeRecNos = this ;

	//���ñ���
	m_pIOFields_ = clone;
	return true;
}
/************************************************************************************
�������ƣ�
    CFakeRecNos::LoadFile
����˵����
    ��һ���ļ�������װ����Ӧ���ֶνṹ���Լ���ʼ����ʱ�ļ�
	1.���� IniReadFile�õ�������Ϣ����ʼ��α��¼����
	2.������ʱ�ļ�
��ϸ���ͣ�
    
���������
    [in,out]  lpszFileName���ļ���
�������ͣ�
    bool:�Ƿ�ɹ�
������
    Eric2002-07-05
�޸�: 
************************************************************************************/
bool CFakeRecNos::LoadFile(LPCTSTR lpszFileName)
{
	Clear();
	if (! m_DriverManager.LoadFile(lpszFileName,&m_pOriginData,m_pIOFields_))
		return false;//###_Mod 2002-9-16 zlq

	
    char szTempFile[MAX_PATH];
	GetTempFile(szTempFile);
	//m_pTempFile = new TempFile(szTempFile,TCLFile::modeReadWrite|TCLFile::modeCreate);
	m_pTempFile = new TempFile(szTempFile,0);
	m_pTempFile->m_pFakeRecNos = this ;

	CFakeRecNo * pFakeRecNo = new  CFakeRecNo();
	pFakeRecNo->m_lngPosition =1;
	pFakeRecNo->m_lngRecCount = m_pOriginData->GetRecordCount();
	pFakeRecNo->m_nStoreType = storeTlFile;
	pFakeRecNo->m_nVersion = 0;
	m_FakeRecNoList.AddTail(pFakeRecNo);
	
	return true;
}


/************************************************************************************
�������ƣ�
    CFakeRecNos::GetTempFile
����˵����
    ȡ��һ��Ψһ���ļ�����������ʱ�ļ�.
��ϸ���ͣ�
    
���������
    [in,out]szFileName��
�������ͣ�
    bool :�Ƿ�ɹ�
������
    Eric2002-07-05
�޸�: 
************************************************************************************/
bool CFakeRecNos::GetTempFile(char * szFileName)
{ 
	CTString spath;
    DWORD dwResult = TCSystem::GetTempPath(spath);
    //assert(dwResult);
    //ȡ��Ψһ���ļ���
	CTString sfile;
    TCSystem::GetTempFileName(spath, _T("~TLTemp"),sfile);
	memcpy(szFileName,sfile.GetData(),sfile.GetLength()+1);

	return true;
}
/************************************************************************************
�������ƣ�
    CFakeRecNos::CFakeRecNos()
����˵��:
    ���캯��
��ϸ����:
�������:
        [in]:
		    1.pPages:ҳ�漯ָ��.
		[out]:
�������ͣ�
����:
    Eric2002-07-08   //### Add by YTLI at 2002/08/12
�޸�:
************************************************************************************/
CFakeRecNos::CFakeRecNos(CPages* pPages)
{
	m_pPages = pPages;
#ifdef MarkWay_Encrypt  //�Ǽ��ܰ汾
	BYTE *pData = (unsigned char *)&m_pPages;
	for (int i= 0;i<4 ;i++)
	{
		*(pData+i) = *(pData+i)^tlcfgBuf[i];
	}
#endif


    char szTempFile[MAX_PATH];
	GetTempFile(szTempFile);
	m_pTempFile = new TempFile(szTempFile,TCLFile::modeReadWrite|TCLFile::modeCreate);
	m_pTempFile->m_pFakeRecNos = this ;


	m_pOriginData = new CFileOperation;
#ifdef MarkWay_Encrypt  //�Ǽ��ܰ汾
	BYTE *pData2 = (unsigned char *)&m_pPages;
	for ( i= 0;i<4 ;i++)
	{
		*(pData2+i) = *(pData2+i)^CurFileBuf[i];
	}
#endif
	//m_pIOFields = new CFields(pPages);//### 2002-9-5 Eric
	m_pIOFields_ = pPages->m_pFields;

}


/************************************************************************************
�������ƣ�
����˵��:
    ����ԭʼ����Դ�ĸ���,�ڶ�δ֪��������Դʱ��.ͬʱ���������Ӧ��pages�ĸ���
��ϸ����:
�������:
        [in]:
		    1.nCount:��ԭ��������ƫ�� ��ԭ������-ʵ�ʸ���
		[out]:
�������ͣ�
����:
    //###_Mod 2002-10-15 zlq
�޸�:
************************************************************************************/
bool CFakeRecNos::AdjustOriginCount(int nOffset)
{
	CFakeRecNo* pFakeRecNoTemp;
	int nstTlCount = 0;
	int nstTempCount = 0;
	POSITION pos = m_FakeRecNoList.GetHeadPosition();
	while(pos)
	{
		pFakeRecNoTemp = (CFakeRecNo*)m_FakeRecNoList.GetAt(pos);
		if( pFakeRecNoTemp->m_nStoreType == storeTempFile )
		{
			nstTempCount += pFakeRecNoTemp->m_lngRecCount;
		}
		else if( pFakeRecNoTemp->m_nStoreType == storeTlFile )
		{
			nstTlCount += pFakeRecNoTemp->m_lngRecCount;
		}
		else 
		{
			//===temp AfxMessageBox("bool CFakeRecNos::AdjustOriginCount(int nCount)����������!");
		}
		m_FakeRecNoList.GetNext(pos);
	}

	//Ҳ�������еı���������
	//assert(pFakeRecNoTemp->m_nStoreType == storeTlFile);
	if (!m_pPages->AdjustOriginCount(nOffset))
	{
		//assert(0);
	}
	
	pFakeRecNoTemp->m_lngRecCount= pFakeRecNoTemp->m_lngRecCount - nOffset;
	//assert(pFakeRecNoTemp->m_lngRecCount>=0);


	return true;
}



int CFakeRecNos::GetFileVersion()
{
	return m_pOriginData->GetFileVersion();
}

void CFakeRecNos::Print()//===temp
{
#ifdef _DEBUG
	POSITION posRecNo       = m_FakeRecNoList.GetHeadPosition();
	long lBlockStart    =  0;//��ǰ������α��¼��� ����α��¼��
	long lBlockEnd      =  0;//��ǰ������α��¼��� ����α��¼��
/*
		//===temp TRACE("***********�ڵ�����*************** \r\n");;
		//===temp TRACE("��ʼ\t����\t����\tλ��\t�汾 \r\n");;*/
	
	//do//����һ����¼��		
	while(  posRecNo)
	{	
		CFakeRecNo* pFakeNode        = (CFakeRecNo*)m_FakeRecNoList.GetAt(posRecNo);	
		//ȷ���ü�¼�����β��¼
		lBlockStart    = lBlockEnd + 1;        
		lBlockEnd     += pFakeNode->m_lngRecCount;
		////===temp TRACE(" ( %d \t  %d )\t%d\t%d\t%d\r\n",lBlockStart,pFakeNode->m_lngRecCount,pFakeNode->m_nStoreType,pFakeNode->m_lngPosition,pFakeNode->m_nVersion);

		m_FakeRecNoList.GetNext(posRecNo);
	}
	////===temp TRACE("************************** \r\n");;
#endif
	
}
