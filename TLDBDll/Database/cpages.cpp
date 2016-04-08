// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "cfakerecnos.h"
#include "cpages.h"
#include "cfileoperation.h"

#include "fieldlist.h"
#include "../../Public/common.h"

extern bool IsCrack;

#ifdef _UNIX
#include <time.h>
UINT GetTempFileName(const char * pathname,const char * prefix,
			UINT unique,char * output){
	clock_t clk = clock();
	time_t curt = time(0);
	sprintf(output,"/tmp/~TLTemp_%u_%u",curt,clk);
}

#endif



/************************************************************************************
函数名称：
    CPages::CPages
功能说明：
    构造函数
详细解释：
出入参数：
返回类型：
制作：
    Eric2002-07-03
修改: 
************************************************************************************/
CPages::CPages()
{
	
	m_nSaveVersion = 0;//默认为最新版本

	m_cDateConvertType = ddmmmyyyy_line;//===temp
	m_ntemp = 0; //===temp
	m_nWorkPageCount =0; 	 //当前工作页面数


	m_nLogicCount = 0;//m_pFakeRecNos->m_pFile->m_Header.GetRecordCount();//总逻辑纪录数
	m_lngLastRecNo=0;		 //上次使用过的纪录号,与当前纪录号比较,用于判断用户移动的方向.
	m_nCurRecNo =0;		     //当前纪录号
	m_nOldCurRecNo = 0;		 //insert,append时,保存纪录号,用于cancel时恢复.

	m_nAlertRange =50;		 //预装范围	
	m_nCriticalRecCount = 20;//在页面中移动时,此页面还有几条纪录时,要预装.

	//此pages中允许的最大页面数,可以设置
	SetMaxPageCount(5);
	//预装的正常值.预装的最大值.//###_Mod 2002-9-12 zlq
	SetNormalCountPerPage(IncreaseSize);

	m_pFakeRecNos=0; 		 //指向伪纪录号集的指针					 ???
	m_pFields=new CFields(this);//### 2002-9-5 Eric 			 //指向字段集,整个pages中只有一个字段集	 ???
#ifdef MarkWay_Encrypt  //是加密版本
	if (IsCrack)
	{//如果被破解
		m_pFields = m_pFields +1;
	}
#endif


    m_pFakeRecNos = new CFakeRecNos(this);	   //伪纪录链表的指针.
}
/************************************************************************************
函数名称：
    CPages::~CPages
功能说明：
    析构函数
详细解释：
出入参数：
返回类型：
制作：
    Eric2002-07-05
修改: 
************************************************************************************/
CPages::~CPages()
{	
	//delete m_pRUMod;
	Clear();

	delete m_pFields;

	delete m_pFakeRecNos;

}
/************************************************************************************
函数名称：
    CPages::SetMaxPageCount
功能说明：
    设置使用的页面数
详细解释：
    不能设得比现使用页面小
出入参数：
    [in]    count ：要设置的页面数
返回类型：
    bool:	设置是否成功
制作：
    Eric2002-07-03
修改: 
************************************************************************************/
bool CPages::SetMaxPageCount(int count)
{//###_Mod 2002-9-12 zlq
	if (count<2)
	{
		//===temp AfxMessageBox("页面个数不能小于2个!");
		return false;
	}
	//#_修改 2002-11-13 $ 14:16:49 zlq 位置调整
	m_nMaxPageCount = count;
	if(count < m_nWorkPageCount)
	{//缩小页面数
		while (count <m_nWorkPageCount)
		{
			CPage * pPage = GetPhysicalEmptyPage();
			delete pPage;
			m_nWorkPageCount--;
			//m_Depot.Clear();
		}			
	}

	m_PageArraySort.SetTotalSize(count);
	
	return true;
}
/************************************************************************************
函数名称：
    CPages::SetCurRecNo
功能说明：
	设置当前的记录号（得到指定记录号的数据）
详细解释：
    1.得到纪录所在的页面
	2.设置所在页面为活动页面
	3.移动到指定的纪录号
出入参数：
    [in]RecNo：	设置此纪录号为当前纪录号
返回类型：
    
制作：
    Eric2002-07-03
修改: 
************************************************************************************/
bool CPages::SetCurRecNo(int RecNo)
{
	if (RecNo<=0)
	{
		First();
		return false;
	}
	////===temp TRACE("CPages::SetCurRecNo当前记录号:%d\r\n",RecNo);===tempG
	CPage * pCurPage=0; //当前页的指针

	//根据逻辑纪录号,返回其所在的页,(其中当页面集中不含此纪录时,要发生等待)
	pCurPage = GetRecNoPageInNormal(RecNo);
	if (pCurPage->m_bReachEndError)
	{
		pCurPage->m_bReachEndError = false;
		Last();
		//===temp TRACE("末尾记录号:%d\n",GetRecordCount());
		return false;
	}
	//把此页设为当前页,设置其状态为pgNormal
 	//pCurPage->SetPageState(pgNormal);
	//SetPagePosition(pCurPage,1);
	//#_修改 2002-11-12 $ 18:14:06 zlq
	m_PageArraySort.SetCurPage(pCurPage);
	pCurPage->HangFields(m_pFields);//内部已经有了!

	//移动到指定的纪录号
	int lngRecNoOffset = RecNo - pCurPage->m_lngLStartRecNo;//页中的纪录位置偏移. 
	pCurPage ->MoveBy(lngRecNoOffset,FromBegin);

	//当前纪录号设为RecNo
	m_nCurRecNo = RecNo;

	m_bIsBof = false;//###_Mod 2002-10-14 zlq
	m_bIsEof = false;

	return true;
}

