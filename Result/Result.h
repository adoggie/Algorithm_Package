#ifndef _RESULT_H
#define _RESULT_H
#pragma  warning(disable:4251)

#include "Define.h"
#include "../Public/TString.h"

#include "TChart1.h"
#include "TChartArea.h"
#include "TChartBar.h"
#include "TChartBoxplot.h"
#include "TChartErrBar.h"
#include "TChartHistogram.h"
#include "TChartLine.h"
#include "TChartPareto.h"
#include "TChartPie.h"
#include "TChartScatterPlot.h"
#include "TChartSequence.h"
#include "TChartControl.h"
#include "TChartROC.h"
#include "TChartClustering.h"
#include "TChartHighLow.h"
#include "TChartPP.h"
#include "TChartAutocorrelations.h"
#include "TChartCrossCorrelations.h"
#include "TChartCurve.h"
#include "TChartPedigree.h"
#include <vector>
using namespace std;

#define SaveDigit(pBuf, value) {memcpy(pBuf,&value,sizeof(value));pBuf = pBuf+sizeof(value);}
#define SaveString(pBuf, value) {int vv = value.GetLength();\
								SaveDigit(pBuf,vv);\
							memcpy(pBuf,value.GetData(),value.GetLength());\
							pBuf = pBuf+value.GetLength(); }
//#define LenDigit(value) sizeof(value)
//#define LenString(value) (sizeof(int)+ value.GetLength())
#define AddDigitLen(toValue,value) toValue=toValue + sizeof(value)
#define AddStringLen(toValue,value) toValue=toValue + sizeof(int)+ value.GetLength()


//���н������ĸ���,����ȡ�����п��ܵĽ�����͵Ĺ�ͬ���Ժͺ���
class AFX_EXT_CLASS CRsltObject
{
public:
	virtual int GetBufLen();
	virtual void Serialize(unsigned char *&pBuf);
	CRsltObject();
	virtual ~CRsltObject();

	virtual void Serialize (TCLFile &file);
	virtual void	Print();
	
	//�趨�����ID��
	void     SetID(unsigned int nID){m_nID=nID;};
	//ȡ�ö����ID��
	unsigned int     GetID(){return m_nID;};
	//�趨���������
	void     SetName(CTString strName){m_strName=strName;};
	//ȡ�ö��������
	CTString  GetName(){return m_strName;};
	//�趨���������
	void     SetType(unsigned int nType){m_nType=nType;};
	//ȡ�ö��������
	unsigned int     GetType(){return m_nType;};

	virtual  CRsltObject* GetByID(unsigned int id){return NULL;};
 
protected:
	unsigned int     m_nID;    //��ǰ��������Ψһ��ʶ��
	unsigned int     m_nType;  //��ǰ������������
	CTString  m_strName;//��ǰ������������
};

/*
��Ŷ�ĳ�������������ӱ������Ա��������з�����Ľ�� ��
��ָ��һ�η��������и���ĳ�������������������в�ͬ���������Ľ������
����Ե�ǰ������������������һϵ�в�ͬ����ļ���
*/

