// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "cfields.h"

extern BYTE  tlcfgBuf[] ;
extern BYTE  CurFileBuf[] ;
/************************************************************************************
�������ƣ�
      CFields::~CFields()
����˵����
    ��������
��ϸ���ͣ�
       1.ɾ�����е��ֶ�.
	  
���������
�������ͣ�
������
    YTLI 2002/07/08
�޸�: 
************************************************************************************/
CFields::~CFields()
{	
}

/************************************************************************************
�������ƣ�
      void CFields::GetBookmark()
����˵����
    ����ǩ.
��ϸ���ͣ�
       1.���浱ǰָ��ֵ,��������ոñ�ʱPhysicalEmptyTable()ʹ����ǩ.
	  
���������
�������ͣ�
������
    YTLI 2002/08/12
�޸�: 
************************************************************************************/
void CFields::GetBookmark()
{
	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		CField* pField = (CField*)m_FieldArray.GetAt(i);
		pField->m_pOldValue  = pField->m_pValue;
	}
	m_pOldDataSet = m_pDataSet;

}

void CFields::GotoBookmark()
{
	CField* pField ;
	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		pField = (CField*)m_FieldArray.GetAt(i);
		pField->m_pValue = pField->m_pOldValue;
	}
	m_pDataSet = m_pOldDataSet ;

}

void CFields::Clear()
{
	//===temp AfxMessageBox("�����˲��õ���!CFields::Clear()");
}
/************************************************************************************
�������ƣ�
void CFields::Add(CField* Field)
����˵�������ֶ�Field���뵽�ֶ�����m_FieldList��.

��ϸ���ͣ�
    
���������
[in]: 1.Field:�ֶ�ָ��.
     
[out]:��.

�������ͣ�void

������YTLI   2002/07/15

�޸�: 
***********************************************************************************/
void CFields::Add(CField* pField)
{
	if(!pField)
		return;
	
	m_FieldVersion.Add(pField);
	SetCurVersion();
}

/************************************************************************************
�������ƣ�
CField* CFields::FieldByName(CTString FieldName)
����˵���������ֶ��������ֶ���Ϣ.

��ϸ���ͣ�
    
���������
[in]: 1.FieldName:�ֶ���.
     
[out]:��.

�������ͣ�CField*

������YTLI   2002/07/15

�޸�: 
***********************************************************************************/
CField* CFields::FieldByName(CTString FieldName)
{	
	CField*pField ;
	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		pField = (CField*)m_FieldArray.GetAt(i);
		if(pField->GetFieldName() == FieldName)
			return pField;	
	}
	return NULL;
	/*
	POSITION pos = m_FieldList.GetHeadPosition();

	while(pos)
	{
		CField* pField = m_FieldList.GetNext(pos);
		
		if(pField->GetFieldName() == FieldName)
			return pField;	
	}
	return NULL;
	*/
}

/************************************************************************************
�������ƣ�
void CFields::Remove(CField* Field)
����˵����ɾ���ֶμ�m_FieldList�е�һ���ֶ�.

��ϸ���ͣ�
    
���������
[in]: 1.Field:Ҫɾ�����ֶ�ָ��.
     
[out]:��.

�������ͣ�void

������YTLI   2002/07/15

�޸�: 
***********************************************************************************/
void CFields::Remove(CField* pField)
{
	m_FieldVersion.Remove(pField);
	m_FieldVersion.SetVersion(m_FieldArray);	
}

/************************************************************************************
�������ƣ�
int CFields::IndexOf(CField* Field)
����˵�����õ�һ���ֶε�Index(��0��ʼ).

��ϸ���ͣ�1.������ֶμ���û���ҵ����ֶ�,�򷵻�-1.
    
���������
[in]: 1.Field:�ֶ�ָ��.
      2.��������ָ����Ч,�򷵻�-1.
     
[out]:��.

�������ͣ�int

������YTLI   2002/07/15

�޸�: 
***********************************************************************************/
int CFields::IndexOf(CField* Field)
{
	if(!Field)
		return -1;

	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		CField* pField =(CField*) m_FieldArray.GetAt(i);
		if( Field == pField )
			return i;
	}
	return -1;
	/*
	POSITION pos = m_FieldList.GetHeadPosition();
	while(pos)
	{
		CField* pField = m_FieldList.GetNext(pos);
		if( Field == pField )
			return i;
		i++;
	}

	return -1;
	*/
}

