// FieldsVersion.cpp: implementation of the CFieldsVersion class.
//
//////////////////////////////////////////////////////////////////////

#include "../headtldb.h"
//#include "TlDatabase.h"
#include "fieldsversion.h"
#include "cfields.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFieldsVersion::CFieldsVersion()
{
	m_FieldAllArray.SetSize(0,_IncreaseNum);
}

CFieldsVersion::~CFieldsVersion()
{
	Clear();	
}

/************************************************************************************
函数名称：
int CFieldsVersion::GetLastVersionByIndex(int nIndex)
功能说明：根据字段在字段链表中的索引号确定该字段最后存在时的版本.

详细解释:	

出入参数：
[in]:nIndex,字段在绝对字段链表中的索引号.      
  
[out]:无.

返回类型:int,最后存在时的版本号.

制作：YTLI 2002/08/28

修改: 
************************************************************************************/
int CFieldsVersion::GetLastVersionByIndex(int nIndex)
{
	//assert(nIndex > -1);

	int iCnt = m_VersionList.GetVersionCount();

	for(int i=iCnt-1; i>=0; i--)
	{
		CDWordArray* pDWORDArray = m_VersionList.GetVersion(i);
		//#_修改 2003-12-24 $ 11:33:43 zlq
		int nCount = pDWORDArray->GetSize();
		for (int iPos = 0; iPos < nCount; iPos++)
		{
			if( nIndex ==(int) (*pDWORDArray)[iPos])
				return iPos;			
		}

	}

	//assert(0);
	return -1;
}

/************************************************************************************
函数名称：
int CFieldsVersion::GetLastVersionByIndex(int nIndex)
功能说明：根据字段在字段链表中的索引号确定该字段最早存在时的版本.

详细解释:	

出入参数：
[in]:nIndex,字段在绝对字段链表中的索引号.      
  
[out]:无.

返回类型:int,最早存在时的版本号.

制作：YTLI 2002/08/28

修改: 
************************************************************************************/
int CFieldsVersion::GetFirstVersionByIndex(int nIndex)
{
	//assert(nIndex > -1);

	int iCnt = m_VersionList.GetVersionCount();

	for(int i=0; i<iCnt; i++)
	{			
		CDWordArray* pDWORDArray = m_VersionList.GetVersion(i);
		//#_修改 2003-12-24 $ 11:33:43 zlq
		int nCount = pDWORDArray->GetSize();
		for (int iPos = 0; iPos < nCount; iPos++)
		{
			if( nIndex ==(int) (*pDWORDArray)[iPos])
				return i;			
		}
	}


	//assert(0);
	return -1;
}

/************************************************************************************
函数名称：
void CFieldsVersion::SetVersion(CTypedPtrList<CObList, CField*>& FieldList, int nVersion)
功能说明：设置版本,默认为新版本.用来装载旧版本数据.但不生成新的空间,也不初始化其他版本字段的指针.

详细解释:	

出入参数：
[in]:1.FieldList,字段在绝对字段链表中的索引号.      
     2.nVersion,
[out]:无.

返回类型:void.

制作：YTLI 2002/08/28

修改: 
************************************************************************************/
void CFieldsVersion::SetVersion(CPtrArray & FieldArray, int nVersion)
{
	//清空FieldList.
	//FieldArray.RemoveAll();
	FieldArray.RemoveAt(0,FieldArray.GetSize());

	//得到所要的版本链表.
	CDWordArray* pDWORDArray = m_VersionList.GetVersion(nVersion);
	int nCount = pDWORDArray->GetSize();
	for (int iPos = 0; iPos < nCount; iPos++)
	{
		CField* pField =(CField*) m_FieldAllArray[ (*pDWORDArray)[iPos] ];
		FieldArray.Add(pField);//#_修改 2002-11-25 $ 13:15:03 zlq
	}

}

