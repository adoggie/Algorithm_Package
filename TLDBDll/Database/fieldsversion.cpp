// FieldsVersion.cpp: implementation of the CFieldsVersion class.
//
//////////////////////////////////////////////////////////////////////

#include "../headtldb.h"
//#include "TlDatabase.h"
#include "fieldsversion.h"
#include "cfields.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFieldsVersion::CFieldsVersion()
{
	m_FieldAllArray.SetSize(0,_IncreaseNum);
}

CFieldsVersion::~CFieldsVersion()
{
	Clear();	
}

/************************************************************************************
�������ƣ�
int CFieldsVersion::GetLastVersionByIndex(int nIndex)
����˵���������ֶ����ֶ������е�������ȷ�����ֶ�������ʱ�İ汾.

��ϸ����:	

���������
[in]:nIndex,�ֶ��ھ����ֶ������е�������.      
  
[out]:��.

��������:int,������ʱ�İ汾��.

������YTLI 2002/08/28

�޸�: 
************************************************************************************/
int CFieldsVersion::GetLastVersionByIndex(int nIndex)
{
	//assert(nIndex > -1);

	int iCnt = m_VersionList.GetVersionCount();

	for(int i=iCnt-1; i>=0; i--)
	{
		CDWordArray* pDWORDArray = m_VersionList.GetVersion(i);
		//#_�޸� 2003-12-24 $ 11:33:43 zlq
		int nCount = pDWORDArray->GetSize();
		for (int iPos = 0; iPos < nCount; iPos++)
		{
			if( nIndex ==(int) (*pDWORDArray)[iPos])
				return iPos;			
		}

	}

	//assert(0);
	return -1;
}

/************************************************************************************
�������ƣ�
int CFieldsVersion::GetLastVersionByIndex(int nIndex)
����˵���������ֶ����ֶ������е�������ȷ�����ֶ��������ʱ�İ汾.

��ϸ����:	

���������
[in]:nIndex,�ֶ��ھ����ֶ������е�������.      
  
[out]:��.

��������:int,�������ʱ�İ汾��.

������YTLI 2002/08/28

�޸�: 
************************************************************************************/
int CFieldsVersion::GetFirstVersionByIndex(int nIndex)
{
	//assert(nIndex > -1);

	int iCnt = m_VersionList.GetVersionCount();

	for(int i=0; i<iCnt; i++)
	{			
		CDWordArray* pDWORDArray = m_VersionList.GetVersion(i);
		//#_�޸� 2003-12-24 $ 11:33:43 zlq
		int nCount = pDWORDArray->GetSize();
		for (int iPos = 0; iPos < nCount; iPos++)
		{
			if( nIndex ==(int) (*pDWORDArray)[iPos])
				return i;			
		}
	}


	//assert(0);
	return -1;
}

/************************************************************************************
�������ƣ�
void CFieldsVersion::SetVersion(CTypedPtrList<CObList, CField*>& FieldList, int nVersion)
����˵�������ð汾,Ĭ��Ϊ�°汾.����װ�ؾɰ汾����.���������µĿռ�,Ҳ����ʼ�������汾�ֶε�ָ��.

��ϸ����:	

���������
[in]:1.FieldList,�ֶ��ھ����ֶ������е�������.      
     2.nVersion,
[out]:��.

��������:void.

������YTLI 2002/08/28

�޸�: 
************************************************************************************/
void CFieldsVersion::SetVersion(CPtrArray & FieldArray, int nVersion)
{
	//���FieldList.
	//FieldArray.RemoveAll();
	FieldArray.RemoveAt(0,FieldArray.GetSize());

	//�õ���Ҫ�İ汾����.
	CDWordArray* pDWORDArray = m_VersionList.GetVersion(nVersion);
	int nCount = pDWORDArray->GetSize();
	for (int iPos = 0; iPos < nCount; iPos++)
	{
		CField* pField =(CField*) m_FieldAllArray[ (*pDWORDArray)[iPos] ];
		FieldArray.Add(pField);//#_�޸� 2002-11-25 $ 13:15:03 zlq
	}

}

