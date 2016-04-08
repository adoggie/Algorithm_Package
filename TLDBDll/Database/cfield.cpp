// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "cfield.h"
#include "cfields.h"


#include "cpage.h"
#include "cpages.h"



///const LPBYTE CField::m_pValue = 0;
CField::CField()
{
	m_sLabel = _T("");
	m_sName  = _T("");	
	
	m_pValue     = 0;	
	m_pOldValue  = 0;	
	m_pFields    = 0;	
	m_nExtLength = 0;
	m_nExtCount	 = 0;
	m_nWidth	 = 8;
	m_nDecimals	 = 2; 

	m_pDefaultValue = 0;	
	m_byteEnumType  = 0xFF; 
	m_byteEnumData  = ddmmmyyyy_line;
	m_bIsEnum	    = true;
	m_bHasDefault   = false;

	//m_ValueLabel.m_pField = this;
	m_ValueLabel.SetOwner(this);

	m_nNextModifyField = -1;
	m_pMissing = new CMissing(this);
	m_byteMeasure = msOrdinal;

	m_nTempBufLength = 100;
	m_pTempBuf = new char[m_nTempBufLength+2];
}

CField::~CField()
{
	if(m_pMissing)
		delete m_pMissing;
	m_pMissing = 0;

	delete []m_pTempBuf;
}

/************************************************************************************
函数名称：
CFieldType CField::GetFieldType()
功能说明：虚函數,返回字段类型.

详细解释：1.子类实现.      

出入参数：
[in]: 无
  
[out]:无　

返回类型：CFieldType

制作：YTLI 2002/07/12

修改: 
************************************************************************************/
//CFieldType CField::GetFieldType()
//{
	//子类实现
//	return fDouble;
//}

/************************************************************************************
函数名称：
int CField::LoadFromBuffer(LPBYTE lpData)
功能说明：虚函數,从传入的lpData地址开始读取一个字段的所有信息,包括扩展区信息.

详细解释：1.从传入的lpData地址开始对各个字节解释.
          2.扩展区部分调用LoadExt()来实现.
		  3.返回读取的字节数.
		  4.如果传入指针无效,则返回-1.

出入参数：
[in]: 1.lpData:开始读取的地址.
  
[out]:无　

返回类型：int

制作：YTLI 2002/07/15

修改: 
************************************************************************************/
int CField::LoadFromBuffer(LPBYTE lpData)
{	
	if(!lpData)
		return -1;
	
	int   iRetVal   = int(lpData);  

	int   iLength   = 0;          
	LPBYTE pCharTemp = NULL;       //指向根据长度读取相应的内容
	
	lpData++;                     //跳过'字段类型',lpData指向'字段长度'
	
	//得到该字段名的长度
	m_nWidth = *(WORD*)lpData;

	lpData += 2;

    //得到该字段的小数位数
	m_nDecimals = (int)*lpData;
	
	lpData++;
	//#_S 2003-3-31 $ 16 zlq 扩展字段名长度
	//得到字段名
	{//肯定是1.2版
		int nFieldNameLength = (int)*lpData;//第一个字节为字段名称长度
		lpData++;//指针下移动 1
		pCharTemp = new BYTE[nFieldNameLength+1];
		
		if( pCharTemp == NULL )
			return 0;
		memcpy( pCharTemp, lpData, nFieldNameLength );
		pCharTemp[nFieldNameLength] = '\0';
		m_sName = _T(pCharTemp);
		m_sName.TrimRight(0x20);  //去掉结尾的空格
		delete []pCharTemp;
		lpData += nFieldNameLength;//指针下移动 nFieldNameLength
	}

	//得到该字段Label内容
	iLength = (int)*lpData;   //该字段Label长度

	lpData++;                 //指向该字段Label内容

	pCharTemp = new BYTE[iLength+1];

	if( pCharTemp == NULL )
		return 0;  
	memcpy( pCharTemp, lpData, iLength );
	pCharTemp[iLength] = '\0';
	m_sLabel = _T(pCharTemp );
	delete []pCharTemp;
	lpData += iLength;        //指向该字段扩展区长度

	//得到该字段扩展区长度
	{
		m_nExtLength = *(int*)lpData;
		lpData += 4;              //指向该字段'扩展区数据块数量'
	}
	
	m_nExtCount = (int)*lpData;                  //得到扩展区数据块数量

	lpData += 1;              //指向该字段扩展区

	//得到该字段扩展区详细信息
	LoadExt( lpData, m_nExtLength );
	
	lpData += m_nExtLength;        //指向该字段扩展区尾

	iRetVal = (int)lpData - iRetVal;
	return iRetVal;
}

/************************************************************************************
函数名称：
bool CField::LoadDataFromBuffer(LPBYTE& lpIndicate, LPBYTE& lpData)
功能说明：虚函數,读取一条记录某一字段的数据,可以用来读取值标签.

详细解释：1.子类实现.      

出入参数：
[in]: 1.lpIndicate:指示字节地址.
      2.lpData    :数据地址.
  
[out]:1.lpIndicate:下一数据块的指示字节地址.
      2.lpData    :下一数据块的数据地址.　

返回类型：bool

制作：YTLI 2002/07/12

修改: 
************************************************************************************/
void CField::LoadDataFromBuffer( LPBYTE& lpData)
{	
	if(m_pValue==NULL)
	{//旧版本
		if(m_nNextModifyField == -1)
		{
			NewData();
			LoadDataFromBuffer(lpData);			
			DeleteData();
		}
		else
		{
			if(m_pFields->IsConvertToLastVersion(this))
			{
				NewData();
				LoadDataFromBuffer(lpData);
				CField* pNewField = m_pFields->m_FieldVersion.GetFieldByAbsolutePos(m_nNextModifyField);
				pNewField->ConvertToCurField(this); 
				DeleteData();
			}
		}
	}
	else
	{//当前最新
		if (GetFieldType() == fString)
		{
			FieldString* pStr = (FieldString*)m_pValue;
			char *pBuf = new char[m_nWidth+2];
			memset(pBuf,0,m_nWidth+2);
			memcpy(pBuf,lpData,m_nWidth);
			*pStr= pBuf;
			/*
  			char* lpstr = pStr->GetBuffer(m_nWidth+2);
			memcpy(lpstr,lpData,m_nWidth);
			*(lpstr+m_nWidth)= 0;
			*(lpstr+m_nWidth+1)= 0;
			pStr->ReleaseBuffer();	
			*/
			//pStr->TrimLeft();//,左边空格保留
			pStr->TrimRight();   

			
			lpData =  lpData + Align4(m_nWidth);
		}
		else
		{
			memcpy( &*((FieldNUM *)m_pValue), lpData, sizeof(FieldNUM) );	//ytli Modify 2002/09/04
			lpData =  lpData + sizeof(FieldNUM);
		}
		
	}
}

/************************************************************************************
函数名称：
CTString CField::GetAsString()
功能说明：虚函數,将m_pValve所指向的数据以CString型返回.

详细解释：1.如果子类没有实现,则仅实现错误提示,以示不能转换.
         2.如果子类实现,将调用子类的方法.

出入参数：
[in]: 无.      
  
[out]:无.　

返回类型：CString

制作：YTLI 2002/07/15

修改: 
************************************************************************************/
CTString CField::GetAsString()
{
	//throw _T("不能提供类型转换");
	//assert(0);
	////===temp AfxMessageBox(_T("不能提供转换\nCField::GetAsString将返回空值"));
	return "";
}

