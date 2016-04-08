// CKTest.h: interface for the CCKTest class.
//				������KS����
//					����ׯ 2006/03/20
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
    void CalcVec_DisCum(CDoubleVector & vec_x, CDoubleVector & vec_weight, CIntVector & vec_index, int nCase, int nStep, CDoubleVector &vec_discum, CDoubleVector &vec_DisValue);//�����ۼƸ��ʣ�����ֵ���������vec_discum

//���������� ʼ
	TIntStruct m_tVarInfo;
	BOOL	m_bWithWeight;
	BOOL	m_nWeight;
	BOOL	m_bCheckDel;//�����ݽ��н�β����
	double	m_dDelPer;//�����ݽ��н�β����ı���

	BOOL	m_bCheckBin;//Binomial�ֲ�
	BOOL	m_bGetVoBin;//Bin�Ĳ����Ƿ�ӽ����ȡ
	int		m_Bin_n;//Binomial�ֲ���n
	double	m_Bin_p;//Binomial�ֲ���p
	BOOL	m_bCheckGeo;//���ηֲ�
	BOOL	m_bGetVoGeo;
	double	m_Geo_p;
	BOOL	m_bCheckNegativBin;//������ʽ�ֲ�
	BOOL	m_bCheckPoisson;//Poisson�ֲ�
	BOOL	m_bGetVoPoisson;
	double	m_Poisson_lambda;
	
	BOOL	m_bCheckExp;//�Ƿ����ָ��
	BOOL	m_bGetVoExp;
	double	m_Exp_lambda;
	BOOL	m_bCheckGam;//gamma�ֲ�
	BOOL	m_bGetVoGam;
	double	m_Gam_alpha;
	double	m_Gam_beta;
	BOOL	m_bCheckHalfNor;//����̬�ֲ�
	BOOL	m_bGetVoHalfNor;
	double	m_HalfNor_mu;
	double	m_HalfNor_sigma;
	BOOL	m_bCheckLogist;//logistic�ֲ�
	BOOL	m_bGetVoLogist;
	double	m_Logist_m;
	double	m_Logist_b;
	BOOL	m_bCheckLogNormal;//�Ƿ���������̬
	BOOL	m_bGetVoLogNormal;
	double	m_lognormalmu;//������̬�ֲ���mu
	double	m_lognormalsigma;//������̬�ֲ���sigma
	BOOL	m_bCheckNormal;//�Ƿ������̬
	BOOL	m_bGetVoNormal;
	double	m_Normal_mu;
	double	m_Normal_sigma;
	BOOL	m_bCheckAverage;//�Ƿ�����ֵ
	BOOL	m_bGetVoAverage;
	double	m_Average_low;
	double	m_Average_up;
	BOOL	m_bCheckBeta;//�Ƿ����beta�ֲ�
	BOOL	m_bGetVoBeta;
	double	m_Beta_alpha;
	double	m_Beta_beta;
	BOOL	m_bCheckWeib;//�Ƿ����Weib�ֲ�
	BOOL	m_bGetVoWeib;
	double	m_Weib_alpha;
	double	m_Weib_beta;
	BOOL	m_bCheckLaplace;//Laplace�ֲ�
	BOOL	m_bGetVoLaplace;
	double	m_Laplace_b;
	double	m_Laplace_mu;

//���������� ֹ

//�����д���㶨��ı��� ʼ
	int				m_nRow;//��Ч�۲���
	double			m_totalweight;//�ܵ�Ȩ��, ��Ȩ��ʱ= m_nRow;
	CResult		   *m_pResult; 
	CTString			m_str_x;//�Ա���������(һ��)
	CTString			m_str_FunInfo;//����ķֲ�����Ϣ(���ּ�����)
	CTStringArray	m_AllFuncInfo;//���еļ��鷽�̵����ֺ���Ϣ
	CDoubleVector	m_vec_D;//Dֵ
	CDoubleVector	m_vec_sig;//sigֵ
	CIntVector		m_vec_index;//sig�����ԭʼid
	CDoubleVector	m_vec_z;//Zֵ
//�����д���㶨��ı��� ֹ
};

#endif // !defined(AFX_CKTEST_H__0E562FF2_E427_4176_A550_66261CBAB943__INCLUDED_)
