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


//所有结果对象的父类,它提取了所有可能的结果类型的共同属性和函数
class AFX_EXT_CLASS CRsltObject
{
public:
	virtual int GetBufLen();
	virtual void Serialize(unsigned char *&pBuf);
	CRsltObject();
	virtual ~CRsltObject();

	virtual void Serialize (TCLFile &file);
	virtual void	Print();
	
	//设定对象的ID号
	void     SetID(unsigned int nID){m_nID=nID;};
	//取得对象的ID号
	unsigned int     GetID(){return m_nID;};
	//设定对象的名称
	void     SetName(CTString strName){m_strName=strName;};
	//取得对象的名称
	CTString  GetName(){return m_strName;};
	//设定对象的类型
	void     SetType(unsigned int nType){m_nType=nType;};
	//取得对象的类型
	unsigned int     GetType(){return m_nType;};

	virtual  CRsltObject* GetByID(unsigned int id){return NULL;};
 
protected:
	unsigned int     m_nID;    //当前结果对象的唯一标识符
	unsigned int     m_nType;  //当前结果对象的类型
	CTString  m_strName;//当前结果对象的名称
};

/*
存放对某个变量（如因子变量或自变量）进行分析后的结果 ，
是指在一次分析运算中根据某个变量对其它变量进行不同分析产生的结果集合
，或对当前变量自身分析后产生的一系列不同结果的集合
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
	//序列化保存函数
	virtual void Serialize(TCLFile &file);
	virtual void	Print();
	
public:
	int GetBufLen();
	void Serialize(unsigned char *&pBuf);
	//取得当前对象集合中所有子对象的总数
	bool     GetSumCount(int &nCount);
	//向当前对象中添加一个结果信息（集）
	int   Add(CRsltVariable* pElement);
	//取得指定索引的子对象
	CRsltVariable* Item(int nIndex);
	//返回指定对象在集合中的索引
	int   Find(CRsltVariable* pElement);
	//在指定的位置插入一个子对象
	bool  InsetAt(int nIndex,CRsltVariable* pElement);
	//取得当前对象集合中直接子对象的总数
	int   Count();
	//移去集合中从指定位置（nIndex）起的指定个数（nCount）的直接子对象
	void  RemoveAt(int nIndex,int nCount=1);
	//清空集合，但不删除子对象
	void  RemoveAll();
	//删除集合中从指定位置起的指定个数的直接子对象
	bool  Delete(int nIndex, int nCount=1);
	//清除集合，并删除所有子对象
	void  Clear();
	//复制当前对象，并返回在指定的对象（pRsltObject）中
	virtual void Copy(CRsltObject* pRsltObject);
	CRsltVariable* GetFirstItem();
	virtual CRsltObject* GetByID(unsigned int id);

private:
	unsigned int  GetNoUsedID();

protected:
	//存放每个独立结果信息的集合。（它有可能是文本、图形、表单等）
	std::vector<CRsltVariable*> m_tpVariable;
};

/* 结果集 ---存放单次分析的结果 统计分析结果类，
存放单次分析运算结果的集合类，它的集合中存放的是变量结果类
CRsltVariable的对象或基结果类CRsltVariable的对象*/
class AFX_EXT_CLASS CResult : public CRsltObject
{
public:
	CResult();
	virtual ~CResult();
	CResult(CTString strName);
	CResult(char * strName);
	//序列化保存函数
	virtual void    Serialize (TCLFile &file);
	virtual void	Print();
public:
	int GetBufLen();
	void Serialize(unsigned char *&pBuf);
	//取得当前对象集合中所有子对象的总数
	int   GetSumCount();
	//向当前对象中添加一个结果信息（集）
	int   Add(CRsltVariable* pVariable);
	//取得指定索引的子对象
	CRsltVariable* Item(int nIndex);
	//取得指定ID的子对象
	CRsltVariable* Item(unsigned int nID);
	//返回指定对象在集合中的索引
	int   Find(CRsltVariable* pVariable);
	//在指定的位置插入一个子对象
	bool  InsetAt(int nIndex,CRsltVariable* pVariable);
	//取得当前对象集合中直接子对象的总数
	int   Count();
	//移去集合中从指定位置（nIndex）起的指定个数（nCount）的直接子对象
	void  RemoveAt(int nIndex,int nCount=1);
	//清空集合，但不删除子对象
	void  RemoveAll();
	//删除集合中从指定位置起的指定个数的直接子对象
	bool  Delete(int nIndex, int nCount=1);
	void  Clear();
	//清除集合，并删除所有子对象
	void  Optimize();//优化集合的结构（即将其中的基元素结果类对象放在前部，把变量结果类对象放在后部）
	//复制当前对象，并返回在指定的对象（pRsltObject）中

	CRsltVariable* GetFirstItem();
	virtual CRsltObject* GetByID(unsigned int id);

private:
	//取得集合中最小的未用的ID号
	unsigned int  GetNoUsedID();

protected:
	//存放每次分析中所有结果信息的集合
	std::vector<CRsltVariable*>m_tpResult;//其中即有基元素结果类对象，也有变量结果类对象（即集合中即可能存在单一的结果对象，也可能存在结果的集合对象）
};


/*
 结果总集 ---存放多次分析的结果 ////////////////////////////////////////////////////////////////////
存放每次分析结果的集合，存放着当前及之前的若干次独立分析运算的结果，
其中有个CTypedPtrArray<CObArray,CResult*>m_tpResultS集合变量，
用于存放每次分析的结果。*/
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
	//存放每次分析的结果
	std::vector<CResult*>m_tpResultS;

};


//////////////////////////////////////////////////////////////////
/// 文本信息类
/*CRsltElementText
所有文本注记结果对象的基类，它的父类是CrsltElement，在它的对象中有一个
成员变量，存放所有的需要输出的文本注记信息。这些信息即可能是一个单字符
串信息，也可能是由多个字符串组成的集合，它们将根据不同的子类要求以不同
的形式输出。（字符串即可能是单行的也可能是多行的）
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
/// 表信息类
/*它的父类是CrsltElement。该类是所有输出表单的容器类或者说是管理类，
它其中只有一个存放表单信息的成员变量，而不具有其它功能*/
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
	//表单信息类，存放着所有的以表单形式输出的结果信息
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
	//决策树图形信息类，存放着决策树图形的所有信息
	CDecTreeWnd *m_pDecTreeWnd;


};

//0未定义 ，4 为整数 ， 8 为浮点数， 16 为复数, 1 为字符串
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

/*存放分析中一个独立的以表单形式输出的结果信息，它继承于CWnd，具有自己
独立的输出函数，它的父窗体句柄是输出窗口的句柄。*/
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
	//创建表，nRow为创建的行数，nCol为创建的列数，nTableType为创建的表的数据类型，
	//数据类型分为rInt, rDouble rString为别为整数，浮点，字符串
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
	CTString   m_strTitle;//表单的标题
	CTString   m_strHead;//表单的表头注解
	CTString   m_strTail;//表单的注记
	CTString   m_strException;//表的异常出错等信息

	int       m_nRows;//表单的总行数
	int       m_nCols;//表单的总列数
	
	CTStringArray m_arColumnInfo;//表格列头 类型 TColumnInfo
	CPtrArray m_arRowInfo;//表单的行信息集合,表的数据内容
	CPtrArray m_arDataInfo;//表的数据内容的附加信息
};

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// 图形信息类
/*它的父类是CrsltElement，它同CＲsltElementTable一样，也只是图形类的一个
管理类，它只有一个存放图形信息的成员变量。*/
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































































