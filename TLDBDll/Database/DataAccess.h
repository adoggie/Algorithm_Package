// DataAccess.h: interface for the CDataAccess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATAACCESS_H__B05FC6FA_9D47_452A_AEAB_CA518B42D3C6__INCLUDED_)
#define AFX_DATAACCESS_H__B05FC6FA_9D47_452A_AEAB_CA518B42D3C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>
#include "cpages.h"

#include "fieldlist.h"

#include "cfield.h"

#include "cdoublefield.h"
#include "cbooleanfield.h"
#include "ccurrencyfield.h"
#include "cdatefield.h"
#include "cstringfield.h"

#include "Missing.h"
//#include "ShowCharacteristic.h"
#include "cvaluelabel.h"
//************************读取其他数据库样本***************************
#define ST_NONE				0
#define ST_NATIVE_APPROX	1
#define ST_NATIVE_EXACT		2
#define ST_TLSS_APPROX		3
#define ST_TLSS_EXACT		4


//*********************统计参数*****************************
class CFieldList;
struct CStatistic
{
	double dMax ;   //最大值
	double dMin ;   //最小值
	double dSum ;   //求和 
	double dRange ; //范围 
	double dMean ;  //求平均值
	double dMedian; //求中位数
	int dValidCount;//有效统计个数
	int dTotalCount;//总统计个数
};
typedef byte  CSortType;
#define stDescent       0x01//降序
#define stAscend        0x02//升序
class CSelected;	
//****************过滤参数***********************************
enum CDealType{dtEffective,dtFilterd,dtDelete};//过滤处理类型 取有效数据  过滤  删除
enum CDealCount{dcAll,dcCur};      //处理个数       全部  当前纪录（暂时没有提供）
	
class AFX_EXT_CLASS CDataAccess  
{
	CTString m_strFileName;
	bool m_bIsOpen;

	CDataAccess& operator = (CDataAccess&);

public:
	bool AddField(std::string sName,CFieldType fType,int nWidth);
	CTString NameByIndex(int nIndex);
	int GetFieldCount();
	int IndexByField(CField* pField);
	int IndexByName(CTString sFieldName);
	CField* FieldByName(CTString FieldName);
	CField* FieldByIndex(int index);

	//*************打开文件操作****************************
	bool LoadFile(LPCTSTR lpszFileName);
	bool NewFile();

	//********************保存文件操作**************************
	//pstrOutFile 返回false 时且保存为天律文件时有效, 为保存时出错时的另存为的文件名字,由内部生成
	bool SaveFile(LPCTSTR lpszFileName,CTString *pstrOutFile);
	bool SaveOldVersionFile(LPCTSTR lpszFileName,CTString *pstrOutFile,int nVersion);
	//*****************************************************

	CTString GetFileName();
	bool SetFileName(CTString strFileName);
	//*****************************************************
/*
		bool GetStatisticButMedian(int nFieldIndex,CStatistic *pStat);
		bool GetStatisticButMedian(int *pnFieldIndex,CStatistic * pStat,int nCount);*/
	
	int GetDCDataType(int nIndex);

	//撤销上次的操作，如果成功则返回true
	bool Undo();
	//重复上次撤销的操作，如果成功则返回true
	bool Redo();
	//****************************************************
	//设置内存中最大的页个数 不能小于2页
	bool SetMaxPageCount(int nCount);
	//设置每个页的正常时的大小 一般50条
	bool SetNormalCountPerPage(int nCount);
	//得到页设置属性 包括 最大的页个数 每个页的正常时的大小 已经使用的页
	void GetPageSet(int &nMaxPageCount,int &nPageNormalSize,int &nUsePage);

	//*******************************************************
	bool Filter(CTString strCondition, CDataAccess** pFilterData,CDealType DealType = dtDelete,CDealCount dcCount=dcAll);
	//从传入的CDataAccess实例,拷贝结构和数据到当前本实例.会清空原来数据.
	bool CopyDataFromOther(CDataAccess*pSourceDA);

	bool CopyRecord(CDataAccess * pDest,CDataAccess * pSource);
	bool MergeSort(CTArray<int, int> & FieldArray) ;
	bool MergeSort(CDataAccess * & pDataAccess,CTArray<int,int> &FieldArray);
	//*******************************************************

	
	bool Close();
	bool IsOpen();
	bool Bof();
	bool Eof();	
	bool SetCurRecNo(int RecNo);
	int Next();
	int Prior();
	int MoveBy(int iSkipLen ,CFromPos MoveFlags);
	int First();
	int Last();
	
	int GetRecordCount();
	int GetRecNo();
	
	bool Edit();
	bool Append();
	bool Delete();
	bool Insert();
	bool Cancel();//不要了
	bool Post();	

	//==================================  Functions =======================================//
	CDataAccess();
	CDataAccess(CDataAccess*pTemplet);//从样板拷贝一个数据库字段结构，但没有记录信息
	~CDataAccess();
	void Swap(CDataAccess*pDA);//交换pDA和当前的数据
	CPages * m_pPages;
	CFieldList * m_pFieldList;
	
	//如果发现前面跳过了,则在这里出错，调用内部函数DemoRecordCount*100-24
	void Flush(int nChekCount);	
private:
	bool Statistic(int nIndex ,CStatistic& stat) ;
	bool CopyRecord(CPages * pDestPages,CPages * pSourcePages);
	int m_nBufLen;
	LPBYTE m_pBuf;
};

#endif // !defined(AFX_DATAACCESS_H__B05FC6FA_9D47_452A_AEAB_CA518B42D3C6__INCLUDED_)
