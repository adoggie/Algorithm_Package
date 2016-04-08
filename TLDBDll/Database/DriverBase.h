// DriverBase.h: interface for the CDriverBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRIVERBASE_H__0F2AA88A_A694_48F4_A80D_A375D7339803__INCLUDED_)
#define AFX_DRIVERBASE_H__0F2AA88A_A694_48F4_A80D_A375D7339803__INCLUDED_
#include "cpage.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CPage;
//###_Mod 2002-10-23 zlq
#define UnknownCount 0x7FFFFFFE //δ֪�ļ�¼�������� //###_Mod 2002-10-15 zlq
class AFX_EXT_CLASS CDriverBase 
{

protected:
	int m_nRecordCount;
public:
	virtual int GetFileVersion();
	void SetRecordCount(int nRecordCount)
	{
		m_nRecordCount = nRecordCount;
	};
	int GetRecordCount();
	CDriverBase();
	virtual ~CDriverBase();
	//��ָ���ļ�¼�ſ�ʼ���������ݵ������У����������ݼ�ָ�빩
	//�ڲ���ȡ
	//����ֵ:
	//ʵ�ʶ�ȡ���������¼λ��.�������������ʼ�����¼�ż���ʵ�ʶ�ȡ����.
	//�������ݼ�¼������δ֪ʱ,���ܷ�������¼��
	//ע��:����Ҫ����m_nRecordCoun,��α��¼�����������
	virtual int ReadRecords(CPage* pPage, CFromPos Flags, long RecNo, int Count)=0;

	//�������д�������ݼ����浽�ļ���,�����ڸ����ļ�ͷ�Ĳ���?
	//Ϣ
	virtual void SaveRecords(CPages* pPages)=0;
};

#endif // !defined(AFX_DRIVERBASE_H__0F2AA88A_A694_48F4_A80D_A375D7339803__INCLUDED_)
