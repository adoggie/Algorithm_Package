// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "fieldlist.h"
/************************************************************************************
函数名称：
	CFieldList::~CFieldList
功能说明：
	析构函数
详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2002-9-4
修改:

************************************************************************************/

CFieldList::~CFieldList()
{
}

/************************************************************************************
函数名称：
	CFieldList::CFieldList
功能说明：
	构造函数
详细解释：
	由CDataAccess内部调用.
出入参数：
	[in]pPages:
返回类型：

制作：
	Eric 2002-9-4
修改:

************************************************************************************/
CFieldList::CFieldList(CPages * pPages)
{
	m_pPages = pPages;
}
/************************************************************************************
函数名称：
	CFieldList::Add
功能说明：
	增加一个字段
详细解释：

出入参数：
	[in] :新增加的字段指针,可调用CreateField产生.
返回类型：

制作：
	Eric 2002-9-4
修改:
************************************************************************************/
bool CFieldList::Add(CField* pField)
{//#_修改 2002-11-15 $ 11:15:24 zlq
	if (m_pPages->m_pFields->IndexOf(pField)>=0)
	{//已经存在于该字段集中
		////===temp AfxMessageBox("该字段已经存在于该字段集中!");
		return false;
	}
	if (m_pPages->m_pFields->FieldByName(pField->GetFieldName()))
	{//已经有相同名字存在于该字段集中
		////===temp AfxMessageBox("已经有相同名字存在于该字段集中!");
		return false;
	}
	m_pPages->AddField(pField);
	return true;
}
/************************************************************************************
函数名称：
	CFieldList::Clear
功能说明：
	清除字段集m_FieldList中所有字段,即当前版本
详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2002-9-4
修改:

************************************************************************************/
void CFieldList::Clear()
{
	while (m_pPages->m_pFields->GetFieldCount())
		Delete(0);
}
/************************************************************************************
函数名称：
	CFieldList::Delete
功能说明：
	删除一个字段
详细解释：

出入参数：
	[in]nIndex :要删除字段在字段集中的索引
返回类型：

制作：
	Eric 2002-9-4
修改:

************************************************************************************/
bool CFieldList::Delete(int nIndex)
{
	m_pPages->DeleteField(nIndex);
	return true;
}
/************************************************************************************
函数名称：
	CFieldList::Insert
功能说明：
	插入一个字段
详细解释：

出入参数：
	[in,out]pField :要插入的字段
	[in,out] nIndex	:插入的位置
返回类型：

制作：
	Eric 2002-9-4
修改:

************************************************************************************/
bool CFieldList::Insert(CField * pField,int nIndex)
{//#_修改 2002-11-15 $ 11:16:02 zlq
	if (m_pPages->m_pFields->IndexOf(pField)>=0)
	{//已经存在于该字段集中
		//===temp AfxMessageBox("该字段已经存在于该字段集中!");
		return false;
	}
	if (m_pPages->m_pFields->FieldByName(pField->GetFieldName()))
	{//已经有相同名字存在于该字段集中
		//===temp AfxMessageBox("已经有相同名字存在于该字段集中!");
		return false;
	}
	m_pPages->InsertField(nIndex,pField);
	return true;
}
/************************************************************************************
函数名称：
	CFieldList::Modify
功能说明：
	修改一个字段
详细解释：

出入参数：
	[in,out]nIndex:旧字段的位置
	[in,out]pField :新的字段
返回类型：

制作：
	Eric 2002-9-4
修改:

************************************************************************************/
bool CFieldList::Modify(int nIndex,CFieldType NewFieldType)
{
	CField *pField = this->FieldByIndex(nIndex);
	if (pField ==NULL)
		return false;
	if (pField->GetFieldType() == NewFieldType)
		return true;
	CField* pNewField=CreateField(NewFieldType);
	pNewField->CopyCommonProperty(pField);
	Modify(IndexOf(pField),pNewField);
	return true;
}
bool CFieldList::Modify(int nIndex,CField * pField)
{//#_修改 2002-11-15 $ 11:18:36 zlq
	if (m_pPages->m_pFields->IndexOf(pField)>=0)
	{//已经存在于该字段集中
		//===temp AfxMessageBox("该字段已经存在于该字段集中!");
		return false;
	}
	CField *pTemp = m_pPages->m_pFields->FieldByName(pField->GetFieldName());
	if (pTemp)
	{//已经有相同名字存在于该字段集中

		if (m_pPages->m_pFields->IndexOf(pTemp) !=nIndex)
		{
			//===temp AfxMessageBox("已经有相同名字存在于该字段集中不同的位置!");
			return false;
		}
	}
	m_pPages->ModifyField(nIndex,pField);
	return true;
}
/************************************************************************************
函数名称：
	CFieldList::FieldByName
功能说明：
	由字段名称取得字段
详细解释：

出入参数：
	[in]FieldName:字段名称
返回类型：

制作：
	Eric 2002-9-4
修改:

************************************************************************************/
CField* CFieldList::FieldByName(CTString FieldName)
{
	return m_pPages->m_pFields->FieldByName(FieldName);
}
/************************************************************************************
函数名称：
	CFieldList::FieldByIndex
功能说明：
	由序号取得字段
详细解释：

出入参数：
	[in]index:字段序号
返回类型：

制作：
	Eric 2002-9-4
修改:

************************************************************************************/
CField* CFieldList::FieldByIndex(int index)
{
	return m_pPages->m_pFields->FieldByIndex(index);
}
/************************************************************************************
函数名称：
	CFieldList::IndexOf
功能说明：
	取得某字段的位置（从零开始）    失败，则返回-1.
详细解释：

出入参数：
	[in]Field :字段指针
返回类型：

制作：
	Eric 2002-9-4
修改:

************************************************************************************/
int CFieldList::IndexOf(CField* pField)
{
	return m_pPages->m_pFields->IndexOf(pField);
}

