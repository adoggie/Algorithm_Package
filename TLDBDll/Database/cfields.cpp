// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "cfields.h"

extern BYTE  tlcfgBuf[] ;
extern BYTE  CurFileBuf[] ;
/************************************************************************************
函数名称：
      CFields::~CFields()
功能说明：
    析构函数
详细解释：
       1.删除所有的字段.
	  
出入参数：
返回类型：
制作：
    YTLI 2002/07/08
修改: 
************************************************************************************/
CFields::~CFields()
{	
}

/************************************************************************************
函数名称：
      void CFields::GetBookmark()
功能说明：
    做书签.
详细解释：
       1.保存当前指针值,在物理清空该表时PhysicalEmptyTable()使用书签.
	  
出入参数：
返回类型：
制作：
    YTLI 2002/08/12
修改: 
************************************************************************************/
void CFields::GetBookmark()
{
	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		CField* pField = (CField*)m_FieldArray.GetAt(i);
		pField->m_pOldValue  = pField->m_pValue;
	}
	m_pOldDataSet = m_pDataSet;

}

void CFields::GotoBookmark()
{
	CField* pField ;
	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		pField = (CField*)m_FieldArray.GetAt(i);
		pField->m_pValue = pField->m_pOldValue;
	}
	m_pDataSet = m_pOldDataSet ;

}

void CFields::Clear()
{
	//===temp AfxMessageBox("发生了不该调用!CFields::Clear()");
}
/************************************************************************************
函数名称：
void CFields::Add(CField* Field)
功能说明：将字段Field加入到字段链表m_FieldList中.

详细解释：
    
出入参数：
[in]: 1.Field:字段指针.
     
[out]:无.

返回类型：void

制作：YTLI   2002/07/15

修改: 
***********************************************************************************/
void CFields::Add(CField* pField)
{
	if(!pField)
		return;
	
	m_FieldVersion.Add(pField);
	SetCurVersion();
}

/************************************************************************************
函数名称：
CField* CFields::FieldByName(CTString FieldName)
功能说明：根据字段名查找字段信息.

详细解释：
    
出入参数：
[in]: 1.FieldName:字段名.
     
[out]:无.

返回类型：CField*

制作：YTLI   2002/07/15

修改: 
***********************************************************************************/
CField* CFields::FieldByName(CTString FieldName)
{	
	CField*pField ;
	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		pField = (CField*)m_FieldArray.GetAt(i);
		if(pField->GetFieldName() == FieldName)
			return pField;	
	}
	return NULL;
	/*
	POSITION pos = m_FieldList.GetHeadPosition();

	while(pos)
	{
		CField* pField = m_FieldList.GetNext(pos);
		
		if(pField->GetFieldName() == FieldName)
			return pField;	
	}
	return NULL;
	*/
}

/************************************************************************************
函数名称：
void CFields::Remove(CField* Field)
功能说明：删除字段集m_FieldList中的一个字段.

详细解释：
    
出入参数：
[in]: 1.Field:要删除的字段指针.
     
[out]:无.

返回类型：void

制作：YTLI   2002/07/15

修改: 
***********************************************************************************/
void CFields::Remove(CField* pField)
{
	m_FieldVersion.Remove(pField);
	m_FieldVersion.SetVersion(m_FieldArray);	
}

/************************************************************************************
函数名称：
int CFields::IndexOf(CField* Field)
功能说明：得到一个字段的Index(从0开始).

详细解释：1.如果在字段集中没有找到该字段,则返回-1.
    
出入参数：
[in]: 1.Field:字段指针.
      2.如果传入的指针无效,则返回-1.
     
[out]:无.

返回类型：int

制作：YTLI   2002/07/15

修改: 
***********************************************************************************/
int CFields::IndexOf(CField* Field)
{
	if(!Field)
		return -1;

	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		CField* pField =(CField*) m_FieldArray.GetAt(i);
		if( Field == pField )
			return i;
	}
	return -1;
	/*
	POSITION pos = m_FieldList.GetHeadPosition();
	while(pos)
	{
		CField* pField = m_FieldList.GetNext(pos);
		if( Field == pField )
			return i;
		i++;
	}

	return -1;
	*/
}

