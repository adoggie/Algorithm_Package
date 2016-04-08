// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "cfield.h"
#include "cbooleanfield.h"
#include "cfields.h"

CBooleanField::CBooleanField(CFields* pOwner)
{ 
	//assert( pOwner );
	m_pFields = pOwner;
	m_pDefaultValue =(LPBYTE) new FieldNUM;//ytli Modify 2002/08/30

	if(m_pDefaultValue)	
		*m_pDefaultValue = NULLValue;	
	m_pTempData = new FieldNUM;
	*((FieldNUM*)m_pTempData) = NULLValue;
	m_byteMeasure=msNominal;
}

CBooleanField::~CBooleanField()
{
	if(m_pDefaultValue)
		delete (FieldNUM *)m_pDefaultValue;
	if(m_pTempData)
		delete (FieldNUM *)m_pTempData;
}



CFieldType CBooleanField::GetFieldType()
{
	return fBoolean;
}

/************************************************************************************
�������ƣ�
int CBooleanField::GetTypeSize()
����˵���������������͵Ĵ�С,��Ϊ������������¿ռ�ʱȷ��.

��ϸ���ͣ�1.����ָʾλ������.    

���������
[in]: ��.      
  
[out]:��.��

�������ͣ�int

������YTLI 2002/07/15

�޸�: 
************************************************************************************/
int CBooleanField::GetTypeSize()
{
	return 2;
}

bool   CBooleanField::GetAsBool()
{
	return (fabs(*(FieldNUM*)m_pValue) < DoubleZero)? 0:1;
}

FieldCurrency CBooleanField::GetAsCurrency()
{

	return GetAsDouble();
}

//���ⲿ�����ͷŷ��صĵ�ַ�ռ�.
FILETIME CBooleanField::GetAsDateTime()
{
/*===uc ʱ������  ͳһΪ ===time
	COleDateTime OleTime( GetAsDouble() );
	
	SYSTEMTIME st;	
	FILETIME   FT ;


	memset(&st,0,sizeof(st));
	st.wDay          = OleTime.GetDay();
	st.wHour         = OleTime.GetHour();
	st.wMinute       = OleTime.GetMinute();
	st.wMonth        = OleTime.GetMonth();
	st.wSecond       = OleTime.GetSecond();
	st.wYear         = OleTime.GetYear();

	SystemTimeToFileTime(&st, &FT);
   */
FILETIME   FT ;
	return FT;
}

CTString  CBooleanField::GetAsString()
{
	if( !IsNull() )
	{
		
		if(*((FieldNUM *)m_pValue) == 0)
			return "0";
		else
			return "1";	
	}		
	else
		return "";
	
}

double CBooleanField::GetAsDouble()
{
	return (double)GetAsBool();
}

int CBooleanField::GetAsInteger()
{
	return (int)GetAsBool();
}

bool CBooleanField::SetAsDouble(double Value)
{
	if(!m_pValue)
	{
		CField::SetAsDouble(Value);
	}

		

		*((FieldNUM *)m_pValue) = (fabs(Value) < DoubleZero)? 0:1;

	return true;
}

bool CBooleanField::SetAsInteger(int Value)
{
	if(!m_pValue)
	{
		CField::SetAsInteger(Value);
	}

	*(FieldNUM *)m_pValue = (Value == 0)? 0:1;
	return true;
}

bool CBooleanField::SetAsString(CTString Value)
{
	if(!m_pValue)
	{
		CField::SetAsString(Value);
	}
	else 
	{
		Value.MakeLower();

		if(Value==_T("")||Value == _T("0") || Value == _T("false") || Value == _T("��") ||
		   Value == _T("n") || Value == _T("no")  || Value == _T("��") || Value == _T("f"))
		{
			*(FieldNUM *)m_pValue = 0;
		}	
		else
		{
			*(FieldNUM *)m_pValue = 1;
		}
	}

	return true;
}

bool CBooleanField::SetAsDateTime(FILETIME* Value)
{
	if(!m_pValue)
	{		
		CField::SetAsDateTime(Value);
	}
	else 
	{		
/*===time  		COleDateTime OleTime( *Value );
		

		if( fabs(OleTime.m_dt-1.0/3)<DoubleZero )
			*(FieldNUM *)m_pValue = 0;
		else 
			*(FieldNUM *)m_pValue = 1;   */
	}

	return true;
}

