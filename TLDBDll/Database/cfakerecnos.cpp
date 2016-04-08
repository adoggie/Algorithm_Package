// Copyright (C) 1991 - 1999 Rational Software Corporation

#include "../headtldb.h"
#include "tempfile.h"
#include "cfakerecnos.h"
#include "cfileoperation.h"
#include "DriverManager.h"
#include "../../Public/System.h"
#include "../../Public/common.h"

extern BYTE  tlcfgBuf[] ;
extern BYTE  CurFileBuf[] ;

/************************************************************************************
函数名称：
    CFakeRecNos::~CFakeRecNos
功能说明：
    析构函数
详细解释：
       1.删除CFileOperation,TempFile的内容.
	   2.清空m_FakeRecNoList和m_QueueItemList链表.
出入参数：
返回类型：
制作：
    Eric2002-07-08   //### Add by YTLI at  2002/08/12
修改: 
************************************************************************************/
CFakeRecNos::~CFakeRecNos()
{	
	Clear();//###_修改 2002-8-27 zlq


}



/************************************************************************************
函数名称：
void CFakeRecNos::InQueueLoad(CPage* pPage, long FakeRecNo, int count, bool IsNeedFeedback)
功能说明：进队列操作.

详细解释：1.根据传入参数先生成结构,并加到排队等待的处理的队列m_pQueueItemList中去.
         2.然后检查m_bThreadStart ,如果线程已经停止,则重新启动线程,
		 3.并将m_bThreadStart设置为启动状态(true).

出入参数：
[in]: 1.pPage             //该页面指针.
      2.FakeRecNo         //
	  3.count             //      
	  4.IsNeedFeedback    //
  
[out]:无　

返回类型：void

制作：zlq   //### Add Document by YTLI at  2002/08/12

修改: 
************************************************************************************/
void CFakeRecNos::InQueueLoad(CPage* pPage, long FakeRecNo, int count, bool IsNeedFeedback)
{
	m_pIOFields_->GetBookmark();
	//int nVersion = m_pIOFields_->GetCurVersion();

	Load(pPage,FakeRecNo,count);

	pPage->m_bIsModify = false;

	m_pIOFields_->SetCurVersion();
	m_pIOFields_->GotoBookmark();
	

}


