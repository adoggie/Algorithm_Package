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
函数名称：
int CBooleanField::GetTypeSize()
功能说明：返回数据类型的大小,在为数据链表分配新空间时确定.

详细解释：1.包含指示位和数据.    

出入参数：
[in]: 无.      
  
[out]:无.　

返回类型：int

制作：YTLI 2002/07/15

修改: 
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

//由外部负责释放返回的地址空间.
FILETIME CBooleanField::GetAsDateTime()
{
/*===uc 时间问题  统一为 ===time
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

		if(Value==_T("")||Value == _T("0") || Value == _T("false") || Value == _T("否") ||
		   Value == _T("n") || Value == _T("no")  || Value == _T("假") || Value == _T("f"))
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
函数名称：
int CBooleanField::GetDataLength(LPBYTE lpData)
功能说明：虚函數,返回数据长度,不包括指示位,用于对文件等读时确定下一个的位置.

详细解释：1.不同类型返回不同长度.       

出入参数：
[in]:1.lpData:指向数据的指针,变长时用(如字符型),定长时为空. 
  
[out]:无.

返回类型:int


制作：YTLI 2002/07/15

修改: 
************************************************************************************/
int CBooleanField::GetDataLength(LPBYTE lpData)
{
	return 1;
}






//根据lpData指向的数据，复制该数据值到本字段，
//两者有相同的数据格式，如用户调用edit，cancel操作时恢复原值
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

//把外部用户输入的数据(字符串形式)保存到字段内,如果成功.则返回true
bool CBooleanField::GetFromDisplay(CTString str)
{
	if(!m_pValue)
		return false;

	
	if(!SetAsString(str))
		return false;

	return true;

}


//将当前数据设置为缺省值,即将m_pDefaultValue所指向的数据拷贝到m_pValue中
void CBooleanField::SetDefaultValue()
{
	//assert( m_pDefaultValue && m_pValue );
	*((FieldNUM *)m_pValue) = *((FieldNUM *)m_pDefaultValue);
}

//用当前的数据设置作为缺省值,即将m_pValue所指向的数据拷贝到m_pDefaultValue中，原来数据没有销毁
bool CBooleanField::CopyToDefaultValue()
{
	//assert( m_pValue && m_pDefaultValue );
	*((FieldNUM *)m_pDefaultValue) = *((FieldNUM *)m_pValue);
	return true;
}


//根据当前的元素数据在标量表(m_ValueLable)中取得相应的标量值
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