/************************************************************************************
函数名称：
bool CFieldsVersion::IsConvertToLastVersion(CField* pField)
功能说明：判断当前字段是否在最新字段中. 

详细解释:	

出入参数：
[in]:pField,字段指针.      
  
[out]:无.

返回类型:bool.

制作：YTLI 2002/08/29

修改: 
************************************************************************************/
bool CFieldsVersion::IsConvertToLastVersion(CField* pField)
{
	//assert(pField);

	int iNext = pField->m_nNextModifyField;
	//得到所连接的最新的一个字段
	while(iNext != -1)
	{		
		//pField = GetFieldByAbsolutePos(iNext);
		pField = (CField*)m_FieldAllArray[iNext];
		iNext = pField->m_nNextModifyField;
	};

	//根据字段指针从字段链表中得到所对应的位置号 iNodeNo
	//int iNodeNo = 0;
	int nCount = m_FieldAllArray.GetSize();
	int iPos = 0;
	for (iPos = 0; iPos < nCount; iPos++)
	{
		if(pField == m_FieldAllArray[iPos])
			break;
	}

	//得到最后一个字段版本链表
	CDWordArray* pDWORDArray = m_VersionList.GetVersion(m_VersionList.GetVersionCount()-1);
	//判断位置号是否在最新链表中
	nCount = pDWORDArray->GetSize();
	for (int iIndexPos = 0; iIndexPos < nCount; iIndexPos++)
	{
		if(iPos ==(int) (*pDWORDArray)[iIndexPos])
			return true;
	}


	return false;
}

/************************************************************************************
函数名称：
int CFieldsVersion::GetLastVersion()
功能说明：得到内存里有提交的数据的最新版本. 

详细解释:	1.它小于等于GetLastVersion()，但相差最多为1.

出入参数：
[in]:pField,字段指针.      
  
[out]:无.

返回类型:int,版本号.

制作：YTLI 2002/08/29

修改: 
************************************************************************************/
int CFieldsVersion::GetLastVersion()
{
	return m_VersionList.GetVersionCount()-1;
}

/************************************************************************************
函数名称：
void CFieldsVersion::Remove(CField* pField)
功能说明：删除最新版本链表中的一个字段.

详细解释:	1.根据字段指针来删除一个字段.

出入参数：
[in]:pField,字段指针.      
  
[out]:无.

返回类型:void.

制作：YTLI 2002/08/29

修改: 
************************************************************************************/
void CFieldsVersion::Remove(CField* pField)
{//===未用,未看
	//===temp AfxMessageBox("暂时未实现,未用");
	int nCount = m_FieldAllArray.GetSize();
	int iPos = 0;
	for (iPos = 0; iPos < nCount; iPos++)
	{
		if(pField == m_FieldAllArray[iPos])
			break;
	}

	CDWordArray* pDWORDArray = m_VersionList.GetVersion(m_VersionList.GetVersionCount()-1);
	
	
	nCount = pDWORDArray->GetSize();
	for (int iIndexPos = 0; iIndexPos < nCount; iIndexPos++)
	{
		if( iPos ==(int) (*pDWORDArray)[iIndexPos])
		{
			pDWORDArray->RemoveAt(iIndexPos);
		}
	}

}

/************************************************************************************
函数名称：
bool CFieldsVersion::Delete(int nIndex)
功能说明：删除在nIndex处的一个字段.

详细解释:	1.检查m_LastPosVersion与GetLastVersion()的一致性决定是否需要
            从上面版本创建一个新版本,以及初始化的问题.

出入参数：
[in]:nIndex,字段索引.      
  
[out]:无.

返回类型:bool.

制作：YTLI 2002/08/29

修改: 
************************************************************************************/
bool CFieldsVersion::Delete(int nIndex)
{
	if( GetLastVersion() == m_pFields->GetLastPostVersion() )
	{
		m_VersionList.AddNewVersion();	
	}

	CDWordArray* pDWORDArray = m_VersionList.GetVersion(GetLastVersion());
	pDWORDArray->RemoveAt(nIndex);

	return true;
}

