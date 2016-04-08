// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "cfakerecnos.h"
#include "cpages.h"
#include "cfileoperation.h"

#include "fieldlist.h"
#include "../../Public/common.h"

extern bool IsCrack;

#ifdef _UNIX
#include <time.h>
UINT GetTempFileName(const char * pathname,const char * prefix,
			UINT unique,char * output){
	clock_t clk = clock();
	time_t curt = time(0);
	sprintf(output,"/tmp/~TLTemp_%u_%u",curt,clk);
}

#endif



/************************************************************************************
�������ƣ�
    CPages::CPages
����˵����
    ���캯��
��ϸ���ͣ�
���������
�������ͣ�
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
CPages::CPages()
{
	
	m_nSaveVersion = 0;//Ĭ��Ϊ���°汾

	m_cDateConvertType = ddmmmyyyy_line;//===temp
	m_ntemp = 0; //===temp
	m_nWorkPageCount =0; 	 //��ǰ����ҳ����


	m_nLogicCount = 0;//m_pFakeRecNos->m_pFile->m_Header.GetRecordCount();//���߼���¼��
	m_lngLastRecNo=0;		 //�ϴ�ʹ�ù��ļ�¼��,�뵱ǰ��¼�űȽ�,�����ж��û��ƶ��ķ���.
	m_nCurRecNo =0;		     //��ǰ��¼��
	m_nOldCurRecNo = 0;		 //insert,appendʱ,�����¼��,����cancelʱ�ָ�.

	m_nAlertRange =50;		 //Ԥװ��Χ	
	m_nCriticalRecCount = 20;//��ҳ�����ƶ�ʱ,��ҳ�滹�м�����¼ʱ,ҪԤװ.

	//��pages����������ҳ����,��������
	SetMaxPageCount(5);
	//Ԥװ������ֵ.Ԥװ�����ֵ.//###_Mod 2002-9-12 zlq
	SetNormalCountPerPage(IncreaseSize);

	m_pFakeRecNos=0; 		 //ָ��α��¼�ż���ָ��					 ???
	m_pFields=new CFields(this);//### 2002-9-5 Eric 			 //ָ���ֶμ�,����pages��ֻ��һ���ֶμ�	 ???
#ifdef MarkWay_Encrypt  //�Ǽ��ܰ汾
	if (IsCrack)
	{//������ƽ�
		m_pFields = m_pFields +1;
	}
#endif


    m_pFakeRecNos = new CFakeRecNos(this);	   //α��¼�����ָ��.
}
/************************************************************************************
�������ƣ�
    CPages::~CPages
����˵����
    ��������
��ϸ���ͣ�
���������
�������ͣ�
������
    Eric2002-07-05
�޸�: 
************************************************************************************/
CPages::~CPages()
{	
	//delete m_pRUMod;
	Clear();

	delete m_pFields;

	delete m_pFakeRecNos;

}
/************************************************************************************
�������ƣ�
    CPages::SetMaxPageCount
����˵����
    ����ʹ�õ�ҳ����
��ϸ���ͣ�
    ������ñ���ʹ��ҳ��С
���������
    [in]    count ��Ҫ���õ�ҳ����
�������ͣ�
    bool:	�����Ƿ�ɹ�
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
bool CPages::SetMaxPageCount(int count)
{//###_Mod 2002-9-12 zlq
	if (count<2)
	{
		//===temp AfxMessageBox("ҳ���������С��2��!");
		return false;
	}
	//#_�޸� 2002-11-13 $ 14:16:49 zlq λ�õ���
	m_nMaxPageCount = count;
	if(count < m_nWorkPageCount)
	{//��Сҳ����
		while (count <m_nWorkPageCount)
		{
			CPage * pPage = GetPhysicalEmptyPage();
			delete pPage;
			m_nWorkPageCount--;
			//m_Depot.Clear();
		}			
	}

	m_PageArraySort.SetTotalSize(count);
	
	return true;
}
/************************************************************************************
�������ƣ�
    CPages::SetCurRecNo
����˵����
	���õ�ǰ�ļ�¼�ţ��õ�ָ����¼�ŵ����ݣ�
��ϸ���ͣ�
    1.�õ���¼���ڵ�ҳ��
	2.��������ҳ��Ϊ�ҳ��
	3.�ƶ���ָ���ļ�¼��
���������
    [in]RecNo��	���ô˼�¼��Ϊ��ǰ��¼��
�������ͣ�
    
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
bool CPages::SetCurRecNo(int RecNo)
{
	if (RecNo<=0)
	{
		First();
		return false;
	}
	////===temp TRACE("CPages::SetCurRecNo��ǰ��¼��:%d\r\n",RecNo);===tempG
	CPage * pCurPage=0; //��ǰҳ��ָ��

	//�����߼���¼��,���������ڵ�ҳ,(���е�ҳ�漯�в����˼�¼ʱ,Ҫ�����ȴ�)
	pCurPage = GetRecNoPageInNormal(RecNo);
	if (pCurPage->m_bReachEndError)
	{
		pCurPage->m_bReachEndError = false;
		Last();
		//===temp TRACE("ĩβ��¼��:%d\n",GetRecordCount());
		return false;
	}
	//�Ѵ�ҳ��Ϊ��ǰҳ,������״̬ΪpgNormal
 	//pCurPage->SetPageState(pgNormal);
	//SetPagePosition(pCurPage,1);
	//#_�޸� 2002-11-12 $ 18:14:06 zlq
	m_PageArraySort.SetCurPage(pCurPage);
	pCurPage->HangFields(m_pFields);//�ڲ��Ѿ�����!

	//�ƶ���ָ���ļ�¼��
	int lngRecNoOffset = RecNo - pCurPage->m_lngLStartRecNo;//ҳ�еļ�¼λ��ƫ��. 
	pCurPage ->MoveBy(lngRecNoOffset,FromBegin);

	//��ǰ��¼����ΪRecNo
	m_nCurRecNo = RecNo;

	m_bIsBof = false;//###_Mod 2002-10-14 zlq
	m_bIsEof = false;

	return true;
}

/************************************************************************************
�������ƣ�
    CPages::GetUsePageCount
����˵����
    ���ػҳ����
��ϸ���ͣ�
���������
�������ͣ�
    int
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
int CPages::GetUsePageCount()
{
	return m_nWorkPageCount;
}
/************************************************************************************
�������ƣ�
    CPages::QueryRecNoState
����˵����
	�õ���¼�Ŷ�Ӧ�����������ڵ�״̬��
��ϸ���ͣ�
	�����¼���״̬��
	��������״̬���ȴ�����װ�룬���ݵ��룬�ȴ�����д�أ�����д��״̬��û��װ�루�������ȴ�����װ�룬
	���ݵ��룬�ȴ�����д�أ�����д��״̬��
	(�ο�ҳ��״̬��
	typedef byte  CPageState;
	#define pgNormal       0x01
	#define pgWaitLoad     0x02  //�ȴ�װ������,��ʱ��ҳ��Ӧ��Ϊ�����ҳ
	#define pgLoading      0x03  //����װ������,����������߳�װ��
	#define pgWaitUnLoad   0x04  //�ȴ�ж������,����Ӧ�����Ѿ��޸�״̬
	#define pgUnLoading    0x05  //����ж������,����������߳�ж��
	#define pgInDisk       0x06  
	����:
	�������ΪpgInDisk���ҳ���ڴ�����,�������,������Ҫ������߼���¼���볤��,
	��StartRecNo��Count��Ч.�����������,�򷵻ظ�ҳ�ĵ�ַ��InPage��ָ��
	�㷨:	����ҳ�漯 ,�����м�¼���ҳ���ӽ�RecNo ������(����,С��)��¼��,����û�ʹ�ü�¼���ķ�����
	����.���������ò�����ֵ.
	ע�⣺���������߼���Ϊ��ҳ��ҳ
���������
    [in]RecNo��			 Ҫȡ�ô˼�¼�ŵ�ҳ��״̬
    [in,out]InPage��	 ����ҳ�漯��ʱ,�õ����õ�һ��ҳ��
    [in,out]StartRecNo�� Ҫװ��Ŀ�ʼֵ
    [in,out]Count��		 Ҫװ��ĸ���
�������ͣ�
    
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
bool CPages::QueryRecNoState( int RecNo, CPage*& InPage,int& StartRecNo,int& Count)
{//false �ڴ���  true �ҵ�

	int lngLargerAndNearest,lngSmallerAndNearest;
	lngSmallerAndNearest=0;//�ȴ���ļ�¼��(RecNo)С���������м�¼��������һ��. "С�����"
	lngLargerAndNearest=this->GetRecordCount()+1;/*�ȴ���ļ�¼��(RecNo)����������м�¼������С��һ��. "������С" */		 

	if(RecNo <= lngSmallerAndNearest || RecNo >= lngLargerAndNearest)
	{  //��¼������Χ.
		//===temp AfxMessageBox("��¼������Χ.CPages::QueryRecNoState");
		return false;
	}
	//#_�޸� 2002-11-12 $ 16:38:54 zlq
	InPage = m_PageArraySort.FindIndexByRecNo(RecNo,lngSmallerAndNearest,lngLargerAndNearest);
	if (InPage)
		return true;

	//�жϷ�����.
	if(m_lngLastRecNo > RecNo)
	{ //���Ϸ�,	��װ���¼Ϊ����ָ��ļ�¼������
		if(RecNo-lngSmallerAndNearest  >  m_nMaxCountPerPage)
		{	//Ҫװ��ļ�¼��m_nMaxCountPerPage������,����һ�β���װ��,
			//���ֻװ m_nNormalCountPerPage��.
			Count=	m_nNormalCountPerPage;
			StartRecNo= RecNo-m_nNormalCountPerPage+1;
		}
		else
		{//Ҫװ��ļ�¼��m_nMaxCountPerPage����.
			if(lngLargerAndNearest - lngSmallerAndNearest-1 >  m_nMaxCountPerPage)
			{//����"һ��װ��ʹ��¼����",��ʱRecNoλ��ҳ���м�.
				StartRecNo =  lngSmallerAndNearest+1;
				Count = m_nMaxCountPerPage;
			}
			else
			{//һ��װ��,ʹ��¼����
				StartRecNo =  lngSmallerAndNearest+1;
				Count = lngLargerAndNearest - lngSmallerAndNearest-1 ;
			}
		}
	}
	else
	{ //���·�, ��װ���¼Ϊ����ָ��ļ�¼������
		if(lngLargerAndNearest - RecNo>m_nMaxCountPerPage)
		{	 //������һ����װ��,����ֻװ��Normal����¼.
			StartRecNo= RecNo;
			Count=	m_nNormalCountPerPage;
		}
		else
		{	
			if(lngLargerAndNearest - lngSmallerAndNearest-1 >  m_nMaxCountPerPage)
			{//����"һ��װ��ʹ��¼����",��ʱRecNoλ��ҳ���м�.
				StartRecNo =  lngLargerAndNearest -m_nMaxCountPerPage; 
				Count = m_nMaxCountPerPage;
			}
			else
			{//һ����װ��.ʹ��¼����.
				StartRecNo =  lngSmallerAndNearest+1;
				Count = lngLargerAndNearest -lngSmallerAndNearest -1;
			}
		}
	}
	return false;
}