/************************************************************************************
函数名称：
CFakeRecNos::Load(CPage* pPage, long FakeRecNo, int count, bool IsNeedFeedback)
功能说明：页面的装入工作.

详细解释：1.如果本页面其实不为空,那么先进行物理清空操作.
         2.根據传入的伪记录号来确定要读取的鏈表的結點范围.

出入参数：
[in]:    1.pPage            //该页面指標
		 2.FakeRecNo        //该页面需要读取的起始伪记录号
		 3.count            //该页面需要读取的记录条数
		 4.IsNeedFeedback   //
[out]:无　
************************************************************************************/
void CFakeRecNos::Load(CPage* pPage, long FakeRecNo, int count)
{
	////===temp TRACE( "Load enter 页号:%d 起始逻辑记录号=%d, 起始伪记录号=%d, OldFakeRecNo = %d count=%d\n", pPage->m_nPageNumber, pPage->m_lngLStartRecNo, pPage->m_lngFakeStartRecNo, FakeRecNo ,count);
	pPage->HangFields(m_pIOFields_);
	//assert(pPage->m_nLogicCount == pPage->m_nOriginLoadCount);
	pPage->m_pFields->m_nLastPosVersion = pPage->m_pFields->GetCurVersion();//###_修改 2002-8-27 zlq
	m_pPages->m_pFields->m_nLastPosVersion = pPage->m_pFields->m_nLastPosVersion;//###_修改 2002-8-27 zlq
#ifdef _DEBUG
	Print();
#endif
	//如果本页面其实不为空,那么先进行物理清空操作
	if(!pPage->IsPhysicalEmpty())	
		pPage->EmptyTable();

	long lStartFakeRecNo   =  0;
	long lEndFakeRecNo     =  0;
	long lPageMaxFakeRecNo =  ( FakeRecNo + count -1 );	

	POSITION pos = m_FakeRecNoList.GetHeadPosition();
	if(!pos)
		return;
	do
	{
		CFakeRecNo* pFakeRecNo = (CFakeRecNo*)m_FakeRecNoList.GetNext(pos);

		lStartFakeRecNo = (lEndFakeRecNo + 1);
		lEndFakeRecNo  += pFakeRecNo->m_lngRecCount;

		int iOffset  = FakeRecNo - lStartFakeRecNo;			

		if( (  FakeRecNo >= lStartFakeRecNo) && 
			(  FakeRecNo <= lEndFakeRecNo  )   )
		{
			int iLoadCount = 0;

			//计算出需要读的数量
			if ( lPageMaxFakeRecNo <= lEndFakeRecNo )
				iLoadCount = count;
			else
				iLoadCount = lEndFakeRecNo - FakeRecNo + 1;//应该加1

			//根据m_nStoreType将数据读出来
			if( pFakeRecNo->m_nStoreType == storeTlFile )
			{//到外面数据源读取				
				pPage->SetCurVersion(0);// 版本设置操作
				int nStartRecNo = (int)pFakeRecNo->m_lngPosition + iOffset;
				int nRealLoadCount = m_pOriginData->ReadRecords(pPage,FromEnd,nStartRecNo, iLoadCount );
				//考虑未知记录个数数据源
				if ( nRealLoadCount != nStartRecNo + iLoadCount - 1 )
				{//没有读到相应的记录条数
					AdjustOriginCount(m_pPages->GetLogicCount() - nRealLoadCount);
					pPage->m_bReachEndError = true;//设置出错标记
				}
			}
			else
			{//临时文件读取                                        
				//m_pTempFile->ReadRecords(pPage,FromEnd,pFakeRecNo->m_lngPosition,iOffset ,iLoadCount);
				m_pTempFile->_LoadPage(pPage,pFakeRecNo->m_lngPosition,iOffset ,pFakeRecNo->m_nVersion,iLoadCount);
			}
			count        -= iLoadCount;
			FakeRecNo    += iLoadCount;
		}
	}while( lEndFakeRecNo < lPageMaxFakeRecNo && pos);
	

	//assert(pPage->m_nLogicCount == pPage->m_nOriginLoadCount);
	//pPage->PrintCurLine();

}
/************************************************************************************
函数名称：
    CFakeRecNos::Clear
功能说明：
    清除所有的信息,关闭文件
详细解释：
    
出入参数：
    [in,out]：
返回类型：
    
制作：
    //Eric2002-07-05
修改: 
************************************************************************************/
bool CFakeRecNos::Clear()
{
	//删除CFileOperation,TempFile的内容.
	if(m_pOriginData)
	{
		delete m_pOriginData;
		m_pOriginData = 0;
	}
	if(m_pTempFile)
	{
		CTString filename = m_pTempFile->GetFilePath();
		delete 	m_pTempFile;
		unlink(filename);

		m_pTempFile=0;
	}

	//清空链表
	while(!m_FakeRecNoList.IsEmpty())
	{
		CFakeRecNo* pFakeRecNo = (CFakeRecNo*)m_FakeRecNoList.RemoveHead();
		delete pFakeRecNo;
	}
	while(!m_QueueItemList.IsEmpty())
	{
		CQueueItem* pQueueItem = (CQueueItem*)m_QueueItemList.RemoveHead();
		delete pQueueItem;
	}
	return true;
}



