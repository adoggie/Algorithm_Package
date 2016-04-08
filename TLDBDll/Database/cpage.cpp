// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "cdataset.h"
#include "cpage.h"
#include "cpages.h"
#include "cfakerecnos.h"
#include "cfield.h"
#include "cfields.h"

//#include "TlDatabase.h"//###_Mod 2002-10-7 zlq

/************************************************************************************
�������ƣ�
    CPage::CPage 
����˵����
    ���캯��
��ϸ���ͣ�
���������
	[in]pOwner:��ҳ��������ҳ�漯ָ��.
�������ͣ�
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
CPage::CPage(CPages* pOwner)
{
	m_pPages = pOwner;	   //�趨������ҳ�漯
	m_bIsModify = false;   //��ҳ�Ƿ�ı��

	m_bReachEndError = false;

	m_lngFakeStart=1; //��ҳ��ʼα��¼��
	m_lngLStartRecNo = 1;  //��ҳ��ʼ�߼���¼��
	m_nLogicCount = 0;     //��ҳ�߼���¼�ŵĸ��� ���ú���SetLogicCount()
	m_nOriginLoadCount = 0;//��ҳα��¼����

	m_nCurNo = -1;			//��ǰ�߼���¼�� 
	m_pFields =0;		   //ָ��CFields ��ָ��



	m_pPages->m_PageArraySort.AddPage(this);
	m_DatasArray.SetSize(0,_IncreaseNum);
}

/************************************************************************************
�������ƣ�
    CPage::~CPage
����˵����
    ��������
��ϸ���ͣ�
    
���������
�������ͣ�
    
������
    Eric2002-07-24
�޸�: 
************************************************************************************/
CPage::~CPage()
{
	for(int i = 0;i<m_DatasArray.GetSize();i++)
	{  //�����ݼ��е����ݵ�ַ�������ֶε�����ָ����
		CField * pField = m_pFields->FieldByIndex(i);
		//#_�޸� 2002-11-26 $ 9:09:41 zlq
		pField->pg_Delete(m_DatasArray[i]);
	}
	m_pPages->m_PageArraySort.RemovePage(this);
}


