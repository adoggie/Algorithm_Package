// VersionList.cpp: implementation of the CVersionList class.
//
//////////////////////////////////////////////////////////////////////

#include "../headtldb.h"
//#include "TlDatabase.h"
#include "versionlist.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVersionList::CVersionList()
{	
}

CVersionList::~CVersionList()
{
	Clear();
}

/************************************************************************************
函数名称：
DWORDList* CVersionList::GetVersion(int VerNo)
功能说明：得到指定的版本号链表.

详细解释:	1.如果指定的版本号无效,则返回NULL.

出入参数：
[in]:VerNo:版本号.      
  
[out]:无.

返回类型:DWORDList*,要得到的版本号链表.


制作：YTLI 2002/08/28

修改: 
************************************************************************************/
CDWordArray* CVersionList::GetVersion(int VerNo)
{
	//assert( VerNo > -1 );
	
	//CDWordArray* pDWORDArray = (CDWordArray* )m_VerDataArray[VerNo];
	return (CDWordArray* )m_VerDataArray[VerNo];

}

/************************************************************************************
函数名称：
int	CVersionList::GetVersionCount()
功能说明：得到版本个数.

详细解释:	1.直接返回版本链表中版本的个数.

出入参数：
[in]:无. 
  
[out]:无.

返回类型:int,版本个数.

制作：YTLI 2002/08/28

修改: 
************************************************************************************/
int	CVersionList::GetVersionCount()
{
	return m_VerDataArray.GetSize();
}

/************************************************************************************
函数名称：
int CVersionList::AddNewVersion()
功能说明：添加一个版本，内容为上一个版本的克隆  返回版本号 以零开始

详细解释:	1.如果版本链表为空,则添加一个新的版本号链.
        2.如果版本链表不为空,则将最新的版本克隆,并添加到版本链表的尾部.

出入参数：
[in]:无.      
  
[out]:无.

返回类型:int,最新的版本号.


制作：YTLI 2002/08/28

修改: 
************************************************************************************/
int CVersionList::AddNewVersion()
{

	if (m_VerDataArray.GetSize()==0)
	{
		CDWordArray* pNewVersion = new CDWordArray();                
		m_VerDataArray.Add(pNewVersion);
	}
	else
	{
		CDWordArray* pDWORDArray  = (CDWordArray*)m_VerDataArray[m_VerDataArray.GetSize()-1];

		CDWordArray* pNewVersion = new CDWordArray();
		pNewVersion->SetSize(0,_IncreaseNum);
		if (pDWORDArray)
		{
			int nCount = pDWORDArray->GetSize();
			for (int iIndexPos = 0; iIndexPos < nCount; iIndexPos++)
			{
				pNewVersion->Add( (*pDWORDArray)[iIndexPos]);
			}
		}

		m_VerDataArray.Add(pNewVersion);
	}
	return m_VerDataArray.GetSize()-1;
}

/************************************************************************************
函数名称：
void CVersionList::Clear()
功能说明：清除字段集m_FieldList中所有字段.以及版本链.

详细解释:	1.先删除横向版本号链,再删除纵向版本链.

出入参数：
[in]:无. 
  
[out]:无.

返回类型:void.

制作：YTLI 2002/08/28

修改: 
************************************************************************************/
void CVersionList::Clear()
{
	//删除横向版本号链.
	int iMaxVer = GetVersionCount();
	for(int i=0; i<iMaxVer; i++)
	{
		CDWordArray* pDWORDArray = GetVersion(i);
		pDWORDArray->RemoveAll();
		delete pDWORDArray;
	}

	//删除纵向版本链.
	m_VerDataArray.RemoveAll();
}

/************************************************************************************
函数名称：
void CVersionList::Clone(CVersionList *pVersionList)
功能说明：克隆字段集m_FieldList中所有字段.以及版本链.

详细解释:	

出入参数：
[in]:无. 
  
[out]:无.

返回类型:pVersionList,版本链表.

制作：YTLI 2002/08/28

修改: 
************************************************************************************/
void CVersionList::Clone(CVersionList *pVersionList)
{
	Clear();
	for (int i = 0;i<pVersionList->GetVersionCount();i++)
	{
		CDWordArray* pDWORDArray = pVersionList->GetVersion(i);
		CDWordArray* pNewVersion = new CDWordArray();
		if (pDWORDArray)
		{
			int nCount = pDWORDArray->GetSize();
			for (int iIndexPos = 0; iIndexPos < nCount; iIndexPos++)
			{
				pNewVersion->Add( (*pDWORDArray)[iIndexPos]);
			}
		}

		m_VerDataArray.Add(pNewVersion);

	}
}
