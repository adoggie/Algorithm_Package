// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "ccurrencyfield.h"
#include "cpage.h"
#include "cpages.h"

CCurrencyField::CCurrencyField(CFields* pOwner)
{
	//assert( pOwner );
	m_pFields = pOwner;
	m_pDefaultValue = (LPBYTE)new FieldCurrency;//ytli Modify 2002/08/30

	m_strCurrencyType  = "$";

	if(m_pDefaultValue)	
		*(FieldNUM*)m_pDefaultValue = NULLValue;	
	m_pTempData = new FieldCurrency;
	*((FieldNUM*)m_pTempData) = NULLValue;
	m_byteMeasure=msScale;
}

CCurrencyField::~CCurrencyField()
{
	if(m_pDefaultValue)
		delete (FieldCurrency *)m_pDefaultValue;
	if(m_pTempData)
		delete (FieldCurrency *)m_pTempData;
}



//##ModelId=3D0988A90363
int CCurrencyField::GetTypeSize()
{	
	return 10;
}

CFieldType CCurrencyField::GetFieldType()
{
	return fCurrency;
}

double CCurrencyField::GetAsDouble()
{
	////assert( m_pValue && *m_pValue == fvValid );	 //ָʾ�ֽڱ���Ϊ1
	
	return *((FieldCurrency *)m_pValue);
}

int CCurrencyField::GetAsInteger()
{
	////assert( m_pValue && *m_pValue == fvValid );	 //ָʾ�ֽڱ���Ϊ1


	return (int)*((FieldCurrency *)m_pValue);
	
}

CTString CCurrencyField::GetAsString()
{
	////assert( m_pValue && *m_pValue == fvValid );	 //ָʾ�ֽڱ���Ϊ1
	
	CTString strRetValue;
	CTString strFormat ;
	//strFormat.Format("%%%d.%df",m_nWidth,m_nDecimals);
	strFormat.Format("%%%d.%df",0,m_nDecimals);
	strRetValue.Format(strFormat,*((FieldNUM *)m_pValue));
	strRetValue.TrimLeft();
	/*
	if (m_nWidth >m_nTempBufLength)//#_�޸� 2002-12-12 $ 14:04:21 zlq
		SetTempBufLength(m_nWidth);//����ռ䲻������������ʱ������	
	gcvt( ((FieldCurrency *)m_pValue)->dwValue, m_nWidth, m_pTempBuf );
	strRetValue = _T(m_pTempBuf);	
	*/
	return strRetValue;
}

bool CCurrencyField::GetAsBool()
{
	////assert( m_pValue && *m_pValue == fvValid );	 //ָʾ�ֽڱ���Ϊ1

	if( fabs(*((FieldCurrency *)m_pValue)) < DoubleZero )
		return false;
	
	return true;
}

FILETIME CCurrencyField::GetAsDateTime()
{	
	////assert( m_pValue && *m_pValue == fvValid );	 //ָʾ�ֽڱ���Ϊ1

	COleDateTime OleTime( *((FieldCurrency *)m_pValue) );

	SYSTEMTIME st;	
	FILETIME   FT;

/*===time  	memset(&st,0,sizeof(st));
	st.wDay          = OleTime.GetDay();
	st.wHour         = OleTime.GetHour();
	st.wMinute       = OleTime.GetMinute();
	st.wMonth        = OleTime.GetMonth();
	st.wSecond       = OleTime.GetSecond();
	st.wYear         = OleTime.GetYear();  

	SystemTimeToFileTime(&st, &FT); */

	return FT;
}


FieldCurrency CCurrencyField::GetAsCurrency()
{
	////assert( m_pValue && *m_pValue == fvValid );	 //ָʾ�ֽڱ���Ϊ1

	return *((FieldCurrency *)m_pValue);
}


