// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "cfield.h"
#include "cfields.h"
#include "cdataset.h"
#include "cvaluelabel.h"
#include  <math.h>
#include "cpages.h"

CValueLabel::CValueLabel()
{
	m_bIsNum = true;
}

CValueLabel::~CValueLabel()
{
	POSITION posData = m_DataList.GetHeadPosition();	
	LPBYTE pOldValue = m_pField->m_pValue;
	while(posData)
	{
		m_pField->m_pValue = (LPBYTE)m_DataList.GetNext( posData );

		if (m_bIsNum ==false)
		{
			FieldString* pStr = (FieldString*)m_pField->m_pValue ;
			delete pStr;
		}
		else
		{
			if( m_pField->m_pValue )
				delete[] m_pField->m_pValue;

		}
		//m_pField->RealDeleteData();	
	}
	m_DataList.RemoveAll();
	m_strLabelList.RemoveAll();
	m_pField->m_pValue = pOldValue;
}

/************************************************************************************
�������ƣ�int CValueLabel::LoadFromBuffer(LPBYTE lpData, int count)
����˵�����ѻ��������ļ���ʽ������ֵ��ǩ���ݽ��ͺ�װ��������
          1.ֵװ��m_DataList
		  2.ֵ������labelװ��m_strLabelList
	ע:�����������ʱ,ָ����ָ����ö�ٵ������ֽ�,Ҫװ���Ϊ��ö������,��ö��ֵ��������

��ϸ���ͣ�

���������
    [in]:1.lpDataָ��ֵ��ǩ�����ֽ� 2.count���ļ���ʽ������ֵ��ǩ���ݳ���
	[out]:���ش�����ֽ���
�������ͣ�int
    
������

�޸�: 
************************************************************************************/
int CValueLabel::LoadFromBuffer(LPBYTE lpData, int count)
{
	if (m_pField->GetFieldType() == fString)
		m_bIsNum = false;
	else
		m_bIsNum = true;

	//����m_pValueָ��
	LPBYTE oldValue = m_pField->m_pValue;

    LPBYTE lpDataTemp = lpData;
	//�����һ�ֽ�(��ö�ٸ���)
	//#_S 2003-3-31 $ 16 zlq ��չ�ֶ�������
	//�õ��ֶ���
	int ItemCount = 0;//��ǩ����

	{//���°汾 4���ֽ�
		int *pInt = (int*)lpDataTemp;
		ItemCount = *pInt;
		lpDataTemp = lpDataTemp+4;
	}
	

	for (int i=0;i<ItemCount;i++)
	{
		//LPBYTE lpDataTemp2 = lpDataTemp + 1;
		m_pField->RealNewData();   //����һ���µ�����ָ��(m_pValue);
		m_pField->LoadDataFromBuffer(lpDataTemp); //�ӻ������е�������lpDataTemp�Զ�����
		m_DataList.AddTail(m_pField->m_pValue);     //Ȼ���ɵ����������������

		//���Ͳ��ִ���: ���� + ����
		int  nCntChar = (int)*lpDataTemp;//��������ַ����ĳ���
		lpDataTemp++;
		//lpDataTemp = ++lpDataTemp2;
		CTString str;

		char *pbb = new char[nCntChar+1];
		memset(pbb,0,nCntChar+1);
		memcpy(pbb,lpDataTemp,nCntChar);
		str = pbb;
		delete[] pbb;
		/*
 		LPTSTR pTemp = str.GetBuffer(nCntChar+1);
		CopyMemory(pTemp,lpDataTemp,nCntChar);
		pTemp[nCntChar] = '\0';
		str.ReleaseBuffer( );
		*/

		m_strLabelList.Add(str);
		lpDataTemp=lpDataTemp + nCntChar ;
	}

    //assert(lpDataTemp-lpData == count);        //���Կ��Ƿ�������ֽ����봫���ֽ�����һ��
    //�ָ�ԭ����m_pValue;
	m_pField->m_pValue = oldValue;

	return count;      //���ش�������ֽ���
}

//##ModelId=3CF5936302DB
/************************************************************************************
�������ƣ�int CValueLabel::GetCount()
����˵����ȡ��ֵ�ĸ���

��ϸ���ͣ�

���������
    [in]:��
	[out]:����ֵ�ĸ���
�������ͣ�int
    
������

�޸�: 
************************************************************************************/
int CValueLabel::GetCount()
{
	return m_DataList.GetCount();
}