/************************************************************************************
�������ƣ�
int CFields::LoadFromBuffer(LPBYTE& lpData,int FieldCount)
����˵������Buffer��ȡָ���������ֶ���Ϣ.

��ϸ���ͣ�1.���ض�ȡ���ֽ���.
          2.�������Ĳ�����Ч,�򷵻�-1.
    
���������
[in]: 1.lpData:Ҫ������ʼ��ַ.
      2.FieldCount:Ҫ��ȡ���ֶθ���.	  
     
[out]:1.lpData:�������lpData����ƶ�FieldCount����¼��ĵ�ַ.

�������ͣ�int

������YTLI   2002/07/15

�޸�: 
***********************************************************************************/
int CFields::LoadFromBuffer(LPBYTE& lpData,int FieldCount)
{
	if( (!lpData) || (FieldCount <=0) )
        return -1;

	m_nCurVersion    = 0;
	m_nLastPosVersion = -1;

	int iRetValue = (int)lpData;
	int iFieldCnt = 0;

	while( iFieldCnt < FieldCount )
	{
		CFieldType chType = GetFieldType(lpData);
		CField* pField   = NewFieldType(this,chType);

		if(!pField)
			return -1;

		pField->m_pFields = this;
		lpData += pField->LoadFromBuffer(lpData);

		Add(pField);
		
		iFieldCnt++;
	}

	iRetValue = (int)lpData - iRetValue;
	return iRetValue;
}

/************************************************************************************
�������ƣ�
int CFields::SaveToBuffer(LPBYTE& lpData)
����˵�������ֶμ�m_FieldList���浽Buffer��.

��ϸ���ͣ�1.���ر�����ֽ���.
          2.�������ָ����Ч,�򷵻�-1.
    
���������
[in]: 1.lpData:Ҫ���浽Buffer����ʼ��ַ.    
     
[out]:1.lpData:�����ĵ�ַ.

�������ͣ�int

������YTLI   2002/07/15

�޸�: 
***********************************************************************************/
/*
int CFields::SaveToBuffer(LPBYTE& lpData)
{
	if(!lpData)
		return -1;

	int iRetValue = (int)lpData;
	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		int nOffset;
		CField* pField = m_FieldArray.GetAt(i);
		if(pField)
			lpData += pField->SaveToBuffer_(lpData);

	}

	iRetValue = (int)lpData - iRetValue;
	return iRetValue;
}
*/

int CFields::SaveToBuffer(TCLFile *pFile)
{
	if(!pFile)
		return false;

	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		CField* pField = (CField*)m_FieldArray.GetAt(i);
		if(pField)
			pField->SaveToBuffer(pFile);

	}

	return true;
}
/************************************************************************************
�������ƣ�
CField* CFields::NewFieldType(CField* clone)
����˵�������ݴ�����ֶ�������Ӧ���ֶ�����.

��ϸ���ͣ�1.���������ɵ��ֶ�.
    
���������
[in]: 1.clone:������ֶ�ָ��.    
     
[out]:��.

�������ͣ�CField*

������YTLI   2002/07/15

�޸�: 
***********************************************************************************/
CField* CFields::NewFieldType(CFields* pOwner ,CField* clone)
{
	if(!clone)
		return NULL;

	CFieldType chType = clone->GetFieldType();
	CField* pField = NewFieldType(pOwner,chType);

	return pField;
}

