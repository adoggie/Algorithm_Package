// TimeSeries.h: interface for the CTimeSeries class.
//					时间序列分析：
 //        包括 AR、ARIMA联立方程
//			    标注： 余卫军
//			2005年10月20日

//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMESERIES_H__34C7ED15_F3B5_4693_88CF_C5349CEE52F6__INCLUDED_)
#define AFX_TIMESERIES_H__34C7ED15_F3B5_4693_88CF_C5349CEE52F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmBase.h"

class  AFX_EXT_CLASS CTimeSeries : public CAlgorithmBase  
{
public:
	CResult * OnTimeSeries(CDataInterface *pDataInterface, int nModel);//没看出来有什么用处
	CResult * OnTimeSeriesAR(CDataInterface *pDataInterface, string VO);
	CResult * OnTimeSeriesMA(CDataInterface *pDataInterface, string VO);
	CResult * OnTimeSeriesARIMA(CDataInterface *pDataInterface, string VO);
	TIntStruct m_tVarInfo,m_tVarInfoAR,m_tVarInfoMA;
	CTimeSeries();
	virtual ~CTimeSeries();
   
	

protected:
	CDataInterface * m_pDataInterface;
	CResult		   * m_pResult;
	CRsltElementText *m_pRsltElementText;
//    CRsltElementTable*m_pRsltVariable;

private:

	CDoubleVector m_eVector;
	int		m_nCurVarNum;
	bool m_bIsToEnd;//出现异常情况不能算。
//	bool CTimeSeries::Non11(CDoubleMatrix matrix_X,int T3orT12,int nMorA);

//以下成员变量用来保存用户界面对话框的设置信息
public:

	CTLTable* briefrTable;

	///ARIMA model
	bool	m_bConstant;
	int		m_nParam;
	int		m_nSquareSum;
	int		m_nAutoReg;
	int		m_nDiff;
	int		m_nMaxIteration;
	int		m_nMoveAv;
	int		m_nForcast;
	bool	m_bResidue;
	////AR model
	bool	m_bConstantAR;
	int		m_nParamAR;
	int		m_nSquareSumAR;
	int		m_nAutoRegAR;
	int		m_nDiffAR;
	int		m_nMaxIterationAR;
	int		m_nMoveAvAR;
	int		m_nForcastAR;
	bool	m_bResidueAR;
	////MA model
	bool	m_bConstantMA;
	int		m_nParamMA;
	int		m_nSquareSumMA;
	int		m_nAutoRegMA;
	int		m_nDiffMA;
	int		m_nMaxIterationMA;
	int		m_nMoveAvMA;
	int		m_nForcastMA;
	bool	m_bResidueMA;
	
	int		m_nModel;//1 for AR model
					 //2 for MA model
					 //3 for ARIMA model


	CDoubleVector GetSingleColData(int index,bool &bIsToEnd);
	bool GetValueFromVOAR(CDataInterface * pDataInterface, string VO);
	bool GetValueFromVOMA(CDataInterface * pDataInterface, string VO);
	bool GetValueFromVOARIMA(CDataInterface * pDataInterface, string VO);


protected:
	bool NormalTest(CResult* pResult,CDoubleVector DataArray);

	void ac_pac(CDoubleVector Z_Vector, int K,
							 CDoubleVector &gamma_Vector,
							 CDoubleVector &rho_Vector);
	int delta(int j,int k);
	CDoubleVector GetARBeta(CDoubleVector Z_Vector,
									   CDoubleVector preBeta_Vector,
									   int p,int cons,double pEps,
									   double sEps,int MaxIteration,			   
									   CDoubleMatrix &covErr_Matrix);
	CDoubleVector GetARBeta2(CDoubleVector Z_Vector,
		CDoubleVector preBeta_Vector,
		int p,int cons,double pEps,
		double sEps,int MaxIteration,			   
		CDoubleMatrix &covErr_Matrix);
	