class CRsltVariable;
class AFX_EXT_CLASS CRsltVariable : public CRsltObject  
{
public:
	static void aa ()
	{
		CRsltVariable *pp = new CRsltVariable;
		delete pp;
	}
	CRsltVariable();
	CRsltVariable(CTString strName);
	virtual ~CRsltVariable();	
	//���л����溯��
	virtual void Serialize(TCLFile &file);
	virtual void	Print();
	
public:
	int GetBufLen();
	void Serialize(unsigned char *&pBuf);
	//ȡ�õ�ǰ���󼯺��������Ӷ��������
	bool     GetSumCount(int &nCount);
	//��ǰ���������һ�������Ϣ������
	int   Add(CRsltVariable* pElement);
	//ȡ��ָ���������Ӷ���
	CRsltVariable* Item(int nIndex);
	//����ָ�������ڼ����е�����
	int   Find(CRsltVariable* pElement);
	//��ָ����λ�ò���һ���Ӷ���
	bool  InsetAt(int nIndex,CRsltVariable* pElement);
	//ȡ�õ�ǰ���󼯺���ֱ���Ӷ��������
	int   Count();
	//��ȥ�����д�ָ��λ�ã�nIndex�����ָ��������nCount����ֱ���Ӷ���
	void  RemoveAt(int nIndex,int nCount=1);
	//��ռ��ϣ�����ɾ���Ӷ���
	void  RemoveAll();
	//ɾ�������д�ָ��λ�����ָ��������ֱ���Ӷ���
	bool  Delete(int nIndex, int nCount=1);
	//������ϣ���ɾ�������Ӷ���
	void  Clear();
	//���Ƶ�ǰ���󣬲�������ָ���Ķ���pRsltObject����
	virtual void Copy(CRsltObject* pRsltObject);
	CRsltVariable* GetFirstItem();
	virtual CRsltObject* GetByID(unsigned int id);

private:
	unsigned int  GetNoUsedID();

protected:
	//���ÿ�����������Ϣ�ļ��ϡ������п������ı���ͼ�Ρ����ȣ�
	std::vector<CRsltVariable*> m_tpVariable;
};

/* ����� ---��ŵ��η����Ľ�� ͳ�Ʒ�������࣬
��ŵ��η����������ļ����࣬���ļ����д�ŵ��Ǳ��������
CRsltVariable�Ķ����������CRsltVariable�Ķ���*/
class AFX_EXT_CLASS CResult : public CRsltObject
{
public:
	CResult();
	virtual ~CResult();
	CResult(CTString strName);
	CResult(char * strName);
	//���л����溯��
	virtual void    Serialize (TCLFile &file);
	virtual void	Print();
public:
	int GetBufLen();
	void Serialize(unsigned char *&pBuf);
	//ȡ�õ�ǰ���󼯺��������Ӷ��������
	int   GetSumCount();
	//��ǰ���������һ�������Ϣ������
	int   Add(CRsltVariable* pVariable);
	//ȡ��ָ���������Ӷ���
	CRsltVariable* Item(int nIndex);
	//ȡ��ָ��ID���Ӷ���
	CRsltVariable* Item(unsigned int nID);
	//����ָ�������ڼ����е�����
	int   Find(CRsltVariable* pVariable);
	//��ָ����λ�ò���һ���Ӷ���
	bool  InsetAt(int nIndex,CRsltVariable* pVariable);
	//ȡ�õ�ǰ���󼯺���ֱ���Ӷ��������
	int   Count();
	//��ȥ�����д�ָ��λ�ã�nIndex�����ָ��������nCount����ֱ���Ӷ���
	void  RemoveAt(int nIndex,int nCount=1);
	//��ռ��ϣ�����ɾ���Ӷ���
	void  RemoveAll();
	//ɾ�������д�ָ��λ�����ָ��������ֱ���Ӷ���
	bool  Delete(int nIndex, int nCount=1);
	void  Clear();
	//������ϣ���ɾ�������Ӷ���
	void  Optimize();//�Ż����ϵĽṹ���������еĻ�Ԫ�ؽ����������ǰ�����ѱ�������������ں󲿣�
	//���Ƶ�ǰ���󣬲�������ָ���Ķ���pRsltObject����

	CRsltVariable* GetFirstItem();
	virtual CRsltObject* GetByID(unsigned int id);

private:
	//ȡ�ü�������С��δ�õ�ID��
	unsigned int  GetNoUsedID();

protected:
	//���ÿ�η��������н����Ϣ�ļ���
	std::vector<CRsltVariable*>m_tpResult;//���м��л�Ԫ�ؽ�������Ҳ�б����������󣨼������м����ܴ��ڵ�һ�Ľ������Ҳ���ܴ��ڽ���ļ��϶���
};


