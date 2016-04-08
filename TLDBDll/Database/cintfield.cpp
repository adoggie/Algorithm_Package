// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "cfield.h"
#include "cintfield.h"
#include "cfields.h"

CIntField::CIntField(CFields* pOwner)
{
	m_pFields = pOwner;
	m_pDefaultValue =(LPBYTE) new FieldNUM;//ytli Modify 2002/08/30

	if(m_pDefaultValue)	
		*(FieldNUM*)m_pDefaultValue = NULLValue;	
	m_pTempData = new FieldNUM;
	*((FieldNUM*)m_pTempData) = NULLValue;
	m_byteMeasure=msScale;
}

CIntField::~CIntField()
{
	delete (FieldNUM *)m_pTempData;
	delete (FieldNUM *)m_pDefaultValue;
}

//##ModelId=3CF33AB00325
CFieldType CIntField::GetFieldType()
{	
	return fInt;
}

bool CIntField::GetAsBool()
{
	if (*((FieldNUM *)m_pValue))
		return true;
	else
		return false;
}

FieldCurrency CIntField::GetAsCurrency()
{
	FieldCurrency c;

	c     = *((FieldNUM *)m_pValue);
	return c;
}

FILETIME CIntField::GetAsDateTime()
{
	COleDateTime OleTime( GetAsDouble() );
	
	SYSTEMTIME st;	
	FILETIME   FT ;

/*===time  	memset(&st,0,sizeof(st));
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

double CIntField::GetAsDouble()
{
	double fRetValue;

    fRetValue = *((FieldNUM *)m_pValue); 				

	return fRetValue;
}

int CIntField::GetAsInteger()
{
	int iRetValue = 0;

	iRetValue = *((FieldNUM *)m_pValue); 				

	return iRetValue;
}

CTString CIntField::GetAsString()
{
	CTString strRetValue;

	if( *(FieldNUM*)m_pValue != NULLValue )
	{
		_itoa( (int)*((FieldNUM *)m_pValue), m_pTempBuf, 10 );   
		strRetValue = _T(m_pTempBuf);		
	}		
	return strRetValue;
}



//�����Y����͵Ĵ�С���ڠ��Y��朱�����¿��g�r�_��
//(����ָʾλԪ���Y��,string��͕r��4)
int CIntField::GetTypeSize()
{
	return 5;
}


bool CIntField::SetAsDouble(double Value)
{
	if(!m_pValue)
	{
		CField::SetAsDouble(Value);
	}

	else
	{
		if(fabs(Value)>0xFFFFFFFF)
		{			
			//int retCode = //===temp AfxMessageBox(_T("����̫����������ݴ洢����ȷ���Ƿ�Ҫת��\nCIntField::SetAsDouble"),MB_OKCANCEL);
			//if(retCode == IDCANCEL )			
			//	return false;			
			SetNull();		
		}
        //��double(8)��ת��Ϊfloat(4)����
		*((FieldNUM *)m_pValue) = (int)Value;
	}

	return true;
}

bool CIntField::SetAsBool(bool Value)
{
	if(!m_pValue)
	{
		CField::SetAsBool(Value);
	}
	else 
	{
		*((FieldNUM *)m_pValue) = (int)Value;
	}

	return true;
}

bool CIntField::SetAsCurrency(FieldCurrency Value)
{
	if(!m_pValue)
	{
		CField::SetAsCurrency(Value);
	}
	else 
	{
		*((FieldNUM *)m_pValue) = (int)Value;
	}

	return true;
}


bool CIntField::SetAsInteger(int Value)
{
	if(!m_pValue)
	{
		CField::SetAsInteger(Value);
	}
	else 
	{		
		*((FieldNUM *)m_pValue) = Value;
	}

	return true;
}


bool CIntField::SetAsDateTime(FILETIME* Value)
{	
	if(!m_pValue)
	{
		CField::SetAsDateTime(Value);
	}
	else 
	{		
/*===time  		COleDateTime OleTime( *Value );

		*((FieldNUM *)m_pValue)  = (int)OleTime.m_dt;	   */
	}

	return true;
}

bool CIntField::SetAsString(CTString str)
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
		int iSignPostion = 0;//+-�ŵ�λ��,0����E�ĺ���.  
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
			else if( !isdigit(str[i]) )//e:��0����E,��С����,��+-�ŵ��ַ����ۼ�.
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
		int    iTemp = int( fTemp );

		double dd = fabs(fTemp - iTemp);
		//if( dd > 1E-10)//�����Ƿ�������.
		//	return false;
	
		*((FieldNUM *)m_pValue) = iTemp;	
	}

	return true;
}



//�������ݴ�С��������ָʾλ,���ڶ��ļ��ȶ�ʱȷ����һ����λ��
//lpData���ڱ䳤ʱ���������ַ��ͣ�ָ�����ݣ�����ʱΪ��
//##ModelId=3CF42CC30261
int CIntField::GetDataLength(LPBYTE lpData)
{
	return 4;
}



//����lpDataָ������ݣ����Ƹ�����ֵ�����ֶΣ�
//��������ͬ�����ݸ�ʽ�����û�����edit��cancel����ʱ�ָ�ԭֵ
bool CIntField::GetData(void*pDestArrar,int nIndex)
{
	if( !(m_pValue ) )
		return false;
	*(FieldNUM*)m_pValue = (*(PF_NUM)pDestArrar)[nIndex];
	return true;
}
bool CIntField::GetData(void* lpData)
{
	if( !(m_pValue ) )
		return false;
	*(FieldNUM*)m_pValue = *(FieldNUM*)lpData;
	return true;
}

//���ⲿ�û����������(�ַ�����ʽ)���浽�ֶ���,����ɹ�.�򷵻�true
bool CIntField::GetFromDisplay(CTString str)
{
	if(!m_pValue)
		return false;


	{	
		if(!SetAsString(str))
			return false;
	}

	return true;
}



//����ǰ��������Ϊȱʡֵ,����m_pDefaultValue��ָ������ݿ�����m_pValue��
void CIntField::SetDefaultValue()
{
	*((FieldNUM *)m_pValue) = *((FieldNUM *)m_pDefaultValue);
	//memcpy(m_pValue,m_pDefaultValue,5);
}

//�õ�ǰ������������Ϊȱʡֵ,����m_pValue��ָ������ݿ�����m_pDefaultValue�У�ԭ������û������
bool CIntField::CopyToDefaultValue()
{
	*((FieldNUM *)m_pDefaultValue) = *((FieldNUM *)m_pValue);
	//memcpy(m_pDefaultValue,m_pValue,5);
	return true;
}

//���ݵ�ǰ��Ԫ�������ڱ�����(m_ValueLable)��ȡ����Ӧ�ı���ֵ
LPCTSTR CIntField::GetValueLable()
{
	int nValue = GetAsInteger();

	int nSize=m_ValueLabel.GetCount();
	for(int n=0; n<nSize; n++)
	{
		m_ValueLabel.SetCurPos(n);
		if(m_ValueLabel.GetAsInteger()== nValue)
			return m_ValueLabel.GetExplainString();
	}
	
	return _T("");
}

CTString CIntField::GetValLable()
{
	int nValue = GetAsInteger();
	
	int nSize=m_ValueLabel.GetCount();
	for(int n=0; n<nSize; n++)
	{
		m_ValueLabel.SetCurPos(n);
		if(m_ValueLabel.GetAsInteger()== nValue)
			return m_ValueLabel.GetExplainString();
	}
	
	return _T("");
}