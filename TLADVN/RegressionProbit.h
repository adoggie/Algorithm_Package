// RegressionProbit.h: interface for the CRegressionProbit class.
//				     概率单位回归
//                    标注： 余卫军
//					时间： 2005年10月20日
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGRESSIONPROBIT_H__D238F140_ED2C_4B89_9E93_92946A1A51F5__INCLUDED_)
#define AFX_REGRESSIONPROBIT_H__D238F140_ED2C_4B89_9E93_92946A1A51F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmBase.h"

class AFX_EXT_CLASS CRegressionProbit : public CAlgorithmBase  
{
public:
	CRegressionProbit();
	virtual ~CRegressionProbit();

	CTLTable * CreateWarningTable();
	CResult * OnProbit(CDataInterface * pDataInterface, string VO);
	double * GetData( CDataInterface *pDataInterface, double*& pFactorData, int& nCaseNum );

	int m_nFactor;//取分层变量的数据
	int m_nResponse;//反应变量下标
	int m_nObserve; //观测总数变量下标
	int  m_nDependIndex;//

private:
	bool GetValueFromVO(CDataInterface * pDataInterface, string VO);
	CIntVector SortVector(CIntVector vect);
	int GrpToIdx(CIntVector vect,CIntVector &vect1);
	int QuickFind( CTArray<int, int>& arSort, int nData, int iBegin, int iEnd );

	CIntMatrix GetYaMatirx(CIntVector vect,int num);
	CIntMatrix Zhankai(CIntMatrix x, int& nCategoryNum);
	bool Probit(CDoubleMatrix matrix,int nNRR,int nMaxIterator,double C,bool bFactor,CIntVector vector_factor = NULL);
	void CalcBeta(CDoubleMatrix matrix_X,CDoubleMatrix matrix_NR,CDoubleVector vector_preBeta,int nNRR,int nMaxIterator,double StepLimit,double Ep,int C,CDoubleVector &vector_lastBeta,CDoubleMatrix &matrix_Hessen,int &cycleNum,bool &bSingularity);
	CDoubleMatrix HessenMat(CDoubleMatrix X,CDoubleMatrix NR,CDoubleVector tVec,int NRR,double C,CDoubleMatrix &g);
	void StatisForProbit(CDoubleMatrix Hessen,CDoubleVector lastBetaVector,CDoubleMatrix NR,int NRR,CDoubleVector P,int row,int col,int nNumFactor,CIntVector vect);
	CDoubleMatrix Hessen(CDoubleMatrix X,CDoubleMatrix NR,CDoubleVector tVec,double C);
	double * GetDataForProbit(CDataInterface * pDataInterface);

	void InitParma();

	CTString m_strErrorInfo;
	CTString m_strMissField;
	float	m_fC;
	int		m_nIterator;
	int		m_nNRR;
	bool m_bStopCalculate;

	CTLTable* briefrTable;
	CTString strDependName;

};

#endif // !defined(AFX_REGRESSIONPROBIT_H__D238F140_ED2C_4B89_9E93_92946A1A51F5__INCLUDED_)
