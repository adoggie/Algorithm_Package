// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "cfield.h"
#include "cfields.h"
#include "cdataset.h"
#include "cvaluelabel.h"
#include  <math.h>
#include "cpages.h"

CValueLabel::CValueLabel()
{
	m_bIsNum = true;
}

CValueLabel::~CValueLabel()
{
	POSITION posData = m_DataList.GetHeadPosition();	
	LPBYTE pOldValue = m_pField->m_pValue;
	while(posData)
	{
		m_pField->m_pValue = (LPBYTE)m_DataList.GetNext( posData );

		if (m_bIsNum ==false)
		{
			FieldString* pStr = (FieldString*)m_pField->m_pValue ;
			delete pStr;
		}
		else
		{
			if( m_pField->m_pValue )
				delete[] m_pField->m_pValue;

		}
		//m_pField->RealDeleteData();	
	}
	m_DataList.RemoveAll();
	m_strLabelList.RemoveAll();
	m_pField->m_pValue = pOldValue;
}

/************************************************************************************
函数名称：int CValueLabel::LoadFromBuffer(LPBYTE lpData, int count)
功能说明：把缓冲区以文件格式描述的值标签内容解释后装入链表中
          1.值装入m_DataList
		  2.值的描述label装入m_strLabelList
	注:调用这个函数时,指针已指向类枚举的项数字节,要装入的为类枚举项数,类枚举值及其描述

详细解释：

出入参数：
    [in]:1.lpData指向值标签项数字节 2.count以文件格式描述的值标签内容长度
	[out]:返回处理的字节数
返回类型：int
    
制作：

修改: 
************************************************************************************/
int CValueLabel::LoadFromBuffer(LPBYTE lpData, int count)
{
	if (m_pField->GetFieldType() == fString)
		m_bIsNum = false;
	else
		m_bIsNum = true;

	//保存m_pValue指针
	LPBYTE oldValue = m_pField->m_pValue;

    LPBYTE lpDataTemp = lpData;
	//读入第一字节(类枚举个数)
	//#_S 2003-3-31 $ 16 zlq 扩展字段名长度
	//得到字段名
	int ItemCount = 0;//标签个数

	{//最新版本 4个字节
		int *pInt = (int*)lpDataTemp;
		ItemCount = *pInt;
		lpDataTemp = lpDataTemp+4;
	}
	

	for (int i=0;i<ItemCount;i++)
	{
		//LPBYTE lpDataTemp2 = lpDataTemp + 1;
		m_pField->RealNewData();   //生成一个新的数据指针(m_pValue);
		m_pField->LoadDataFromBuffer(lpDataTemp); //从缓冲区中调出来，lpDataTemp自动后移
		m_DataList.AddTail(m_pField->m_pValue);     //然后由调用者添加在链表中

		//解释部分处理: 长度 + 内容
		int  nCntChar = (int)*lpDataTemp;//读入解释字符串的长度
		lpDataTemp++;
		//lpDataTemp = ++lpDataTemp2;
		CTString str;

		char *pbb = new char[nCntChar+1];
		memset(pbb,0,nCntChar+1);
		memcpy(pbb,lpDataTemp,nCntChar);
		str = pbb;
		delete[] pbb;
		/*
 		LPTSTR pTemp = str.GetBuffer(nCntChar+1);
		CopyMemory(pTemp,lpDataTemp,nCntChar);
		pTemp[nCntChar] = '\0';
		str.ReleaseBuffer( );
		*/

		m_strLabelList.Add(str);
		lpDataTemp=lpDataTemp + nCntChar ;
	}

    //assert(lpDataTemp-lpData == count);        //断言看是否处理完的字节数与传入字节数的一致
    //恢复原来的m_pValue;
	m_pField->m_pValue = oldValue;

	return count;      //返回处理完的字节数
}

//##ModelId=3CF5936302DB
/************************************************************************************
函数名称：int CValueLabel::GetCount()
功能说明：取得值的个数

详细解释：

出入参数：
    [in]:无
	[out]:返回值的个数
返回类型：int
    
制作：

修改: 
************************************************************************************/
int CValueLabel::GetCount()
{
	return m_DataList.GetCount();
}

