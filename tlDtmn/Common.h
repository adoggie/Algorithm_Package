// Common.h: interface for the CCommon class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMON_H__8C5BD960_14CD_4859_9805_596809D5BE0E__INCLUDED_)
#define AFX_COMMON_H__8C5BD960_14CD_4859_9805_596809D5BE0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCommon  
{
	class CItem{
		char m_buf[8];
		int m_bIsNull;
		CFieldType m_ftType;
		
	public:
		bool SetAsDouble(double value);
		bool IsNull();
		void SetNull();
		void SetType(CFieldType ftType);
		CItem();
		~CItem();
	};
public:
	static int RecordDiffLevel(CDataAccess *pDAA, CDWordArray *pIndexA, CDataAccess *pDAB,CDWordArray *pIndexB);
	static bool IsNumField(CField *pField);
	static void CopyFieldValue(CField *pFieldSource, CField *pFieldOut);
	static int CompareRecordValue(CDataAccess *pDAA,CDWordArray *pIndexA, CDataAccess *pDAB,CDWordArray *pIndexB);
	static int CompareFieldValue(CField *pFieldA, CField *pFieldB);
	CCommon();
	virtual ~CCommon();
	static CTString GetValueLabel(CField *pField ,int dataType,TDataSet *ptempdata,CDataInterface *pDI = NULL);

};
class CHeapInt
{//通过对象来管理内存
	int *m_pInt;
public:
	CHeapInt(int nLen){
		m_pInt = new int[nLen];
	}
	~CHeapInt(){
		delete []m_pInt;
	}
	int* GetAddr(){ return m_pInt;}
	
};
#endif // !defined(AFX_COMMON_H__8C5BD960_14CD_4859_9805_596809D5BE0E__INCLUDED_)
