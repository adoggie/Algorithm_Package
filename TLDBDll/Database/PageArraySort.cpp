#include "../headtldb.h"

#include "cpage.h"

#include "PageArraySort.h"

static int m_nDebug =0;
CPageArraySort::CPageArraySort()
{	
	m_nTotalSize  = 0;		
	m_nWorkSize   = 0;
	m_nCurrent    = 0;
	m_nPageCount  = 0;
	m_ptrBuf      = NULL;
}

CPageArraySort::~CPageArraySort()
{
	if(m_ptrBuf != NULL)
	{
		delete []m_ptrBuf;
		m_ptrBuf = NULL;
	}
}

/************************************************************************************
函数名称：
	CPageArraySort::SetTotalSize
功能说明：
	设置总页数
详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2002-11-7
修改:

************************************************************************************/
void CPageArraySort::SetTotalSize(int nNewSize)
{

	if(nNewSize <= m_nTotalSize)
	{
		return;                //如果小于等于原有尺寸，不处理.
	}

	CPage **ppTemp = (CPage**)new byte[sizeof(void*) * nNewSize];
	memset(ppTemp ,0,sizeof(void*) * nNewSize);
	if(m_ptrBuf != NULL)
	{
		memcpy(ppTemp,m_ptrBuf,m_nPageCount*sizeof(void *));
		delete m_ptrBuf;//复制原有数据
	}
	m_nTotalSize = nNewSize;	
	m_ptrBuf = ppTemp;
}
/************************************************************************************
函数名称：
	CPageArraySort::InsertAt
功能说明：
	在指定位置插入一页
详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2002-11-7
修改:

************************************************************************************/
bool CPageArraySort::InsertAt(int nIndex ,CPage * pPage)
{  
	if( nIndex > m_nTotalSize-1 || nIndex < 0)
		return false;

	if(nIndex < m_nPageCount)
	{
		memmove(&m_ptrBuf[nIndex+1],&m_ptrBuf[nIndex],sizeof(void*) * (m_nPageCount-nIndex));
	}
	m_ptrBuf[nIndex] = pPage;
	m_nPageCount ++;
	return true;
}
/************************************************************************************
函数名称：
	CPageArraySort::RemoveAt
功能说明：
	在指定位置删除一页
详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2002-11-7
修改:

************************************************************************************/
bool CPageArraySort::RemoveAt(int nIndex)
{
	if(nIndex > m_nPageCount-1 || nIndex <0)
		return false;
	
	memmove(&m_ptrBuf[nIndex],&m_ptrBuf[nIndex+1],(m_nPageCount-nIndex-1)*sizeof(void*));
	m_nPageCount --;
	return true;
}


/************************************************************************************
函数名称：
	CPageArraySort::RemoveAll
功能说明：
	删除所以的页
详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2002-11-7
修改:

************************************************************************************/
bool CPageArraySort::RemoveAll()
{
	m_nWorkSize  = 0;
	return true;
}


/************************************************************************************
函数名称：
	CPageArraySort::GetTotalSize
功能说明：
	取得总页数
详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2002-11-7
修改:

************************************************************************************/
int CPageArraySort::GetTotalSize()
{
	return m_nTotalSize ;
}
/************************************************************************************
函数名称：
	CPageArraySort::GetWorkSize
功能说明：
	取得工作页面数
详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2002-11-7
修改:

************************************************************************************/
int CPageArraySort::GetWorkSize()
{
	return m_nWorkSize ;
}
/************************************************************************************
函数名称：
	CPageArraySort::GrowInsert
功能说明：
	系统增加一个页时,若此数组中没有记录,则插入.
详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2002-11-7
修改:

************************************************************************************/
bool CPageArraySort::GrowInsert(CPage* pPage)
{
	int nIndex;
	if(!FindIndex(pPage,nIndex))
	{//没找到
		m_nWorkSize ++;
		InsertAt(nIndex,pPage);
		return true;
	}
	//已经有此页
	return false;
}
/************************************************************************************
函数名称：
	CPageArraySort::DeleteRemove
功能说明：

详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2002-11-7
修改:

************************************************************************************/
bool CPageArraySort::DeleteRemove(CPage* pPage)