/************************************************************************************
函数名称：
void CFieldsVersion::Insert(CField* pField, int nIndex)
功能说明：在nIndex处插入一个字段.

详细解释:	1.检查m_LastPosVersion与GetLastVersion()的一致性决定是否需要
            从上面版本创建一个新版本,以及初始化的问题.

出入参数：
[in]: 1.pField,新的字段指针.
	  2.nIndex,插入的索引位置.
  
[out]:无.

返回类型:void.

制作：YTLI 2002/08/29

修改: 
************************************************************************************/
void CFieldsVersion::Insert(CField* pField, int nIndex)
{
	if( GetLastVersion() == m_pFields->GetLastPostVersion() )
	{
		m_VersionList.AddNewVersion();	
	}

	//m_FieldList.AddTail(pField);
	m_FieldAllArray.Add(pField);
	CDWordArray* pDWORDArray = m_VersionList.GetVersion(GetLastVersion());
	pDWORDArray->InsertAt(nIndex,m_FieldAllArray.GetSize()-1);

}

/************************************************************************************
函数名称：
void CFieldsVersion::Modify(int nIndex,CField* pOldField, CField* pNewField)
功能说明：字段类型修改.

详细解释:	1.检查m_LastPosVersion与GetLastVersion()的一致性决定是否需要
            从上面版本创建一个新版本,以及初始化的问题.

出入参数：
[in]: 1.nIndex,相对的索引位置.
      2.pOldField,旧的字段指针.
	  3.pNewField,新的字段指针.
  
[out]:无.

返回类型:void.

制作：YTLI 2002/08/29

修改: 
************************************************************************************/
void CFieldsVersion::Modify(int nIndex,CField* pOldField, CField* pNewField)
{
	if( GetLastVersion() == m_pFields->GetLastPostVersion() )
	{
		m_VersionList.AddNewVersion();	
	}

	//m_FieldList.AddTail(pNewField);
	m_FieldAllArray.Add(pNewField);
	int nNewIndex = m_FieldAllArray.GetSize()-1;

	pOldField->m_nNextModifyField = nNewIndex;

	CDWordArray* pDWORDArray = m_VersionList.GetVersion(GetLastVersion());	
	(*pDWORDArray)[nIndex] = nNewIndex;

}

/************************************************************************************
函数名称：
void CFieldsVersion::Clone(CFieldsVersion* pFieldsVersion)
功能说明：数据克隆，根据传入的字段链表样板克隆本字段链表的基本信息.

详细解释:	1.内部调用每个字段的克隆方法.

出入参数：
[in]: 1.pFieldsVersion.
     
[out]:无.

返回类型:void.

制作：YTLI 2002/08/29

修改: 
************************************************************************************/
void CFieldsVersion::Clone(CFieldsVersion* pFieldsVersion)
{
	Clear();
	int nCount = pFieldsVersion->m_FieldAllArray.GetSize();
	for (int iPos = 0; iPos < nCount; iPos++)
	{
		//if(pField == m_FieldAllArray[iPos])
		CField* pField =(CField*) pFieldsVersion->m_FieldAllArray[iPos];
		CField* pNewField = pFieldsVersion->m_pFields->NewFieldType(m_pFields,pField);
		
		pNewField->Clone( pField );		
		m_FieldAllArray.Add(pNewField);
	}

	m_VersionList.Clone(&pFieldsVersion->m_VersionList);
}


/************************************************************************************
函数名称：
void CFieldsVersion::Clear()
功能说明：清除字段集m_FieldList中所有字段.以及版本链.

详细解释:	

出入参数：
[in]: 1.FieldList,输入的一串字段.
     
[out]:无.

返回类型:bool.

制作：YTLI 2002/08/29

修改: 
************************************************************************************/
void CFieldsVersion::Clear()
{
	//删除字段集m_FieldList中所有字段.
	while( m_FieldAllArray.GetSize())
	{
		CField* pField =(CField*) m_FieldAllArray[0];
		m_FieldAllArray.RemoveAt(0);
		delete pField;
	}

	//删除版本链
	m_VersionList.Clear();

}