/*
 ����ܼ� ---��Ŷ�η����Ľ�� ////////////////////////////////////////////////////////////////////
���ÿ�η�������ļ��ϣ�����ŵ�ǰ��֮ǰ�����ɴζ�����������Ľ����
�����и�CTypedPtrArray<CObArray,CResult*>m_tpResultS���ϱ�����
���ڴ��ÿ�η����Ľ����*/
class AFX_EXT_CLASS CResultS : public CRsltObject  
{
public:
	CResultS();
	virtual ~CResultS();
	virtual void Serialize(TCLFile &file);
	virtual void	Print();
	
public:
	void  SetName(CTString strName){m_strName=strName;};
	CTString GetName(){return m_strName;};
	void  SetFileName(CTString str){};
	CTString GetFileName(){return "";};
	int   Add(CResult* pResult);
	int   Find(CResult* pResult);
	int   Count();
	CResult* Item(int nIndex);
	CResult* Item(unsigned int nID);
	bool  InsertAt(int nIndex,CResult* pResult);
	void  RemoveAt(int nIndex,int nCount=1);
	void  RemoveAll();
	void  Delete(int nIndex,int nCount=1);
	void  Clear();
 	virtual CRsltObject* GetByID(unsigned int id);
	int GetSumCount();
	virtual void Copy(CRsltObject* pRsltObject);

private:
	unsigned int  GetNoUsedID();

protected:
	//���ÿ�η����Ľ��
	std::vector<CResult*>m_tpResultS;

};


//////////////////////////////////////////////////////////////////
/// �ı���Ϣ��
/*CRsltElementText
�����ı�ע�ǽ������Ļ��࣬���ĸ�����CrsltElement�������Ķ�������һ��
��Ա������������е���Ҫ������ı�ע����Ϣ����Щ��Ϣ��������һ�����ַ�
����Ϣ��Ҳ�������ɶ���ַ�����ɵļ��ϣ����ǽ����ݲ�ͬ������Ҫ���Բ�ͬ
����ʽ��������ַ����������ǵ��е�Ҳ�����Ƕ��еģ�
*/
class AFX_EXT_CLASS CRsltElementText : public CRsltVariable  
{
public:
	void Serialize(unsigned char *&pBuf);
	int GetBufLen();
	CRsltElementText();
	CRsltElementText(CTString strName);
	CRsltElementText(char * strName);
	virtual ~CRsltElementText();
	virtual void Serialize(TCLFile &file);
	virtual void	Print();
	int   AddString(CTString strValue);
	int   Count();
	CTString GetString(int nIndex);
	bool  DeleteAt(int nIndex);

private:
	int     GetStringRow(CTString string);
protected:
	void IniData();
	CTStringArray    m_strTextArray;
};

////////////////////////////////////////////////////////////
/// ����Ϣ��
/*���ĸ�����CrsltElement����������������������������˵�ǹ����࣬
������ֻ��һ����ű���Ϣ�ĳ�Ա����������������������*/
class CTLTable;
class AFX_EXT_CLASS CRsltElementTable : public CRsltVariable  
{
public:
	CRsltElementTable();
	CRsltElementTable(CTString strName,CTLTable* pTable);
	virtual ~CRsltElementTable();
	virtual void Serialize(TCLFile &file);
	virtual void	Print();
	
public:
	int GetBufLen();
	void Serialize(unsigned char *&pBuf);
	//����Ϣ�࣬��������е��Ա���ʽ����Ľ����Ϣ
	CTLTable *m_table;

};


#include "DecTreeWnd.h"
class AFX_EXT_CLASS CRsltEleDecTree : public CRsltVariable  
{
public:
	CRsltEleDecTree();
	CRsltEleDecTree(CTString strName,CDecTreeWnd *pDecTreeWnd);
	virtual ~CRsltEleDecTree();
	virtual void Serialize(TCLFile &file);
	virtual void	Print();
public:
	int GetBufLen();
	void Serialize(unsigned char *&pBuf);
	//������ͼ����Ϣ�࣬����ž�����ͼ�ε�������Ϣ
	CDecTreeWnd *m_pDecTreeWnd;


};

