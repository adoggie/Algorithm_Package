// Var.h: interface for the CVar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VAR_H__F135339C_1A30_4F26_A057_49D245F880DC__INCLUDED_)
#define AFX_VAR_H__F135339C_1A30_4F26_A057_49D245F880DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "AlgorithmBase.h"
class CVar   : public CAlgorithmBase 
{
public:
	CVar();
	virtual ~CVar();
	CResult * OnVAR(CDataInterface *pDataInterface, string VO);//外部调用主要程序
	TIntStruct		m_tVarInfo;//回归变量struct
	TIntStruct		m_tVarInfo_D;//外生变量struct	
private:
	
	//////////////////////////////////////
	/////		输入参数列表		//////
	//////////////////////////////////////

	int m_nLagnum;                            //从界面获得滞后阶数
	int m_nForcast;						      //从界面获得是否进行一步预测的信息 
	bool m_bXWithconst;				          //从界面获得模型是否含有常数项
	CDoubleMatrix m_mxX;				      //从界面获得回归变量矩阵
	CDoubleMatrix m_mxExto;					  //从界面获得外生变量矩阵


	//////////////////////////////////////
	////		调用输入输出函数		//
	//////////////////////////////////////
	int				m_nRegre;           //回归变量个数
	int				m_nExto;            //外生变量个数
	CTStringArray	m_strDepend;        //回归变量名称字符串
	CTStringArray	m_strVarSel;        //外生变量名称字符串
	bool m_bChartScatterPlot;			//是否输出散点图
	bool m_bChartLine;                  //是否输出拟合图
	
	//////////////////////////////////////
	///////       输出结果列表   /////////
	//////////////////////////////////////

	//////////////////////////////////////////模型系数表
	CDoubleMatrix m_mxestB;					//模型系数
	CDoubleMatrix m_mxparattest;			//模型系数t检验值
	CDoubleMatrix m_mxparastde;				//模型系数标准误

	//////////////////////////////////////////分组的统计量
	CDoubleVector m_vrRsqu;					//模型的R^2
	CDoubleVector m_vrAdjRsqu;				//模型的调整的R^2
	CDoubleVector m_vrSSE;					//模型分组的SSE
	CDoubleVector m_vrSST;					//模型分组的SST
	CDoubleVector m_vrSER;					//模型分组的回归标准误
	CDoubleVector m_vrFST;					//模型分组的F统计量
	CDoubleVector m_vrLogLG;				//模型分组的对数似然值
	CDoubleVector m_vrAICG;					//模型分组的AIC准则
	CDoubleVector m_vrSCG;					//模型分组的SC准则
	CDoubleVector m_vrSDD;                  //模型分组的标准误
	CDoubleVector m_vrMD;					//模型分组的均值
	CDoubleMatrix m_mxResidul;				//模型的残差矩阵
	double m_dDRC;							//模型的决定残差协方差
	double m_dAdjDRC;						//模型的修正的决定残差协方差
	double m_bLogl;					    	//模型的对数似然
	double m_bAIC;							//模型的AIC准则
	double m_bSC;							//模型的SC准则
	double m_bSBC;							//模型的SBC准则
	double m_bFPE;							//模型的FPE准则
	CDoubleMatrix m_mxYhat;					//模型的预测值，用来作散点图

	//////////////////////////////////////预测
	CDoubleMatrix m_mxForcast;				//预测值

private:
	bool GetValueFromVo(CDataInterface * pDataInterface, string VO);
	void DrawResult();					//输出结果展示					
	bool Varest(CDoubleMatrix& m_mxX,bool m_bXWithconst,CDoubleMatrix& m_mxExto,int lagnum,int m_nForcast);//主进程函数，回归变量、外生变量、滞后阶数、是否预测
	void GetMatrixData(CDoubleMatrix& Matrix_X, CDoubleMatrix& Matrix_y);//读数，返回matrix_x[回归变量]，matrix_y[外生变量]
};

#endif // !defined(AFX_VAR_H__F135339C_1A30_4F26_A057_49D245F880DC__INCLUDED_)



















































































