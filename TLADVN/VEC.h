// VEC.h: interface for the CVEC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VEC_H__F4404C5D_A336_4042_A970_3077BAA4335A__INCLUDED_)
#define AFX_VEC_H__F4404C5D_A336_4042_A970_3077BAA4335A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmBase.h"

class CVEC : public CAlgorithmBase  
{
public:
	CVEC();
	virtual ~CVEC();
public:
	CResult* OnRun(CDataInterface *pDataInterface, string VO);
	TIntStruct		m_tVarInfo;			//校验变量struct
	TIntStruct		m_tVarInfo_D;		//外生变量struct	
private:
	BOOL GetValueFromVo(CDataInterface * pDataInterface, string VO);
	void DrawResult();
	void GetMatrixData(CDoubleMatrix &Matrix_X, CDoubleMatrix &Matrix_Y);//从界面读数据		
	
	CTStringArray	m_strDepend;        //校验变量名称字符串
	CTStringArray	m_strVarSel;        //外生变量名称字符串
	
	struct VI							//定义输入变量结构
	{
		CDoubleMatrix m_mxX;			//输入回归变量矩阵
		CDoubleMatrix m_mxY;			//外生变量
		int m_nLagnum;					//序列滞后阶数
		int	m_nRegre;					//回归变量个数
		int	m_nExto;					//外生变量个数
		int m_nModel;					//选择模型
		int m_nEquNum;					//选择模型中包含的协整方程的个数 
	} m_tIn;
	struct VO 
	{
		CDoubleVector m_vrEigValue;		//特征值

		CDoubleMatrix m_mxCoff;			//经过Johansen归一化的特征向量矩阵
		double m_nIndex;				//得到的协整关系的个数
		CDoubleMatrix m_mxCoff1;		//每种协整模式的系数的综合矩阵
		CDoubleMatrix m_mxBeta;				//回归系数矩阵
		CDoubleMatrix m_mxSEReg;			//回归系数的标准误
		CDoubleMatrix m_mxTvalue;			//回归系数的T值
		CDoubleMatrix m_mxPvalue;			//回归系数的P值
		
		double m_dDRC;						//模型的决定残差协方差
		double m_dAdjDRC;					//模型的修正的决定残差协方差
		double m_dLogl;					    //模型的对数似然
		double m_dAIC;						//模型的AIC准则
		double m_dSC;						//模型的SC准则
		double m_dSBC;						//模型的SBC准则
		double m_dFPE;						//模型的FPE准则
		CDoubleVector m_vrSSE;				//各个回归方程的残差平方和
		CDoubleVector m_vrSST;				//各个回归方程的回归平方和
		CDoubleVector m_vrRsqu;				//各个回归方程的复相关系数
		CDoubleVector m_vrAdjR;				//各个回归方程的修正复相关系数
		CDoubleVector m_vrSSR;				//各个回归方程的回归平方和
		CDoubleVector m_vrSER;				//各个回归方程的回归标准误
		CDoubleVector m_vrDw;				//各个回归方程的DW统计量
		CDoubleVector m_vrMD;				//各个回归方程的因变量标准差
		CDoubleVector m_vrSDD;				//各个回归方程的因变量均值
		CDoubleVector m_vrFtest;			//各个回归方程的F统计量
		CDoubleVector m_vrDRC;				//各个回归方程的DRC
		CDoubleVector m_vrLogL;				//各个回归方程的对数似然比
		CDoubleVector m_vrAIC;				//各个回归方程的AIC准则
		CDoubleVector m_vrSC;				//各个回归方程的SC准则
	} m_tOut;
	bool VECest();					//协整检验主函数，采用Johansen方法
	BOOL	Paratest(CDoubleMatrix mxBeta,	//要进行参数检验的回归系数
						CDoubleMatrix mxRes,	//残差矩阵
						CDoubleMatrix mxYnew,	//回归因变量
						CDoubleMatrix mxXnew,	//回归自变量
						int VARindex);
	CDoubleMatrix JJtest(CDoubleMatrix mxX,		//进行JJ算法的待检测变量
						CDoubleMatrix mxY,		//进行检验时的外生变量
						int dChoice,			//给出选择的模型
						int lag,				//给出滞后阶数
						CDoubleMatrix &mxXnew);	//计算给定变量的新的自变量列表

};

#endif // !defined(AFX_VEC_H__F4404C5D_A336_4042_A970_3077BAA4335A__INCLUDED_)
