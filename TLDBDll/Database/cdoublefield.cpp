// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "cdoublefield.h"

CDoubleField::CDoubleField(CFields* pOwner)
{
	m_pFields = pOwner;
	m_pDefaultValue =(LPBYTE) new FieldNUM;//ytli Modify 2002/08/30

	if(m_pDefaultValue)	
		*(FieldNUM*)m_pDefaultValue = NULLValue;	
	m_pTempData = new FieldNUM;
	*((FieldNUM*)m_pTempData) = NULLValue;

	m_byteMeasure=msScale;
}

CDoubleField::~CDoubleField()
{
	delete (FieldNUM *)m_pDefaultValue;
	delete (FieldNUM *)m_pTempData;
}


int CDoubleField::GetTypeSize()
{
	return 9;
}

CFieldType CDoubleField::GetFieldType()
{
	return fDouble;
}

double CDoubleField::GetAsDouble()
{


	double dd = *((FieldNUM *)m_pValue);
	return dd; 
}


FieldCurrency CDoubleField::GetAsCurrency()
{
	FieldCurrency c;
	//c.Indicate     = ((FieldNUM *)m_pValue)->Indicate;
	c      = *((FieldNUM *)m_pValue);
	return c;
}


FILETIME CDoubleField::GetAsDateTime()
{
	
	
	SYSTEMTIME st;	
	FILETIME   FT ;
/*===time  COleDateTime OleTime( GetAsDouble() );
	memset(&st,0,sizeof(st));
	st.wDay          = OleTime.GetDay();
	st.wHour         = OleTime.GetHour();
	st.wMinute       = OleTime.GetMinute();
	st.wMonth        = OleTime.GetMonth();
	st.wSecond       = OleTime.GetSecond();
	st.wYear         = OleTime.GetYear();

	SystemTimeToFileTime(&st, &FT);
   */
	return FT;
}
bool CDoubleField::GetAsBool()
{
	if (fabs(*((FieldNUM *)m_pValue))<=DoubleZero)
		return true;//#_�޸� 2002-12-2 $ 9:59:35 zlq
	else
		return false;
}

int CDoubleField::GetAsInteger()
{
	return (int)*((FieldNUM *)m_pValue);
}

CTString CDoubleField::GetAsString()
{
	////assert( m_pValue && *m_pValue == fvValid );	 //ָʾ�ֽڱ���Ϊ1

	CTString strRetValue;
	if( !IsNull() )
	{
		CTString strFormat ;
		//strFormat.Format("%%%d.%df",m_nWidth,m_nDecimals);
		strFormat.Format("%%%d.%df",0,m_nDecimals);
		strRetValue.Format(strFormat,*((FieldNUM *)m_pValue));
		strRetValue.TrimLeft();
	
	}		
	else
		return "";
	return strRetValue;
}

bool CDoubleField::SetAsDouble(double Value)
{ 
	if(!m_pValue)
	{
		CField::SetAsDouble(Value);
	}
	else 
	{		
		*((FieldNUM *)m_pValue) = Value;
	}

	return true;
}

bool CDoubleField::SetAsInteger(int Value)
{
	if(!m_pValue)
	{
		CField::SetAsInteger(Value);
	}
	else 
	{		
		*((FieldNUM *)m_pValue) = (double)Value;
	}

	return true;
}

bool CDoubleField::SetAsBool(bool Value)
{
	if(!m_pValue)
	{
		CField::SetAsBool(Value);
	}
	else 
	{		
		*((FieldNUM *)m_pValue) = (double)Value;
	}

	return true;
}

bool CDoubleField::SetAsCurrency(FieldCurrency Value)
{
	if(!m_pValue)
	{
		CField::SetAsCurrency(Value);
	}
	else 
	{		
		*((FieldNUM *)m_pValue) = Value;
	}

	return true;
}

bool CDoubleField::SetAsDateTime(FILETIME* Value)
{	//������ת��.
	if(!m_pValue)
	{
		CField::SetAsDateTime(Value);
	}
	else
	{		
/*===time  		COleDateTime OleTime( *Value );
		
		*((FieldNUM *)m_pValue)  = OleTime.m_dt-1.0/3;	   */
	}

	return true;
}

