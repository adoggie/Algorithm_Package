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
		return true;//#_修改 2002-12-2 $ 9:59:35 zlq
	else
		return false;
}

int CDoubleField::GetAsInteger()
{
	return (int)*((FieldNUM *)m_pValue);
}

CTString CDoubleField::GetAsString()
{
	////assert( m_pValue && *m_pValue == fvValid );	 //指示字节必须为1

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
{	//不可以转换.
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
		str.Remove(0x20);//除掉空格.
		str.Remove(',');
		str.MakeLower(); //转化为小写.		
		int iLength  = str.GetLength();				
		
		int iDecimalCnt  = 0;//小数点个数,最多只允许有一个,位置必须在E之前.
		int iECnt        = 0;//E的个数,最多只允许有一个(科学记数法).
		int iOtherCnt    = 0;//非0，非E,非小数点,非+-号的字符数累加,用于确定当返回0时是否真的应该是0.
		int iSignPostion = 0;//+-号的位置,最开头或者E的后面.  
		//检验:正负号只可能两种位置:字符串的开头和E的后面,E的后面必须要有数字.
		
		for(int i = 0; i<iLength; i++)
		{							
			if(str[i] == '+' ||str[i] == '-')
			{
				if(!(i == 0 || i == iSignPostion ))//检验正负号的位置.
					return false;
			}
			else if(str[i] == '.')//小数点.
			{
				if( (iSignPostion != 0) && (i >= iSignPostion) )
					return false;

				iDecimalCnt++;
			}
			else if(str[i] == 'e')//e:小写.
			{
				if(i==0)
					return false;

				iECnt++;
				iSignPostion = i + 1;
			}
			else if( !isdigit(str[i]) )//e:非数字，非E,非小数点,非+-号的字符数累加.
			{
				iOtherCnt++;
			}
		}

		if(iSignPostion == iLength)//防止输入类似'4e'的情况.
			return false;

		if(iDecimalCnt > 1 || iECnt > 1 || iOtherCnt > 0 )
			return false;

		char*  pChar = (char*)(LPCTSTR)str;

		double fTemp = atof( pChar );
		
		*((FieldNUM *)m_pValue) = fTemp;		
	}

	return true;
}

//返回数据大小，不包括指示位,用于对文件等读时确定下一个的位置
//lpData用于变长时，（如是字符型，指向数据）定长时为空
int CDoubleField::GetDataLength(LPBYTE lpData)
{
	return 8;
}



//将本字段的数据生成一个拷贝，地址空间也由本函数生成，
//并返回该地址，用于数据传递和保存旧值
/*
LPBYTE CDoubleField::SetData()
{
	LPBYTE byteRetValue = new BYTE[9];

	//assert( (m_pValue)&&(byteRetValue) );//###_Mod 2002-9-4 zlq
	
	memcpy(byteRetValue,m_pValue,9);
	return byteRetValue;
}*/

//根据lpData指向的数据，复制该数据值到本字段，
//两者有相同的数据格式，如用户调用edit，cancel操作时恢复原值
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



//把外部用户输入的数据(字符串形式)保存到字段内,如果成功,则返回true.
bool CDoubleField::GetFromDisplay(CTString str)
{	
	if(!m_pValue)
		return false;


		if(!SetAsString(str))
			return false;

	return true;
}


//将当前数据设置为缺省值,即将m_pDefaultValue所指向的数据拷贝到m_pValue中
void CDoubleField::SetDefaultValue()
{
	*((FieldNUM *)m_pValue) = *((FieldNUM *)m_pDefaultValue);
	//memcpy(m_pValue,m_pDefaultValue,9);
}

//用当前的数据设置作为缺省值,即将m_pValue所指向的数据拷贝到m_pDefaultValue中，原来数据没有销毁
bool CDoubleField::CopyToDefaultValue()
{
	*((FieldNUM *)m_pDefaultValue) = *((FieldNUM *)m_pValue);
	//memcpy(m_pDefaultValue,m_pValue,9);
	return true;
}


//根据当前的元素数据在标量表(m_ValueLable)中取得相应的标量值
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