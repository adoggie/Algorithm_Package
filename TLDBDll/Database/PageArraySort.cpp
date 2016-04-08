#include "../headtldb.h"

#include "cpage.h"

#include "PageArraySort.h"

static int m_nDebug =0;
CPageArraySort::CPageArraySort()
{	
	m_nTotalSize  = 0;		
	m_nWorkSize   = 0;
	m_nCurrent    = 0;
	m_nPageCount  = 0;
	m_ptrBuf      = NULL;
}

CPageArraySort::~CPageArraySort()
{
	if(m_ptrBuf != NULL)
	{
		delete []m_ptrBuf;
		m_ptrBuf = NULL;
	}
}

/************************************************************************************
�������ƣ�
	CPageArraySort::SetTotalSize
����˵����
	������ҳ��
��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2002-11-7
�޸�:

************************************************************************************/
void CPageArraySort::SetTotalSize(int nNewSize)
{

	if(nNewSize <= m_nTotalSize)
	{
		return;                //���С�ڵ���ԭ�гߴ磬������.
	}

	CPage **ppTemp = (CPage**)new byte[sizeof(void*) * nNewSize];
	memset(ppTemp ,0,sizeof(void*) * nNewSize);
	if(m_ptrBuf != NULL)
	{
		memcpy(ppTemp,m_ptrBuf,m_nPageCount*sizeof(void *));
		delete m_ptrBuf;//����ԭ������
	}
	m_nTotalSize = nNewSize;	
	m_ptrBuf = ppTemp;
}
/************************************************************************************
�������ƣ�
	CPageArraySort::InsertAt
����˵����
	��ָ��λ�ò���һҳ
��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2002-11-7
�޸�:

************************************************************************************/
bool CPageArraySort::InsertAt(int nIndex ,CPage * pPage)
{  
	if( nIndex > m_nTotalSize-1 || nIndex < 0)
		return false;

	if(nIndex < m_nPageCount)
	{
		memmove(&m_ptrBuf[nIndex+1],&m_ptrBuf[nIndex],sizeof(void*) * (m_nPageCount-nIndex));
	}
	m_ptrBuf[nIndex] = pPage;
	m_nPageCount ++;
	return true;
}
/************************************************************************************
�������ƣ�
	CPageArraySort::RemoveAt
����˵����
	��ָ��λ��ɾ��һҳ
��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2002-11-7
�޸�:

************************************************************************************/
bool CPageArraySort::RemoveAt(int nIndex)
{
	if(nIndex > m_nPageCount-1 || nIndex <0)
		return false;
	
	memmove(&m_ptrBuf[nIndex],&m_ptrBuf[nIndex+1],(m_nPageCount-nIndex-1)*sizeof(void*));
	m_nPageCount --;
	return true;
}


/************************************************************************************
�������ƣ�
	CPageArraySort::RemoveAll
����˵����
	ɾ�����Ե�ҳ
��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2002-11-7
�޸�:

************************************************************************************/
bool CPageArraySort::RemoveAll()
{
	m_nWorkSize  = 0;
	return true;
}


/************************************************************************************
�������ƣ�
	CPageArraySort::GetTotalSize
����˵����
	ȡ����ҳ��
��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2002-11-7
�޸�:

************************************************************************************/
int CPageArraySort::GetTotalSize()
{
	return m_nTotalSize ;
}
/************************************************************************************
�������ƣ�
	CPageArraySort::GetWorkSize
����˵����
	ȡ�ù���ҳ����
��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2002-11-7
�޸�:

************************************************************************************/
int CPageArraySort::GetWorkSize()
{
	return m_nWorkSize ;
}
/************************************************************************************
�������ƣ�
	CPageArraySort::GrowInsert
����˵����
	ϵͳ����һ��ҳʱ,����������û�м�¼,�����.
��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2002-11-7
�޸�:

************************************************************************************/
bool CPageArraySort::GrowInsert(CPage* pPage)
{
	int nIndex;
	if(!FindIndex(pPage,nIndex))
	{//û�ҵ�
		m_nWorkSize ++;
		InsertAt(nIndex,pPage);
		return true;
	}
	//�Ѿ��д�ҳ
	return false;
}
/************************************************************************************
�������ƣ�
	CPageArraySort::DeleteRemove
����˵����

��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2002-11-7
�޸�:

************************************************************************************/
bool CPageArraySort::DeleteRemove(CPage* pPage)

