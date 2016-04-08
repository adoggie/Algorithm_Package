// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "cfield.h"
#include "cfields.h"


#include "cpage.h"
#include "cpages.h"



///const LPBYTE CField::m_pValue = 0;
CField::CField()
{
	m_sLabel = _T("");
	m_sName  = _T("");	
	
	m_pValue     = 0;	
	m_pOldValue  = 0;	
	m_pFields    = 0;	
	m_nExtLength = 0;
	m_nExtCount	 = 0;
	m_nWidth	 = 8;
	m_nDecimals	 = 2; 

	m_pDefaultValue = 0;	
	m_byteEnumType  = 0xFF; 
	m_byteEnumData  = ddmmmyyyy_line;
	m_bIsEnum	    = true;
	m_bHasDefault   = false;

	//m_ValueLabel.m_pField = this;
	m_ValueLabel.SetOwner(this);

	m_nNextModifyField = -1;
	m_pMissing = new CMissing(this);
	m_byteMeasure = msOrdinal;

	m_nTempBufLength = 100;
	m_pTempBuf = new char[m_nTempBufLength+2];
}

CField::~CField()
{
	if(m_pMissing)
		delete m_pMissing;
	m_pMissing = 0;

	delete []m_pTempBuf;
}

/************************************************************************************
�������ƣ�
CFieldType CField::GetFieldType()
����˵�����麯��,�����ֶ�����.

��ϸ���ͣ�1.����ʵ��.      

���������
[in]: ��
  
[out]:�ޡ�

�������ͣ�CFieldType

������YTLI 2002/07/12

�޸�: 
************************************************************************************/
//CFieldType CField::GetFieldType()
//{
	//����ʵ��
//	return fDouble;
//}

/************************************************************************************
�������ƣ�
int CField::LoadFromBuffer(LPBYTE lpData)
����˵�����麯��,�Ӵ����lpData��ַ��ʼ��ȡһ���ֶε�������Ϣ,������չ����Ϣ.

��ϸ���ͣ�1.�Ӵ����lpData��ַ��ʼ�Ը����ֽڽ���.
          2.��չ�����ֵ���LoadExt()��ʵ��.
		  3.���ض�ȡ���ֽ���.
		  4.�������ָ����Ч,�򷵻�-1.

���������
[in]: 1.lpData:��ʼ��ȡ�ĵ�ַ.
  
[out]:�ޡ�

�������ͣ�int

������YTLI 2002/07/15

�޸�: 
************************************************************************************/
int CField::LoadFromBuffer(LPBYTE lpData)
{	
	if(!lpData)
		return -1;
	
	int   iRetVal   = int(lpData);  

	int   iLength   = 0;          
	LPBYTE pCharTemp = NULL;       //ָ����ݳ��ȶ�ȡ��Ӧ������
	
	lpData++;                     //����'�ֶ�����',lpDataָ��'�ֶγ���'
	
	//�õ����ֶ����ĳ���
	m_nWidth = *(WORD*)lpData;

	lpData += 2;

    //�õ����ֶε�С��λ��
	m_nDecimals = (int)*lpData;
	
	lpData++;
	//#_S 2003-3-31 $ 16 zlq ��չ�ֶ�������
	//�õ��ֶ���
	{//�϶���1.2��
		int nFieldNameLength = (int)*lpData;//��һ���ֽ�Ϊ�ֶ����Ƴ���
		lpData++;//ָ�����ƶ� 1
		pCharTemp = new BYTE[nFieldNameLength+1];
		
		if( pCharTemp == NULL )
			return 0;
		memcpy( pCharTemp, lpData, nFieldNameLength );
		pCharTemp[nFieldNameLength] = '\0';
		m_sName = _T(pCharTemp);
		m_sName.TrimRight(0x20);  //ȥ����β�Ŀո�
		delete []pCharTemp;
		lpData += nFieldNameLength;//ָ�����ƶ� nFieldNameLength
	}

	//�õ����ֶ�Label����
	iLength = (int)*lpData;   //���ֶ�Label����

	lpData++;                 //ָ����ֶ�Label����

	pCharTemp = new BYTE[iLength+1];

	if( pCharTemp == NULL )
		return 0;  
	memcpy( pCharTemp, lpData, iLength );
	pCharTemp[iLength] = '\0';
	m_sLabel = _T(pCharTemp );
	delete []pCharTemp;
	lpData += iLength;        //ָ����ֶ���չ������

	//�õ����ֶ���չ������
	{
		m_nExtLength = *(int*)lpData;
		lpData += 4;              //ָ����ֶ�'��չ�����ݿ�����'
	}
	
	m_nExtCount = (int)*lpData;                  //�õ���չ�����ݿ�����

	lpData += 1;              //ָ����ֶ���չ��

	//�õ����ֶ���չ����ϸ��Ϣ
	LoadExt( lpData, m_nExtLength );
	
	lpData += m_nExtLength;        //ָ����ֶ���չ��β

	iRetVal = (int)lpData - iRetVal;
	return iRetVal;
}

/************************************************************************************
�������ƣ�
bool CField::LoadDataFromBuffer(LPBYTE& lpIndicate, LPBYTE& lpData)
����˵�����麯��,��ȡһ����¼ĳһ�ֶε�����,����������ȡֵ��ǩ.

��ϸ���ͣ�1.����ʵ��.      

���������
[in]: 1.lpIndicate:ָʾ�ֽڵ�ַ.
      2.lpData    :���ݵ�ַ.
  
[out]:1.lpIndicate:��һ���ݿ��ָʾ�ֽڵ�ַ.
      2.lpData    :��һ���ݿ�����ݵ�ַ.��

�������ͣ�bool

������YTLI 2002/07/12

�޸�: 
************************************************************************************/
void CField::LoadDataFromBuffer( LPBYTE& lpData)
{	
	if(m_pValue==NULL)
	{//�ɰ汾
		if(m_nNextModifyField == -1)
		{
			NewData();
			LoadDataFromBuffer(lpData);			
			DeleteData();
		}
		else
		{
			if(m_pFields->IsConvertToLastVersion(this))
			{
				NewData();
				LoadDataFromBuffer(lpData);
				CField* pNewField = m_pFields->m_FieldVersion.GetFieldByAbsolutePos(m_nNextModifyField);
				pNewField->ConvertToCurField(this); 
				DeleteData();
			}
		}
	}
	else
	{//��ǰ����
		if (GetFieldType() == fString)
		{
			FieldString* pStr = (FieldString*)m_pValue;
			char *pBuf = new char[m_nWidth+2];
			memset(pBuf,0,m_nWidth+2);
			memcpy(pBuf,lpData,m_nWidth);
			*pStr= pBuf;
			/*
  			char* lpstr = pStr->GetBuffer(m_nWidth+2);
			memcpy(lpstr,lpData,m_nWidth);
			*(lpstr+m_nWidth)= 0;
			*(lpstr+m_nWidth+1)= 0;
			pStr->ReleaseBuffer();	
			*/
			//pStr->TrimLeft();//,��߿ո���
			pStr->TrimRight();   

			
			lpData =  lpData + Align4(m_nWidth);
		}
		else
		{
			memcpy( &*((FieldNUM *)m_pValue), lpData, sizeof(FieldNUM) );	//ytli Modify 2002/09/04
			lpData =  lpData + sizeof(FieldNUM);
		}
		
	}
}

/************************************************************************************
�������ƣ�
CTString CField::GetAsString()
����˵�����麯��,��m_pValve��ָ���������CString�ͷ���.

��ϸ���ͣ�1.�������û��ʵ��,���ʵ�ִ�����ʾ,��ʾ����ת��.
         2.�������ʵ��,����������ķ���.

���������
[in]: ��.      
  
[out]:��.��

�������ͣ�CString

������YTLI 2002/07/15

�޸�: 
************************************************************************************/
CTString CField::GetAsString()
{
	//throw _T("�����ṩ����ת��");
	//assert(0);
	////===temp AfxMessageBox(_T("�����ṩת��\nCField::GetAsString�����ؿ�ֵ"));
	return "";
}

