// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "cstringfield.h"
#include "cpages.h"


CStringField::CStringField(CFields* pOwner)
{
	m_pFields = pOwner;
	FieldString* pStr = new FieldString();


	m_pDefaultValue = (LPBYTE)(pStr);//ytli Modify 2002/08/30
	m_pTempData = new FieldString;

	m_byteMeasure=msNominal;
}

CStringField::~CStringField()
{
	FieldString* pStr = (FieldString*)m_pDefaultValue;
	
	delete pStr;//ytli Modify 2002/08/30
	delete (FieldString *)m_pTempData;
}

bool  CStringField::GetAsBool()
{
	////assert( m_pValue && ((FieldString*)m_pValue)->Indicate == fvValid);	 //ָʾ�ֽڱ���Ϊ1

	FieldString* pStr = (FieldString*)m_pValue;
	CTString Value = *pStr;
	Value.MakeLower();
	
	if(Value == _T("1") || Value == _T("true") || Value == _T("��") ||
		Value == _T("y") || Value == _T("yes")  || Value == _T("��") || Value == _T("t"))
	{
		return true;
	}	
	else
	{
		return false;	
	}	
	
}

FieldCurrency CStringField::GetAsCurrency()
{
	////assert( m_pValue && ((FieldString*)m_pValue)->Indicate == fvValid);	 //ָʾ�ֽڱ���Ϊ1
	return GetAsDouble();

}

FILETIME CStringField::GetAsDateTime()
{
	////assert( m_pValue && ((FieldString*)m_pValue)->Indicate == fvValid);	 //ָʾ�ֽڱ���Ϊ1

	DateType byteType = GetDateConvertType();
	CTString str = GetAsString();
	SYSTEMTIME st;
	CDateField::StringToSysTime(str, st, byteType);
	//	return NULL;
	FILETIME ft ;
/*===time  	
	::SystemTimeToFileTime( &st, &ft);
   */
	return ft;
}

double  CStringField::GetAsDouble()
{
//	//assert( m_pValue && ((FieldString*)m_pValue)->Indicate == fvValid);	 //ָʾ�ֽڱ���Ϊ1

	FieldString* pStr = (FieldString*)m_pValue;
	const char*p = (const char*)(*pStr);
	return atof(p);
}

int  CStringField::GetAsInteger()
{
//	//assert( m_pValue && ((FieldString*)m_pValue)->Indicate == fvValid);	 //ָʾ�ֽڱ���Ϊ1

	FieldString* pStr = (FieldString*)m_pValue;
	const char*p = (const char*)(*pStr);
	return atoi(p);

}

CTString CStringField::GetAsString()
{	
//	//assert( m_pValue && ((FieldString*)m_pValue)->Indicate == fvValid);	 //ָʾ�ֽڱ���Ϊ1

	FieldString* pStr = (FieldString*)m_pValue;	

	return *pStr;
}

bool CStringField::SetAsBool(bool Value)
{
	if(!m_pValue)
	{
		CField::SetAsBool(Value);
	}
	else 
	{		
		FieldString* pCStr = (FieldString*)m_pValue;

		if(Value)
			*pCStr = _T("1");
		else
			*pCStr= _T("0");
    }

	return true;
}

bool CStringField::SetAsDouble(double Value)
{
	if(!m_pValue)
	{
		CField::SetAsDouble(Value);
	}
	else 
	{
		char buffer[50];
		gcvt( Value, 20, buffer );
		FieldString* pCStr = (FieldString*)m_pValue;

		*pCStr = _T(buffer);
		if (pCStr->GetLength()>m_nWidth)//�������ָ������,���ȥ�ұߵ�.//#_S 2003-1-8 $ 14 zlq
			*pCStr = pCStr->Left(m_nWidth);
    }

	return true;
}

bool CStringField::SetAsInteger(int Value)
{
	if(!m_pValue)
	{
		CField::SetAsInteger(Value);
	}
	else 
	{
		char buffer[11];
		_itoa( Value, buffer, 10 );
		FieldString* pCStr = (FieldString*)m_pValue;

		*pCStr = _T(buffer);
		if (pCStr->GetLength()>m_nWidth)//�������ָ������,���ȥ�ұߵ�.//#_S 2003-1-8 $ 14 zlq
			*pCStr = pCStr->Left(m_nWidth);
    }

	return true;
}

bool CStringField::SetAsString(CTString Value)
{	
	if(!m_pValue)
	{
		CField::SetAsString(Value);
	}
	else 
	{	
		//Value.TrimLeft();
		Value.TrimRight();
		int iLength = Value.GetLength();		
		FieldString* pStr = (FieldString*)m_pValue;
		*pStr    = Value;		
		if (iLength>m_nWidth)//�������ָ������,���ȥ�ұߵ�.//#_S 2003-1-8 $ 14 zlq
			*pStr = pStr->Left(m_nWidth);
	}

	return true;
}