/************************************************************************************
�������ƣ�
bool CFieldsVersion::IsConvertToLastVersion(CField* pField)
����˵�����жϵ�ǰ�ֶ��Ƿ��������ֶ���. 

��ϸ����:	

���������
[in]:pField,�ֶ�ָ��.      
  
[out]:��.

��������:bool.

������YTLI 2002/08/29

�޸�: 
************************************************************************************/
bool CFieldsVersion::IsConvertToLastVersion(CField* pField)
{
	//assert(pField);

	int iNext = pField->m_nNextModifyField;
	//�õ������ӵ����µ�һ���ֶ�
	while(iNext != -1)
	{		
		//pField = GetFieldByAbsolutePos(iNext);
		pField = (CField*)m_FieldAllArray[iNext];
		iNext = pField->m_nNextModifyField;
	};

	//�����ֶ�ָ����ֶ������еõ�����Ӧ��λ�ú� iNodeNo
	//int iNodeNo = 0;
	int nCount = m_FieldAllArray.GetSize();
	int iPos = 0;
	for (iPos = 0; iPos < nCount; iPos++)
	{
		if(pField == m_FieldAllArray[iPos])
			break;
	}

	//�õ����һ���ֶΰ汾����
	CDWordArray* pDWORDArray = m_VersionList.GetVersion(m_VersionList.GetVersionCount()-1);
	//�ж�λ�ú��Ƿ�������������
	nCount = pDWORDArray->GetSize();
	for (int iIndexPos = 0; iIndexPos < nCount; iIndexPos++)
	{
		if(iPos ==(int) (*pDWORDArray)[iIndexPos])
			return true;
	}


	return false;
}

/************************************************************************************
�������ƣ�
int CFieldsVersion::GetLastVersion()
����˵�����õ��ڴ������ύ�����ݵ����°汾. 

��ϸ����:	1.��С�ڵ���GetLastVersion()����������Ϊ1.

���������
[in]:pField,�ֶ�ָ��.      
  
[out]:��.

��������:int,�汾��.

������YTLI 2002/08/29

�޸�: 
************************************************************************************/
int CFieldsVersion::GetLastVersion()
{
	return m_VersionList.GetVersionCount()-1;
}

/************************************************************************************
�������ƣ�
void CFieldsVersion::Remove(CField* pField)
����˵����ɾ�����°汾�����е�һ���ֶ�.

��ϸ����:	1.�����ֶ�ָ����ɾ��һ���ֶ�.

���������
[in]:pField,�ֶ�ָ��.      
  
[out]:��.

��������:void.

������YTLI 2002/08/29

�޸�: 
************************************************************************************/
void CFieldsVersion::Remove(CField* pField)
{//===δ��,δ��
	//===temp AfxMessageBox("��ʱδʵ��,δ��");
	int nCount = m_FieldAllArray.GetSize();
	int iPos = 0;
	for (iPos = 0; iPos < nCount; iPos++)
	{
		if(pField == m_FieldAllArray[iPos])
			break;
	}

	CDWordArray* pDWORDArray = m_VersionList.GetVersion(m_VersionList.GetVersionCount()-1);
	
	
	nCount = pDWORDArray->GetSize();
	for (int iIndexPos = 0; iIndexPos < nCount; iIndexPos++)
	{
		if( iPos ==(int) (*pDWORDArray)[iIndexPos])
		{
			pDWORDArray->RemoveAt(iIndexPos);
		}
	}

}

/************************************************************************************
�������ƣ�
bool CFieldsVersion::Delete(int nIndex)
����˵����ɾ����nIndex����һ���ֶ�.

��ϸ����:	1.���m_LastPosVersion��GetLastVersion()��һ���Ծ����Ƿ���Ҫ
            ������汾����һ���°汾,�Լ���ʼ��������.

���������
[in]:nIndex,�ֶ�����.      
  
[out]:��.

��������:bool.

������YTLI 2002/08/29

�޸�: 
************************************************************************************/
bool CFieldsVersion::Delete(int nIndex)
{
	if( GetLastVersion() == m_pFields->GetLastPostVersion() )
	{
		m_VersionList.AddNewVersion();	
	}

	CDWordArray* pDWORDArray = m_VersionList.GetVersion(GetLastVersion());
	pDWORDArray->RemoveAt(nIndex);

	return true;
}