/************************************************************************************
函数名称：
    CPages::GetUsePageCount
功能说明：
    返回活动页面数
详细解释：
出入参数：
返回类型：
    int
制作：
    Eric2002-07-03
修改: 
************************************************************************************/
int CPages::GetUsePageCount()
{
	return m_nWorkPageCount;
}
/************************************************************************************
函数名称：
    CPages::QueryRecNoState
功能说明：
	得到记录号对应的数据所处于的状态。
详细解释：
	有以下几种状态：
	正常工作状态，等待数据装入，数据调入，等待数据写回，数据写回状态，没有装入（不包括等待数据装入，
	数据调入，等待数据写回，数据写回状态）
	(参考页面状态）
	typedef byte  CPageState;
	#define pgNormal       0x01
	#define pgWaitLoad     0x02  //等待装入数据,此时该页面应该为物理空页
	#define pgLoading      0x03  //正在装入数据,由输入输出线程装入
	#define pgWaitUnLoad   0x04  //等待卸载数据,数据应该是已经修改状态
	#define pgUnLoading    0x05  //正在卸载数据,由输入输出线程卸载
	#define pgInDisk       0x06  
	解释:
	如果返回为pgInDisk则该页处于磁盘中,必须调入,返回需要调入的逻辑记录号与长度,
	即StartRecNo和Count有效.如果返回其它,则返回该页的地址在InPage中指定
	算法:	遍历页面集 ,从所有纪录中找出最接近RecNo 的两个(大于,小于)纪录号,结合用户使用纪录集的方向性
	计算.并给各引用参数赋值.
	注意：必须跳过逻辑上为空页的页
出入参数：
    [in]RecNo：			 要取得此纪录号的页面状态
    [in,out]InPage：	 不在页面集中时,得到可用的一个页面
    [in,out]StartRecNo： 要装入的开始值
    [in,out]Count：		 要装入的个数
返回类型：
    
制作：
    Eric2002-07-03
修改: 
************************************************************************************/
bool CPages::QueryRecNoState( int RecNo, CPage*& InPage,int& StartRecNo,int& Count)
{//false 在磁盘  true 找到

	int lngLargerAndNearest,lngSmallerAndNearest;
	lngSmallerAndNearest=0;//比传入的纪录号(RecNo)小的所有现有纪录号中最大的一个. "小中最大"
	lngLargerAndNearest=this->GetRecordCount()+1;/*比传入的纪录号(RecNo)大的所有现有纪录号中最小的一个. "大中最小" */		 

	if(RecNo <= lngSmallerAndNearest || RecNo >= lngLargerAndNearest)
	{  //纪录超出范围.
		//===temp AfxMessageBox("纪录超出范围.CPages::QueryRecNoState");
		return false;
	}
	//#_修改 2002-11-12 $ 16:38:54 zlq
	InPage = m_PageArraySort.FindIndexByRecNo(RecNo,lngSmallerAndNearest,lngLargerAndNearest);
	if (InPage)
		return true;

	//判断方向性.
	if(m_lngLastRecNo > RecNo)
	{ //向上翻,	则装入纪录为参数指向的纪录号往上
		if(RecNo-lngSmallerAndNearest  >  m_nMaxCountPerPage)
		{	//要装入的纪录比m_nMaxCountPerPage条还多,所以一次不能装完,
			//因此只装 m_nNormalCountPerPage条.
			Count=	m_nNormalCountPerPage;
			StartRecNo= RecNo-m_nNormalCountPerPage+1;
		}
		else
		{//要装入的纪录比m_nMaxCountPerPage条少.
			if(lngLargerAndNearest - lngSmallerAndNearest-1 >  m_nMaxCountPerPage)
			{//不能"一次装入使记录连续",此时RecNo位于页的中间.
				StartRecNo =  lngSmallerAndNearest+1;
				Count = m_nMaxCountPerPage;
			}
			else
			{//一次装入,使记录连续
				StartRecNo =  lngSmallerAndNearest+1;
				Count = lngLargerAndNearest - lngSmallerAndNearest-1 ;
			}
		}
	}
	else
	{ //向下翻, 则装入纪录为参数指向的纪录号往下
		if(lngLargerAndNearest - RecNo>m_nMaxCountPerPage)
		{	 //不可能一次性装入,所以只装入Normal条记录.
			StartRecNo= RecNo;
			Count=	m_nNormalCountPerPage;
		}
		else
		{	
			if(lngLargerAndNearest - lngSmallerAndNearest-1 >  m_nMaxCountPerPage)
			{//不能"一次装入使记录连续",此时RecNo位于页的中间.
				StartRecNo =  lngLargerAndNearest -m_nMaxCountPerPage; 
				Count = m_nMaxCountPerPage;
			}
			else
			{//一次性装入.使记录连续.
				StartRecNo =  lngSmallerAndNearest+1;
				Count = lngLargerAndNearest -lngSmallerAndNearest -1;
			}
		}
	}
	return false;
}

/************************************************************************************
函数名称：CPages::UnLoad
功能说明：
    把页面放入队列,同时调整其它比该页大的页面的伪起始纪录号
详细解释：
	由于是单通道，所以可以根据该页返回值调整其它比该页大的页面的伪起始位置
	m_lngLOldStartRecNo。（通过遍历调用每一个页面的AdjustFakeRecNo函数）
	注意：所有页面，不管处于什么状态
出入参数：
    [in]CPage*：要UNLOAD 的页面
返回类型：
    void
制作：
    Eric2002-07-03
修改: 
************************************************************************************/
void CPages::UnLoad(CPage* pPage,bool IsNeedFeedback)
{
	int nOldfake = pPage->m_lngFakeStart;	   //pPage->UnLoad 将改变此值
	int lngOffset =  pPage->UnLoad(IsNeedFeedback);	//偏移量为逻辑纪录数与伪纪录数之差.

	AdjustFakeRecNo(pPage->m_lngLStartRecNo,nOldfake, lngOffset) ;
}
/************************************************************************************
功能说明：
	得到逻辑记录号与伪记录号的偏移   逻辑号 - 伪纪录号	 
详细解释：
	遍历页面集,找到首记录记录号小于LogicRecNo中最接近的页面.
	首记录号保存在 lngSmallerAndNearet中.
	本函数仅对于 LogicRecNo 不在已有页面中时有效.	!!!
出入参数：
    [in]LogicRecNo：逻辑记录号,据此得到偏移
返回类型：
    int	     逻辑记录号与伪记录号的偏移
制作：
    Eric2002-07-03
修改: 
************************************************************************************/
int CPages::GetLogicToFake(int LogicRecNo)
{
	int lngOffset = 0;
	CPage* pPage,*pTargetPage = 0;
	
	int lngSmallerAndNearet = 0;; //首纪录号最接近且的纪录(较LogicRecNo小)
	int lngSANFake = 0; 

	//遍历页面集,找到首记录记录号小于LogicRecNo中最接近的页面.修改lngSmallerAndNearet的值
	for (int i=0;i<m_PageArraySort.GetPageSize();i++)
	{
		pPage = m_PageArraySort.GetPageAt(i);
		
		if (pPage->IsPhysicalEmpty())//物理空页不能计算在内.
			continue;
		if(pPage->m_lngLStartRecNo <= LogicRecNo)
		{//本页中的纪录的纪录号都比 LogicRecNo 小,从这些页面中选出最大的纪录号
			if(lngSmallerAndNearet < pPage->m_lngLStartRecNo)
			{
				lngSmallerAndNearet = pPage->m_lngLStartRecNo;
				lngSANFake = pPage->m_lngFakeStart;
				pTargetPage = pPage;
			}
			if (lngSmallerAndNearet == pPage->m_lngLStartRecNo)
			{
				if (lngSANFake < pPage->m_lngFakeStart)
				{
					lngSmallerAndNearet = pPage->m_lngLStartRecNo;
					lngSANFake = pPage->m_lngFakeStart;
					pTargetPage = pPage;			
				}
			}
		}
	}

	//纪录号在页面集外
	if(pTargetPage && LogicRecNo > pTargetPage->m_lngLStartRecNo + pTargetPage->GetLogicCount()-1)
	{
		// pTargetPage中保存了最接近的一个页面指针,从其最后一个逻辑纪录号与最后一个伪纪录号的差值,可得出两种号的偏移.
		lngOffset =(pTargetPage->m_lngLStartRecNo + pTargetPage->GetLogicCount()-1)     /*最后一个逻辑纪录号	*/ \
				- (pTargetPage->m_lngFakeStart + pTargetPage->m_nOriginLoadCount-1); //最后一个伪纪录号

		return lngOffset ;// 逻辑号 - 伪纪录号
	}

	//纪录号在页面集内,未处理
	return lngOffset;
	//========================================================================
}
/************************************************************************************
函数名称：

功能说明：
    得到当前一个空页
详细解释：
	1.如果当前系统页面没有达到指定的个数,则新生成一个空页.
	2.如果已经达到指定的个数,则调用GetRecentNoUse()得到一个可用的页，
	并检查是否需要卸载。然后发出页请求，并等待返回（因为是多线程，所以
	必须用到WaitForSingleObject API函数）Unload中传递等待的参数
    
出入参数：
返回类型：
    CPage*
制作：
    Eric2002-07-03
修改: 
************************************************************************************/
CPage* CPages::GetPhysicalEmptyPage()
{
	CPage * pPage;
	if(m_nMaxPageCount>m_nWorkPageCount)
	{
		//生成一个新页
		pPage =  new CPage(this);
		m_nWorkPageCount++;
		m_pFields->SetCurVersion();
		
		pPage->IniFieldCnt(m_pFields);
		pPage->HangFields(m_pFields);
		return pPage;
	}
	
	int nIndex;
	pPage = m_PageArraySort.GetAvailablePage(&nIndex);
	if(pPage->m_bIsModify)
	{
		WaitForUnLoad(pPage);
	}

	if (pPage->GetLogicCount())
	{
		m_PageArraySort.MoveToEmptyArea(nIndex);		
	}

	pPage->EmptyTable();
	pPage->SetLogicCount(0);
	return pPage;
}