/************************************************************************************
函数名称：
功能说明：頁面不用時,该页面的保存操作.

详细解释：1.定義臨時鏈表localFakeRecNoList存放需要保存的內容.
		 2.調用,該函數將頁內記錄按Load時的來源文件不同劃分爲小塊(Block).
		   然後對每小塊(Block)調用CFakeRecNos::UnLoadBlock()來填充鏈表localFakeRecNoList.
		 3.調用CFakeRecNos::DecomposeList()分解临时链表.
		 4.調用CFakeRecNos::ReplaceList()替换链表.
		 5.調用CFakeRecNos::MergeList()合并链表.

出入参数：
[in]: 1.pPage            //该页面指標
      2.FakeRecNo        //该页面起始的伪记录号
      3.IsNeedFeedback   //反馈
修改: 
***********************************************************************************/
void CFakeRecNos::UnLoad(CPage* pPage )
{
	m_pIOFields_->GetBookmark();
	//===temp   应该删除 调试用
	POSITION p1       = m_FakeRecNoList.GetHeadPosition();
	long lAllFake      =  0;//当前操作的伪记录块的 结束伪记录号
	while( p1)
	{	
		CFakeRecNo* pFakeNode        = (CFakeRecNo*)m_FakeRecNoList.GetAt(p1);	
		lAllFake     += pFakeNode->m_lngRecCount;
		m_FakeRecNoList.GetNext(p1);
	}

	//***temp end;
	////===temp TRACE( "起始逻辑号= %d,个数= %d\t起始伪记录号 = %d,个数:%d totalFake:%d\n",  
	//pPage->m_lngLStartRecNo,pPage->m_nLogicCount,pPage->m_lngFakeStart,pPage->m_nOriginLoadCount ,lAllFake);
	pPage->HangFields(m_pIOFields_);

	Print();


	m_pIOFields_->SetCurVersion();//### 08-23
	pPage->m_pFields->m_nLastPosVersion = pPage->m_pFields->GetCurVersion();
	m_pPages->m_pFields->m_nLastPosVersion = pPage->m_pFields->m_nLastPosVersion;
	int nLastMemoryVersion = pPage->m_pFields->GetLastVersion();
	int nRecordLen = pPage->m_pFields->QueryLength();
	
	//求lOriPageRecCnt  End************************************
	long lMaxOriginFakeNo   =  pPage->m_nOriginLoadCount + pPage->m_lngFakeStart - 1;//最初Load時本頁最大的僞記錄號
	if ( lMaxOriginFakeNo <  pPage->m_lngFakeStart)//确保lMaxOriginFakeNo〉=FakeRecNo （如果该页是全部添加的）
		 lMaxOriginFakeNo =  pPage->m_lngFakeStart;
	long lMinFakeNo = pPage->m_lngFakeStart;//start

	POSITION posDeleteStart = NULL;      //开始删除的位置
	int      iDeleteCnt     = 0;         //最后需要删除的结点数量
	long     InLocallistPos =0  ;         //

	CPtrList localFakeRecNoList;
	long lPhyCnt    = pPage->GetLogicCount(); //该页中的全部记录条数.


	POSITION posRecNo       = m_FakeRecNoList.GetHeadPosition();
	long lBlockStart    =  0;//当前操作的伪记录块的 启动伪记录号
	long lBlockEnd      =  0;//当前操作的伪记录块的 结束伪记录号
	bool bIsVersionEqual = true;//所有相关节点的版本和当前最新的是否一致,而且都是临时文件中

	
	int nLastNodeReamin = 0;//最后一个节点
	POSITION LastNodePos = NULL;//最后一个节点所在位置
	
	//do//处理一个记录块		
	while( lMaxOriginFakeNo > lBlockEnd && posRecNo)
	{	
		CFakeRecNo* pFakeNode        = (CFakeRecNo*)m_FakeRecNoList.GetAt(posRecNo);	
		//确定该记录块的首尾记录
		lBlockStart    = lBlockEnd + 1;        
		lBlockEnd     += pFakeNode->m_lngRecCount;
		
		if (lMaxOriginFakeNo < lBlockStart )
			break;//结束
		
		if (lMinFakeNo <= lBlockEnd )//落于该块
		{		
			nLastNodeReamin = lBlockEnd-(pPage->m_nOriginLoadCount + pPage->m_lngFakeStart)+1;
			LastNodePos = posRecNo;
				
			if(posDeleteStart == NULL)
			{
				posDeleteStart = posRecNo; //保存Delete时的起始结点位置（下面用）
				InLocallistPos = lMinFakeNo - lBlockStart;
			}
			iDeleteCnt++;//（下面用）
			if (nLastMemoryVersion != pFakeNode->m_nVersion 
				|| pFakeNode->m_nStoreType != storeTempFile)
				bIsVersionEqual = false;
		}

		m_FakeRecNoList.GetNext(posRecNo);
	}


	//**********   处理新链表  ************************
	if (posDeleteStart == NULL)
	{//在末尾添加
		POSITION posRecNo   = m_FakeRecNoList.GetTailPosition();
		CFakeRecNo* pFakeNode   = NULL;
		if (posRecNo)
			pFakeNode  = (CFakeRecNo*)m_FakeRecNoList.GetAt(posRecNo);	
		int nFilePos = m_pTempFile->_SavePage(pPage,0,pPage->m_nLogicCount);
		//( nVersion, nStoreType, lngPosition,  lngRecCount)
		if (pFakeNode && pFakeNode->m_nVersion == nLastMemoryVersion //版本相同
			//&& 0 //===temp
			&& lAllFake +1 ==pPage->m_lngFakeStart  //===uc
			&& pFakeNode->m_lngPosition + pFakeNode->m_lngRecCount*nRecordLen == nFilePos)
		{
			pFakeNode->m_lngRecCount = pFakeNode->m_lngRecCount+pPage->m_nLogicCount;
			return;//链表不用替换
		}
		else
		{
			CFakeRecNo* pFakeRecNo = new CFakeRecNo(nLastMemoryVersion,storeTempFile
				,nFilePos,pPage->m_nLogicCount);
			localFakeRecNoList.AddTail(pFakeRecNo);	
		}
	}
	else
	{

		Print();

		if (bIsVersionEqual ==true && pPage->m_nLogicCount == pPage->m_nOriginLoadCount)
		{//仅仅 修改, 字段也不变化,都是在临时文件内
			POSITION curPos = posDeleteStart;
			int nStartOffset = InLocallistPos;
			int nRemain  = pPage->m_nLogicCount ;
			int nPagePos = 0;
			for (int i=0; i<iDeleteCnt ;i++)
			{
				CFakeRecNo* pNode = (CFakeRecNo*)m_FakeRecNoList.GetAt(curPos);
				int nc =0;//当前节点要保存的个数
				if (pNode->m_lngRecCount - nStartOffset> nRemain)
					nc = nRemain;
				else
					nc = pNode->m_lngRecCount - nStartOffset;
				nRemain = nRemain -nc;
				m_pTempFile->_SavePage(pPage,nPagePos,nc,pNode->m_lngPosition,nStartOffset);

				nPagePos = nPagePos+nc;
				nStartOffset = 0;//第一个不为零,其余为零
				m_FakeRecNoList.GetNext(curPos);
			}
			//链表不用替换
			m_pIOFields_->SetCurVersion();
			m_pIOFields_->GotoBookmark();
			return;
		}
		else
		{//修改,且发生版本变化或者增减记录
			if (pPage->m_nLogicCount)
			{//非空页
				int nFilePos = m_pTempFile->_SavePage(pPage,0,pPage->m_nLogicCount);
				CFakeRecNo* pFakeRecNo = new CFakeRecNo(nLastMemoryVersion,
					storeTempFile,nFilePos,pPage->m_nLogicCount);			
				localFakeRecNoList.AddTail(pFakeRecNo);	
			}

			if (InLocallistPos)
			{//头节点
				CFakeRecNo* pNodeOld = (CFakeRecNo*)m_FakeRecNoList.GetAt(posDeleteStart);
				CFakeRecNo* pNodeNew = new CFakeRecNo(pNodeOld);
				pNodeNew->m_lngRecCount = InLocallistPos;
				localFakeRecNoList.AddHead(pNodeNew);
			}

			if (nLastNodeReamin )
			{//有尾节点
				CFakeRecNo* pNodeOld = (CFakeRecNo*)m_FakeRecNoList.GetAt(LastNodePos );
				CFakeRecNo* pNodeNew = new CFakeRecNo(pNodeOld);
				m_pIOFields_->SetCurVersion(pNodeNew->m_nVersion);//### 08-23
				if (pNodeNew->m_nStoreType == storeTempFile)
					pNodeNew->m_lngPosition =  pNodeNew->m_lngPosition 
						+ m_pIOFields_->QueryLength()*(pNodeNew->m_lngRecCount - nLastNodeReamin );
				else
					pNodeNew->m_lngPosition =  pNodeNew->m_lngPosition + 
						(pNodeNew->m_lngRecCount - nLastNodeReamin) ; 

				pNodeNew->m_lngRecCount = nLastNodeReamin ;
				
				localFakeRecNoList.AddTail(pNodeNew);
				m_pIOFields_->SetCurVersion();//恢复版本
			}
		}
		
	}
	
	//*****************链表的替换**************
	ReplaceList(localFakeRecNoList,posDeleteStart,iDeleteCnt);	

	Print();



	m_pIOFields_->SetCurVersion();
	m_pIOFields_->GotoBookmark();
}