//##ModelId=3CFB07A6024B
/************************************************************************************
函数名称：int CValueLabel::SaveToBuffer(LPBYTE lpData)
功能说明：为函数LoadFromBuffer的逆过程，返回值为保存的字节数

详细解释：

出入参数：
    [in]: lpData 以文件格式保存值标签的缓冲区指针
	[out]:返回保存的字节数
返回类型：int
    
制作：

修改: 
************************************************************************************/
int CValueLabel::SaveToBuffer(LPBYTE lpData)
{
	//保存m_pValue指针
	LPBYTE oldValue = m_pField->m_pValue;

	LPBYTE lpDataTemp = lpData;
	int nItem = m_DataList.GetCount();
	
	//写入第一个字节(项目数);
	//读入第一字节(类枚举个数)
	//#_S 2003-3-31 $ 16 zlq 扩展字段名长度
	//得到字段名
	{//最新版本保存
		*((int*)lpDataTemp )= nItem;
		lpDataTemp = lpDataTemp+4;	
	}

    //int nTypeSize = m_pField->GetTypeSize();

	POSITION posData;
	posData = m_DataList.GetHeadPosition();
 	int posStr = m_strLabelList.GetSize();
	int nPos = 0;
	while(posData !=NULL &&  nPos < posStr)
	{
		//LPBYTE lpDataTemp2 = lpDataTemp + 1;
		//从链表中取得Value指针
		m_pField->m_pValue = (LPBYTE)m_DataList.GetNext( posData );
		
		//保存Value值为文件中的存储格式
		m_pField -> SaveDataToBuffer( lpDataTemp);
		//lpDataTemp = lpDataTemp2;

		CTString str = m_strLabelList.GetAt(nPos);
		BYTE nStrSize = (BYTE)str.GetLength();
		*lpDataTemp =nStrSize;       //写入解释Value的字符串的长度
		lpDataTemp++;
		//保存解释Value的字符串
		memcpy(lpDataTemp, str, nStrSize);
		lpDataTemp += nStrSize;
		nPos++;
	}

	//恢复原来的m_pValue指针
    m_pField->m_pValue=oldValue;
	
	return (lpDataTemp - lpData);
}
/************************************************************************************
函数名称：int CValueLabel::QueryLength()
功能说明：取得值标签保存时的长度

详细解释：

出入参数：
    [in]:
	[out]:返回值标签保存时的长度
返回类型：int
    
制作：

修改: 
************************************************************************************/
int CValueLabel::QueryLength()
{
	int nLength =0;
	//得到项目数
	int nItem = m_strLabelList.GetSize();
	//累加项目数占用的一个字节
	
	//#_S 2003-3-31 $ 16 zlq 
	nLength = nLength+4;

	POSITION posData;
	posData = m_DataList.GetHeadPosition();
 	int posStr = m_strLabelList.GetSize();
	LPBYTE pOldValue = m_pField->m_pValue;
	int nPos = 0;
	while(posData != NULL && nPos < posStr)
	{
		//累加valuelabel长度
		m_pField->m_pValue = (LPBYTE)m_DataList.GetNext( posData );
		nLength += m_pField->QuerySaveLength();
		//累加上解释字符串长度及保存长度的一个字节
		CTString str = m_strLabelList.GetAt(nPos);
		nLength += str.GetLength()+1;
		nPos++;
	}
	m_pField->m_pValue = pOldValue;
	return nLength;
}
/************************************************************************************
函数名称：void CValueLabel::Clone(CValueLabel *pValueLabel)
功能说明：把传入的值标签的基本信息克隆到当前的值标签中

详细解释：

出入参数：
    [in]:pValueLabel 为克隆的pValueLabel指针
	[out]:
返回类型：void
    
制作：

修改: 
************************************************************************************/