/************************************************************************************
函数名称：
int CFields::LoadFromBuffer(LPBYTE& lpData,int FieldCount)
功能说明：从Buffer读取指定个数的字段信息.

详细解释：1.返回读取的字节数.
          2.如果传入的参数无效,则返回-1.
    
出入参数：
[in]: 1.lpData:要读的起始地址.
      2.FieldCount:要读取的字段个数.	  
     
[out]:1.lpData:传入参数lpData向后移动FieldCount个记录后的地址.

返回类型：int

制作：YTLI   2002/07/15

修改: 
***********************************************************************************/
int CFields::LoadFromBuffer(LPBYTE& lpData,int FieldCount)
{
	if( (!lpData) || (FieldCount <=0) )
        return -1;

	m_nCurVersion    = 0;
	m_nLastPosVersion = -1;

	int iRetValue = (int)lpData;
	int iFieldCnt = 0;

	while( iFieldCnt < FieldCount )
	{
		CFieldType chType = GetFieldType(lpData);
		CField* pField   = NewFieldType(this,chType);

		if(!pField)
			return -1;

		pField->m_pFields = this;
		lpData += pField->LoadFromBuffer(lpData);

		Add(pField);
		
		iFieldCnt++;
	}

	iRetValue = (int)lpData - iRetValue;
	return iRetValue;
}

/************************************************************************************
函数名称：
int CFields::SaveToBuffer(LPBYTE& lpData)
功能说明：将字段集m_FieldList保存到Buffer中.

详细解释：1.返回保存的字节数.
          2.如果传入指针无效,则返回-1.
    
出入参数：
[in]: 1.lpData:要保存到Buffer的起始地址.    
     
[out]:1.lpData:保存后的地址.

返回类型：int

制作：YTLI   2002/07/15

修改: 
***********************************************************************************/
/*
int CFields::SaveToBuffer(LPBYTE& lpData)
{
	if(!lpData)
		return -1;

	int iRetValue = (int)lpData;
	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		int nOffset;
		CField* pField = m_FieldArray.GetAt(i);
		if(pField)
			lpData += pField->SaveToBuffer_(lpData);

	}

	iRetValue = (int)lpData - iRetValue;
	return iRetValue;
}
*/

int CFields::SaveToBuffer(TCLFile *pFile)
{
	if(!pFile)
		return false;

	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		CField* pField = (CField*)m_FieldArray.GetAt(i);
		if(pField)
			pField->SaveToBuffer(pFile);

	}

	return true;
}
/************************************************************************************
函数名称：
CField* CFields::NewFieldType(CField* clone)
功能说明：根据传入的字段生成相应的字段类型.

详细解释：1.返回新生成的字段.
    
出入参数：
[in]: 1.clone:传入的字段指针.    
     
[out]:无.

返回类型：CField*

制作：YTLI   2002/07/15

修改: 
***********************************************************************************/
CField* CFields::NewFieldType(CFields* pOwner ,CField* clone)
{
	if(!clone)
		return NULL;

	CFieldType chType = clone->GetFieldType();
	CField* pField = NewFieldType(pOwner,chType);

	return pField;
}