/************************************************************************************
函数名称：
    CPages::Next
功能说明：
    逻辑纪录的移动

************************************************************************************/
int CPages::Next()
{
	//PrintPages();
	if (GetRecordCount()==0)
	{
		m_bIsEof = true;
		m_bIsBof = true;
		return false;
	}
	if(m_nCurRecNo == GetRecordCount())
	{
		//已到末纪录
		m_bIsEof = true;//###_Mod 2002-10-14 zlq
		m_bIsBof = false;
		
		return false;
	}
	else
	{
		m_bIsEof = false;
		m_bIsBof = false;
	}

	//检查,设置纪录集状态
/*
		CDataSetState dsState;
		dsState = dsBrowse;
		if(!CheckStateAndPost(dsState))
		{
			return 0;
		}
	
	m_FState = dsState;
*/
	m_lngLastRecNo = m_nCurRecNo;

	CPage *pPage;
	pPage = GetCurPage();
	if(pPage->IsLogicalEmpty() || pPage->m_nCurNo == pPage->GetLogicCount()-1  )
	{
		//已到本页末纪录,寻找下一个页(或装入)
		SetCurRecNo(++m_nCurRecNo);
		return 0;
	}
	//未到本页末纪录
	//pPage->PrintCurLine();
	pPage->Next();
	


	m_nCurRecNo ++;	 

	return 0;
}
/************************************************************************************
函数名称：
    CPages::Prior
功能说明：
    逻辑纪录的移动
************************************************************************************/
int CPages::Prior()
{
	if (GetRecordCount()==0)
	{
		m_bIsEof = true;
		m_bIsBof = true;
		return false;
	}

	if(m_nCurRecNo == 1)
	{
		m_bIsBof = true;//###_Mod 2002-10-14 zlq
		//已到首纪录
		return false;
	}
	else
	{
		m_bIsBof = false;//###_Mod 2002-10-14 zlq
		m_bIsEof = false;
	}

	//检查,设置纪录集状态	 
/*
		CDataSetState dsState;
		dsState = dsBrowse;
		if(!CheckStateAndPost(dsState))
		{
			return 0;
		}
		m_FState = dsState;*/
	

	CPage *pPage;
	pPage = GetCurPage();
	if(pPage->m_nCurNo == 0)
	{
		//已到本页首纪录,寻找上一个页(或装入)
		SetCurRecNo(--m_nCurRecNo);
		return 0;
	}

	//未到本页首纪录
	pPage->Prior();



	m_nCurRecNo --;
	m_lngLastRecNo = m_nCurRecNo;

	return 0;
}
/************************************************************************************
函数名称：
    CPages::MoveBy
功能说明：
    逻辑纪录的移动
详细解释：
    
出入参数：
    [in]iSkipLen：移动的步长,可为负数.
    [in]MoveFlags：移动的起始位置.1头2尾3当前位置
返回类型：
    
制作：
    Eric2002-07-03
修改: 
************************************************************************************/
int CPages::MoveBy(int iSkipLen ,CFromPos MoveFlags)
{
	int nRecNoToMoveTo = 0;	 //保存将要移动到的逻辑纪录号
	//CDataSetState dsState;
	//dsState = dsBrowse;

	//检查参数
	switch(MoveFlags)
	{
	case FromBegin:
		if(iSkipLen<0)
			return false;

		if(iSkipLen >= m_nLogicCount)
		{
			//如果超出末纪录
			Last();
			return false;//???
		}
		nRecNoToMoveTo = iSkipLen+1;

		SetCurRecNo(nRecNoToMoveTo);
		break;
	case FromCur:
		
		//检查,设置纪录集状态
/*
				if(!CheckStateAndPost(dsState))
				{
					return 0;
				}
				m_FState = dsState;*/
		

		if(iSkipLen < 1-m_nCurRecNo)
		{
			//如果超出首纪录
			First();
			return false;
		}
		if(iSkipLen > m_nLogicCount-m_nCurRecNo)	  
		{
			//如果超出末纪录
			Last();
			return false;//???
		}
		nRecNoToMoveTo = m_nCurRecNo+iSkipLen	;
		SetCurRecNo(nRecNoToMoveTo);
		break;
	case FromEnd:
		if(iSkipLen>0)
			return false;//???
		

		if(iSkipLen<1-m_nLogicCount)
		{
			//如果超出首纪录
			First();
			return false;//???
		}
		nRecNoToMoveTo = m_nLogicCount+iSkipLen	;
		SetCurRecNo(nRecNoToMoveTo);
		break;
	}

	m_lngLastRecNo = m_nCurRecNo;
	return 1;
}
/************************************************************************************
函数名称：
    CPages::First
功能说明：
    移到逻辑第一条纪录
************************************************************************************/
int CPages::First()
{
	if (GetRecordCount()==0)//###_Mod 2002-9-16 zlq
	{
		m_bIsBof = true;
		m_bIsEof = true;
		return 0;
	}


	SetCurRecNo(1);
	m_lngLastRecNo = m_nCurRecNo;

	m_bIsBof = true;//###_Mod 2002-10-14 zlq
	if (GetRecordCount()==0)
		m_bIsEof = true;
	else
		m_bIsEof = false;
	return true;
}
/************************************************************************************
函数名称：
    CPages::Last
功能说明：
    移到逻辑最后一条纪录
************************************************************************************/
int CPages::Last()
{
	if (GetRecordCount()==0)//###_Mod 2002-9-16 zlq
	{
		m_bIsBof = true;
		m_bIsEof = true;
		return 0;
	}

/*
		CDataSetState dsState;
		dsState = dsBrowse;
		if(!CheckStateAndPost(dsState))
		{
			return 0;
		}
		m_FState = dsState;*/
	


	SetCurRecNo(m_nLogicCount);
	m_lngLastRecNo = m_nCurRecNo;

	m_bIsEof = true;//###_Mod 2002-10-14 zlq
	if (GetRecordCount()==0)
		m_bIsEof = true;
	else
		m_bIsBof = false;
	return true;
}

/************************************************************************************
函数名称：
    CPages::GetCurPage
功能说明：
    得到当前页
详细解释：
出入参数：
    [in,out]：
返回类型：
    CPage* 当前页
制作：
    Eric2002-07-03
修改: 
************************************************************************************/
CPage* CPages::GetCurPage()
{
	CPage * pPage;
	if (m_PageArraySort.GetPageSize()==0)
	{
		return 0;
	}
	pPage = m_PageArraySort.GetCurPage();

	return pPage;
}
/************************************************************************************
函数名称：
    CPages::GetRecNo
功能说明：
	返回当前记录号
详细解释：
出入参数：
返回类型：
    int: 当前逻辑纪录号
制作：
    Eric2002-07-03
修改: 
************************************************************************************/
int CPages::GetRecNo()
{
	return m_nCurRecNo;
}