{
	int nIndex;
	if(!FindIndex(pPage,nIndex))
	{//û�ҵ�
		return false;
	}
	RemoveAt(nIndex);
	return true;
}	

/************************************************************************************
�������ƣ�

����˵����

��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2002-11-7
�޸�:

************************************************************************************/
bool CPageArraySort::FindIndexByRecNo(int RecNo,int &nIndex)
{
	CPage * pTemp;
	switch(m_nWorkSize)
	{
	case 0:
		////assert(0);
		nIndex = 0;
		return false;
	case 1:
		pTemp = m_ptrBuf[0];
		if((RecNo >= pTemp->m_lngLStartRecNo) && (RecNo < pTemp->m_lngLStartRecNo + pTemp->GetLogicCount()))
		{
			nIndex = 0;
			return true;
		}
		else
		{
			if( RecNo < pTemp->m_lngLStartRecNo )
			{//���Ҫ����,������0��
				nIndex = 0; 
			}
			else
			{//���Ҫ����,������1��
				nIndex = 1; 
			}
			return false;
		}
	default:
		break;
	}

	int nTemp;
	int nLargeIndex = m_nWorkSize-1;
	int nSmallIndex = 0;
	while (nLargeIndex >= nSmallIndex)
	{
		nTemp = (nLargeIndex + nSmallIndex)/2;
		pTemp = m_ptrBuf[nTemp];
		
		if(RecNo < pTemp->m_lngLStartRecNo)
		{//��ǰ���ҳ��
			nLargeIndex = nTemp-1;
			continue; 
		}
		else
		{//�ں����ҳ��
			nSmallIndex = nTemp+1;
			continue ;
		}
	}

	if (nLargeIndex<0)
	{//û���ҵ�
		nIndex = nLargeIndex+1;
		return false;
	}

	pTemp = m_ptrBuf[nLargeIndex];
	if((RecNo >= pTemp->m_lngLStartRecNo) && (RecNo < pTemp->m_lngLStartRecNo + pTemp->GetLogicCount()))
	{//�ڵ�ǰҳ
		nIndex = nLargeIndex;
		return true;
	}
	else
	{
		nIndex = nLargeIndex+1;
		return false;
	}
}

bool CPageArraySort::FindIndexInEmpty(CPage*pPage,int &nIndex)
{
	int nTemp = m_nWorkSize;
	int nLargeIndex = m_nPageCount-1;
	int nSmallIndex = m_nWorkSize;
	while (nLargeIndex >= nSmallIndex)
	{
		nTemp = (nLargeIndex + nSmallIndex)/2;
		
		if(pPage <= m_ptrBuf[nTemp])
		{//��ǰ���ҳ��
			nLargeIndex = nTemp-1;
			continue; 
		}
		else
		{//�ں����ҳ��
			nSmallIndex = nTemp+1;
			continue ;
		}
	}

	nIndex = nSmallIndex;
	if(nIndex<m_nPageCount && m_ptrBuf[nIndex]== pPage)
	{
		return true;
	}
	else
		return false;
}
/************************************************************************************
�������ƣ�

����˵����

��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2002-11-7
�޸�:

************************************************************************************/
bool CPageArraySort::FindIndex(CPage* pPage,int& nIndex)
{
	return FindIndexByRecNo(pPage->m_lngLStartRecNo,nIndex);
}

/************************************************************************************
�������ƣ�

����˵����

��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2002-11-7
�޸�:

************************************************************************************/
CPage* CPageArraySort::FindRecNoInPage(int nRecNo ) 
{
	int nIndex;
	if(FindIndexByRecNo(nRecNo,nIndex))
	{					  
		return m_ptrBuf[nIndex];
	}
	else
	{
		return NULL;
	}
}