//0δ���� ��4 Ϊ���� �� 8 Ϊ�������� 16 Ϊ����, 1 Ϊ�ַ���
////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
enum ETableType
{
	rNull = 0,
	rString,
	rInt,
	rDouble,
	rComplex
};
struct TableData
{
	ETableType nDataType;
	union 
	{
	public:
		int		nValue;
		char   *pValue;
		double  dValue;
	};
};


struct TRowData
{
	CTString  m_szName;
	CPtrArray m_arData;
};

#include <vector>

/*��ŷ�����һ���������Ա���ʽ����Ľ����Ϣ�����̳���CWnd�������Լ�
������������������ĸ���������������ڵľ����*/
class AFX_EXT_CLASS CTLTable
{
public:
	CTLTable();           // protected constructor used by dynamic creation
	virtual ~CTLTable();
	virtual void Serialize(TCLFile &file);
	virtual void	Print();
	
public:
	int GetBufLen();
	void Serialize(unsigned char *&pBuf);
	void SetTitle(char *pszTitle);
	void SetTitle(CTString pszTitle);
	CTString GetTitle(){return m_strTitle;};
	void SetHead(char * str);
	void SetHead(CTString str);
	CTString GetHead(){return m_strHead;};
	void SetTail(char * str);
	void SetTail(CTString str);
	CTString GetTail(){return m_strTail;};
	//������nRowΪ������������nColΪ������������nTableTypeΪ�����ı���������ͣ�
	//�������ͷ�ΪrInt, rDouble rStringΪ��Ϊ���������㣬�ַ���
	bool CreateTable(int nRow, int nCol); 
	void SetRows(int nRow);
	void SetCols(int nCol);
	long  GetTableRowS();
	int  GetTableColS();

	void InsertColumn(int nItemNum, CTString strColumnName);
	void InsertItem(int nSubItem, CTString strColumnName,bool bFlag = true);
//	void InsertColumn(int nItemNum, char *pColumnName);
//	void InsertItem(int nSubItem, char *pColumnName,bool bFlag = true);
	
	void SetItemText(const int nItemNum,const int nSubItem,int nData);
	void SetItemText(const int nItemNum,const int nSubItem,double dbData);
	void SetItemText(const int nItemNum,const int nSubItem,char * pszName);
	void SetItemText(const int nItemNum,const int nSubItem,CTString strData);

	TableData*  GetItemText(const int nItemNum,const int nSubItem);

	void Clear();
	CTString GetColumnInfo(int nIndex);
	CTString GetRowInfo(int nIndex);
protected:
	void AutoSize(int nRows,int nCols);
	CTString   m_strTitle;//���ı���
	CTString   m_strHead;//���ı�ͷע��
	CTString   m_strTail;//����ע��
	CTString   m_strException;//����쳣�������Ϣ

	int       m_nRows;//����������
	int       m_nCols;//����������
	
	CTStringArray m_arColumnInfo;//�����ͷ ���� TColumnInfo
	CPtrArray m_arRowInfo;//��������Ϣ����,�����������
	CPtrArray m_arDataInfo;//����������ݵĸ�����Ϣ
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// ͼ����Ϣ��
/*���ĸ�����CrsltElement����ͬC��sltElementTableһ����Ҳֻ��ͼ�����һ��
�����࣬��ֻ��һ�����ͼ����Ϣ�ĳ�Ա������*/
class AFX_EXT_CLASS CRsltElementChart : public CRsltVariable  
{
public:
	void Serialize(unsigned char *&pBuf);
	int GetBufLen();
	CTChart* m_pChart;
	virtual void Serialize(TCLFile &file);
	virtual void	Print();
	CRsltElementChart(CTString strName,CTChart* pChart);
	CRsltElementChart();
	virtual ~CRsltElementChart();
};

#pragma  warning(default:4251)

#endif































