bool CField::GetAsBool()
{
	//throw _T("不能提供类型转换");
	//assert(0);
	//	//===temp AfxMessageBox(_T("不能提供转换\nCField::GetAsBool将返回FALSE"));
	return false;
}
/************************************************************************************
函数名称：
bool CField::SetAsString(CTString Value)
功能说明：虚函數,将m_pValve所指向的数据修改为传入的CString型数据.

详细解释：1.如果子类没有实现,则仅实现错误提示,以示不能转换.
         1.如果子类实现,将调用子类的方法.

出入参数：
[in]: 无.      
  
[out]:无.　

返回类型：bool

制作：YTLI 2002/07/15

修改: 
************************************************************************************/
bool CField::SetAsString(CTString Value)
{	
	if(m_nNextModifyField == -1)
		return false;
	else
	{
		if(m_pFields->IsConvertToLastVersion(this))
		{
			NewData();
			SetAsString(Value);//###_Mod 2002-9-26 zlq
			CField* pNewField = m_pFields->m_FieldVersion.GetFieldByAbsolutePos(m_nNextModifyField);			
			pNewField ->ConvertToCurField(this);
			DeleteData();
		}
	}
	return true;
}

/************************************************************************************
函数名称：
int CField::GetAsInteger()
功能说明：虚函數,将m_pValve所指向的数据以int型返回.

详细解释：1.如果子类没有实现,则仅实现错误提示,以示不能转换.
         1.如果子类实现,将调用子类的方法.

出入参数：
[in]:无.      
  
[out]:无.

返回类型:int


制作：YTLI 2002/07/15

修改: 
************************************************************************************/
int CField::GetAsInteger()
{
	//throw _T("不能提供类型转换");
	//assert(0);
	////===temp AfxMessageBox(_T("不能提供转换\nCField::GetAsInteger将返回空值"));
	return 0;
}

/************************************************************************************
函数名称：
bool CField::SetAsInteger(int Value)
功能说明：虚函數,将m_pValve所指向的数据修改为传入的int型数据.

详细解释：1.如果子类没有实现,则仅实现错误提示,以示不能转换.
         2.如果子类实现,将调用子类的方法.

出入参数：
[in]: 无.      
  
[out]:无.　

返回类型：bool

制作：YTLI 2002/07/15

修改: 
************************************************************************************/
bool CField::SetAsInteger(int Value)
{
	if(m_nNextModifyField == -1)
		return false;
	else
	{
		if(m_pFields->IsConvertToLastVersion(this))
		{
			NewData();
			SetAsInteger(Value);
			CField* pNewField = m_pFields->m_FieldVersion.GetFieldByAbsolutePos(m_nNextModifyField);			
			pNewField ->ConvertToCurField(this);
			DeleteData();
		}
	}
	return true;
}

/************************************************************************************
函数名称：
int CField::GetDataLength(LPBYTE lpData)
功能说明：虚函數,返回数据长度,不包括指示位,用于对文件等读时确定下一个的位置.

详细解释：1.子类实现,不同类型返回不同长度.       

出入参数：
[in]:1.lpData:指向数据的指针,变长时用(如字符型),定长时为空. 
  
[out]:无.

返回类型:int


制作：YTLI 2002/07/15

修改: 
************************************************************************************/
int CField::GetDataLength(LPBYTE lpData)
{
	//子类实现
	return 0;
}

/************************************************************************************
函数名称：

功能说明：虚函數,将m_pValve所指向的数据以FILETIME*型返回.

详细解释：1.如果子类没有实现,则仅实现错误提示,以示不能转换.
         2.如果子类实现,将调用子类的方法.

出入参数：
[in]:无.      
  
[out]:无.

返回类型:FILETIME *


制作：YTLI 2002/07/15

修改: 
************************************************************************************/
FILETIME  CField::GetAsDateTime()
{
	//throw _T("不能提供类型转换");
	//assert(0);
	//	//===temp AfxMessageBox(_T("不能提供转换\nCField::GetAsDateTime将返回空值"));
	FILETIME ft;
	return ft;
}

/************************************************************************************
函数名称：
bool CField::SetAsDateTime(FILETIME *Value)
功能说明：虚函數,将m_pValve所指向的数据修改为传入的FILETIME*型数据.

详细解释：1.如果子类没有实现,则仅实现错误提示,以示不能转换.
         2.如果子类实现,将调用子类的方法.

出入参数：
[in]: 无.      
  
[out]:无.　

返回类型：bool

制作：YTLI 2002/07/15

修改: 
************************************************************************************/
bool CField::SetAsDateTime(FILETIME *Value)
{
	if(m_nNextModifyField == -1)
		return false;
	else
	{
		if(m_pFields->IsConvertToLastVersion(this))
		{
			NewData();
			SetAsDateTime(Value);//###_Mod 2002-9-26 zlq
			CField* pNewField = m_pFields->m_FieldVersion.GetFieldByAbsolutePos(m_nNextModifyField);			
			pNewField ->ConvertToCurField(this);
			DeleteData();
		}
	}
	return true;
}

/************************************************************************************
函数名称：

功能说明：虚函數,将m_pValve所指向的数据以FieldCurrency型返回.

详细解释：1.如果子类没有实现,则仅实现错误提示,以示不能转换.
         2.如果子类实现,将调用子类的方法.

出入参数：
[in]:无.      
  
[out]:无.

返回类型:FieldCurrency


制作：YTLI 2002/07/15

修改: 
************************************************************************************/
FieldCurrency CField::GetAsCurrency()
{
	//throw _T("不能提供类型转换");
	//assert(0);
	//	//===temp AfxMessageBox(_T("不能提供转换\nCField::GetAsCurrency将返回空值"));
	FieldCurrency b;
	//b.Indicate = fvNULL;
	return b;
}

/************************************************************************************
函数名称：

功能说明：虚函數,将m_pValve所指向的数据修改为传入的FieldCurrency型数据.

详细解释：1.如果子类没有实现,则仅实现错误提示,以示不能转换.
         2.如果子类实现,将调用子类的方法.

出入参数：
[in]: 无.      
  
[out]:无.　

返回类型：bool

制作：YTLI 2002/07/15

修改: 
************************************************************************************/
bool CField::SetAsCurrency(FieldCurrency Value)
{	
	if(m_nNextModifyField == -1)
		return false;
	else
	{
		if(m_pFields->IsConvertToLastVersion(this))
		{
			NewData();
			SetAsCurrency(Value);
			CField* pNewField = m_pFields->m_FieldVersion.GetFieldByAbsolutePos(m_nNextModifyField);			
			pNewField ->ConvertToCurField(this);

			DeleteData();
		}
	}
	return true;
}

bool CField::SetAsBool(bool Value)
{		
	if(m_nNextModifyField == -1)
		return false;
	else
	{
		if(m_pFields->IsConvertToLastVersion(this))
		{
			NewData();
			SetAsBool(Value);//###_Mod 2002-9-26 zlq
			CField* pNewField = m_pFields->m_FieldVersion.GetFieldByAbsolutePos(m_nNextModifyField);			
			pNewField ->ConvertToCurField(this);
			DeleteData();
		}
	}
	return true;
}