/************************************************************************************
�������ƣ�
    CPage::Load
����˵����
    ��"װ������"����IO������
��ϸ���ͣ�
	1���ȵ���ҳ�漯�����GetLogicToFake�õ�ƫ�ƣ������Ϊƫ��
	2���޸ı�ҳ��ĳ�Ա����m_lngLStartRecNo��m_lngFakeStartRecNo
	3�������޸��Լ���ҳ��״̬Ϊ�ȴ�װ��״̬
	4������α��¼����Ľ�ջ����
���������
    [in]logicRecNo��װ�����ʼ�߼���¼��
    [in]count��	װ��ĸ���.
    [in]IsNeedFeedback���Ƿ�Ҫ������.
�������ͣ�
    void
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
void CPage::Load(int logicRecNo,int count, bool IsNeedFeedback)
{
	//�������ʼα��¼��,װ��ҳ��ʱʹ��.
	int iOffset;
	iOffset = m_pPages->GetLogicToFake(logicRecNo);
	//m_pPages->PrintPages();
	//assert( logicRecNo - iOffset>0);
	//���ô�ҳ��ʼ�߼���¼��
	m_lngLStartRecNo = logicRecNo;	
	m_lngFakeStart = logicRecNo - iOffset ;
	m_nOriginLoadCount = count;
	SetLogicCount(count);//730 add

	//����װ��,�ŵ�װ�����,������ҳ״̬
	//SetPageState(pgWaitLoad);
	//�޸���ر���
	m_bIsModify = false;
	
	m_pPages->m_pFakeRecNos->InQueueLoad(this,m_lngFakeStart,count,IsNeedFeedback);
}

/************************************************************************************
�������ƣ�
    CPage::UnLoad
����˵����
	д������,�������ز���������ҳ������ɾ�����������)
	�����޸��Լ���ҳ��״̬Ϊ�ȴ�״̬д��
��ϸ���ͣ�
	�������ǵ�ͨ�������Կ������ϼ����ݷ���ֵ�����������Լ����ҳ��ľ���ʼλ�ã�
���������
	[in]bIsNeedFeedBack:�Ƿ������.
�������ͣ�
    int	:���������¼���߼���¼�Ĳ�ֵ.
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
int CPage::UnLoad(bool bIsNeedFeedBack)
{
	//�������ز�����
	int nOffset;
	nOffset = m_nLogicCount - m_nOriginLoadCount;

	
	//�ŵ�UnLoad������,���޸��Լ���ҳ��״̬,����״̬�Ȳ��޸�
	m_pPages->m_pFakeRecNos->UnLoad(this);
	m_nOriginLoadCount = m_nLogicCount;
	//SetPageState(pgUnLoading);
	m_bIsModify = false;
	
	return nOffset;
}

/************************************************************************************
�������ƣ�
    CPage::HangFields
����˵����
    ���ֶμ������ݼ���ϵ��һ��
��ϸ���ͣ�
    ʵ���ϵ������ֶε�m_pValueָ��
    �˺���������ϼ�����
���������
    [in,out]pFields��  �ֶμ���ָ��
�������ͣ�
    void
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
void CPage::HangFields(CFields* pFields)
{
	m_pFields=pFields;
	if(pFields)
		pFields->m_pDataSet =  this;
}

/************************************************************************************
�������ƣ�
    CPage::BindFields
����˵����
    �����ݼ��е����ݵ�ַ�������ֶε�����ָ����
��ϸ���ͣ�
���������
�������ͣ�
    void
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
void CPage::BindFields()
{  
	//���棺�������߼���ҳ���жϣ������������ҳ����������IsPhysicalEmpty����

	for(int i = 0;i<m_DatasArray.GetSize();i++)
	{  //�����ݼ��е����ݵ�ַ�������ֶε�����ָ����
		CField * pField = m_pFields->FieldByIndex(i);
		pField->m_pValue = pField->pg_GetAt(m_DatasArray[i],m_nCurNo);

	}
}







/************************************************************************************
����˵����
	��ҳ����ͣ����Ӷ�����Ӵ�ʱ��Ҫ����
��ϸ���ͣ�
	��һ�������ݿ���������Ҷ�棬�ڴ�֮ǰ������Ҷ��Ӧ����д�����������CPages��GetEmptyPageʵ�֡�
	ͬʱ�ù��̱��븺���Ѿ���������Ҷ��������ڱ�Ҷ����ȥ��
���������
	[in]CopyFlags�� 	���ƵĿ�ʼλ��
	[in]nPhysicalCount�����Ƶĸ���
�������ͣ�

������
	Eric2002-07-03
�޸�: 
************************************************************************************/
int CPage::CopyToOther(CFromPos CopyFlags, int nPhysicalCount)
{//===uc  CopyToOther
	//ȡ�ÿ�����Ŀ��ҳ,������pPageEmpty��
	CPage * pPageEmpty;
	pPageEmpty = m_pPages->GetPhysicalEmptyPage();
	
	//��ҳͷ��ʼ����nPhysicalCount����¼.
	if( CopyFlags == FromBegin )
	{
		for(int i=0;i<nPhysicalCount;++i)
		{
			for(int j = 0;j<m_DatasArray.GetSize();j++)
			{  //�����ݼ��е����ݵ�ַ�������ֶε�����ָ����
				CField * pField = m_pFields->FieldByIndex(j);				
				switch(pField->GetFieldType())
				{
				case fCurrency:
				case fBoolean:
				case fDate:
				case fInt:
				case fDouble:
					((PF_NUM)pPageEmpty->m_DatasArray[j])->Add(((PF_NUM)m_DatasArray[j])->GetAt(i));
					break;
				case fString:
					((PF_STRING)pPageEmpty->m_DatasArray[j])->Add(((PF_STRING)m_DatasArray[j])->GetAt(i));
					break;
				default:
					break;
				}
			}
		}

		//һ����ɾ��
		for(int j = 0;j<m_DatasArray.GetSize();j++)
		{  //�����ݼ��е����ݵ�ַ�������ֶε�����ָ����
			CField * pField = m_pFields->FieldByIndex(j);
			switch(pField->GetFieldType())
			{
				case fCurrency:
				case fBoolean:
				case fDate:
				case fInt:
				case fDouble:
				((PF_NUM)m_DatasArray[j])->RemoveAt(0,nPhysicalCount);
				break;
			case fString:
				((PF_STRING)m_DatasArray[j])->RemoveAt(0,nPhysicalCount);
				break;
			default:
				break;
			}
		}
        //�޸�Ŀ��ҳ��״̬�ȱ���
		//pPageEmpty->SetPageState(pgNormal);
		pPageEmpty->m_bIsModify = true;

		
		//Ŀ��ҳ��ͷ����Ϣ.
		pPageEmpty->m_lngFakeStart = m_lngFakeStart;
		//assert(pPageEmpty->m_lngFakeStart>0);
		pPageEmpty->m_lngLStartRecNo = m_lngLStartRecNo;
		pPageEmpty->SetLogicCount(nPhysicalCount);//�ƶ�������-����ɾ���ĸ���
		pPageEmpty->m_nOriginLoadCount = 0;//��ҳα��¼���� ===temp

		
		m_bIsModify = true;
		
        //Դҳ��ͷ����Ϣ.
		// ���ڵ�ֵ = ԭ����ֵ + �Ƶ���ҳ�ĸ���
		m_lngLStartRecNo = m_lngLStartRecNo + nPhysicalCount;//()��Ϊ���ƶ����߼���¼�ŵĸ���;
		// ���ڵ�ֵ = ԭ����ֵ - �Ƶ���ҳ�ĸ���
		m_nLogicCount = m_nLogicCount - pPageEmpty->GetLogicCount();
		m_nOriginLoadCount;//��ҳα��¼���� ����

		
		//���¶����������Ϊ��ֵ,�����.����Ϊ��һ���߼���¼��ֵ.
		m_nCurNo -= pPageEmpty->m_nLogicCount;
		if(m_nCurNo<0)
			First();
		if (pPageEmpty->GetLogicCount())
		{
			m_pPages->m_PageArraySort.MoveToWorkArea(pPageEmpty); //12-30
		}

    }
////////////////////////////////////////////////////////////////////////////////////////
	//��ҳβ��ʼ����nPhysicalCount����¼
	if(CopyFlags == FromEnd)
	{
		for(int i = m_nLogicCount-nPhysicalCount;i<m_nLogicCount;++i)
		{
			for(int j = 0;j<m_DatasArray.GetSize();j++)
			{  //�����ݼ��е����ݵ�ַ�������ֶε�����ָ����
				CField * pField = m_pFields->FieldByIndex(j);				
				switch(pField->GetFieldType())
				{
				case fCurrency:
				case fBoolean:
				case fDate:
				case fInt:
				case fDouble:
					((PF_NUM)pPageEmpty->m_DatasArray[j])->Add(((PF_NUM)m_DatasArray[j])->GetAt(i));
					break;
				case fString:
					((PF_STRING)pPageEmpty->m_DatasArray[j])->Add(((PF_STRING)m_DatasArray[j])->GetAt(i));
					break;
				default:
					break;
				}
			}
		}

		//һ����ɾ��
		for(int j = 0;j<m_DatasArray.GetSize();j++)
		{  //�����ݼ��е����ݵ�ַ�������ֶε�����ָ����
			CField * pField = m_pFields->FieldByIndex(j);
			switch(pField->GetFieldType())
			{
				case fCurrency:
				case fBoolean:
				case fDate:
				case fInt:
				case fDouble:
				((PF_NUM)m_DatasArray[j])->RemoveAt(m_nLogicCount-nPhysicalCount,nPhysicalCount);
				break;
			case fString:
				((PF_STRING)m_DatasArray[j])->RemoveAt(m_nLogicCount-nPhysicalCount,nPhysicalCount);
				break;
			default:
				break;
			}
		}
		
		//�޸�Ŀ��ҳ��״̬�ȱ���
		pPageEmpty->m_bIsModify = true;
		pPageEmpty->m_nOriginLoadCount = m_nOriginLoadCount;//ת�Ƶ�����һҳ
		m_nOriginLoadCount = 0;
		
		//pPageEmpty->m_FState = dsBrowse;
		//Դҳ����Ϣ
		//m_lngFakeStart;  //����
		//m_lngLStartRecNo;          //����
		m_nLogicCount = m_nLogicCount - nPhysicalCount;
		//Ŀ��ҳ����Ϣ
		pPageEmpty->m_lngFakeStart = m_lngFakeStart ;
		//assert(pPageEmpty->m_lngFakeStart>0);
		pPageEmpty->m_lngLStartRecNo = m_lngLStartRecNo + m_nLogicCount;
		pPageEmpty->SetLogicCount(nPhysicalCount);
		
		m_bIsModify = true;
		//�����ǰ��¼���ƶ�.����Ϊ��һ���߼���¼��ֵ.
		if(m_nCurNo>=m_nLogicCount)
			First(); 

		if (pPageEmpty->GetLogicCount())
		{
			m_pPages->m_PageArraySort.MoveToWorkArea(pPageEmpty);
		}
    }
	this->HangFields(m_pPages->m_pFields);
	return true;

}

/***********************************************************************************
�������ƣ�
CPage::AdjustFakeRecNo
����˵����
    ���ݴ����logicRecNo,fakeRecNo�����Ƿ���Ҫ������ҳ�棬�뱾ҳ����״̬�޹�

��ϸ���ͣ�
    ��Ҷ�漯��UnLoadһ��ҳʱ���ã�����ƫ���� nFakeOffset ��ж��ҳ���unload��������
    ҳ�漯�����ø���ҳ�����������������¼ҳ����ʼα��¼��
    �޸���ʼα��¼��
���������
    [in]logicRecNo������ж��ҳ���߼���¼��,ӦΪĳҳ���߼���ʼ��¼��
    [in]fakeRecNo:����ж��ҳ��α��¼��,���߼���¼����ͬʱ�ٱȽϸò���,
	              ����ɾ������ҳ,������߼���¼����ͬ�����.

    [in]nFakeOffset:�߼���¼����α��¼�ŵ�ƫ��
�������ͣ�

������
    Eric2002-07-03
�޸�: 
************************************************************************************/
void CPage::AdjustFakeRecNo(int logicRecNo,int fakeRecNo, int nFakeOffset)
{
	
	if(m_lngLStartRecNo > logicRecNo) //(==ʱ���ı�)
	{  //�����ҳ����ʼ�߼���¼�Ŵ��ڴ���ļ�¼��,����ҳ�洦�ں���״̬,��Ҫ�ı���ʼα��¼��.
		m_lngFakeStart = m_lngFakeStart+nFakeOffset;
		//assert(m_lngFakeStart>0);
	}
	if (m_lngLStartRecNo == logicRecNo)
	{//����߼���¼�����,����ͨ��α��¼���б�
		if (m_lngFakeStart > fakeRecNo)
		{
			m_lngFakeStart = m_lngFakeStart+nFakeOffset;
			//assert(m_lngFakeStart>0);
		}
	}
	//assert(m_lngFakeStart>0);
}