bool CField::GetAsBool()
{
	//throw _T("�����ṩ����ת��");
	//assert(0);
	//	//===temp AfxMessageBox(_T("�����ṩת��\nCField::GetAsBool������FALSE"));
	return false;
}
/************************************************************************************
�������ƣ�
bool CField::SetAsString(CTString Value)
����˵�����麯��,��m_pValve��ָ��������޸�Ϊ�����CString������.

��ϸ���ͣ�1.�������û��ʵ��,���ʵ�ִ�����ʾ,��ʾ����ת��.
         1.�������ʵ��,����������ķ���.

���������
[in]: ��.      
  
[out]:��.��

�������ͣ�bool

������YTLI 2002/07/15

�޸�: 
************************************************************************************/
bool CField::SetAsString(CTString Value)
{	
	if(m_nNextModifyField == -1)
		return false;
	else
	{
		if(m_pFields->IsConvertToLastVersion(this))
		{
			NewData();
			SetAsString(Value);//###_Mod 2002-9-26 zlq
			CField* pNewField = m_pFields->m_FieldVersion.GetFieldByAbsolutePos(m_nNextModifyField);			
			pNewField ->ConvertToCurField(this);
			DeleteData();
		}
	}
	return true;
}

/************************************************************************************
�������ƣ�
int CField::GetAsInteger()
����˵�����麯��,��m_pValve��ָ���������int�ͷ���.

��ϸ���ͣ�1.�������û��ʵ��,���ʵ�ִ�����ʾ,��ʾ����ת��.
         1.�������ʵ��,����������ķ���.

���������
[in]:��.      
  
[out]:��.

��������:int


������YTLI 2002/07/15

�޸�: 
************************************************************************************/
int CField::GetAsInteger()
{
	//throw _T("�����ṩ����ת��");
	//assert(0);
	////===temp AfxMessageBox(_T("�����ṩת��\nCField::GetAsInteger�����ؿ�ֵ"));
	return 0;
}

/************************************************************************************
�������ƣ�
bool CField::SetAsInteger(int Value)
����˵�����麯��,��m_pValve��ָ��������޸�Ϊ�����int������.

��ϸ���ͣ�1.�������û��ʵ��,���ʵ�ִ�����ʾ,��ʾ����ת��.
         2.�������ʵ��,����������ķ���.

���������
[in]: ��.      
  
[out]:��.��

�������ͣ�bool

������YTLI 2002/07/15

�޸�: 
************************************************************************************/
bool CField::SetAsInteger(int Value)
{
	if(m_nNextModifyField == -1)
		return false;
	else
	{
		if(m_pFields->IsConvertToLastVersion(this))
		{
			NewData();
			SetAsInteger(Value);
			CField* pNewField = m_pFields->m_FieldVersion.GetFieldByAbsolutePos(m_nNextModifyField);			
			pNewField ->ConvertToCurField(this);
			DeleteData();
		}
	}
	return true;
}

/************************************************************************************
�������ƣ�
int CField::GetDataLength(LPBYTE lpData)
����˵�����麯��,�������ݳ���,������ָʾλ,���ڶ��ļ��ȶ�ʱȷ����һ����λ��.

��ϸ���ͣ�1.����ʵ��,��ͬ���ͷ��ز�ͬ����.       

���������
[in]:1.lpData:ָ�����ݵ�ָ��,�䳤ʱ��(���ַ���),����ʱΪ��. 
  
[out]:��.

��������:int


������YTLI 2002/07/15

�޸�: 
************************************************************************************/
int CField::GetDataLength(LPBYTE lpData)
{
	//����ʵ��
	return 0;
}

/************************************************************************************
�������ƣ�

����˵�����麯��,��m_pValve��ָ���������FILETIME*�ͷ���.

��ϸ���ͣ�1.�������û��ʵ��,���ʵ�ִ�����ʾ,��ʾ����ת��.
         2.�������ʵ��,����������ķ���.

���������
[in]:��.      
  
[out]:��.

��������:FILETIME *


������YTLI 2002/07/15

�޸�: 
************************************************************************************/
FILETIME  CField::GetAsDateTime()
{
	//throw _T("�����ṩ����ת��");
	//assert(0);
	//	//===temp AfxMessageBox(_T("�����ṩת��\nCField::GetAsDateTime�����ؿ�ֵ"));
	FILETIME ft;
	return ft;
}

/************************************************************************************
�������ƣ�
bool CField::SetAsDateTime(FILETIME *Value)
����˵�����麯��,��m_pValve��ָ��������޸�Ϊ�����FILETIME*������.

��ϸ���ͣ�1.�������û��ʵ��,���ʵ�ִ�����ʾ,��ʾ����ת��.
         2.�������ʵ��,����������ķ���.

���������
[in]: ��.      
  
[out]:��.��

�������ͣ�bool

������YTLI 2002/07/15

�޸�: 
************************************************************************************/
bool CField::SetAsDateTime(FILETIME *Value)
{
	if(m_nNextModifyField == -1)
		return false;
	else
	{
		if(m_pFields->IsConvertToLastVersion(this))
		{
			NewData();
			SetAsDateTime(Value);//###_Mod 2002-9-26 zlq
			CField* pNewField = m_pFields->m_FieldVersion.GetFieldByAbsolutePos(m_nNextModifyField);			
			pNewField ->ConvertToCurField(this);
			DeleteData();
		}
	}
	return true;
}

/************************************************************************************
�������ƣ�

����˵�����麯��,��m_pValve��ָ���������FieldCurrency�ͷ���.

��ϸ���ͣ�1.�������û��ʵ��,���ʵ�ִ�����ʾ,��ʾ����ת��.
         2.�������ʵ��,����������ķ���.

���������
[in]:��.      
  
[out]:��.

��������:FieldCurrency


������YTLI 2002/07/15

�޸�: 
************************************************************************************/
FieldCurrency CField::GetAsCurrency()
{
	//throw _T("�����ṩ����ת��");
	//assert(0);
	//	//===temp AfxMessageBox(_T("�����ṩת��\nCField::GetAsCurrency�����ؿ�ֵ"));
	FieldCurrency b;
	//b.Indicate = fvNULL;
	return b;
}

/************************************************************************************
�������ƣ�

����˵�����麯��,��m_pValve��ָ��������޸�Ϊ�����FieldCurrency������.

��ϸ���ͣ�1.�������û��ʵ��,���ʵ�ִ�����ʾ,��ʾ����ת��.
         2.�������ʵ��,����������ķ���.

���������
[in]: ��.      
  
[out]:��.��

�������ͣ�bool

������YTLI 2002/07/15

�޸�: 
************************************************************************************/
bool CField::SetAsCurrency(FieldCurrency Value)
{	
	if(m_nNextModifyField == -1)
		return false;
	else
	{
		if(m_pFields->IsConvertToLastVersion(this))
		{
			NewData();
			SetAsCurrency(Value);
			CField* pNewField = m_pFields->m_FieldVersion.GetFieldByAbsolutePos(m_nNextModifyField);			
			pNewField ->ConvertToCurField(this);

			DeleteData();
		}
	}
	return true;
}

bool CField::SetAsBool(bool Value)
{		
	if(m_nNextModifyField == -1)
		return false;
	else
	{
		if(m_pFields->IsConvertToLastVersion(this))
		{
			NewData();
			SetAsBool(Value);//###_Mod 2002-9-26 zlq
			CField* pNewField = m_pFields->m_FieldVersion.GetFieldByAbsolutePos(m_nNextModifyField);			
			pNewField ->ConvertToCurField(this);
			DeleteData();
		}
	}
	return true;
}


/************************************************************************************
�������ƣ�
int CField::GetTypeSize()
����˵���������������͵Ĵ�С,��Ϊ������������¿ռ�ʱȷ��.

��ϸ���ͣ�1.(����ָʾλ������,string����ʱΪ4).    

���������
[in]: ��.      
  
[out]:��.��

�������ͣ�int

������YTLI 2002/07/15

�޸�: 
************************************************************************************/
//int CField::GetTypeSize()
//{	
	//����ʵ��	
//	return 0;
//}