/************************************************************************************
函数名称：
void CFieldsVersion::ClearAllVersionPointer()
功能说明：将所有的字段的数据指针设置为NULL.

详细解释:	1.将所有的字段的m_pValue设置为NULL.

出入参数：
[in]: 
     
[out]:无.

返回类型:void.

制作：YTLI 2002/08/29

修改: 
************************************************************************************/
void CFieldsVersion::ClearAllVersionPointer()
{
	int nCount = m_FieldAllArray.GetSize();
	for (int iPos = 0; iPos < nCount; iPos++)
	{
		CField*pField = (CField*)m_FieldAllArray[iPos];
		pField->m_pValue = NULL;
	}

}

/************************************************************************************
函数名称：
CField* CFieldsVersion::GetFieldByAbsolutePos(int nIndex)
功能说明：根据绝对索引号得到字段指针.

详细解释:	

出入参数：
[in]: 1.nIndex,
     
[out]:无.

返回类型:CField*.

制作：YTLI 2002/08/29

修改: 
************************************************************************************/
CField* CFieldsVersion::GetFieldByAbsolutePos(int nIndex)
{
	//assert(nIndex >= 0);
	return (CField*)m_FieldAllArray[nIndex];

}

CFields* CFieldsVersion::SetFields(CFields *pFields)
{
	CFields *ptemp = pFields;
	m_pFields = pFields;
	return ptemp;
}

/************************************************************************************
函数名称：
void CFieldsVersion::Add(CField* pField)
功能说明：添加一个字段,从末尾添加.

详细解释:

出入参数：
[in]: 1.pField,
     
[out]:无.

返回类型:void.

制作：YTLI 2002/08/29

修改: 
************************************************************************************/
void CFieldsVersion::Add(CField* pField)
{
	if( GetLastVersion() == m_pFields->GetLastPostVersion() )
	{
		m_VersionList.AddNewVersion();        
	}

	//m_FieldList.AddTail(pField);
	m_FieldAllArray.Add(pField);

	CDWordArray* pDWORDArray = m_VersionList.GetVersion(GetLastVersion());
	pDWORDArray->Add(m_FieldAllArray.GetSize()-1);
}



int CFieldsVersion::GetAbsoluteFieldCount()
{//###_ADD 2002-9-24 zlq
	return m_FieldAllArray.GetSize();
}
/************************************************************************************
函数名称：
void CFieldsVersion::Modify(int nIndex,CField* pOldField, CField* pNewField)
功能说明：字段移动.

详细解释:	1.检查m_LastPosVersion与GetLastVersion()的一致性决定是否需要
            从上面版本创建一个新版本,以及初始化的问题.

出入参数：
[in]: 
      1.nFrom,原来的索引号
	  2.nTo,移动到的新的索引号(注意是相对老字段还在时的位置)
  
[out]:无.

返回类型:void.
制作：//#_S 2003-9-12 $ 9 zlq

修改: 
************************************************************************************/
void CFieldsVersion::Mov(int nFrom, int nTo)
{
	if (nFrom == nTo)
		return;
	if( GetLastVersion() == m_pFields->GetLastPostVersion() )
	{
		m_VersionList.AddNewVersion();	
	}

	CDWordArray* pDWORDArray = m_VersionList.GetVersion(GetLastVersion());
	if (nFrom < nTo)
		nTo--;//如果移动到后面,则先减位置号
	
	DWORD value = (*pDWORDArray)[nFrom] ;
	pDWORDArray->RemoveAt(nFrom);//从老位置去掉
	pDWORDArray->InsertAt(nTo,value);//移动到新位置

}