bool CCurrencyField::SetAsBool(bool Value)
{
	if(!m_pValue)
	{		
		CField::SetAsBool(Value);
	}
	else 
	{
		*((FieldCurrency *)m_pValue) = (double)Value;
	}

	return true;
}
//���������ҷ��� ������
bool CCurrencyField::SetAsString(CTString str)
{	
	if(!m_pValue)
	{
		CField::SetAsString(str);
	}
	else if(str.GetLength()==0) 
	{
		SetNull();
		return true;
	}
	else 
	{
/*===time  		str.Remove(0x20);//�����ո�.
		if(str.Find(m_strCurrencyType)>=0)
			str = str.Right(str.GetLength()-m_strCurrencyType.GetLength());
		str.MakeLower(); //ת��ΪСд.	
		
		int iLength  = str.GetLength();				
		
		int iDecimalCnt  = 0;//С�������,���ֻ������һ��,λ�ñ�����E֮ǰ.
		int iECnt        = 0;//E�ĸ���,���ֻ������һ��(��ѧ������).
		int iOtherCnt    = 0;//��0����E,��С����,��+-�ŵ��ַ����ۼ�,����ȷ��������0ʱ�Ƿ����Ӧ����0.
		int iSignPostion = 0;//+-�ŵ�λ��,�ͷ����E�ĺ���.  
		//����:������ֻ��������λ��:�ַ����Ŀ�ͷ��E�ĺ���,E�ĺ������Ҫ������.
		
		for(int i = 0; i<iLength; i++)
		{							
			if(str.GetAt(i) == '+' ||strGetAt(i) == '-')
			{
				if(!(i == 0 || i == iSignPostion ))//���������ŵ�λ��.
					return false;
			}
			else if(strGetAt(i) == '.')//С����.
			{
				if( (iSignPostion != 0) && (i >= iSignPostion) )
					return false;

				iDecimalCnt++;
			}
			else if(strGetAt(i) == 'e')//e:Сд.
			{
				if(i==0)
					return false;

				iECnt++;
				iSignPostion = i + 1;
			}
			else if( !isdigit(strGetAt(i)) )//e:�����֣���E,��С����,��+-�ŵ��ַ����ۼ�.
			{
				iOtherCnt++;
			}
		}

		if(iSignPostion == iLength)//��ֹ��������'4e'�����.
			return false;

		if(iDecimalCnt > 1 || iECnt > 1 || iOtherCnt > 0 )
			return false;

		char*  pChar = (char*)(LPCTSTR)str;
		double fTemp = atof( pChar );

	
		*((FieldCurrency *)m_pValue) = fTemp;	   */				
	}

	return true;
}

bool CCurrencyField::SetAsCurrency(FieldCurrency Value)
{
	if(!m_pValue)
	{
		CField::SetAsCurrency(Value);
		return true;
	}

	*((FieldCurrency *)m_pValue) = Value;

	return true;
}

bool CCurrencyField::SetAsDouble(double Value)
{
	if(!m_pValue)
	{
		CField::SetAsDouble(Value);
	}
	else
	{		
		*((FieldCurrency *)m_pValue) = Value;
	}

	return true;
}

bool CCurrencyField::SetAsDateTime(FILETIME* Value)
{//����ת��.
	if(!m_pValue)
	{		
		CField::SetAsDateTime(Value);
	}
	else 
	{
/*===time  		COleDateTime OleTime( *Value );
		
		*((FieldCurrency *)m_pValue)  = OleTime.m_dt;   */
	}

	return true;
}

bool CCurrencyField::SetAsInteger(int Value)
{
	if(!m_pValue)
	{
		CField::SetAsInteger(Value);
	}
	else 
	{
		*((FieldCurrency *)m_pValue) = Value;
	}


	return true;
}

//�������ݴ�С��������ָʾλ,���ڶ��ļ��ȶ�ʱȷ����һ����λ��
//lpData���ڱ䳤ʱ���������ַ��ͣ�ָ�����ݣ�����ʱΪ��
//##ModelId=3CF42CC30261
int CCurrencyField::GetDataLength(LPBYTE lpData)
{
	return 10;
}




//����lpDataָ������ݣ����Ƹ�����ֵ�����ֶΣ�
//��������ͬ�����ݸ�ʽ�����û�����edit��cancel����ʱ�ָ�ԭֵ

bool CCurrencyField::GetData(void*pDestArrar,int nIndex)
{
	if( !(m_pValue ) )
		return false;
	*(FieldCurrency*)m_pValue = (*(PF_NUM)pDestArrar)[nIndex];
	return true;
}
bool CCurrencyField::GetData(void* lpData)
{
	if( !(m_pValue ) )
		return false;
	*(FieldCurrency*)m_pValue = *(FieldCurrency*)lpData;
	return true;
}

//���ⲿ�û����������(�ַ�����ʽ)���浽�ֶ���,����ɹ�.�򷵻�true
bool CCurrencyField::GetFromDisplay(CTString str)
{
	if(!m_pValue)
		return false;
	
	if(!SetAsString(str))
		return false;		

	return true;
}



//����ǰ��������Ϊȱʡֵ,����m_pDefaultValue��ָ������ݿ�����m_pValue��
void CCurrencyField::SetDefaultValue()
{
	*((FieldCurrency *)m_pValue) = *((FieldCurrency *)m_pDefaultValue);

}

//�õ�ǰ������������Ϊȱʡֵ,����m_pValue��ָ������ݿ�����m_pDefaultValue�У�ԭ������û������
bool CCurrencyField::CopyToDefaultValue()
{
	*((FieldCurrency *)m_pDefaultValue) = *((FieldCurrency *)m_pValue);

	return true;
}

//���û�������
void CCurrencyField::SetCurrencyType(CTString strType)
{
	if(strType.GetLength()>255)
		return;
	m_strCurrencyType = strType;
}

