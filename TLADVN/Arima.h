// Arima.h: interface for the CArima class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ARIMA_H__7A619F23_CEF1_4028_AB05_EAD8F0458BD0__INCLUDED_)
#define AFX_ARIMA_H__7A619F23_CEF1_4028_AB05_EAD8F0458BD0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "AlgorithmBase.h"

class CArima : public CAlgorithmBase  
{
public:
	CArima();
	virtual ~CArima();
	CResult * OnArima(CDataInterface *pDataInterface, string VO); //外部调用主算法
	CDoubleVector ArimaPred(CDoubleVector &OrgVec, int pAR, int qMA, int dIFF, int PredStep, BOOL m_bWithConst);//对一个原始的向量进行分析、预测


private:
	int Factorial(int number);   //阶乘函数
	double Getcni(int n,int i);   ////排列组合
	BOOL GetValueFromVO(CDataInterface *pDataInterface, string VO);
	void DrawResult();
	CDoubleVector Predict(CDoubleVector &Xmin_Vec, CDoubleVector &Z_Vec, int d, BOOL  m_bWithConst);///求预测转化函数
	CDoubleVector Getpredict(CDoubleVector &Org_Vec, CDoubleVector &CofficVec, int p, int q,int L, BOOL m_bWithConst);//求残差和预测
    CDoubleVector GetInverseFunc(CDoubleVector Vec_1, CDoubleVector Vec_2, int numW); //求逆函数
	void Powell(CDoubleVector &Data_Vec, CDoubleMatrix &Direction_mx, int &iter, double &FRET);  //多元函数包维尔方法
	void Linmin(CDoubleVector &Data_Vec, CDoubleVector &Direction_Vec, double &FRET); //搜索从初始点沿指定方向的极小值点
	void Mnbank(double &ax, double &xx, double &bx, double fa, double fx, double fb, CDoubleVector Data_Vec, CDoubleVector Direction_Vec); //用黄金分割法确定极小点所在的区间
	double Brent(double ax, double xx, double bx, double &Xmin, CDoubleVector Data_Vec, CDoubleVector Direction_Vec); //布伦特一维优化
	double FuncX(CDoubleVector VecX);//求函数值
	void Erase(CDoubleVector &Direction_Vec, CDoubleVector &PTT_Vec, CDoubleVector &Data_Vec); //还原方向重新搜索
	void GetArimaStatis(CDoubleVector diffvec, CDoubleVector ep_Vector, CDoubleVector CofficVec);//////输出统计量
	void GetPreStatis(/*CDoubleVector pre_Vec*/ double stderor/*, double alphavalue*/);  ///输出预测统计量
    BOOL GetARbeta(int numP, int numQ); //自回归系数估计

	//用户输入的参数 始
	TIntStruct m_tVarInfo;						//输入变量下标集
	int m_nRow;                                 //定义变量行数
	int m_pAR;                                  //定义模型自回归阶数
	int m_qMA;                                  //定义模型滑动平均阶数
	int m_dIFF;                                 //定义差分阶数
	int m_nPredStep;                            //定义预测步长
	BOOL m_bWithConst;                          //定义有无常数项
	
	//用户输入的参数 止


	//编程方便设置的变量 始	
	CDataInterface * m_pDataInterface;
	CResult * m_pResult;
	CRsltElementText * m_pWarningTextRslt;//报错信息
	CDoubleVector m_AutoCovF_Vec;   //自协方差函数
	CDoubleVector m_AutoCorrF_Vec;  //自相关函数
	double m_VecRow;
	CDoubleVector m_diff_Vec;  //差分后的序列
	CDoubleVector m_ARCoefficient_Vec;
	CDoubleVector m_ARvariance_Vec;
	CDoubleVector m_Residual_Vec;
    CDoubleVector m_ep_Vector;  //残差序列
	CDoubleVector m_Coef_vec;
    double m_stderror;
	double m_dw;
	double m_Rsquare;
	double m_adjustedRSquare;
	double m_AIC;
	double m_SC;
	double m_loglikeli;
	double m_SSQ;
	CDoubleVector m_stderror_Vec;
	CDoubleVector m_tvalue_Vec;
	CDoubleVector m_pvalue_Vec;
	CDoubleVector m_predict_Vec;
	CDoubleVector m_orginal_Vec;

	//编程方便设置的变量 止

};

#endif // !defined(AFX_ARIMA_H__7A619F23_CEF1_4028_AB05_EAD8F0458BD0__INCLUDED_)