/************************************************************************************
�������ƣ�
CField* CFields::NewFieldType(CFieldType chType)
����˵�������ݴ�����ֶ�����������Ӧ���ֶ�����.

��ϸ���ͣ�1.���������ɵ��ֶ�.
          2.��������chType��Ч,�򷵻�NULL.
    
���������
[in]: 1.chType:������ֶ�����.    
     
[out]:��.

�������ͣ�CField*

������YTLI   2002/07/15

�޸�: 
***********************************************************************************/
CField* CFields::NewFieldType(CFields* pOwner ,CFieldType chType)
{
	CField* pField = NULL;//��������chType��Ч,�򷵻�NULL.

	switch(chType)
	{
	case fString :
		pField = new CStringField(this);
		break;
	case fDouble  :
		pField = new CDoubleField(this);
		break;
	case fInt :
		pField = new CIntField(this);
		break;
	case fBoolean :
		pField = new CBooleanField(this);
		break;
	case fDate :
		pField = new CDateField(this);
		break;
	case fCurrency :
		pField = new CCurrencyField(this);
		break;
	}
	
	CTString str;//###_Mod 2002-9-24 zlq  ȱʡ�ֶ���
	int nCount = m_FieldVersion.GetAbsoluteFieldCount();
	do
	{//#_S 2003-5-28 $ 9 zlq ȷ������ʱ,��Ψһ
		str.Format("%d",nCount+1);
		while (str.GetLength()<4)
		{//#_�޸� 2002-11-15 $ 9:15:44 zlq
			str = "0"+str;
		}
		str = "����" +str;
		nCount++;
	}while (FieldByName(str)!=NULL);
	pField->SetFieldName(str);
	pField->m_pFields = pOwner;
	return pField ;
}

/************************************************************************************
�������ƣ�
void CFields::Clone(CFields* pFields)
����˵�������ݴ�����ֶμ�����һ��һ�����ֶμ�.

��ϸ���ͣ�
    
���������
[in]: 1.pFields:������ֶμ�.    
     
[out]:��.

�������ͣ�void

������YTLI   2002/07/15

�޸�: 
���ݿ�¡��
���ݴ�����ֶ����������¡���ֶ�����Ļ�����Ϣ��
�ڲ����ø��ֶεĿ�¡����
***********************************************************************************/
void CFields::Clone(CFields* pFields)
{	
	if(!pFields)
		return;

	m_pOldDataSet     = pFields->m_pOldDataSet;
	m_pDataSet        = pFields->m_pDataSet;

	m_nCurVersion     = pFields->m_nCurVersion;
	m_nLastPosVersion = pFields->m_nLastPosVersion;

	m_FieldVersion.Clone(&pFields->m_FieldVersion);
}

/************************************************************************************
�������ƣ�
CFieldType CFields::GetFieldType(LPBYTE lpData)
����˵�����õ��ֶ�����,�����������ֶ�ʱ.

��ϸ���ͣ�
    
���������
[in]: 1.lpData:Buffer�и��ֶε��׵�ַ.    
     
[out]:��.

�������ͣ�CFieldType.

������YTLI   2002/07/15

�޸�: 
***********************************************************************************/
CFieldType CFields::GetFieldType(LPBYTE lpData)
{
	if(!lpData)
		return 0x0;

	return *(lpData);
}

/************************************************************************************
�������ƣ�
void CFields::NewData() 
����˵����Ϊһ����¼����һ���¿ռ�.

��ϸ���ͣ�
    
���������
[in]: ��.
     
[out]:��.

�������ͣ�void.

������YTLI   2002/07/15

�޸�: 
***********************************************************************************/
void CFields::NewData() 
{
	int nOldVersion = m_nCurVersion;
	SetCurVersion();

	ClearAllVersionPointer();

	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		CField* pField = (CField*)m_FieldArray.GetAt(i);
		if(pField)
			pField->NewData();
	}

	SetCurVersion(nOldVersion);	
}

/************************************************************************************
�������ƣ�
void CFields::DeleteData()
����˵����ɾ����ǰ��¼.

��ϸ���ͣ�
    
���������
[in]: ��.
     
[out]:��.

�������ͣ�void.

������YTLI   2002/07/15

�޸�: 
***********************************************************************************/
void CFields::DeleteData()
{	
	int nOldVersion = m_nCurVersion;

	SetCurVersion();

	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		CField* pField = (CField*)m_FieldArray.GetAt(i);
		if(pField)
			pField->DeleteData();
	}
/*
	POSITION pos = m_FieldList.GetHeadPosition();
	while(pos)
	{
		CField* pField = m_FieldList.GetNext(pos);
		
		if(pField)
			pField->DeleteData();
	}
*/
	SetCurVersion(nOldVersion);		
}