/************************************************************************************
�������ƣ�CPages::UnLoad
����˵����
    ��ҳ��������,ͬʱ���������ȸ�ҳ���ҳ���α��ʼ��¼��
��ϸ���ͣ�
	�����ǵ�ͨ�������Կ��Ը��ݸ�ҳ����ֵ���������ȸ�ҳ���ҳ���α��ʼλ��
	m_lngLOldStartRecNo����ͨ����������ÿһ��ҳ���AdjustFakeRecNo������
	ע�⣺����ҳ�棬���ܴ���ʲô״̬
���������
    [in]CPage*��ҪUNLOAD ��ҳ��
�������ͣ�
    void
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
void CPages::UnLoad(CPage* pPage,bool IsNeedFeedback)
{
	int nOldfake = pPage->m_lngFakeStart;	   //pPage->UnLoad ���ı��ֵ
	int lngOffset =  pPage->UnLoad(IsNeedFeedback);	//ƫ����Ϊ�߼���¼����α��¼��֮��.

	AdjustFakeRecNo(pPage->m_lngLStartRecNo,nOldfake, lngOffset) ;
}
/************************************************************************************
����˵����
	�õ��߼���¼����α��¼�ŵ�ƫ��   �߼��� - α��¼��	 
��ϸ���ͣ�
	����ҳ�漯,�ҵ��׼�¼��¼��С��LogicRecNo����ӽ���ҳ��.
	�׼�¼�ű����� lngSmallerAndNearet��.
	������������ LogicRecNo ��������ҳ����ʱ��Ч.	!!!
���������
    [in]LogicRecNo���߼���¼��,�ݴ˵õ�ƫ��
�������ͣ�
    int	     �߼���¼����α��¼�ŵ�ƫ��
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
int CPages::GetLogicToFake(int LogicRecNo)
{
	int lngOffset = 0;
	CPage* pPage,*pTargetPage = 0;
	
	int lngSmallerAndNearet = 0;; //�׼�¼����ӽ��ҵļ�¼(��LogicRecNoС)
	int lngSANFake = 0; 

	//����ҳ�漯,�ҵ��׼�¼��¼��С��LogicRecNo����ӽ���ҳ��.�޸�lngSmallerAndNearet��ֵ
	for (int i=0;i<m_PageArraySort.GetPageSize();i++)
	{
		pPage = m_PageArraySort.GetPageAt(i);
		
		if (pPage->IsPhysicalEmpty())//�����ҳ���ܼ�������.
			continue;
		if(pPage->m_lngLStartRecNo <= LogicRecNo)
		{//��ҳ�еļ�¼�ļ�¼�Ŷ��� LogicRecNo С,����Щҳ����ѡ�����ļ�¼��
			if(lngSmallerAndNearet < pPage->m_lngLStartRecNo)
			{
				lngSmallerAndNearet = pPage->m_lngLStartRecNo;
				lngSANFake = pPage->m_lngFakeStart;
				pTargetPage = pPage;
			}
			if (lngSmallerAndNearet == pPage->m_lngLStartRecNo)
			{
				if (lngSANFake < pPage->m_lngFakeStart)
				{
					lngSmallerAndNearet = pPage->m_lngLStartRecNo;
					lngSANFake = pPage->m_lngFakeStart;
					pTargetPage = pPage;			
				}
			}
		}
	}

	//��¼����ҳ�漯��
	if(pTargetPage && LogicRecNo > pTargetPage->m_lngLStartRecNo + pTargetPage->GetLogicCount()-1)
	{
		// pTargetPage�б�������ӽ���һ��ҳ��ָ��,�������һ���߼���¼�������һ��α��¼�ŵĲ�ֵ,�ɵó����ֺŵ�ƫ��.
		lngOffset =(pTargetPage->m_lngLStartRecNo + pTargetPage->GetLogicCount()-1)     /*���һ���߼���¼��	*/ \
				- (pTargetPage->m_lngFakeStart + pTargetPage->m_nOriginLoadCount-1); //���һ��α��¼��

		return lngOffset ;// �߼��� - α��¼��
	}

	//��¼����ҳ�漯��,δ����
	return lngOffset;
	//========================================================================
}
/************************************************************************************
�������ƣ�

����˵����
    �õ���ǰһ����ҳ
��ϸ���ͣ�
	1.�����ǰϵͳҳ��û�дﵽָ���ĸ���,��������һ����ҳ.
	2.����Ѿ��ﵽָ���ĸ���,�����GetRecentNoUse()�õ�һ�����õ�ҳ��
	������Ƿ���Ҫж�ء�Ȼ�󷢳�ҳ���󣬲��ȴ����أ���Ϊ�Ƕ��̣߳�����
	�����õ�WaitForSingleObject API������Unload�д��ݵȴ��Ĳ���
    
���������
�������ͣ�
    CPage*
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
CPage* CPages::GetPhysicalEmptyPage()
{
	CPage * pPage;
	if(m_nMaxPageCount>m_nWorkPageCount)
	{
		//����һ����ҳ
		pPage =  new CPage(this);
		m_nWorkPageCount++;
		m_pFields->SetCurVersion();
		
		pPage->IniFieldCnt(m_pFields);
		pPage->HangFields(m_pFields);
		return pPage;
	}
	
	int nIndex;
	pPage = m_PageArraySort.GetAvailablePage(&nIndex);
	if(pPage->m_bIsModify)
	{
		WaitForUnLoad(pPage);
	}

	if (pPage->GetLogicCount())
	{
		m_PageArraySort.MoveToEmptyArea(nIndex);		
	}

	pPage->EmptyTable();
	pPage->SetLogicCount(0);
	return pPage;
}

/************************************************************************************
�������ƣ�
    CPages::Next
����˵����
    �߼���¼���ƶ�

************************************************************************************/
int CPages::Next()
{
	//PrintPages();
	if (GetRecordCount()==0)
	{
		m_bIsEof = true;
		m_bIsBof = true;
		return false;
	}
	if(m_nCurRecNo == GetRecordCount())
	{
		//�ѵ�ĩ��¼
		m_bIsEof = true;//###_Mod 2002-10-14 zlq
		m_bIsBof = false;
		
		return false;
	}
	else
	{
		m_bIsEof = false;
		m_bIsBof = false;
	}

	//���,���ü�¼��״̬
/*
		CDataSetState dsState;
		dsState = dsBrowse;
		if(!CheckStateAndPost(dsState))
		{
			return 0;
		}
	
	m_FState = dsState;
*/
	m_lngLastRecNo = m_nCurRecNo;

	CPage *pPage;
	pPage = GetCurPage();
	if(pPage->IsLogicalEmpty() || pPage->m_nCurNo == pPage->GetLogicCount()-1  )
	{
		//�ѵ���ҳĩ��¼,Ѱ����һ��ҳ(��װ��)
		SetCurRecNo(++m_nCurRecNo);
		return 0;
	}
	//δ����ҳĩ��¼
	//pPage->PrintCurLine();
	pPage->Next();
	


	m_nCurRecNo ++;	 

	return 0;
}
/************************************************************************************
�������ƣ�
    CPages::Prior
����˵����
    �߼���¼���ƶ�
************************************************************************************/
int CPages::Prior()
{
	if (GetRecordCount()==0)
	{
		m_bIsEof = true;
		m_bIsBof = true;
		return false;
	}

	if(m_nCurRecNo == 1)
	{
		m_bIsBof = true;//###_Mod 2002-10-14 zlq
		//�ѵ��׼�¼
		return false;
	}
	else
	{
		m_bIsBof = false;//###_Mod 2002-10-14 zlq
		m_bIsEof = false;
	}

	//���,���ü�¼��״̬	 
/*
		CDataSetState dsState;
		dsState = dsBrowse;
		if(!CheckStateAndPost(dsState))
		{
			return 0;
		}
		m_FState = dsState;*/
	

	CPage *pPage;
	pPage = GetCurPage();
	if(pPage->m_nCurNo == 0)
	{
		//�ѵ���ҳ�׼�¼,Ѱ����һ��ҳ(��װ��)
		SetCurRecNo(--m_nCurRecNo);
		return 0;
	}

	//δ����ҳ�׼�¼
	pPage->Prior();



	m_nCurRecNo --;
	m_lngLastRecNo = m_nCurRecNo;

	return 0;
}
/************************************************************************************
�������ƣ�
    CPages::MoveBy
����˵����
    �߼���¼���ƶ�
��ϸ���ͣ�
    
���������
    [in]iSkipLen���ƶ��Ĳ���,��Ϊ����.
    [in]MoveFlags���ƶ�����ʼλ��.1ͷ2β3��ǰλ��
�������ͣ�
    
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
int CPages::MoveBy(int iSkipLen ,CFromPos MoveFlags)
{
	int nRecNoToMoveTo = 0;	 //���潫Ҫ�ƶ������߼���¼��
	//CDataSetState dsState;
	//dsState = dsBrowse;

	//������
	switch(MoveFlags)
	{
	case FromBegin:
		if(iSkipLen<0)
			return false;

		if(iSkipLen >= m_nLogicCount)
		{
			//�������ĩ��¼
			Last();
			return false;//???
		}
		nRecNoToMoveTo = iSkipLen+1;

		SetCurRecNo(nRecNoToMoveTo);
		break;
	case FromCur:
		
		//���,���ü�¼��״̬
/*
				if(!CheckStateAndPost(dsState))
				{
					return 0;
				}
				m_FState = dsState;*/
		

		if(iSkipLen < 1-m_nCurRecNo)
		{
			//��������׼�¼
			First();
			return false;
		}
		if(iSkipLen > m_nLogicCount-m_nCurRecNo)	  
		{
			//�������ĩ��¼
			Last();
			return false;//???
		}
		nRecNoToMoveTo = m_nCurRecNo+iSkipLen	;
		SetCurRecNo(nRecNoToMoveTo);
		break;
	case FromEnd:
		if(iSkipLen>0)
			return false;//???
		

		if(iSkipLen<1-m_nLogicCount)
		{
			//��������׼�¼
			First();
			return false;//???
		}
		nRecNoToMoveTo = m_nLogicCount+iSkipLen	;
		SetCurRecNo(nRecNoToMoveTo);
		break;
	}

	m_lngLastRecNo = m_nCurRecNo;
	return 1;
}
/************************************************************************************
�������ƣ�
    CPages::First
����˵����
    �Ƶ��߼���һ����¼
************************************************************************************/
int CPages::First()
{
	if (GetRecordCount()==0)//###_Mod 2002-9-16 zlq
	{
		m_bIsBof = true;
		m_bIsEof = true;
		return 0;
	}


	SetCurRecNo(1);
	m_lngLastRecNo = m_nCurRecNo;

	m_bIsBof = true;//###_Mod 2002-10-14 zlq
	if (GetRecordCount()==0)
		m_bIsEof = true;
	else
		m_bIsEof = false;
	return true;
}
/************************************************************************************
�������ƣ�
    CPages::Last
����˵����
    �Ƶ��߼����һ����¼
************************************************************************************/
int CPages::Last()
{
	if (GetRecordCount()==0)//###_Mod 2002-9-16 zlq
	{
		m_bIsBof = true;
		m_bIsEof = true;
		return 0;
	}

/*
		CDataSetState dsState;
		dsState = dsBrowse;
		if(!CheckStateAndPost(dsState))
		{
			return 0;
		}
		m_FState = dsState;*/
	


	SetCurRecNo(m_nLogicCount);
	m_lngLastRecNo = m_nCurRecNo;

	m_bIsEof = true;//###_Mod 2002-10-14 zlq
	if (GetRecordCount()==0)
		m_bIsEof = true;
	else
		m_bIsBof = false;
	return true;
}