/************************************************************************************
�������ƣ�
    CPage::Next
����˵����
    �߼��ƶ���¼
��ϸ���ͣ�
���������
�������ͣ�
    
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
int CPage::Next()
{	

	//����ѵ����߼�ĩ��¼,�򷵻�.
	if(m_nCurNo == m_nLogicCount-1) 
	{
		return false;
		//assert(0);
	}

	m_nCurNo++;
	//�����¼�ŷ����ı�

	BindFields();//===temp yh ���Բ�Ҫ
	return true;

//}}}END  2002-11-20 Eric
}

/************************************************************************************
�������ƣ�
    CPage::Prior
����˵����
    �߼��ƶ���¼
��ϸ���ͣ�
���������
�������ͣ�
    
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
int CPage::Prior()
{

	//assert(m_nCurNo);//����ѵ����߼��׼�¼,�򷵻�.

	m_nCurNo--;

	BindFields();
	return true;

//}}}END  2002-11-20 Eric
}

/************************************************************************************
�������ƣ�
    CPage::MoveBy
����˵����
    �߼��ƶ���¼
��ϸ���ͣ�
���������
    [in]iSkipLen������
    [in]MoveFlags:����ʼλ��
�������ͣ�
    void
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
int CPage::MoveBy(int iSkipLen ,CFromPos MoveFlags)
{
	//�ƶ��� CFromPos ָ����λ��,ͬʱ������
	switch(MoveFlags)
	{
	case FromBegin:
		{
			m_nCurNo = iSkipLen;
		}
		break;		
	}

	//�ָ���ҳ�İ���Ϣ
	BindFields();
	return true;
}





/************************************************************************************
�������ƣ�
    CPage::First
����˵����
    �ƶ����߼���һ����¼
��ϸ���ͣ�
���������
�������ͣ�
    void
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
int CPage::First()
{
	//�ƶ��������׼�¼,�����Ϊɾ��״̬,��������һ����¼.


	m_nCurNo = 0;

	BindFields();
	return true;

//}}}END  2002-11-20 Eric
}

/************************************************************************************
�������ƣ�
    CPage::Last
����˵����
    �ƶ����߼����һ����¼
��ϸ���ͣ�
    �ƶ�������ĩ��¼,�����Ϊɾ��״̬,��������һ����¼.
���������
�������ͣ�
    
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
int CPage::Last()
{

	//�ƶ�������ĩ��¼,�����Ϊɾ��״̬,��������һ����¼.

	m_nCurNo = m_nLogicCount -1;
	//�����¼�ŷ����ı�
	BindFields();

	return true;
//}}}END  2002-11-20 Eric

}




/************************************************************************************
�������ƣ�
    CPage::GetRecNo
����˵����
    ���ص�ǰ��¼��
��ϸ���ͣ�
���������
�������ͣ�
    int	:��¼��
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
int CPage::GetRecNo()
{
	return m_nCurNo+m_lngLStartRecNo;
}

/************************************************************************************
�������ƣ�
   CPage::FieldByName 
����˵����
    ���ֶ����Ƶõ��ֶ�
��ϸ���ͣ�
���������
    [in]FieldName���ֶ�����
�������ͣ�
    CField*	:�ֶ�ָ��
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
CField* CPage::FieldByName(CTString FieldName)
{
	if(m_pFields)
	{
		CField * pField;
		pField = m_pFields->FieldByName(FieldName);
		return pField;
	}
	return 0;
}



/************************************************************************************
�������ƣ�

����˵����
    ����ҳ��Ϊ����״̬
��ϸ���ͣ�
    Appendʱ,ֵ�ȱ�����NEW ���Ļ�������
���������
�������ͣ�
    void
������

�޸�: 
************************************************************************************/
#ifdef TEMP_Reserve
bool CPage::Append()
{
	if(m_pFields)
	{
		m_pFields->NewData();
	}
	return true;
}
bool CPage::Post()
{  /*
	if (m_pFields->GetLastVersion())//#_�޸� 2003-12-24 $ 14:22:30 zlq
		m_pFields->SetCurVersion();//08-26

	//CField * pTempField; //ʹ���ֶεķ�����������
	int i=0;

	switch(m_FState)
	{
	case dsBrowse:
		break;
	case dsInsert:
		//��ÿ���ֶβ�����ֵ
		for( i = 0;i<m_DatasArray.GetSize();i++)
		{  //�����ݼ��е����ݵ�ַ�������ֶε�����ָ����
			CField * pField = m_pFields->FieldByIndex(i);
			pField->pgInsertAt(m_DatasArray[i],m_nPhysicalNo);
		}//#_�޸� 2002-11-25 $ 17:39:16 zlq

		//�޸ļ�¼״̬�б�����Ϣ
		m_bytePhysicalStateArray.InsertAt(m_nPhysicalNo,psAdd);
		m_OriginRecPosArray.InsertAt(m_nPhysicalNo,m_TempOriginPos);//#_�޸� 2002-12-26 $ 14:06:05 zlq
		m_LogNoArray.InsertAt(m_nCurNo,m_nPhysicalNo);
		for( i = m_nCurNo+1;i<m_LogNoArray.GetSize();i++)
		{
			m_LogNoArray[i]++;
		}


		if (GetPageState() == pgNormal)
		{//ȷ������IO�̵߳���
			SetLogicCount(m_nLogicCount+1);//ҳ�����߼�����
			m_bIsModify = true;
			
			if(GetPhysicalCount() >= m_pPages->GetMaxCountPerPage())//200
			{
				CFromPos CopyPos;
				if(m_nCurNo >= m_pPages->GetNormalCountPerPage())
					CopyPos = FromBegin;
				else
					CopyPos = FromEnd;
				CopyToOther(CopyPos,m_pPages->GetNormalCountPerPage());
			}		
		}
		BindFields();
		
		break;
	case dsAppend:
		for( i = 0;i<m_DatasArray.GetSize();i++)
		{  //�����ݼ��е����ݵ�ַ�������ֶε�����ָ����
			CField * pField = m_pFields->FieldByIndex(i);
			pField->pgAdd(m_DatasArray[i]);
		}//#_�޸� 2002-11-25 $ 17:51:52 zlq

		//�޸ļ�¼״̬�б�����Ϣ
		m_bytePhysicalStateArray.Add (psAdd);
		m_OriginRecPosArray.Add(m_TempOriginPos);//#_�޸� 2002-12-26 $ 14:06:05 zlq
		m_nCurNo++;//��ǰ�߼���¼��(ÿҳ���´�0��ʼ���.)
		m_nPhysicalNo =GetPhysicalCount()-1;
		m_LogNoArray.Add(m_nPhysicalNo);		

		if (GetPageState() == pgNormal)
		{//ȷ������IO�̵߳���
			SetLogicCount(m_nLogicCount+1);//ҳ�����߼�����
			m_bIsModify = true;

			if(GetPhysicalCount() >= m_pPages->GetMaxCountPerPage())
			{
				CopyToOther(FromBegin,m_pPages->GetNormalCountPerPage());
			}
		}
		BindFields();
		break;
	case dsEdit:
		for( i = 0;i<m_DatasArray.GetSize();i++)
		{  //�����ݼ��е����ݵ�ַ�������ֶε�����ָ����
			CField * pField = m_pFields->FieldByIndex(i);
			//#_�޸� 2002-11-25 $ 18:11:55 zlq
			pField->pgSetAt(m_DatasArray[i],m_nPhysicalNo);
		}

		BindFields();

		if (m_bytePhysicalStateArray[m_nPhysicalNo] == psNoChange)//�޸ļ�¼״̬�б�����Ϣ,ԭ����ӵĲ��ܽ����޸�״̬
			m_bytePhysicalStateArray[m_nPhysicalNo] = psModify;
		m_bIsModify = true;
		break;
	default:
		break;
	}
	m_FState = dsBrowse;*/
	return true;

//}}}END  2002-11-20 Eric
}
/************************************************************************************
�������ƣ�
    CPage::Edit
����˵����
    ����ҳ��Ϊ�༭״̬
��ϸ���ͣ�
    EDITʱ,ֵ�ȱ�����NEW ���Ļ�������
���������
�������ͣ�
    void
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
bool CPage::Edit()
{/*	
	//����������д������,һ��д��һ���ֶ�.
	m_pFields->NewData();
	for(int i = 0;i<m_DatasArray.GetSize();i++)
	{  //�����ݼ��е����ݵ�ַ�������ֶε�����ָ����
		CField * pField = m_pFields->FieldByIndex(i);
		PF_NUM pDoubleField;
		switch(pField->GetFieldType())
		{
		case fInt:
			pField->GetData ((LPBYTE) &((*(PF_INT)m_DatasArray[i])[m_nPhysicalNo]));
			break;
		case fDouble:
			pDoubleField = (PF_NUM)m_DatasArray[i];
			pField->GetData ((LPBYTE) &((*pDoubleField)[m_nPhysicalNo]));
			break;
		case fString:
			pField->GetData ((LPBYTE) &((*(PF_STRING)m_DatasArray[i])[m_nPhysicalNo]));
			break;
		case fCurrency:
			pField->GetData ((LPBYTE) &((*(PF_CURRENCY)m_DatasArray[i])[m_nPhysicalNo]));
			break;
		case fBoolean:
			pField->GetData ((LPBYTE) &((*(PF_BOOL)m_DatasArray[i])[m_nPhysicalNo]));
			break;
		case fDate:
			pField->GetData ((LPBYTE) &((*(PF_DATE)m_DatasArray[i])[m_nPhysicalNo]));
			break;
		default:
			break;
		}
	}*/
	//m_FState = dsEdit;
	return true;
}