/************************************************************************************
函数名称：
CField* CFields::NewFieldType(CFieldType chType)
功能说明：根据传入的字段类型生成相应的字段类型.

详细解释：1.返回新生成的字段.
          2.如果传入的chType无效,则返回NULL.
    
出入参数：
[in]: 1.chType:传入的字段类型.    
     
[out]:无.

返回类型：CField*

制作：YTLI   2002/07/15

修改: 
***********************************************************************************/
CField* CFields::NewFieldType(CFields* pOwner ,CFieldType chType)
{
	CField* pField = NULL;//如果传入的chType无效,则返回NULL.

	switch(chType)
	{
	case fString :
		pField = new CStringField(this);
		break;
	case fDouble  :
		pField = new CDoubleField(this);
		break;
	case fInt :
		pField = new CIntField(this);
		break;
	case fBoolean :
		pField = new CBooleanField(this);
		break;
	case fDate :
		pField = new CDateField(this);
		break;
	case fCurrency :
		pField = new CCurrencyField(this);
		break;
	}
	
	CTString str;//###_Mod 2002-9-24 zlq  缺省字段名
	int nCount = m_FieldVersion.GetAbsoluteFieldCount();
	do
	{//#_S 2003-5-28 $ 9 zlq 确保生成时,就唯一
		str.Format("%d",nCount+1);
		while (str.GetLength()<4)
		{//#_修改 2002-11-15 $ 9:15:44 zlq
			str = "0"+str;
		}
		str = "变量" +str;
		nCount++;
	}while (FieldByName(str)!=NULL);
	pField->SetFieldName(str);
	pField->m_pFields = pOwner;
	return pField ;
}

/************************************************************************************
函数名称：
void CFields::Clone(CFields* pFields)
功能说明：根据传入的字段集生成一个一样的字段集.

详细解释：
    
出入参数：
[in]: 1.pFields:传入的字段集.    
     
[out]:无.

返回类型：void

制作：YTLI   2002/07/15

修改: 
数据克隆，
根据传入的字段链表样板克隆本字段链表的基本信息，
内部调用个字段的克隆方法
***********************************************************************************/
void CFields::Clone(CFields* pFields)
{	
	if(!pFields)
		return;

	m_pOldDataSet     = pFields->m_pOldDataSet;
	m_pDataSet        = pFields->m_pDataSet;

	m_nCurVersion     = pFields->m_nCurVersion;
	m_nLastPosVersion = pFields->m_nLastPosVersion;

	m_FieldVersion.Clone(&pFields->m_FieldVersion);
}

/************************************************************************************
函数名称：
CFieldType CFields::GetFieldType(LPBYTE lpData)
功能说明：得到字段类型,用于在生成字段时.

详细解释：
    
出入参数：
[in]: 1.lpData:Buffer中该字段的首地址.    
     
[out]:无.

返回类型：CFieldType.

制作：YTLI   2002/07/15

修改: 
***********************************************************************************/
CFieldType CFields::GetFieldType(LPBYTE lpData)
{
	if(!lpData)
		return 0x0;

	return *(lpData);
}

/************************************************************************************
函数名称：
void CFields::NewData() 
功能说明：为一条记录分配一个新空间.

详细解释：
    
出入参数：
[in]: 无.
     
[out]:无.

返回类型：void.

制作：YTLI   2002/07/15

修改: 
***********************************************************************************/
void CFields::NewData() 
{
	int nOldVersion = m_nCurVersion;
	SetCurVersion();

	ClearAllVersionPointer();

	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		CField* pField = (CField*)m_FieldArray.GetAt(i);
		if(pField)
			pField->NewData();
	}

	SetCurVersion(nOldVersion);	
}

/************************************************************************************
函数名称：
void CFields::DeleteData()
功能说明：删除当前记录.

详细解释：
    
出入参数：
[in]: 无.
     
[out]:无.

返回类型：void.

制作：YTLI   2002/07/15

修改: 
***********************************************************************************/
void CFields::DeleteData()
{	
	int nOldVersion = m_nCurVersion;

	SetCurVersion();

	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		CField* pField = (CField*)m_FieldArray.GetAt(i);
		if(pField)
			pField->DeleteData();
	}
/*
	POSITION pos = m_FieldList.GetHeadPosition();
	while(pos)
	{
		CField* pField = m_FieldList.GetNext(pos);
		
		if(pField)
			pField->DeleteData();
	}
*/
	SetCurVersion(nOldVersion);		
}