/************************************************************************************
函数名称：
    CPages::Append
功能说明：
    把页面集设置为Append状态,当前纪录号设为-1
************************************************************************************/
#ifdef TEMP_Reserve/*
bool CPages::Post()
{
	//调用当前页面的方法
	CPage *pCurPage;
	
	switch(m_FState)
	{
	case dsBrowse:
		break;
	case dsInsert:
		pCurPage = GetCurPage();
		AdjustLogicRecNo(pCurPage->m_lngLStartRecNo,1);//730 add 注意要在post之前,避免分页
		
		//当前为内存
		pCurPage->Post(storeMemory,0);//###_Mod 2002-9-17 zlq
		m_bIsBof = false;
		m_bIsEof = false;
		m_nCurRecNo = m_nOldCurRecNo;
		
		if (m_bIsEnableRedoUndo)
		{//#_S 2003-5-28 $ 13 zlq 撤消重做
			pCurPage->BindFields();
			int nRecordLength = m_pFields->QuerySaveLength();
			BYTE *pData = new BYTE[nRecordLength];
			pCurPage->SetRecord(pData);
			CRURecordAdd*pRU = new CRURecordAdd(this);
			pRU->SetData(m_nCurRecNo,pData,nRecordLength);
			m_ruManager.Push(pRU);
		}
		
		m_nOldCurRecNo = 0;
		m_nLogicCount++;
		if (m_nLogicCount==1)
		{//#_修改 2002-11-13 $ 8:47:22 zlq
			m_PageArraySort.MoveToWorkArea(pCurPage);//新的
		}
		break;
	case dsAppend:
		pCurPage = GetCurPage(); //append 前页面已设为最后一页
		AdjustLogicRecNo(pCurPage->m_lngLStartRecNo,1);//730 add 注意要在post之前,避免分页
		//当前为内存
		pCurPage->Post(storeMemory,0);//###_Mod 2002-9-17 zlq
		m_bIsBof = false;
		m_bIsEof = false;
		
		m_nOldCurRecNo = 0;
		m_nLogicCount++;
		m_nCurRecNo = m_nLogicCount;

		if (m_bIsEnableRedoUndo)
		{//#_S 2003-5-28 $ 13 zlq 撤消重做
			pCurPage->BindFields();
			int nRecordLength = m_pFields->QuerySaveLength();
			BYTE *pData = new BYTE[nRecordLength];
			pCurPage->SetRecord(pData);
			CRURecordAdd*pRU = new CRURecordAdd(this);
			pRU->SetData(m_nCurRecNo,pData,nRecordLength);
			m_ruManager.Push(pRU);
		}
		
		if (m_nLogicCount==1)
		{//#_修改 2002-11-13 $ 8:47:22 zlq
			m_PageArraySort.MoveToWorkArea(pCurPage);//新的
		}
		break;
	case dsEdit:
		pCurPage = GetCurPage();
		//当前为内存
		pCurPage->Post(storeMemory,0);//###_Mod 2002-9-17 zlq

		if (m_bIsEnableRedoUndo)
		{//#_S 2003-5-28 $ 13 zlq 撤消重做
			pCurPage->BindFields();
			int nRecordLength ;//= m_pFields->QuerySaveLength();
			//BYTE *pData = new BYTE[nRecordLength];
			//pCurPage->SetRecord(pData);
			CRURecordMod*pRU = new CRURecordMod(this);
			BYTE*pData = m_pRUMod->MoveExistData(nRecordLength );
			if(pData==NULL)
			{
				//===temp AfxMessageBox("编辑时,撤消重做发生错误,请报告供应商!");
			}
			pRU->SetData(m_nCurRecNo,pData,nRecordLength);
			m_ruManager.Push(pRU);
		}

		m_bIsBof = false;
		m_bIsEof = false;
		break;
	default:
		break;
	}
	if(m_pFields->GetLastPostVersion()<m_pFields->GetLastVersion())         //mmmm
	{
		m_pFields->m_nLastPosVersion = m_pFields->GetLastVersion();
		m_pFakeRecNos->m_pIOFields->m_nLastPosVersion = m_pFakeRecNos->m_pIOFields->GetLastVersion();
	}
	m_FState = dsBrowse;

	return true;
	//撤销，重做 时使用下面  暂时保留上面的 2002-12-31 $ 11:02:58 zlq

}*/
#endif
/************************************************************************************
函数名称：
    CPages::Delete
功能说明：
    删除一条纪录
详细解释：
    调用当前页面的方法
出入参数：
返回类型：
    
制作：
    Eric2002-07-03
修改: 
************************************************************************************/
bool CPages::Delete()
{
	//调用当前页面的方法
	CPage *pPage;
	pPage = GetCurPage();

	//调整所有相关页面的启动记录号
	AdjustLogicRecNo(pPage->m_lngLStartRecNo,-1);



	pPage->_DeleteEx() ;

	if(m_nCurRecNo == m_nLogicCount)
	{
		if (m_nLogicCount==1)
		{//#_修改 2002-11-13 $ 8:55:51 zlq
			m_PageArraySort.MoveToEmptyArea(0);
		}

		 m_nCurRecNo--;
	}
	if (pPage->GetLogicCount()==0 && pPage->m_nOriginLoadCount==0)
	{//该页全部新增，且又全部删除了
		pPage->m_bIsModify = false;
	}

	m_nLogicCount--;
	if(m_nCurRecNo)
		SetCurRecNo(m_nCurRecNo);
	else
	{//###_Mod 2002-11-5 zlq
		m_bIsBof = true;
		m_bIsEof = true;
	}
	return true;
}
/************************************************************************************
函数名称：
    CPages::Cancel
功能说明：
    恢复页面集为浏览状态,当前纪录恢复.
详细解释：
    //调用当前页面的方法
出入参数：
    [in,out]：
返回类型：
    
制作：
    Eric2002-07-03
修改: 
************************************************************************************/
bool CPages::Cancel()
{
	//调用当前页面的方法

	bool result = 0;
	return result;
}

/************************************************************************************
函数名称：
    CPages::GetRecordCount
功能说明：
	返回数据库记录的个数，包括没有装入内存的记录
详细解释：
出入参数：
返回类型：
    int	:纪录个数
制作：
    Eric2002-07-03
修改: 
************************************************************************************/
int CPages::GetRecordCount()
{
	if (m_nLogicCount==UnknownCount)
	{
		SetCurRecNo(UnknownCount);
	}
	return m_nLogicCount;
}


/************************************************************************************
函数名称：
功能说明：
	设置当前条记录的所有的字段的数据信息到一个指定的缓冲区里
详细解释：
	数据组织按指示信息部分（所有字段，每个字段一字节）+数据?	分（所有字段，可能某些字段也没有数据，前面能够表达时）
出入参数：
    [in,out]lpData：缓冲区的指针
返回类型：
    int	返回数据长度
制作：
    Eric2002-07-03
修改: 
************************************************************************************/
int CPages::SetRecord(LPBYTE lpData)
{
	CPage *pPage;
	pPage = GetCurPage();
	return pPage->_SetRecord(lpData);
}

/************************************************************************************
函数名称：
    CPages::GetRecord
功能说明：
    对当前的记录数据赋值操作
详细解释：
	数据集在此之前应该通过处于编辑
	或添加状态，调用append,edit等方法），
出入参数：
    [in,out]lpData：缓冲区指针
    [in,out]count：有效缓冲区数据大小
返回类型：
    bool :是否成功
制作：
    Eric2002-07-03
修改: 
************************************************************************************/
bool CPages::GetRecord(LPBYTE lpData, int& count)
{
	CPage *pPage;
	pPage = GetCurPage();
	return pPage->GetRecord(lpData,count);
}