/************************************************************************************
�������ƣ�
    CPage::Insert
����˵����
    ����ҳ��Ϊ����״̬
��ϸ���ͣ�
���������
�������ͣ�
    
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
bool CPage::Insert()
{
	if(m_pFields)
	{
		m_pFields->NewData();
		//m_FState = dsInsert;
	}
	return true;
}

bool CPage::Post(CStoreType nStoreType ,long lngPosition )
{
	SetOriginPos(nStoreType,lngPosition);
	return Post();	
}
#endif
/************************************************************************************
�������ƣ�
    CPage::Delete
����˵����
    �߼�ɾ����ǰ��¼
��ϸ���ͣ�
    ������������ļ�¼,�������ɾ�����.
	����������ļ�¼,����ڴ���ɾ��.
���������
�������ͣ�
    void
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
bool CPage::_DeleteEx()
{
	if (m_pFields->GetLastVersion())//===ky  �϶�ʱ���°汾��
		m_pFields->SetCurVersion();

	{
		for(int i = 0;i<m_DatasArray.GetSize();i++)
		{  //�����ݼ��е����ݵ�ַ�������ֶε�����ָ����
			CField * pField = m_pFields->FieldByIndex(i);
			pField->pg_RemoveAt(m_DatasArray[i],m_nCurNo);
		}

		m_nLogicCount--;
		if(m_nCurNo >= m_nLogicCount)
		{			
			m_nCurNo = m_nLogicCount-1;	
		} 

		m_bIsModify = true;
		if (m_nLogicCount == 0)
			return false;
		BindFields();
	}

	return true;
}





/************************************************************************************
�������ƣ�
    CPage::SetRecord
����˵����
	���õ�ǰ����¼�����е��ֶε�������Ϣ��һ��ָ���Ļ�������
	������֯��ָʾ��Ϣ���֣������ֶΣ�ÿ���ֶ�һ�ֽڣ�+���ݲ
	�֣������ֶΣ�����ĳЩ�ֶ�Ҳû�����ݣ�ǰ���ܹ����ʱ��
	�������ݳ���
��ϸ���ͣ�
���������
    [in,out]lpData��������ָ��
�������ͣ�
    int	: ���ݳ���
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
int CPage::_SetRecord(LPBYTE lpData)
{ 
	if(m_pFields)
	{
	    //BindFields();
		return (int)m_pFields->SaveDataToBuffer(lpData);  //
	}
	else
	{
		return false;
	}
}

/************************************************************************************
�������ƣ�
    CPage::GetRecord
����˵����
    �Ե�ǰ�ļ�¼���ݸ�ֵ����
��ϸ���ͣ�
	�����ݼ��ڴ�֮ǰӦ��ͨ�����ڱ༭
	�����״̬������append,edit�ȷ�������
	�����ֶ�������ֶε�LoadDataFromBuffer
	�����Ƿ�װ�سɹ�������Ч���������ݴ�С
    
���������
    [in,out]lpData�� ������
    [in,out]count���ֽ���
�������ͣ�
    bool :�Ƿ�ɹ�
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
bool CPage::GetRecord(LPBYTE lpData, int& count)
{
	LPBYTE lpDatatemp = lpData;
	count = 0;
	if (!GetRecord(lpDatatemp))
		return false;
	count = lpDatatemp - lpData;
	return true;
}

/************************************************************************************
�������ƣ�
    CPage::GetRecord
����˵����
    �Ե�ǰ�ļ�¼���ݸ�ֵ����
��ϸ���ͣ�
	�����ݼ��ڴ�֮ǰӦ��ͨ�����ڱ༭
	�����״̬������append,edit�ȷ������������޸�ָ��
	�����ֶ�������ֶε�LoadDataFromBuffer
���������
    [in,out]lpData�� ������ָ��
�������ͣ�
    bool:�Ƿ�ɹ�
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
bool CPage::GetRecord(LPBYTE& lpData)
{
	//�����ҳ���� append edit Insert ״̬,����
	//if(m_FState != dsAppend&&m_FState != dsInsert&&m_FState != dsEdit)
	//	return false;


	if(m_pFields)
	{
	    //BindFields();
		int count;
		return m_pFields->LoadDataFromBuffer(lpData, count); 
	}
	else
	{
		return false;
	}
}



/************************************************************************************
�������ƣ�

����˵����
    ���ڴ���ɾ������,ֻʣ���ֶ�����
��ϸ���ͣ�
���������
�������ͣ�
    void
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
void CPage::EmptyTable()
{
	//if (GetPageState()== pgNormal)
	//if (nStoreType == storeMemory)
	{//���̵߳���
		m_pPages->m_pFields->GetBookmark();	 
		HangFields(m_pPages->m_pFields);
	}

	for( int i = 0;i<m_DatasArray.GetSize();i++)
	{  //�����ݼ��е����ݵ�ַ�������ֶε�����ָ����
		CField * pField = m_pFields->FieldByIndex(i);
		pField->pg_RemoveAll(m_DatasArray[i]);
	}//#_�޸� 2002-11-26 $ 9:03:55 zlq


	m_bIsModify = false;   //��ҳ�Ƿ�ı��

	m_bReachEndError = false;
	
	//m_lngFakeStartRecNo=1; //��ҳ��ʼα��¼��
	//m_lngLStartRecNo = 1;  //��ҳ��ʼ�߼���¼��
	//m_nLogicCount = 0;     //��ҳ�߼���¼�ŵĸ��� ���ú���SetLogicCount()
	//m_nOriginLoadCount = 50;//��ҳα��¼����
	
	m_nCurNo = -1;			//��ǰ�߼���¼�� 
	//m_pFields =0;		   //ָ��CFields ��ָ��
	
	
	//if (GetPageState()== pgNormal)
	//if (nStoreType == storeMemory)	
	{//���̵߳���
		m_pPages->m_pFields->GotoBookmark();
	}

}
/************************************************************************************
�������ƣ�
    CPage::IsLogiclEmpty
����˵����
    �Ƿ��߼���ҳ
��ϸ���ͣ�
    ��ҳ�洦������״̬ʱ,�Ƿ����߼���ҳ.��ȫ����ɾ��������,�������������¼�Ų�����
���������
�������ͣ�
    bool
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
bool CPage::IsLogicalEmpty()
{
	if(m_nLogicCount == 0)
		return true;
	else
		return false;
}

/************************************************************************************
����˵����
    �Ƿ��������ҳ
��ϸ���ͣ�
    û�������¼.
���������
�������ͣ�
    
������
    Eric2002-07-03
�޸�: 
************************************************************************************/
bool CPage::IsPhysicalEmpty()
{
	{//���̵߳���
		m_pPages->m_pFields->GetBookmark();	 
		HangFields(m_pPages->m_pFields);
	}	
	bool brst = true;
	for(int i = 0;i<m_DatasArray.GetSize();i++)
	{  //�����ݼ��е����ݵ�ַ�������ֶε�����ָ����
		CField * pField = m_pFields->FieldByIndex(i);
		int ArraySize = 0;
		if (pField->GetFieldType() == fString)
			ArraySize =((PF_STRING)m_DatasArray[i])->GetSize();
		else
			ArraySize =((PF_NUM)m_DatasArray[i])->GetSize();
		////assert(ArraySize ==m_nLogicCount);
		if (ArraySize >0)
			brst = false;
	}	
	{//���̵߳���
		m_pPages->m_pFields->GotoBookmark();
	}
	return brst;
	/*
	if (m_nLogicCount==0 && m_nOriginLoadCount==0)
		return true;
	else
		return false;
	
	if(m_bytePhysicalStateArray.GetSize() == 0)
		return true;
	else
		return false;*/
}
/************************************************************************************
�������ƣ�
    CPage::GetCurVersion
����˵����
    �õ���ǰҳ��İ汾��
��ϸ���ͣ�
���������
�������ͣ�
    
������
    Eric2002-07-12
�޸�: 
************************************************************************************/
int CPage::GetCurVersion()
{
	 return m_pFields->GetCurVersion();
}
/************************************************************************************
�������ƣ�
	CPage::AdjustLogicRecNo
����˵����
    ���ݴ����logicRecNo�����Ƿ���Ҫ��������ҳ�棬�뱾ҳ����״̬�޹�
��ϸ���ͣ�
    ҳ�漯�����ø���ҳ�����������������¼ҳ����ʼ�߼���¼��
    �޸���ʼ�߼���¼��
���������
    [in]logicRecNo��������߼���¼��,ӦΪĳҳ���߼���ʼ��¼��
	[in]nFakeOffset:�߼���¼�ŵ�ƫ��
�������ͣ�
    
������
    Eric2002-07-24
�޸�: 
************************************************************************************/

