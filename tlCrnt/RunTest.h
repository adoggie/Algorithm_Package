// RunTest.h: interface for the CRunTest class.
//         游程检验
//      标注： 余卫军 2005年10月20日
//			修改：马良庄	2006/03/22
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RUNTEST_H__0DBB707A_A7A8_4075_957E_0D4ED89AD492__INCLUDED_)
#define AFX_RUNTEST_H__0DBB707A_A7A8_4075_957E_0D4ED89AD492__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class AFX_EXT_CLASS CRunTest  
{
public:
	TIntStruct m_tVarInfo;
	CRunTest();
	virtual ~CRunTest();
	CResult* OnRunTest(CDataInterface *pDataInterface, string VO);


protected:

	BOOL GetValueFromVo(CDataInterface *pDataInterface, string VO);
	int * CRunTest::Binarilize(CTArray<double,double> &DataArray,
									CTArray<double,double> &WeightArray,
									int nBinaryMethod,double &dSpecified);
	BOOL CRunTest::RunTestMain(CTArray<double,double> &DataArray,
							   CTArray<double,double> &WeightArray,
							   int nBinaryMethod,double dSpecified);
	void CRunTest::CreateResult();
	void CRunTest::FreeVarialbe();

	CDataInterface * m_pDataInterface;
//	CWnd           * m_pWnd;
	CResult		   * m_pResult;
	CRsltElementText *m_pRsltElementText;
	int m_nFieldIndex;
	CTStringArray m_NameStrArray,m_GroupStrArray,m_FrequencyStrArray;
	CTArray<int,int> m_RowArray,m_RunArray;
	CTArray<double,double> m_ZArray,m_PArray;
	BOOL m_bWeight;
	int m_nWeightCol;

	//以下成员变量用来传递和保存界面设置信息
public:
	double m_dSpecify;
	int m_nBttn;

};

#endif // !defined(AFX_RUNTEST_H__0DBB707A_A7A8_4075_957E_0D4ED89AD492__INCLUDED_)