/************************************************************************************
函数名称：
int CField::GetTypeSize()
功能说明：返回数据类型的大小,在为数据链表分配新空间时确定.

详细解释：1.(包含指示位和数据,string类型时为4).    

出入参数：
[in]: 无.      
  
[out]:无.　

返回类型：int

制作：YTLI 2002/07/15

修改: 
************************************************************************************/
//int CField::GetTypeSize()
//{	
	//子类实现	
//	return 0;
//}

/************************************************************************************
函数名称：
bool CField::IsNull()
功能说明：查看m_pValve所指向的数据是否为空NULL.

详细解释：1.如果m_pValve所指向的数据为空NULL,返回TRUE.    

出入参数：
[in]: 无.      
  
[out]:无.　

返回类型：bool

制作：YTLI 2002/07/15

修改: 
************************************************************************************/
bool CField::IsNull()
{
	if(!m_pValue)
		return false;
	if (GetFieldType() == fString)
	{
		FieldString* pStr = (FieldString*)m_pValue;
		return (pStr->GetLength()==0);
	}
	else
	{
		return ( *((FieldNUM*)m_pValue ) ==NULLValue);
	}

	return false;
}

/************************************************************************************
函数名称：
double CField::GetAsDouble()
功能说明：虚函數,将m_pValve所指向的数据以double型返回.

详细解释：1.如果子类没有实现,则仅实现错误提示,以示不能转换.
         2.如果子类实现,将调用子类的方法.

出入参数：
[in]:无.      
  
[out]:无.

返回类型:double


制作：YTLI 2002/07/15

修改: 
************************************************************************************/
double CField::GetAsDouble()
{	
	//throw _T("不能提供类型转换");
	//assert(0);
	////===temp AfxMessageBox(_T("不能提供转换\nCField::GetAsDouble将返回空值"));
	return 0;
}

/************************************************************************************
函数名称：
bool CField::SetAsDouble(double Value)
功能说明：虚函數,将m_pValve所指向的数据修改为传入的double型数据.

详细解释：1.如果子类没有实现,则仅实现错误提示,以示不能转换.
         2.如果子类实现,将调用子类的方法.

出入参数：
[in]: 无.      
  
[out]:无.　

返回类型：bool

制作：YTLI 2002/07/15

修改: 
************************************************************************************/
bool CField::SetAsDouble(double Value)
{
	if(m_nNextModifyField == -1)
		return false;
	else
	{
		if(m_pFields->IsConvertToLastVersion(this))
		{
			NewData();
			SetAsDouble(Value);//###_Mod 2002-9-26 zlq 
			CField* pNewField = m_pFields->m_FieldVersion.GetFieldByAbsolutePos(m_nNextModifyField);
			pNewField ->ConvertToCurField(this);
			DeleteData();
		}
	}
	return true;
}

/************************************************************************************
函数名称：
void CField::SetNull()
功能说明：将m_pValve所指向的数据设置为空值NULL.

详细解释：

出入参数：
[in]: 无.      
  
[out]:无.　

返回类型：void

制作：YTLI 2002/07/15

修改: 
************************************************************************************/
void CField::SetNull()
{
	if (GetFieldType() == fString)
		*(FieldString*)m_pValue = "";
	else
		*(FieldNUM*)m_pValue = NULLValue;
}


int CField::SaveToBuffer(TCLFile *pFile)
{
	if(!pFile)
		return false;


	BYTE bType = (BYTE)GetFieldType();
	pFile->Write(&bType,1);

	pFile->Write(&m_nWidth,2);//

	pFile->Write(&m_nDecimals,1);//指向该字段小数的长度.
	//#_S 2003-3-31 $ 16 zlq 扩展字段名长度**************************
	//得到字段名

	
	{//文件名
		int iFieldNameLength = m_sName.GetLength();
		pFile->Write(&iFieldNameLength,1);

		pFile->Write((const char*)m_sName,iFieldNameLength);
	}
	//此时 lpData 指向 Label 长度***************************

	int iFieldLabelLength = m_sLabel.GetLength();
	pFile->Write(&iFieldLabelLength,1);

	//指向该字段 Label 的内容
	pFile->Write((const char*)m_sLabel,iFieldLabelLength);

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
	
	if( m_ValueLabel.m_strLabelList.GetSize() != 0 )
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
	if (GetFieldType() ==fCurrency)
	{
		CCurrencyField *pField = (CCurrencyField*)this;
		wdExtLength += 3+pField->m_strCurrencyType.GetLength();
		iExtCnt++;
	}

	//### 2002-9-25 Eric==>
	wdExtLength += 1;//m_byteMeasure	
	wdExtLength += m_pMissing->QueryLength();
	//===temp TRACE("m_pMissing %d\n",m_pMissing->QueryLength());
	//### 2002-9-25 Eric<==
	BYTE *lpBuf = new BYTE[wdExtLength+5];
	BYTE *lpData = lpBuf;
	{
		memcpy( lpData, &wdExtLength, 4 );
		lpData += 4;               //指向该字段扩展区的块数
	}

	*lpData = iExtCnt;        
	lpData += 1;               //指向该字段扩展区

	SaveExt(lpData,wdExtLength);
	pFile->Write(lpBuf,wdExtLength+5);
	//iRetVal = (int)lpData - iRetVal + wdExtLength;        
	delete []lpBuf;
	return true;
}
/************************************************************************************
函数名称：

功能说明：将m_pValue指向的值按文件格式存入Buffer中.

详细解释：1.
          2.返回数据长度.
		  3.如果传入指针无效,则返回-1.

出入参数：
[in]: 1.lpData:指向的数据为数据指示+数据内容. 
  
[out]:无. 

返回类型：long

制作：YTLI 2002/07/15

修改: 
************************************************************************************/
void CField::SaveDataToBuffer(LPBYTE& lpData)
{
	if (GetFieldType() == fString)
	{
  		FieldString* pStr = (FieldString*)m_pValue;

		char* lpstr = pStr->GetData();
		memcpy(lpData,lpstr,pStr->GetLength()+1);

		/*char* lpstr = pStr->GetBuffer(m_nWidth+2);
		memcpy(lpData,lpstr,m_nWidth);
		pStr->ReleaseBuffer();	   */
		lpData +=Align4(m_nWidth);
	}
	else
	{
		memcpy( lpData,m_pValue, sizeof(FieldNUM) );	//ytli Modify 2002/09/04
		lpData +=sizeof(FieldNUM);
	}

}

