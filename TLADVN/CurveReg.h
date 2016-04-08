// CurveReg.h: interface for the CCurveReg class.
//                   曲线回归分析 
//                   功能：各种协方差分析的共同方法和属性
//					 标注：  余卫军																																																																																																																																																																																					
//					 时间：2005年10月20日
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CURVEREG_H__51B7B4AF_1459_43A4_AC51_DFE598250FC5__INCLUDED_)
#define AFX_CURVEREG_H__51B7B4AF_1459_43A4_AC51_DFE598250FC5__INCLUDED_
#include "AlgorithmBase.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class AFX_EXT_CLASS CCurveReg : public CAlgorithmBase  
{
public:
	int m_nDependIndex;//自变量下标
	CResult * OnCurveReg(CDataInterface *pDataInterface, string VO);//脚本调用
	CCurveReg();
	virtual ~CCurveReg();
	TIntStruct m_tVarInfo;
private:
	CResult * m_pResult;
	CDataInterface * m_pDataInterface;
private:
	CRsltElementText * m_pResultVariable;
	int m_nCurVarIndex;
	CTString m_strModel;
	CTString m_strHead;
	double m_u;//logistics
	CIntVector m_VectAvail;
	
public:
	CTString m_strUpper;  //zfm

	CTLTable* briefrTable;
	CTString strModel;
private:
	//	陈思多，01/13/03
	bool m_bIsWeighted;//是否对数据进行了加权
	double* m_pdWeights;

	void ReportError();
	BOOL GetValueFromVo(CDataInterface *pDataInterface, string VO);
	void CreateResult(CTString strHead,double r,double rs,double ars,double se,int df1,int df2,double ss1,double ss2,
							 double ms1,double ms2,double b1,double b0,double sb1,double sb0,double sbeta,double t1,
							 double t0 ,double F,bool bLogistic=false); //zfm 添加bool字段判是不是在输逻辑斯蒂克回归
	void CreateResult(CTString m_strHead, double R, double R_S, double A_R, double SER, int DF_Rg,double DF_Rs,
		double U, double Q, double M_S_Rg, double M_S_Rs, double F, 
		CDoubleMatrix BetaMatrix, CDoubleVector serrbeta, CDoubleVector Beta, CDoubleVector T); //zfm


//2003.09.23 zkc
	bool m_flag;
	CTStringArray m_Legend;
	CPtrArray m_dataPtr;

	void FillChartData(int nLen,double pDataX[],double pDataY[],int nMethod,CTString strLegend,
					   double b0,double b1,double b2=0,double b3=0);
	void FillChartData2(int nLen,CDoubleVector Vect_X,CDoubleVector Vect_Y,int nMethod,CTString strLegend,
		double b0,double b1,double b2=0,double b3=0);
		
	void CreateChartResult(double pDataX[],CTString strField,CResult* pResult,int nLen);
	void sort(double x[],double y[],double w[],int nLen);


//	01/13/02，陈思多．考虑增加权重，增加带权重参数的重载函数

	void Quad_Cubic(int n, int p, double *x, double *y,double *w);//加权重 double * MissingValue)
	double* GetDataSingle(CDataInterface* pDataInterface, int nIndex, double * MissingValue);
	double* GetDataSingle(CDataInterface* pDataInterface, int nIndex, CIntVector& Vect_Avail);
	void Linear(int n, double x[], double y[], double w[], double a[]);
	void Exponential(int n, double x[], double y[], double t, double w[], double a[]);
	void Power(int n, double x[], double y[], double w[], double a[]);
	void Compound(int n, double x[], double y[], double w[], double a[]);
	void Growth(int n, double x[], double y[], double w[], double a[]);
	void S(int n, double x[], double y[], double w[], double a[]);
	void Inverse(int n, double x[], double y[], double w[], double a[]);
	void Logistic(int n, double x[], double y[], double w[], double a[], double &u);
	void Logarithmic(int n, double x[], double y[], double w[], double a[]);

	TIntStruct m_IndependIndex;//因变量下标
	double m_dUpper;//logistic 上界
	bool	m_bCompound;//复合增长曲线 
	bool	m_bExponential;//指数曲线
	bool	m_bLogistic;//逻辑斯蒂克曲线
	bool	m_bGrowth;//生长曲线
	bool	m_bInverse;//逆曲线
	bool	m_bLineare;  //线性回归
	bool	m_bLogarithmic;//对数曲线
	bool	m_bPower;   //幂率曲线
	bool	m_bS;//S曲线
	bool    m_bQuad;//二次曲线
	bool    m_bCubic;//三次曲线
	
};

#endif // !defined(AFX_CURVEREG_H__51B7B4AF_1459_43A4_AC51_DFE598250FC5__INCLUDED_)
