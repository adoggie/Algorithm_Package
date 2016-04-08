// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CQUEUEITEM_3D08452801D1_INCLUDED
#define _INC_CQUEUEITEM_3D08452801D1_INCLUDED


//操作类型，是装入还是写出

typedef byte  COperateType;

#define otRead       0x01
#define otWrite      0x02
#define otPagesRead  0x03
class CPage;

//页操作请求队列
//##ModelId=3D08452801D1
struct CQueueItem 
{
public:
	//操作类型，是装入还是写出
	//
	//typedef byte  COperateType
	//
	//#define otRead       0x01
	//#define otWrite      0x02
	//#define otPagesRead  0x03
	//##ModelId=3D0845D90190
	COperateType nOperateType;

	//指向需要操作的数据页
	//##ModelId=3D08460E0132
	union{
	CPage* pPage;
	CPages* pPages;
	};

	//都有效
	//##ModelId=3D0846220307
	union{
	long FakeRecNo;
	long lngPhysicalRecNo;
	long lngFilePos;
	};

	//在装入时有效
	//##ModelId=3D08464B0270
	int count;

	//标志，0为正常，1为主过程处于等待状态，需要向主线程返回一
	//个消息（通过SetEvent来结束主过程的等待状态）
	//##ModelId=3D084E2401BB
	byte nFlag;

};

#endif /* _INC_CQUEUEITEM_3D08452801D1_INCLUDED */