void CPage::AdjustLogicRecNo(int logicRecNo,int nOffset)
{
	if(m_lngLStartRecNo > logicRecNo) //(==ʱ���ı�)
	{  //�����ҳ����ʼ�߼���¼�Ŵ��ڴ���ļ�¼��,����ҳ�洦�ں���״̬,��Ҫ�ı���ʼα��¼��.
		m_lngLStartRecNo += nOffset;
	}
}
/************************************************************************************
�������ƣ�
	CPage::InsertField
����˵����
	����һ���ֶ�
��ϸ���ͣ�

���������
	[in]nIndex	�������λ��
	[in]pNewField:������ֶ�
�������ͣ�

������
	Eric 2002-9-4
�޸�:

************************************************************************************/
void CPage::InsertField(int nIndex,CField* pNewField)
{

	PF_NUM                pDouble;    
	PF_STRING                pString;    
     

	FieldNUM                F_Double;
	FieldString                        F_Str;
	
	F_Double =   NULLValue;

   
	
	int nPhyCount = m_nLogicCount ;//m_bytePhysicalStateArray.GetSize();
	
	int i;
	switch(pNewField->GetFieldType())
	{
	case fInt:
	case fDate:
	case fBoolean:
	case fCurrency:
	case fDouble:
		pDouble = new F_NUM;
		for(i = 0;i<nPhyCount;i++)
		{
			pDouble->Add(F_Double);
		}
		m_DatasArray.InsertAt(nIndex,pDouble);
		break;
	case fString:
		pString = new F_STRING;
		for(i = 0;i<nPhyCount;i++)
		{
			pString->Add(F_Str);
		}
		m_DatasArray.InsertAt(nIndex,pString);
		break;

	default:
		break;
	}

}
/************************************************************************************
�������ƣ�
	CPage::AddField
����˵����
	����һ���ֶ�
��ϸ���ͣ�

���������
	[in] pNewField :�������ֶε�ָ��
�������ͣ�

������
	Eric 2002-9-4
�޸�:

************************************************************************************/
void CPage::AddField(CField* pNewField)
{ 

  
		PF_NUM		pDouble;    
		PF_STRING		pString;    

		FieldNUM		F_Double;
		FieldString			F_Str;

		//F_Double.Data[0] = 0xFE;
		F_Double = NULLValue;

		F_Str ="";
								  
		int nPhyCount = m_nLogicCount;//m_bytePhysicalStateArray.GetSize();

		int i;
		switch(pNewField->GetFieldType())
		{
		case fInt:
		case fDate:
		case fBoolean:
		case fCurrency:
		case fDouble:
			pDouble = new F_NUM;
			pDouble->SetSize(nPhyCount,IncreaseSize);
			for(i = 0;i<nPhyCount;i++)
				pDouble->SetAt(i,F_Double);
 			m_DatasArray.Add(pDouble);
			break;
		case fString:
			pString = new F_STRING;
			pString->SetSize(nPhyCount,IncreaseSize);
			for(i = 0;i<nPhyCount;i++)
				pString->SetAt(i,F_Str);
   			m_DatasArray.Add(pString);
			break;

		default:
			break;
		}
}