/************************************************************************************
�������ƣ�
    CPages::GetCurPage
����˵����
    �õ���ǰҳ
��ϸ���ͣ�
���������
    [in,out]��
�������ͣ�
    CPage* ��ǰҳ
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
CPage* CPages::GetCurPage()
{
	CPage * pPage;
	if (m_PageArraySort.GetPageSize()==0)
	{
		return 0;
	}
	pPage = m_PageArraySort.GetCurPage();

	return pPage;
}
/************************************************************************************
�������ƣ�
    CPages::GetRecNo
����˵����
	���ص�ǰ��¼��
��ϸ���ͣ�
���������
�������ͣ�
    int: ��ǰ�߼���¼��
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
int CPages::GetRecNo()
{
	return m_nCurRecNo;
}


/************************************************************************************
�������ƣ�
    CPages::Append
����˵����
    ��ҳ�漯����ΪAppend״̬,��ǰ��¼����Ϊ-1
************************************************************************************/
#ifdef TEMP_Reserve/*
bool CPages::Post()
{
	//���õ�ǰҳ��ķ���
	CPage *pCurPage;
	
	switch(m_FState)
	{
	case dsBrowse:
		break;
	case dsInsert:
		pCurPage = GetCurPage();
		AdjustLogicRecNo(pCurPage->m_lngLStartRecNo,1);//730 add ע��Ҫ��post֮ǰ,�����ҳ
		
		//��ǰΪ�ڴ�
		pCurPage->Post(storeMemory,0);//###_Mod 2002-9-17 zlq
		m_bIsBof = false;
		m_bIsEof = false;
		m_nCurRecNo = m_nOldCurRecNo;
		
		if (m_bIsEnableRedoUndo)
		{//#_S 2003-5-28 $ 13 zlq ��������
			pCurPage->BindFields();
			int nRecordLength = m_pFields->QuerySaveLength();
			BYTE *pData = new BYTE[nRecordLength];
			pCurPage->SetRecord(pData);
			CRURecordAdd*pRU = new CRURecordAdd(this);
			pRU->SetData(m_nCurRecNo,pData,nRecordLength);
			m_ruManager.Push(pRU);
		}
		
		m_nOldCurRecNo = 0;
		m_nLogicCount++;
		if (m_nLogicCount==1)
		{//#_�޸� 2002-11-13 $ 8:47:22 zlq
			m_PageArraySort.MoveToWorkArea(pCurPage);//�µ�
		}
		break;
	case dsAppend:
		pCurPage = GetCurPage(); //append ǰҳ������Ϊ���һҳ
		AdjustLogicRecNo(pCurPage->m_lngLStartRecNo,1);//730 add ע��Ҫ��post֮ǰ,�����ҳ
		//��ǰΪ�ڴ�
		pCurPage->Post(storeMemory,0);//###_Mod 2002-9-17 zlq
		m_bIsBof = false;
		m_bIsEof = false;
		
		m_nOldCurRecNo = 0;
		m_nLogicCount++;
		m_nCurRecNo = m_nLogicCount;

		if (m_bIsEnableRedoUndo)
		{//#_S 2003-5-28 $ 13 zlq ��������
			pCurPage->BindFields();
			int nRecordLength = m_pFields->QuerySaveLength();
			BYTE *pData = new BYTE[nRecordLength];
			pCurPage->SetRecord(pData);
			CRURecordAdd*pRU = new CRURecordAdd(this);
			pRU->SetData(m_nCurRecNo,pData,nRecordLength);
			m_ruManager.Push(pRU);
		}
		
		if (m_nLogicCount==1)
		{//#_�޸� 2002-11-13 $ 8:47:22 zlq
			m_PageArraySort.MoveToWorkArea(pCurPage);//�µ�
		}
		break;
	case dsEdit:
		pCurPage = GetCurPage();
		//��ǰΪ�ڴ�
		pCurPage->Post(storeMemory,0);//###_Mod 2002-9-17 zlq

		if (m_bIsEnableRedoUndo)
		{//#_S 2003-5-28 $ 13 zlq ��������
			pCurPage->BindFields();
			int nRecordLength ;//= m_pFields->QuerySaveLength();
			//BYTE *pData = new BYTE[nRecordLength];
			//pCurPage->SetRecord(pData);
			CRURecordMod*pRU = new CRURecordMod(this);
			BYTE*pData = m_pRUMod->MoveExistData(nRecordLength );
			if(pData==NULL)
			{
				//===temp AfxMessageBox("�༭ʱ,����������������,�뱨�湩Ӧ��!");
			}
			pRU->SetData(m_nCurRecNo,pData,nRecordLength);
			m_ruManager.Push(pRU);
		}

		m_bIsBof = false;
		m_bIsEof = false;
		break;
	default:
		break;
	}
	if(m_pFields->GetLastPostVersion()<m_pFields->GetLastVersion())         //mmmm
	{
		m_pFields->m_nLastPosVersion = m_pFields->GetLastVersion();
		m_pFakeRecNos->m_pIOFields->m_nLastPosVersion = m_pFakeRecNos->m_pIOFields->GetLastVersion();
	}
	m_FState = dsBrowse;

	return true;
	//���������� ʱʹ������  ��ʱ��������� 2002-12-31 $ 11:02:58 zlq

}*/
#endif
/************************************************************************************
�������ƣ�
    CPages::Delete
����˵����
    ɾ��һ����¼
��ϸ���ͣ�
    ���õ�ǰҳ��ķ���
���������
�������ͣ�
    
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
bool CPages::Delete()
{
	//���õ�ǰҳ��ķ���
	CPage *pPage;
	pPage = GetCurPage();

	//�����������ҳ���������¼��
	AdjustLogicRecNo(pPage->m_lngLStartRecNo,-1);



	pPage->_DeleteEx() ;

	if(m_nCurRecNo == m_nLogicCount)
	{
		if (m_nLogicCount==1)
		{//#_�޸� 2002-11-13 $ 8:55:51 zlq
			m_PageArraySort.MoveToEmptyArea(0);
		}

		 m_nCurRecNo--;
	}
	if (pPage->GetLogicCount()==0 && pPage->m_nOriginLoadCount==0)
	{//��ҳȫ������������ȫ��ɾ����
		pPage->m_bIsModify = false;
	}

	m_nLogicCount--;
	if(m_nCurRecNo)
		SetCurRecNo(m_nCurRecNo);
	else
	{//###_Mod 2002-11-5 zlq
		m_bIsBof = true;
		m_bIsEof = true;
	}
	return true;
}
/************************************************************************************
�������ƣ�
    CPages::Cancel
����˵����
    �ָ�ҳ�漯Ϊ���״̬,��ǰ��¼�ָ�.
��ϸ���ͣ�
    //���õ�ǰҳ��ķ���
���������
    [in,out]��
�������ͣ�
    
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
bool CPages::Cancel()
{
	//���õ�ǰҳ��ķ���

	bool result = 0;
	return result;
}

/************************************************************************************
�������ƣ�
    CPages::GetRecordCount
����˵����
	�������ݿ��¼�ĸ���������û��װ���ڴ�ļ�¼
��ϸ���ͣ�
���������
�������ͣ�
    int	:��¼����
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
int CPages::GetRecordCount()
{
	if (m_nLogicCount==UnknownCount)
	{
		SetCurRecNo(UnknownCount);
	}
	return m_nLogicCount;
}


/************************************************************************************
�������ƣ�
����˵����
	���õ�ǰ����¼�����е��ֶε�������Ϣ��һ��ָ���Ļ�������
��ϸ���ͣ�
	������֯��ָʾ��Ϣ���֣������ֶΣ�ÿ���ֶ�һ�ֽڣ�+����?	�֣������ֶΣ�����ĳЩ�ֶ�Ҳû�����ݣ�ǰ���ܹ����ʱ��
���������
    [in,out]lpData����������ָ��
�������ͣ�
    int	�������ݳ���
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
int CPages::SetRecord(LPBYTE lpData)
{
	CPage *pPage;
	pPage = GetCurPage();
	return pPage->_SetRecord(lpData);
}

/************************************************************************************
�������ƣ�
    CPages::GetRecord
����˵����
    �Ե�ǰ�ļ�¼���ݸ�ֵ����
��ϸ���ͣ�
	���ݼ��ڴ�֮ǰӦ��ͨ�����ڱ༭
	�����״̬������append,edit�ȷ�������
���������
    [in,out]lpData��������ָ��
    [in,out]count����Ч���������ݴ�С
�������ͣ�
    bool :�Ƿ�ɹ�
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
bool CPages::GetRecord(LPBYTE lpData, int& count)
{
	CPage *pPage;
	pPage = GetCurPage();
	return pPage->GetRecord(lpData,count);
}

/************************************************************************************
�������ƣ�
    CPages::GetRecord
����˵����
    �Ե�ǰ�ļ�¼���ݸ�ֵ����
��ϸ���ͣ�
	�����ݼ��ڴ�֮ǰӦ��ͨ�����ڱ༭�����״̬������append,edit�ȷ�����
	�˷���Ҫ�޸�ָ��
���������
    [in,out]lpData��
�������ͣ�
    bool :�Ƿ�ɹ�
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
bool CPages::GetRecord(LPBYTE& lpData)
{
	CPage *pPage;
	pPage = GetCurPage();
	return pPage->GetRecord(lpData);
}
/************************************************************************************
�������ƣ�

����˵����
    ����ĳһҳ��ҳ�漯�е�λ��
��ϸ���ͣ�
	nPosΪ1����pPage����ҳ�漯�е�һλ.
	pPage	���������:ҳ�漯������,ҳ�漯��û��.
���������
    [in] pPage ��ҳ���ָ��,����������λ��.
    [in] nPos ��λ��.
�������ͣ�
    bool �Ƿ�ɹ�.
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
/*//#_�޸� 2002-11-12 $ 18:12:45 zlq
bool CPages::SetPagePosition(CPage* pPage ,int nPos)
{
	
	POSITION posOld = m_pPageList.GetHeadPosition();
	POSITION posOldPrev = posOld;
	POSITION posDelete = 0;//ɾ����λ��
	POSITION posInsertBefore = 0;//�����ڴ�λ��ǰ�����
	int CurPos = 1;

	//���۵�ǰҳ�Ƿ��ı�,�ȶϿ��ֶμ���.
	CPage* pPageTemp =GetCurPage();
	if(pPageTemp)
		pPageTemp->HangFields(0);

	//Ѱ������λ��
	while(posOld)
	{
		CPage* ptemp = (CPage*)m_pPageList.GetNext(posOld);

		if (CurPos == nPos)
			posInsertBefore = posOldPrev; //�����λ��
		
		if (ptemp == pPage)
		{
			posDelete = posOldPrev;
			if (!posInsertBefore)
				nPos++;//ɾ����λ���ڲ����λ��֮ǰ.
		}

		posOldPrev = posOld;
		CurPos++;
	}

	if (posInsertBefore)
		m_pPageList.InsertBefore(posInsertBefore,(CObject*)pPage);
	else
		m_pPageList.AddTail(pPage);//���뵽��һ��λ��

	if (posDelete)
		m_pPageList.RemoveAt(posDelete);


	//�󶨵�һ��page��m_pFields;
	pPageTemp =GetCurPage();
	pPageTemp->HangFields(m_pFields);
	return true;
}
*/
/************************************************************************************
�������ƣ�
    CPages::WaitForUnLoad
����˵����
    �ȴ�ĳ��ҳ���ж��
��ϸ���ͣ�
	����ʱ��ҳ�Ѿ���Ч�����Ѿ�ж�ع���
	1���ñ�ҳ�漯��UnLoad�������ú����ڲ���Ҫ��������ҳ���α��¼��ַ��
	2���߳̽���ȴ�״̬
���������
    [in,out]CPage*���ȴ���ҳ��ָ��
�������ͣ�
    void
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
void CPages::WaitForUnLoad(CPage* pPage)
{
	if(pPage)
	{
		//PrintPages();
		UnLoad(pPage,true);
		//PrintPages();
	}	
}
/************************************************************************************
�������ƣ�
    CPages::WaitForLoad
����˵����
    �ȴ�ĳ��ҳ���װ��
��ϸ���ͣ�
	����ʱ��ҳ�Ѿ���Ч
	 1���ø�ҳ��load����
	 2���߳̽���ȴ�״̬
���������
    [in,out]pPage���ȴ���ҳ��,���ݽ�װ������
    [in]logicRecNo����ʼ�߼���
    [in,out]count��	   ����
�������ͣ�
    void
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
void CPages::WaitForLoad(CPage* pPage,int logicRecNo, int count )
{
	if(pPage)
	{
		pPage->Load(logicRecNo,count,true);
	}
}
/************************************************************************************
�������ƣ�

����˵����
	���ݴ���ļ�¼�ţ��õ����ڵ�ҳ�������ڴ����ڲ�װ�벢���ظ�ҳ��
��ϸ���ͣ�
	ע�⣺�ú���������ҳ��˳��ĵ���
	�Ȳ�ѯ�ü�¼��״̬QueryRecNoState
	1 �������ڴ��У��򷵻ظ�ҳ
	2 �ڶ����У�����WaitForInQueue(pPage : CPage*)���ظ�ҳ
	3 ��������ڴ�ҳ�У������GetPhysicalEmptyPage�õ�һ�����õĿ�ҳ,
	WaitForLoad�õ���ҳ
    
���������
    [in]RecNo���߼���¼��
�������ͣ�
    CPage* ����¼���ڵ�ҳ
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
CPage* CPages::GetRecNoPageInNormal(int RecNo)
{
	int  lngStartRecNo=0;   //���Ҫװ��,�ڴ˱���װ�����ʼ��
	int   nCount=0 ;		//���Ҫװ��,�ڴ˱���Ҫװ��ĸ���
	CPage * InPage=0;		//��¼���ڵ�ҳ
	CPage * pGotPage;		//Ҫ���ص�ҳ

	//��ѯ�˼�¼�ŵ�״̬---���1.��ҳ����,2.����ҳ����,������ʼ��¼�ż�����,�Ա�װ��.
	bool psPageState = QueryRecNoState(RecNo,InPage,lngStartRecNo,nCount);


	if (psPageState ==false)
	{
		//�ڴ���û������,����һ����ҳ,��װ������.
		CPage * pPageNeedLoad;
		pPageNeedLoad = GetPhysicalEmptyPage();	 
		WaitForLoad(pPageNeedLoad,lngStartRecNo,nCount);
		pGotPage = pPageNeedLoad;
	}
	else
	{
		pGotPage = InPage;
	}

	return 	pGotPage;
}


/************************************************************************************
�������ƣ�
    CPages::LoadFile
����˵����
    ��һ���ļ�
��ϸ���ͣ�
    ��һ�����ݿ��ļ�,װ����Ӧ���ֶνṹ���Լ���ʼ���ڲ������ݼ�����Ϣ,
	����α��¼�Ź����LoadFile�ɹ��󣬽��б��ֶνṹ�ĸ���
���������
    [in]lpszFileName���ļ���.
�������ͣ�
    bool :�Ƿ�ɹ�
������
    Eric2002-07-05
�޸�: 
************************************************************************************/
bool CPages::LoadFile(LPCTSTR lpszFileName)
{
	//���ԭ������,����ɾ��.
	Clear();

	//װ������,��
	if(!m_pFakeRecNos->LoadFile(lpszFileName))
	{
		return false;
	}
	//m_pFields->Clone(m_pFakeRecNos->m_pIOFields);
	m_pFields->SetCurVersion();

	m_nLogicCount = m_pFakeRecNos->m_pOriginData->GetRecordCount();//���߼���¼��
	First();//###_Mod 2002-10-14 zlq
	return true;
}