void CValueLabel::Clone(CValueLabel *pValueLabel)
{
	if (m_pField->GetFieldType() == fString)
		m_bIsNum = false;
	else
		m_bIsNum = true;

	LPBYTE pOldValue = m_pField->m_pValue;

	POSITION posData = m_DataList.GetHeadPosition();	
	
	while(posData)
	{
		m_pField->m_pValue = (LPBYTE)m_DataList.GetNext( posData );
		m_pField->RealDeleteData();	
	}
	m_DataList.RemoveAll();
	m_strLabelList.RemoveAll();

	posData = pValueLabel->m_DataList.GetHeadPosition();
    int posStr  = pValueLabel->m_strLabelList.GetSize();
    int nPos = 0;
	while(posData != NULL && nPos < posStr)
	{
		//从链表中取得Value指针
		LPBYTE pData = (LPBYTE)pValueLabel->m_DataList.GetNext( posData );
		//LPBYTE pByte = m_pField->SetData();
		m_pField->RealNewData();//###_Mod 2002-9-4 zlq
		m_pField->GetData(pData);
		//delete [] pByte;
		
		m_DataList.AddTail(m_pField->m_pValue);

        //取得label字符串然后添加在当前对象中相应的链表中
		CTString str = pValueLabel->m_strLabelList.GetAt(nPos);
		m_strLabelList.Add(str);
		nPos++;
	}
	//assert(posData == NULL && posStr == NULL);
	m_pField->m_pValue = pOldValue;
}

//从零开始
void CValueLabel::SetCurPos(int nIndex)
{
	m_nCurPos = nIndex;
}

CTString CValueLabel::GetExplainString()
{
// 	POSITION pos = m_strLabelList.FindIndex(m_nCurPos);
	return m_strLabelList.GetAt(m_nCurPos);
}

void CValueLabel::SetExplainString(CTString str)
{
//	POSITION pos = m_strLabelList.FindIndex(m_nCurPos);
	m_strLabelList.InsertAt(m_nCurPos,str);
	m_strLabelList.GetAt(m_nCurPos);
	//assert(pos);

	m_strLabelList.RemoveAt(m_nCurPos);
}

bool CValueLabel::GetAsBool()
{
	LPBYTE lpByte = m_pField->m_pValue;

 	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);

	bool bRetVal = m_pField->GetAsBool();

	m_pField->m_pValue = lpByte;

	if (bRetVal)
		return true;
	else
		return false;
}

FieldCurrency CValueLabel::GetAsCurrency()
{
	LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);

	FieldCurrency RetVal = m_pField->GetAsCurrency();

	m_pField->m_pValue = lpByte;

	return RetVal;	
}

FILETIME CValueLabel::GetAsDateTime()
{
	LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);

	FILETIME pRetVal = m_pField->GetAsDateTime();

	m_pField->m_pValue = lpByte;

	return pRetVal;	
}

CTString  CValueLabel::GetAsString()
{
    LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);

	CTString strRetVal = m_pField->GetAsString();

	m_pField->m_pValue = lpByte;

	return strRetVal;	
}

double CValueLabel::GetAsDouble()
{
	LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);

	double dRetVal = m_pField->GetAsDouble();

	m_pField->m_pValue = lpByte;

	return dRetVal;	
}

int CValueLabel::GetAsInteger()
{
    LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);

	int iRetVal = m_pField->GetAsInteger();

	m_pField->m_pValue = lpByte;

	return iRetVal;	
}

bool CValueLabel::SetAsDouble(double Value)
{
	LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);
	m_pField->SetAsDouble(Value);

	m_pField->m_pValue = lpByte;

	return true;
}

bool CValueLabel::SetAsInteger(int Value)
{
	LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);
	m_pField->SetAsInteger(Value);

	m_pField->m_pValue = lpByte;

	return true;
}

bool CValueLabel::SetAsString(CTString Value)
{
	LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);
	m_pField->SetAsString(Value);

	m_pField->m_pValue = lpByte;

	return true;
}

bool CValueLabel::SetAsDateTime(FILETIME* Value)
{
	LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);
	m_pField->SetAsDateTime(Value);

	m_pField->m_pValue = lpByte;

	return true;
}

bool CValueLabel::SetAsCurrency(FieldCurrency Value)
{	
	LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);
	m_pField->SetAsCurrency(Value);

	m_pField->m_pValue = lpByte;

	return true;
}

bool CValueLabel::SetAsBool(bool Value)
{
	LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(m_nCurPos);
	m_pField->m_pValue = (LPBYTE)m_DataList.GetAt(pos);
	m_pField->SetAsBool(Value);

	m_pField->m_pValue = lpByte;

	return true;
}


