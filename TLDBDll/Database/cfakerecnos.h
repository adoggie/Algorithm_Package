// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CFAKERECNOS_3DCB548A0230_INCLUDED
#define _INC_CFAKERECNOS_3DCB548A0230_INCLUDED

#include "cpage.h"
#include "cqueueitem.h"
#include "cfakerecno.h"
//#include <process.h>
#include "DriverManager.h"

class TempFile;


class CFileOperation;
//typedef CTypedPtrList<CPtrList, CFakeRecNo*> FakeRecNoList;
typedef byte  CLockState;

#define lsSucceed      0x00 //设置成功
#define lsNoIn         0x01 //已经刚处理完该页

#define StandardBlockRecCnt  100//每个结点中标准的记录条数
#define MaxBlockRecCnt       150//每个结点中最大的记录条数
class CPages;
class CDriverBase;
//该链表的个数不定，随着不停的修改添加会增大，当然也有可能会缩小
//伪记录号集决定读入时的数据来源以及是否保存到临时文件，调整链表等
class CFakeRecNos 
{
public:
	void Print();
	int GetFileVersion();

	CDriverManager m_DriverManager;

	bool Clear();
	bool NewFile( CFields* clone );
	bool LoadFile(LPCTSTR lpszFileName);
	static bool GetTempFile(char * szFileName);
	CFakeRecNos(CPages*pPages);
	~CFakeRecNos();

	CPages*m_pPages ;//

	//##ModelId=3D07E1CD032C
	TempFile* m_pTempFile;

	//##ModelId=3D07E21300FC
	CDriverBase* m_pOriginData;
	//排队等待的处理的队列
	//CTypedPtrList<CPtrList, CQueueItem*> m_QueueItemList;
	CPtrList m_QueueItemList;
	//记录对应的伪记录号与物理文件的对应关系
	//CTypedPtrList<CPtrList, CFakeRecNo*> m_FakeRecNoList;
	CPtrList m_FakeRecNoList;


	//用于IO操作的字段集
	CFields* m_pIOFields_;

	//先根据传入参数先生成结构，并加到排队等待的处理的队列m_pQueueItemList中去
	//然后检查m_bThreadStart 是否如果线程已经停止，则重新启动线程,
	//并m_bThreadStart设置为启动状态(true)	
	void InQueueLoad(CPage* pPage, long FakeRecNo, int count, bool IsNeedFeedback = false);

	//注意：应该将当前的伪记录号放到结构CQueueItem中，因为是多
	//线程，页pPage内的伪记录号，会被后来的写入操作所修改
	//##ModelId=3D093C8300A5
	//void InQueueUnLoad(CPage* pPage, bool IsNeedFeedback = false);

	//进行页面的装入工作,如果本页面其实不为空,那么先进行物理清空操作
	//##ModelId=3D055874032F
	void Load(CPage* pPage, long FakeRecNo, int count);


	//页面的保存操作，同时修改链表，必须使用传入的伪记录号，而
	//不是页面内的伪记录号	
	void UnLoad(CPage* pPage);


private:
	bool AdjustOriginCount(int nOffset);


	void ReplaceList(CPtrList &localFakeRecNoList,POSITION posDeleteStart,int iDeleteCnt);


};

#endif /* _INC_CFAKERECNOS_3DCB548A0230_INCLUDED */