/************************************************************************************
�������ƣ�
bool CField::IsNull()
����˵�����鿴m_pValve��ָ��������Ƿ�Ϊ��NULL.

��ϸ���ͣ�1.���m_pValve��ָ�������Ϊ��NULL,����TRUE.    

���������
[in]: ��.      
  
[out]:��.��

�������ͣ�bool

������YTLI 2002/07/15

�޸�: 
************************************************************************************/
bool CField::IsNull()
{
	if(!m_pValue)
		return false;
	if (GetFieldType() == fString)
	{
		FieldString* pStr = (FieldString*)m_pValue;
		return (pStr->GetLength()==0);
	}
	else
	{
		return ( *((FieldNUM*)m_pValue ) ==NULLValue);
	}

	return false;
}

/************************************************************************************
�������ƣ�
double CField::GetAsDouble()
����˵�����麯��,��m_pValve��ָ���������double�ͷ���.

��ϸ���ͣ�1.�������û��ʵ��,���ʵ�ִ�����ʾ,��ʾ����ת��.
         2.�������ʵ��,����������ķ���.

���������
[in]:��.      
  
[out]:��.

��������:double


������YTLI 2002/07/15

�޸�: 
************************************************************************************/
double CField::GetAsDouble()
{	
	//throw _T("�����ṩ����ת��");
	//assert(0);
	////===temp AfxMessageBox(_T("�����ṩת��\nCField::GetAsDouble�����ؿ�ֵ"));
	return 0;
}

/************************************************************************************
�������ƣ�
bool CField::SetAsDouble(double Value)
����˵�����麯��,��m_pValve��ָ��������޸�Ϊ�����double������.

��ϸ���ͣ�1.�������û��ʵ��,���ʵ�ִ�����ʾ,��ʾ����ת��.
         2.�������ʵ��,����������ķ���.

���������
[in]: ��.      
  
[out]:��.��

�������ͣ�bool

������YTLI 2002/07/15

�޸�: 
************************************************************************************/
bool CField::SetAsDouble(double Value)
{
	if(m_nNextModifyField == -1)
		return false;
	else
	{
		if(m_pFields->IsConvertToLastVersion(this))
		{
			NewData();
			SetAsDouble(Value);//###_Mod 2002-9-26 zlq 
			CField* pNewField = m_pFields->m_FieldVersion.GetFieldByAbsolutePos(m_nNextModifyField);
			pNewField ->ConvertToCurField(this);
			DeleteData();
		}
	}
	return true;
}

/************************************************************************************
�������ƣ�
void CField::SetNull()
����˵������m_pValve��ָ�����������Ϊ��ֵNULL.

��ϸ���ͣ�

���������
[in]: ��.      
  
[out]:��.��

�������ͣ�void

������YTLI 2002/07/15

�޸�: 
************************************************************************************/
void CField::SetNull()
{
	if (GetFieldType() == fString)
		*(FieldString*)m_pValue = "";
	else
		*(FieldNUM*)m_pValue = NULLValue;
}


int CField::SaveToBuffer(TCLFile *pFile)
{
	if(!pFile)
		return false;


	BYTE bType = (BYTE)GetFieldType();
	pFile->Write(&bType,1);

	pFile->Write(&m_nWidth,2);//

	pFile->Write(&m_nDecimals,1);//ָ����ֶ�С���ĳ���.
	//#_S 2003-3-31 $ 16 zlq ��չ�ֶ�������**************************
	//�õ��ֶ���

	
	{//�ļ���
		int iFieldNameLength = m_sName.GetLength();
		pFile->Write(&iFieldNameLength,1);

		pFile->Write((const char*)m_sName,iFieldNameLength);
	}
	//��ʱ lpData ָ�� Label ����***************************

	int iFieldLabelLength = m_sLabel.GetLength();
	pFile->Write(&iFieldLabelLength,1);

	//ָ����ֶ� Label ������
	pFile->Write((const char*)m_sLabel,iFieldLabelLength);

	//��ʱ lpData ָ����ֶ���չ���ĳ���*****************************
	int wdExtLength = 0;
	int  iExtCnt     = 0; //��չ���Ŀ���.
	int nTitleLength = 0;

	nTitleLength = Ext_TitleLength110;

	if(m_bIsEnum)
	{
		//===temp TRACE("ԭʼwdExtLength %d\n",wdExtLength);
		wdExtLength += nTitleLength + 2;//ö��
		iExtCnt++;
	}
	
	if( m_ValueLabel.m_strLabelList.GetSize() != 0 )
	{
		//===temp TRACE("m_ValueLabel %d\n",m_ValueLabel.QueryLength());
		wdExtLength += (m_ValueLabel.QueryLength() + nTitleLength);//ֵ��ǩ.
		iExtCnt++;
	}
	
	if( m_ShowCharacteristic.QueryLength() != 0 )
	{
		//===temp TRACE("m_ShowCharacteristic %d\n",m_ShowCharacteristic.QueryLength());
		wdExtLength += (m_ShowCharacteristic.QueryLength() + nTitleLength);//��ʾ����
		iExtCnt++;
	}
	if (GetFieldType() ==fCurrency)
	{
		CCurrencyField *pField = (CCurrencyField*)this;
		wdExtLength += 3+pField->m_strCurrencyType.GetLength();
		iExtCnt++;
	}

	//### 2002-9-25 Eric==>
	wdExtLength += 1;//m_byteMeasure	
	wdExtLength += m_pMissing->QueryLength();
	//===temp TRACE("m_pMissing %d\n",m_pMissing->QueryLength());
	//### 2002-9-25 Eric<==
	BYTE *lpBuf = new BYTE[wdExtLength+5];
	BYTE *lpData = lpBuf;
	{
		memcpy( lpData, &wdExtLength, 4 );
		lpData += 4;               //ָ����ֶ���չ���Ŀ���
	}

	*lpData = iExtCnt;        
	lpData += 1;               //ָ����ֶ���չ��

	SaveExt(lpData,wdExtLength);
	pFile->Write(lpBuf,wdExtLength+5);
	//iRetVal = (int)lpData - iRetVal + wdExtLength;        
	delete []lpBuf;
	return true;
}
/************************************************************************************
�������ƣ�

����˵������m_pValueָ���ֵ���ļ���ʽ����Buffer��.

��ϸ���ͣ�1.
          2.�������ݳ���.
		  3.�������ָ����Ч,�򷵻�-1.

���������
[in]: 1.lpData:ָ�������Ϊ����ָʾ+��������. 
  
[out]:��. 

�������ͣ�long

������YTLI 2002/07/15

�޸�: 
************************************************************************************/
void CField::SaveDataToBuffer(LPBYTE& lpData)
{
	if (GetFieldType() == fString)
	{
  		FieldString* pStr = (FieldString*)m_pValue;

		char* lpstr = pStr->GetData();
		memcpy(lpData,lpstr,pStr->GetLength()+1);

		/*char* lpstr = pStr->GetBuffer(m_nWidth+2);
		memcpy(lpData,lpstr,m_nWidth);
		pStr->ReleaseBuffer();	   */
		lpData +=Align4(m_nWidth);
	}
	else
	{
		memcpy( lpData,m_pValue, sizeof(FieldNUM) );	//ytli Modify 2002/09/04
		lpData +=sizeof(FieldNUM);
	}

}