//ȡ�û�������
CTString CCurrencyField::GetCurrencyType()
{
	return m_strCurrencyType;
}


/************************************************************************************
�������ƣ�
void CCurrencyField::LoadExt(LPBYTE lpData, int count)
����˵������Buffer�ж�ȡ�ֶε���չ����Ϣ.

��ϸ���ͣ�1.protected����,��LoadFromBuffer����.       

���������
[in]: 1.lpData:��չ������ʼ�ֽڵ�ַ. 
      2.count:��չ���ĳ���.
  
[out]:��. 

�������ͣ�void

������YTLI 2002/12/17

�޸�: 
************************************************************************************/
void CCurrencyField::LoadExt(LPBYTE lpData, int count)
{	
	if(!lpData || count <= 0 )
		return;

	//��ʱ lpData ָ�����ݿ�1�ı�־λ
	int iByteCnt  = 0;        //��չ������
	int iBlockCnt = 0;        //��ĸ���
	m_bIsEnum = false;        //��ʼ��ΪFALSE;	
	
	while(iByteCnt < count && iBlockCnt < m_nExtCount)
	{
		if((int)*lpData == Ext_Enum )
		{
			{
				lpData += Ext_TitleLength110; 
				iByteCnt += Ext_TitleLength110 +2;
			}

			m_bIsEnum      = true;		
			m_byteEnumType = *(lpData);

			lpData++;         //ָ��ö��ֵ

			m_byteEnumData = *(lpData);          	
			
			lpData++;         //lpDataָ����һ���ݿ�
		
			iBlockCnt++;
			
		}
		else if((int)*lpData == Ext_ValueLabel )
		{//ֵ��ǩ
			lpData += 1;   
			int wExtLength = 0;
			{
				wExtLength = *(int*)lpData;
				lpData += sizeof(int);      //ָ����ö�����͵�����			
				iByteCnt += (Ext_TitleLength110 + wExtLength);
			}

			m_ValueLabel.LoadFromBuffer(lpData,wExtLength);
			lpData += wExtLength;                //lpDataָ����һ���ݿ�
		
			iBlockCnt++;
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
            
			iBlockCnt++;
			
		}	
		else if((*lpData) == 0x04 )//��������.
		{
			lpData += 1;      //ָ���������,һ���ֽ�.
			WORD wLength = *(WORD*)lpData;//�����ֽڵĳ���.
			lpData += 2; 
			LPBYTE pCharTemp = new BYTE[wLength+1];
			
			if( pCharTemp == NULL )
				return ;  
			memcpy( pCharTemp, lpData, wLength );
			pCharTemp[wLength] = '\0';
			m_strCurrencyType = (char*)_T(pCharTemp );
			delete []pCharTemp;
			lpData   += wLength;
			iBlockCnt++;
			iByteCnt += 3+wLength;   
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
void CCurrencyField::SaveExt(LPBYTE lpData, int count)
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

void CCurrencyField::SaveExt(LPBYTE lpData, int count)
{
	if(!lpData || count <= 0 )
		return;

	int iByteCnt  = 0;        //��չ������;

	if( m_bIsEnum == true && iByteCnt < count )
	{
		*lpData = Ext_Enum;//ö��
		lpData += 1;

		//���볤��ֵ2
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
	if( m_ValueLabel.m_strLabelList.GetSize()!= 0 && iByteCnt < count )
	{			
		*lpData = Ext_ValueLabel;//ֵ��ǩ
		lpData += 1;

		//���볤��ֵ
		int wLength = m_ValueLabel.QueryLength();
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
		{
			memcpy(lpData,&wLength,sizeof(int));
			lpData += sizeof(int);	
			iByteCnt += (Ext_TitleLength110 + wLength);
		}

		//������ʾ��������
		lpData += m_ShowCharacteristic.saveShowCharacter(lpData);	

	}
	if(iByteCnt < count)
	{
		*lpData = 0x04;
		lpData += 1;
		*(WORD*)lpData = m_strCurrencyType.GetLength();
		lpData += 2;               
		for(int j = 0; j<m_strCurrencyType.GetLength(); j++)
		{
			*lpData = m_strCurrencyType.GetAt(j);
			lpData += 1;
		}
	}
	//### 2002-9-25 Eric==>
	*lpData = m_byteMeasure;
	lpData ++;
	m_pMissing->SaveToBuffer(lpData);
	//### 2002-9-25 Eric<==
}

/************************************************************************************
�������ƣ�
void CCurrencyField::Clone(CField* pField)
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
void CCurrencyField::Clone(CField* pField)
{	
	if(!pField)
		return ;

	m_bIsEnum               = pField->m_bIsEnum;
	m_byteEnumData          = pField->m_byteEnumData;
	m_byteEnumType          = pField->m_byteEnumType;
	m_nDecimals             = pField->m_nDecimals;
	m_nExtCount             = pField->m_nExtCount;
	m_nExtLength            = pField->m_nExtLength;
	m_nWidth                = pField->m_nWidth;
	m_sLabel                = pField->m_sLabel;
	m_sName                 = pField->GetFieldName();
	m_byteMeasure           = pField->m_byteMeasure;	  //### 2002-9-25 Eric
	m_strCurrencyType       = ((CCurrencyField*)pField)->m_strCurrencyType;

	//m_ValueLabel.m_pField   = this;
	m_ValueLabel.SetOwner(this);
    m_ValueLabel.Clone(&pField->m_ValueLabel);
	m_ShowCharacteristic.Clone(&pField->m_ShowCharacteristic);
	m_pMissing->Clone(pField->m_pMissing); //### 2002-9-25 Eric

}

/************************************************************************************
�������ƣ�
int CCurrencyField::SaveToBuffer(LPBYTE lpData)
����˵�������ֶ���Ϣ���ļ���ʽ����Buffer��.

��ϸ���ͣ�1.�ڲ�����SaveExt()ʵ����չ���ı���.
          2.���ر�����ֽ���.
		  3.�������ָ����Ч,�򷵻�-1.

���������
[in]: 1.lpData:���ֶα�����Buffer���׵�ַ.      
  
[out]:��.��

�������ͣ�int

������YTLI 2002/07/15

�޸�: 
************************************************************************************/
/*
int CCurrencyField::SaveToBuffer(LPBYTE lpData)
{
	if(!lpData)
		return -1;

	int iRetVal = (int)lpData; //����ֵ��������ֽ���.

	*lpData = (BYTE)GetFieldType();
	lpData += 1;               //ָ����ֶ����ݳ���.

	//WORD wTemp;
	memcpy( lpData, &m_nWidth, 2 );
	lpData += 2;               //ָ����ֶ�С���ĳ���.

	*lpData = (BYTE)m_nDecimals;        
	lpData += 1;               //ָ����ֶ���.
	//#_S 2003-3-31 $ 16 zlq ��չ�ֶ�������**************************
	//�õ��ֶ���
	{
		int iFieldNameLength = m_sName.GetLength();
		*lpData = iFieldNameLength;
		lpData++;//ָ������ 1

		for(int i = 0; i<iFieldNameLength; i++)
		{
			if( i <  iFieldNameLength )        
				*lpData = m_sName.GetAt(i);                
		
			lpData += 1;
		}

	}
	//��ʱ lpData ָ�� Label ����***************************

	int iFieldLabelLength = m_sLabel.GetLength();
	*lpData = iFieldLabelLength;
	lpData += 1;               //ָ����ֶ� Label ������
	for(int j = 0; j<iFieldLabelLength; j++)
	{
		*lpData = m_sLabel.GetAt(j);
		lpData += 1;
	}
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
	
	if( m_ValueLabel.m_strLabelList.GetCount() != 0 )
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
	wdExtLength += 3+m_strCurrencyType.GetLength();
	iExtCnt++;

	//### 2002-9-25 Eric==>
	wdExtLength += 1;//m_byteMeasure	
	wdExtLength += m_pMissing->QueryLength();
	//===temp TRACE("m_pMissing %d\n",m_pMissing->QueryLength());
	//### 2002-9-25 Eric<==

	{
		memcpy( lpData, &wdExtLength, 4 );
		lpData += 4;               //ָ����ֶ���չ���Ŀ���
	}

	*lpData = iExtCnt;        
	lpData += 1;               //ָ����ֶ���չ��

	SaveExt(lpData,wdExtLength);

	iRetVal = (int)lpData - iRetVal + wdExtLength;        
	return iRetVal;
}*/

//���ݵ�ǰ��Ԫ�������ڱ�����(m_ValueLable)��ȡ����Ӧ�ı���ֵ
LPCTSTR CCurrencyField::GetValueLable()
{
	double nValue = GetAsDouble();

	int nSize=m_ValueLabel.GetCount();
	for(int n=0; n<nSize; n++)
	{
		m_ValueLabel.SetCurPos(n);
		if(fabs(m_ValueLabel.GetAsDouble() - nValue) < DoubleZero)
			return m_ValueLabel.GetExplainString();
	}
	
	return _T("");
}
CTString CCurrencyField::GetValLable()
{
	double nValue = GetAsDouble();
	
	int nSize=m_ValueLabel.GetCount();
	for(int n=0; n<nSize; n++)
	{
		m_ValueLabel.SetCurPos(n);
		if(fabs(m_ValueLabel.GetAsDouble() - nValue) < DoubleZero)
			return m_ValueLabel.GetExplainString();
	}
	
	return _T("");
}