//##ModelId=3CFB07A6024B
/************************************************************************************
�������ƣ�int CValueLabel::SaveToBuffer(LPBYTE lpData)
����˵����Ϊ����LoadFromBuffer������̣�����ֵΪ������ֽ���

��ϸ���ͣ�

���������
    [in]: lpData ���ļ���ʽ����ֵ��ǩ�Ļ�����ָ��
	[out]:���ر�����ֽ���
�������ͣ�int
    
������

�޸�: 
************************************************************************************/
int CValueLabel::SaveToBuffer(LPBYTE lpData)
{
	//����m_pValueָ��
	LPBYTE oldValue = m_pField->m_pValue;

	LPBYTE lpDataTemp = lpData;
	int nItem = m_DataList.GetCount();
	
	//д���һ���ֽ�(��Ŀ��);
	//�����һ�ֽ�(��ö�ٸ���)
	//#_S 2003-3-31 $ 16 zlq ��չ�ֶ�������
	//�õ��ֶ���
	{//���°汾����
		*((int*)lpDataTemp )= nItem;
		lpDataTemp = lpDataTemp+4;	
	}

    //int nTypeSize = m_pField->GetTypeSize();

	POSITION posData;
	posData = m_DataList.GetHeadPosition();
 	int posStr = m_strLabelList.GetSize();
	int nPos = 0;
	while(posData !=NULL &&  nPos < posStr)
	{
		//LPBYTE lpDataTemp2 = lpDataTemp + 1;
		//��������ȡ��Valueָ��
		m_pField->m_pValue = (LPBYTE)m_DataList.GetNext( posData );
		
		//����ValueֵΪ�ļ��еĴ洢��ʽ
		m_pField -> SaveDataToBuffer( lpDataTemp);
		//lpDataTemp = lpDataTemp2;

		CTString str = m_strLabelList.GetAt(nPos);
		BYTE nStrSize = (BYTE)str.GetLength();
		*lpDataTemp =nStrSize;       //д�����Value���ַ����ĳ���
		lpDataTemp++;
		//�������Value���ַ���
		memcpy(lpDataTemp, str, nStrSize);
		lpDataTemp += nStrSize;
		nPos++;
	}

	//�ָ�ԭ����m_pValueָ��
    m_pField->m_pValue=oldValue;
	
	return (lpDataTemp - lpData);
}
/************************************************************************************
�������ƣ�int CValueLabel::QueryLength()
����˵����ȡ��ֵ��ǩ����ʱ�ĳ���

��ϸ���ͣ�

���������
    [in]:
	[out]:����ֵ��ǩ����ʱ�ĳ���
�������ͣ�int
    
������

�޸�: 
************************************************************************************/
int CValueLabel::QueryLength()
{
	int nLength =0;
	//�õ���Ŀ��
	int nItem = m_strLabelList.GetSize();
	//�ۼ���Ŀ��ռ�õ�һ���ֽ�
	
	//#_S 2003-3-31 $ 16 zlq 
	nLength = nLength+4;

	POSITION posData;
	posData = m_DataList.GetHeadPosition();
 	int posStr = m_strLabelList.GetSize();
	LPBYTE pOldValue = m_pField->m_pValue;
	int nPos = 0;
	while(posData != NULL && nPos < posStr)
	{
		//�ۼ�valuelabel����
		m_pField->m_pValue = (LPBYTE)m_DataList.GetNext( posData );
		nLength += m_pField->QuerySaveLength();
		//�ۼ��Ͻ����ַ������ȼ����泤�ȵ�һ���ֽ�
		CTString str = m_strLabelList.GetAt(nPos);
		nLength += str.GetLength()+1;
		nPos++;
	}
	m_pField->m_pValue = pOldValue;
	return nLength;
}
/************************************************************************************
�������ƣ�void CValueLabel::Clone(CValueLabel *pValueLabel)
����˵�����Ѵ����ֵ��ǩ�Ļ�����Ϣ��¡����ǰ��ֵ��ǩ��

��ϸ���ͣ�

���������
    [in]:pValueLabel Ϊ��¡��pValueLabelָ��
	[out]:
�������ͣ�void
    
������

�޸�: 
************************************************************************************/