/************************************************************************************
�������ƣ�
    CPages::NewFile	����
����˵����
    �½�һ���ļ�.
��ϸ���ͣ�
    �����ֶ���Ϣ,ͬʱ���뽫ԭ�������,ͬʱ������ʱ�ļ�.
    
���������
    [in,out]��
�������ͣ�
    
������
    Eric2002-07-16
�޸�: 
************************************************************************************/
bool CPages::NewFile()
{
	//���ԭ������,����ɾ��.
	Clear();

	if(!m_pFakeRecNos->NewFile(m_pFields))
	{
		return false;
	}
	First();//###_Mod 2002-10-14 zlq
	return true;
}
/************************************************************************************
�������ƣ�
    CPages::Clear
����˵����
    ������е���Ϣ,�����ֶ���Ϣ.
��ϸ���ͣ�
    
���������
    [in,out]��
�������ͣ�
    
������
    //Eric2002-07-05
�޸�: 
************************************************************************************/
bool CPages::Clear()
{
	//assert(m_pFields);
	if(m_pFields&&(m_pFields->GetFieldCount()))
	{
		CPage * pPage;
		//#_�޸� 2002-11-12 $ 18:16:49 zlq
		//while(!m_pPageList.IsEmpty())
		for (int i=m_PageArraySort.GetPageSize()-1;i>=0;i--)
		{	
			//pPage = (CPage *)m_pPageList.RemoveHead();
			pPage = m_PageArraySort.GetPageAt(i);
			pPage->HangFields(m_pFields);
			delete pPage;
		}
		m_pFields->ClearAll();//���ҳ�漯�е�Ψһ���ֶ��б�.
	}

	m_pFakeRecNos->Clear();

	m_PageArraySort.Clear();//###_Mod 2002-11-11 zlq

	m_nLogicCount = 0;
	m_nCurRecNo = 0;
	m_nWorkPageCount = 0;

	return true;
}
/************************************************************************************
�������ƣ�
    
����˵����
    �������ݿⱣ��,�ȱ��浽һ��ʱ�ļ���,������.
	������ٴ����ݿ�.
��ϸ���ͣ�
    
���������
    [in]lpszFileName��������ļ���
	[out]pstrOutFile ����false ʱ�ұ���Ϊ�����ļ�ʱ��Ч, Ϊ����ʱ����ʱ�����Ϊ���ļ�����,���ڲ�����
�������ͣ�
    bool :�Ƿ�ɹ�
������
    Eric2002-07-05
�޸�: //#_�޸� 2002-12-18 $ 13:44:58 zlq
************************************************************************************/
bool CPages::SaveFile(LPCTSTR lpszFileName,CTString *pstrOutFile)
{
//--UNIX PORT #ifdef WINPLAT
	int nCurRecNo = GetRecNo();	//��ǰ��¼,�ٴδ�ʱ,Ҫʹ��.

	CFileOperation *pFileOperation = new CFileOperation;
	pFileOperation->m_Header.SetHeaderSize(64);
	pFileOperation->m_Header.SetFieldPosition(64);
	pFileOperation->m_Header.SetFieldCount(m_pFields->GetFieldCount());

	//�����ʱ�ļ���·��.
	char szPathName[MAX_PATH];
	strcpy(szPathName,lpszFileName);
	char * pTemp = (char *)szPathName + strlen(szPathName);
	while(*pTemp !=CHAR_SLASH) //--hehe,if no slash or invalid full pathname,program dump
	{
		pTemp--;
	}
	*pTemp = 0;

	//�����ʱ�ļ���.
	char szTempFile[MAX_PATH];
//--UNIX PORT
	UINT nResult = GetTempFileName(szPathName, _T("~TLTemp"),0,szTempFile);

    //assert(nResult);
	//���浽��ʱ�ļ���.
	pFileOperation->IniSaveFile(szTempFile,this,m_pFields);
	pFileOperation->SaveRecords((CPages*)this);

	//�������,�����������,�ͷ��ļ����,�Ա���и���.	
	delete pFileOperation;

	Clear();
	
	//���ļ���
	try
	{/*
        WIN32_FIND_DATA FindData;
		HANDLE hFileFile = FindFirstFile(lpszFileName , &FindData );
		if(hFileFile != INVALID_HANDLE_VALUE)
		{//���ļ��Ѵ���,Ҫ��ɾ��.
			FindClose(hFileFile);
			if (!DeleteFile(lpszFileName))
			{
				//DeleteFile(szTempFile);
				CTString strTempOut;
				RenameFile(szTempFile,lpszFileName,strTempOut);
				//���´��ļ�,��λ��ǰ��¼.
				LoadFile(strTempOut);
				if (nCurRecNo)//###_Mod 2002-9-16 zlq
					SetCurRecNo(nCurRecNo);
				*pstrOutFile = strTempOut;
				return false;
			}
		}
		else
			FindClose(hFileFile);
		
		MoveFile(szTempFile,lpszFileName);
		*/
		if( !_access(lpszFileName,0)){
			if( remove( lpszFileName)){ //--if failed
				CTString strTempOut;
				RenameFile(szTempFile,lpszFileName,strTempOut);
				//���´��ļ�,��λ��ǰ��¼.
				LoadFile(strTempOut);
				if (nCurRecNo)//###_Mod 2002-9-16 zlq
					SetCurRecNo(nCurRecNo);
				*pstrOutFile = strTempOut;
				return false;
			}
		}
		rename(szTempFile,lpszFileName);
	}
	catch(... )
	{
		
		CTString strTempOut;
		RenameFile(szTempFile,lpszFileName,strTempOut);
		//���´��ļ�,��λ��ǰ��¼.
		LoadFile(strTempOut);
		if (nCurRecNo)//###_Mod 2002-9-16 zlq
			SetCurRecNo(nCurRecNo);
		*pstrOutFile = strTempOut;
		return false;
	}
	

	//���´��ļ�,��λ��ǰ��¼.
	LoadFile(lpszFileName);
	if (nCurRecNo)//###_Mod 2002-9-16 zlq
		SetCurRecNo(nCurRecNo);


	return true;

}
/************************************************************************************
�������ƣ�
    
����˵����
    �ļ�����.
��ϸ���ͣ�
    
���������
    [in]strTempFileName����ʱ�ļ���
	[in]strTargetFileName��Ҫ�����ԭ���ļ���
	[Out]strOutFileName�����ձ�����ļ���
�������ͣ�
    bool :�Ƿ�ɹ�
������
�޸�:
************************************************************************************/					
bool CPages::RenameFile(CTString strTempFileName,CTString strTargetFileName, CTString &strOutFileName)
{


	CTString filePath = strTargetFileName;
	int iPos = filePath.ReverseFind(CHAR_SLASH);
	if(iPos != 0)
		filePath.Delete(iPos+1,filePath.GetLength()-iPos-1);

	CTString filename = strTargetFileName;
	iPos = filename.ReverseFind(CHAR_SLASH);
	if(iPos != 0)
		filename.Delete(0,iPos+1);

	iPos = filename.ReverseFind('.');
	if(iPos != 0)
		filename.Delete(iPos,filename.GetLength()-iPos);
	
	CTString strExt = strTargetFileName;
	iPos = strExt.ReverseFind('.');
	if(iPos != 0)
		strExt.Delete(0,iPos);
	strExt.MakeLower();// .mkw
	//////////////////////////////////////////////////
	bool IsFalse = true;
	int m = 1;
	while (IsFalse)
	{
		CTString strOut = filePath;
		if (filename.GetLength()>240)
		{
			filename.Delete(240,filename.GetLength()-240);
		}

		CTString strtemp ;
		strtemp.Format("%d",m);
		strOut +=  filename ;
		strOut+= "_" ;
		strOut+=strtemp;
		
		strOut +=strExt;
/* -- UNIX PORT
        WIN32_FIND_DATA FindData;
		HANDLE hFileFile = FindFirstFile(strOut , &FindData );
		if(hFileFile != INVALID_HANDLE_VALUE)
		{//���ļ��Ѵ���			
			m++;
			FindClose(hFileFile);
			continue;
		}
		FindClose(hFileFile);
		*/
		if( !_access(strOut,0)){
			m++;
			continue;
		}
		if(!rename(strTempFileName,strOut))
		//if (MoveFile(strTempFileName,strOut))
		{
			strOutFileName = strOut;
			return true;
		}
	}
	return false;
}
/************************************************************************************
�������ƣ�
    CPages::GetPageCount
����˵����
    �õ�ҳ�ĸ���.
��ϸ���ͣ�
    
���������
    [in,out]��
�������ͣ�
   int :ҳ������. 
������
    Eric2002-07-03
�޸�: 
************************************************************************************/

