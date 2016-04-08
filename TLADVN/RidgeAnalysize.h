// RidgeAnalysize.h: interface for the CRidgeAnalysize class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RIDGEANALYSIZE_H__D4F7E9F0_470C_4F34_97A8_7E1760DAFC1E__INCLUDED_)
#define AFX_RIDGEANALYSIZE_H__D4F7E9F0_470C_4F34_97A8_7E1760DAFC1E__INCLUDED_
#include "AlgorithmBase.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "RidgeDlg.h"
//岭回归的模型输出结构体
struct TRidgeResult 
{
public:
	//初始化
	TRidgeResult()
	{
			m_bWithConst = true;
//			m_strEqation = "";
			m_dkValue=0.5;
			m_nkMethod=4;

	}
	//复制操作(=操作)
	TRidgeResult& operator =(TRidgeResult &RidgeModelResult)
	{
		m_bWithConst=RidgeModelResult.m_bWithConst;
		m_vectBeta=RidgeModelResult.m_vectBeta;
		m_dkValue=RidgeModelResult.m_dkValue;
		m_dRSS=RidgeModelResult.m_dRSS;
		m_dESS=RidgeModelResult.m_dESS;
		m_Matrix_R=RidgeModelResult.m_Matrix_R;
		m_nkMethod=RidgeModelResult.m_nkMethod;
		m_strVarlist.Copy(RidgeModelResult.m_strVarlist);
	}
	//清空内存
	void clear()
	{
		m_strVarlist.RemoveAll();
	}
	//结构成员变量
public:
	CDoubleVector	m_vectBeta;//回归系数，若带常数项，则m_vectBeta(0)为常数项。
// 	CString			m_strRegEquation; //回归方程
	CTStringArray	m_strVarlist;//变量名列表，第一个为因变量，2至Varlist.GetLength()-1为自变量
	BOOL			m_bWithConst;//是否带常数项0- 否 ，1－表示带常数项
// 	BOOL			m_bWithRidgeTrace;//是否岭迹分析0- 否 ，1－表示岭迹分析图
	double			m_dkValue; //岭参数k，》0
	double			m_dRSS;//回归平方和，》0
	double			m_dESS;//残差平方和，》0
	CDoubleMatrix   m_Matrix_R;//自变量相关系数矩阵
	int				m_nkMethod;//k取值方法0-4帮助文档
	
};
class CRidgeDlg;
class CRidgeAnalysize : public CAlgorithmBase  
{
public:
	CRidgeAnalysize();
	virtual ~CRidgeAnalysize();
	bool			m_bOutputModel;//是否输出模型，0-不输出；1－输出，默认为0;	
	//*****************************注意：此变量连同Struct RidgeResult只有在算法包调用时才可设置

	CResult *		OnRidgeAnalysize(CDataInterface *pDataInterface, string VO);//岭回归
	TRidgeResult*	m_pRidgeModelResult;//岭回归的模型输出结构体

private:
	int				m_nXRow;//有效样本容量
	int				m_nXCol;//自变量个数
	CRidgeDlg *		m_pDlg;//对话框
	CResult *		m_pResult;//结果
	BOOL			m_bWithConst;//是否带常数项
	BOOL			m_bWithRidgeTrace;//是否岭迹分析
	CDoubleMatrix	m_MatCoeff;//回归系数
	double			m_dINTERCEPT;//常数项，截距
	double			m_dkValue; //岭参数k，》0
	double			m_dRSS;//回归平方和，》0
	double			m_dESS;//残差平方和，》0
	CDoubleMatrix   m_Matrix_R;//自变量相关系数矩阵
 	int				m_nkMethod;//k取值方法0-4
	int				m_nMissingMethod;//缺失值处理方法
	double			m_kMax;//岭迹分析的k值
private:
	BOOL			GetValueFromVo(CDataInterface * pDataInterface, string VO);
	CDoubleMatrix	EigValPlusVec(CDoubleMatrix* m_EigVec, bool nIndex);//特征值和特征向量，参量的意义请见函数定义		
	void			CreateResultTrace(CDoubleMatrix m_EigValue, CDoubleMatrix m_EigVetor,CDoubleMatrix Sy, double kMax);//岭迹分析结果
    void			CreateResult();//创建结果集
	double			OptiFastDownward(CDoubleMatrix m_EigValue,CDoubleMatrix Sy,double sigma,double xInit);//最速下降法求目标函数的最优解，留待扩展
	double			H(double k, CDoubleMatrix Sy,CDoubleMatrix lambda,double sigma);//均方差k函数，只适用于岭估计
	double          HDeri(double k, CDoubleMatrix Sy,CDoubleMatrix lambda,double sigma);//均方差H(k)的导数，只适用于岭估计
 	void			GetMatrixData(CDoubleMatrix* Matrix_X, CDoubleMatrix* Matrix_y);//从数据接口提取变量矩阵，Matrix_X自变量，Matrix_y自变量
	double			GetSigma(CDoubleMatrix* Matrix_X,CDoubleMatrix* Matrix_y,CDoubleMatrix Sy, CDoubleMatrix EigValue, CDoubleMatrix EigVector);
//	double			GetkValue(CDoubleMatrix Sy,CDoubleMatrix EigValue, CDoubleMatrix EigVector, int m_nkMethod);
protected:
	CDataInterface*	m_pDataInterface;
	CTStringArray	m_strVarlist;//变量名，第一个为因变量，2至Varlist.GetLength()-1为自变量	
	TIntStruct		m_tVarInfo;//自变量struct
	TIntStruct		m_tVarInfo_D;//因变量struct
	
};

#endif // !defined(AFX_RIDGEANALYSIZE_H__D4F7E9F0_470C_4F34_97A8_7E1760DAFC1E__INCLUDED_)
