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
	////assert( m_pValue && ((FieldString*)m_pValue)->Indicate == fvValid);	 //指示字节必须为1

	FieldString* pStr = (FieldString*)m_pValue;
	CTString Value = *pStr;
	Value.MakeLower();
	
	if(Value == _T("1") || Value == _T("true") || Value == _T("是") ||
		Value == _T("y") || Value == _T("yes")  || Value == _T("真") || Value == _T("t"))
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
	////assert( m_pValue && ((FieldString*)m_pValue)->Indicate == fvValid);	 //指示字节必须为1
	return GetAsDouble();

}

FILETIME CStringField::GetAsDateTime()
{
	////assert( m_pValue && ((FieldString*)m_pValue)->Indicate == fvValid);	 //指示字节必须为1

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
//	//assert( m_pValue && ((FieldString*)m_pValue)->Indicate == fvValid);	 //指示字节必须为1

	FieldString* pStr = (FieldString*)m_pValue;
	const char*p = (const char*)(*pStr);
	return atof(p);
}

int  CStringField::GetAsInteger()
{
//	//assert( m_pValue && ((FieldString*)m_pValue)->Indicate == fvValid);	 //指示字节必须为1

	FieldString* pStr = (FieldString*)m_pValue;
	const char*p = (const char*)(*pStr);
	return atoi(p);

}

CTString CStringField::GetAsString()
{	
//	//assert( m_pValue && ((FieldString*)m_pValue)->Indicate == fvValid);	 //指示字节必须为1

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
		if (pCStr->GetLength()>m_nWidth)//如果超过指定长度,则剪去右边的.//#_S 2003-1-8 $ 14 zlq
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
		if (pCStr->GetLength()>m_nWidth)//如果超过指定长度,则剪去右边的.//#_S 2003-1-8 $ 14 zlq
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
		if (iLength>m_nWidth)//如果超过指定长度,则剪去右边的.//#_S 2003-1-8 $ 14 zlq
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
{//不可以转换.
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
		if (strRetValue.GetLength()>m_nWidth)//如果超过指定长度,则剪去右边的.//#_S 2003-1-8 $ 14 zlq
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





//根据lpData指向的数据，复制该数据值到本字段，
//两者有相同的数据格式，如用户调用edit，cancel操作时恢复原值
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



//把外部用户输入的数据(字符串形式)保存到字段内,如果成功.则返回true
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



//将原来版本的字段数据转换到当前字段的数据。
//1。如果本字段空间为空，生成空间
//2。要进行原来字段空值判断！
//2。1如果为空则本字段为空
//2。2如果不为空则将原来的数据以原来的数据类型读    出，然后调用本字段的SetAs***设置。
//然后，如果有新的next字段，则调用该字段的ConvertToCurField函数，并且负责释放本空间，并且清0
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
	else   //最后一层转换.
	{
		if(pOldField->IsNull())
			SetNull();
		else
			SetAsString( pOldField->SetToDisplay());	

			//assert(m_nNextModifyField == -1);		
	}
}
*/
//将当前数据设置为缺省值,即将m_pDefaultValue所指向的数据拷贝到m_pValue中
void CStringField::SetDefaultValue()
{
	FieldString* pStr = (FieldString*)m_pValue;
	FieldString* pDefaultStr = (FieldString*)m_pDefaultValue;

	*pStr    = *pDefaultStr;
}

//用当前的数据设置作为缺省值,即将m_pValue所指向的数据拷贝到m_pDefaultValue中，原来数据没有销毁
bool CStringField::CopyToDefaultValue()
{
	FieldString* pStr = (FieldString*)m_pValue;
	FieldString* pDefaultStr = (FieldString*)m_pDefaultValue;

	*pDefaultStr    = *pStr;
	return true;
}


///cwh 2002-11-14
//根据当前的元素数据在标量表(m_ValueLable)中取得相应的标量值
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