int CPages::GetPageCount()
{	//#_�޸� 2002-11-12 $ 18:17:52 zlq
	return m_PageArraySort.GetPageSize(); 
}
/************************************************************************************
�������ƣ�

����˵����
    ����ԭʼ����Դ�ĸ���,�ڶ�δ֪��������Դʱ��.ͬʱ���������Ӧ��pages�ĸ���
��ϸ���ͣ�
    
���������
    [in,out]��
�������ͣ�
   
������
    //###_Mod 2002-10-15 zlq
�޸�: 
************************************************************************************/

bool CPages::AdjustOriginCount(int nOffset)
{
	m_nLogicCount = m_nLogicCount - nOffset;
	return true;
}
/************************************************************************************
�������ƣ�

����˵����
	���ݴ����logicRecNo�����Ƿ���Ҫ�����������ҳ�棬��ҳ����״̬�޹�
    ��Ҷ�漯��UnLoadһ��ҳʱ���ã�����ƫ������ж��ҳ���unload��������
��ϸ���ͣ�
���������
	[in]logicRecNo : �߼���¼��,�������ҳ��Ҫ����.
	[in]fakeRecNo :	 �߼���¼�Ŷ�Ӧ��α��¼��
	[in]nFakeOffset : Ҫ������ƫ����.

�������ͣ�
	void
������

�޸�: 
************************************************************************************/
void CPages::AdjustFakeRecNo(int logicRecNo,int fakeRecNo, int nFakeOffset)
{
    CPage* pPageTemp;
 
	for (int i=0;i<m_PageArraySort.GetPageSize();i++)
    {
		pPageTemp = m_PageArraySort.GetPageAt(i);
		pPageTemp->AdjustFakeRecNo(logicRecNo,fakeRecNo,nFakeOffset);
    }

}
/************************************************************************************
�������ƣ�
	CPages::AdjustLogicRecNo
����˵����
	���ݴ����logicRecNo�����Ƿ���Ҫ�����������ҳ���߼���¼��
��ϸ���ͣ�
���������
	[in]logicRecNo:�߼���¼��,�������ҳ��Ҫ����.
	[in]nOffset:ƫ����.
�������ͣ�void

������

�޸�: 
************************************************************************************/
void CPages::AdjustLogicRecNo(int logicRecNo , int nOffset)
{
    CPage* pPageTemp;
 
	for (int i=0;i<m_PageArraySort.GetPageSize();i++)
    {
		pPageTemp = m_PageArraySort.GetPageAt(i);
		pPageTemp->AdjustLogicRecNo(logicRecNo,nOffset);
    }
}