/************************************************************************************
�������ƣ�
void CField::LoadExt(LPBYTE lpData, int count)
����˵������Buffer�ж�ȡ�ֶε���չ����Ϣ.

��ϸ���ͣ�1.protected����,��LoadFromBuffer����.       

���������
[in]: 1.lpData:��չ������ʼ�ֽڵ�ַ. 
      2.count:��չ���ĳ���.
  
[out]:��. 

�������ͣ�void

������YTLI 2002/07/15

�޸�: 
************************************************************************************/
void CField::LoadExt(LPBYTE lpData, int count)
{	
	if(!lpData || count <= 0 )
		return;

	//��ʱ lpData ָ�����ݿ�1�ı�־λ
	int iByteCnt  = 0;        //��չ������
	int iBlockCnt = 0;        //��ĸ���
	m_bIsEnum = false;        //��ʼ��ΪFALSE;	
	
	while(iByteCnt < count && iBlockCnt < m_nExtCount)
	{
		iBlockCnt++;

		if((int)*lpData == Ext_Enum )
		{

			lpData += Ext_TitleLength110; //ָ��ö�����͵����ݽṹ,��Ϊö�����ͺ�ö��ֵ��ռһ���ֽڣ����ö����ݿ�ĳ���
			iByteCnt += Ext_TitleLength110 +2;


			m_bIsEnum      = true;		
			m_byteEnumType = *(lpData);

			lpData++;         //ָ��ö��ֵ

			m_byteEnumData = *(lpData);          	
			
			lpData++;         //lpDataָ����һ���ݿ�
		}
		else if((int)*lpData == Ext_ValueLabel )
		{//ֵ��ǩ
			lpData += 1;   
			int wExtLength = 0;

			wExtLength = *(int*)lpData;
			lpData += sizeof(int);      //ָ����ö�����͵�����			
			iByteCnt += (Ext_TitleLength110 + wExtLength);

			m_ValueLabel.LoadFromBuffer(lpData,wExtLength);
			lpData += wExtLength;                //lpDataָ����һ���ݿ�
		}
		else if((*lpData) == Ext_ShowCharacteristic )//���޸�
		{			
			lpData += 1;      //ָ����ʾ���Եĳ���
			int wShowLength = 0;
			{
				wShowLength = *(int*)lpData;
				lpData += sizeof(int);      //ָ����ʾ���Ե�����
				iByteCnt += ( Ext_TitleLength110 + wShowLength);
			}

			m_ShowCharacteristic.loadShowCharacter(lpData,wShowLength);
			lpData += wShowLength;
            
		}
		else//
		{//��������ʶ�ģ���������
			lpData += 1;   
			int wExtLength = 0;
			{
				wExtLength = *(int*)lpData;
				lpData += sizeof(int);      //ָ����ö�����͵�����			
				iByteCnt += (Ext_TitleLength110 + wExtLength);
			}

			lpData += wExtLength;                //lpDataָ����һ���ݿ�			
		}
	}

	////### 2002-9-25 Eric ===>
	//Missing ,Mesure ������չ����
	m_byteMeasure = *lpData;
	lpData ++;
	m_pMissing->LoadFromBuffer(lpData);
	//### 2002-9-25 Eric<===
}

/************************************************************************************
�������ƣ�
void CField::SaveExt(LPBYTE lpData, int count)
����˵��������չ������Ϣ����Buffer��.

��ϸ���ͣ�1.protected����,��SaveToBuffer����.

���������
[in]: 1.lpData:��չ������ʼ�ֽڵ�ַ. 
      2.count:��չ���ĳ���.
  
[out]:��. 

�������ͣ�void

������YTLI 2002/07/15

�޸�: 
************************************************************************************/

void CField::SaveExt(LPBYTE lpData, int count)
{
	if(!lpData || count <= 0 )
		return;

	int iByteCnt  = 0;        //��չ������;

	if( m_bIsEnum == true && iByteCnt < count )
	{
		*lpData = Ext_Enum;//ö��
		lpData += 1;

		//���볤��ֵ2
		if (m_pFields->m_pPages->GetSaveFileVersion()==100)
		{//�����1.0�汾		
			WORD wdTemp = 2;
			memcpy(lpData,&wdTemp,sizeof(WORD));
			lpData += sizeof(WORD);
			iByteCnt += Ext_TitleLength100 +2;
		}
		else
		{
			int wdTemp = 2;
			memcpy(lpData,&wdTemp,sizeof(int));
			lpData += sizeof(int);
			iByteCnt += Ext_TitleLength110 +2;//����Ϊ���ݳ���
		}

		//����ö������
		*lpData = m_byteEnumType;
		lpData += 1;

		//����ö��ֵ
		*lpData = m_byteEnumData;
		lpData += 1;
	
	}
	if( m_ValueLabel.m_strLabelList.GetSize() != 0 && iByteCnt < count )
	{			
		*lpData = Ext_ValueLabel;//ֵ��ǩ
		lpData += 1;

		//���볤��ֵ
		int wLength = m_ValueLabel.QueryLength();
		if (m_pFields->m_pPages->GetSaveFileVersion()==100)
		{//�����1.0�汾
			WORD wL = wLength;
			memcpy(lpData,&wL,sizeof(WORD));
			lpData += sizeof(WORD);
			iByteCnt += (Ext_TitleLength100 + wLength);
		}
		else
		{
			memcpy(lpData,&wLength,sizeof(int));
			lpData += sizeof(int);	
			iByteCnt += (Ext_TitleLength110 + wLength);
		}

		//����ö������
		lpData += m_ValueLabel.SaveToBuffer(lpData);
		
	}
	if(m_ShowCharacteristic.QueryLength() != 0 && iByteCnt < count )                //��������ʾ���Ե�
	{
		*lpData = Ext_ShowCharacteristic;//��־Ϊ��ʾ����
		lpData += 1;

		//���볤��ֵ
		int wLength = m_ShowCharacteristic.QueryLength();
		if (m_pFields->m_pPages->GetSaveFileVersion()==100)
		{//�����1.0�汾
			WORD wL = wLength;
			memcpy(lpData,&wL,sizeof(WORD));
			lpData += sizeof(WORD);
			iByteCnt += (Ext_TitleLength100 + wLength);
		}
		else
		{
			memcpy(lpData,&wLength,sizeof(int));
			lpData += sizeof(int);	
			iByteCnt += (Ext_TitleLength110 + wLength);
		}

		//������ʾ��������
		lpData += m_ShowCharacteristic.saveShowCharacter(lpData);	

	}

	//### 2002-9-25 Eric==>
	*lpData = m_byteMeasure;
	lpData ++;
	m_pMissing->SaveToBuffer(lpData);
	//### 2002-9-25 Eric<==
}

/************************************************************************************
�������ƣ�
void CField::Clone(CField* pField)
����˵��: ���ݿ�¡�����ݴ�����ֶ������¡�ֶεĻ�����Ϣ.
        
��ϸ����:1.����m_ValueLabel,����CValueLabel���Clone()����. 
         2.����m_ShowCharacteristic,����CShowCharacteristic���Clone()����. 
  
�������:
[in]: 1.pField:������ֶ�����.
 
[out]:��.

��������:void.

����:YTLI      2002/07/12

�޸�: 
************************************************************************************/
void CField::Clone(CField* pField)
{	
	if(!pField)
		return ;

	m_bIsEnum      = pField->m_bIsEnum;
	m_byteEnumData = pField->m_byteEnumData;
	m_byteEnumType = pField->m_byteEnumType;
	m_nDecimals    = pField->m_nDecimals;
	m_nExtCount    = pField->m_nExtCount;
	m_nExtLength   = pField->m_nExtLength;
	m_nWidth       = pField->m_nWidth;
	m_sLabel       = pField->m_sLabel;
	m_sName        = pField->m_sName;
	m_byteMeasure  = pField->m_byteMeasure;	  //### 2002-9-25 Eric

	//m_ValueLabel.m_pField = this;
	m_ValueLabel.SetOwner(this);
    m_ValueLabel.Clone(&pField->m_ValueLabel);
	m_ShowCharacteristic.Clone(&pField->m_ShowCharacteristic);
	m_pMissing->Clone(pField->m_pMissing); //### 2002-9-25 Eric

}

/************************************************************************************
�������ƣ�
void CField::NewData()
����˵��: �麯��,�����µ����ݿռ�,�����ݼ�����insert,append�Ȳ���ʱ,�ȵ���.
        
��ϸ����:1.����ʵ��,��ͬ�����в�ͬ�Ĵ�С. 
  
�������:
[in]: ��.
 
[out]:��.

��������:void.

����:YTLI      2002/07/12

�޸�: 
************************************************************************************/

void CField::RealNewData()
{
	if (GetFieldType() == fString)
	{
		FieldString* pStr = new FieldString();
		
		m_pValue = (LPBYTE)(pStr);
	}
	else
	{
		m_pValue =(LPBYTE) new FieldNUM;
		*(FieldNUM*)m_pValue = NULLValue;
	}
}


void CField::NewData()
{
	//m_pFields->m_pPages->m_Depot.NewData(this);
	m_pValue =(LPBYTE) m_pTempData;
	SetNull();
}

