// Copyright (C) 1991 - 1999 Rational Software Corporation

#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef _INC_CQUEUEITEM_3D08452801D1_INCLUDED
#define _INC_CQUEUEITEM_3D08452801D1_INCLUDED


//�������ͣ���װ�뻹��д��

typedef byte  COperateType;

#define otRead       0x01
#define otWrite      0x02
#define otPagesRead  0x03
class CPage;

//ҳ�����������
//##ModelId=3D08452801D1
struct CQueueItem 
{
public:
	//�������ͣ���װ�뻹��д��
	//
	//typedef byte  COperateType
	//
	//#define otRead       0x01
	//#define otWrite      0x02
	//#define otPagesRead  0x03
	//##ModelId=3D0845D90190
	COperateType nOperateType;

	//ָ����Ҫ����������ҳ
	//##ModelId=3D08460E0132
	union{
	CPage* pPage;
	CPages* pPages;
	};

	//����Ч
	//##ModelId=3D0846220307
	union{
	long FakeRecNo;
	long lngPhysicalRecNo;
	long lngFilePos;
	};

	//��װ��ʱ��Ч
	//##ModelId=3D08464B0270
	int count;

	//��־��0Ϊ������1Ϊ�����̴��ڵȴ�״̬����Ҫ�����̷߳���һ
	//����Ϣ��ͨ��SetEvent�����������̵ĵȴ�״̬��
	//##ModelId=3D084E2401BB
	byte nFlag;

};

#endif /* _INC_CQUEUEITEM_3D08452801D1_INCLUDED */