/************************************************************************************
�������ƣ�
	CPage::DeleteField
����˵����
	ɾ��һ���ֶ�
��ϸ���ͣ�

���������
	[in]nIndex	 ��ɾ����λ��
	[in]pOldField  ��ɾ�����ֶ�.��ʱFields����û�д��ֶ�,���������洫����
�������ͣ�

������
	Eric 2002-9-4
�޸�:

************************************************************************************/
void CPage::DeleteField(int nIndex,CField* pOldField)
{

	if(nIndex >m_DatasArray.GetSize()||nIndex<0)
	{
		//assert(0);
		return ;
	}


	PF_NUM	pDouble;    
	PF_STRING	pString;    
 
	FieldNUM F_Double;
	F_Double= NULLValue;
	//int i;
	//CField *pField = m_pFields->FieldByIndex(nIndex);
	switch(pOldField->GetFieldType())
	{
	case fInt:
	case fDate:
	case fBoolean:
	case fCurrency:
	case fDouble:
		pDouble = (PF_NUM)(m_DatasArray[nIndex]);
		pDouble->RemoveAll();
		delete pDouble;
		m_DatasArray.RemoveAt(nIndex);
		break;
	case fString:
		pString = (PF_STRING)(m_DatasArray[nIndex]);
		pString->RemoveAll();
		delete pString;
		m_DatasArray.RemoveAt(nIndex);
		break;

	default:
		break;
	}

	if(m_DatasArray.GetSize() == 0)
	{
		m_bIsModify = false;   //��ҳ�Ƿ�ı��


		m_lngFakeStart=1; //��ҳ��ʼα��¼��
		m_lngLStartRecNo = 1;  //��ҳ��ʼ�߼���¼��
		SetLogicCount(0);      //��ҳ�߼���¼�ŵĸ���
		m_nOriginLoadCount = 0;//��ҳα��¼����

		m_nCurNo = 0;			//��ǰ�߼���¼�� 
	}
}
/************************************************************************************
�������ƣ�
	CPage::ModifyField
����˵����
	�޸�һ���ֶ�
��ϸ���ͣ�

���������
	[in] nIndex	 ��Ҫ�޸ĵ�λ��
	[in] pOldField	��Ҫ�޸ĵ��ֶ�
	[in] pNewField��  ����Ϊ���ֶ�
�������ͣ�

������
	Eric 2002-9-4
�޸�:

************************************************************************************/
void CPage::ModifyField(int nIndex,CField* pOldField, CField* pNewField)
{
	if(nIndex > m_DatasArray.GetSize() || nIndex < 0)
	{
		//assert(0);
		return ;
	}

	PF_NUM		pInt;   
	PF_NUM	pDouble;    
	PF_STRING	pString;    
	PF_STRING	pString_;    
	PF_NUM	pCurrency;  
	PF_NUM		pBool;      
     
	PF_NUM		pDate;  

	FieldNUM F_Double;
	F_Double = NULLValue;
	int i;
	pNewField->NewData();

	switch(pOldField->GetFieldType())
	{
	case fInt://���ֶ�
		pInt = (PF_NUM)(m_DatasArray[nIndex]);
		switch(pNewField->GetFieldType())
		{
			case fInt:
				return;//ʵ����û���޸�
			case fDouble:
				pDouble =  new F_NUM;
				pDouble->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pDouble);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pInt)[i]);
					TransFieldValue(pNewField,pOldField);
					pDouble->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fString:
				pString = new F_STRING;
				pString->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pString);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pInt)[i]);
					TransFieldValue(pNewField,pOldField);
					FieldString strTemp;
					FieldString * pTemp = (FieldString *)pNewField->m_pValue;
					strTemp	= *pTemp ;
					pString->Add(strTemp);
				}
				break;
			case fCurrency:
				pCurrency = new F_NUM;
				pCurrency->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pCurrency);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pInt)[i]);
					TransFieldValue(pNewField,pOldField);
					pCurrency->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fBoolean:
				pBool = new F_NUM;
				pBool->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pBool);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pInt)[i]);
					TransFieldValue(pNewField,pOldField);
					pBool->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;

			case fDate:
				pDate = new F_NUM;
				pDate->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pDate);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pInt)[i]);
					TransFieldValue(pNewField,pOldField);
					pDate->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			default:
				break;	
		}
		delete pInt;

		break;
	case fDouble://���ֶ�
		pDouble = (PF_NUM)(m_DatasArray[nIndex]);
		switch(pNewField->GetFieldType())
		{
			case fInt:
				pInt = new F_NUM;
				pInt->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pInt);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pDouble)[i]);
					TransFieldValue(pNewField,pOldField);
					pInt->Add(*((FieldNUM*)pNewField->m_pValue));
				} 

				break;
			case fDouble:
				return;//δ�ı�
			case fString:
				pString = new F_STRING;
				pString->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pString);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pDouble)[i]);
					TransFieldValue(pNewField,pOldField);
					FieldString strTemp;
					FieldString * pTemp = (FieldString *)pNewField->m_pValue;
					strTemp	= *pTemp;
					pString->Add(strTemp);
				}
				break;
			case fCurrency:
				pCurrency = new F_NUM;
				pCurrency->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pCurrency);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pDouble)[i]);
					TransFieldValue(pNewField,pOldField);
					pCurrency->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fBoolean:
				pBool = new F_NUM;
				pBool->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pBool);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue =(LPBYTE) &((*pDouble)[i]);
					TransFieldValue(pNewField,pOldField);
					pBool->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;

			case fDate:
				pDate = new F_NUM;
				pDate->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pDate);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pDouble)[i]);
					TransFieldValue(pNewField,pOldField);
					pDate->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			default:
				break;	
		}
		delete pDouble;

		break;
	case fString://���ֶ�
		pString = (PF_STRING)(m_DatasArray[nIndex]);
		switch(pNewField->GetFieldType())
		{
			case fInt:
				pInt = new F_NUM;
				pInt->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pInt);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pString) [i]);
					TransFieldValue(pNewField,pOldField);
					pInt->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fDouble:
				pDouble =  new F_NUM;
				pDouble->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pDouble);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pString) [i]);
					TransFieldValue(pNewField,pOldField);
					pDouble->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fString:
				pString_ = new F_STRING;
				pString_->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pString_);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pString) [i]);
					TransFieldValue(pNewField,pOldField);
					FieldString strTemp;
					FieldString * pTemp = (FieldString *)pNewField->m_pValue;
					strTemp	= *pTemp;
					pString_->Add(strTemp);
				}
				break;				
				//return;//δ�ı�
			case fCurrency:
				pCurrency = new F_NUM;
				pCurrency->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pCurrency);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pString) [i]);
					TransFieldValue(pNewField,pOldField);
					pCurrency->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fBoolean:
				pBool = new F_NUM;
				pBool->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pBool);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pString) [i]);
					TransFieldValue(pNewField,pOldField);
					pBool->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;

			case fDate:
				pDate = new F_NUM;
				pDate->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pDate);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &((*pString) [i]);
					TransFieldValue(pNewField,pOldField);
					pDate->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			default:
				break;	
		}
		delete pString;

		break;
	case fCurrency://���ֶ�
		pCurrency = (PF_NUM)(m_DatasArray[nIndex]);
		switch(pNewField->GetFieldType())
		{
			case fInt:
				pInt = new F_NUM;
				pInt->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pInt);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pCurrency)[i];
					TransFieldValue(pNewField,pOldField);
					pInt->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;//ʵ����û���޸�
			case fDouble:
				pDouble =  new F_NUM;
				pDouble->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pDouble);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pCurrency)[i];
					TransFieldValue(pNewField,pOldField);
					pDouble->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fString:
				pString = new F_STRING;
				pString->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pString);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pCurrency)[i];
					TransFieldValue(pNewField,pOldField);
					pString->Add(*((FieldString*)pNewField->m_pValue));
				}
				break;
			case fCurrency:
				return;//δ�ı�
			case fBoolean:
				pBool = new F_NUM;
				pBool->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pBool);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pCurrency)[i];
					TransFieldValue(pNewField,pOldField);
					pBool->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fDate:
				pDate = new F_NUM;
				pDate->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pDate);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pCurrency)[i];
					TransFieldValue(pNewField,pOldField);
					pDate->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			default:
				break;	
		}
		delete pCurrency;

		break;
	case fBoolean://���ֶ�
		pBool = (PF_NUM)(m_DatasArray[nIndex]);
		switch(pNewField->GetFieldType())
		{
			case fInt:
				pInt = new F_NUM;
				pInt->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pInt);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pBool)[i];
					TransFieldValue(pNewField,pOldField);
					pInt->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;//ʵ����û���޸�
			case fDouble:
				pDouble =  new F_NUM;
				pDouble->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pDouble);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pBool)[i];
					TransFieldValue(pNewField,pOldField);
					pDouble->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fString:
				pString = new F_STRING;
				pString->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pString);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pBool)[i];
					TransFieldValue(pNewField,pOldField);

					pString->Add(*((FieldString*)pNewField->m_pValue));
				}
				break;
			case fCurrency:
				pCurrency = new F_NUM;
				pCurrency->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pCurrency);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pBool)[i];
					TransFieldValue(pNewField,pOldField);
					pCurrency->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fBoolean:
				return;//δ�ı�

			case fDate:
				pDate = new F_NUM;
				pDate->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pDate);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pBool)[i];
					TransFieldValue(pNewField,pOldField);
					pDate->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			default:
				break;	
		}
		delete pBool;

		break;

	case fDate://���ֶ�
		pDate = (PF_NUM)(m_DatasArray[nIndex]);
		switch(pNewField->GetFieldType())
		{
			case fInt:
				pInt = new F_NUM;
				pInt->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pInt);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pDate)[i];
					TransFieldValue(pNewField,pOldField);
					pInt->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fDouble:
				pDouble =  new F_NUM;
				pDouble->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pDouble);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pDate)[i];
					TransFieldValue(pNewField,pOldField);
					pDouble->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fString:
				pString = new F_STRING;
				pString->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pString);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pDate)[i];
					TransFieldValue(pNewField,pOldField);
					FieldString strTemp;
					FieldString * pTemp = (FieldString *)pNewField->m_pValue;
					strTemp	= *pTemp;
					pString->Add(strTemp);
				}
				break;
			case fCurrency:
				pCurrency = new F_NUM;
				pCurrency->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pCurrency);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pDate)[i];
					TransFieldValue(pNewField,pOldField);
					pCurrency->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;
			case fBoolean:
				pBool = new F_NUM;
				pBool->SetSize(0,IncreaseSize);
				m_DatasArray.SetAt(nIndex,pBool);
				for(i = 0;i<GetLogicCount();i++)
				{
					pOldField->m_pValue = (LPBYTE) &(*pDate)[i];
					TransFieldValue(pNewField,pOldField);
					pBool->Add(*((FieldNUM*)pNewField->m_pValue));
				} 
				break;

			case fDate:
				return;//δ�ı�
			default:
				break;	
		}
		delete pDate;

		break;
	default:
		break;
	}

	pNewField->DeleteData();//

	if(m_DatasArray.GetSize() == 0)
	{
		m_bIsModify = false;   //��ҳ�Ƿ�ı��

		m_lngFakeStart=1; //��ҳ��ʼα��¼��
		m_lngLStartRecNo = 1;  //��ҳ��ʼ�߼���¼��
		SetLogicCount(0);      //��ҳ�߼���¼�ŵĸ���
		m_nOriginLoadCount = 0;//��ҳα��¼����

		m_nCurNo = 0;			//��ǰ�߼���¼�� 
	//}}}END  2002-11-22 Eric
	}
}