//ɾ��m_pValue��ָ��Ŀռ�
void CField::RealDeleteData()
{
	if (GetFieldType() == fString)
	{
		FieldString* pStr = (FieldString*)m_pValue;
		delete pStr;
		m_pValue = NULL;
	}
	else
	{
		if( m_pValue )
			delete[] m_pValue;
		
		m_pValue = NULL;
	}

}

void CField::DeleteData()
{
	//m_pFields->m_pPages->m_Depot.DeleteData(this);//ytli Modify 2002/09/03
	m_pValue = NULL;
}

/************************************************************************************
�������ƣ�
bool CField::GetData(LPBYTE lpData)
����˵��: �麯��,����lpDataָ�������,���Ƹ�����ֵ�����ֶ�,��������ͬ�����ݸ�ʽ,
          ���û�����edit,cancel����ʱ�ָ�ԭֵ.

��ϸ����:1.����ʵ��. 
  
�������:
[in]: 1.lpData :��Ҫ���Ƶ����ݵĵ�ַ.
 
[out]:�ޡ�

��������:bool.

����:YTLI      2002/07/12

�޸�: 
************************************************************************************/
bool CField::GetData(void* lpData)
{
	return 0;
}
bool CField::GetData(void*pDestArrar,int nIndex)
{
	return 0;
}
/************************************************************************************
�������ƣ�
LPBYTE CField::SetData()
����˵��: �麯��,�����ֶε���������һ������,��ַ�ռ�Ҳ�ɱ���������,�����ظõ�ַ,
          �������ݴ��ݺͱ����ֵ.
��ϸ����:1.����ʵ��. 
  
�������:
[in]: ��.
  
[out]:�ޡ�

��������:LPBYTE.

����:YTLI      2002/07/12

�޸�: 
************************************************************************************/
/*
LPBYTE CField::SetData()
{
	//����ʵ��
	return (LPBYTE)0;
}
*/
/************************************************************************************
�������ƣ�
int CField::QuerySaveLength()
����˵��: �麯��,�õ���ǰ�ֶε�ֵ(m_pValueָ���)���浽�ļ�ʱ����ĳ���,����ָʾλ,Ҳ���ܾ�
          ָʾλҲ����,����ȷ���������Ĵ�С.
��ϸ����:1.����ʵ��. 
         2.��С��ֵ���.

�������:
[in]:��.
  
[out]:�ޡ�

��������:int.

����:YTLI 2002/07/12

�޸�: 
************************************************************************************/

int CField::QuerySaveLength()
{
	if (GetFieldType() == fString)
	{

		return Align4(m_nWidth);
	}
	else
	{
		return sizeof(FieldNUM);
	}
}

/************************************************************************************
�������ƣ�
bool CField::GetFromDisplay(CTString str)
����˵��: �麯��,���ⲿ�û����������(�ַ�����ʽ)���浽�ֶ���.

��ϸ����:1.����ʵ��,��ͬ�����в�ͬ��ʵ��. 
        2. ����ɹ�,�򷵻�true.

�������:
[in]: 1.str: �ӽ����ϵõ����ַ���.
  
[out]:�ޡ�

��������:bool.

����:YTLI     2002/07/16

�޸�:
************************************************************************************/
bool CField::GetFromDisplay(CTString str)
{
	//����ʵ��;
	return false;
}



//����ǰ��������Ϊȱʡֵ,����m_pDefaultValue��ָ������ݿ�����m_pValue��
void CField::SetDefaultValue()
{
}

//�õ�ǰ������������Ϊȱʡֵ,����m_pValue��ָ������ݿ�����m_pDefaultValue�У�ԭ������û������
bool CField::CopyToDefaultValue()
{
	return true;
}

//��ԭ���汾���ֶ�����ת������ǰ�ֶε����ݡ�
//1��������ֶοռ�Ϊ�գ����ɿռ�
//2��Ҫ����ԭ���ֶο�ֵ�жϣ�
//2��1���Ϊ�����ֶ�Ϊ��
//2��2�����Ϊ����ԭ����������ԭ�����������Ͷ�    ����Ȼ����ñ��ֶε�SetAs***���á�
//Ȼ��������µ�next�ֶΣ�����ø��ֶε�ConvertToCurField���������Ҹ����ͷű��ռ䣬������0
void CField::ConvertToCurField(CField* pOldField)
{
	/*
	if(this->GetFieldType() == fString)
	{
		ConvertToCurField(pOldField);
		return ;
	}
*/
	if(!m_pValue)
	{//���ֶ�ҲΪ�м��ֶ�
		NewData();

		if(pOldField->IsNull())
			SetNull();
		else
		{
			CFieldType ftType = pOldField->GetFieldType();
			if( ftType == fBoolean )			
				SetAsBool( pOldField->GetAsBool());

			else if( ftType == fCurrency )			
				SetAsCurrency( pOldField->GetAsCurrency());

			else if( ftType == fDate )			
				//SetAsBool( pOldField->GetAsDateTime());
				SetAsBool( true);//#_�޸� 2002-12-2 $ 10:01:35 zlq
			else if( ftType == fDouble )			
				SetAsDouble( pOldField->GetAsDouble());

			else if( ftType == fInt )			
				SetAsInteger( pOldField->GetAsInteger());

			else if( ftType == fString )			
				SetAsString( pOldField->GetAsString());		

			//assert(m_nNextModifyField != -1);

			CField* pNewField = m_pFields->m_FieldVersion.GetFieldByAbsolutePos(m_nNextModifyField);
			pNewField->ConvertToCurField(this); 			
		}
		DeleteData();
	}
	else   //���һ��ת��.
	{
		if(pOldField->IsNull())		
			SetNull();			
		else
		{
			if( pOldField->GetFieldType() == fBoolean )			
				SetAsBool( pOldField->GetAsBool());

			else if( pOldField->GetFieldType() == fCurrency )			
				SetAsCurrency( pOldField->GetAsCurrency());

			else if( pOldField->GetFieldType() == fDate )			
				//SetAsBool( pOldField->GetAsDateTime());
				SetAsBool( true);

			else if( pOldField->GetFieldType() == fDouble )			
				SetAsDouble( pOldField->GetAsDouble());

			else if( pOldField->GetFieldType() == fInt )			
				SetAsInteger( pOldField->GetAsInteger());

			else if( pOldField->GetFieldType() == fString )			
				SetAsString( pOldField->GetAsString());		

			//assert(m_nNextModifyField == -1);
		}
	}
}

void CField::SetLabel(CTString strLabel)
{
	m_sLabel = strLabel;
}

CTString CField::GetLabel()
{
	return m_sLabel;
}

bool CField::SetFieldName(CTString strFieldName)
{
	if (m_pFields)
	{
		if (m_pFields->IndexOf(this)>=0)
		{//�ڵ�ǰ�汾�ֶ���
			CField*pTemp = m_pFields->FieldByName(strFieldName);
			if (pTemp)
			{
				if (pTemp != this)
				{//����Ǳ����ֶ�,������
					//===temp AfxMessageBox("�Ѿ��и��ֶ����ƣ�����ʧ�ܣ�");
					return false;
				}
			}
		}
	}
	if (strFieldName.GetLength()>255)
	{
	//	//===temp AfxMessageBox("���ֳ��Ȳ��ܳ���10���ַ���");
		return false;
	}
	if (strFieldName.Find('\"') != -1 )
		return false;
	if (strFieldName.Find('*') != -1 )
		return false;
	if (strFieldName.Find('?') != -1 )
		return false;
	if (strFieldName.Find('+') != -1 )
		return false;
	if (strFieldName.Find('-') != -1 )
		return false;
	if (strFieldName.Find('=') != -1 )
		return false;
	if (strFieldName.Find('/') != -1 )
		return false;
	if (strFieldName.Find('>') != -1 )
		return false;
	if (strFieldName.Find('<') != -1 )
		return false;
	if (strFieldName.Find('&') != -1 )
		return false;
	if (strFieldName.Find('|') != -1 )
		return false;
	if (strFieldName.Find('(') != -1 )
		return false;
	if (strFieldName.Find(')') != -1 )
		return false;
	if (strFieldName.Find('[') != -1 )
		return false;
	if (strFieldName.Find(']') != -1 )
		return false;
	if (strFieldName.Find('{') != -1 )
		return false;
	if (strFieldName.Find('}') != -1 )
		return false;
	if (strFieldName.Find('%') != -1 )
		return false;
	if (strFieldName.Find('$') != -1 )
		return false;
	if (strFieldName.Find('^') != -1 )
		return false;
	if (strFieldName.Find('\'') != -1 )
		return false;
	if (strFieldName.Find(';') != -1 )
		return false;
	if (strFieldName.Find(':') != -1 )
		return false;
	if (strFieldName.Find(',') != -1 )
		return false;
	if (strFieldName.Find('.') != -1 )
		return false;

	//char tt = strFieldName.GetAt(0);
	//if (tt>='0' && tt<='9')//��һ���ַ�����Ϊ����
	//	return false;

	m_sName = strFieldName;
	return true;
}

