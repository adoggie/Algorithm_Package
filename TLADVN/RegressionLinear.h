// RegressionLinear.h: interface for the CRegressionLinear class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REGRESSIONLINEAR_H__5DFA1AFB_8267_4338_BB7E_C8F300EE0C59__INCLUDED_)
#define AFX_REGRESSIONLINEAR_H__5DFA1AFB_8267_4338_BB7E_C8F300EE0C59__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmBase.h"

class CScript;

class CRegressionLinear : public CAlgorithmBase  
{
public:
	CRegressionLinear();
	virtual ~CRegressionLinear();
	CResult * OnStepWise(CDataInterface * pDataInterface, string VO);
	CResult * OnRegPred(CDataInterface * pDataInterface, string VO);

private:
	string m_strModel;//字符串排列顺序：const_n个变量_系数_变量名_(xx)_因变量_MSE
	bool GetValueFromVo(CDataInterface * pDataInterface, string VO);
	bool GetPredValueVo(CDataInterface * pDataInterface, string VO);
	bool GetValueFromModel(CDataInterface* pDataInterface, string Model);//解读模型
	bool Export();//输出模型
	void DrawTable(int XCol, int WithConst);
	void DrawTableBody(int XCol, int WithConst, int &number, CTString strWarning);
	void DrawTableHead(int XCol, int WithConst);
	void InitParma();//赋初值
	void  GetData(CDataInterface *pDataInterface, CDoubleMatrix& Matrix_Data);//读数
	CDoubleMatrix EigValPlusVec(CDoubleMatrix* m_EigVec, bool nIndex);//求特征向量、特征根
	double GetQValue(CDoubleMatrix matrix,CDoubleMatrix matrix_Y);
	CDoubleMatrix DelMatrix(CDoubleMatrix matrix_comein, int mm);//删除矩阵的第mm行mm列
	CDoubleVector DelVector(CDoubleVector vector_comein, int mm);//删除向量的第mm列
	void RegLinear(CDoubleMatrix XX, CDoubleMatrix Xy, CDoubleMatrix CorrMatrix,double sum_weight, int XCol, int XRow, bool withconst);//算值
	CDoubleVector mmthSelfReg(CDoubleMatrix matrix_comein, CDoubleVector Xmean, bool WithConst);//求Tolerance
	CDoubleVector SelectVector(CDoubleVector SingVector,CDoubleVector SVector, int tot);//同下
	CDoubleMatrix SelectMatrix(CDoubleVector SingVector,CDoubleMatrix SMatrix, int tot);
	//选出SingVector中大于0的对应id，对应的Smatrix中的项。tot是选出几个。
	void SelectStrAry(CDoubleVector SingVector,CTStringArray &m_strX_nameAry, CTStringArray &arrayTemp);
	//同上，选出的Array放在m_X_temp_nameAry里边
	void DrawResult(int XCol, int WithConst);//基本结果输出结果窗口
	void DrawCorrolation(CDoubleMatrix Xy_Sigma);
	void ResiPlot(CDoubleMatrix data_read_resi, bool WithConst, CDoubleVector coefficiency, CDoubleMatrix XtimesX, double MSE);//输出需要残差的图形等。
	bool IsExisted(CDataInterface *p,CTString str);//判断变量是否重名
	void Collinearity(CDoubleMatrix m_XtimesX, int XCol, bool WithConst);//计算共线性中的特征根和条件比
		
	////////////
	CTLTable* briefrTable;
	////////////////////////////
	int m_nColl;
	int m_nLine;
	int  m_nDependIndex;
	bool m_bResidue;
	bool m_bPredict;
	int m_nWeightID;//权重的变量ID
	bool m_bOutput;//是否输出结果模型
	bool m_bSaveModel;//是否保持模型
	CTString m_ModelFile;//保存模型的名字、路径
	CDoubleMatrix m_X_inv;//用于输出结果，确定上下界的。
	int m_nMissMethod;//缺失值处理方法
	int m_nMethod;//变量选择方法，0为全部选，1为向后，2为向前，3为逐步
	bool m_bStopCalculate;
	bool m_bWithWeight;//是否有权重
	double sum_weight;//权重总和
	bool m_bCorr; //是否输出相关矩阵
	double m_fEntry;
	double m_fRemoval;
	CTString m_strErrorInfo;
	CTStringArray m_strX_nameAry;//自变量名
	CTStringArray m_strX_org;
	CTStringArray m_X_temp_nameAry;//在向后、逐步回归中，自变量名的排列会发生变化时，用他来表示。
	CTString m_stry_name;//因变量名。
	CDoubleVector Xmean;
	CDoubleVector m_coefficiency;//方程系数。
	CDoubleVector m_Tolerance;
	double m_df_residual;//自由度
	CDoubleVector m_vector_standbeta;//标准化后的X得到的beta
	CDoubleVector m_vector_T;//t值
	CDoubleVector m_vector_Sig;//置信度
	double m_R_square;//r方
	double m_R_adjust;//r方调整
	CDoubleVector m_vector_sbeta;//系数的方差
	CDoubleMatrix m_CorrMatrix;//相关系数
	bool m_bWithColl;//是否输出共线性诊断信息
	CDoubleVector m_useforcoll_e;
	CDoubleVector m_vector_condindex;
	double m_var_y;
	double m_ytimesy;
	double m_SSE;
	double m_SST;
	double m_SSR;
	double m_MSE;
	int m_nXRow;//有效数据行数
	int m_nXCol;//有多少个自变量
	CDoubleMatrix m_EigValue;//特征根向量
	CDoubleMatrix m_XtimesX;//X×X，有常数
	CDoubleMatrix m_Xtimesy;//X×y，有常数
	CDoubleMatrix m_XyTimesXy;//所有自变量、因变量的乘积矩阵
	//后边是需要修改的
	double m_dConfidence;//置信度
	bool m_bCoefInter;//是否输出β的置信区间
	CDoubleVector m_CoefUpper;//beta的置信上届
	CDoubleVector m_CoefLower;//beta的置信下届
	double  m_dTest;
	bool    m_bChartLine; //拟合图
	bool    m_bChartScatterPlot; // 残差散点图
	bool    m_bChartScatterPlot2; // 残差正态概率图
	bool    m_bWithConst;          //是否含有常数项
	bool    m_bDrawTableHead;
	CTLTable *pTable1;
	CTLTable *pTable2;
	CTLTable *pTable3;
	CTLTable *pTable4;
	CTLTable *pTable6;
	TIntStruct m_tVarPred;//预测比例的下标，按参数的顺序排列的。
	CTString m_strDepPred;//输出的预测变量的名字
	int m_nVar;//预测模型多少个变量
	bool m_bPredRange;//是否输出预测值的置信区间
	CTStringArray m_strArr_FinalXname;//最后有那些变量名
};

#endif // !defined(AFX_REGRESSIONLINEAR_H__5DFA1AFB_8267_4338_BB7E_C8F300EE0C59__INCLUDED_)