/************************************************************************************
函数名称：
CField* CFields::FieldByIndex(int index)
功能说明：根据索引值得到字段信息.

详细解释：
    
出入参数：
[in]: 1.index:索引值.
     
[out]:无.

返回类型：CField*.

制作：YTLI   2002/07/15

修改: 
***********************************************************************************/
CField* CFields::FieldByIndex(int index)
{
	if(index<0)
		return NULL;
	if (index>=m_FieldArray.GetSize())
	{
		return NULL;
	}

	return (CField*)m_FieldArray.GetAt(index);
}

/************************************************************************************
函数名称：

功能说明：将一条记录保存到Buffer中.

详细解释：1.内部对每一个字段调用CField::SaveDataToBuffer().
         2.返回保存的到Buffer中的字节数.
    
出入参数：
[in]: 1.lpData:要保存到Buffer中的起始地址.
      2.如果传入指针无效,则返回-1.

[out]:无.

返回类型：long.

制作：YTLI   2002/07/15

修改: 
***********************************************************************************/
long CFields::SaveDataToBuffer(LPBYTE lpData)
{
	if(!lpData)
		return -1;

	int iRetValue = (int)lpData;                 //将地址差作为引用返回值
	
	int iFieldCnt  = m_FieldArray.GetSize();
	LPBYTE lpData2 = lpData;

	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		CField* pField = (CField*)m_FieldArray.GetAt(i);
		if(!pField)
			return -1;
        
		pField->SaveDataToBuffer(lpData2);	//引用传值
	}

	iRetValue = (int)(lpData2 - lpData);
	return iRetValue;
}

/************************************************************************************
函数名称：
bool CFields::LoadDataFromBuffer(LPBYTE& lpData, int& count)
功能说明：从Buffer中读取一条记录.

详细解释：1.内部对每一个字段调用CField::LoadDataFromBuffer().
          2.如果传入指针无效,则返回FALSE.
    
出入参数：
[in]: 1.lpData:从Buffer中读取的起始地址.
     
[out]:1.lpData:下一条记录的首地址.
      2.count:读取的字节数.

返回类型：long.

制作：YTLI   2002/07/15

修改: 
***********************************************************************************/
bool CFields::LoadDataFromBuffer(LPBYTE& lpData, int& count)
{
	if( !lpData )
		return false;


	int iFieldCnt  = m_FieldArray.GetSize();
	LPBYTE lpData2 = lpData ;

	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		CField* pField = (CField*)m_FieldArray.GetAt(i);
		if(!pField)
			return false;

		pField->LoadDataFromBuffer(lpData2);
	}
	count = (int)( lpData2 - lpData);
	lpData = lpData2;

	return true;
}

/************************************************************************************
函数名称：
int CFields::QuerySaveLength()
功能说明：得到保存一条记录到Buffer所需要的字节数.

详细解释：1.内部对每一个字段调用CField::QuerySaveLength().
          2.如果函数执行失败,则返回-1.
    
出入参数：
[in]: 无.
     
[out]:无.

返回类型：int.

制作：YTLI   2002/07/15

修改: 
***********************************************************************************/
int CFields::QueryLength()
{
	int iRetValue = 0; 
	for (int i= 0;i<m_FieldArray.GetSize();i++)
	{
		CField* pField = (CField*)m_FieldArray.GetAt(i);
		if(!pField)
			return -1;

		iRetValue += pField->QuerySaveLength();
	}
	/*
	POSITION pos  = m_FieldList.GetHeadPosition();

	while(pos)
	{
		CField* pField = m_FieldList.GetNext(pos);
		
		if(!pField)
			return -1;

		iRetValue += pField->QuerySaveLength();
	}
	*/
	return iRetValue;
}

/************************************************************************************
函数名称：
int CFields::GetFieldCount()
功能说明：得到字段集所拥有的字段个数.

详细解释：
    
出入参数：
[in]: 无.
     
[out]:无.

返回类型：int.

制作：YTLI   2002/07/15

修改: 
***********************************************************************************/
int CFields::GetFieldCount()
{
	return m_FieldArray.GetSize();
}