void CValueLabel::Clone(CValueLabel *pValueLabel)
{
	if (m_pField->GetFieldType() == fString)
		m_bIsNum = false;
	else
		m_bIsNum = true;

	LPBYTE pOldValue = m_pField->m_pValue;

	POSITION posData = m_DataList.GetHeadPosition();	
	
	while(posData)
	{
		m_pField->m_pValue = (LPBYTE)m_DataList.GetNext( posData );
		m_pField->RealDeleteData();	
	}
	m_DataList.RemoveAll();
	m_strLabelList.RemoveAll();

	posData = pValueLabel->m_DataList.GetHeadPosition();
    int posStr  = pValueLabel->m_strLabelList.GetSize();
    int nPos = 0;
	while(posData != NULL && nPos < posStr)
	{
		//��������ȡ��Valueָ��
		LPBYTE pData = (LPBYTE)pValueLabel->m_DataList.GetNext( posData );
		//LPBYTE pByte = m_pField->SetData();
		m_pField->RealNewData();//###_Mod 2002-9-4 zlq
		m_pField->GetData(pData);
		//delete [] pByte;
		
		m_DataList.AddTail(m_pField->m_pValue);

        //ȡ��label�ַ���Ȼ������ڵ�ǰ��������Ӧ��������
		CTString str = pValueLabel->m_strLabelList.GetAt(nPos);
		m_strLabelList.Add(str);
		nPos++;
	}
	//assert(posData == NULL && posStr == NULL);
	m_pField->m_pValue = pOldValue;
}

//���㿪ʼ
void CValueLabel::SetCurPos(int nIndex)
{
	m_nCurPos = nIndex;
}

CTString CValueLabel::GetExplainString()
{
// 	POSITION pos = m_strLabelList.FindIndex(m_nCurPos);
	return m_strLabelList.GetAt(m_nCurPos);
}

void CValueLabel::SetExplainString(CTString str)
{
//	POSITION pos = m_strLabelList.FindIndex(m_nCurPos);
	m_strLabelList.InsertAt(m_nCurPos,str);
	m_strLabelList.GetAt(m_nCurPos);
	//assert(pos);

	m_strLabelList.RemoveAt(m_nCurPos);
}

bool CValueLabel::GetAsBool()
{
	LPBYTE lpByte = m_pField->m_pValue;

 	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);

	bool bRetVal = m_pField->GetAsBool();

	m_pField->m_pValue = lpByte;

	if (bRetVal)
		return true;
	else
		return false;
}

FieldCurrency CValueLabel::GetAsCurrency()
{
	LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);

	FieldCurrency RetVal = m_pField->GetAsCurrency();

	m_pField->m_pValue = lpByte;

	return RetVal;	
}

FILETIME CValueLabel::GetAsDateTime()
{
	LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);

	FILETIME pRetVal = m_pField->GetAsDateTime();

	m_pField->m_pValue = lpByte;

	return pRetVal;	
}

CTString  CValueLabel::GetAsString()
{
    LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);

	CTString strRetVal = m_pField->GetAsString();

	m_pField->m_pValue = lpByte;

	return strRetVal;	
}

double CValueLabel::GetAsDouble()
{
	LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);

	double dRetVal = m_pField->GetAsDouble();

	m_pField->m_pValue = lpByte;

	return dRetVal;	
}

int CValueLabel::GetAsInteger()
{
    LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);

	int iRetVal = m_pField->GetAsInteger();

	m_pField->m_pValue = lpByte;

	return iRetVal;	
}

bool CValueLabel::SetAsDouble(double Value)
{
	LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);
	m_pField->SetAsDouble(Value);

	m_pField->m_pValue = lpByte;

	return true;
}

bool CValueLabel::SetAsInteger(int Value)
{
	LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);
	m_pField->SetAsInteger(Value);

	m_pField->m_pValue = lpByte;

	return true;
}

bool CValueLabel::SetAsString(CTString Value)
{
	LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);
	m_pField->SetAsString(Value);

	m_pField->m_pValue = lpByte;

	return true;
}

bool CValueLabel::SetAsDateTime(FILETIME* Value)
{
	LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);
	m_pField->SetAsDateTime(Value);

	m_pField->m_pValue = lpByte;

	return true;
}