/************************************************************************************
函数名称：
    CPages::GetRecord
功能说明：
    对当前的记录数据赋值操作
详细解释：
	（数据集在此之前应该通过处于编辑或添加状态，调用append,edit等方法）
	此方法要修改指针
出入参数：
    [in,out]lpData：
返回类型：
    bool :是否成功
制作：
    Eric2002-07-03
修改: 
************************************************************************************/
bool CPages::GetRecord(LPBYTE& lpData)
{
	CPage *pPage;
	pPage = GetCurPage();
	return pPage->GetRecord(lpData);
}
/************************************************************************************
函数名称：

功能说明：
    设置某一页在页面集中的位置
详细解释：
	nPos为1即把pPage放在页面集中第一位.
	pPage	有两种情况:页面集中已有,页面集中没有.
出入参数：
    [in] pPage ：页面的指针,将设置它的位置.
    [in] nPos ：位置.
返回类型：
    bool 是否成功.
制作：
    Eric2002-07-03
修改: 
************************************************************************************/
/*//#_修改 2002-11-12 $ 18:12:45 zlq
bool CPages::SetPagePosition(CPage* pPage ,int nPos)
{
	
	POSITION posOld = m_pPageList.GetHeadPosition();
	POSITION posOldPrev = posOld;
	POSITION posDelete = 0;//删除的位置
	POSITION posInsertBefore = 0;//可能在此位置前面插入
	int CurPos = 1;

	//无论当前页是否会改变,先断开字段集绑定.
	CPage* pPageTemp =GetCurPage();
	if(pPageTemp)
		pPageTemp->HangFields(0);

	//寻找两个位置
	while(posOld)
	{
		CPage* ptemp = (CPage*)m_pPageList.GetNext(posOld);

		if (CurPos == nPos)
			posInsertBefore = posOldPrev; //插入的位置
		
		if (ptemp == pPage)
		{
			posDelete = posOldPrev;
			if (!posInsertBefore)
				nPos++;//删除的位置在插入的位置之前.
		}

		posOldPrev = posOld;
		CurPos++;
	}

	if (posInsertBefore)
		m_pPageList.InsertBefore(posInsertBefore,(CObject*)pPage);
	else
		m_pPageList.AddTail(pPage);//插入到后一个位置

	if (posDelete)
		m_pPageList.RemoveAt(posDelete);


	//绑定第一个page的m_pFields;
	pPageTemp =GetCurPage();
	pPageTemp->HangFields(m_pFields);
	return true;
}
*/
/************************************************************************************
函数名称：
    CPages::WaitForUnLoad
功能说明：
    等待某个页面的卸载
详细解释：
	返回时该页已经有效（即已经卸载过）
	1调用本页面集的UnLoad函数（该函数内部还要调整其它页面的伪记录起址）
	2本线程进入等待状态
出入参数：
    [in,out]CPage*：等待的页面指针
返回类型：
    void
制作：
    Eric2002-07-03
修改: 
************************************************************************************/
void CPages::WaitForUnLoad(CPage* pPage)
{
	if(pPage)
	{
		//PrintPages();
		UnLoad(pPage,true);
		//PrintPages();
	}	
}
/************************************************************************************
函数名称：
    CPages::WaitForLoad
功能说明：
    等待某个页面的装入
详细解释：
	返回时该页已经有效
	 1调用该页的load函数
	 2本线程进入等待状态
出入参数：
    [in,out]pPage：等待的页面,数据将装入其中
    [in]logicRecNo：起始逻辑号
    [in,out]count：	   个数
返回类型：
    void
制作：
    Eric2002-07-03
修改: 
************************************************************************************/
void CPages::WaitForLoad(CPage* pPage,int logicRecNo, int count )
{
	if(pPage)
	{
		pPage->Load(logicRecNo,count,true);
	}
}
/************************************************************************************
函数名称：

功能说明：
	根据传入的记录号，得到所在的页（不在内存则内部装入并返回该页）
详细解释：
	注意：该函数不进行页面顺序的调整
	先查询该记录号状态QueryRecNoState
	1 正常在内存中，则返回该页
	2 在队列中，调用WaitForInQueue(pPage : CPage*)返回该页
	3 如果不在内存页中，则调用GetPhysicalEmptyPage得到一个可用的空页,
	WaitForLoad得到该页
    
出入参数：
    [in]RecNo：逻辑纪录号
返回类型：
    CPage* ：记录所在的页
制作：
    Eric2002-07-03
修改: 
************************************************************************************/
CPage* CPages::GetRecNoPageInNormal(int RecNo)
{
	int  lngStartRecNo=0;   //如果要装入,在此保存装入的起始号
	int   nCount=0 ;		//如果要装入,在此保存要装入的个数
	CPage * InPage=0;		//纪录所在的页
	CPage * pGotPage;		//要返回的页

	//查询此纪录号的状态---结果1.在页面中,2.不在页面中,给出开始纪录号及长度,以便装入.
	bool psPageState = QueryRecNoState(RecNo,InPage,lngStartRecNo,nCount);


	if (psPageState ==false)
	{
		//内存中没有数据,申请一个新页,并装入数据.
		CPage * pPageNeedLoad;
		pPageNeedLoad = GetPhysicalEmptyPage();	 
		WaitForLoad(pPageNeedLoad,lngStartRecNo,nCount);
		pGotPage = pPageNeedLoad;
	}
	else
	{
		pGotPage = InPage;
	}

	return 	pGotPage;
}


/************************************************************************************
函数名称：
    CPages::LoadFile
功能说明：
    打开一个文件
详细解释：
    打开一个数据库文件,装入相应的字段结构，以及初始化内部的数据集等信息,
	调用伪记录号管理的LoadFile成功后，进行本字段结构的复制
出入参数：
    [in]lpszFileName：文件名.
返回类型：
    bool :是否成功
制作：
    Eric2002-07-05
修改: 
************************************************************************************/
bool CPages::LoadFile(LPCTSTR lpszFileName)
{
	//如果原有数据,则先删除.
	Clear();

	//装载数据,仅
	if(!m_pFakeRecNos->LoadFile(lpszFileName))
	{
		return false;
	}
	//m_pFields->Clone(m_pFakeRecNos->m_pIOFields);
	m_pFields->SetCurVersion();

	m_nLogicCount = m_pFakeRecNos->m_pOriginData->GetRecordCount();//总逻辑纪录数
	First();//###_Mod 2002-10-14 zlq
	return true;
}