CTString CField::GetFieldName()
{
	return m_sName;
}

void CField::SetDecimals(int nDecimals)
{
	m_nDecimals = nDecimals;
}

int CField::GetDecimals()
{
	return m_nDecimals;
}

void CField::SetWidth(int nWidth)
{
	m_nWidth = nWidth;
}

int CField::GetWidth()
{
	return m_nWidth;
}

//### 2002-9-25 Eric===>
CMeasure CField::GetMeasure()
{
	return m_byteMeasure;
}

void CField::SetMeasure(CMeasure measure)
{
	m_byteMeasure = measure;
}
//### 2002-9-25 Eric<===

//ȡ����ʾ��ص�����
//###_Mod 2002-11-1 zlq
bool CField::GetViewSet(CViewSet &ViewSet)
{
	//*************�е�����******************//	
	ViewSet.nCWidth			= m_ShowCharacteristic.m_nCWidth		;
	ViewSet.cCColor			= m_ShowCharacteristic.m_cCColor		;
	ViewSet.byteCAlign		= m_ShowCharacteristic.m_byteCAlign		;
	ViewSet.strCFontName	= m_ShowCharacteristic.m_strCFontName	;
	ViewSet.byteCFontStyle	= m_ShowCharacteristic.m_byteCFontStyle	;
	ViewSet.byteCFontSize	= m_ShowCharacteristic.m_byteCFontSize	;
	//*************�е�����******************//
	ViewSet.nRWidth			= m_ShowCharacteristic.m_nRWidth		;	
	ViewSet.cRColor			= m_ShowCharacteristic.m_cRColor		;
	ViewSet.byteRAlign		= m_ShowCharacteristic.m_byteRAlign		;
	ViewSet.strRFontName	= m_ShowCharacteristic.m_strRFontName	;
	ViewSet.byteRFontStyle	= m_ShowCharacteristic.m_byteRFontStyle	;
	ViewSet.byteRFontSize	= m_ShowCharacteristic.m_byteRFontSize	;	
	return true;	
}

//������ʾ��ص�����
bool CField::SetViewSet(CViewSet ViewSet)
{
	//*************�е�����******************//
	m_ShowCharacteristic.m_nCWidth			= ViewSet.nCWidth			;
	m_ShowCharacteristic.m_cCColor			= ViewSet.cCColor			;				
	m_ShowCharacteristic.m_byteCAlign		= ViewSet.byteCAlign		;
	m_ShowCharacteristic.m_strCFontName		= ViewSet.strCFontName		;
	m_ShowCharacteristic.m_byteCFontStyle	= ViewSet.byteCFontStyle	;
    m_ShowCharacteristic.m_byteCFontSize	= ViewSet.byteCFontSize		;
	//*************�е�����******************//
	m_ShowCharacteristic.m_nRWidth 			= ViewSet.nRWidth			;
	m_ShowCharacteristic.m_cRColor			= ViewSet.cRColor			;
	m_ShowCharacteristic.m_byteRAlign		= ViewSet.byteRAlign		;
	m_ShowCharacteristic.m_strRFontName		= ViewSet.strRFontName		;
	m_ShowCharacteristic.m_byteRFontStyle	= ViewSet.byteRFontStyle	;
	m_ShowCharacteristic.m_byteRFontSize	= ViewSet.byteRFontSize		;
	return true;
}


//ȡ���������ͣ���ɢ��/�����ͣ�
int CField::GetDCDataType(int nIndex)
{
	int nMeasure=GetMeasure(),
		nDataType=GetFieldType();
	
	if(nMeasure== msNominal)
		return CONTINU_DATA;
	
	if(nDataType==fBoolean || nDataType==fString || nMeasure==msScale || nMeasure==msOrdinal)
		return DISPERSE_DATA;
	
	return 0;
}

///cwh 2002-11-14
//���ݵ�ǰ��Ԫ�������ڱ�����(m_ValueLable)��ȡ����Ӧ�ı���ֵ
LPCTSTR CField::GetValueLable()
{
    LPCTSTR strValue=GetAsString();

    int nSize=m_ValueLabel.GetCount();
    for(int n=0;n<nSize;n++)
    {
            m_ValueLabel.SetCurPos(n);
            if(m_ValueLabel.GetAsString()==strValue)
                    return m_ValueLabel.GetExplainString();
    }

    return _T("");
}
//
CTString CField::GetValLable()
{
	CTString strValue = GetAsString();

	int nSize=m_ValueLabel.GetCount();
	for(int n=0;n<nSize;n++)
	{
		m_ValueLabel.SetCurPos(n);
		if(m_ValueLabel.GetAsString()==strValue)
			return m_ValueLabel.GetExplainString();
	}
	
	return _T("");
}

bool CField::SetEnumType(DateType nType)
{
	return true;
}

DateType CField::GetEnumType()
{
	return -1;
}

bool CField::SetDateConvertType(BYTE cType)
{
	if( cType>=48 && cType<=127 )
	{
		m_bIsEnum = true;
		m_byteEnumData = cType;
		return true;
	}
	m_bIsEnum = false;
	return false;
}

BYTE CField::GetDateConvertType()
{
	return m_byteEnumData;
}



/*��Ŀ���ֶ�pSourceField�����ֶ���Ϣ
1:Ŀ���ֶ���Դ�ֶ�ͬ����ʱ,����:
	m_sName m_bIsEnum  m_byteEnumData 
	m_byteEnumType m_nDecimals   m_nExtCount  m_nExtLength   m_nWidth       
	m_sLabel       m_byteMeasure 	m_ValueLabel	m_ShowCharacteristic
	m_pMissing
2:Ŀ���ֶ���Դ�ֶβ�ͬ����ʱ,����:
	�ֶ���m_sName.
	���Ŀ���ֶ�Ϊ:
	1:fInt:m_nDecimals = 0.
	2:fBoolean:m_nWidth = 2,m_nDecimals = 0*/