/************************************************************************************
函数名称：
功能说明：将m_FakeRecNoList中需要保存的节点用临时链表的节点代替,私有函数,仅供内部调用.

详细解释：1.将临时链表的节点插入到m_FakeRecNoList中的相应的位置.
         2.删除m_FakeRecNoList中旧的节点.
    
出入参数：
[in]: 1.localFakeRecNoList    //临时链表.
      2.posDeleteStart        //开始删除的位置(可能为空,则在后面添加)
      3.iDeleteCnt            //删除的个数	  

[out]:1.posNewStart           //返回值,第一个被替换的节点,用于链表合并.
	  2.posNewEnd             //返回值,最后一个被替换的节点,用于链表合并.

	  注意:如果localFakeRecNoList为空,则返回posNewStart=posNewEnd,
	  posNewEnd返回posDeleteStart 前面一个(不可能为空)
返回类型：void

制作：YTLI   2002/07/12

修改: 
***********************************************************************************/
void CFakeRecNos::ReplaceList(CPtrList &localFakeRecNoList,
							  POSITION posDeleteStart,int iDeleteCnt 
							  )
{ 
	POSITION posRecNo = localFakeRecNoList.GetHeadPosition();//可能为空,因为全部删除的情况.

	//assert(posRecNo || posDeleteStart);//不可能两个同时为空.
	POSITION posNewStart =NULL,posNewEnd=NULL;
	if(!posDeleteStart)
	{//全部新增.且局部链表不为空.
		while(posRecNo != NULL)
		{
			m_FakeRecNoList.AddTail( localFakeRecNoList.GetNext(posRecNo) );
		}		
		posNewStart = m_FakeRecNoList.GetHeadPosition();
		posNewEnd = m_FakeRecNoList.GetTailPosition();
	}
	else
	{//有删除起始位置
		if (posRecNo)
		{//局部链表不为空,此种情况posNewStart,posNewEnd都不能为空
			//进行插入操作.
			posNewStart = 0;
			while(posRecNo != NULL)
			{
				if (!posNewStart )//给新的首节点赋值.
					posNewStart = m_FakeRecNoList.InsertBefore(posDeleteStart,localFakeRecNoList.GetNext(posRecNo));
				else
					m_FakeRecNoList.InsertBefore(posDeleteStart,localFakeRecNoList.GetNext(posRecNo));
			}
			//给新的尾节点赋值.
			posNewEnd = posDeleteStart;
			m_FakeRecNoList.GetPrev (posNewEnd);//引用传递,得到posNewEnd,不可能为空
		}
		else
		{//局部链表为空,此种情况posNewStart 为空,posNewEnd 可能为空
			posNewStart = 0;
			posNewEnd = posDeleteStart;
			m_FakeRecNoList.GetPrev (posNewEnd);//引用传递,得到posNewEnd,可能为空,即是头一个节点
		}

		//链表的删除	
		for(int i = 0; i<iDeleteCnt; i++)
		{
			POSITION posTemp     = posDeleteStart;
			CFakeRecNo*   pCFakeRecNo = (CFakeRecNo* )m_FakeRecNoList.GetNext(posDeleteStart);
			m_FakeRecNoList.RemoveAt(posTemp);
			delete pCFakeRecNo;
		}
		if (!posNewEnd)//如果posDeleteStart前面没有节点(优先原则),就取删除后面的节点
			posNewEnd = posDeleteStart;
		if (posNewStart==0)
			posNewStart = posNewEnd;//如果局部链表为空.则返回两个相等.
	}
}