/************************************************************************************
函数名称：
    CPages::NewFile	重载
功能说明：
    新建一个文件.
详细解释：
    传入字段信息,同时必须将原来的清空,同时创建临时文件.
    
出入参数：
    [in,out]：
返回类型：
    
制作：
    Eric2002-07-16
修改: 
************************************************************************************/
bool CPages::NewFile()
{
	//如果原有数据,则先删除.
	Clear();

	if(!m_pFakeRecNos->NewFile(m_pFields))
	{
		return false;
	}
	First();//###_Mod 2002-10-14 zlq
	return true;
}
/************************************************************************************
函数名称：
    CPages::Clear
功能说明：
    清除所有的信息,包括字段信息.
详细解释：
    
出入参数：
    [in,out]：
返回类型：
    
制作：
    //Eric2002-07-05
修改: 
************************************************************************************/
bool CPages::Clear()
{
	//assert(m_pFields);
	if(m_pFields&&(m_pFields->GetFieldCount()))
	{
		CPage * pPage;
		//#_修改 2002-11-12 $ 18:16:49 zlq
		//while(!m_pPageList.IsEmpty())
		for (int i=m_PageArraySort.GetPageSize()-1;i>=0;i--)
		{	
			//pPage = (CPage *)m_pPageList.RemoveHead();
			pPage = m_PageArraySort.GetPageAt(i);
			pPage->HangFields(m_pFields);
			delete pPage;
		}
		m_pFields->ClearAll();//清空页面集中的唯一的字段列表.
	}

	m_pFakeRecNos->Clear();

	m_PageArraySort.Clear();//###_Mod 2002-11-11 zlq

	m_nLogicCount = 0;
	m_nCurRecNo = 0;
	m_nWorkPageCount = 0;

	return true;
}
/************************************************************************************
函数名称：
    
功能说明：
    进行数据库保存,先保存到一临时文件中,最后改名.
	保存后再打开数据库.
详细解释：
    
出入参数：
    [in]lpszFileName：保存的文件名
	[out]pstrOutFile 返回false 时且保存为天律文件时有效, 为保存时出错时的另存为的文件名字,由内部生成
返回类型：
    bool :是否成功
制作：
    Eric2002-07-05
修改: //#_修改 2002-12-18 $ 13:44:58 zlq
************************************************************************************/
bool CPages::SaveFile(LPCTSTR lpszFileName,CTString *pstrOutFile)
{
//--UNIX PORT #ifdef WINPLAT
	int nCurRecNo = GetRecNo();	//当前纪录,再次打开时,要使用.

	CFileOperation *pFileOperation = new CFileOperation;
	pFileOperation->m_Header.SetHeaderSize(64);
	pFileOperation->m_Header.SetFieldPosition(64);
	pFileOperation->m_Header.SetFieldCount(m_pFields->GetFieldCount());

	//获得临时文件的路径.
	char szPathName[MAX_PATH];
	strcpy(szPathName,lpszFileName);
	char * pTemp = (char *)szPathName + strlen(szPathName);
	while(*pTemp !=CHAR_SLASH) //--hehe,if no slash or invalid full pathname,program dump
	{
		pTemp--;
	}
	*pTemp = 0;

	//获得临时文件名.
	char szTempFile[MAX_PATH];
//--UNIX PORT
	UINT nResult = GetTempFileName(szPathName, _T("~TLTemp"),0,szTempFile);

    //assert(nResult);
	//保存到临时文件中.
	pFileOperation->IniSaveFile(szTempFile,this,m_pFields);
	pFileOperation->SaveRecords((CPages*)this);

	//保存完后,进行清理操作,释放文件句柄,以便进行改名.	
	delete pFileOperation;

	Clear();
	
	//改文件名
	try
	{/*
        WIN32_FIND_DATA FindData;
		HANDLE hFileFile = FindFirstFile(lpszFileName , &FindData );
		if(hFileFile != INVALID_HANDLE_VALUE)
		{//此文件已存在,要先删除.
			FindClose(hFileFile);
			if (!DeleteFile(lpszFileName))
			{
				//DeleteFile(szTempFile);
				CTString strTempOut;
				RenameFile(szTempFile,lpszFileName,strTempOut);
				//重新打开文件,定位当前纪录.
				LoadFile(strTempOut);
				if (nCurRecNo)//###_Mod 2002-9-16 zlq
					SetCurRecNo(nCurRecNo);
				*pstrOutFile = strTempOut;
				return false;
			}
		}
		else
			FindClose(hFileFile);
		
		MoveFile(szTempFile,lpszFileName);
		*/
		if( !_access(lpszFileName,0)){
			if( remove( lpszFileName)){ //--if failed
				CTString strTempOut;
				RenameFile(szTempFile,lpszFileName,strTempOut);
				//重新打开文件,定位当前纪录.
				LoadFile(strTempOut);
				if (nCurRecNo)//###_Mod 2002-9-16 zlq
					SetCurRecNo(nCurRecNo);
				*pstrOutFile = strTempOut;
				return false;
			}
		}
		rename(szTempFile,lpszFileName);
	}
	catch(... )
	{
		
		CTString strTempOut;
		RenameFile(szTempFile,lpszFileName,strTempOut);
		//重新打开文件,定位当前纪录.
		LoadFile(strTempOut);
		if (nCurRecNo)//###_Mod 2002-9-16 zlq
			SetCurRecNo(nCurRecNo);
		*pstrOutFile = strTempOut;
		return false;
	}
	

	//重新打开文件,定位当前纪录.
	LoadFile(lpszFileName);
	if (nCurRecNo)//###_Mod 2002-9-16 zlq
		SetCurRecNo(nCurRecNo);


	return true;

}
/************************************************************************************
函数名称：
    
功能说明：
    文件更名.
详细解释：
    
出入参数：
    [in]strTempFileName：临时文件名
	[in]strTargetFileName：要保存的原来文件名
	[Out]strOutFileName：最终保存的文件名
返回类型：
    bool :是否成功
制作：
修改:
************************************************************************************/					
bool CPages::RenameFile(CTString strTempFileName,CTString strTargetFileName, CTString &strOutFileName)
{


	CTString filePath = strTargetFileName;
	int iPos = filePath.ReverseFind(CHAR_SLASH);
	if(iPos != 0)
		filePath.Delete(iPos+1,filePath.GetLength()-iPos-1);

	CTString filename = strTargetFileName;
	iPos = filename.ReverseFind(CHAR_SLASH);
	if(iPos != 0)
		filename.Delete(0,iPos+1);

	iPos = filename.ReverseFind('.');
	if(iPos != 0)
		filename.Delete(iPos,filename.GetLength()-iPos);
	
	CTString strExt = strTargetFileName;
	iPos = strExt.ReverseFind('.');
	if(iPos != 0)
		strExt.Delete(0,iPos);
	strExt.MakeLower();// .mkw
	//////////////////////////////////////////////////
	bool IsFalse = true;
	int m = 1;
	while (IsFalse)
	{
		CTString strOut = filePath;
		if (filename.GetLength()>240)
		{
			filename.Delete(240,filename.GetLength()-240);
		}

		CTString strtemp ;
		strtemp.Format("%d",m);
		strOut +=  filename ;
		strOut+= "_" ;
		strOut+=strtemp;
		
		strOut +=strExt;
/* -- UNIX PORT
        WIN32_FIND_DATA FindData;
		HANDLE hFileFile = FindFirstFile(strOut , &FindData );
		if(hFileFile != INVALID_HANDLE_VALUE)
		{//此文件已存在			
			m++;
			FindClose(hFileFile);
			continue;
		}
		FindClose(hFileFile);
		*/
		if( !_access(strOut,0)){
			m++;
			continue;
		}
		if(!rename(strTempFileName,strOut))
		//if (MoveFile(strTempFileName,strOut))
		{
			strOutFileName = strOut;
			return true;
		}
	}
	return false;
}
/************************************************************************************
函数名称：
    CPages::GetPageCount
功能说明：
    得到页的个数.
详细解释：
    
出入参数：
    [in,out]：
返回类型：
   int :页的数量. 
制作：
    Eric2002-07-03
修改: 
************************************************************************************/

int CPages::GetPageCount()
{	//#_修改 2002-11-12 $ 18:17:52 zlq
	return m_PageArraySort.GetPageSize(); 
}
/************************************************************************************
函数名称：

功能说明：
    调整原始数据源的个数,在读未知个数数据源时用.同时必须调整相应的pages的个数
详细解释：
    
出入参数：
    [in,out]：
返回类型：
   
制作：
    //###_Mod 2002-10-15 zlq
修改: 
************************************************************************************/

bool CPages::AdjustOriginCount(int nOffset)
{
	m_nLogicCount = m_nLogicCount - nOffset;
	return true;
}
/************************************************************************************
函数名称：

功能说明：
	根据传入的logicRecNo决定是否需要调整所有相关页面，与页所处状态无关
    由叶面集的UnLoad一个页时调用，它的偏移量由卸载页面的unload方法返回
详细解释：
出入参数：
	[in]logicRecNo : 逻辑纪录号,比它大的页面要调整.
	[in]fakeRecNo :	 逻辑纪录号对应的伪纪录号
	[in]nFakeOffset : 要调整的偏移量.

返回类型：
	void
制作：

修改: 
************************************************************************************/
void CPages::AdjustFakeRecNo(int logicRecNo,int fakeRecNo, int nFakeOffset)
{
    CPage* pPageTemp;
 
	for (int i=0;i<m_PageArraySort.GetPageSize();i++)
    {
		pPageTemp = m_PageArraySort.GetPageAt(i);
		pPageTemp->AdjustFakeRecNo(logicRecNo,fakeRecNo,nFakeOffset);
    }

}
/************************************************************************************
函数名称：
	CPages::AdjustLogicRecNo
功能说明：
	根据传入的logicRecNo决定是否需要调整所有相关页面逻辑记录号
详细解释：
出入参数：
	[in]logicRecNo:逻辑纪录号,比它大的页面要调整.
	[in]nOffset:偏移量.
返回类型：void

制作：

修改: 
************************************************************************************/
void CPages::AdjustLogicRecNo(int logicRecNo , int nOffset)
{
    CPage* pPageTemp;
 
	for (int i=0;i<m_PageArraySort.GetPageSize();i++)
    {
		pPageTemp = m_PageArraySort.GetPageAt(i);
		pPageTemp->AdjustLogicRecNo(logicRecNo,nOffset);
    }
}