void CField::CopyCommonProperty(CField *pSourceField)
{
	if(!pSourceField)
		return;
	
	//�ֶ���������Ҫ��.
	m_sName        = pSourceField->m_sName;
	switch(pSourceField->GetFieldType())
	{
	case fString:
		{
			switch(GetFieldType())
			{
			case fString:
				m_bIsEnum      = pSourceField->m_bIsEnum;
				m_byteEnumData = pSourceField->m_byteEnumData;
				m_byteEnumType = pSourceField->m_byteEnumType;
				m_nDecimals    = pSourceField->m_nDecimals;
				m_nExtCount    = pSourceField->m_nExtCount;
				m_nExtLength   = pSourceField->m_nExtLength;
				m_nWidth       = pSourceField->m_nWidth;
				m_sLabel       = pSourceField->m_sLabel;
				m_byteMeasure  = pSourceField->m_byteMeasure;	  //### 2002-9-25 Eric
				
			//	m_ValueLabel.m_pField = this;
				m_ValueLabel.Clone(&pSourceField->m_ValueLabel);
				m_ShowCharacteristic.Clone(&pSourceField->m_ShowCharacteristic);
				m_pMissing->Clone(pSourceField->m_pMissing); //### 2002-9-25 Eric
				break;
			case fInt:
				m_nDecimals    = 0;//pSourceField->m_nDecimals;
				break;
			case fDouble:
				break;
			case fBoolean:
				m_nWidth = 2;
				break;
			case fCurrency:
				break;
			case fDate:
				m_nWidth = 12;
				break;
			}
		};
		break;
	case fInt:
		{
			m_nDecimals    = 0;//pSourceField->m_nDecimals;
			switch(GetFieldType())
			{
			case fString:
				break;
			case fInt:
				m_bIsEnum      = pSourceField->m_bIsEnum;
				m_byteEnumData = pSourceField->m_byteEnumData;
				m_byteEnumType = pSourceField->m_byteEnumType;
				m_nDecimals    = pSourceField->m_nDecimals;
				m_nExtCount    = pSourceField->m_nExtCount;
				m_nExtLength   = pSourceField->m_nExtLength;
				m_nWidth       = pSourceField->m_nWidth;
				m_sLabel       = pSourceField->m_sLabel;
				m_sName        = pSourceField->m_sName;
				m_byteMeasure  = pSourceField->m_byteMeasure;	  //### 2002-9-25 Eric
				
			//	m_ValueLabel.m_pField = this;
				m_ValueLabel.Clone(&pSourceField->m_ValueLabel);
				m_ShowCharacteristic.Clone(&pSourceField->m_ShowCharacteristic);
				m_pMissing->Clone(pSourceField->m_pMissing); //### 2002-9-25 Eric
			case fDouble:
				m_nDecimals    = 2;
				m_nWidth       = pSourceField->m_nWidth;
				break;
			case fBoolean:
				m_nWidth = 2;
				break;
			case fCurrency:
				m_nDecimals    = 2;
				m_nWidth       = pSourceField->m_nWidth;
				break;
			case fDate:
				m_nWidth = 12;
				break;
			}
		};
		break;
	case fDouble:
		{
			switch(GetFieldType())
			{
			case fString:
				break;
			case fInt:
				break;
			case fDouble:
				m_bIsEnum      = pSourceField->m_bIsEnum;
				m_byteEnumData = pSourceField->m_byteEnumData;
				m_byteEnumType = pSourceField->m_byteEnumType;
				m_nDecimals    = pSourceField->m_nDecimals;
				m_nExtCount    = pSourceField->m_nExtCount;
				m_nExtLength   = pSourceField->m_nExtLength;
				m_nWidth       = pSourceField->m_nWidth;
				m_sLabel       = pSourceField->m_sLabel;
				m_sName        = pSourceField->m_sName;
				m_byteMeasure  = pSourceField->m_byteMeasure;	  //### 2002-9-25 Eric
				
			//	m_ValueLabel.m_pField = this;
				m_ValueLabel.Clone(&pSourceField->m_ValueLabel);
				m_ShowCharacteristic.Clone(&pSourceField->m_ShowCharacteristic);
				m_pMissing->Clone(pSourceField->m_pMissing); //### 2002-9-25 Eric
			case fBoolean:
				m_nWidth = 2;
				break;
			case fCurrency:
				m_nDecimals    = pSourceField->m_nDecimals;
				m_nWidth       = pSourceField->m_nWidth;
				break;

			case fDate:
				m_nWidth = 12;
				break;
			}
		};
		break;
	case fBoolean:
		{
			m_nWidth = 2;
			m_nDecimals    = 0;//pSourceField->m_nDecimals;
			switch(GetFieldType())
			{
			case fString:
				break;
			case fInt:
				m_nWidth = 8;
				m_nDecimals    = 0;//pSourceField->m_nDecimals;
				break;
			case fDouble:
				m_nWidth = 8;
				m_nDecimals    = 2;//pSourceField->m_nDecimals;
				break;
			case fBoolean:
				m_bIsEnum      = pSourceField->m_bIsEnum;
				m_byteEnumData = pSourceField->m_byteEnumData;
				m_byteEnumType = pSourceField->m_byteEnumType;
				m_nDecimals    = pSourceField->m_nDecimals;
				m_nExtCount    = pSourceField->m_nExtCount;
				m_nExtLength   = pSourceField->m_nExtLength;
				m_nWidth       = pSourceField->m_nWidth;
				m_sLabel       = pSourceField->m_sLabel;
				m_sName        = pSourceField->m_sName;
				m_byteMeasure  = pSourceField->m_byteMeasure;	  //### 2002-9-25 Eric
				
			//	m_ValueLabel.m_pField = this;
				m_ValueLabel.Clone(&pSourceField->m_ValueLabel);
				m_ShowCharacteristic.Clone(&pSourceField->m_ShowCharacteristic);
				m_pMissing->Clone(pSourceField->m_pMissing); //### 2002-9-25 Eric
			case fCurrency:
				m_nWidth = 8;
				m_nDecimals    = 2;//pSourceField->m_nDecimals;
				break;

			case fDate:
				m_nWidth = 12;
				break;
			}
		};
		break;
	case fCurrency:
		{
			switch(GetFieldType())
			{
			case fString:
				break;
			case fInt:
				break;
			case fDouble:
				m_nDecimals    = pSourceField->m_nDecimals;
				m_nWidth       = pSourceField->m_nWidth;
				break;
			case fBoolean:
				break;
			case fCurrency:
				m_bIsEnum      = pSourceField->m_bIsEnum;
				m_byteEnumData = pSourceField->m_byteEnumData;
				m_byteEnumType = pSourceField->m_byteEnumType;
				m_nDecimals    = pSourceField->m_nDecimals;
				m_nExtCount    = pSourceField->m_nExtCount;
				m_nExtLength   = pSourceField->m_nExtLength;
				m_nWidth       = pSourceField->m_nWidth;
				m_sLabel       = pSourceField->m_sLabel;
				m_sName        = pSourceField->m_sName;
				m_byteMeasure  = pSourceField->m_byteMeasure;	  //### 2002-9-25 Eric
				
			//	m_ValueLabel.m_pField = this;
				m_ValueLabel.Clone(&pSourceField->m_ValueLabel);
				m_ShowCharacteristic.Clone(&pSourceField->m_ShowCharacteristic);
				m_pMissing->Clone(pSourceField->m_pMissing); //### 2002-9-25 Eric
			case fDate:
				m_nWidth = 12;
				break;
			}
		};
		break;

	case fDate:
		{
			switch(GetFieldType())
			{
			case fString:
				break;
			case fInt:
				break;
			case fDouble:
				break;
			case fBoolean:
				break;
			case fCurrency:
				break;
			case fDate:
				m_bIsEnum      = pSourceField->m_bIsEnum;
				m_byteEnumData = pSourceField->m_byteEnumData;
				m_byteEnumType = pSourceField->m_byteEnumType;
				m_nDecimals    = pSourceField->m_nDecimals;
				m_nExtCount    = pSourceField->m_nExtCount;
				m_nExtLength   = pSourceField->m_nExtLength;
				m_nWidth       = pSourceField->m_nWidth;
				m_sLabel       = pSourceField->m_sLabel;
				m_sName        = pSourceField->m_sName;
				m_byteMeasure  = pSourceField->m_byteMeasure;	  //### 2002-9-25 Eric
				
			//	m_ValueLabel.m_pField = this;
				m_ValueLabel.Clone(&pSourceField->m_ValueLabel);
				m_ShowCharacteristic.Clone(&pSourceField->m_ShowCharacteristic);
				m_pMissing->Clone(pSourceField->m_pMissing); //### 2002-9-25 Eric
				break;
			}
		};
	}
	return;
}