/************************************************************************************
函数名称：
    CFakeRecNos::NewFile
功能说明：
    清空原来的链表。同时创建临时文件，根据传入的字段信息复制到m_pIOFields中
详细解释：
    
出入参数：
    [in,out]clone：
返回类型：
    bool :是否成功
制作：
    Eric2002-07-05
修改: 
************************************************************************************/
bool CFakeRecNos::NewFile( CFields* clone )
{
	Clear();
	//创建临时文件
    char szTempFile[MAX_PATH];
	GetTempFile(szTempFile);
	//m_pTempFile = new TempFile(szTempFile,TCLFile::modeReadWrite|TCLFile::modeCreate);
	m_pTempFile = new TempFile(szTempFile,0);
	m_pTempFile->m_pFakeRecNos = this ;

	//设置变量
	m_pIOFields_ = clone;
	return true;
}
/************************************************************************************
函数名称：
    CFakeRecNos::LoadFile
功能说明：
    打开一个文件，并且装入相应的字段结构，以及初始化临时文件
	1.调用 IniReadFile得到基本信息，初始化伪记录链表。
	2.创建临时文件
详细解释：
    
出入参数：
    [in,out]  lpszFileName：文件名
返回类型：
    bool:是否成功
制作：
    Eric2002-07-05
修改: 
************************************************************************************/
bool CFakeRecNos::LoadFile(LPCTSTR lpszFileName)
{
	Clear();
	if (! m_DriverManager.LoadFile(lpszFileName,&m_pOriginData,m_pIOFields_))
		return false;//###_Mod 2002-9-16 zlq

	
    char szTempFile[MAX_PATH];
	GetTempFile(szTempFile);
	//m_pTempFile = new TempFile(szTempFile,TCLFile::modeReadWrite|TCLFile::modeCreate);
	m_pTempFile = new TempFile(szTempFile,0);
	m_pTempFile->m_pFakeRecNos = this ;

	CFakeRecNo * pFakeRecNo = new  CFakeRecNo();
	pFakeRecNo->m_lngPosition =1;
	pFakeRecNo->m_lngRecCount = m_pOriginData->GetRecordCount();
	pFakeRecNo->m_nStoreType = storeTlFile;
	pFakeRecNo->m_nVersion = 0;
	m_FakeRecNoList.AddTail(pFakeRecNo);
	
	return true;
}


