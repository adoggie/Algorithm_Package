// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "fieldlist.h"
/************************************************************************************
�������ƣ�
	CFieldList::~CFieldList
����˵����
	��������
��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2002-9-4
�޸�:

************************************************************************************/

CFieldList::~CFieldList()
{
}

/************************************************************************************
�������ƣ�
	CFieldList::CFieldList
����˵����
	���캯��
��ϸ���ͣ�
	��CDataAccess�ڲ�����.
���������
	[in]pPages:
�������ͣ�

������
	Eric 2002-9-4
�޸�:

************************************************************************************/
CFieldList::CFieldList(CPages * pPages)
{
	m_pPages = pPages;
}
/************************************************************************************
�������ƣ�
	CFieldList::Add
����˵����
	����һ���ֶ�
��ϸ���ͣ�

���������
	[in] :�����ӵ��ֶ�ָ��,�ɵ���CreateField����.
�������ͣ�

������
	Eric 2002-9-4
�޸�:
************************************************************************************/
bool CFieldList::Add(CField* pField)
{//#_�޸� 2002-11-15 $ 11:15:24 zlq
	if (m_pPages->m_pFields->IndexOf(pField)>=0)
	{//�Ѿ������ڸ��ֶμ���
		////===temp AfxMessageBox("���ֶ��Ѿ������ڸ��ֶμ���!");
		return false;
	}
	if (m_pPages->m_pFields->FieldByName(pField->GetFieldName()))
	{//�Ѿ�����ͬ���ִ����ڸ��ֶμ���
		////===temp AfxMessageBox("�Ѿ�����ͬ���ִ����ڸ��ֶμ���!");
		return false;
	}
	m_pPages->AddField(pField);
	return true;
}
/************************************************************************************
�������ƣ�
	CFieldList::Clear
����˵����
	����ֶμ�m_FieldList�������ֶ�,����ǰ�汾
��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2002-9-4
�޸�:

************************************************************************************/
void CFieldList::Clear()
{
	while (m_pPages->m_pFields->GetFieldCount())
		Delete(0);
}
/************************************************************************************
�������ƣ�
	CFieldList::Delete
����˵����
	ɾ��һ���ֶ�
��ϸ���ͣ�

���������
	[in]nIndex :Ҫɾ���ֶ����ֶμ��е�����
�������ͣ�

������
	Eric 2002-9-4
�޸�:

************************************************************************************/
bool CFieldList::Delete(int nIndex)
{
	m_pPages->DeleteField(nIndex);
	return true;
}
/************************************************************************************
�������ƣ�
	CFieldList::Insert
����˵����
	����һ���ֶ�
��ϸ���ͣ�

���������
	[in,out]pField :Ҫ������ֶ�
	[in,out] nIndex	:�����λ��
�������ͣ�

������
	Eric 2002-9-4
�޸�:

************************************************************************************/
bool CFieldList::Insert(CField * pField,int nIndex)
{//#_�޸� 2002-11-15 $ 11:16:02 zlq
	if (m_pPages->m_pFields->IndexOf(pField)>=0)
	{//�Ѿ������ڸ��ֶμ���
		//===temp AfxMessageBox("���ֶ��Ѿ������ڸ��ֶμ���!");
		return false;
	}
	if (m_pPages->m_pFields->FieldByName(pField->GetFieldName()))
	{//�Ѿ�����ͬ���ִ����ڸ��ֶμ���
		//===temp AfxMessageBox("�Ѿ�����ͬ���ִ����ڸ��ֶμ���!");
		return false;
	}
	m_pPages->InsertField(nIndex,pField);
	return true;
}
/************************************************************************************
�������ƣ�
	CFieldList::Modify
����˵����
	�޸�һ���ֶ�
��ϸ���ͣ�

���������
	[in,out]nIndex:���ֶε�λ��
	[in,out]pField :�µ��ֶ�
�������ͣ�

������
	Eric 2002-9-4
�޸�:

************************************************************************************/
bool CFieldList::Modify(int nIndex,CFieldType NewFieldType)
{
	CField *pField = this->FieldByIndex(nIndex);
	if (pField ==NULL)
		return false;
	if (pField->GetFieldType() == NewFieldType)
		return true;
	CField* pNewField=CreateField(NewFieldType);
	pNewField->CopyCommonProperty(pField);
	Modify(IndexOf(pField),pNewField);
	return true;
}
bool CFieldList::Modify(int nIndex,CField * pField)
{//#_�޸� 2002-11-15 $ 11:18:36 zlq
	if (m_pPages->m_pFields->IndexOf(pField)>=0)
	{//�Ѿ������ڸ��ֶμ���
		//===temp AfxMessageBox("���ֶ��Ѿ������ڸ��ֶμ���!");
		return false;
	}
	CField *pTemp = m_pPages->m_pFields->FieldByName(pField->GetFieldName());
	if (pTemp)
	{//�Ѿ�����ͬ���ִ����ڸ��ֶμ���

		if (m_pPages->m_pFields->IndexOf(pTemp) !=nIndex)
		{
			//===temp AfxMessageBox("�Ѿ�����ͬ���ִ����ڸ��ֶμ��в�ͬ��λ��!");
			return false;
		}
	}
	m_pPages->ModifyField(nIndex,pField);
	return true;
}
/************************************************************************************
�������ƣ�
	CFieldList::FieldByName
����˵����
	���ֶ�����ȡ���ֶ�
��ϸ���ͣ�

���������
	[in]FieldName:�ֶ�����
�������ͣ�

������
	Eric 2002-9-4
�޸�:

************************************************************************************/
CField* CFieldList::FieldByName(CTString FieldName)
{
	return m_pPages->m_pFields->FieldByName(FieldName);
}
/************************************************************************************
�������ƣ�
	CFieldList::FieldByIndex
����˵����
	�����ȡ���ֶ�
��ϸ���ͣ�

���������
	[in]index:�ֶ����
�������ͣ�

������
	Eric 2002-9-4
�޸�:

************************************************************************************/
CField* CFieldList::FieldByIndex(int index)
{
	return m_pPages->m_pFields->FieldByIndex(index);
}
/************************************************************************************
�������ƣ�
	CFieldList::IndexOf
����˵����
	ȡ��ĳ�ֶε�λ�ã����㿪ʼ��    ʧ�ܣ��򷵻�-1.
��ϸ���ͣ�

���������
	[in]Field :�ֶ�ָ��
�������ͣ�

������
	Eric 2002-9-4
�޸�:

************************************************************************************/
int CFieldList::IndexOf(CField* pField)
{
	return m_pPages->m_pFields->IndexOf(pField);
}