/************************************************************************************
�������ƣ�
void CFieldsVersion::Insert(CField* pField, int nIndex)
����˵������nIndex������һ���ֶ�.

��ϸ����:	1.���m_LastPosVersion��GetLastVersion()��һ���Ծ����Ƿ���Ҫ
            ������汾����һ���°汾,�Լ���ʼ��������.

���������
[in]: 1.pField,�µ��ֶ�ָ��.
	  2.nIndex,���������λ��.
  
[out]:��.

��������:void.

������YTLI 2002/08/29

�޸�: 
************************************************************************************/
void CFieldsVersion::Insert(CField* pField, int nIndex)
{
	if( GetLastVersion() == m_pFields->GetLastPostVersion() )
	{
		m_VersionList.AddNewVersion();	
	}

	//m_FieldList.AddTail(pField);
	m_FieldAllArray.Add(pField);
	CDWordArray* pDWORDArray = m_VersionList.GetVersion(GetLastVersion());
	pDWORDArray->InsertAt(nIndex,m_FieldAllArray.GetSize()-1);

}

/************************************************************************************
�������ƣ�
void CFieldsVersion::Modify(int nIndex,CField* pOldField, CField* pNewField)
����˵�����ֶ������޸�.

��ϸ����:	1.���m_LastPosVersion��GetLastVersion()��һ���Ծ����Ƿ���Ҫ
            ������汾����һ���°汾,�Լ���ʼ��������.

���������
[in]: 1.nIndex,��Ե�����λ��.
      2.pOldField,�ɵ��ֶ�ָ��.
	  3.pNewField,�µ��ֶ�ָ��.
  
[out]:��.

��������:void.

������YTLI 2002/08/29

�޸�: 
************************************************************************************/
void CFieldsVersion::Modify(int nIndex,CField* pOldField, CField* pNewField)
{
	if( GetLastVersion() == m_pFields->GetLastPostVersion() )
	{
		m_VersionList.AddNewVersion();	
	}

	//m_FieldList.AddTail(pNewField);
	m_FieldAllArray.Add(pNewField);
	int nNewIndex = m_FieldAllArray.GetSize()-1;

	pOldField->m_nNextModifyField = nNewIndex;

	CDWordArray* pDWORDArray = m_VersionList.GetVersion(GetLastVersion());	
	(*pDWORDArray)[nIndex] = nNewIndex;

}

/************************************************************************************
�������ƣ�
void CFieldsVersion::Clone(CFieldsVersion* pFieldsVersion)
����˵�������ݿ�¡�����ݴ�����ֶ����������¡���ֶ�����Ļ�����Ϣ.

��ϸ����:	1.�ڲ�����ÿ���ֶεĿ�¡����.

���������
[in]: 1.pFieldsVersion.
     
[out]:��.

��������:void.

������YTLI 2002/08/29

�޸�: 
************************************************************************************/
void CFieldsVersion::Clone(CFieldsVersion* pFieldsVersion)
{
	Clear();
	int nCount = pFieldsVersion->m_FieldAllArray.GetSize();
	for (int iPos = 0; iPos < nCount; iPos++)
	{
		//if(pField == m_FieldAllArray[iPos])
		CField* pField =(CField*) pFieldsVersion->m_FieldAllArray[iPos];
		CField* pNewField = pFieldsVersion->m_pFields->NewFieldType(m_pFields,pField);
		
		pNewField->Clone( pField );		
		m_FieldAllArray.Add(pNewField);
	}

	m_VersionList.Clone(&pFieldsVersion->m_VersionList);
}


