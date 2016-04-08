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
#define UnknownCount 0x7FFFFFFE //未知的记录条数不清 //###_Mod 2002-10-15 zlq
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
	//从指定的记录号开始，读入数据到缓存中，并返回数据集指针供
	//内部读取
	//返回值:
	//实际读取到的物理记录位置.正常情况下是起始物理记录号加上实际读取个数.
	//但当数据记录条数是未知时,可能返回最后记录数
	//注意:不需要调整m_nRecordCoun,由伪记录管理调度设置
	virtual int ReadRecords(CPage* pPage, CFromPos Flags, long RecNo, int Count)=0;

	//根据所有传入的数据集保存到文件里,并在内更新文件头的部分?
	//息
	virtual void SaveRecords(CPages* pPages)=0;
};

#endif // !defined(AFX_DRIVERBASE_H__0F2AA88A_A694_48F4_A80D_A375D7339803__INCLUDED_)