/************************************************************************************
�������ƣ�
	CFieldList::GetFieldCount
����˵����
	ȡ���ֶ���
��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2002-9-4
�޸�:

************************************************************************************/
int CFieldList::GetFieldCount()
{
	return 	m_pPages->m_pFields->GetFieldCount();
}
/************************************************************************************
�������ƣ�
	CFieldList::CreateField
����˵����
	�½�һ��Field
��ϸ���ͣ�

���������
	[in]FieldType:�ֶ�����
�������ͣ�

������
	Eric 2002-9-4
�޸�:

************************************************************************************/
CField* CFieldList::CreateField(CFieldType FieldType)
{
	return 	m_pPages->m_pFields->NewFieldType(m_pPages->m_pFields,FieldType);
}
/************************************************************************************
�������ƣ�
	CFieldList::CreateField
����˵����
	�½�һ���ֶ�
��ϸ���ͣ�

���������
	[in] pTempleField:ģ���ֶ�
�������ͣ�

������
	Eric 2002-9-4
�޸�:

************************************************************************************/
CField* CFieldList::CreateField(CField * pTempleField)
{
	return 	m_pPages->m_pFields->NewFieldType(m_pPages->m_pFields,pTempleField);
}

/******************************************************
����˵����
��ϸ���ͣ�
���������
�������ͣ�
������
�޸�:
******************************************************/
struct TVarInfo
{
	CTString	strName;
	CTString	strBaseAls;
	int count;
	
//	TVarInfo(){	memset(this, 0, sizeof(*this));}
	TVarInfo(CTString strName,CTString strBaseAls)
	{
		strName = strName;
		strBaseAls = strBaseAls;
		count = 0;
	}
	int ParseText(CTString str,int nCount)
	{
		if(str == "")
			return 0;
		
  		//char* pChar =str.GetBuffer(str.GetLength());
		//str.ReleaseBuffer();
		char* pChar =str.GetData();
		bool bSingleWChar = false;
		
		int nOffset;
		for(nOffset = 0;(nOffset<nCount && *pChar);nOffset++)
		{
			if(*pChar < 0)
			{
				bSingleWChar = ! bSingleWChar;
			}
			pChar++;
		}
		
		if(bSingleWChar)
		{
			return nOffset-1;
		}
		else
		{
			return nOffset;
		}   
	}
	