	void GetArimaBeta2(CDoubleVector Z_Vector,
								  CDoubleVector preBeta_Vector,
								  int p,int q,int cons,
								  double pEps,double sEps,int MaxIteration,
								  double &Q,double &ResVar,double &StdE,int &df,double &AIC,
								  CDoubleVector &beta_Vector,
								  CDoubleMatrix &covErr_Matrix);
	void GetArimaBeta(CDoubleVector Z_Vector,
		CDoubleVector preBeta_Vector,
		int p,int q,int cons,
		double pEps,double sEps,int MaxIteration,
		double &Q,double &ResVar,double &StdE,int &df,double &AIC,
		CDoubleVector &beta_Vector,
		CDoubleMatrix &covErr_Matrix);
	
	CDoubleMatrix GetArimaStatis(CDoubleVector beta_Vector,int df,int cons,
												 CDoubleMatrix covErr_Matrix,
												 CDoubleVector &stdError_Vector,
												 CDoubleVector &t_Vector,CDoubleVector &P_Vector);
	CDoubleVector GetARMAPreBeta(CDoubleVector X_Vector,int p,int d,int q,
											  int cons,int MaxIteration,int &NumCir,double &mu);
	int sigmaPhi(int p,int q,CDoubleVector rho_Vector,
							 CDoubleVector W_Vector,int kappa,
							 int MaxIteration,
							 CDoubleVector &PHIEst_Vector,
							 CDoubleVector &theta_Vector);
	CDoubleVector GetArmaPredict(CDoubleVector XOrg_Vector,
											 CDoubleVector Beta_Vector,
											 int p,int q,int L,int cons);
	void GetARStatis(CDoubleVector X_Vector,
								 CDoubleVector beta_Vector,int cons,
								 CDoubleMatrix covErr_Matrix,
								 double &SSQ,double &ResVar,double &AIC,
								 CDoubleMatrix &S_Matrix,
								 CDoubleMatrix &RR_Matrix,
								 CDoubleVector &stdError_Vector,
								 CDoubleVector &t_Vector,CDoubleVector &P_Vector,double &StdE,int &df);
	CDoubleVector GetAutoregression(CDoubleVector XOrg_Vector,int p,int Cons);
	CDoubleVector GetAutoregression2(CDoubleVector XOrg_Vector,int p,int cons);
	double GetCni(int n,int i);
	CDoubleVector GetDiffPredict(CDoubleVector X_Vector,CDoubleVector Z_Vector,int d);
	CDoubleMatrix GetFF(CDoubleVector w_Vector,
									CDoubleVector I_Vector,
									CDoubleMatrix II_Matrix,int p,int q,
									int Num,int cons);
	CDoubleVector GetI(CDoubleVector w_Vector,
								   CDoubleVector beta_Vector,int p,int q,int Num,
								   CDoubleVector &I_Vector);
	CDoubleMatrix GetII(CDoubleVector I_Vector,CDoubleVector beta_Vector,
									int p,int q,int Num);
	bool Arima(CDoubleVector X_Vector,int p,int q,int d,int cons,
						   int L,double pEps,double sEps,int MaxIteration,
						   int MaxIter);
	CDoubleVector GetPhi(CDoubleVector beta_Vector,int p,int d);
	CDoubleVector GetPsi(CDoubleVector beta_Vector,int p,int d,int q,int L,int cons);
	CDoubleVector GetGGValue(CDoubleVector beta_Vector,int p,int q,int L,int cons);
	void GetBelieveDegree(CDoubleVector Z_Vector,double ResVar,
						  CDoubleVector beta_Vector,int p,int d,
						  int q,int cons,
						  CDoubleVector &pStdE_Vector,
						  CDoubleVector &lBound_Vector,
						  CDoubleVector &uBound_Vector);
	void CreateResult(double SSQ,double ResVar,double StdE,int df,double AIC,
								   CDoubleVector beta_Vector,CDoubleVector StdError_Vector,
								   CDoubleVector t_Vector,CDoubleVector P_Vector,
								   CDoubleMatrix covErr_Matrix,CDoubleMatrix RR_Matrix,
								   CDoubleVector Z_Vector,CDoubleVector pStdE_Vector,
								   CDoubleVector lBound_Vector,CDoubleVector uBound_Vector,
								   int nForcast,int nAutoReg,CDoubleVector X_Vector);

	bool IsExisted(CDataInterface *p,CTString str);

};



#endif // !defined(AFX_TIMESERIES_H__34C7ED15_F3B5_4693_88CF_C5349CEE52F6__INCLUDED_)





