{
	int nIndex;
	if(!FindIndex(pPage,nIndex))
	{//没找到
		return false;
	}
	RemoveAt(nIndex);
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
	Eric 2002-11-7
修改:

************************************************************************************/
bool CPageArraySort::FindIndexByRecNo(int RecNo,int &nIndex)
{
	CPage * pTemp;
	switch(m_nWorkSize)
	{
	case 0:
		////assert(0);
		nIndex = 0;
		return false;
	case 1:
		pTemp = m_ptrBuf[0];
		if((RecNo >= pTemp->m_lngLStartRecNo) && (RecNo < pTemp->m_lngLStartRecNo + pTemp->GetLogicCount()))
		{
			nIndex = 0;
			return true;
		}
		else
		{
			if( RecNo < pTemp->m_lngLStartRecNo )
			{//如果要插入,将插在0处
				nIndex = 0; 
			}
			else
			{//如果要插入,将插在1处
				nIndex = 1; 
			}
			return false;
		}
	default:
		break;
	}

	int nTemp;
	int nLargeIndex = m_nWorkSize-1;
	int nSmallIndex = 0;
	while (nLargeIndex >= nSmallIndex)
	{
		nTemp = (nLargeIndex + nSmallIndex)/2;
		pTemp = m_ptrBuf[nTemp];
		
		if(RecNo < pTemp->m_lngLStartRecNo)
		{//在前面的页中
			nLargeIndex = nTemp-1;
			continue; 
		}
		else
		{//在后面的页中
			nSmallIndex = nTemp+1;
			continue ;
		}
	}

	if (nLargeIndex<0)
	{//没有找到
		nIndex = nLargeIndex+1;
		return false;
	}

	pTemp = m_ptrBuf[nLargeIndex];
	if((RecNo >= pTemp->m_lngLStartRecNo) && (RecNo < pTemp->m_lngLStartRecNo + pTemp->GetLogicCount()))
	{//在当前页
		nIndex = nLargeIndex;
		return true;
	}
	else
	{
		nIndex = nLargeIndex+1;
		return false;
	}
}

bool CPageArraySort::FindIndexInEmpty(CPage*pPage,int &nIndex)
{
	int nTemp = m_nWorkSize;
	int nLargeIndex = m_nPageCount-1;
	int nSmallIndex = m_nWorkSize;
	while (nLargeIndex >= nSmallIndex)
	{
		nTemp = (nLargeIndex + nSmallIndex)/2;
		
		if(pPage <= m_ptrBuf[nTemp])
		{//在前面的页中
			nLargeIndex = nTemp-1;
			continue; 
		}
		else
		{//在后面的页中
			nSmallIndex = nTemp+1;
			continue ;
		}
	}

	nIndex = nSmallIndex;
	if(nIndex<m_nPageCount && m_ptrBuf[nIndex]== pPage)
	{
		return true;
	}
	else
		return false;
}
/************************************************************************************
函数名称：

功能说明：

详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2002-11-7
修改:

************************************************************************************/
bool CPageArraySort::FindIndex(CPage* pPage,int& nIndex)
{
	return FindIndexByRecNo(pPage->m_lngLStartRecNo,nIndex);
}

/************************************************************************************
函数名称：

功能说明：

详细解释：

出入参数：
	[in,out]
返回类型：

制作：
	Eric 2002-11-7
修改:

************************************************************************************/
CPage* CPageArraySort::FindRecNoInPage(int nRecNo ) 
{
	int nIndex;
	if(FindIndexByRecNo(nRecNo,nIndex))
	{					  
		return m_ptrBuf[nIndex];
	}
	else
	{
		return NULL;
	}
}

//###_Mod 2002-11-11 zlq
/*
清除工作大小,在页面清空时用到
*/
void CPageArraySort::Clear()
{
	m_nWorkSize = 0;
	m_nPageCount = 0;
	m_nCurrent = 0;
	memset(&m_ptrBuf[0],0,(m_nTotalSize)*sizeof(void*));
	
}
/************************************************************************************
函数名称：
功能说明：
		添加一个页插入到空逻辑记录区域
详细解释：

出入参数：
	[in,out]
	CPage *pPage 要添加的页
返回类型：

制作：
    zlq 2002-11-29 
修改:
************************************************************************************/
void CPageArraySort::AddPage(CPage *pPage)
{	
	int nIndex;
	if (!FindIndexInEmpty(pPage,nIndex))
	{//找到需要插入的位置
		InsertAt(nIndex,pPage);
		////===temp TRACE("AddPage 工作页个数 %d\n",m_nPageCount);
		return ;
	}
	else
	{//已经存在
		//assert(0);
	}
	//===temp AfxMessageBox("出错!PageArraySort::AddPage");
	
}

/************************************************************************************
函数名称：
功能说明：
		删除一个页插入,在所有区域内
详细解释：

出入参数：
	[in,out]
	CPage *pPage 要删除的页
返回类型：

制作：
    zlq 2002-11-29 
修改:
************************************************************************************/
void CPageArraySort::RemovePage(CPage *pPage)
{	
	int nPos = FindPageByAllAddr(pPage);//在所有区域内找位置
	//assert(nPos>=0);
	if (nPos<m_nWorkSize)
	{//不太执行到
		if (m_nCurrent>nPos)
			m_nCurrent--;
		m_nWorkSize --;		
	}
	else
	{
	}
	RemoveAt(nPos);
}
/************************************************************************************
函数名称：
功能说明：
		在所有区域内找,通过地址来查找页的位置.最慢
详细解释：

出入参数：
	[in,out]
	CPage *pPage 要删除的页
返回类型：

制作：
    zlq 2002-11-29 
修改:
************************************************************************************/
int CPageArraySort::FindPageByAllAddr(CPage *pPage)
{
	for (int i = 0;i<m_nPageCount;i++)
	{
		if(m_ptrBuf[i] ==pPage)
			return i;
	}
	return -1;
}

/************************************************************************************
函数名称：
功能说明：
		得到当前页,可能也在逻辑空区域
详细解释：

出入参数：
	[in,out]
	
返回类型：
	返回当前页指针
制作：
    zlq 2002-11-29 
修改:
************************************************************************************/
CPage *CPageArraySort::GetCurPage()
{
	return m_ptrBuf[m_nCurrent];
}
/************************************************************************************
函数名称：
功能说明：
		设置当前页
详细解释：

出入参数：
	[in,out]
	CPage *pPage 要设置的页.
返回类型：
	返回是否成功
制作：
    zlq 2002-11-29 
修改:
************************************************************************************/
bool CPageArraySort::SetCurPage(CPage *pPage)
{
	//1.1 原始页调整
	CPage*pOldPage = m_ptrBuf[m_nCurrent];
	if (pOldPage->GetLogicCount()==0)
	{//变为逻辑空页
		if (m_nCurrent<m_nWorkSize)
		{//在工作区,则移动到非空页区
			int nIndex;
			if (FindIndexInEmpty(pOldPage,nIndex))
			{
				//assert(0);//不可能在非空页区也存在
			}
			else
			{
				RemoveAt(m_nCurrent);
				m_nWorkSize--;
				InsertAt(nIndex-1,pOldPage);
			}			
		}
	}
	else
	{//非空则不调整
	}

	////////////////////////
	//1.2 目标页调整
	int nIndex;
	if (FindIndexInEmpty(pPage,nIndex))
	{//2.1 在空页区存在
		RemoveAt(nIndex);
		
		if (FindIndexByRecNo(pPage->m_lngLStartRecNo,nIndex))
		{//又存在的话,出错
			//assert(0);
		}
		else
		{
			InsertAt(nIndex,pPage);
			m_nWorkSize++;//工作页增大
		}

	}
	else
	{//2.2在非空页区存在,不需要调整位置
		if (!FindIndexByRecNo(pPage->m_lngLStartRecNo,nIndex))
		{//不存在的话,出错
			//assert(0);
		}
	}

	//设置当前页
	m_nCurrent = nIndex;	
	
	return true;
}
/************************************************************************************
函数名称：
功能说明：
		得到一个可用的页,所在位置索引号
详细解释：

出入参数：
	[in,out]
	pnIndex 输出,如果有效,则返回可用的页在页数组中的位置.
返回类型：
	可用的页地址
制作：
    zlq 2002-11-29 
修改:
************************************************************************************/
CPage *CPageArraySort::GetAvailablePage(int *pnIndex)
{
	if (m_nPageCount>m_nWorkSize)
	{//有空页,包括逻辑和物理空页
		if (m_nWorkSize==0)
		{//排序页为零时
			int m = 1;
			if (m_nCurrent == m_nPageCount-1)
				m++;//最后一个为当前页
			if (pnIndex)
				*pnIndex = m_nPageCount-m;//可用页位置
			return m_ptrBuf[m_nPageCount-m];
			
		}
		else 
		{//不为零，则找最后一个
			if (pnIndex)
				*pnIndex = m_nPageCount-1;
			return m_ptrBuf[m_nPageCount-1];
		}
	}
	else
	{//没有空页
		if (m_nWorkSize>=m_nCurrent+m_nCurrent+2)
		{//工作页在前半区
			if (pnIndex)
				*pnIndex = m_nWorkSize-1;
			return m_ptrBuf[m_nWorkSize-1];
		}
		else
		{//工作页在后半区
			if (pnIndex)
				*pnIndex = 0;
			return m_ptrBuf[0];
		}
	}
}

/************************************************************************************
函数名称：
功能说明：
		移动到空页区域.按地址排序
详细解释：

出入参数：
	[in,out]
	nIndex 输入,页在页数组中的位置.
返回类型：
	可用的页地址
制作：
    zlq 2002-11-29 
修改:
************************************************************************************/
bool CPageArraySort::MoveToEmptyArea(int nIndex)
{
	if (m_nWorkSize>nIndex)
	{//如果处于有效区,则调整.
		CPage*pOldPage = m_ptrBuf[nIndex];
		int nNewIndex;
		if (!FindIndexInEmpty(pOldPage,nNewIndex))
		{
			RemoveAt(nIndex);
			m_nWorkSize--;
			InsertAt(nNewIndex-1,pOldPage);
			if (nIndex<m_nCurrent)
			{
				m_nCurrent--;				
			}
			else if (nIndex == m_nCurrent)
			{
				m_nCurrent = nNewIndex-1;
			}
		
		}
		else
		{
			//assert(0);//不可能找到
			return false;
		}
		return true;
	}
	else
	{
		return false;
	}
	
}

/************************************************************************************
函数名称：
功能说明：
		根据传入的逻辑记录号,找对应的页,如果不在,在求出上下区域,由外部决定申请范围.
详细解释：

出入参数：
	[in,out]
	RecNo  输入: 传入的逻辑记录号.
	nLeft  输出: 返回上区域
	nRight 输出: 返回下区域 默认传入总记录条数
返回类型：
	页地址,没有存在时为空.
制作：
    zlq 2002-11-29 
修改:
************************************************************************************/
CPage* CPageArraySort::FindIndexByRecNo(int RecNo,int&nLeft,int& nRight)
{
	int nIndex;
	if (FindIndexByRecNo(RecNo,nIndex))
	{//找到
		return m_ptrBuf[nIndex];
	}
	else
	{//没有找到
		if (m_nWorkSize==0)
		{
			nLeft = 0;
		}
		else
		{
			if (m_nWorkSize>nIndex)
			{//有右边界
				nRight = m_ptrBuf[nIndex]->m_lngLStartRecNo;
				if (nIndex > 0)
				{//有左边界
					nLeft = m_ptrBuf[nIndex-1]->m_lngLStartRecNo+m_ptrBuf[nIndex-1]->GetLogicCount()-1;
				}
			}
			else
			{//没有右边界								
				//if (nIndex > 0)
				{//有左边界
					nLeft = m_ptrBuf[nIndex-1]->m_lngLStartRecNo+m_ptrBuf[nIndex-1]->GetLogicCount()-1;					
				}
			}
			
			
		}
		return 0;
	}
	
}
/************************************************************************************
函数名称：
功能说明：
		得到nIndex所对应的页
详细解释：

出入参数：
	[in,out]
	nIndex  输入: 位置号码.
返回类型：
	页地址,没有存在时为空.
制作：
    zlq 2002-11-29 
修改:
************************************************************************************/
CPage *CPageArraySort::GetPageAt(int nIndex)
{
	//assert(nIndex<m_nPageCount);
	return m_ptrBuf[nIndex];
}
/************************************************************************************
函数名称：
功能说明：
		取得页个数
详细解释：

出入参数：
	[in,out]
	nIndex  输入: 位置号码.
返回类型：
	页地址,没有存在时为空.
制作：
    zlq 2002-11-29 
修改:
************************************************************************************/
int CPageArraySort::GetPageSize()
{
	return m_nPageCount;
}

/************************************************************************************
函数名称：
功能说明：
		移动到工作区(原来一定在逻辑空页区)
详细解释：

出入参数：
	[in,out]
	pPage  输入: 要移动的页.
返回类型：
	
制作：
    zlq 2002-11-29 
修改:
************************************************************************************/
bool CPageArraySort::MoveToWorkArea(CPage *pPage)
{
	int nIndex,nIndexNew;
	if(FindIndexInEmpty(pPage,nIndex))
	{
		if (!FindIndexByRecNo(pPage->m_lngLStartRecNo,nIndexNew))
		//if (m_nWorkSize==0)
		{
			RemoveAt(nIndex);
			InsertAt(nIndexNew,pPage);
			if (m_nWorkSize == 0 )
			{
				if (nIndex == m_nCurrent)
					m_nCurrent = nIndexNew;
				//else
					//===temp AfxMessageBox("发生全部为空页时，当前页出错！");
			}
			else if (nIndexNew<=m_nCurrent )//&& m_nWorkSize>0
			{
				m_nCurrent++;
			}
			m_nWorkSize++;
			return true;
		}
		else
		{
			//===temp AfxMessageBox("没有考虑！CPageArraySort::MoveToWorkArea");
		}
	}
	else
	{
		//===temp AfxMessageBox("没有找到！CPageArraySort::MoveToWorkArea");
		//assert(0);
	}
	return false;
}
