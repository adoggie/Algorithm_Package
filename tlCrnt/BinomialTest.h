// BinomialTest.h: interface for the CBinomialTest class.
//                  二项分布检验
//				标注：余卫军 2005年10月21日
//			修改：马良庄	2006/03/22
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BINOMIALTEST_H__64763B9B_A0DC_49E2_BD05_5FC5667FEFEB__INCLUDED_)
#define AFX_BINOMIALTEST_H__64763B9B_A0DC_49E2_BD05_5FC5667FEFEB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "tlcrnt.h"
class AFX_EXT_CLASS CBinomialTest
{
public:
	CBinomialTest();
	~CBinomialTest();
	TIntStruct m_tVarInfo;
	CResult* OnBinomialTest(CDataInterface *pDataInterface, string VO);
	
protected:
	BOOL GetValueFromVo(CDataInterface *pDataInterface, string VO);
	int * CBinomialTest::Binarilize(CTArray<double,double> &DataArray,
									CTArray<double,double> &WeightArray,
									int nBinaryMethod,double &dSpecified);
	double Cnk(int n,int k);
	void CBinomialTest::FreeVariable();

	BOOL CBinomialTest::BinomialTestMain(CTArray<double,double> &DataArray,
										 CTArray<double,double> &WeightArray,
										 double p,int nBinaryMethod,
										 double dSpecified);
										 
	void CreateResult();

protected:
	CDataInterface * m_pDataInterface;
	//CWnd           * m_pWnd;
	CResult		   * m_pResult;
	CRsltElementText *m_pRsltElementText;
	int m_nFieldIndex;
	BOOL m_bWeight;
	int m_nWeightCol;
	CTArray<int,int> m_RowArray;
	CTArray<double,double> m_TestPropArray,m_SigArray,m_minPropArray;
	CTStringArray m_NameStrArray,m_GroupStrArray,m_CategoryStrArray,m_FrequencyStrArray,m_ObservedStrArray;

	//以下成员变量用来传递和保存用户界面设置信息
public:
	double m_dSpecify;
	int m_nBttn;
	double m_dProportion;


};

#endif // !defined(AFX_BINOMIALTEST_H__64763B9B_A0DC_49E2_BD05_5FC5667FEFEB__INCLUDED_)