/************************************************************************************
函数名称：
    CPages::InsertField
功能说明：
    动态增加一个字段时,每一个page都要相应增加一个列. 
详细解释：
    等到系统空闲.
	改变字段列表的信息.
	调用每一个page的函数
出入参数：
    [in]nIndex：插入的位置
    [in]pNewField: 插入的字段
返回类型：
    
制作：
    Eric2002-08-19
修改: 
************************************************************************************/
void CPages::InsertField(int nIndex,CField* pNewField)
{
	pNewField->m_pFields = m_pFields;
	m_pFields->Insert(pNewField,nIndex);


    CPage* pPageTemp;
	for (int i=0;i<m_PageArraySort.GetPageSize();i++)
    {
		pPageTemp = m_PageArraySort.GetPageAt(i);
		pPageTemp->InsertField(nIndex,pNewField);
    }
}
/************************************************************************************
函数名称：
    CPages::DeleteField
功能说明：
    动态删除一个字段时,每一个page都要删除一列数据. 
详细解释：
    等到系统空闲.
	改变字段列表的信息.
	调用每一个page的函数
出入参数：
    [in]nIndex：删除的位置
    [in]pOldField：已删除的字段
返回类型：
    void
制作：
    Eric2002-08-19
修改: 
************************************************************************************/
void CPages::DeleteField(int nIndex)
{

	CField * pOldField = m_pFields->FieldByIndex(nIndex);
	LPBYTE pOldValue = pOldField->m_pValue;//保存原来数据值
	//本身字段集
	m_pFields->Delete(nIndex);

    CPage* pPageTemp;
	for (int i=0;i<m_PageArraySort.GetPageSize();i++)
    {
		pPageTemp = m_PageArraySort.GetPageAt(i);
		pPageTemp->DeleteField(nIndex,pOldField);
    }
	////////////////////////////
	//pOldField->m_pValue =pOldValue;//恢复原来数据值
	pOldField->m_pValue = NULL;//恢复原来数据值
	if(m_pFields->GetFieldCount()== 0)
	{
		NewFile();

	}
	SetCurRecNo( GetRecNo());
}
/************************************************************************************
函数名称：
    CPages::ModifyField
功能说明：
    动态更改一个字段时,每一个page都要转化一列数据. 
详细解释：
    等到系统空闲.
	改变字段列表的信息.
	调用每一个page的函数
出入参数：
    [in]nIndex：要更改的字段位置
    [in]pOldField：旧的字段信息
    [in]pNewField：新的字段信息
返回类型：
    void
制作：
    Eric2002-08-19
修改: 
************************************************************************************/
void CPages::ModifyField(int nIndex,CField* pNewField)
{
	//本身字段集
	CField * pNewField1 = pNewField;
	CField * pOldField1 = m_pFields->FieldByIndex(nIndex);
	LPBYTE pOldValue1 = pOldField1->m_pValue;//保存原来数据值
	pNewField1->m_pFields = m_pFields;
	m_pFields->Modify(nIndex,pNewField1);


    CPage* pPageTemp;

	if (m_PageArraySort.GetPageSize())
	{

		for (int i=0;i<m_PageArraySort.GetPageSize();i++)
		{
			pPageTemp = m_PageArraySort.GetPageAt(i);
			pPageTemp->ModifyField(nIndex,pOldField1,pNewField1);
		}
	}

	pOldField1->m_pValue = NULL;
	pNewField1->m_pValue =pOldValue1;//拷贝原来数据值
	//pNewField2->m_pValue =pOldValue2;//拷贝原来数据值

	SetCurRecNo( GetRecNo());
}

/************************************************************************************
函数名称：
    CPages::AddField
功能说明：
    增加一个字段
详细解释：
    
出入参数：
    [in]pNewField：新字段
返回类型：
    
制作：
    Eric2002-08-22
修改: 
************************************************************************************/
void CPages::AddField(CField* pNewField)
{


	//本身字段集
	pNewField->m_pFields = m_pFields;
	m_pFields->Add(pNewField);

	CPage* pPageTemp;
	for (int i=0;i<m_PageArraySort.GetPageSize();i++)
    {
		pPageTemp = m_PageArraySort.GetPageAt(i);
		pPageTemp->AddField(pNewField);
    }
	SetCurRecNo( GetRecNo());
}

/************************************************************************************
函数名称：
    CPages::GetInsertPos
功能说明：
    当数据已排序时,根据相应的值,得到插入的位置.  此处先进行初始化,然后调用SearchPos
	进行递归查找.
详细解释：
出入参数：
返回类型：
    int :返回在前面插入的位置,-1为Append
制作：
    Eric2002-09-03
修改: 
************************************************************************************/
int CPages::GetInsertPos(int nColIndex, double fValue, CSortType nType,int nNullCount)
{
	if(m_nLogicCount == 0)
		return -1;
	int nRecLarge = GetRecordCount();
	int nRecSmall = 1;
	if(nType == stAscend)
	{
		nRecSmall = 1+ nNullCount;
		SetCurRecNo(nRecSmall);
		if(m_pFields->FieldByIndex(nColIndex)->GetAsDouble() > fValue)
			return nRecSmall;
		Last();
		if(m_pFields->FieldByIndex(nColIndex)->GetAsDouble() < fValue)
			return -1;//Append
	}
	else
	{
		nRecLarge -= nNullCount;
		if(!nRecLarge)
			return 1;
		First();
		if(m_pFields->FieldByIndex(nColIndex)->GetAsDouble() < fValue)
			return 1;
		SetCurRecNo(nRecLarge);
		if(m_pFields->FieldByIndex(nColIndex)->GetAsDouble() > fValue)
		{
			if(nNullCount == 0)
				return -1;//Append
			else
				return 	nRecLarge + 1;
		}
	}

	return SearchPos(nRecSmall,nRecLarge,fValue,nColIndex,nType);
}
/************************************************************************************
函数名称：
    CPages::SearchPos
功能说明：
    在两个纪录号之间,采用递归算法,查找插入的位置.
详细解释：
出入参数：
    [in]nRecSmall :小的纪录号
    [in]nRecLarge :大的纪录号
    [in]fValue :用于比较的值
    [in]nColIndex :字段的位置
    [in]nType :排序种类.
返回类型：
    int :要在前面插入的位置.
制作：
    Eric2002-09-03
修改: 
************************************************************************************/
int CPages::SearchPos(int nRecSmall,int nRecLarge,double fValue,int nColIndex,CSortType nType)
{
	if(nRecSmall == nRecLarge)
		return nRecLarge;
	if(nRecSmall == nRecLarge-1)
		return nRecLarge;

	int nCurPos = (nRecSmall+ nRecLarge)/2;


	SetCurRecNo(nCurPos);
	if(nType == stAscend)
	{
		if(m_pFields->FieldByIndex(nColIndex)->GetAsDouble() < fValue)
			nRecSmall = nCurPos;
		else
			nRecLarge = nCurPos;
	}
	else
	{
		if(m_pFields->FieldByIndex(nColIndex)->GetAsDouble() > fValue)
			nRecSmall = nCurPos;
		else
			nRecLarge = nCurPos;
	}
	return SearchPos(nRecSmall,nRecLarge,fValue,nColIndex,nType);
}
/************************************************************************************
函数名称：
    CPages::BindFieldList
功能说明：
	改变pFieldList 中的私有变量m_pPages的值.
详细解释：
出入参数：
返回类型：
    void
制作：
    Eric2002-09-03
修改: 
************************************************************************************/
void CPages::BindFieldList(CFieldList *pFieldList)
{
	pFieldList->m_pPages = this;
}

int  CPages::GetMaxCountPerPage()
{
	return m_nMaxCountPerPage;
}

bool CPages::SetNormalCountPerPage(int nCount)
{
	if (nCount<1)
		return false;
	m_nNormalCountPerPage = nCount; //正常时每页的记录条数.
	m_nMaxCountPerPage = nCount * 2;    //每个页的最大记录条数.
	return true;
}