bool CDoubleField::SetAsString(CTString str)
{	
	if(!m_pValue)
	{
		CField::SetAsString(str);
	}
	else 
	{
		str.Remove(0x20);//�����ո�.
		str.Remove(',');
		str.MakeLower(); //ת��ΪСд.		
		int iLength  = str.GetLength();				
		
		int iDecimalCnt  = 0;//С�������,���ֻ������һ��,λ�ñ�����E֮ǰ.
		int iECnt        = 0;//E�ĸ���,���ֻ������һ��(��ѧ������).
		int iOtherCnt    = 0;//��0����E,��С����,��+-�ŵ��ַ����ۼ�,����ȷ��������0ʱ�Ƿ����Ӧ����0.
		int iSignPostion = 0;//+-�ŵ�λ��,�ͷ����E�ĺ���.  
		//����:������ֻ��������λ��:�ַ����Ŀ�ͷ��E�ĺ���,E�ĺ������Ҫ������.
		
		for(int i = 0; i<iLength; i++)
		{							
			if(str[i] == '+' ||str[i] == '-')
			{
				if(!(i == 0 || i == iSignPostion ))//���������ŵ�λ��.
					return false;
			}
			else if(str[i] == '.')//С����.
			{
				if( (iSignPostion != 0) && (i >= iSignPostion) )
					return false;

				iDecimalCnt++;
			}
			else if(str[i] == 'e')//e:Сд.
			{
				if(i==0)
					return false;

				iECnt++;
				iSignPostion = i + 1;
			}
			else if( !isdigit(str[i]) )//e:�����֣���E,��С����,��+-�ŵ��ַ����ۼ�.
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
		
		*((FieldNUM *)m_pValue) = fTemp;		
	}

	return true;
}

//�������ݴ�С��������ָʾλ,���ڶ��ļ��ȶ�ʱȷ����һ����λ��
//lpData���ڱ䳤ʱ���������ַ��ͣ�ָ�����ݣ�����ʱΪ��
int CDoubleField::GetDataLength(LPBYTE lpData)
{
	return 8;
}



//�����ֶε���������һ����������ַ�ռ�Ҳ�ɱ��������ɣ�
//�����ظõ�ַ���������ݴ��ݺͱ����ֵ
/*
LPBYTE CDoubleField::SetData()
{
	LPBYTE byteRetValue = new BYTE[9];

	//assert( (m_pValue)&&(byteRetValue) );//###_Mod 2002-9-4 zlq
	
	memcpy(byteRetValue,m_pValue,9);
	return byteRetValue;
}*/

//����lpDataָ������ݣ����Ƹ�����ֵ�����ֶΣ�
//��������ͬ�����ݸ�ʽ�����û�����edit��cancel����ʱ�ָ�ԭֵ
bool CDoubleField::GetData(void*pDestArrar,int nIndex)
{
	if( !(m_pValue ) )
		return false;
	*(FieldNUM*)m_pValue = (*(PF_NUM)pDestArrar)[nIndex];
	return true;
}
bool CDoubleField::GetData(void* lpData)
{
	if( !(m_pValue ) )
		return false;
	*(FieldNUM*)m_pValue = *(FieldNUM*)lpData;
	return true;
}



//���ⲿ�û����������(�ַ�����ʽ)���浽�ֶ���,����ɹ�,�򷵻�true.
bool CDoubleField::GetFromDisplay(CTString str)
{	
	if(!m_pValue)
		return false;


		if(!SetAsString(str))
			return false;

	return true;
}


//����ǰ��������Ϊȱʡֵ,����m_pDefaultValue��ָ������ݿ�����m_pValue��
void CDoubleField::SetDefaultValue()
{
	*((FieldNUM *)m_pValue) = *((FieldNUM *)m_pDefaultValue);
	//memcpy(m_pValue,m_pDefaultValue,9);
}

//�õ�ǰ������������Ϊȱʡֵ,����m_pValue��ָ������ݿ�����m_pDefaultValue�У�ԭ������û������
bool CDoubleField::CopyToDefaultValue()
{
	*((FieldNUM *)m_pDefaultValue) = *((FieldNUM *)m_pValue);
	//memcpy(m_pDefaultValue,m_pValue,9);
	return true;
}


//���ݵ�ǰ��Ԫ�������ڱ�����(m_ValueLable)��ȡ����Ӧ�ı���ֵ
LPCTSTR CDoubleField::GetValueLable()
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
CTString CDoubleField::GetValLable()
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