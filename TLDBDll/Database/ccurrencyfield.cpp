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
	////assert( m_pValue && *m_pValue == fvValid );	 //指示字节必须为1
	
	return *((FieldCurrency *)m_pValue);
}

int CCurrencyField::GetAsInteger()
{
	////assert( m_pValue && *m_pValue == fvValid );	 //指示字节必须为1


	return (int)*((FieldCurrency *)m_pValue);
	
}

CTString CCurrencyField::GetAsString()
{
	////assert( m_pValue && *m_pValue == fvValid );	 //指示字节必须为1
	
	CTString strRetValue;
	CTString strFormat ;
	//strFormat.Format("%%%d.%df",m_nWidth,m_nDecimals);
	strFormat.Format("%%%d.%df",0,m_nDecimals);
	strRetValue.Format(strFormat,*((FieldNUM *)m_pValue));
	strRetValue.TrimLeft();
	/*
	if (m_nWidth >m_nTempBufLength)//#_修改 2002-12-12 $ 14:04:21 zlq
		SetTempBufLength(m_nWidth);//如果空间不够，则扩大临时缓冲区	
	gcvt( ((FieldCurrency *)m_pValue)->dwValue, m_nWidth, m_pTempBuf );
	strRetValue = _T(m_pTempBuf);	
	*/
	return strRetValue;
}

bool CCurrencyField::GetAsBool()
{
	////assert( m_pValue && *m_pValue == fvValid );	 //指示字节必须为1

	if( fabs(*((FieldCurrency *)m_pValue)) < DoubleZero )
		return false;
	
	return true;
}