int CPages::GetNormalCountPerPage()
{
	return m_nNormalCountPerPage;
}
/************************************************************************************
函数名称：
	CPages::MoveRecord
功能说明：
	记录号都是调整前的位置
详细解释：

出入参数：
	[in] nSourceRec		:
	[in] nDestinationRec:
返回类型：

制作：
	Eric 2002-9-16
修改:

************************************************************************************/
bool CPages::MoveRecord(int nSourceRec,int nDestinationRec)
{
	//===temp TRACE("源位置：%d 目标位置：%d \n",nSourceRec,nDestinationRec);
	SetCurRecNo(nSourceRec);
	int nLength = m_pFields->QueryLength();
	BYTE *pBuffer = new BYTE[nLength+2];
	SetRecord(pBuffer);
	Delete(); 
	if(nSourceRec < nDestinationRec)
		nDestinationRec--;
	SetCurRecNo(nDestinationRec);
	_Insert();
	int nCount;
	GetRecord(pBuffer,nCount);
	//Post();
	delete pBuffer	;
	return true;
}
//判断是否处于顶部位置
bool CPages::Bof()
{
	return m_bIsBof;
}
//判断是否处于结尾位置
bool CPages::Eof()
{
	return m_bIsEof;
}


//得到页设置属性 包括 最大的页个数 每个页的正常时的大小 已经使用的页
void CPages::GetPageSet(int &nMaxPageCount,int &nPageNormalSize,int &nUsePage)
{
	nMaxPageCount = m_nMaxPageCount;
	nPageNormalSize = m_nNormalCountPerPage;
	nUsePage = m_nWorkPageCount;
}



int CPages::GetFileVersion()
{
	return m_pFakeRecNos->GetFileVersion();
}

int CPages::GetSaveFileVersion()
{
	return m_nSaveVersion;
}

void CPages::SetSaveFileVersion(int nVersion)
{
	m_nSaveVersion = nVersion;
}



/************************************************************************************
功能说明：
	将页面设置为数据能够全部装入内存
出入参数：
    [in]bAtOnce:是否立刻将所有数据装入内存//暂时没有实现

返回类型：
    bool :是否成功
	//#_S 2003-8-22 $ 9 zlq  
************************************************************************************/
bool CPages::AdjustPageCount(bool bAtOnce)
{
	
	//*************页面设置**********************
	int nMaxPageCount,nPageNormalSize,nUsePage;
	int nCount = this->GetRecordCount();
	this->GetPageSet(nMaxPageCount,nPageNormalSize,nUsePage);
	int nMax = nCount/nPageNormalSize+20;
	if (nCount>500)//不超过十万页
	{
		nMax = 5;
		this->SetMaxPageCount(nMax);//设定页面大小
	}
	else
	{//预先内存装入
		this->SetMaxPageCount(nMax);//设定页面大小
		/*
		int nOld = GetRecNo();//包存原来的记录号		
		int nPos = 1;
		while (nPos <m_nLogicCount )
		{
			SetCurRecNo(nPos);
			nPos = nPos+nPageNormalSize;
		}

		SetCurRecNo(nOld);//恢复
		*/
	}

	return true;
}

/************************************************************************************
功能说明：
    移动一个字段时,每一个page都要移动 
详细解释：
    等到系统空闲.
	改变字段列表的信息.
	调用每一个page的函数
出入参数：
      1.nFrom,原来的索引号
	  2.nTo,移动到的新的索引号(注意是相对老字段还在时的位置)
返回类型：
    void
制作：
    //#_S 2003-9-12 $ 10 zlq
修改: 
************************************************************************************/
void CPages::MoveField(int nFrom, int nTo)
{

	m_pFields->Mov(nFrom,nTo);

    CPage* pPageTemp;
	for (int i=0;i<m_PageArraySort.GetPageSize();i++)
    {
		pPageTemp = m_PageArraySort.GetPageAt(i);
		pPageTemp->MoveField(nFrom,nTo);
    }
}

bool CPages::_Insert()
{
	CPage *pCurPage = GetCurPage();
	if (m_nCurRecNo == 0 && m_nLogicCount==0)//###_Mod 2002-10-30 zlq
	{//如果当前记录个数为空，且当前记录号为0
		_Append();
		return true;
	}
	

	AdjustLogicRecNo(pCurPage->m_lngLStartRecNo,1);//730 add 注意要在post之前,避免分页

	pCurPage->_InsertEx();
	m_nOldCurRecNo = m_nCurRecNo;
	m_nCurRecNo = -1;
	
	
	//当前为内存
	//pCurPage->Post(storeMemory,0);//###_Mod 2002-9-17 zlq
	
	m_bIsBof = false;
	m_bIsEof = false;
	m_nCurRecNo = m_nOldCurRecNo;
	

	
	m_nOldCurRecNo = 0;
	m_nLogicCount++;
	if (m_nLogicCount==1)
	{//#_修改 2002-11-13 $ 8:47:22 zlq
		m_PageArraySort.MoveToWorkArea(pCurPage);//新的
	}
	
	
	
	
	if(m_pFields->GetLastPostVersion()<m_pFields->GetLastVersion())         //mmmm
	{
		m_pFields->m_nLastPosVersion = m_pFields->GetLastVersion();
		//m_pFakeRecNos->m_pIOFields->m_nLastPosVersion = m_pFakeRecNos->m_pIOFields->GetLastVersion();
	}
	return true;
}

bool CPages::_Append()
{
	CPage * pCurPage = GetCurPage();
	if(m_nCurRecNo == 0 && m_nLogicCount == 0)
	{//如果当前记录个数为空，且当前记录号为0
		if(!pCurPage)
			pCurPage = GetPhysicalEmptyPage();
	}
	else
	{
		Last();
		m_nOldCurRecNo = m_nCurRecNo;
	}
	
	AdjustLogicRecNo(pCurPage->m_lngLStartRecNo,1);//730 add 注意要在post之前,避免分页
	pCurPage->_AppendEx();
	m_nCurRecNo = -1;

	//当前为内存
	//pCurPage->Post(storeMemory,0);//###_Mod 2002-9-17 zlq

	m_bIsBof = false;
	m_bIsEof = false;
	
	m_nOldCurRecNo = 0;
	m_nLogicCount++;
	m_nCurRecNo = m_nLogicCount;
	
	
	if (m_nLogicCount==1)
	{//#_修改 2002-11-13 $ 8:47:22 zlq
		m_PageArraySort.MoveToWorkArea(pCurPage);//新的
	}

	if(m_pFields->GetLastPostVersion()<m_pFields->GetLastVersion())         //mmmm
	{
		m_pFields->m_nLastPosVersion = m_pFields->GetLastVersion();
		//m_pFakeRecNos->m_pIOFields->m_nLastPosVersion = m_pFakeRecNos->m_pIOFields->GetLastVersion();
	}
	return true;
}

bool CPages::_Edit()
{
	//调用当前页面的方法
	CPage *pPage;
	pPage = GetCurPage();
	if(!pPage)//###_Mod 2002-9-16 zlq
	{
		pPage = GetPhysicalEmptyPage();
		m_nOldCurRecNo = 0;
		pPage->_AppendEx();
		pPage->m_bIsModify = true;
	}
	else
	{

		m_nOldCurRecNo = m_nCurRecNo;		//===ky
		pPage->_Edit();
	}

	return true;
}

void CPages::PrintPages()//===temp
{
	//===temp TRACE("页面输出 开始\r\n");
	for (int i=0;i<m_PageArraySort.GetPageSize();i++)
	{
		CPage*pPage = m_PageArraySort.GetPageAt(i);
		//===temp TRACE("Page: %p  LS:( %d - %d ) FakeS:( %d  - %d )\r\n",pPage,pPage->m_lngLStartRecNo,pPage->m_nLogicCount,
		//	pPage->m_lngFakeStart,pPage->m_nOriginLoadCount);
	}
	//===temp TRACE("****************\r\n");
}