/************************************************************************************
�������ƣ�
CField* CFields::FieldByIndex(int index)
����˵������������ֵ�õ��ֶ���Ϣ.

��ϸ���ͣ�
    
���������
[in]: 1.index:����ֵ.
     
[out]:��.

�������ͣ�CField*.

������YTLI   2002/07/15

�޸�: 
***********************************************************************************/
CField* CFields::FieldByIndex(int index)
{
	if(index<0)
		return NULL;
	if (index>=m_FieldArray.GetSize())
	{
		return NULL;
	}

	return (CField*)m_FieldArray.GetAt(index);
}

/************************************************************************************
�������ƣ�

����˵������һ����¼���浽Buffer��.

��ϸ���ͣ�1.�ڲ���ÿһ���ֶε���CField::SaveDataToBuffer().
         2.���ر���ĵ�Buffer�е��ֽ���.
    
���������
[in]: 1.lpData:Ҫ���浽Buffer�е���ʼ��ַ.
      2.�������ָ����Ч,�򷵻�-1.

[out]:��.

�������ͣ�long.

������YTLI   2002/07/15

�޸�: 
***********************************************************************************/
long CFields::SaveDataToBuffer(LPBYTE lpData)
{
	if(!lpData)
		return -1;

	int iRetValue = (int)lpData;                 //����ַ����Ϊ���÷���ֵ
	
	int iFieldCnt  = m_FieldArray.GetSize();
	LPBYTE lpData2 = lpData;

	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		CField* pField = (CField*)m_FieldArray.GetAt(i);
		if(!pField)
			return -1;
        
		pField->SaveDataToBuffer(lpData2);	//���ô�ֵ
	}

	iRetValue = (int)(lpData2 - lpData);
	return iRetValue;
}

/************************************************************************************
�������ƣ�
bool CFields::LoadDataFromBuffer(LPBYTE& lpData, int& count)
����˵������Buffer�ж�ȡһ����¼.

��ϸ���ͣ�1.�ڲ���ÿһ���ֶε���CField::LoadDataFromBuffer().
          2.�������ָ����Ч,�򷵻�FALSE.
    
���������
[in]: 1.lpData:��Buffer�ж�ȡ����ʼ��ַ.
     
[out]:1.lpData:��һ����¼���׵�ַ.
      2.count:��ȡ���ֽ���.

�������ͣ�long.

������YTLI   2002/07/15

�޸�: 
***********************************************************************************/
bool CFields::LoadDataFromBuffer(LPBYTE& lpData, int& count)
{
	if( !lpData )
		return false;


	int iFieldCnt  = m_FieldArray.GetSize();
	LPBYTE lpData2 = lpData ;

	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		CField* pField = (CField*)m_FieldArray.GetAt(i);
		if(!pField)
			return false;

		pField->LoadDataFromBuffer(lpData2);
	}
	count = (int)( lpData2 - lpData);
	lpData = lpData2;

	return true;
}

/************************************************************************************
�������ƣ�
int CFields::QuerySaveLength()
����˵�����õ�����һ����¼��Buffer����Ҫ���ֽ���.

��ϸ���ͣ�1.�ڲ���ÿһ���ֶε���CField::QuerySaveLength().
          2.�������ִ��ʧ��,�򷵻�-1.
    
���������
[in]: ��.
     
[out]:��.

�������ͣ�int.

������YTLI   2002/07/15

�޸�: 
***********************************************************************************/
int CFields::QueryLength()
{
	int iRetValue = 0; 
	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		CField* pField = (CField*)m_FieldArray.GetAt(i);
		if(!pField)
			return -1;

		iRetValue += pField->QuerySaveLength();
	}
	/*
	POSITION pos  = m_FieldList.GetHeadPosition();

	while(pos)
	{
		CField* pField = m_FieldList.GetNext(pos);
		
		if(!pField)
			return -1;

		iRetValue += pField->QuerySaveLength();
	}
	*/
	return iRetValue;
}