//###_Mod 2002-11-11 zlq
/*
���������С,��ҳ�����ʱ�õ�
*/
void CPageArraySort::Clear()
{
	m_nWorkSize = 0;
	m_nPageCount = 0;
	m_nCurrent = 0;
	memset(&m_ptrBuf[0],0,(m_nTotalSize)*sizeof(void*));
	
}
/************************************************************************************
�������ƣ�
����˵����
		���һ��ҳ���뵽���߼���¼����
��ϸ���ͣ�

���������
	[in,out]
	CPage *pPage Ҫ��ӵ�ҳ
�������ͣ�

������
    zlq 2002-11-29 
�޸�:
************************************************************************************/
void CPageArraySort::AddPage(CPage *pPage)
{	
	int nIndex;
	if (!FindIndexInEmpty(pPage,nIndex))
	{//�ҵ���Ҫ�����λ��
		InsertAt(nIndex,pPage);
		////===temp TRACE("AddPage ����ҳ���� %d\n",m_nPageCount);
		return ;
	}
	else
	{//�Ѿ�����
		//assert(0);
	}
	//===temp AfxMessageBox("����!PageArraySort::AddPage");
	
}

/************************************************************************************
�������ƣ�
����˵����
		ɾ��һ��ҳ����,������������
��ϸ���ͣ�

���������
	[in,out]
	CPage *pPage Ҫɾ����ҳ
�������ͣ�

������
    zlq 2002-11-29 
�޸�:
************************************************************************************/
void CPageArraySort::RemovePage(CPage *pPage)
{	
	int nPos = FindPageByAllAddr(pPage);//��������������λ��
	//assert(nPos>=0);
	if (nPos<m_nWorkSize)
	{//��ִ̫�е�
		if (m_nCurrent>nPos)
			m_nCurrent--;
		m_nWorkSize --;		
	}
	else
	{
	}
	RemoveAt(nPos);
}
/************************************************************************************
�������ƣ�
����˵����
		��������������,ͨ����ַ������ҳ��λ��.����
��ϸ���ͣ�

���������
	[in,out]
	CPage *pPage Ҫɾ����ҳ
�������ͣ�

������
    zlq 2002-11-29 
�޸�:
************************************************************************************/
int CPageArraySort::FindPageByAllAddr(CPage *pPage)
{
	for (int i = 0;i<m_nPageCount;i++)
	{
		if(m_ptrBuf[i] ==pPage)
			return i;
	}
	return -1;
}

