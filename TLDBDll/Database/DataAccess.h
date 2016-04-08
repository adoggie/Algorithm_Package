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
//************************��ȡ�������ݿ�����***************************
#define ST_NONE				0
#define ST_NATIVE_APPROX	1
#define ST_NATIVE_EXACT		2
#define ST_TLSS_APPROX		3
#define ST_TLSS_EXACT		4


//*********************ͳ�Ʋ���*****************************
class CFieldList;
struct CStatistic
{
	double dMax ;   //���ֵ
	double dMin ;   //��Сֵ
	double dSum ;   //��� 
	double dRange ; //��Χ 
	double dMean ;  //��ƽ��ֵ
	double dMedian; //����λ��
	int dValidCount;//��Чͳ�Ƹ���
	int dTotalCount;//��ͳ�Ƹ���
};
typedef byte  CSortType;
#define stDescent       0x01//����
#define stAscend        0x02//����
class CSelected;	
//****************���˲���***********************************
enum CDealType{dtEffective,dtFilterd,dtDelete};//���˴������� ȡ��Ч����  ����  ɾ��
enum CDealCount{dcAll,dcCur};      //�������       ȫ��  ��ǰ��¼����ʱû���ṩ��
	
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

	//*************���ļ�����****************************
	bool LoadFile(LPCTSTR lpszFileName);
	bool NewFile();

	//********************�����ļ�����**************************
	//pstrOutFile ����false ʱ�ұ���Ϊ�����ļ�ʱ��Ч, Ϊ����ʱ����ʱ�����Ϊ���ļ�����,���ڲ�����
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

	//�����ϴεĲ���������ɹ��򷵻�true
	bool Undo();
	//�ظ��ϴγ����Ĳ���������ɹ��򷵻�true
	bool Redo();
	//****************************************************
	//�����ڴ�������ҳ���� ����С��2ҳ
	bool SetMaxPageCount(int nCount);
	//����ÿ��ҳ������ʱ�Ĵ�С һ��50��
	bool SetNormalCountPerPage(int nCount);
	//�õ�ҳ�������� ���� ����ҳ���� ÿ��ҳ������ʱ�Ĵ�С �Ѿ�ʹ�õ�ҳ
	void GetPageSet(int &nMaxPageCount,int &nPageNormalSize,int &nUsePage);

	//*******************************************************
	bool Filter(CTString strCondition, CDataAccess** pFilterData,CDealType DealType = dtDelete,CDealCount dcCount=dcAll);
	//�Ӵ����CDataAccessʵ��,�����ṹ�����ݵ���ǰ��ʵ��.�����ԭ������.
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
	bool Cancel();//��Ҫ��
	bool Post();	

	//==================================  Functions =======================================//
	CDataAccess();
	CDataAccess(CDataAccess*pTemplet);//�����忽��һ�����ݿ��ֶνṹ����û�м�¼��Ϣ
	~CDataAccess();
	void Swap(CDataAccess*pDA);//����pDA�͵�ǰ������
	CPages * m_pPages;
	CFieldList * m_pFieldList;
	
	//�������ǰ��������,����������������ڲ�����DemoRecordCount*100-24
	void Flush(int nChekCount);	
private:
	bool Statistic(int nIndex ,CStatistic& stat) ;
	bool CopyRecord(CPages * pDestPages,CPages * pSourcePages);
	int m_nBufLen;
	LPBYTE m_pBuf;
};

#endif // !defined(AFX_DATAACCESS_H__B05FC6FA_9D47_452A_AEAB_CA518B42D3C6__INCLUDED_)