/************************************************************************************
�������ƣ�
int CFields::GetFieldCount()
����˵�����õ��ֶμ���ӵ�е��ֶθ���.

��ϸ���ͣ�
    
���������
[in]: ��.
     
[out]:��.

�������ͣ�int.

������YTLI   2002/07/15

�޸�: 
***********************************************************************************/
int CFields::GetFieldCount()
{
	return m_FieldArray.GetSize();
}



//����ֶμ�m_FieldList�������ֶ�,������а汾.
void CFields::ClearAll()
{
	m_nCurVersion    = 0;
	m_nLastPosVersion = -1;

	m_FieldVersion.Clear();
	m_FieldArray.RemoveAll();
}

//ɾ����nIndex����һ���ֶ�.
bool CFields::Delete(int nIndex )
{
	m_FieldVersion.Delete(nIndex);
	SetCurVersion();
	return true;
}

//��nIndex������һ���ֶ�.
void CFields::Insert(CField* pField , int nIndex)
{
	m_FieldVersion.Insert(pField , nIndex);
	SetCurVersion();
}

//�ֶ������޸�
void CFields::Modify(int nIndex, CField* pNewField)
{	
	m_FieldVersion.Modify(nIndex,FieldByIndex(nIndex), pNewField);
///////////////////////////////////////////////{{lps added.2003/02/18
	CField* pOldFld = FieldByIndex(nIndex);

	LPBYTE pOldValue = pNewField->m_pValue;

	POSITION posData = pNewField->m_ValueLabel.m_DataList.GetHeadPosition();	
	
	while(posData)
	{
		pNewField->m_pValue = (LPBYTE)pNewField->m_ValueLabel.m_DataList.GetNext( posData );
		pNewField->RealDeleteData();	
	}
	pNewField->m_ValueLabel.m_DataList.RemoveAll();
	pNewField->m_ValueLabel.m_strLabelList.RemoveAll();

	posData = pOldFld->m_ValueLabel.m_DataList.GetHeadPosition();
    int posStr  = pOldFld->m_ValueLabel.m_strLabelList.GetSize();
    int nPos = 0;
	while(posData != NULL && nPos < posStr)
	{

 		LPBYTE pData = (LPBYTE)pOldFld->m_ValueLabel.m_DataList.GetNext( posData );
		CTString strLabel = pOldFld->m_ValueLabel.m_strLabelList.GetAt(nPos);
		pOldFld->m_ValueLabel.m_pField->m_pValue = pData;//~~~~~~~~~~~~~~~~~~~~~~~~~
		pNewField->RealNewData();
		//�������ֶ�ת������������,���ߴ������ֶ�ת���������ֶ�.
		if((pOldFld->GetFieldType() == fDate)
			||(pNewField->GetFieldType() == fDate))
		{
			FILETIME ftValue = pOldFld->m_ValueLabel.m_pField->GetAsDateTime();
			//���벻�ɹ�,��m_pValue��Ч.
			if(!pNewField->SetAsDateTime(&ftValue))
			{
				pNewField->m_pValue = pOldValue;
				pNewField->RealDeleteData();
				continue;
			}
		}
		else
		{
			CTString strr = pOldFld->m_ValueLabel.m_pField->GetAsString();
			//���벻�ɹ�,��m_pValue��Ч.��ʱ��ԭ��:���¿���ת����,�����޷�ת����.
			//����fString�ʹ�"2002/01/30"ת��ΪfInt,fDouble��ʱ��Ϊfalse.
			if(!pNewField->SetAsString(strr))
			{
				pNewField->m_pValue = pOldValue;
				pNewField->RealDeleteData();
				continue;
			}
		}
	
		pNewField->m_ValueLabel.m_DataList.AddTail(pNewField->m_pValue);

        //ȡ��label�ַ���Ȼ������ڵ�ǰ��������Ӧ��������
	//	CTString str = pValueLabel->m_strLabelList.GetNext(posStr);
		pNewField->m_ValueLabel.m_strLabelList.Add(strLabel);
		nPos++;
	}
	//assert(posData == NULL && posStr == NULL);
	pNewField->m_pValue = pOldValue;
///////////////////////////////////////////////}}
	SetCurVersion();
}