bool CValueLabel::SetAsCurrency(FieldCurrency Value)
{	
	LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);
	m_pField->SetAsCurrency(Value);

	m_pField->m_pValue = lpByte;

	return true;
}

bool CValueLabel::SetAsBool(bool Value)
{
	LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);
	m_pField->SetAsBool(Value);

	m_pField->m_pValue = lpByte;

	return true;
}


//### 2002-11-11 Eric ======>
/************************************************************************************
�������ƣ�

����˵����

��ϸ���ͣ�

���������
	[in,out]
�������ͣ�

������
	Eric 2002-11-11
�޸�:

************************************************************************************/
bool CValueLabel::AddValue(CTString Value,CTString strExplain)
{
	if (m_pField->GetFieldType() == fString)
		m_bIsNum = false;
	else
		m_bIsNum = true;

	LPBYTE lpByte = m_pField->m_pValue;

	m_pField->RealNewData();
	CPage*pOldDataSet = m_pField->m_pFields->m_pDataSet;
	m_pField->m_pFields->m_pDataSet = NULL;

	if(m_pField->SetAsString(Value)==false ||IsValueExist(m_pField->m_pValue))
	{
		m_pField->RealDeleteData();
		m_pField->m_pValue = lpByte;
		m_pField->m_pFields->m_pDataSet = pOldDataSet;
		return false;
	}

	m_pField->m_pFields->m_pDataSet = pOldDataSet;
	m_DataList.AddTail(m_pField->m_pValue);
	m_strLabelList.Add(strExplain);

	m_pField->m_pValue = lpByte;
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
	Eric 2002-11-11
�޸�:

************************************************************************************/
bool CValueLabel::AddValue(int Value,CTString strExplain)
{
	if (m_pField->GetFieldType() == fString)
		m_bIsNum = false;
	else
		m_bIsNum = true;

	LPBYTE lpByte = m_pField->m_pValue;

	m_pField->RealNewData();
	CPage*pOldDataSet = m_pField->m_pFields->m_pDataSet;
	m_pField->m_pFields->m_pDataSet = NULL;

	if(m_pField->SetAsInteger(Value)==false ||IsValueExist(m_pField->m_pValue))
	{
		m_pField->RealDeleteData();
		m_pField->m_pValue = lpByte;
		m_pField->m_pFields->m_pDataSet = pOldDataSet;
		return false;
	}

	m_pField->m_pFields->m_pDataSet = pOldDataSet;
	m_DataList.AddTail(m_pField->m_pValue);
	m_strLabelList.Add(strExplain);

	m_pField->m_pValue = lpByte;
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
	Eric 2002-11-11
�޸�:

************************************************************************************/
bool CValueLabel::AddValue(double Value,CTString strExplain)
{
	if (m_pField->GetFieldType() == fString)
		m_bIsNum = false;
	else
		m_bIsNum = true;

	LPBYTE lpByte = m_pField->m_pValue;

	m_pField->RealNewData();
	CPage*pOldDataSet = m_pField->m_pFields->m_pDataSet;
	m_pField->m_pFields->m_pDataSet = NULL;

	if(m_pField->SetAsDouble(Value)==false ||IsValueExist(m_pField->m_pValue))
	{
		m_pField->RealDeleteData();
		m_pField->m_pValue = lpByte;
		m_pField->m_pFields->m_pDataSet = pOldDataSet;
		return false;
	}
	m_pField->m_pFields->m_pDataSet = pOldDataSet;
	m_DataList.AddTail(m_pField->m_pValue);
	m_strLabelList.Add(strExplain);

	m_pField->m_pValue = lpByte;
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
	Eric 2002-11-11
�޸�:

************************************************************************************/
bool CValueLabel::AddValue(FILETIME* Value,CTString strExplain)
{
	if (m_pField->GetFieldType() == fString)
		m_bIsNum = false;
	else
		m_bIsNum = true;

	LPBYTE lpByte = m_pField->m_pValue;

	m_pField->RealNewData();
	CPage*pOldDataSet = m_pField->m_pFields->m_pDataSet;
	m_pField->m_pFields->m_pDataSet = NULL;

	if(m_pField->SetAsDateTime(Value)==false ||IsValueExist(m_pField->m_pValue))
	{
		m_pField->RealDeleteData();
		m_pField->m_pValue = lpByte;
		m_pField->m_pFields->m_pDataSet = pOldDataSet;
		return false;
	}
	m_pField->m_pFields->m_pDataSet = pOldDataSet;
	m_DataList.AddTail(m_pField->m_pValue);
	m_strLabelList.Add(strExplain);

	m_pField->m_pValue = lpByte;
	return true;
}

/************************************************************************************
�������ƣ�

����˵����

��ϸ���ͣ�

���������
	[in,out]
	nIndex  ���������λ��,��0��ʼ
�������ͣ�

������
	Eric 2002-11-11
�޸�:

************************************************************************************/
bool CValueLabel::Delete(int nIndex)
{
	if(nIndex<0 || nIndex> m_DataList.GetCount())
		return false;
	LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(nIndex);
	LPBYTE lpTemp = (LPBYTE)(m_DataList.GetAt(pos));
	delete lpTemp;
	m_DataList.RemoveAt(pos);
// 	pos = m_strLabelList.FindIndex(nIndex);
	m_strLabelList.RemoveAt(nIndex);

	m_pField->m_pValue = lpByte;
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
	Eric 2002-11-11
	�޸�:
	
************************************************************************************/

bool CValueLabel::IsValueExist(LPBYTE lpData)
{				 
	double		 fValue,fTemp;
	CTString	     strValue,strTemp;
	int		 	 nValue;
	COleDateTime tValue,tTemp;
	
	LPBYTE lpByte = m_pField->m_pValue;
	
	m_pField->m_pValue = lpData;
	switch (m_pField->GetFieldType())
	{//#_�޸� 2002-12-2 $ 16:01:13 zlq ===temp 
	case fDouble:
		fValue = m_pField->GetAsDouble();
		break;
	case fString :
		strValue = m_pField->GetAsString();
		break;
	case fInt :
		nValue = m_pField->GetAsInteger();
		break;
	case fDate :
/*===time  		tValue = COleDateTime(m_pField->GetAsDateTime());   */
		break;
	case fCurrency :
	case fBoolean :
	default:
		break;
	}
	
	
	
	
	
	

	POSITION pos = m_DataList.GetHeadPosition();
	for(int i = 0;i<m_DataList.GetCount();i++)
	{
		m_pField->m_pValue = (LPBYTE)m_DataList.GetNext(pos);

		switch (m_pField->GetFieldType())
		{
		case fString :
			strTemp = m_pField->GetAsString();
			if(strTemp == strValue)
			{
				m_pField->m_pValue = lpByte;
				return true;
			}
			break;
		case fInt :
			fTemp = m_pField->GetAsInteger();
			if(fTemp == nValue)
			{
				m_pField->m_pValue = lpByte;
				return true;
			}
			break;
		case fDate :
/*===time  			tTemp = COleDateTime(m_pField->GetAsDateTime());
			if(tTemp == tValue)
			{
				m_pField->m_pValue = lpByte;
				return true;
			}   */
			break;
		case fDouble:
			fTemp = m_pField->GetAsDouble();
			if(fabs(fTemp - fValue) <DoubleZero )//to be replaced ### 2002-11-11 Eric
			{
				m_pField->m_pValue = lpByte;
				return true;
			}
			break;
		case fCurrency :

		case fBoolean :
		default:
			break;
		}
	}

	m_pField->m_pValue = lpByte;

	return false;
}
//### 2002-11-11 Eric<======

//Ҫ��ӵı�ǩֵ�Ƿ���Ч
bool CValueLabel::IsValid(CTString Value)
{
	LPBYTE lpByte = m_pField->m_pValue;//����ԭ������

	m_pField->RealNewData();
	CPage*pOldDataSet = m_pField->m_pFields->m_pDataSet;
	m_pField->m_pFields->m_pDataSet = NULL;
	bool bIsValid = true;
	if(m_pField->SetAsString(Value)==false )
	{
		bIsValid = false;
	}

	m_pField->RealDeleteData();
	m_pField->m_pValue = lpByte;//�ָ�ԭ������
	m_pField->m_pFields->m_pDataSet = pOldDataSet;
	return bIsValid ;
}

void CValueLabel::SetOwner(CField *pField)
{
	m_pField = pField;

}