//### 2002-11-11 Eric ======>
/************************************************************************************
函数名称：

功能说明：

详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2002-11-11
修改:

************************************************************************************/
bool CValueLabel::AddValue(CTString Value,CTString strExplain)
{
	if (m_pField->GetFieldType() == fString)
		m_bIsNum = false;
	else
		m_bIsNum = true;

	LPBYTE lpByte = m_pField->m_pValue;

	m_pField->RealNewData();
	CPage*pOldDataSet = m_pField->m_pFields->m_pDataSet;
	m_pField->m_pFields->m_pDataSet = NULL;

	if(m_pField->SetAsString(Value)==false ||IsValueExist(m_pField->m_pValue))
	{
		m_pField->RealDeleteData();
		m_pField->m_pValue = lpByte;
		m_pField->m_pFields->m_pDataSet = pOldDataSet;
		return false;
	}

	m_pField->m_pFields->m_pDataSet = pOldDataSet;
	m_DataList.AddTail(m_pField->m_pValue);
	m_strLabelList.Add(strExplain);

	m_pField->m_pValue = lpByte;
	return true;
}

/************************************************************************************
函数名称：

功能说明：

详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2002-11-11
修改:

************************************************************************************/
bool CValueLabel::AddValue(int Value,CTString strExplain)
{
	if (m_pField->GetFieldType() == fString)
		m_bIsNum = false;
	else
		m_bIsNum = true;

	LPBYTE lpByte = m_pField->m_pValue;

	m_pField->RealNewData();
	CPage*pOldDataSet = m_pField->m_pFields->m_pDataSet;
	m_pField->m_pFields->m_pDataSet = NULL;

	if(m_pField->SetAsInteger(Value)==false ||IsValueExist(m_pField->m_pValue))
	{
		m_pField->RealDeleteData();
		m_pField->m_pValue = lpByte;
		m_pField->m_pFields->m_pDataSet = pOldDataSet;
		return false;
	}

	m_pField->m_pFields->m_pDataSet = pOldDataSet;
	m_DataList.AddTail(m_pField->m_pValue);
	m_strLabelList.Add(strExplain);

	m_pField->m_pValue = lpByte;
	return true;
}
 
/************************************************************************************
函数名称：

功能说明：

详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2002-11-11
修改:

************************************************************************************/
bool CValueLabel::AddValue(double Value,CTString strExplain)
{
	if (m_pField->GetFieldType() == fString)
		m_bIsNum = false;
	else
		m_bIsNum = true;

	LPBYTE lpByte = m_pField->m_pValue;

	m_pField->RealNewData();
	CPage*pOldDataSet = m_pField->m_pFields->m_pDataSet;
	m_pField->m_pFields->m_pDataSet = NULL;

	if(m_pField->SetAsDouble(Value)==false ||IsValueExist(m_pField->m_pValue))
	{
		m_pField->RealDeleteData();
		m_pField->m_pValue = lpByte;
		m_pField->m_pFields->m_pDataSet = pOldDataSet;
		return false;
	}
	m_pField->m_pFields->m_pDataSet = pOldDataSet;
	m_DataList.AddTail(m_pField->m_pValue);
	m_strLabelList.Add(strExplain);

	m_pField->m_pValue = lpByte;
	return true;
}
 
/************************************************************************************
函数名称：

功能说明：

详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2002-11-11
修改:

************************************************************************************/
bool CValueLabel::AddValue(FILETIME* Value,CTString strExplain)
{
	if (m_pField->GetFieldType() == fString)
		m_bIsNum = false;
	else
		m_bIsNum = true;

	LPBYTE lpByte = m_pField->m_pValue;

	m_pField->RealNewData();
	CPage*pOldDataSet = m_pField->m_pFields->m_pDataSet;
	m_pField->m_pFields->m_pDataSet = NULL;

	if(m_pField->SetAsDateTime(Value)==false ||IsValueExist(m_pField->m_pValue))
	{
		m_pField->RealDeleteData();
		m_pField->m_pValue = lpByte;
		m_pField->m_pFields->m_pDataSet = pOldDataSet;
		return false;
	}
	m_pField->m_pFields->m_pDataSet = pOldDataSet;
	m_DataList.AddTail(m_pField->m_pValue);
	m_strLabelList.Add(strExplain);

	m_pField->m_pValue = lpByte;
	return true;
}