//�õ���ǰ���µİ汾,��������GetLastPostVersion�õ��İ汾��
//�������GetLastPostVersion��������������Ϊ1
//����: �汾��
int CFields::GetLastVersion()
{
	//assert(m_FieldVersion.GetLastVersion()==GetLastPostVersion() ||
	//	   m_FieldVersion.GetLastVersion()==GetLastPostVersion()+1 );

	return m_FieldVersion.GetLastVersion();
}

//�õ��ڴ������ύ�����ݵ����°汾��
//��С�ڵ���GetLastVersion��������������Ϊ1.
//����: �汾��
int CFields::GetLastPostVersion()
{
	//assert(m_FieldVersion.GetLastVersion() == m_nLastPosVersion ||
	//	   m_FieldVersion.GetLastVersion() == m_nLastPosVersion + 1 );

	return m_nLastPosVersion; 
}

//���ð汾,Ĭ��Ϊ���°汾.����װ�ؾɰ汾����.���������µĿռ�.
void CFields::SetCurVersion(int nVersion)
{
	if (nVersion == -1)
		nVersion = m_FieldVersion.GetLastVersion();

	m_FieldVersion.SetVersion(m_FieldArray,nVersion);
	m_nCurVersion = nVersion;

}

//�õ���ǰ�ֶεİ汾,���㿪ʼ.
int CFields::GetCurVersion()
{
	return m_nCurVersion;
}

//�жϵ�ǰ�ֶ��Ƿ��������ֶ���.
bool CFields::IsConvertToLastVersion(CField* pField)
{
	return m_FieldVersion.IsConvertToLastVersion(pField);
}



//�����µ��ֶκ�û��ԭ�����ֶμ�Ĳ����ȷ���Ƿ���Ҫ�����汾��
//����
//true ��Ҫ����
//false ����Ҫ
bool CFields::IsNeedUpdateVersion(int nIndex, CField* pNewField )
{
	if(pNewField == FieldByIndex(nIndex))
		return false;
	
	return true;
}

//�����а汾���ֶε�����ָ������ΪNULL������CFieldsVersion�ĸ÷���ʵ��.
void CFields::ClearAllVersionPointer()
{
	m_FieldVersion.ClearAllVersionPointer();
}

CFields::CFields(CPages* pOwner)
{	
	CFields *pthis = this;
#ifdef MarkWay_Encrypt  //�Ǽ��ܰ汾
	BYTE *pData3 = (unsigned char *)&pthis;
	for (int i= 0;i<15 ;i++)
		*(pData3 + i % 4 ) = *(pData3 + i % 4 )^tlcfgBuf[i];
#endif	
	m_pPages = pOwner;
#ifdef MarkWay_Encrypt  //�Ǽ��ܰ汾
	BYTE *pData4 = (unsigned char *)&pthis;
	for ( i= 0;i<15 ;i++)
		*(pData4 + i % 4 ) = *(pData4 + i % 4 )^CurFileBuf[i];
#endif	

	m_FieldVersion.SetFields(pthis);
#ifdef MarkWay_Encrypt  //�Ǽ��ܰ汾
	BYTE *pData = (unsigned char *)&m_pPages;
	for ( i= 0;i<15 ;i++)
	{
		*(pData + i % 4 ) = *(pData + i % 4 )^tlcfgBuf[i];
	}
#endif


	m_pOldDataSet    = 0;
	m_pDataSet       = 0;
	m_nCurVersion    = 0;
	m_nLastPosVersion = -1;
	m_FieldArray.SetSize(0,_IncreaseNum);

#ifdef MarkWay_Encrypt  //�Ǽ��ܰ汾
	BYTE *pData2 = (unsigned char *)&m_pPages;
	for ( i= 0;i<15 ;i++)
	{
		*(pData2 + i % 4 ) = *(pData2 + i % 4 )^CurFileBuf[i];
	}
#endif
}

int CFields::GetAbsoluteFieldCount()
{//###_ADD 2002-9-24 zlq
	return m_FieldVersion.GetAbsoluteFieldCount();
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
void CFields::Mov(int nFrom, int nTo)
{
	m_FieldVersion.Mov(nFrom,nTo);
	SetCurVersion();
}