/************************************************************************************
�������ƣ�
    CPages::InsertField
����˵����
    ��̬����һ���ֶ�ʱ,ÿһ��page��Ҫ��Ӧ����һ����. 
��ϸ���ͣ�
    �ȵ�ϵͳ����.
	�ı��ֶ��б����Ϣ.
	����ÿһ��page�ĺ���
���������
    [in]nIndex�������λ��
    [in]pNewField: ������ֶ�
�������ͣ�
    
������
    Eric2002-08-19
�޸�: 
************************************************************************************/
void CPages::InsertField(int nIndex,CField* pNewField)
{
	pNewField->m_pFields = m_pFields;
	m_pFields->Insert(pNewField,nIndex);


    CPage* pPageTemp;
	for (int i=0;i<m_PageArraySort.GetPageSize();i++)
    {
		pPageTemp = m_PageArraySort.GetPageAt(i);
		pPageTemp->InsertField(nIndex,pNewField);
    }
}
/************************************************************************************
�������ƣ�
    CPages::DeleteField
����˵����
    ��̬ɾ��һ���ֶ�ʱ,ÿһ��page��Ҫɾ��һ������. 
��ϸ���ͣ�
    �ȵ�ϵͳ����.
	�ı��ֶ��б����Ϣ.
	����ÿһ��page�ĺ���
���������
    [in]nIndex��ɾ����λ��
    [in]pOldField����ɾ�����ֶ�
�������ͣ�
    void
������
    Eric2002-08-19
�޸�: 
************************************************************************************/
void CPages::DeleteField(int nIndex)
{

	CField * pOldField = m_pFields->FieldByIndex(nIndex);
	LPBYTE pOldValue = pOldField->m_pValue;//����ԭ������ֵ
	//�����ֶμ�
	m_pFields->Delete(nIndex);

    CPage* pPageTemp;
	for (int i=0;i<m_PageArraySort.GetPageSize();i++)
    {
		pPageTemp = m_PageArraySort.GetPageAt(i);
		pPageTemp->DeleteField(nIndex,pOldField);
    }
	////////////////////////////
	//pOldField->m_pValue =pOldValue;//�ָ�ԭ������ֵ
	pOldField->m_pValue = NULL;//�ָ�ԭ������ֵ
	if(m_pFields->GetFieldCount()== 0)
	{
		NewFile();

	}
	SetCurRecNo( GetRecNo());
}
/************************************************************************************
�������ƣ�
    CPages::ModifyField
����˵����
    ��̬����һ���ֶ�ʱ,ÿһ��page��Ҫת��һ������. 
��ϸ���ͣ�
    �ȵ�ϵͳ����.
	�ı��ֶ��б����Ϣ.
	����ÿһ��page�ĺ���
���������
    [in]nIndex��Ҫ���ĵ��ֶ�λ��
    [in]pOldField���ɵ��ֶ���Ϣ
    [in]pNewField���µ��ֶ���Ϣ
�������ͣ�
    void
������
    Eric2002-08-19
�޸�: 
************************************************************************************/
void CPages::ModifyField(int nIndex,CField* pNewField)
{
	//�����ֶμ�
	CField * pNewField1 = pNewField;
	CField * pOldField1 = m_pFields->FieldByIndex(nIndex);
	LPBYTE pOldValue1 = pOldField1->m_pValue;//����ԭ������ֵ
	pNewField1->m_pFields = m_pFields;
	m_pFields->Modify(nIndex,pNewField1);


    CPage* pPageTemp;

	if (m_PageArraySort.GetPageSize())
	{

		for (int i=0;i<m_PageArraySort.GetPageSize();i++)
		{
			pPageTemp = m_PageArraySort.GetPageAt(i);
			pPageTemp->ModifyField(nIndex,pOldField1,pNewField1);
		}
	}

	pOldField1->m_pValue = NULL;
	pNewField1->m_pValue =pOldValue1;//����ԭ������ֵ
	//pNewField2->m_pValue =pOldValue2;//����ԭ������ֵ

	SetCurRecNo( GetRecNo());
}