////zjw add 2002-12-19
//Ϊ�ֶ�����Ƿ�ɸѡ����
//DEL void CField::SetFieldFiltrComma(int nComma)
//DEL {
//DEL    m_nComma = nComma;
//DEL }
//��ñ�־
//DEL int CField::GetFieldFiltrComma()
//DEL {
//DEL  return m_nComma;
//DEL }
/************************************************************************************
����˵��:
	���������ֵ�ڵ�ǰֵ��ǩ�б��в��Ҷ�Ӧ��ֵ����,û���ҵ�,�򷵻�false;
	�ҵ���true,������strLabel�������Ӧ�Ľ���
��ϸ����:

�������:
[in]: nValue ��Ӧ��ֵ
 
[out]:strLabel �������

��������:bool.

����://#_S 2003-4-1 $ 13 zlq
************************************************************************************/
bool CField::GetValueVL(int nValue,CTString &strLabel)
{
	int nCount = m_ValueLabel.GetCount();
	for (int i=0; i<nCount ;i++)
	{
		m_ValueLabel.SetCurPos(i);
		if (m_ValueLabel.GetAsInteger()==nValue)
		{//�ҵ�
			strLabel = m_ValueLabel.GetExplainString();
			return true;
		}
	}
	return false;
}
/************************************************************************************
����˵��:
	���������ֵ�ڵ�ǰֵ��ǩ�б��в��Ҷ�Ӧ��ֵ����,û���ҵ�,�򷵻�false;
	�ҵ���true,������strLabel�������Ӧ�Ľ���
��ϸ����:

�������:
[in]: dValue ��Ӧ��ֵ
 
[out]:strLabel �������

��������:bool.

����://#_S 2003-4-1 $ 13 zlq
************************************************************************************/
bool CField::GetValueVL(double dValue,CTString &strLabel)
{
	int nCount = m_ValueLabel.GetCount();
	for (int i=0; i<nCount ;i++)
	{
		m_ValueLabel.SetCurPos(i);
		if (fabs(m_ValueLabel.GetAsDouble()-dValue) < DoubleZero)
		{//�ҵ�
			strLabel = m_ValueLabel.GetExplainString();
			return true;
		}
	}
	return false;
}
/************************************************************************************
����˵��:
	���������ֵ�ڵ�ǰֵ��ǩ�б��в��Ҷ�Ӧ��ֵ����,û���ҵ�,�򷵻�false;
	�ҵ���true,������strLabel�������Ӧ�Ľ���
��ϸ����:

�������:
[in]: strValue ��Ӧ��ֵ
 
[out]:strLabel �������

��������:bool.

����://#_S 2003-4-1 $ 13 zlq
************************************************************************************/
bool CField::GetValueVL(CTString strValue,CTString &strLabel)
{
	int nCount = m_ValueLabel.GetCount();
	for (int i=0; i<nCount ;i++)
	{
		m_ValueLabel.SetCurPos(i);
		CTString str = m_ValueLabel.GetAsString();
		if (str ==strValue)
		{//�ҵ�
			strLabel = m_ValueLabel.GetAsString();
//			strLabel = m_ValueLabel.GetExplainString();
			return true;
		}
	}
	return false;
}

/************************************************************************************
����˵��:
	���������ֵ�ڵ�ǰֵ��ǩ�б��в��Ҷ�Ӧ��ֵ����,û���ҵ�,�򷵻�false;
	�ҵ���true,������strLabel�������Ӧ�Ľ���
��ϸ����:

�������:
[in]: bValue ��Ӧ��ֵ
 
[out]:strLabel �������

��������:bool.

����://#_S 2003-4-1 $ 13 zlq
************************************************************************************/
bool CField::GetValueVL(bool bValue,CTString &strLabel)
{
	int nCount = m_ValueLabel.GetCount();
	for (int i=0; i<nCount ;i++)
	{
		m_ValueLabel.SetCurPos(i);
		if (m_ValueLabel.GetAsBool()==bValue)
		{//�ҵ�
			strLabel = m_ValueLabel.GetExplainString();
			return true;
		}
	}
	return false;
}

/************************************************************************************
����˵��:
	���������ֵ�ڵ�ǰֵ��ǩ�б��в��Ҷ�Ӧ��ֵ����,û���ҵ�,�򷵻�false;
	�ҵ���true,������strLabel�������Ӧ�Ľ���
��ϸ����:

�������:
[in]: ftValue ��Ӧ��ֵ
 
[out]:strLabel �������

��������:bool.

����://#_S 2003-4-1 $ 13 zlq
************************************************************************************/
bool CField::GetValueVL(FILETIME ftValue,CTString &strLabel)
{
	int nCount = m_ValueLabel.GetCount();
	for (int i=0; i<nCount ;i++)
	{
		m_ValueLabel.SetCurPos(i);
		FILETIME ft = m_ValueLabel.GetAsDateTime();
		if (memcmp(&ft,&ftValue,sizeof(FILETIME))==0)
		{//�ҵ�
			strLabel = m_ValueLabel.GetExplainString();
			return true;
		}
	}
	return false;
}

void CField::pg_Add(void*pDestArrar)
{
	if (GetFieldType() == fString)
		((PF_STRING)pDestArrar)->Add(*(FieldString*)m_pTempData);
	else
		((PF_NUM)pDestArrar)->Add(*(FieldNUM*)m_pTempData);
}
void CField::pg_Add(void*pDestArrar,int nCount)
{
	if (GetFieldType() == fString)
	{
		for (int i=0;i<nCount ;i++)
			((PF_STRING)pDestArrar)->Add(*(FieldString*)m_pTempData);
	}
	else
	{
		for (int i=0;i<nCount ;i++)
			((PF_NUM)pDestArrar)->Add(*(FieldNUM*)m_pTempData);
	}
}

void CField::pg_InsertAt(void*pDestArrar,int nIndex)
{
	if (GetFieldType() == fString)
		((PF_STRING)pDestArrar)->InsertAt(nIndex,*(FieldString*)m_pTempData);
	else
		((PF_NUM)pDestArrar)->InsertAt(nIndex,*(FieldNUM*)m_pTempData);
}

void CField::pg_RemoveAt(void *pDestArrar,int nIndex)
{
	if (GetFieldType() == fString)
		((PF_STRING)pDestArrar)->RemoveAt(nIndex);
	else
		((PF_NUM)pDestArrar)->RemoveAt(nIndex);
}

void CField::pg_RemoveAll(void *pDestArrar)
{
	if (GetFieldType() == fString)
		((PF_STRING)pDestArrar)->RemoveAll();
	else
		((PF_NUM)pDestArrar)->RemoveAll();
}

void CField::pg_Delete(void *pDestArrar)
{
	if (GetFieldType() == fString)
		delete ((PF_STRING)pDestArrar);
	else
		delete ((PF_NUM)pDestArrar);
}

void* CField::pg_New()
{
	if (GetFieldType() == fString)
	{
		PF_STRING pResult = new F_STRING;
		pResult->SetSize(0,IncreaseSize);
		return pResult;
	}
	else
	{
		PF_NUM pResult = new F_NUM;
		pResult->SetSize(0,IncreaseSize);
		return pResult;
	}
}

LPBYTE CField::pg_GetAt(void*pDestArrar,int nIndex)
{
	if (GetFieldType() == fString)
		return(LPBYTE) &((*(PF_STRING)pDestArrar)[nIndex]);
	else
		return(LPBYTE) &((*(PF_NUM)pDestArrar)[nIndex]);
}


bool CField::bHaveValueLabel()
{
	int nCount = m_ValueLabel.GetCount();
	if(nCount>0)
		return true;
	else
		return false;
}

void CField::PrintValue()
{
	printf(GetAsString());
}

//�жϵ�ǰֵ�Ƿ�����ȱʧֵ����ֵҲ��ȱʧֵ
bool CField::IsMissingValue()
{
	bool bValueMissed = false;
	double dData[3], dOrgData;
	int nCount, iIndex;

	if( IsNull() )
		bValueMissed = true;
	else if( m_pMissing!=NULL )
	{
		if( m_pMissing->GetMissingType()!=mtNoMissing )
		{
			dOrgData = GetAsDouble();	
			m_pMissing->GetDoubleData( dData[0], dData[1], dData[2] );
			nCount =  m_pMissing->GetCount();

			if( m_pMissing->GetMissingType()==mtDiscrete )
			{
				for( iIndex=0; iIndex<nCount; iIndex++ )
				{
					if( dOrgData==dData[iIndex] )
					{
						bValueMissed = true;
						break;
					}
				}
			}
			else if( m_pMissing->GetMissingType()==msRange )
			{
				if( dOrgData>=dData[0] && dOrgData<=dData[1] )
				 	bValueMissed = true;
				else if( nCount==3 && dOrgData==dData[2] )
					bValueMissed = true;
			}
		}
	}

	return bValueMissed;
}
