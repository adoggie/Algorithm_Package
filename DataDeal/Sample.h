// Sample.h: interface for the CSample class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SAMPLE_H__F79DC4C4_9C9B_4C56_824B_A304F51C1FC2__INCLUDED_)
#define AFX_SAMPLE_H__F79DC4C4_9C9B_4C56_824B_A304F51C1FC2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../DataInterface/DataInterface.h"

const char SAMPLE[]    = "SAMPLE";
const char FETCH[]     = "/FETCH";
const char ALL[]       = "ALL";
const char BEF[]       = "BEF";
const char SEP[]       = "SEP";
const char LAYER[]     = "LAYER";
const char LAYER_[]    = "/LAYER";
const char PERCENT[]   = "PERCENT";
const char NUMBER[]    = "NUMBER";
const char VARIN[]     = "/VARINPUT";
const char LAYVARIN[]  = "/LAYERVARINPUT";

//抽样方法
const int   A = 1; //所有中的百分数
const int   B = 2; //前多少记录
const int   S = 4; //每隔多少取几个
const int   L = 8; //按层取（百分数/个数）

class CDataInterface;
class CField;

class CSample  
{
public:
	CSample();
	virtual ~CSample();
public:
	bool Run(CDataInterface *pData, const string &VO);
protected:
	bool Parse(const string &VO);
	bool Sample_A(CDataAccess *pDataIn, CDataAccess *&pDataRet, double fPercent);
	bool Sample_B(CDataAccess *pDataIn, int nStartNo);
	bool Sample_S(CDataAccess *pDataIn, CDataAccess *&pDataRet, double fPercent);
	bool Sample_L(CDataAccess *pDataIn, CDataAccess *&pDataRet, double dPercent);
private:
	bool Verify();
	int CompareFieldValue(CField *pFieldA, CField *pFieldB);
	int IsDiff(CDataAccess *pDAA, CTArray<int,int>arrIndexA, CDataAccess *pDAB, CTArray<int,int>arrIndexB);
private:
	bool           m_bPercent; //分层时是否为取百分数
	int            m_nType;    //抽样类型
	float          m_fValue;   //存放值－各种可能
	TIntStruct     m_tVar;     //层抽取时，变量
	TIntStruct     m_tLayerVar;//层抽取时，层变量
	CDataInterface *m_pData;
};

#endif // !defined(AFX_SAMPLE_H__F79DC4C4_9C9B_4C56_824B_A304F51C1FC2__INCLUDED_)