/************************************************************************************
�������ƣ�
void CFieldsVersion::Clear()
����˵��������ֶμ�m_FieldList�������ֶ�.�Լ��汾��.

��ϸ����:	

���������
[in]: 1.FieldList,�����һ���ֶ�.
     
[out]:��.

��������:bool.

������YTLI 2002/08/29

�޸�: 
************************************************************************************/
void CFieldsVersion::Clear()
{
	//ɾ���ֶμ�m_FieldList�������ֶ�.
	while( m_FieldAllArray.GetSize())
	{
		CField* pField =(CField*) m_FieldAllArray[0];
		m_FieldAllArray.RemoveAt(0);
		delete pField;
	}

	//ɾ���汾��
	m_VersionList.Clear();

}

/************************************************************************************
�������ƣ�
void CFieldsVersion::ClearAllVersionPointer()
����˵���������е��ֶε�����ָ������ΪNULL.

��ϸ����:	1.�����е��ֶε�m_pValue����ΪNULL.

���������
[in]: 
     
[out]:��.

��������:void.

������YTLI 2002/08/29

�޸�: 
************************************************************************************/
void CFieldsVersion::ClearAllVersionPointer()
{
	int nCount = m_FieldAllArray.GetSize();
	for (int iPos = 0; iPos < nCount; iPos++)
	{
		CField*pField = (CField*)m_FieldAllArray[iPos];
		pField->m_pValue = NULL;
	}

}

/************************************************************************************
�������ƣ�
CField* CFieldsVersion::GetFieldByAbsolutePos(int nIndex)
����˵�������ݾ��������ŵõ��ֶ�ָ��.

��ϸ����:	

���������
[in]: 1.nIndex,
     
[out]:��.

��������:CField*.

������YTLI 2002/08/29

�޸�: 
************************************************************************************/
CField* CFieldsVersion::GetFieldByAbsolutePos(int nIndex)
{
	//assert(nIndex >= 0);
	return (CField*)m_FieldAllArray[nIndex];

}

CFields* CFieldsVersion::SetFields(CFields *pFields)
{
	CFields *ptemp = pFields;
	m_pFields = pFields;
	return ptemp;
}

/************************************************************************************
�������ƣ�
void CFieldsVersion::Add(CField* pField)
����˵�������һ���ֶ�,��ĩβ���.

��ϸ����:

���������
[in]: 1.pField,
     
[out]:��.

��������:void.

������YTLI 2002/08/29

�޸�: 
************************************************************************************/
void CFieldsVersion::Add(CField* pField)
{
	if( GetLastVersion() == m_pFields->GetLastPostVersion() )
	{
		m_VersionList.AddNewVersion();        
	}

	//m_FieldList.AddTail(pField);
	m_FieldAllArray.Add(pField);

	CDWordArray* pDWORDArray = m_VersionList.GetVersion(GetLastVersion());
	pDWORDArray->Add(m_FieldAllArray.GetSize()-1);
}



int CFieldsVersion::GetAbsoluteFieldCount()
{//###_ADD 2002-9-24 zlq
	return m_FieldAllArray.GetSize();
}
/************************************************************************************
�������ƣ�
void CFieldsVersion::Modify(int nIndex,CField* pOldField, CField* pNewField)
����˵�����ֶ��ƶ�.

��ϸ����:	1.���m_LastPosVersion��GetLastVersion()��һ���Ծ����Ƿ���Ҫ
            ������汾����һ���°汾,�Լ���ʼ��������.

���������
[in]: 
      1.nFrom,ԭ����������
	  2.nTo,�ƶ������µ�������(ע����������ֶλ���ʱ��λ��)
  
[out]:��.

��������:void.
������//#_S 2003-9-12 $ 9 zlq

�޸�: 
************************************************************************************/
void CFieldsVersion::Mov(int nFrom, int nTo)
{
	if (nFrom == nTo)
		return;
	if( GetLastVersion() == m_pFields->GetLastPostVersion() )
	{
		m_VersionList.AddNewVersion();	
	}

	CDWordArray* pDWORDArray = m_VersionList.GetVersion(GetLastVersion());
	if (nFrom < nTo)
		nTo--;//����ƶ�������,���ȼ�λ�ú�
	
	DWORD value = (*pDWORDArray)[nFrom] ;
	pDWORDArray->RemoveAt(nFrom);//����λ��ȥ��
	pDWORDArray->InsertAt(nTo,value);//�ƶ�����λ��

}