/************************************************************************************
�������ƣ�
����˵����
		�õ���ǰҳ,����Ҳ���߼�������
��ϸ���ͣ�

���������
	[in,out]
	
�������ͣ�
	���ص�ǰҳָ��
������
    zlq 2002-11-29 
�޸�:
************************************************************************************/
CPage *CPageArraySort::GetCurPage()
{
	return m_ptrBuf[m_nCurrent];
}
/************************************************************************************
�������ƣ�
����˵����
		���õ�ǰҳ
��ϸ���ͣ�

���������
	[in,out]
	CPage *pPage Ҫ���õ�ҳ.
�������ͣ�
	�����Ƿ�ɹ�
������
    zlq 2002-11-29 
�޸�:
************************************************************************************/
bool CPageArraySort::SetCurPage(CPage *pPage)
{
	//1.1 ԭʼҳ����
	CPage*pOldPage = m_ptrBuf[m_nCurrent];
	if (pOldPage->GetLogicCount()==0)
	{//��Ϊ�߼���ҳ
		if (m_nCurrent<m_nWorkSize)
		{//�ڹ�����,���ƶ����ǿ�ҳ��
			int nIndex;
			if (FindIndexInEmpty(pOldPage,nIndex))
			{
				//assert(0);//�������ڷǿ�ҳ��Ҳ����
			}
			else
			{
				RemoveAt(m_nCurrent);
				m_nWorkSize--;
				InsertAt(nIndex-1,pOldPage);
			}			
		}
	}
	else
	{//�ǿ��򲻵���
	}

	////////////////////////
	//1.2 Ŀ��ҳ����
	int nIndex;
	if (FindIndexInEmpty(pPage,nIndex))
	{//2.1 �ڿ�ҳ������
		RemoveAt(nIndex);
		
		if (FindIndexByRecNo(pPage->m_lngLStartRecNo,nIndex))
		{//�ִ��ڵĻ�,����
			//assert(0);
		}
		else
		{
			InsertAt(nIndex,pPage);
			m_nWorkSize++;//����ҳ����
		}

	}
	else
	{//2.2�ڷǿ�ҳ������,����Ҫ����λ��
		if (!FindIndexByRecNo(pPage->m_lngLStartRecNo,nIndex))
		{//�����ڵĻ�,����
			//assert(0);
		}
	}

	//���õ�ǰҳ
	m_nCurrent = nIndex;	
	
	return true;
}
/************************************************************************************
�������ƣ�
����˵����
		�õ�һ�����õ�ҳ,����λ��������
��ϸ���ͣ�

���������
	[in,out]
	pnIndex ���,�����Ч,�򷵻ؿ��õ�ҳ��ҳ�����е�λ��.
�������ͣ�
	���õ�ҳ��ַ
������
    zlq 2002-11-29 
�޸�:
************************************************************************************/
CPage *CPageArraySort::GetAvailablePage(int *pnIndex)
{
	if (m_nPageCount>m_nWorkSize)
	{//�п�ҳ,�����߼��������ҳ
		if (m_nWorkSize==0)
		{//����ҳΪ��ʱ
			int m = 1;
			if (m_nCurrent == m_nPageCount-1)
				m++;//���һ��Ϊ��ǰҳ
			if (pnIndex)
				*pnIndex = m_nPageCount-m;//����ҳλ��
			return m_ptrBuf[m_nPageCount-m];
			
		}
		else 
		{//��Ϊ�㣬�������һ��
			if (pnIndex)
				*pnIndex = m_nPageCount-1;
			return m_ptrBuf[m_nPageCount-1];
		}
	}
	else
	{//û�п�ҳ
		if (m_nWorkSize>=m_nCurrent+m_nCurrent+2)
		{//����ҳ��ǰ����
			if (pnIndex)
				*pnIndex = m_nWorkSize-1;
			return m_ptrBuf[m_nWorkSize-1];
		}
		else
		{//����ҳ�ں����
			if (pnIndex)
				*pnIndex = 0;
			return m_ptrBuf[0];
		}
	}
}

/************************************************************************************
�������ƣ�
����˵����
		�ƶ�����ҳ����.����ַ����
��ϸ���ͣ�

���������
	[in,out]
	nIndex ����,ҳ��ҳ�����е�λ��.
�������ͣ�
	���õ�ҳ��ַ
������
    zlq 2002-11-29 
�޸�:
************************************************************************************/
bool CPageArraySort::MoveToEmptyArea(int nIndex)
{
	if (m_nWorkSize>nIndex)
	{//���������Ч��,�����.
		CPage*pOldPage = m_ptrBuf[nIndex];
		int nNewIndex;
		if (!FindIndexInEmpty(pOldPage,nNewIndex))
		{
			RemoveAt(nIndex);
			m_nWorkSize--;
			InsertAt(nNewIndex-1,pOldPage);
			if (nIndex<m_nCurrent)
			{
				m_nCurrent--;				
			}
			else if (nIndex == m_nCurrent)
			{
				m_nCurrent = nNewIndex-1;
			}
		
		}
		else
		{
			//assert(0);//�������ҵ�
			return false;
		}
		return true;
	}
	else
	{
		return false;
	}
	
}