//清除字段集m_FieldList中所有字段,针对所有版本.
void CFields::ClearAll()
{
	m_nCurVersion    = 0;
	m_nLastPosVersion = -1;

	m_FieldVersion.Clear();
	m_FieldArray.RemoveAll();
}

//删除在nIndex处的一个字段.
bool CFields::Delete(int nIndex )
{
	m_FieldVersion.Delete(nIndex);
	SetCurVersion();
	return true;
}

//在nIndex处插入一个字段.
void CFields::Insert(CField* pField , int nIndex)
{
	m_FieldVersion.Insert(pField , nIndex);
	SetCurVersion();
}

//字段类型修改
void CFields::Modify(int nIndex, CField* pNewField)
{	
	m_FieldVersion.Modify(nIndex,FieldByIndex(nIndex), pNewField);
///////////////////////////////////////////////{{lps added.2003/02/18
	CField* pOldFld = FieldByIndex(nIndex);

	LPBYTE pOldValue = pNewField->m_pValue;

	POSITION posData = pNewField->m_ValueLabel.m_DataList.GetHeadPosition();	
	
	while(posData)
	{
		pNewField->m_pValue = (LPBYTE)pNewField->m_ValueLabel.m_DataList.GetNext( posData );
		pNewField->RealDeleteData();	
	}
	pNewField->m_ValueLabel.m_DataList.RemoveAll();
	pNewField->m_ValueLabel.m_strLabelList.RemoveAll();

	posData = pOldFld->m_ValueLabel.m_DataList.GetHeadPosition();
    int posStr  = pOldFld->m_ValueLabel.m_strLabelList.GetSize();
    int nPos = 0;
	while(posData != NULL && nPos < posStr)
	{

 		LPBYTE pData = (LPBYTE)pOldFld->m_ValueLabel.m_DataList.GetNext( posData );
		CTString strLabel = pOldFld->m_ValueLabel.m_strLabelList.GetAt(nPos);
		pOldFld->m_ValueLabel.m_pField->m_pValue = pData;//~~~~~~~~~~~~~~~~~~~~~~~~~
		pNewField->RealNewData();
		//从日期字段转化到其他类型,或者从其他字段转化到日期字段.
		if((pOldFld->GetFieldType() == fDate)
			||(pNewField->GetFieldType() == fDate))
		{
			FILETIME ftValue = pOldFld->m_ValueLabel.m_pField->GetAsDateTime();
			//填入不成功,则m_pValue无效.
			if(!pNewField->SetAsDateTime(&ftValue))
			{
				pNewField->m_pValue = pOldValue;
				pNewField->RealDeleteData();
				continue;
			}
		}
		else
		{
			CTString strr = pOldFld->m_ValueLabel.m_pField->GetAsString();
			//填入不成功,则m_pValue无效.此时的原则:留下可以转换的,抛弃无法转化的.
			//比如fString型串"2002/01/30"转换为fInt,fDouble等时将为false.
			if(!pNewField->SetAsString(strr))
			{
				pNewField->m_pValue = pOldValue;
				pNewField->RealDeleteData();
				continue;
			}
		}
	
		pNewField->m_ValueLabel.m_DataList.AddTail(pNewField->m_pValue);

        //取得label字符串然后添加在当前对象中相应的链表中
	//	CTString str = pValueLabel->m_strLabelList.GetNext(posStr);
		pNewField->m_ValueLabel.m_strLabelList.Add(strLabel);
		nPos++;
	}
	//assert(posData == NULL && posStr == NULL);
	pNewField->m_pValue = pOldValue;
///////////////////////////////////////////////}}
	SetCurVersion();
}

//得到当前最新的版本,它区别于GetLastPostVersion得到的版本。
//它与等于GetLastPostVersion（），但相差最多为1
//返回: 版本号
int CFields::GetLastVersion()
{
	//assert(m_FieldVersion.GetLastVersion()==GetLastPostVersion() ||
	//	   m_FieldVersion.GetLastVersion()==GetLastPostVersion()+1 );

	return m_FieldVersion.GetLastVersion();
}