/************************************************************************************
函数名称：
    CFakeRecNos::GetTempFile
功能说明：
    取得一个唯一的文件名称用于临时文件.
详细解释：
    
出入参数：
    [in,out]szFileName：
返回类型：
    bool :是否成功
制作：
    Eric2002-07-05
修改: 
************************************************************************************/
bool CFakeRecNos::GetTempFile(char * szFileName)
{ 
	CTString spath;
    DWORD dwResult = TCSystem::GetTempPath(spath);
    //assert(dwResult);
    //取得唯一的文件名
	CTString sfile;
    TCSystem::GetTempFileName(spath, _T("~TLTemp"),sfile);
	memcpy(szFileName,sfile.GetData(),sfile.GetLength()+1);

	return true;
}
/************************************************************************************
函数名称：
    CFakeRecNos::CFakeRecNos()
功能说明:
    构造函数
详细解释:
出入参数:
        [in]:
		    1.pPages:页面集指针.
		[out]:
返回类型：
制作:
    Eric2002-07-08   //### Add by YTLI at 2002/08/12
修改:
************************************************************************************/
CFakeRecNos::CFakeRecNos(CPages* pPages)
{
	m_pPages = pPages;
#ifdef MarkWay_Encrypt  //是加密版本
	BYTE *pData = (unsigned char *)&m_pPages;
	for (int i= 0;i<4 ;i++)
	{
		*(pData+i) = *(pData+i)^tlcfgBuf[i];
	}
#endif


    char szTempFile[MAX_PATH];
	GetTempFile(szTempFile);
	m_pTempFile = new TempFile(szTempFile,TCLFile::modeReadWrite|TCLFile::modeCreate);
	m_pTempFile->m_pFakeRecNos = this ;


	m_pOriginData = new CFileOperation;
#ifdef MarkWay_Encrypt  //是加密版本
	BYTE *pData2 = (unsigned char *)&m_pPages;
	for ( i= 0;i<4 ;i++)
	{
		*(pData2+i) = *(pData2+i)^CurFileBuf[i];
	}
#endif
	//m_pIOFields = new CFields(pPages);//### 2002-9-5 Eric
	m_pIOFields_ = pPages->m_pFields;

}


