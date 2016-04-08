// PCR.h: interface for the CPrinCompReg class.
//							主成分回归
//              gezehui
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PCR_H__36F24C0F_127E_45DD_B569_969506E6006D__INCLUDED_)
#define AFX_PCR_H__36F24C0F_127E_45DD_B569_969506E6006D__INCLUDED_

#include "AlgorithmBase.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct TPrinCompRegResult 
{
public:
	//初始化
	TPrinCompRegResult()
	{
		m_bWithConst = true;
		//			m_strEqation = "";
		m_ncMethod=				0;
		m_ncValue =			2;	
		
	}
	//复制结构
	TPrinCompRegResult& operator =(TPrinCompRegResult &PrinCompRegResult)
	{
		m_bWithConst=PrinCompRegResult.m_bWithConst;
		m_vectBeta=PrinCompRegResult.m_vectBeta;
		m_ncValue=PrinCompRegResult.m_ncValue;
		m_dRSS=PrinCompRegResult.m_dRSS;
		m_dESS=PrinCompRegResult.m_dESS;
		m_Matrix_R=PrinCompRegResult.m_Matrix_R;
		m_ncMethod=PrinCompRegResult.m_ncMethod;
		m_matEigVal=PrinCompRegResult.m_matEigVal;
		m_matEigVect=PrinCompRegResult.m_matEigVect;
		m_strVarlist.Copy(PrinCompRegResult.m_strVarlist);
	}
	//释放内存
	void clear()
	{
		m_strVarlist.RemoveAll();
	}
	
public:
	CTStringArray	m_strVarlist;//变量名列表，第一个为因变量，2至Varlist.GetLength()-1为自变量
	CDoubleVector	m_vectBeta;//回归系数，若带常数项，则m_vectBeta(0)为常数项。
	BOOL			m_bWithConst;//是否带常数项；0- 否 ，1－表示带常数项
	CDoubleMatrix   m_matEigVal;//特征值，从大到小排列
	CDoubleMatrix	m_matEigVect;//特征向量每列对应一个特征值，排列从大到小。
	int 			m_ncValue; //主成分数，在零到m_nXCol之间
	double			m_dRSS;//回归平方和，》0
	double			m_dESS;//残差平方和，》0
	CDoubleMatrix   m_Matrix_R;//自变量相关系数矩阵
	int				m_ncMethod;//c取值方法0-3
	// 	CString			m_strRegEquation; //回归方程
};

class CPCRDlg;
//Principal Component Regression 主成分回归
class CPrinCompReg : public CAlgorithmBase
{
public:
	CPrinCompReg();
	virtual ~CPrinCompReg();
	CResult	*OnPCR(CDataInterface *pDataInterface, string VO);//对外主接口
	BOOL    m_bOutputModel;
	TPrinCompRegResult* m_pPCRResult;
private:
	int				m_nXRow;//有效样本容量（缺失值处理之后）
	int				m_nXCol;//自变量个数
	CPCRDlg *		m_pDlg;//对话框
	CResult *		m_pResult;//结果
	BOOL			m_bWithConst;//是否带常数项
	BOOL			m_bWithPC;//是否显示主轴（特征向量）
	CDoubleMatrix	m_MatCoeff;//回归系数
	double			m_dINTERCEPT;//常数项，截距
	CDoubleMatrix   m_EigValue;//特征值
	CDoubleMatrix	m_EigVector;//特征向量
	int 			m_ncValue; //主成分数，在零到m_nXCol之间
	double			m_dRSS;//回归平方和
	double			m_dESS;//残差平方和
	CDoubleMatrix   m_Matrix_R;//自变量相关系数矩阵
	int				m_ncMethod;//c取值方法
	int				m_nMissingMethod;//缺失值处理方法，0添均值，1整行删除
	double m_dcValueCalContr;//界面参数
	double m_dcValueCorrEigVal;//界面参数
private:
// 	int				GetCurSec();
	BOOL			GetValueFromVo(CDataInterface *pDataInterface, string VO);
	CDoubleMatrix	EigValPlusVec(CDoubleMatrix* m_EigVec, bool nIndex);
	void			CreateResult();
 	void			GetMatrixData(CDoubleMatrix* Matrix_X,CDoubleMatrix* Matrix_y);//取数据并处理缺失值，将数据放入矩阵Matrix_X&Matrix_y
// 	CDoubleMatrix	TranProd(CDoubleMatrix &X, CDoubleMatrix &Y);		
protected:
	CDataInterface*	m_pDataInterface;
	TIntStruct		m_tVarInfo;//自变量struct
	TIntStruct		m_tVarInfo_D;//因变量struct
	CTStringArray  	m_strVarlist;//变量名列表，第一个为因变量，2至Varlist.GetLength()-1为自变量
		
};

#endif // !defined(AFX_PCR_H__36F24C0F_127E_45DD_B569_969506E6006D__INCLUDED_)





