bool CStringField::SetAsCurrency(FieldCurrency Value)
{
	SetAsDouble( Value );

	return true;
}

bool CStringField::SetAsDateTime(FILETIME* Value)
{//������ת��.
	if(!m_pValue)
	{
		CField::SetAsDateTime(Value);
	}
	else
	{
		FieldString* pCStr = (FieldString*)m_pValue;


/*===time  		CTString strRetValue;
		COleDateTime m_tm = COleDateTime(*Value );
		strRetValue = m_tm.Format(CDateField::GetDateType( m_byteEnumData ));
		*pCStr = strRetValue;
		if (strRetValue.GetLength()>m_nWidth)//�������ָ������,���ȥ�ұߵ�.//#_S 2003-1-8 $ 14 zlq
			*pCStr = pCStr->Left(m_nWidth);   */
	}

	return true;
}

int CStringField::GetDataLength(LPBYTE lpData)
{	
	FieldString* pStr = (FieldString*)lpData;	
	return pStr->GetLength();
}

CFieldType CStringField::GetFieldType()
{
	return fString;
}

int CStringField::GetTypeSize()
{	
	return 4;
}





//����lpDataָ������ݣ����Ƹ�����ֵ�����ֶΣ�
//��������ͬ�����ݸ�ʽ�����û�����edit��cancel����ʱ�ָ�ԭֵ
bool CStringField::GetData(void*pDestArrar,int nIndex)
{
	if( !(m_pValue ) )
		return false;
	*(FieldString*)m_pValue = (*(PF_STRING)pDestArrar)[nIndex];
	return true;
}
bool CStringField::GetData(void* lpData)
{
	if( !(m_pValue ) )
		return false;
	*(FieldString*)m_pValue = *(FieldString*)lpData;
	return true;
}



//���ⲿ�û����������(�ַ�����ʽ)���浽�ֶ���,����ɹ�.�򷵻�true
bool CStringField::GetFromDisplay(CTString str)
{
	if(!m_pValue)
		return false;


	{	
		int iLength = str.GetLength();

		if(iLength > m_nWidth)
			str = str.Left(m_nWidth);

		if(!SetAsString(str))
			return false;		
	}

	return true;
}



//��ԭ���汾���ֶ�����ת������ǰ�ֶε����ݡ�
//1��������ֶοռ�Ϊ�գ����ɿռ�
//2��Ҫ����ԭ���ֶο�ֵ�жϣ�
//2��1���Ϊ�����ֶ�Ϊ��
//2��2�����Ϊ����ԭ����������ԭ�����������Ͷ�    ����Ȼ����ñ��ֶε�SetAs***���á�
//Ȼ��������µ�next�ֶΣ�����ø��ֶε�ConvertToCurField���������Ҹ����ͷű��ռ䣬������0
/*
void CStringField::ConvertToCurField(CField* pOldField)
{
	if(!m_pValue)
	{
		NewData();

		if(pOldField->IsNull())
			SetNull();
		else
			SetAsString( pOldField->GetAsString());

		//assert(m_nNextModifyField != -1);

		CField* pNewField = m_pFields->m_FieldVersion.GetFieldByAbsolutePos(m_nNextModifyField);
		pNewField->ConvertToCurField(this); 	

		DeleteData();
	}
	else   //���һ��ת��.
	{
		if(pOldField->IsNull())
			SetNull();
		else
			SetAsString( pOldField->SetToDisplay());	

			//assert(m_nNextModifyField == -1);		
	}
}
*/
//����ǰ��������Ϊȱʡֵ,����m_pDefaultValue��ָ������ݿ�����m_pValue��
void CStringField::SetDefaultValue()
{
	FieldString* pStr = (FieldString*)m_pValue;
	FieldString* pDefaultStr = (FieldString*)m_pDefaultValue;

	*pStr    = *pDefaultStr;
}

//�õ�ǰ������������Ϊȱʡֵ,����m_pValue��ָ������ݿ�����m_pDefaultValue�У�ԭ������û������
bool CStringField::CopyToDefaultValue()
{
	FieldString* pStr = (FieldString*)m_pValue;
	FieldString* pDefaultStr = (FieldString*)m_pDefaultValue;

	*pDefaultStr    = *pStr;
	return true;
}


///cwh 2002-11-14
//���ݵ�ǰ��Ԫ�������ڱ�����(m_ValueLable)��ȡ����Ӧ�ı���ֵ
LPCTSTR CStringField::GetValueLable()
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
CTString CStringField::GetValLable()
{
	CTString strValue=GetAsString();
	
	int nSize=m_ValueLabel.GetCount();
	for(int n=0;n<nSize;n++)
	{
		m_ValueLabel.SetCurPos(n);
		if(m_ValueLabel.GetAsString()==strValue)
			return m_ValueLabel.GetExplainString();
	}
	
	return _T("");
}