FILETIME CCurrencyField::GetAsDateTime()
{	
	////assert( m_pValue && *m_pValue == fvValid );	 //指示字节必须为1

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
	////assert( m_pValue && *m_pValue == fvValid );	 //指示字节必须为1

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
//不包含货币符号 纯数字
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
/*===time  		str.Remove(0x20);//除掉空格.
		if(str.Find(m_strCurrencyType)>=0)
			str = str.Right(str.GetLength()-m_strCurrencyType.GetLength());
		str.MakeLower(); //转化为小写.	
		
		int iLength  = str.GetLength();				
		
		int iDecimalCnt  = 0;//小数点个数,最多只允许有一个,位置必须在E之前.
		int iECnt        = 0;//E的个数,最多只允许有一个(科学记数法).
		int iOtherCnt    = 0;//非0，非E,非小数点,非+-号的字符数累加,用于确定当返回0时是否真的应该是0.
		int iSignPostion = 0;//+-号的位置,最开头或者E的后面.  
		//检验:正负号只可能两种位置:字符串的开头和E的后面,E的后面必须要有数字.
		
		for(int i = 0; i<iLength; i++)
		{							
			if(str.GetAt(i) == '+' ||strGetAt(i) == '-')
			{
				if(!(i == 0 || i == iSignPostion ))//检验正负号的位置.
					return false;
			}
			else if(strGetAt(i) == '.')//小数点.
			{
				if( (iSignPostion != 0) && (i >= iSignPostion) )
					return false;

				iDecimalCnt++;
			}
			else if(strGetAt(i) == 'e')//e:小写.
			{
				if(i==0)
					return false;

				iECnt++;
				iSignPostion = i + 1;
			}
			else if( !isdigit(strGetAt(i)) )//e:非数字，非E,非小数点,非+-号的字符数累加.
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
{//不可转换.
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

//返回数据大小，不包括指示位,用于对文件等读时确定下一个的位置
//lpData用于变长时，（如是字符型，指向数据）定长时为空
//##ModelId=3CF42CC30261
int CCurrencyField::GetDataLength(LPBYTE lpData)
{
	return 10;
}




//根据lpData指向的数据，复制该数据值到本字段，
//两者有相同的数据格式，如用户调用edit，cancel操作时恢复原值

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

//把外部用户输入的数据(字符串形式)保存到字段内,如果成功.则返回true
bool CCurrencyField::GetFromDisplay(CTString str)
{
	if(!m_pValue)
		return false;
	
	if(!SetAsString(str))
		return false;		

	return true;
}



//将当前数据设置为缺省值,即将m_pDefaultValue所指向的数据拷贝到m_pValue中
void CCurrencyField::SetDefaultValue()
{
	*((FieldCurrency *)m_pValue) = *((FieldCurrency *)m_pDefaultValue);

}

//用当前的数据设置作为缺省值,即将m_pValue所指向的数据拷贝到m_pDefaultValue中，原来数据没有销毁
bool CCurrencyField::CopyToDefaultValue()
{
	*((FieldCurrency *)m_pDefaultValue) = *((FieldCurrency *)m_pValue);

	return true;
}

//设置货币类型
void CCurrencyField::SetCurrencyType(CTString strType)
{
	if(strType.GetLength()>255)
		return;
	m_strCurrencyType = strType;
}

//取得货币类型
CTString CCurrencyField::GetCurrencyType()
{
	return m_strCurrencyType;
}


/************************************************************************************
函数名称：
void CCurrencyField::LoadExt(LPBYTE lpData, int count)
功能说明：从Buffer中读取字段的扩展区信息.

详细解释：1.protected函数,供LoadFromBuffer调用.       

出入参数：
[in]: 1.lpData:扩展区的起始字节地址. 
      2.count:扩展区的长度.
  
[out]:无. 

返回类型：void

制作：YTLI 2002/12/17

修改: 
************************************************************************************/
void CCurrencyField::LoadExt(LPBYTE lpData, int count)
{	
	if(!lpData || count <= 0 )
		return;

	//此时 lpData 指向数据块1的标志位
	int iByteCnt  = 0;        //扩展区长度
	int iBlockCnt = 0;        //块的个数
	m_bIsEnum = false;        //初始化为FALSE;	
	
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

			lpData++;         //指向枚举值

			m_byteEnumData = *(lpData);          	
			
			lpData++;         //lpData指向下一数据块
		
			iBlockCnt++;
			
		}
		else if((int)*lpData == Ext_ValueLabel )
		{//值标签
			lpData += 1;   
			int wExtLength = 0;
			{
				wExtLength = *(int*)lpData;
				lpData += sizeof(int);      //指向类枚举类型的内容			
				iByteCnt += (Ext_TitleLength110 + wExtLength);
			}

			m_ValueLabel.LoadFromBuffer(lpData,wExtLength);
			lpData += wExtLength;                //lpData指向下一数据块
		
			iBlockCnt++;
		}
		else if((*lpData) == Ext_ShowCharacteristic )//需修改
		{			
			lpData += 1;      //指向显示特性的长度
			int wShowLength = 0;
			{
				wShowLength = *(int*)lpData;
				lpData += sizeof(int);      //指向显示特性的内容
				iByteCnt += ( Ext_TitleLength110 + wShowLength);
			}

			m_ShowCharacteristic.loadShowCharacter(lpData,wShowLength);
			lpData += wShowLength;
            
			iBlockCnt++;
			
		}	
		else if((*lpData) == 0x04 )//货币种类.
		{
			lpData += 1;      //指向货币种类,一个字节.
			WORD wLength = *(WORD*)lpData;//两个字节的长度.
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
	//Missing ,Mesure 放在扩展区后。
	m_byteMeasure = *lpData;
	lpData ++;
	m_pMissing->LoadFromBuffer(lpData);
	//### 2002-9-25 Eric<===
}

/************************************************************************************
函数名称：
void CCurrencyField::SaveExt(LPBYTE lpData, int count)
功能说明：将扩展区的信息存入Buffer中.

详细解释：1.protected函数,供SaveToBuffer调用.

出入参数：
[in]: 1.lpData:扩展区的起始字节地址. 
      2.count:扩展区的长度.
  
[out]:无. 

返回类型：void

制作：YTLI 2002/07/15

修改: 
************************************************************************************/

void CCurrencyField::SaveExt(LPBYTE lpData, int count)
{
	if(!lpData || count <= 0 )
		return;

	int iByteCnt  = 0;        //扩展区长度;

	if( m_bIsEnum == true && iByteCnt < count )
	{
		*lpData = Ext_Enum;//枚举
		lpData += 1;

		//存入长度值2
		{
			int wdTemp = 2;
			memcpy(lpData,&wdTemp,sizeof(int));
			lpData += sizeof(int);
			iByteCnt += Ext_TitleLength110 +2;//两个为数据长度
		}

		//存入枚举类型
		*lpData = m_byteEnumType;
		lpData += 1;

		//存入枚举值
		*lpData = m_byteEnumData;
		lpData += 1;
	
	}
	if( m_ValueLabel.m_strLabelList.GetSize()!= 0 && iByteCnt < count )
	{			
		*lpData = Ext_ValueLabel;//值标签
		lpData += 1;

		//存入长度值
		int wLength = m_ValueLabel.QueryLength();
		{
			memcpy(lpData,&wLength,sizeof(int));
			lpData += sizeof(int);	
			iByteCnt += (Ext_TitleLength110 + wLength);
		}

		//存入枚举内容
		lpData += m_ValueLabel.SaveToBuffer(lpData);
		
	}
	if(m_ShowCharacteristic.QueryLength() != 0 && iByteCnt < count )                //总是有显示特性的
	{
		*lpData = Ext_ShowCharacteristic;//标志为显示特性
		lpData += 1;

		//存入长度值
		int wLength = m_ShowCharacteristic.QueryLength();
		{
			memcpy(lpData,&wLength,sizeof(int));
			lpData += sizeof(int);	
			iByteCnt += (Ext_TitleLength110 + wLength);
		}

		//存入显示特性内容
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
函数名称：
void CCurrencyField::Clone(CField* pField)
功能说明: 数据克隆，根据传入的字段样板克隆字段的基本信息.
        
详细解释:1.对于m_ValueLabel,调用CValueLabel类的Clone()函数. 
         2.对于m_ShowCharacteristic,调用CShowCharacteristic类的Clone()函数. 
  
出入参数:
[in]: 1.pField:传入的字段样板.
 
[out]:无.

返回类型:void.

制作:YTLI      2002/07/12

修改: 
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
函数名称：
int CCurrencyField::SaveToBuffer(LPBYTE lpData)
功能说明：将字段信息按文件格式存入Buffer中.

详细解释：1.内部调用SaveExt()实现扩展区的保存.
          2.返回保存的字节数.
		  3.如果传入指针无效,则返回-1.

出入参数：
[in]: 1.lpData:将字段保存入Buffer的首地址.      
  
[out]:无.　

返回类型：int

制作：YTLI 2002/07/15

修改: 
************************************************************************************/
/*
int CCurrencyField::SaveToBuffer(LPBYTE lpData)
{
	if(!lpData)
		return -1;

	int iRetVal = (int)lpData; //返回值，保存的字节数.

	*lpData = (BYTE)GetFieldType();
	lpData += 1;               //指向该字段内容长度.

	//WORD wTemp;
	memcpy( lpData, &m_nWidth, 2 );
	lpData += 2;               //指向该字段小数的长度.

	*lpData = (BYTE)m_nDecimals;        
	lpData += 1;               //指向该字段名.
	//#_S 2003-3-31 $ 16 zlq 扩展字段名长度**************************
	//得到字段名
	{
		int iFieldNameLength = m_sName.GetLength();
		*lpData = iFieldNameLength;
		lpData++;//指针下移 1

		for(int i = 0; i<iFieldNameLength; i++)
		{
			if( i <  iFieldNameLength )        
				*lpData = m_sName.GetAt(i);                
		
			lpData += 1;
		}

	}
	//此时 lpData 指向 Label 长度***************************

	int iFieldLabelLength = m_sLabel.GetLength();
	*lpData = iFieldLabelLength;
	lpData += 1;               //指向该字段 Label 的内容
	for(int j = 0; j<iFieldLabelLength; j++)
	{
		*lpData = m_sLabel.GetAt(j);
		lpData += 1;
	}
	//此时 lpData 指向该字段扩展区的长度*****************************

	int wdExtLength = 0;
	int  iExtCnt     = 0; //扩展区的块数.
	int nTitleLength = 0;

		nTitleLength = Ext_TitleLength110;

	if(m_bIsEnum)
	{
		//===temp TRACE("原始wdExtLength %d\n",wdExtLength);
		wdExtLength += nTitleLength + 2;//枚举
		iExtCnt++;
	}
	
	if( m_ValueLabel.m_strLabelList.GetCount() != 0 )
	{
		//===temp TRACE("m_ValueLabel %d\n",m_ValueLabel.QueryLength());
		wdExtLength += (m_ValueLabel.QueryLength() + nTitleLength);//值标签.
		iExtCnt++;
	}
	
	if( m_ShowCharacteristic.QueryLength() != 0 )
	{
		//===temp TRACE("m_ShowCharacteristic %d\n",m_ShowCharacteristic.QueryLength());
		wdExtLength += (m_ShowCharacteristic.QueryLength() + nTitleLength);//显示特性
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
		lpData += 4;               //指向该字段扩展区的块数
	}

	*lpData = iExtCnt;        
	lpData += 1;               //指向该字段扩展区

	SaveExt(lpData,wdExtLength);

	iRetVal = (int)lpData - iRetVal + wdExtLength;        
	return iRetVal;
}*/

//根据当前的元素数据在标量表(m_ValueLable)中取得相应的标量值
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