/************************************************************************************
函数名称：
void CField::LoadExt(LPBYTE lpData, int count)
功能说明：从Buffer中读取字段的扩展区信息.

详细解释：1.protected函数,供LoadFromBuffer调用.       

出入参数：
[in]: 1.lpData:扩展区的起始字节地址. 
      2.count:扩展区的长度.
  
[out]:无. 

返回类型：void

制作：YTLI 2002/07/15

修改: 
************************************************************************************/
void CField::LoadExt(LPBYTE lpData, int count)
{	
	if(!lpData || count <= 0 )
		return;

	//此时 lpData 指向数据块1的标志位
	int iByteCnt  = 0;        //扩展区长度
	int iBlockCnt = 0;        //块的个数
	m_bIsEnum = false;        //初始化为FALSE;	
	
	while(iByteCnt < count && iBlockCnt < m_nExtCount)
	{
		iBlockCnt++;

		if((int)*lpData == Ext_Enum )
		{

			lpData += Ext_TitleLength110; //指向枚举类型的内容结构,因为枚举类型和枚举值各占一个字节，不用读数据块的长度
			iByteCnt += Ext_TitleLength110 +2;


			m_bIsEnum      = true;		
			m_byteEnumType = *(lpData);

			lpData++;         //指向枚举值

			m_byteEnumData = *(lpData);          	
			
			lpData++;         //lpData指向下一数据块
		}
		else if((int)*lpData == Ext_ValueLabel )
		{//值标签
			lpData += 1;   
			int wExtLength = 0;

			wExtLength = *(int*)lpData;
			lpData += sizeof(int);      //指向类枚举类型的内容			
			iByteCnt += (Ext_TitleLength110 + wExtLength);

			m_ValueLabel.LoadFromBuffer(lpData,wExtLength);
			lpData += wExtLength;                //lpData指向下一数据块
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
            
		}
		else//
		{//碰到不认识的，继续跳过
			lpData += 1;   
			int wExtLength = 0;
			{
				wExtLength = *(int*)lpData;
				lpData += sizeof(int);      //指向类枚举类型的内容			
				iByteCnt += (Ext_TitleLength110 + wExtLength);
			}

			lpData += wExtLength;                //lpData指向下一数据块			
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
void CField::SaveExt(LPBYTE lpData, int count)
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

void CField::SaveExt(LPBYTE lpData, int count)
{
	if(!lpData || count <= 0 )
		return;

	int iByteCnt  = 0;        //扩展区长度;

	if( m_bIsEnum == true && iByteCnt < count )
	{
		*lpData = Ext_Enum;//枚举
		lpData += 1;

		//存入长度值2
		if (m_pFields->m_pPages->GetSaveFileVersion()==100)
		{//如果是1.0版本		
			WORD wdTemp = 2;
			memcpy(lpData,&wdTemp,sizeof(WORD));
			lpData += sizeof(WORD);
			iByteCnt += Ext_TitleLength100 +2;
		}
		else
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
	if( m_ValueLabel.m_strLabelList.GetSize() != 0 && iByteCnt < count )
	{			
		*lpData = Ext_ValueLabel;//值标签
		lpData += 1;

		//存入长度值
		int wLength = m_ValueLabel.QueryLength();
		if (m_pFields->m_pPages->GetSaveFileVersion()==100)
		{//如果是1.0版本
			WORD wL = wLength;
			memcpy(lpData,&wL,sizeof(WORD));
			lpData += sizeof(WORD);
			iByteCnt += (Ext_TitleLength100 + wLength);
		}
		else
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
		if (m_pFields->m_pPages->GetSaveFileVersion()==100)
		{//如果是1.0版本
			WORD wL = wLength;
			memcpy(lpData,&wL,sizeof(WORD));
			lpData += sizeof(WORD);
			iByteCnt += (Ext_TitleLength100 + wLength);
		}
		else
		{
			memcpy(lpData,&wLength,sizeof(int));
			lpData += sizeof(int);	
			iByteCnt += (Ext_TitleLength110 + wLength);
		}

		//存入显示特性内容
		lpData += m_ShowCharacteristic.saveShowCharacter(lpData);	

	}

	//### 2002-9-25 Eric==>
	*lpData = m_byteMeasure;
	lpData ++;
	m_pMissing->SaveToBuffer(lpData);
	//### 2002-9-25 Eric<==
}

/************************************************************************************
函数名称：
void CField::Clone(CField* pField)
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
void CField::Clone(CField* pField)
{	
	if(!pField)
		return ;

	m_bIsEnum      = pField->m_bIsEnum;
	m_byteEnumData = pField->m_byteEnumData;
	m_byteEnumType = pField->m_byteEnumType;
	m_nDecimals    = pField->m_nDecimals;
	m_nExtCount    = pField->m_nExtCount;
	m_nExtLength   = pField->m_nExtLength;
	m_nWidth       = pField->m_nWidth;
	m_sLabel       = pField->m_sLabel;
	m_sName        = pField->m_sName;
	m_byteMeasure  = pField->m_byteMeasure;	  //### 2002-9-25 Eric

	//m_ValueLabel.m_pField = this;
	m_ValueLabel.SetOwner(this);
    m_ValueLabel.Clone(&pField->m_ValueLabel);
	m_ShowCharacteristic.Clone(&pField->m_ShowCharacteristic);
	m_pMissing->Clone(pField->m_pMissing); //### 2002-9-25 Eric

}

/************************************************************************************
函数名称：
void CField::NewData()
功能说明: 虚函數,申请新的数据空间,在数据集进行insert,append等操作时,先调用.
        
详细解释:1.子类实现,不同类型有不同的大小. 
  
出入参数:
[in]: 无.
 
[out]:无.

返回类型:void.

制作:YTLI      2002/07/12

修改: 
************************************************************************************/

void CField::RealNewData()
{
	if (GetFieldType() == fString)
	{
		FieldString* pStr = new FieldString();
		
		m_pValue = (LPBYTE)(pStr);
	}
	else
	{
		m_pValue =(LPBYTE) new FieldNUM;
		*(FieldNUM*)m_pValue = NULLValue;
	}
}


void CField::NewData()
{
	//m_pFields->m_pPages->m_Depot.NewData(this);
	m_pValue =(LPBYTE) m_pTempData;
	SetNull();
}

//删除m_pValue所指向的空间
void CField::RealDeleteData()
{
	if (GetFieldType() == fString)
	{
		FieldString* pStr = (FieldString*)m_pValue;
		delete pStr;
		m_pValue = NULL;
	}
	else
	{
		if( m_pValue )
			delete[] m_pValue;
		
		m_pValue = NULL;
	}

}

void CField::DeleteData()
{
	//m_pFields->m_pPages->m_Depot.DeleteData(this);//ytli Modify 2002/09/03
	m_pValue = NULL;
}

/************************************************************************************
函数名称：
bool CField::GetData(LPBYTE lpData)
功能说明: 虚函數,根据lpData指向的数据,复制该数据值到本字段,两者有相同的数据格式,
          如用户调用edit,cancel操作时恢复原值.

详细解释:1.子类实现. 
  
出入参数:
[in]: 1.lpData :需要复制的数据的地址.
 
[out]:无　

返回类型:bool.

制作:YTLI      2002/07/12

修改: 
************************************************************************************/
bool CField::GetData(void* lpData)
{
	return 0;
}
bool CField::GetData(void*pDestArrar,int nIndex)
{
	return 0;
}
/************************************************************************************
函数名称：
LPBYTE CField::SetData()
功能说明: 虚函數,将本字段的数据生成一个拷贝,地址空间也由本函数生成,并返回该地址,
          用于数据传递和保存旧值.
详细解释:1.子类实现. 
  
出入参数:
[in]: 无.
  
[out]:无　

返回类型:LPBYTE.

制作:YTLI      2002/07/12

修改: 
************************************************************************************/
/*
LPBYTE CField::SetData()
{
	//子类实现
	return (LPBYTE)0;
}
*/
/************************************************************************************
函数名称：
int CField::QuerySaveLength()
功能说明: 虚函數,得到当前字段的值(m_pValue指向的)保存到文件时所需的长度,包括指示位,也可能就
          指示位也够了,用于确定缓冲区的大小.
详细解释:1.子类实现. 
         2.大小与值相关.

出入参数:
[in]:无.
  
[out]:无　

返回类型:int.

制作:YTLI 2002/07/12

修改: 
************************************************************************************/

int CField::QuerySaveLength()
{
	if (GetFieldType() == fString)
	{

		return Align4(m_nWidth);
	}
	else
	{
		return sizeof(FieldNUM);
	}
}

/************************************************************************************
函数名称：
bool CField::GetFromDisplay(CTString str)
功能说明: 虚函數,把外部用户输入的数据(字符串形式)保存到字段内.

详细解释:1.子类实现,不同的类有不同的实现. 
        2. 如果成功,则返回true.

出入参数:
[in]: 1.str: 从界面上得到的字符串.
  
[out]:无　

返回类型:bool.

制作:YTLI     2002/07/16

修改:
************************************************************************************/
bool CField::GetFromDisplay(CTString str)
{
	//子类实现;
	return false;
}



//将当前数据设置为缺省值,即将m_pDefaultValue所指向的数据拷贝到m_pValue中
void CField::SetDefaultValue()
{
}

//用当前的数据设置作为缺省值,即将m_pValue所指向的数据拷贝到m_pDefaultValue中，原来数据没有销毁
bool CField::CopyToDefaultValue()
{
	return true;
}

//将原来版本的字段数据转换到当前字段的数据。
//1。如果本字段空间为空，生成空间
//2。要进行原来字段空值判断！
//2。1如果为空则本字段为空
//2。2如果不为空则将原来的数据以原来的数据类型读    出，然后调用本字段的SetAs***设置。
//然后，如果有新的next字段，则调用该字段的ConvertToCurField函数，并且负责释放本空间，并且清0
void CField::ConvertToCurField(CField* pOldField)
{
	/*
	if(this->GetFieldType() == fString)
	{
		ConvertToCurField(pOldField);
		return ;
	}
*/
	if(!m_pValue)
	{//本字段也为中间字段
		NewData();

		if(pOldField->IsNull())
			SetNull();
		else
		{
			CFieldType ftType = pOldField->GetFieldType();
			if( ftType == fBoolean )			
				SetAsBool( pOldField->GetAsBool());

			else if( ftType == fCurrency )			
				SetAsCurrency( pOldField->GetAsCurrency());

			else if( ftType == fDate )			
				//SetAsBool( pOldField->GetAsDateTime());
				SetAsBool( true);//#_修改 2002-12-2 $ 10:01:35 zlq
			else if( ftType == fDouble )			
				SetAsDouble( pOldField->GetAsDouble());

			else if( ftType == fInt )			
				SetAsInteger( pOldField->GetAsInteger());

			else if( ftType == fString )			
				SetAsString( pOldField->GetAsString());		

			//assert(m_nNextModifyField != -1);

			CField* pNewField = m_pFields->m_FieldVersion.GetFieldByAbsolutePos(m_nNextModifyField);
			pNewField->ConvertToCurField(this); 			
		}
		DeleteData();
	}
	else   //最后一层转换.
	{
		if(pOldField->IsNull())		
			SetNull();			
		else
		{
			if( pOldField->GetFieldType() == fBoolean )			
				SetAsBool( pOldField->GetAsBool());

			else if( pOldField->GetFieldType() == fCurrency )			
				SetAsCurrency( pOldField->GetAsCurrency());

			else if( pOldField->GetFieldType() == fDate )			
				//SetAsBool( pOldField->GetAsDateTime());
				SetAsBool( true);

			else if( pOldField->GetFieldType() == fDouble )			
				SetAsDouble( pOldField->GetAsDouble());

			else if( pOldField->GetFieldType() == fInt )			
				SetAsInteger( pOldField->GetAsInteger());

			else if( pOldField->GetFieldType() == fString )			
				SetAsString( pOldField->GetAsString());		

			//assert(m_nNextModifyField == -1);
		}
	}
}

void CField::SetLabel(CTString strLabel)
{
	m_sLabel = strLabel;
}

CTString CField::GetLabel()
{
	return m_sLabel;
}

bool CField::SetFieldName(CTString strFieldName)
{
	if (m_pFields)
	{
		if (m_pFields->IndexOf(this)>=0)
		{//在当前版本字段内
			CField*pTemp = m_pFields->FieldByName(strFieldName);
			if (pTemp)
			{
				if (pTemp != this)
				{//如果是本身字段,则正常
					//===temp AfxMessageBox("已经有该字段名称，设置失败！");
					return false;
				}
			}
		}
	}
	if (strFieldName.GetLength()>255)
	{
	//	//===temp AfxMessageBox("名字长度不能超过10个字符！");
		return false;
	}
	if (strFieldName.Find('\"') != -1 )
		return false;
	if (strFieldName.Find('*') != -1 )
		return false;
	if (strFieldName.Find('?') != -1 )
		return false;
	if (strFieldName.Find('+') != -1 )
		return false;
	if (strFieldName.Find('-') != -1 )
		return false;
	if (strFieldName.Find('=') != -1 )
		return false;
	if (strFieldName.Find('/') != -1 )
		return false;
	if (strFieldName.Find('>') != -1 )
		return false;
	if (strFieldName.Find('<') != -1 )
		return false;
	if (strFieldName.Find('&') != -1 )
		return false;
	if (strFieldName.Find('|') != -1 )
		return false;
	if (strFieldName.Find('(') != -1 )
		return false;
	if (strFieldName.Find(')') != -1 )
		return false;
	if (strFieldName.Find('[') != -1 )
		return false;
	if (strFieldName.Find(']') != -1 )
		return false;
	if (strFieldName.Find('{') != -1 )
		return false;
	if (strFieldName.Find('}') != -1 )
		return false;
	if (strFieldName.Find('%') != -1 )
		return false;
	if (strFieldName.Find('$') != -1 )
		return false;
	if (strFieldName.Find('^') != -1 )
		return false;
	if (strFieldName.Find('\'') != -1 )
		return false;
	if (strFieldName.Find(';') != -1 )
		return false;
	if (strFieldName.Find(':') != -1 )
		return false;
	if (strFieldName.Find(',') != -1 )
		return false;
	if (strFieldName.Find('.') != -1 )
		return false;

	//char tt = strFieldName.GetAt(0);
	//if (tt>='0' && tt<='9')//第一个字符不能为数字
	//	return false;

	m_sName = strFieldName;
	return true;
}

CTString CField::GetFieldName()
{
	return m_sName;
}

void CField::SetDecimals(int nDecimals)
{
	m_nDecimals = nDecimals;
}

int CField::GetDecimals()
{
	return m_nDecimals;
}

void CField::SetWidth(int nWidth)
{
	m_nWidth = nWidth;
}

int CField::GetWidth()
{
	return m_nWidth;
}

//### 2002-9-25 Eric===>
CMeasure CField::GetMeasure()
{
	return m_byteMeasure;
}

void CField::SetMeasure(CMeasure measure)
{
	m_byteMeasure = measure;
}
//### 2002-9-25 Eric<===

//取得显示相关的特性
//###_Mod 2002-11-1 zlq
bool CField::GetViewSet(CViewSet &ViewSet)
{
	//*************行的设置******************//	
	ViewSet.nCWidth			= m_ShowCharacteristic.m_nCWidth		;
	ViewSet.cCColor			= m_ShowCharacteristic.m_cCColor		;
	ViewSet.byteCAlign		= m_ShowCharacteristic.m_byteCAlign		;
	ViewSet.strCFontName	= m_ShowCharacteristic.m_strCFontName	;
	ViewSet.byteCFontStyle	= m_ShowCharacteristic.m_byteCFontStyle	;
	ViewSet.byteCFontSize	= m_ShowCharacteristic.m_byteCFontSize	;
	//*************列的设置******************//
	ViewSet.nRWidth			= m_ShowCharacteristic.m_nRWidth		;	
	ViewSet.cRColor			= m_ShowCharacteristic.m_cRColor		;
	ViewSet.byteRAlign		= m_ShowCharacteristic.m_byteRAlign		;
	ViewSet.strRFontName	= m_ShowCharacteristic.m_strRFontName	;
	ViewSet.byteRFontStyle	= m_ShowCharacteristic.m_byteRFontStyle	;
	ViewSet.byteRFontSize	= m_ShowCharacteristic.m_byteRFontSize	;	
	return true;	
}

//设置显示相关的特性
bool CField::SetViewSet(CViewSet ViewSet)
{
	//*************列的设置******************//
	m_ShowCharacteristic.m_nCWidth			= ViewSet.nCWidth			;
	m_ShowCharacteristic.m_cCColor			= ViewSet.cCColor			;				
	m_ShowCharacteristic.m_byteCAlign		= ViewSet.byteCAlign		;
	m_ShowCharacteristic.m_strCFontName		= ViewSet.strCFontName		;
	m_ShowCharacteristic.m_byteCFontStyle	= ViewSet.byteCFontStyle	;
    m_ShowCharacteristic.m_byteCFontSize	= ViewSet.byteCFontSize		;
	//*************行的设置******************//
	m_ShowCharacteristic.m_nRWidth 			= ViewSet.nRWidth			;
	m_ShowCharacteristic.m_cRColor			= ViewSet.cRColor			;
	m_ShowCharacteristic.m_byteRAlign		= ViewSet.byteRAlign		;
	m_ShowCharacteristic.m_strRFontName		= ViewSet.strRFontName		;
	m_ShowCharacteristic.m_byteRFontStyle	= ViewSet.byteRFontStyle	;
	m_ShowCharacteristic.m_byteRFontSize	= ViewSet.byteRFontSize		;
	return true;
}


//取得数据类型（离散型/连续型）
int CField::GetDCDataType(int nIndex)
{
	int nMeasure=GetMeasure(),
		nDataType=GetFieldType();
	
	if(nMeasure== msNominal)
		return CONTINU_DATA;
	
	if(nDataType==fBoolean || nDataType==fString || nMeasure==msScale || nMeasure==msOrdinal)
		return DISPERSE_DATA;
	
	return 0;
}

///cwh 2002-11-14
//根据当前的元素数据在标量表(m_ValueLable)中取得相应的标量值
LPCTSTR CField::GetValueLable()
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
//
CTString CField::GetValLable()
{
	CTString strValue = GetAsString();

	int nSize=m_ValueLabel.GetCount();
	for(int n=0;n<nSize;n++)
	{
		m_ValueLabel.SetCurPos(n);
		if(m_ValueLabel.GetAsString()==strValue)
			return m_ValueLabel.GetExplainString();
	}
	
	return _T("");
}

bool CField::SetEnumType(DateType nType)
{
	return true;
}

DateType CField::GetEnumType()
{
	return -1;
}

bool CField::SetDateConvertType(BYTE cType)
{
	if( cType>=48 && cType<=127 )
	{
		m_bIsEnum = true;
		m_byteEnumData = cType;
		return true;
	}
	m_bIsEnum = false;
	return false;
}

BYTE CField::GetDateConvertType()
{
	return m_byteEnumData;
}



/*从目标字段pSourceField复制字段信息
1:目标字段与源字段同类型时,复制:
	m_sName m_bIsEnum  m_byteEnumData 
	m_byteEnumType m_nDecimals   m_nExtCount  m_nExtLength   m_nWidth       
	m_sLabel       m_byteMeasure 	m_ValueLabel	m_ShowCharacteristic
	m_pMissing
2:目标字段与源字段不同类型时,复制:
	字段名m_sName.
	如果目标字段为:
	1:fInt:m_nDecimals = 0.
	2:fBoolean:m_nWidth = 2,m_nDecimals = 0*/
void CField::CopyCommonProperty(CField *pSourceField)
{
	if(!pSourceField)
		return;
	
	//字段名总是需要的.
	m_sName        = pSourceField->m_sName;
	switch(pSourceField->GetFieldType())
	{
	case fString:
		{
			switch(GetFieldType())
			{
			case fString:
				m_bIsEnum      = pSourceField->m_bIsEnum;
				m_byteEnumData = pSourceField->m_byteEnumData;
				m_byteEnumType = pSourceField->m_byteEnumType;
				m_nDecimals    = pSourceField->m_nDecimals;
				m_nExtCount    = pSourceField->m_nExtCount;
				m_nExtLength   = pSourceField->m_nExtLength;
				m_nWidth       = pSourceField->m_nWidth;
				m_sLabel       = pSourceField->m_sLabel;
				m_byteMeasure  = pSourceField->m_byteMeasure;	  //### 2002-9-25 Eric
				
			//	m_ValueLabel.m_pField = this;
				m_ValueLabel.Clone(&pSourceField->m_ValueLabel);
				m_ShowCharacteristic.Clone(&pSourceField->m_ShowCharacteristic);
				m_pMissing->Clone(pSourceField->m_pMissing); //### 2002-9-25 Eric
				break;
			case fInt:
				m_nDecimals    = 0;//pSourceField->m_nDecimals;
				break;
			case fDouble:
				break;
			case fBoolean:
				m_nWidth = 2;
				break;
			case fCurrency:
				break;
			case fDate:
				m_nWidth = 12;
				break;
			}
		};
		break;
	case fInt:
		{
			m_nDecimals    = 0;//pSourceField->m_nDecimals;
			switch(GetFieldType())
			{
			case fString:
				break;
			case fInt:
				m_bIsEnum      = pSourceField->m_bIsEnum;
				m_byteEnumData = pSourceField->m_byteEnumData;
				m_byteEnumType = pSourceField->m_byteEnumType;
				m_nDecimals    = pSourceField->m_nDecimals;
				m_nExtCount    = pSourceField->m_nExtCount;
				m_nExtLength   = pSourceField->m_nExtLength;
				m_nWidth       = pSourceField->m_nWidth;
				m_sLabel       = pSourceField->m_sLabel;
				m_sName        = pSourceField->m_sName;
				m_byteMeasure  = pSourceField->m_byteMeasure;	  //### 2002-9-25 Eric
				
			//	m_ValueLabel.m_pField = this;
				m_ValueLabel.Clone(&pSourceField->m_ValueLabel);
				m_ShowCharacteristic.Clone(&pSourceField->m_ShowCharacteristic);
				m_pMissing->Clone(pSourceField->m_pMissing); //### 2002-9-25 Eric
			case fDouble:
				m_nDecimals    = 2;
				m_nWidth       = pSourceField->m_nWidth;
				break;
			case fBoolean:
				m_nWidth = 2;
				break;
			case fCurrency:
				m_nDecimals    = 2;
				m_nWidth       = pSourceField->m_nWidth;
				break;
			case fDate:
				m_nWidth = 12;
				break;
			}
		};
		break;
	case fDouble:
		{
			switch(GetFieldType())
			{
			case fString:
				break;
			case fInt:
				break;
			case fDouble:
				m_bIsEnum      = pSourceField->m_bIsEnum;
				m_byteEnumData = pSourceField->m_byteEnumData;
				m_byteEnumType = pSourceField->m_byteEnumType;
				m_nDecimals    = pSourceField->m_nDecimals;
				m_nExtCount    = pSourceField->m_nExtCount;
				m_nExtLength   = pSourceField->m_nExtLength;
				m_nWidth       = pSourceField->m_nWidth;
				m_sLabel       = pSourceField->m_sLabel;
				m_sName        = pSourceField->m_sName;
				m_byteMeasure  = pSourceField->m_byteMeasure;	  //### 2002-9-25 Eric
				
			//	m_ValueLabel.m_pField = this;
				m_ValueLabel.Clone(&pSourceField->m_ValueLabel);
				m_ShowCharacteristic.Clone(&pSourceField->m_ShowCharacteristic);
				m_pMissing->Clone(pSourceField->m_pMissing); //### 2002-9-25 Eric
			case fBoolean:
				m_nWidth = 2;
				break;
			case fCurrency:
				m_nDecimals    = pSourceField->m_nDecimals;
				m_nWidth       = pSourceField->m_nWidth;
				break;

			case fDate:
				m_nWidth = 12;
				break;
			}
		};
		break;
	case fBoolean:
		{
			m_nWidth = 2;
			m_nDecimals    = 0;//pSourceField->m_nDecimals;
			switch(GetFieldType())
			{
			case fString:
				break;
			case fInt:
				m_nWidth = 8;
				m_nDecimals    = 0;//pSourceField->m_nDecimals;
				break;
			case fDouble:
				m_nWidth = 8;
				m_nDecimals    = 2;//pSourceField->m_nDecimals;
				break;
			case fBoolean:
				m_bIsEnum      = pSourceField->m_bIsEnum;
				m_byteEnumData = pSourceField->m_byteEnumData;
				m_byteEnumType = pSourceField->m_byteEnumType;
				m_nDecimals    = pSourceField->m_nDecimals;
				m_nExtCount    = pSourceField->m_nExtCount;
				m_nExtLength   = pSourceField->m_nExtLength;
				m_nWidth       = pSourceField->m_nWidth;
				m_sLabel       = pSourceField->m_sLabel;
				m_sName        = pSourceField->m_sName;
				m_byteMeasure  = pSourceField->m_byteMeasure;	  //### 2002-9-25 Eric
				
			//	m_ValueLabel.m_pField = this;
				m_ValueLabel.Clone(&pSourceField->m_ValueLabel);
				m_ShowCharacteristic.Clone(&pSourceField->m_ShowCharacteristic);
				m_pMissing->Clone(pSourceField->m_pMissing); //### 2002-9-25 Eric
			case fCurrency:
				m_nWidth = 8;
				m_nDecimals    = 2;//pSourceField->m_nDecimals;
				break;

			case fDate:
				m_nWidth = 12;
				break;
			}
		};
		break;
	case fCurrency:
		{
			switch(GetFieldType())
			{
			case fString:
				break;
			case fInt:
				break;
			case fDouble:
				m_nDecimals    = pSourceField->m_nDecimals;
				m_nWidth       = pSourceField->m_nWidth;
				break;
			case fBoolean:
				break;
			case fCurrency:
				m_bIsEnum      = pSourceField->m_bIsEnum;
				m_byteEnumData = pSourceField->m_byteEnumData;
				m_byteEnumType = pSourceField->m_byteEnumType;
				m_nDecimals    = pSourceField->m_nDecimals;
				m_nExtCount    = pSourceField->m_nExtCount;
				m_nExtLength   = pSourceField->m_nExtLength;
				m_nWidth       = pSourceField->m_nWidth;
				m_sLabel       = pSourceField->m_sLabel;
				m_sName        = pSourceField->m_sName;
				m_byteMeasure  = pSourceField->m_byteMeasure;	  //### 2002-9-25 Eric
				
			//	m_ValueLabel.m_pField = this;
				m_ValueLabel.Clone(&pSourceField->m_ValueLabel);
				m_ShowCharacteristic.Clone(&pSourceField->m_ShowCharacteristic);
				m_pMissing->Clone(pSourceField->m_pMissing); //### 2002-9-25 Eric
			case fDate:
				m_nWidth = 12;
				break;
			}
		};
		break;

	case fDate:
		{
			switch(GetFieldType())
			{
			case fString:
				break;
			case fInt:
				break;
			case fDouble:
				break;
			case fBoolean:
				break;
			case fCurrency:
				break;
			case fDate:
				m_bIsEnum      = pSourceField->m_bIsEnum;
				m_byteEnumData = pSourceField->m_byteEnumData;
				m_byteEnumType = pSourceField->m_byteEnumType;
				m_nDecimals    = pSourceField->m_nDecimals;
				m_nExtCount    = pSourceField->m_nExtCount;
				m_nExtLength   = pSourceField->m_nExtLength;
				m_nWidth       = pSourceField->m_nWidth;
				m_sLabel       = pSourceField->m_sLabel;
				m_sName        = pSourceField->m_sName;
				m_byteMeasure  = pSourceField->m_byteMeasure;	  //### 2002-9-25 Eric
				
			//	m_ValueLabel.m_pField = this;
				m_ValueLabel.Clone(&pSourceField->m_ValueLabel);
				m_ShowCharacteristic.Clone(&pSourceField->m_ShowCharacteristic);
				m_pMissing->Clone(pSourceField->m_pMissing); //### 2002-9-25 Eric
				break;
			}
		};
	}
	return;
}

////zjw add 2002-12-19
//为字段添加是否筛选逗号
//DEL void CField::SetFieldFiltrComma(int nComma)
//DEL {
//DEL    m_nComma = nComma;
//DEL }
//获得标志
//DEL int CField::GetFieldFiltrComma()
//DEL {
//DEL  return m_nComma;
//DEL }
/************************************************************************************
功能说明:
	根据输入的值在当前值标签列表中查找对应的值解释,没有找到,则返回false;
	找到则true,并且在strLabel中输出对应的解释
详细解释:

出入参数:
[in]: nValue 对应的值
 
[out]:strLabel 输出解释

返回类型:bool.

制作://#_S 2003-4-1 $ 13 zlq
************************************************************************************/
bool CField::GetValueVL(int nValue,CTString &strLabel)
{
	int nCount = m_ValueLabel.GetCount();
	for (int i=0; i<nCount ;i++)
	{
		m_ValueLabel.SetCurPos(i);
		if (m_ValueLabel.GetAsInteger()==nValue)
		{//找到
			strLabel = m_ValueLabel.GetExplainString();
			return true;
		}
	}
	return false;
}
/************************************************************************************
功能说明:
	根据输入的值在当前值标签列表中查找对应的值解释,没有找到,则返回false;
	找到则true,并且在strLabel中输出对应的解释
详细解释:

出入参数:
[in]: dValue 对应的值
 
[out]:strLabel 输出解释

返回类型:bool.

制作://#_S 2003-4-1 $ 13 zlq
************************************************************************************/
bool CField::GetValueVL(double dValue,CTString &strLabel)
{
	int nCount = m_ValueLabel.GetCount();
	for (int i=0; i<nCount ;i++)
	{
		m_ValueLabel.SetCurPos(i);
		if (fabs(m_ValueLabel.GetAsDouble()-dValue) < DoubleZero)
		{//找到
			strLabel = m_ValueLabel.GetExplainString();
			return true;
		}
	}
	return false;
}
/************************************************************************************
功能说明:
	根据输入的值在当前值标签列表中查找对应的值解释,没有找到,则返回false;
	找到则true,并且在strLabel中输出对应的解释
详细解释:

出入参数:
[in]: strValue 对应的值
 
[out]:strLabel 输出解释

返回类型:bool.

制作://#_S 2003-4-1 $ 13 zlq
************************************************************************************/
bool CField::GetValueVL(CTString strValue,CTString &strLabel)
{
	int nCount = m_ValueLabel.GetCount();
	for (int i=0; i<nCount ;i++)
	{
		m_ValueLabel.SetCurPos(i);
		CTString str = m_ValueLabel.GetAsString();
		if (str ==strValue)
		{//找到
			strLabel = m_ValueLabel.GetAsString();
//			strLabel = m_ValueLabel.GetExplainString();
			return true;
		}
	}
	return false;
}

/************************************************************************************
功能说明:
	根据输入的值在当前值标签列表中查找对应的值解释,没有找到,则返回false;
	找到则true,并且在strLabel中输出对应的解释
详细解释:

出入参数:
[in]: bValue 对应的值
 
[out]:strLabel 输出解释

返回类型:bool.

制作://#_S 2003-4-1 $ 13 zlq
************************************************************************************/
bool CField::GetValueVL(bool bValue,CTString &strLabel)
{
	int nCount = m_ValueLabel.GetCount();
	for (int i=0; i<nCount ;i++)
	{
		m_ValueLabel.SetCurPos(i);
		if (m_ValueLabel.GetAsBool()==bValue)
		{//找到
			strLabel = m_ValueLabel.GetExplainString();
			return true;
		}
	}
	return false;
}

/************************************************************************************
功能说明:
	根据输入的值在当前值标签列表中查找对应的值解释,没有找到,则返回false;
	找到则true,并且在strLabel中输出对应的解释
详细解释:

出入参数:
[in]: ftValue 对应的值
 
[out]:strLabel 输出解释

返回类型:bool.

制作://#_S 2003-4-1 $ 13 zlq
************************************************************************************/
bool CField::GetValueVL(FILETIME ftValue,CTString &strLabel)
{
	int nCount = m_ValueLabel.GetCount();
	for (int i=0; i<nCount ;i++)
	{
		m_ValueLabel.SetCurPos(i);
		FILETIME ft = m_ValueLabel.GetAsDateTime();
		if (memcmp(&ft,&ftValue,sizeof(FILETIME))==0)
		{//找到
			strLabel = m_ValueLabel.GetExplainString();
			return true;
		}
	}
	return false;
}

void CField::pg_Add(void*pDestArrar)
{
	if (GetFieldType() == fString)
		((PF_STRING)pDestArrar)->Add(*(FieldString*)m_pTempData);
	else
		((PF_NUM)pDestArrar)->Add(*(FieldNUM*)m_pTempData);
}
void CField::pg_Add(void*pDestArrar,int nCount)
{
	if (GetFieldType() == fString)
	{
		for (int i=0;i<nCount ;i++)
			((PF_STRING)pDestArrar)->Add(*(FieldString*)m_pTempData);
	}
	else
	{
		for (int i=0;i<nCount ;i++)
			((PF_NUM)pDestArrar)->Add(*(FieldNUM*)m_pTempData);
	}
}

void CField::pg_InsertAt(void*pDestArrar,int nIndex)
{
	if (GetFieldType() == fString)
		((PF_STRING)pDestArrar)->InsertAt(nIndex,*(FieldString*)m_pTempData);
	else
		((PF_NUM)pDestArrar)->InsertAt(nIndex,*(FieldNUM*)m_pTempData);
}

void CField::pg_RemoveAt(void *pDestArrar,int nIndex)
{
	if (GetFieldType() == fString)
		((PF_STRING)pDestArrar)->RemoveAt(nIndex);
	else
		((PF_NUM)pDestArrar)->RemoveAt(nIndex);
}

void CField::pg_RemoveAll(void *pDestArrar)
{
	if (GetFieldType() == fString)
		((PF_STRING)pDestArrar)->RemoveAll();
	else
		((PF_NUM)pDestArrar)->RemoveAll();
}

void CField::pg_Delete(void *pDestArrar)
{
	if (GetFieldType() == fString)
		delete ((PF_STRING)pDestArrar);
	else
		delete ((PF_NUM)pDestArrar);
}

void* CField::pg_New()
{
	if (GetFieldType() == fString)
	{
		PF_STRING pResult = new F_STRING;
		pResult->SetSize(0,IncreaseSize);
		return pResult;
	}
	else
	{
		PF_NUM pResult = new F_NUM;
		pResult->SetSize(0,IncreaseSize);
		return pResult;
	}
}

LPBYTE CField::pg_GetAt(void*pDestArrar,int nIndex)
{
	if (GetFieldType() == fString)
		return(LPBYTE) &((*(PF_STRING)pDestArrar)[nIndex]);
	else
		return(LPBYTE) &((*(PF_NUM)pDestArrar)[nIndex]);
}


bool CField::bHaveValueLabel()
{
	int nCount = m_ValueLabel.GetCount();
	if(nCount>0)
		return true;
	else
		return false;
}

void CField::PrintValue()
{
	printf(GetAsString());
}

//判断当前值是否属于缺失值，空值也是缺失值
bool CField::IsMissingValue()
{
	bool bValueMissed = false;
	double dData[3], dOrgData;
	int nCount, iIndex;

	if( IsNull() )
		bValueMissed = true;
	else if( m_pMissing!=NULL )
	{
		if( m_pMissing->GetMissingType()!=mtNoMissing )
		{
			dOrgData = GetAsDouble();	
			m_pMissing->GetDoubleData( dData[0], dData[1], dData[2] );
			nCount =  m_pMissing->GetCount();

			if( m_pMissing->GetMissingType()==mtDiscrete )
			{
				for( iIndex=0; iIndex<nCount; iIndex++ )
				{
					if( dOrgData==dData[iIndex] )
					{
						bValueMissed = true;
						break;
					}
				}
			}
			else if( m_pMissing->GetMissingType()==msRange )
			{
				if( dOrgData>=dData[0] && dOrgData<=dData[1] )
				 	bValueMissed = true;
				else if( nCount==3 && dOrgData==dData[2] )
					bValueMissed = true;
			}
		}
	}

	return bValueMissed;
}