	CTString AdjustAlias()
	{
		int iNameSize  = this->strName.GetLength();
		
		if(iNameSize <= 6)//���ڻ����3��
		{
			this->strBaseAls = this->strName;
		}
		else if((iNameSize > 6) && (this->count == 1))//���ڻ����4��,�Ҹ�������100(iNameSize <= 8) &&
		{
			int iCnt = iNameSize <= 10 ? iNameSize:10;
			iCnt = ParseText(strName,iCnt);
			this->strBaseAls = this->strName.Left(iCnt);//10
		}
		else if((iNameSize > 6) && (this->count < 100))//���ڻ����4��,�Ҹ�������100(iNameSize <= 8) &&
		{
			int iCnt = iNameSize <= 8 ? iNameSize:8;
			iCnt = ParseText(strName,iCnt);
			this->strBaseAls = this->strName.Left(iCnt);
		}
		else if((iNameSize > 6) && (this->count <= 9999))//���ڻ����4��,�Ҹ�������99,���֧��1000����Ҳ����չ
		{
			int iCnt = iNameSize <= 6 ? iNameSize:6;
			iCnt = ParseText(strName,iCnt);
			this->strBaseAls = this->strName.Left(6);
		}
		else if((iNameSize > 6) && (this->count <= 999999))
		{
			int iCnt = iNameSize <= 4 ? iNameSize:4;
			iCnt = ParseText(strName,iCnt);
			this->strBaseAls = this->strName.Left(iCnt);
		}
		else if((iNameSize > 6) && (this->count > 999999))
		{
			int iCnt = iNameSize <= 4 ? iNameSize:4;
			iCnt = ParseText(strName,iCnt);
			this->strBaseAls = this->strName.Left(iCnt);
		//	//===temp AfxMessageBox("���򿪵����ݱ���Ŀ����1000000��");
		}
		return this->strBaseAls;
	}
};
/************************************************************************************
�������ƣ�
	CFieldList::GetExclusiveName
����˵����
	���strFldName���ֶμ��е�Ψһ�ֶ���,�����ֶ�������ʱ��ȡ.
��ϸ���ͣ�

���������
	[in] :strFldName,�ֶ�����.
�������ͣ�

������
�޸�:

************************************************************************************/
CTString CFieldList::GetExclusiveName(CField* pFld)
{
	CTString strOldName = pFld->GetFieldName();
	CTString strName(strOldName);
	int Cnt = 0;
	TVarInfo var(strName,"");
	var.strName = strName;
	while(FieldByName(strName))
	{
		Cnt++;
		var.count = Cnt;
		strName = var.AdjustAlias();
		strName.Format("%s%d",strName,Cnt);
	}
	return strName;
}

/************************************************************************************
�������ƣ�

����˵�����ֶ��ƶ�

���������
[in]: 
      1.nFrom,ԭ����������
	  2.nTo,�ƶ������µ�������(ע����������ֶλ���ʱ��λ��)
��������:void.

������//#_S 2003-9-12 $ 10 zlq

�޸�: 
************************************************************************************/
bool CFieldList::Mov(int nFrom, int nTo)
{
	int nCount = m_pPages->m_pFields->GetFieldCount();
	if (nFrom<0 || nFrom>=nCount)
	{//��Χ�ж�	
		return false;
	}
	if (nTo<0 || nTo>=nCount)
	{//��Χ�ж�
		return false;
	}

	m_pPages->MoveField(nFrom,nTo);
	return true;
}

//ȡ��ĳ�ֶε�λ�ã����㿪ʼ��    ʧ�ܣ��򷵻�-1.
int CFieldList::IndexOf(CTString strFieldName)
{
	CField *pField = FieldByName(strFieldName);
	if (pField ==NULL )
		return -1;
	return m_pPages->m_pFields->IndexOf(pField);
}