/************************************************************************************
�������ƣ�
    CPages::AddField
����˵����
    ����һ���ֶ�
��ϸ���ͣ�
    
���������
    [in]pNewField�����ֶ�
�������ͣ�
    
������
    Eric2002-08-22
�޸�: 
************************************************************************************/
void CPages::AddField(CField* pNewField)
{


	//�����ֶμ�
	pNewField->m_pFields = m_pFields;
	m_pFields->Add(pNewField);

	CPage* pPageTemp;
	for (int i=0;i<m_PageArraySort.GetPageSize();i++)
    {
		pPageTemp = m_PageArraySort.GetPageAt(i);
		pPageTemp->AddField(pNewField);
    }
	SetCurRecNo( GetRecNo());
}

/************************************************************************************
�������ƣ�
    CPages::GetInsertPos
����˵����
    ������������ʱ,������Ӧ��ֵ,�õ������λ��.  �˴��Ƚ��г�ʼ��,Ȼ�����SearchPos
	���еݹ����.
��ϸ���ͣ�
���������
�������ͣ�
    int :������ǰ������λ��,-1ΪAppend
������
    Eric2002-09-03
�޸�: 
************************************************************************************/
int CPages::GetInsertPos(int nColIndex, double fValue, CSortType nType,int nNullCount)
{
	if(m_nLogicCount == 0)
		return -1;
	int nRecLarge = GetRecordCount();
	int nRecSmall = 1;
	if(nType == stAscend)
	{
		nRecSmall = 1+ nNullCount;
		SetCurRecNo(nRecSmall);
		if(m_pFields->FieldByIndex(nColIndex)->GetAsDouble() > fValue)
			return nRecSmall;
		Last();
		if(m_pFields->FieldByIndex(nColIndex)->GetAsDouble() < fValue)
			return -1;//Append
	}
	else
	{
		nRecLarge -= nNullCount;
		if(!nRecLarge)
			return 1;
		First();
		if(m_pFields->FieldByIndex(nColIndex)->GetAsDouble() < fValue)
			return 1;
		SetCurRecNo(nRecLarge);
		if(m_pFields->FieldByIndex(nColIndex)->GetAsDouble() > fValue)
		{
			if(nNullCount == 0)
				return -1;//Append
			else
				return 	nRecLarge + 1;
		}
	}

	return SearchPos(nRecSmall,nRecLarge,fValue,nColIndex,nType);
}
/************************************************************************************
�������ƣ�
    CPages::SearchPos
����˵����
    ��������¼��֮��,���õݹ��㷨,���Ҳ����λ��.
��ϸ���ͣ�
���������
    [in]nRecSmall :С�ļ�¼��
    [in]nRecLarge :��ļ�¼��
    [in]fValue :���ڱȽϵ�ֵ
    [in]nColIndex :�ֶε�λ��
    [in]nType :��������.
�������ͣ�
    int :Ҫ��ǰ������λ��.
������
    Eric2002-09-03
�޸�: 
************************************************************************************/
int CPages::SearchPos(int nRecSmall,int nRecLarge,double fValue,int nColIndex,CSortType nType)
{
	if(nRecSmall == nRecLarge)
		return nRecLarge;
	if(nRecSmall == nRecLarge-1)
		return nRecLarge;

	int nCurPos = (nRecSmall+ nRecLarge)/2;


	SetCurRecNo(nCurPos);
	if(nType == stAscend)
	{
		if(m_pFields->FieldByIndex(nColIndex)->GetAsDouble() < fValue)
			nRecSmall = nCurPos;
		else
			nRecLarge = nCurPos;
	}
	else
	{
		if(m_pFields->FieldByIndex(nColIndex)->GetAsDouble() > fValue)
			nRecSmall = nCurPos;
		else
			nRecLarge = nCurPos;
	}
	return SearchPos(nRecSmall,nRecLarge,fValue,nColIndex,nType);
}
/************************************************************************************
�������ƣ�
    CPages::BindFieldList
����˵����
	�ı�pFieldList �е�˽�б���m_pPages��ֵ.
��ϸ���ͣ�
���������
�������ͣ�
    void
������
    Eric2002-09-03
�޸�: 
************************************************************************************/
void CPages::BindFieldList(CFieldList *pFieldList)
{
	pFieldList->m_pPages = this;
}

int  CPages::GetMaxCountPerPage()
{
	return m_nMaxCountPerPage;
}

bool CPages::SetNormalCountPerPage(int nCount)
{
	if (nCount<1)
		return false;
	m_nNormalCountPerPage = nCount; //����ʱÿҳ�ļ�¼����.
	m_nMaxCountPerPage = nCount * 2;    //ÿ��ҳ������¼����.
	return true;
}