bool CBooleanField::SetAsCurrency(FieldCurrency Value)
{	
	if(!m_pValue)
	{		
		CField::SetAsCurrency(Value);
	}
	else 
	{
			SetAsDouble(Value);
	}

	return true;
}

bool CBooleanField::SetAsBool(bool Value)
{
	if(!m_pValue)
	{		
		CField::SetAsBool(Value);
	}
	else
	{

		if (Value)
			*(FieldNUM *)m_pValue = 1;
		else
			*(FieldNUM *)m_pValue = 0;
	}

	return true;
}

/************************************************************************************
�������ƣ�
int CBooleanField::GetDataLength(LPBYTE lpData)
����˵�����麯��,�������ݳ���,������ָʾλ,���ڶ��ļ��ȶ�ʱȷ����һ����λ��.

��ϸ���ͣ�1.��ͬ���ͷ��ز�ͬ����.       

���������
[in]:1.lpData:ָ�����ݵ�ָ��,�䳤ʱ��(���ַ���),����ʱΪ��. 
  
[out]:��.

��������:int


������YTLI 2002/07/15

�޸�: 
************************************************************************************/
int CBooleanField::GetDataLength(LPBYTE lpData)
{
	return 1;
}






//����lpDataָ������ݣ����Ƹ�����ֵ�����ֶΣ�
//��������ͬ�����ݸ�ʽ�����û�����edit��cancel����ʱ�ָ�ԭֵ
bool CBooleanField::GetData(void*pDestArrar,int nIndex)
{
	//assert( pDestArrar && nIndex>=0 );

	if( !(m_pValue ) )
		return false;
	*(FieldNUM*)m_pValue = (*(PF_NUM)pDestArrar)[nIndex];
	return true;
}
bool CBooleanField::GetData(void* lpData)
{
	//assert( lpData );

	if( !(m_pValue ) )
		return false;
	*(FieldNUM*)m_pValue = *(FieldNUM*)lpData;
	return true;
}

//���ⲿ�û����������(�ַ�����ʽ)���浽�ֶ���,����ɹ�.�򷵻�true
bool CBooleanField::GetFromDisplay(CTString str)
{
	if(!m_pValue)
		return false;

	
	if(!SetAsString(str))
		return false;

	return true;

}


//����ǰ��������Ϊȱʡֵ,����m_pDefaultValue��ָ������ݿ�����m_pValue��
void CBooleanField::SetDefaultValue()
{
	//assert( m_pDefaultValue && m_pValue );
	*((FieldNUM *)m_pValue) = *((FieldNUM *)m_pDefaultValue);
}

//�õ�ǰ������������Ϊȱʡֵ,����m_pValue��ָ������ݿ�����m_pDefaultValue�У�ԭ������û������
bool CBooleanField::CopyToDefaultValue()
{
	//assert( m_pValue && m_pDefaultValue );
	*((FieldNUM *)m_pDefaultValue) = *((FieldNUM *)m_pValue);
	return true;
}


//���ݵ�ǰ��Ԫ�������ڱ�����(m_ValueLable)��ȡ����Ӧ�ı���ֵ
LPCTSTR CBooleanField::GetValueLable()
{
	bool bValue = GetAsBool();

	int nSize=m_ValueLabel.GetCount();
	for(int n=0; n<nSize; n++)
	{
		m_ValueLabel.SetCurPos(n);
		if(m_ValueLabel.GetAsBool()== bValue)
			return m_ValueLabel.GetExplainString();
	}
	
	return _T("");
}
CTString CBooleanField::GetValLable()
{
	bool bValue = GetAsBool();
	
	int nSize=m_ValueLabel.GetCount();
	for(int n=0; n<nSize; n++)
	{
		m_ValueLabel.SetCurPos(n);
		if(m_ValueLabel.GetAsBool()== bValue)
			return m_ValueLabel.GetExplainString();
	}
	
	return _T("");
}