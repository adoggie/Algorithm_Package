// VersionList.cpp: implementation of the CVersionList class.
//
//////////////////////////////////////////////////////////////////////

#include "../headtldb.h"
//#include "TlDatabase.h"
#include "versionlist.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVersionList::CVersionList()
{	
}

CVersionList::~CVersionList()
{
	Clear();
}

/************************************************************************************
�������ƣ�
DWORDList* CVersionList::GetVersion(int VerNo)
����˵�����õ�ָ���İ汾������.

��ϸ����:	1.���ָ���İ汾����Ч,�򷵻�NULL.

���������
[in]:VerNo:�汾��.      
  
[out]:��.

��������:DWORDList*,Ҫ�õ��İ汾������.


������YTLI 2002/08/28

�޸�: 
************************************************************************************/
CDWordArray* CVersionList::GetVersion(int VerNo)
{
	//assert( VerNo > -1 );
	
	//CDWordArray* pDWORDArray = (CDWordArray* )m_VerDataArray[VerNo];
	return (CDWordArray* )m_VerDataArray[VerNo];

}

/************************************************************************************
�������ƣ�
int	CVersionList::GetVersionCount()
����˵�����õ��汾����.

��ϸ����:	1.ֱ�ӷ��ذ汾�����а汾�ĸ���.

���������
[in]:��. 
  
[out]:��.

��������:int,�汾����.

������YTLI 2002/08/28

�޸�: 
************************************************************************************/
int	CVersionList::GetVersionCount()
{
	return m_VerDataArray.GetSize();
}

/************************************************************************************
�������ƣ�
int CVersionList::AddNewVersion()
����˵�������һ���汾������Ϊ��һ���汾�Ŀ�¡  ���ذ汾�� ���㿪ʼ

��ϸ����:	1.����汾����Ϊ��,�����һ���µİ汾����.
        2.����汾����Ϊ��,�����µİ汾��¡,����ӵ��汾�����β��.

���������
[in]:��.      
  
[out]:��.

��������:int,���µİ汾��.


������YTLI 2002/08/28

�޸�: 
************************************************************************************/
int CVersionList::AddNewVersion()
{

	if (m_VerDataArray.GetSize()==0)
	{
		CDWordArray* pNewVersion = new CDWordArray();                
		m_VerDataArray.Add(pNewVersion);
	}
	else
	{
		CDWordArray* pDWORDArray  = (CDWordArray*)m_VerDataArray[m_VerDataArray.GetSize()-1];

		CDWordArray* pNewVersion = new CDWordArray();
		pNewVersion->SetSize(0,_IncreaseNum);
		if (pDWORDArray)
		{
			int nCount = pDWORDArray->GetSize();
			for (int iIndexPos = 0; iIndexPos < nCount; iIndexPos++)
			{
				pNewVersion->Add( (*pDWORDArray)[iIndexPos]);
			}
		}

		m_VerDataArray.Add(pNewVersion);
	}
	return m_VerDataArray.GetSize()-1;
}

/************************************************************************************
�������ƣ�
void CVersionList::Clear()
����˵��������ֶμ�m_FieldList�������ֶ�.�Լ��汾��.

��ϸ����:	1.��ɾ������汾����,��ɾ������汾��.

���������
[in]:��. 
  
[out]:��.

��������:void.

������YTLI 2002/08/28

�޸�: 
************************************************************************************/
void CVersionList::Clear()
{
	//ɾ������汾����.
	int iMaxVer = GetVersionCount();
	for(int i=0; i<iMaxVer; i++)
	{
		CDWordArray* pDWORDArray = GetVersion(i);
		pDWORDArray->RemoveAll();
		delete pDWORDArray;
	}

	//ɾ������汾��.
	m_VerDataArray.RemoveAll();
}

/************************************************************************************
�������ƣ�
void CVersionList::Clone(CVersionList *pVersionList)
����˵������¡�ֶμ�m_FieldList�������ֶ�.�Լ��汾��.

��ϸ����:	

���������
[in]:��. 
  
[out]:��.

��������:pVersionList,�汾����.

������YTLI 2002/08/28

�޸�: 
************************************************************************************/
void CVersionList::Clone(CVersionList *pVersionList)
{
	Clear();
	for (int i = 0;i<pVersionList->GetVersionCount();i++)
	{
		CDWordArray* pDWORDArray = pVersionList->GetVersion(i);
		CDWordArray* pNewVersion = new CDWordArray();
		if (pDWORDArray)
		{
			int nCount = pDWORDArray->GetSize();
			for (int iIndexPos = 0; iIndexPos < nCount; iIndexPos++)
			{
				pNewVersion->Add( (*pDWORDArray)[iIndexPos]);
			}
		}

		m_VerDataArray.Add(pNewVersion);

	}
}