/************************************************************************************
�������ƣ�
����˵����
		���ݴ�����߼���¼��,�Ҷ�Ӧ��ҳ,�������,�������������,���ⲿ�������뷶Χ.
��ϸ���ͣ�

���������
	[in,out]
	RecNo  ����: ������߼���¼��.
	nLeft  ���: ����������
	nRight ���: ���������� Ĭ�ϴ����ܼ�¼����
�������ͣ�
	ҳ��ַ,û�д���ʱΪ��.
������
    zlq 2002-11-29 
�޸�:
************************************************************************************/
CPage* CPageArraySort::FindIndexByRecNo(int RecNo,int&nLeft,int& nRight)
{
	int nIndex;
	if (FindIndexByRecNo(RecNo,nIndex))
	{//�ҵ�
		return m_ptrBuf[nIndex];
	}
	else
	{//û���ҵ�
		if (m_nWorkSize==0)
		{
			nLeft = 0;
		}
		else
		{
			if (m_nWorkSize>nIndex)
			{//���ұ߽�
				nRight = m_ptrBuf[nIndex]->m_lngLStartRecNo;
				if (nIndex > 0)
				{//����߽�
					nLeft = m_ptrBuf[nIndex-1]->m_lngLStartRecNo+m_ptrBuf[nIndex-1]->GetLogicCount()-1;
				}
			}
			else
			{//û���ұ߽�								
				//if (nIndex > 0)
				{//����߽�
					nLeft = m_ptrBuf[nIndex-1]->m_lngLStartRecNo+m_ptrBuf[nIndex-1]->GetLogicCount()-1;					
				}
			}
			
			
		}
		return 0;
	}
	
}
/************************************************************************************
�������ƣ�
����˵����
		�õ�nIndex����Ӧ��ҳ
��ϸ���ͣ�

���������
	[in,out]
	nIndex  ����: λ�ú���.
�������ͣ�
	ҳ��ַ,û�д���ʱΪ��.
������
    zlq 2002-11-29 
�޸�:
************************************************************************************/
CPage *CPageArraySort::GetPageAt(int nIndex)
{
	//assert(nIndex<m_nPageCount);
	return m_ptrBuf[nIndex];
}
/************************************************************************************
�������ƣ�
����˵����
		ȡ��ҳ����
��ϸ���ͣ�

���������
	[in,out]
	nIndex  ����: λ�ú���.
�������ͣ�
	ҳ��ַ,û�д���ʱΪ��.
������
    zlq 2002-11-29 
�޸�:
************************************************************************************/
int CPageArraySort::GetPageSize()
{
	return m_nPageCount;
}

/************************************************************************************
�������ƣ�
����˵����
		�ƶ���������(ԭ��һ�����߼���ҳ��)
��ϸ���ͣ�

���������
	[in,out]
	pPage  ����: Ҫ�ƶ���ҳ.
�������ͣ�
	
������
    zlq 2002-11-29 
�޸�:
************************************************************************************/
bool CPageArraySort::MoveToWorkArea(CPage *pPage)
{
	int nIndex,nIndexNew;
	if(FindIndexInEmpty(pPage,nIndex))
	{
		if (!FindIndexByRecNo(pPage->m_lngLStartRecNo,nIndexNew))
		//if (m_nWorkSize==0)
		{
			RemoveAt(nIndex);
			InsertAt(nIndexNew,pPage);
			if (m_nWorkSize == 0 )
			{
				if (nIndex == m_nCurrent)
					m_nCurrent = nIndexNew;
				//else
					//===temp AfxMessageBox("����ȫ��Ϊ��ҳʱ����ǰҳ����");
			}
			else if (nIndexNew<=m_nCurrent )//&& m_nWorkSize>0
			{
				m_nCurrent++;
			}
			m_nWorkSize++;
			return true;
		}
		else
		{
			//===temp AfxMessageBox("û�п��ǣ�CPageArraySort::MoveToWorkArea");
		}
	}
	else
	{
		//===temp AfxMessageBox("û���ҵ���CPageArraySort::MoveToWorkArea");
		//assert(0);
	}
	return false;
}
