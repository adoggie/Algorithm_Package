// Simuequation.h: interface for the CSimuequation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMUEQUATION_H__D77A8B1F_5DCC_40D9_BC11_F5D30BB745D9__INCLUDED_)
#define AFX_SIMUEQUATION_H__D77A8B1F_5DCC_40D9_BC11_F5D30BB745D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmBase.h"

class CSimuequation : public CAlgorithmBase  
{
public:
	CSimuequation();
	virtual ~CSimuequation();
	CResult * OnSimEqu(CDataInterface *pDataInterface, string VO);//外部调用主要程序
	TIntStruct		m_tVarInfo;//回归变量struct
	TIntStruct		m_tVarInfo_D;//外生变量struct	
	
private:
	BOOL m_bWarning;
	int m_SEmethod;						//给定模型的估计方法
	int m_SEqnumber;					//模型的解释方程的个数
 	CDoubleMatrix m_mxX;
 	CDoubleMatrix m_mxY;
 	CDoubleMatrix m_mxP;				//解释方程的信息矩阵
 	CDoubleMatrix m_mxINP;
	CTStringArray	m_strDepend;        //内生变量名称字符串
	CTStringArray	m_strVarSel;        //先决变量名称字符串
	CTStringArray	m_szArrSE;
	CTStringArray	m_ListVarArr;		//P矩阵的变量的按顺序排列
	CTStringArray	m_coef_name;		//系数名c(0)等
	CTStringArray	m_coef_var;			//系数名对应的变量名
	CDoubleVector	m_vrG;				//存储每个方程右侧中含有多少个内生变量
	CDoubleVector	m_vrK;				//存储每个方程右侧中含有多少个先决变量
	double m_dCoeffMax;					//存储多个方程每个方程中最多有多少个系数
	double m_dCoffTotal;				//存储多个方程总共有多少的系数
	CDoubleMatrix mxPLag;				//滞后向量的指示矩阵
	CDoubleMatrix	m_lagMatrix;		//滞后矩阵
	CIntVector		m_ivLagNum;			//滞后矩阵列滞后的阶数
	CDoubleVector   m_vrLagVarNum;		//每个方程的滞后变量的个数
	double LagMaxTotal ;						//最大滞后阶数
	
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

	} m_tOut;

	//主程序
	BOOL SimEqu(CDoubleMatrix &mxX,		//内生变量矩阵
			CDoubleMatrix &mxY,			//先决变量矩阵
			CDoubleMatrix &mxP,			//模型描述矩阵
			CDoubleMatrix &mxINP);		//约束方程描述
	BOOL GetValueFromVo(CDataInterface* pDataInterface, string VO);
	BOOL GetData(CDoubleMatrix &Matrix_X, CDoubleMatrix &Matrix_Y);//读取数据
	void DrawResult();					//输出结果展示					


	BOOL IV(CDoubleMatrix &mxX,			//内生变量矩阵
		CDoubleMatrix &mxY,			//先决变量矩阵
		CDoubleMatrix &mxP,			//模型描述矩阵
		CDoubleMatrix &mxINP);		//约束矩阵
	BOOL ILS(CDoubleMatrix &mxX,			//内生变量矩阵
		CDoubleMatrix &mxY,			//先决变量矩阵
		CDoubleMatrix &mxP,			//模型描述矩阵
		CDoubleMatrix &mxINP);		//约束矩阵
	BOOL TwoLS(CDoubleMatrix &mxX,		//内生变量矩阵
		CDoubleMatrix &mxY,			//先决变量矩阵
		CDoubleMatrix &mxP,			//模型描述矩阵
		CDoubleMatrix &mxINP);		//约束矩阵
	BOOL ThreeLS(CDoubleMatrix &mxX,		//内生变量矩阵
		CDoubleMatrix &mxY,			//先决变量矩阵
		CDoubleMatrix &mxP,			//模型描述矩阵
		CDoubleMatrix &mxINP,		//约束矩阵
		CDoubleMatrix &mxBeta_3SLS);//传出的系数矩阵
	BOOL FIML(CDoubleMatrix &mxX,		//内生变量矩阵
		CDoubleMatrix &mxY,			//先决变量矩阵
		CDoubleMatrix &mxP,			//模型描述矩阵
		CDoubleMatrix &mxINP);		//约束矩阵
	BOOL MODELTEST(int i,				//第i个方程
		CDoubleMatrix &mxP,			//模型的描述矩阵
		CDoubleMatrix &mxINP,		//约束方程描述
		int g0,						//模型中总的内生变量个数
		int k0);					//模型中总的先决变量个数

	//参数检验函数
	BOOL Paramtest(int i,			//第i个方程
			CDoubleMatrix &mxYest,	//第i个方程右边的内生变量的预测值
			CDoubleVector &vrYi,	//第i个方程左面的内生变量
			CDoubleMatrix &mxY0,	//第i个方程右边的内生变量的原始值
			CDoubleMatrix &mxXi,	//第i个方程右面的先决变量的值
			CDoubleMatrix &mxBi,	//估计的第i个方程的系数（内生变量的系数）
			CDoubleMatrix &mxGamai,	//估计的第i个方程的系数（先决变量的系数）
			int nn,int gi,int ki);	//分别给出第i个方程的观测值个数，内生变量的个数，先决变量的个数
	CDoubleMatrix Kproduct(CDoubleMatrix & mxX,CDoubleMatrix &mxY);//求矩阵和矩阵的叉积

	//调用Nelder—Meade算法给出最优的解
	CDoubleVector Nelder(CDoubleMatrix &mxSTar,int nN,double n1,double neldtol,double n2,CDoubleMatrix &mxXmat,CDoubleMatrix &mxYmat);
	
	//给出极大似然函数的值
	double logfiml(CDoubleVector &vrBetai, CDoubleMatrix &mxYmat,CDoubleMatrix &mxXmat);

	BOOL ReadStrs(CTStringArray& equs);//解析所有方程是否合理
	BOOL legstr(CTString eqution);//解析一句方程是否合理
	BOOL inselectinlist(CTString strTemp0);//判断字符串是否和内生变量相同
	BOOL inselectlist(CTString strTemp0);//判断字符串是否和选入变量相同
	BOOL strIsANum(CTString strTemp0);//判断字符串是否为整数
	BOOL strIsADouble(CTString strTemp0);//判断是否为浮点数
	BOOL ReadStrToMx();//把字符串读入P矩阵
	BOOL ReadLineToMx(CTString strTemp, int i);//把字符串读入矩阵的第i列
	BOOL ReadLineToINPMx(CTString senTemp, int i);//读第i行入INP矩阵
	BOOL ReadStrToINPMx();//读入INP矩阵
	CTStringArray 	m_StrVarOrd;
	CTStringArray	m_str_fun;
	CTStringArray	m_CID_fun;
	CTStringArray	m_LimitArrSE;
	BOOL m_bBeginwithC;//字符串的一行是c开头的还是数字开头的，用于决定是否为约束矩阵
};

#endif // !defined(AFX_SIMUEQUATION_H__3A3E1DC3_F18E_40D1_B129_3DD17F9B856D__INCLUDED_)
