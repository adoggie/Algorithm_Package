// CKTest.h: interface for the CCKTest class.
//				单样本KS检验
//					马良庄 2006/03/20
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CKTEST_H__0E562FF2_E427_4176_A550_66261CBAB943__INCLUDED_)
#define AFX_CKTEST_H__0E562FF2_E427_4176_A550_66261CBAB943__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "StatiMethod.h"

class AFX_EXT_CLASS CCKTest //: public CStatiMethod  
{
public:
	CCKTest();
	virtual ~CCKTest();
	CResult * OnCKTest(CDataInterface *pDataInterface, string VO);

private:
	BOOL	GetValueFromVo(CDataInterface *pDataInterface, string VO);
	BOOL	GetData(CDataInterface *pDataInterface, CDoubleMatrix & Matrix_Data, int index);
	BOOL	CKTestMain(CDoubleMatrix & data_read);
	double	CalcP(double Z);
	void	DrawOutput();
	inline bool IsInt(double fValue);
    void CalcVec_DisCum(CDoubleVector & vec_x, CDoubleVector & vec_weight, CIntVector & vec_index, int nCase, int nStep, CDoubleVector &vec_discum, CDoubleVector &vec_DisValue);//计算累计概率，返回值存放在向量vec_discum

//界面读入参数 始
	TIntStruct m_tVarInfo;
	BOOL	m_bWithWeight;
	BOOL	m_nWeight;
	BOOL	m_bCheckDel;//对数据进行截尾处理
	double	m_dDelPer;//对数据进行截尾处理的比例

	BOOL	m_bCheckBin;//Binomial分布
	BOOL	m_bGetVoBin;//Bin的参数是否从界面读取
	int		m_Bin_n;//Binomial分布的n
	double	m_Bin_p;//Binomial分布的p
	BOOL	m_bCheckGeo;//几何分布
	BOOL	m_bGetVoGeo;
	double	m_Geo_p;
	BOOL	m_bCheckNegativBin;//负二项式分布
	BOOL	m_bCheckPoisson;//Poisson分布
	BOOL	m_bGetVoPoisson;
	double	m_Poisson_lambda;
	
	BOOL	m_bCheckExp;//是否检验指数
	BOOL	m_bGetVoExp;
	double	m_Exp_lambda;
	BOOL	m_bCheckGam;//gamma分布
	BOOL	m_bGetVoGam;
	double	m_Gam_alpha;
	double	m_Gam_beta;
	BOOL	m_bCheckHalfNor;//半正态分布
	BOOL	m_bGetVoHalfNor;
	double	m_HalfNor_mu;
	double	m_HalfNor_sigma;
	BOOL	m_bCheckLogist;//logistic分布
	BOOL	m_bGetVoLogist;
	double	m_Logist_m;
	double	m_Logist_b;
	BOOL	m_bCheckLogNormal;//是否检验对数正态
	BOOL	m_bGetVoLogNormal;
	double	m_lognormalmu;//对数正态分布的mu
	double	m_lognormalsigma;//对数正态分布的sigma
	BOOL	m_bCheckNormal;//是否检验正态
	BOOL	m_bGetVoNormal;
	double	m_Normal_mu;
	double	m_Normal_sigma;
	BOOL	m_bCheckAverage;//是否检验均值
	BOOL	m_bGetVoAverage;
	double	m_Average_low;
	double	m_Average_up;
	BOOL	m_bCheckBeta;//是否检验beta分布
	BOOL	m_bGetVoBeta;
	double	m_Beta_alpha;
	double	m_Beta_beta;
	BOOL	m_bCheckWeib;//是否服从Weib分布
	BOOL	m_bGetVoWeib;
	double	m_Weib_alpha;
	double	m_Weib_beta;
	BOOL	m_bCheckLaplace;//Laplace分布
	BOOL	m_bGetVoLaplace;
	double	m_Laplace_b;
	double	m_Laplace_mu;

//界面读入参数 止

//程序编写方便定义的变量 始
	int				m_nRow;//有效观测数
	double			m_totalweight;//总的权重, 无权重时= m_nRow;
	CResult		   *m_pResult; 
	CTString			m_str_x;//自变量的名字(一个)
	CTString			m_str_FunInfo;//检验的分布的信息(名字及参数)
	CTStringArray	m_AllFuncInfo;//所有的检验方程的名字和信息
	CDoubleVector	m_vec_D;//D值
	CDoubleVector	m_vec_sig;//sig值
	CIntVector		m_vec_index;//sig排序的原始id
	CDoubleVector	m_vec_z;//Z值
//程序编写方便定义的变量 止
};

#endif // !defined(AFX_CKTEST_H__0E562FF2_E427_4176_A550_66261CBAB943__INCLUDED_)
