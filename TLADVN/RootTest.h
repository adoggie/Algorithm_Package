// RootTest.h: interface for the CRootTest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROOTTEST_H__B320738C_3229_4C2D_AFD4_BB0D238FD57A__INCLUDED_)
#define AFX_ROOTTEST_H__B320738C_3229_4C2D_AFD4_BB0D238FD57A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmBase.h"

class CRootTest : public CAlgorithmBase  
{
public:
	CRootTest();
	virtual ~CRootTest();
	CResult * OnRun(CDataInterface *pDataInterface, string VO);//外部调用主要程序
	TIntStruct		m_tVarInfo;				//回归变量struct
	CTStringArray m_strVarSel;				//变量名
	int m_nRegre;							//回归变量个数
	int m_nTestnum;							//检验方程
private:
	struct VI								//定义输入变量结构
	{
		CDoubleMatrix m_mxX;				//输入回归变量矩阵
		int m_nDifnum;						//序列差分阶数
		int m_nLagnum;						//序列滞后阶数
		int m_nTruncat;						//序列窗宽
		int m_nModel;						//选择模型
	} m_tIn;
	
	struct  VO								//定义输出变量结构
	{
		CDoubleVector m_vrTest;				//检验统计量
		double  m_dTdf;						//检验的临界值
		
		CDoubleMatrix m_mxBeta;				//回归系数矩阵
		CDoubleMatrix m_mxSEReg;			//回归系数的标准误
		CDoubleMatrix m_mxTvalue;			//回归系数的T值
		CDoubleMatrix m_mxPvalue;			//回归系数的P值

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

	BOOL GetValueFromVo(CDataInterface *pDataInterface, string VO);
	//结果函数	
	void DrawResult();

	//获取回归矩阵Matrix_X
	void GetMatrixData(CDoubleMatrix & Matrix_X);
				
	//ADF检验主程序，成功则返回TRUE，否则返回FALSE
	BOOL ADFtest();												
	
	//PP检验主程序，成功则返回TRUE，否则返回FALSE
	BOOL PPtest();								
	
	//KPSS主程序，成功则返回TRUE，否则返回FALSE
	BOOL KPSStest();											
																
	//用于对模型给出单个方程的统计量
	BOOL Paratest(CDoubleMatrix mxBeta,     //输入系数阵
				  CDoubleMatrix mxRes,		//残差阵
				  CDoubleMatrix mxY,		//回归因变量
				  CDoubleMatrix mxX,		//回归自变量矩阵
				  int VARindex);			//第几个矩阵序号
	
	//用于计算指定矩阵的差分函数
	CDoubleMatrix Vdif(CDoubleMatrix mxX,int m_nLagnum);
};

#endif // !defined(AFX_ROOTTEST_H__B320738C_3229_4C2D_AFD4_BB0D238FD57A__INCLUDED_)










