int CPages::GetNormalCountPerPage()
{
	return m_nNormalCountPerPage;
}
/************************************************************************************
�������ƣ�
	CPages::MoveRecord
����˵����
	��¼�Ŷ��ǵ���ǰ��λ��
��ϸ���ͣ�

���������
	[in] nSourceRec		:
	[in] nDestinationRec:
�������ͣ�

������
	Eric 2002-9-16
�޸�:

************************************************************************************/
bool CPages::MoveRecord(int nSourceRec,int nDestinationRec)
{
	//===temp TRACE("Դλ�ã�%d Ŀ��λ�ã�%d \n",nSourceRec,nDestinationRec);
	SetCurRecNo(nSourceRec);
	int nLength = m_pFields->QueryLength();
	BYTE *pBuffer = new BYTE[nLength+2];
	SetRecord(pBuffer);
	Delete(); 
	if(nSourceRec < nDestinationRec)
		nDestinationRec--;
	SetCurRecNo(nDestinationRec);
	_Insert();
	int nCount;
	GetRecord(pBuffer,nCount);
	//Post();
	delete pBuffer	;
	return true;
}
//�ж��Ƿ��ڶ���λ��
bool CPages::Bof()
{
	return m_bIsBof;
}
//�ж��Ƿ��ڽ�βλ��
bool CPages::Eof()
{
	return m_bIsEof;
}


//�õ�ҳ�������� ���� ����ҳ���� ÿ��ҳ������ʱ�Ĵ�С �Ѿ�ʹ�õ�ҳ
void CPages::GetPageSet(int &nMaxPageCount,int &nPageNormalSize,int &nUsePage)
{
	nMaxPageCount = m_nMaxPageCount;
	nPageNormalSize = m_nNormalCountPerPage;
	nUsePage = m_nWorkPageCount;
}



int CPages::GetFileVersion()
{
	return m_pFakeRecNos->GetFileVersion();
}

int CPages::GetSaveFileVersion()
{
	return m_nSaveVersion;
}

void CPages::SetSaveFileVersion(int nVersion)
{
	m_nSaveVersion = nVersion;
}



/************************************************************************************
����˵����
	��ҳ������Ϊ�����ܹ�ȫ��װ���ڴ�
���������
    [in]bAtOnce:�Ƿ����̽���������װ���ڴ�//��ʱû��ʵ��

�������ͣ�
    bool :�Ƿ�ɹ�
	//#_S 2003-8-22 $ 9 zlq  
************************************************************************************/
bool CPages::AdjustPageCount(bool bAtOnce)
{
	
	//*************ҳ������**********************
	int nMaxPageCount,nPageNormalSize,nUsePage;
	int nCount = this->GetRecordCount();
	this->GetPageSet(nMaxPageCount,nPageNormalSize,nUsePage);
	int nMax = nCount/nPageNormalSize+20;
	if (nCount>500)//������ʮ��ҳ
	{
		nMax = 5;
		this->SetMaxPageCount(nMax);//�趨ҳ���С
	}
	else
	{//Ԥ���ڴ�װ��
		this->SetMaxPageCount(nMax);//�趨ҳ���С
		/*
		int nOld = GetRecNo();//����ԭ���ļ�¼��		
		int nPos = 1;
		while (nPos <m_nLogicCount )
		{
			SetCurRecNo(nPos);
			nPos = nPos+nPageNormalSize;
		}

		SetCurRecNo(nOld);//�ָ�
		*/
	}

	return true;
}

/************************************************************************************
����˵����
    �ƶ�һ���ֶ�ʱ,ÿһ��page��Ҫ�ƶ� 
��ϸ���ͣ�
    �ȵ�ϵͳ����.
	�ı��ֶ��б����Ϣ.
	����ÿһ��page�ĺ���
���������
      1.nFrom,ԭ����������
	  2.nTo,�ƶ������µ�������(ע����������ֶλ���ʱ��λ��)
�������ͣ�
    void
������
    //#_S 2003-9-12 $ 10 zlq
�޸�: 
************************************************************************************/
void CPages::MoveField(int nFrom, int nTo)
{

	m_pFields->Mov(nFrom,nTo);

    CPage* pPageTemp;
	for (int i=0;i<m_PageArraySort.GetPageSize();i++)
    {
		pPageTemp = m_PageArraySort.GetPageAt(i);
		pPageTemp->MoveField(nFrom,nTo);
    }
}

bool CPages::_Insert()
{
	CPage *pCurPage = GetCurPage();
	if (m_nCurRecNo == 0 && m_nLogicCount==0)//###_Mod 2002-10-30 zlq
	{//�����ǰ��¼����Ϊ�գ��ҵ�ǰ��¼��Ϊ0
		_Append();
		return true;
	}
	

	AdjustLogicRecNo(pCurPage->m_lngLStartRecNo,1);//730 add ע��Ҫ��post֮ǰ,�����ҳ

	pCurPage->_InsertEx();
	m_nOldCurRecNo = m_nCurRecNo;
	m_nCurRecNo = -1;
	
	
	//��ǰΪ�ڴ�
	//pCurPage->Post(storeMemory,0);//###_Mod 2002-9-17 zlq
	
	m_bIsBof = false;
	m_bIsEof = false;
	m_nCurRecNo = m_nOldCurRecNo;
	

	
	m_nOldCurRecNo = 0;
	m_nLogicCount++;
	if (m_nLogicCount==1)
	{//#_�޸� 2002-11-13 $ 8:47:22 zlq
		m_PageArraySort.MoveToWorkArea(pCurPage);//�µ�
	}
	
	
	
	
	if(m_pFields->GetLastPostVersion()<m_pFields->GetLastVersion())         //mmmm
	{
		m_pFields->m_nLastPosVersion = m_pFields->GetLastVersion();
		//m_pFakeRecNos->m_pIOFields->m_nLastPosVersion = m_pFakeRecNos->m_pIOFields->GetLastVersion();
	}
	return true;
}

bool CPages::_Append()
{
	CPage * pCurPage = GetCurPage();
	if(m_nCurRecNo == 0 && m_nLogicCount == 0)
	{//�����ǰ��¼����Ϊ�գ��ҵ�ǰ��¼��Ϊ0
		if(!pCurPage)
			pCurPage = GetPhysicalEmptyPage();
	}
	else
	{
		Last();
		m_nOldCurRecNo = m_nCurRecNo;
	}
	
	AdjustLogicRecNo(pCurPage->m_lngLStartRecNo,1);//730 add ע��Ҫ��post֮ǰ,�����ҳ
	pCurPage->_AppendEx();
	m_nCurRecNo = -1;

	//��ǰΪ�ڴ�
	//pCurPage->Post(storeMemory,0);//###_Mod 2002-9-17 zlq

	m_bIsBof = false;
	m_bIsEof = false;
	
	m_nOldCurRecNo = 0;
	m_nLogicCount++;
	m_nCurRecNo = m_nLogicCount;
	
	
	if (m_nLogicCount==1)
	{//#_�޸� 2002-11-13 $ 8:47:22 zlq
		m_PageArraySort.MoveToWorkArea(pCurPage);//�µ�
	}

	if(m_pFields->GetLastPostVersion()<m_pFields->GetLastVersion())         //mmmm
	{
		m_pFields->m_nLastPosVersion = m_pFields->GetLastVersion();
		//m_pFakeRecNos->m_pIOFields->m_nLastPosVersion = m_pFakeRecNos->m_pIOFields->GetLastVersion();
	}
	return true;
}

bool CPages::_Edit()
{
	//���õ�ǰҳ��ķ���
	CPage *pPage;
	pPage = GetCurPage();
	if(!pPage)//###_Mod 2002-9-16 zlq
	{
		pPage = GetPhysicalEmptyPage();
		m_nOldCurRecNo = 0;
		pPage->_AppendEx();
		pPage->m_bIsModify = true;
	}
	else
	{

		m_nOldCurRecNo = m_nCurRecNo;		//===ky
		pPage->_Edit();
	}

	return true;
}

void CPages::PrintPages()//===temp
{
	//===temp TRACE("ҳ����� ��ʼ\r\n");
	for (int i=0;i<m_PageArraySort.GetPageSize();i++)
	{
		CPage*pPage = m_PageArraySort.GetPageAt(i);
		//===temp TRACE("Page: %p  LS:( %d - %d ) FakeS:( %d  - %d )\r\n",pPage,pPage->m_lngLStartRecNo,pPage->m_nLogicCount,
		//	pPage->m_lngFakeStart,pPage->m_nOriginLoadCount);
	}
	//===temp TRACE("****************\r\n");
}
