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

#define lsSucceed      0x00 //���óɹ�
#define lsNoIn         0x01 //�Ѿ��մ������ҳ

#define StandardBlockRecCnt  100//ÿ������б�׼�ļ�¼����
#define MaxBlockRecCnt       150//ÿ����������ļ�¼����
class CPages;
class CDriverBase;
//������ĸ������������Ų�ͣ���޸���ӻ����󣬵�ȻҲ�п��ܻ���С
//α��¼�ż���������ʱ��������Դ�Լ��Ƿ񱣴浽��ʱ�ļ������������
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
	//�Ŷӵȴ��Ĵ���Ķ���
	//CTypedPtrList<CPtrList, CQueueItem*> m_QueueItemList;
	CPtrList m_QueueItemList;
	//��¼��Ӧ��α��¼���������ļ��Ķ�Ӧ��ϵ
	//CTypedPtrList<CPtrList, CFakeRecNo*> m_FakeRecNoList;
	CPtrList m_FakeRecNoList;


	//����IO�������ֶμ�
	CFields* m_pIOFields_;

	//�ȸ��ݴ�����������ɽṹ�����ӵ��Ŷӵȴ��Ĵ���Ķ���m_pQueueItemList��ȥ
	//Ȼ����m_bThreadStart �Ƿ�����߳��Ѿ�ֹͣ�������������߳�,
	//��m_bThreadStart����Ϊ����״̬(true)	
	void InQueueLoad(CPage* pPage, long FakeRecNo, int count, bool IsNeedFeedback = false);

	//ע�⣺Ӧ�ý���ǰ��α��¼�ŷŵ��ṹCQueueItem�У���Ϊ�Ƕ�
	//�̣߳�ҳpPage�ڵ�α��¼�ţ��ᱻ������д��������޸�
	//##ModelId=3D093C8300A5
	//void InQueueUnLoad(CPage* pPage, bool IsNeedFeedback = false);

	//����ҳ���װ�빤��,�����ҳ����ʵ��Ϊ��,��ô�Ƚ���������ղ���
	//##ModelId=3D055874032F
	void Load(CPage* pPage, long FakeRecNo, int count);


	//ҳ��ı��������ͬʱ�޸���������ʹ�ô����α��¼�ţ���
	//����ҳ���ڵ�α��¼��	
	void UnLoad(CPage* pPage);


private:
	bool AdjustOriginCount(int nOffset);


	void ReplaceList(CPtrList &localFakeRecNoList,POSITION posDeleteStart,int iDeleteCnt);


};

#endif /* _INC_CFAKERECNOS_3DCB548A0230_INCLUDED */