/************************************************************************************
�������ƣ�
	CPage::TransFieldValue
����˵����
	��ͬ�ֶ����͵��ֶμ�ֵ��ת��.
��ϸ���ͣ�

���������
	[in]pNewField���µ��ֶ�
	[in]pOldField ���ɵ��ֶ�
�������ͣ�

������
	Eric 2002-9-4
�޸�:

************************************************************************************/
void CPage::TransFieldValue(CField* pNewField, CField* pOldField)
{
	if(pOldField->IsNull())
	{
		pNewField->SetNull();
		return;
	}

	if(pNewField->GetFieldType()==fString)
	{
	    pNewField->SetAsString(pOldField->GetAsString());
		return;
	}

	pNewField->SetNull();

	switch(pOldField->GetFieldType())
	{
	case fString :
		pNewField->SetAsString(pOldField->GetAsString());
		break;
	case fInt :
		pNewField->SetAsInteger(pOldField->GetAsInteger());
		break;
	case fBoolean :
		pNewField->SetAsBool(pOldField->GetAsBool());
		break;
	case fDate :
		pNewField->SetAsDateTime(&pOldField->GetAsDateTime());
		break;

	case fCurrency :
		pNewField->SetAsCurrency(pOldField->GetAsCurrency());
		break;
	case fDouble:
		pNewField->SetAsDouble(pOldField->GetAsDouble());
		break;
	}
}
/************************************************************************************
�������ƣ�
	CPage::SetCurVersion
����˵����
	���ñ�ҳ��ĵ�ǰ�汾
��ϸ���ͣ�

���������
	[in] nVersion :�汾��
�������ͣ�

������
	Eric 2002-9-4
�޸�:

************************************************************************************/
void CPage::SetCurVersion(int nVersion)
{
	 m_pFields->SetCurVersion(nVersion);
}
/************************************************************************************
�������ƣ�
   CPage::GetNewestVersion
����˵����
	�õ����µİ汾��
��ϸ���ͣ�

���������
�������ͣ�
	int : �汾��
������
	Eric 2002-9-4
�޸�:

************************************************************************************/
int CPage::GetNewestVersion()
{
	return m_pFields->GetLastVersion();
}