/************************************************************************************
函数名称：
	CFieldList::GetFieldCount
功能说明：
	取得字段数
详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2002-9-4
修改:

************************************************************************************/
int CFieldList::GetFieldCount()
{
	return 	m_pPages->m_pFields->GetFieldCount();
}
/************************************************************************************
函数名称：
	CFieldList::CreateField
功能说明：
	新建一个Field
详细解释：

出入参数：
	[in]FieldType:字段类型
返回类型：

制作：
	Eric 2002-9-4
修改:

************************************************************************************/
CField* CFieldList::CreateField(CFieldType FieldType)
{
	return 	m_pPages->m_pFields->NewFieldType(m_pPages->m_pFields,FieldType);
}
/************************************************************************************
函数名称：
	CFieldList::CreateField
功能说明：
	新建一个字段
详细解释：

出入参数：
	[in] pTempleField:模板字段
返回类型：

制作：
	Eric 2002-9-4
修改:

************************************************************************************/
CField* CFieldList::CreateField(CField * pTempleField)
{
	return 	m_pPages->m_pFields->NewFieldType(m_pPages->m_pFields,pTempleField);
}

/******************************************************
功能说明：
详细解释：
出入参数：
返回类型：
制作：
修改:
******************************************************/
struct TVarInfo
{
	CTString	strName;
	CTString	strBaseAls;
	int count;
	
//	TVarInfo(){	memset(this, 0, sizeof(*this));}
	TVarInfo(CTString strName,CTString strBaseAls)
	{
		strName = strName;
		strBaseAls = strBaseAls;
		count = 0;
	}
	int ParseText(CTString str,int nCount)
	{
		if(str == "")
			return 0;
		
  		//char* pChar =str.GetBuffer(str.GetLength());
		//str.ReleaseBuffer();
		char* pChar =str.GetData();
		bool bSingleWChar = false;
		
		int nOffset;
		for(nOffset = 0;(nOffset<nCount && *pChar);nOffset++)
		{
			if(*pChar < 0)
			{
				bSingleWChar = ! bSingleWChar;
			}
			pChar++;
		}
		
		if(bSingleWChar)
		{
			return nOffset-1;
		}
		else
		{
			return nOffset;
		}   
	}
	
	CTString AdjustAlias()
	{
		int iNameSize  = this->strName.GetLength();
		
		if(iNameSize <= 6)//少于或等于3个
		{
			this->strBaseAls = this->strName;
		}
		else if((iNameSize > 6) && (this->count == 1))//多于或等于4个,且个数不到100(iNameSize <= 8) &&
		{
			int iCnt = iNameSize <= 10 ? iNameSize:10;
			iCnt = ParseText(strName,iCnt);
			this->strBaseAls = this->strName.Left(iCnt);//10
		}
		else if((iNameSize > 6) && (this->count < 100))//多于或等于4个,且个数不到100(iNameSize <= 8) &&
		{
			int iCnt = iNameSize <= 8 ? iNameSize:8;
			iCnt = ParseText(strName,iCnt);
			this->strBaseAls = this->strName.Left(iCnt);
		}
		else if((iNameSize > 6) && (this->count <= 9999))//多于或等于4个,且个数超过99,最多支持1000个表也可扩展
		{
			int iCnt = iNameSize <= 6 ? iNameSize:6;
			iCnt = ParseText(strName,iCnt);
			this->strBaseAls = this->strName.Left(6);
		}
		else if((iNameSize > 6) && (this->count <= 999999))
		{
			int iCnt = iNameSize <= 4 ? iNameSize:4;
			iCnt = ParseText(strName,iCnt);
			this->strBaseAls = this->strName.Left(iCnt);
		}
		else if((iNameSize > 6) && (this->count > 999999))
		{
			int iCnt = iNameSize <= 4 ? iNameSize:4;
			iCnt = ParseText(strName,iCnt);
			this->strBaseAls = this->strName.Left(iCnt);
		//	//===temp AfxMessageBox("您打开的数据表数目多于1000000个");
		}
		return this->strBaseAls;
	}
};
/************************************************************************************
函数名称：
	CFieldList::GetExclusiveName
功能说明：
	获得strFldName在字段集中的唯一字段名,用于字段有重名时获取.
详细解释：

出入参数：
	[in] :strFldName,字段名字.
返回类型：

制作：
修改:

************************************************************************************/
CTString CFieldList::GetExclusiveName(CField* pFld)
{
	CTString strOldName = pFld->GetFieldName();
	CTString strName(strOldName);
	int Cnt = 0;
	TVarInfo var(strName,"");
	var.strName = strName;
	while(FieldByName(strName))
	{
		Cnt++;
		var.count = Cnt;
		strName = var.AdjustAlias();
		strName.Format("%s%d",strName,Cnt);
	}
	return strName;
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
bool CFieldList::Mov(int nFrom, int nTo)
{
	int nCount = m_pPages->m_pFields->GetFieldCount();
	if (nFrom<0 || nFrom>=nCount)
	{//范围判断	
		return false;
	}
	if (nTo<0 || nTo>=nCount)
	{//范围判断
		return false;
	}

	m_pPages->MoveField(nFrom,nTo);
	return true;
}

//取得某字段的位置（从零开始）    失败，则返回-1.
int CFieldList::IndexOf(CTString strFieldName)
{
	CField *pField = FieldByName(strFieldName);
	if (pField ==NULL )
		return -1;
	return m_pPages->m_pFields->IndexOf(pField);
}