/************************************************************************************
函数名称：

功能说明：

详细解释：

出入参数：
	[in,out]
	nIndex  输入的索引位置,从0开始
返回类型：

制作：
	Eric 2002-11-11
修改:

************************************************************************************/
bool CValueLabel::Delete(int nIndex)
{
	if(nIndex<0 || nIndex> m_DataList.GetCount())
		return false;
	LPBYTE lpByte = m_pField->m_pValue;

	POSITION pos = m_DataList.FindIndex(nIndex);
	LPBYTE lpTemp = (LPBYTE)(m_DataList.GetAt(pos));
	delete lpTemp;
	m_DataList.RemoveAt(pos);
// 	pos = m_strLabelList.FindIndex(nIndex);
	m_strLabelList.RemoveAt(nIndex);

	m_pField->m_pValue = lpByte;
	return true;
}

/************************************************************************************
函数名称：

功能说明：

详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2002-11-11
	修改:
	
************************************************************************************/

bool CValueLabel::IsValueExist(LPBYTE lpData)
{				 
	double		 fValue,fTemp;
	CTString	     strValue,strTemp;
	int		 	 nValue;
	COleDateTime tValue,tTemp;
	
	LPBYTE lpByte = m_pField->m_pValue;
	
	m_pField->m_pValue = lpData;
	switch (m_pField->GetFieldType())
	{//#_修改 2002-12-2 $ 16:01:13 zlq ===temp 
	case fDouble:
		fValue = m_pField->GetAsDouble();
		break;
	case fString :
		strValue = m_pField->GetAsString();
		break;
	case fInt :
		nValue = m_pField->GetAsInteger();
		break;
	case fDate :
/*===time  		tValue = COleDateTime(m_pField->GetAsDateTime());   */
		break;
	case fCurrency :
	case fBoolean :
	default:
		break;
	}
	
	
	
	
	
	

	POSITION pos = m_DataList.GetHeadPosition();
	for(int i = 0;i<m_DataList.GetCount();i++)
	{
		m_pField->m_pValue = (LPBYTE)m_DataList.GetNext(pos);

		switch (m_pField->GetFieldType())
		{
		case fString :
			strTemp = m_pField->GetAsString();
			if(strTemp == strValue)
			{
				m_pField->m_pValue = lpByte;
				return true;
			}
			break;
		case fInt :
			fTemp = m_pField->GetAsInteger();
			if(fTemp == nValue)
			{
				m_pField->m_pValue = lpByte;
				return true;
			}
			break;
		case fDate :
/*===time  			tTemp = COleDateTime(m_pField->GetAsDateTime());
			if(tTemp == tValue)
			{
				m_pField->m_pValue = lpByte;
				return true;
			}   */
			break;
		case fDouble:
			fTemp = m_pField->GetAsDouble();
			if(fabs(fTemp - fValue) <DoubleZero )//to be replaced ### 2002-11-11 Eric
			{
				m_pField->m_pValue = lpByte;
				return true;
			}
			break;
		case fCurrency :

		case fBoolean :
		default:
			break;
		}
	}

	m_pField->m_pValue = lpByte;

	return false;
}
//### 2002-11-11 Eric<======

//要添加的标签值是否有效
bool CValueLabel::IsValid(CTString Value)
{
	LPBYTE lpByte = m_pField->m_pValue;//保存原有数据

	m_pField->RealNewData();
	CPage*pOldDataSet = m_pField->m_pFields->m_pDataSet;
	m_pField->m_pFields->m_pDataSet = NULL;
	bool bIsValid = true;
	if(m_pField->SetAsString(Value)==false )
	{
		bIsValid = false;
	}

	m_pField->RealDeleteData();
	m_pField->m_pValue = lpByte;//恢复原有数据
	m_pField->m_pFields->m_pDataSet = pOldDataSet;
	return bIsValid ;
}

void CValueLabel::SetOwner(CField *pField)
{
	m_pField = pField;

}