//得到内存里有提交的数据的最新版本。
//它小于等于GetLastVersion（），但相差最多为1.
//返回: 版本号
int CFields::GetLastPostVersion()
{
	//assert(m_FieldVersion.GetLastVersion() == m_nLastPosVersion ||
	//	   m_FieldVersion.GetLastVersion() == m_nLastPosVersion + 1 );

	return m_nLastPosVersion; 
}

//设置版本,默认为最新版本.用来装载旧版本数据.但不生成新的空间.
void CFields::SetCurVersion(int nVersion)
{
	if (nVersion == -1)
		nVersion = m_FieldVersion.GetLastVersion();

	m_FieldVersion.SetVersion(m_FieldArray,nVersion);
	m_nCurVersion = nVersion;

}

//得到当前字段的版本,从零开始.
int CFields::GetCurVersion()
{
	return m_nCurVersion;
}

//判断当前字段是否在最新字段中.
bool CFields::IsConvertToLastVersion(CField* pField)
{
	return m_FieldVersion.IsConvertToLastVersion(pField);
}



//检查更新的字段和没有原来的字段间的差别，以确定是否需要升级版本。
//返回
//true 需要更新
//false 不需要
bool CFields::IsNeedUpdateVersion(int nIndex, CField* pNewField )
{
	if(pNewField == FieldByIndex(nIndex))
		return false;
	
	return true;
}

//将所有版本的字段的数据指针设置为NULL，调用CFieldsVersion的该方法实现.
void CFields::ClearAllVersionPointer()
{
	m_FieldVersion.ClearAllVersionPointer();
}

CFields::CFields(CPages* pOwner)
{	
	CFields *pthis = this;
#ifdef MarkWay_Encrypt  //是加密版本
	BYTE *pData3 = (unsigned char *)&pthis;
	for (int i= 0;i<15 ;i++)
		*(pData3 + i % 4 ) = *(pData3 + i % 4 )^tlcfgBuf[i];
#endif	
	m_pPages = pOwner;
#ifdef MarkWay_Encrypt  //是加密版本
	BYTE *pData4 = (unsigned char *)&pthis;
	for ( i= 0;i<15 ;i++)
		*(pData4 + i % 4 ) = *(pData4 + i % 4 )^CurFileBuf[i];
#endif	

	m_FieldVersion.SetFields(pthis);
#ifdef MarkWay_Encrypt  //是加密版本
	BYTE *pData = (unsigned char *)&m_pPages;
	for ( i= 0;i<15 ;i++)
	{
		*(pData + i % 4 ) = *(pData + i % 4 )^tlcfgBuf[i];
	}
#endif


	m_pOldDataSet    = 0;
	m_pDataSet       = 0;
	m_nCurVersion    = 0;
	m_nLastPosVersion = -1;
	m_FieldArray.SetSize(0,_IncreaseNum);

#ifdef MarkWay_Encrypt  //是加密版本
	BYTE *pData2 = (unsigned char *)&m_pPages;
	for ( i= 0;i<15 ;i++)
	{
		*(pData2 + i % 4 ) = *(pData2 + i % 4 )^CurFileBuf[i];
	}
#endif
}

int CFields::GetAbsoluteFieldCount()
{//###_ADD 2002-9-24 zlq
	return m_FieldVersion.GetAbsoluteFieldCount();
}

/************************************************************************************
函数名称：

功能说明：字段移动

出入参数：
[in]: 
      1.nFrom,原来的索引号
	  2.nTo,移动到的新的索引号(注意是相对老字段还在时的位置)
返回类型:void.

制作：//#_S 2003-9-12 $ 10 zlq

修改: 
************************************************************************************/
void CFields::Mov(int nFrom, int nTo)
{
	m_FieldVersion.Mov(nFrom,nTo);
	SetCurVersion();
}