/************************************************************************************
�������ƣ�

����˵����

��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2002-11-20
�޸�:

************************************************************************************/
void CPage::IniFieldCnt(CFields *pFields)
{
	int nFieldCount = pFields->GetFieldCount();
	for(int i = 0;i<m_DatasArray.GetSize();i++)
	{  //�����ݼ��е����ݵ�ַ�������ֶε�����ָ����
		CField * pField = pFields->FieldByIndex(i);
		pField->pg_RemoveAll(m_DatasArray[i]);
	}

	m_DatasArray.RemoveAll();


	for(int iFieldIndex = 0;iFieldIndex < nFieldCount;iFieldIndex++	)
	{
		CField * pField = pFields->FieldByIndex(iFieldIndex);
		m_DatasArray.Add(pField->pg_New());
	}//#_�޸� 2002-11-26 $ 9:38:29 zlq
}


void CPage::MoveField(int nFrom, int nTo)
{
	if (nFrom == nTo)
		return;

	if (nFrom < nTo)
		nTo--;//����ƶ�������,���ȼ�λ�ú�
	
	void* value = m_DatasArray[nFrom] ;
	m_DatasArray.RemoveAt(nFrom);//����λ��ȥ��
	m_DatasArray.InsertAt(nTo,value);//�ƶ�����λ��	
}

bool CPage::_AppendIn(CStoreType nStoreType, long lngPosition)
{//���ⶼ����CPages::_Append()   CFileOperation::ReadRecords()
	int nCurVersion = m_pFields->GetCurVersion();
	m_pFields->SetCurVersion();

	{//case dsAppend:
		
		for(int i = 0;i<m_DatasArray.GetSize();i++)
		{  //�����ݼ��е����ݵ�ַ�������ֶε�����ָ����
			CField * pField = m_pFields->FieldByIndex(i);
			pField->pg_Add(m_DatasArray[i]);
			
		}
		m_nCurNo++;//��ǰ�߼���¼��(ÿҳ���´�0��ʼ���.)

		BindFields();
	}
	
	m_pFields->SetCurVersion(nCurVersion);
	return true;
}
bool CPage::_AppendInBatch( int nCount)
{//���ⶼ����CPages::_Append()   CFileOperation::ReadRecords()
	int nCurVersion = m_pFields->GetCurVersion();
	m_pFields->SetCurVersion();//�������°汾

	m_LastFieldsAry.RemoveAll();
	for(int i = 0;i<m_DatasArray.GetSize();i++)
	{  //�����ݼ��е����ݵ�ַ�������ֶε�����ָ����
		CField * pField = m_pFields->FieldByIndex(i);
		pField->pg_Add(m_DatasArray[i],nCount);
		m_LastFieldsAry.Add(pField);
	}
	//m_nCurNo++;//��ǰ�߼���¼��(ÿҳ���´�0��ʼ���.)
	
	//BindFields();

	
	m_pFields->SetCurVersion(nCurVersion);
	return true;
}
bool CPage::_AppendEx()
{//���ⶼ����CPages::_Append()   CFileOperation::ReadRecords()
	if (m_pFields->GetLastVersion())//===ky  �϶�ʱ���°汾�� ===temp ��������
		m_pFields->SetCurVersion();

	{//case dsAppend:
		for(int i = 0;i<m_DatasArray.GetSize();i++)
		{  //�����ݼ��е����ݵ�ַ�������ֶε�����ָ����
			CField * pField = m_pFields->FieldByIndex(i);
			pField->pg_Add(m_DatasArray[i]);
		}

		m_nCurNo++;//��ǰ�߼���¼��(ÿҳ���´�0��ʼ���.)
		m_nLogicCount++;
		m_bIsModify = true;
		
		if(GetLogicCount() >= m_pPages->GetMaxCountPerPage())
		{
			CopyToOther(FromBegin,m_pPages->GetNormalCountPerPage());
		}

		BindFields();
		//break;
	}

	return true;
}
bool CPage::_Edit()
{
	m_bIsModify = true;
	return true;
}

bool CPage::_InsertEx()
{
	if (m_pFields->GetLastVersion())
		m_pFields->SetCurVersion();

	//��ÿ���ֶβ�����ֵ
	for(int  i = 0;i<m_DatasArray.GetSize();i++)
	{  //�����ݼ��е����ݵ�ַ�������ֶε�����ָ����
		CField * pField = m_pFields->FieldByIndex(i);
		pField->pg_InsertAt(m_DatasArray[i],m_nCurNo);
	}
	
	m_nLogicCount++;
	m_bIsModify = true;
	
	if(m_nLogicCount >= m_pPages->GetMaxCountPerPage())//200
	{
		CFromPos CopyPos;
		if(m_nCurNo >= m_pPages->GetNormalCountPerPage())
			CopyPos = FromBegin;
		else
			CopyPos = FromEnd;
		CopyToOther(CopyPos,m_pPages->GetNormalCountPerPage());
	}	
		
	BindFields();
	return true;
}

void CPage::PrintCurLine()//===temp
{
#ifdef tl_debug	
	for(int i = 0;i<m_DatasArray.GetSize();i++)
	{  //�����ݼ��е����ݵ�ַ�������ֶε�����ָ����
		CField * pField = m_pFields->FieldByIndex(i);
		int ArraySize = 0;
		if (pField->GetFieldType() == fString)
			ArraySize =((PF_STRING)m_DatasArray[i])->GetSize();
		else
			ArraySize =((PF_NUM)m_DatasArray[i])->GetSize();
		//assert(ArraySize ==m_nLogicCount);
	}	

	for ( i=0;i<m_pFields->GetFieldCount();i++)
	{
		//===temp TRACE(m_pFields->FieldByIndex(i)->GetAsString()+"\t");
	}
	//===temp TRACE("\r\n");
#endif	
}

bool CPage::_SetRec(int nRelativePos)
{
	if (nRelativePos <0 || nRelativePos>=m_nLogicCount)
		return false;
	m_nCurNo = nRelativePos;
	BindFields();
	return true;
}

bool CPage::_SetRecLast(int nRelativePos)
{
	if (nRelativePos <0 || nRelativePos>=m_nLogicCount)
		return false;
	m_nCurNo = nRelativePos;
	for(int i = 0;i<m_DatasArray.GetSize();i++)
	{  //�����ݼ��е����ݵ�ַ�������ֶε�����ָ����
		CField * pField = (CField *)m_LastFieldsAry.GetAt(i);
		pField->m_pValue = pField->pg_GetAt(m_DatasArray[i],m_nCurNo);

	}
	return true;
}