/************************************************************************************
函数名称：
功能说明:
    调整原始数据源的个数,在读未知个数数据源时用.同时必须调整相应的pages的个数
详细解释:
出入参数:
        [in]:
		    1.nCount:与原来个数的偏移 即原来个数-实际个数
		[out]:
返回类型：
制作:
    //###_Mod 2002-10-15 zlq
修改:
************************************************************************************/
bool CFakeRecNos::AdjustOriginCount(int nOffset)
{
	CFakeRecNo* pFakeRecNoTemp;
	int nstTlCount = 0;
	int nstTempCount = 0;
	POSITION pos = m_FakeRecNoList.GetHeadPosition();
	while(pos)
	{
		pFakeRecNoTemp = (CFakeRecNo*)m_FakeRecNoList.GetAt(pos);
		if( pFakeRecNoTemp->m_nStoreType == storeTempFile )
		{
			nstTempCount += pFakeRecNoTemp->m_lngRecCount;
		}
		else if( pFakeRecNoTemp->m_nStoreType == storeTlFile )
		{
			nstTlCount += pFakeRecNoTemp->m_lngRecCount;
		}
		else 
		{
			//===temp AfxMessageBox("bool CFakeRecNos::AdjustOriginCount(int nCount)处发生错误!");
		}
		m_FakeRecNoList.GetNext(pos);
	}

	//也即对列中的必须做处理
	//assert(pFakeRecNoTemp->m_nStoreType == storeTlFile);
	if (!m_pPages->AdjustOriginCount(nOffset))
	{
		//assert(0);
	}
	
	pFakeRecNoTemp->m_lngRecCount= pFakeRecNoTemp->m_lngRecCount - nOffset;
	//assert(pFakeRecNoTemp->m_lngRecCount>=0);


	return true;
}



int CFakeRecNos::GetFileVersion()
{
	return m_pOriginData->GetFileVersion();
}

void CFakeRecNos::Print()//===temp
{
#ifdef _DEBUG
	POSITION posRecNo       = m_FakeRecNoList.GetHeadPosition();
	long lBlockStart    =  0;//当前操作的伪记录块的 启动伪记录号
	long lBlockEnd      =  0;//当前操作的伪记录块的 结束伪记录号
/*
		//===temp TRACE("***********节点链表*************** \r\n");;
		//===temp TRACE("开始\t个数\t类型\t位置\t版本 \r\n");;*/
	
	//do//处理一个记录块		
	while(  posRecNo)
	{	
		CFakeRecNo* pFakeNode        = (CFakeRecNo*)m_FakeRecNoList.GetAt(posRecNo);	
		//确定该记录块的首尾记录
		lBlockStart    = lBlockEnd + 1;        
		lBlockEnd     += pFakeNode->m_lngRecCount;
		////===temp TRACE(" ( %d \t  %d )\t%d\t%d\t%d\r\n",lBlockStart,pFakeNode->m_lngRecCount,pFakeNode->m_nStoreType,pFakeNode->m_lngPosition,pFakeNode->m_nVersion);

		m_FakeRecNoList.GetNext(posRecNo);
	}
	////===temp TRACE("************************** \r\n");;
#endif
	
}
