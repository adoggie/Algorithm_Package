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



//返回資料類型的大小，在爲資料鏈表分配新空間時確定
//(包含指示位元和資料,string類型時爲4)
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
			//int retCode = //===temp AfxMessageBox(_T("数据太大会引起数据存储错误，确认是否要转换\nCIntField::SetAsDouble"),MB_OKCANCEL);
			//if(retCode == IDCANCEL )			
			//	return false;			
			SetNull();		
		}
        //将double(8)型转换为float(4)存入
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
		str.Remove(0x20);//除掉空格.
		str.Remove(',');
		str.MakeLower(); //转化为小写.		
		int iLength  = str.GetLength();				
		
		int iDecimalCnt  = 0;//小数点个数,最多只允许有一个,位置必须在E之前.
		int iECnt        = 0;//E的个数,最多只允许有一个(科学记数法).
		int iOtherCnt    = 0;//非0，非E,非小数点,非+-号的字符数累加,用于确定当返回0时是否真的应该是0.
		int iSignPostion = 0;//+-号的位置,0或者E的后面.  
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
			else if( !isdigit(str[i]) )//e:非0，非E,非小数点,非+-号的字符数累加.
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
		int    iTemp = int( fTemp );

		double dd = fabs(fTemp - iTemp);
		//if( dd > 1E-10)//检验是否是整数.
		//	return false;
	
		*((FieldNUM *)m_pValue) = iTemp;	
	}

	return true;
}



//返回数据大小，不包括指示位,用于对文件等读时确定下一个的位置
//lpData用于变长时，（如是字符型，指向数据）定长时为空
//##ModelId=3CF42CC30261
int CIntField::GetDataLength(LPBYTE lpData)
{
	return 4;
}



//根据lpData指向的数据，复制该数据值到本字段，
//两者有相同的数据格式，如用户调用edit，cancel操作时恢复原值
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

//把外部用户输入的数据(字符串形式)保存到字段内,如果成功.则返回true
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



//将当前数据设置为缺省值,即将m_pDefaultValue所指向的数据拷贝到m_pValue中
void CIntField::SetDefaultValue()
{
	*((FieldNUM *)m_pValue) = *((FieldNUM *)m_pDefaultValue);
	//memcpy(m_pValue,m_pDefaultValue,5);
}

//用当前的数据设置作为缺省值,即将m_pValue所指向的数据拷贝到m_pDefaultValue中，原来数据没有销毁
bool CIntField::CopyToDefaultValue()
{
	*((FieldNUM *)m_pDefaultValue) = *((FieldNUM *)m_pValue);
	//memcpy(m_pDefaultValue,m_pValue,5);
	return true;
}

//根据当前的元素数据在标量表(m_ValueLable)中取得相应的标量值
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