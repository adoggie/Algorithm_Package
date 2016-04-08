// ChiSquare.h: interface for the CChiSquare class.
//				��������
//					����ׯ    2006/03/15
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHISQUARE_H__9FE7D572_27BC_49E3_B3DE_83B0D524A4D8__INCLUDED_)
#define AFX_CHISQUARE_H__9FE7D572_27BC_49E3_B3DE_83B0D524A4D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class  AFX_EXT_CLASS CChiSquare// : public CStatiMethod  
{
public:
	CResult* OnChiSquare(CDataInterface * pDataInterface, string VO);//�ⲿ���õ�������
	TIntStruct m_tVarInfo;//�Ա����±꼯
	CChiSquare();
	virtual ~CChiSquare();

private:
	BOOL GetValueFromVo(CDataInterface * pDataInterface, string VO);
	BOOL GetData(CDataInterface *pDataInterface, CDoubleMatrix & Matrix_Data, int index);//��ȡ����
	BOOL CountFreq(CDoubleVector & vec_x, CDoubleVector & vec_weight, CIntVector & vec_index, CDoubleVector & vec_crit, CDoubleVector & vec_count);//���������͵ķֲ�����, ��vec_critΪ�ֶα�׼, ����vec_x�ڸ����������������vec_count�ι۲�
	double	 countfordis(CDoubleVector & vec_x, CDoubleVector & vec_weight, CIntVector & vec_index, int i);//����vec_x��, �۲�ֵ����"i"ֵ�ĳ����˶��ٴ�
	double	 countfordis(CDoubleVector & vec_x, CDoubleVector & vec_weight, CIntVector & vec_index, double i);//����vec_x��, �۲�ֵ����"i"ֵ�ĳ����˶��ٴ�
	int FindStartIndex(CDoubleVector &vec_x,int nVxInd,int nVtData,int nStep_g,int nStep_b);//���ҵ�һ������Ҫ�������
	int FindEndIndex(CDoubleVector &vec_x,int nVxInd,int nVtData,int nStep_g,int nStep_b);  //�������һ��������Ҫ�������(����Ҫ�����һ������)
	BOOL _CountFreq(CDoubleVector & vec_x, CDoubleVector & vec_weight, CIntVector & vec_index, CDoubleVector & vec_crit, CDoubleVector & vec_count);//������ɢ���ݵ�Ƶ��(�����޳���������Ҫ��Ĳ���,��Ե�������)
	BOOL CalcFreq(CDoubleVector & vec_x, CDoubleVector & vec_weight, CIntVector & vec_index, CDoubleMatrix &frq_matrix);//ͳ��������Ƶ��������ֵ��Ƶ������fre_matrix.
	int  FindNextIndex(CDoubleVector &vec_x, int nVxInd, int nStep_g, int nStep_b);//�ҵ�����Ҫ����ֵͬ������������
	void ReSize(CDoubleVector & vec_x, double fPercent);//���������ĳ��ȣ������β��һ�� fPercent = 1%)
	void DrawResult(CDoubleVector vec_crit, CDoubleVector vec_count, CDoubleVector vec_expect, CDoubleVector vec_res, CResult *pResult);
	void DrawResultInt(CDoubleVector vec_crit, CDoubleVector vec_count, CDoubleVector vec_expect, CDoubleVector vec_res, CResult *pResult);
	void DrawResultBin(CDoubleVector vec_crit, CDoubleVector vec_count, CDoubleVector vec_expect, CDoubleVector vec_res, CResult *pResult);
	void DrawResultFreq(CDoubleVector vec_crit, CDoubleVector vec_count, CDoubleVector vec_expect, CDoubleVector vec_res, CResult *pResult);
	void ReSize(CIntVector & vec_x, double fPercent);//���������ĳ��ȣ������β��һ�� fPercent = 1%)
	void DrawOutput(CDoubleVector vec_Chi2, CDoubleVector vec_df, CDoubleVector vec_sig, CResult *pResult);//�����������



//�ӽ����õĲ���:ʼ
	BOOL	m_bWithWeight;//�Ƿ���Ȩ��
	int		m_nWeight;//Ȩ�ص�id
	BOOL	m_bOutputFreqTable;//�Ƿ����Ƶ�ʷ�����ϸ���
	BOOL	m_bCheckDel;//�����ݽ��н�β����
	double	m_dDelPer;//�����ݽ��н�β����ı���

	BOOL	m_bCheckDiscrib;//���û�������ɢ�ĸ������ʵķֲ�
	CDoubleVector vec_y_dis;//�û��������ɢ�����۲�ֵ
	CDoubleVector vec_y_prob;//�û��������ɢ�������ֵ�Ԥ�ڸ���

	int		m_nType;//0-��ɢ, 1- ����
	BOOL	m_bCheckFreq;//�Ƿ����Ƶ����ͬ
	BOOL	m_bCheckBin;//Binomial�ֲ�
	BOOL	m_bGetVoBin;//Bin�Ĳ����Ƿ�ӽ����ȡ
	int		m_Bin_n;//Binomial�ֲ���n
	double	m_Bin_p;//Binomial�ֲ���p
	BOOL	m_bCheckGeo;//���ηֲ�
	BOOL	m_bGetVoGeo;
	double	m_Geo_p;
	BOOL	m_bCheckNegativBin;//������ʽ�ֲ�//������ʽ�ֲ�, û�и�����ʽ�ֲ����溯��, ��ʱ������.
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
//�ӽ����õĲ���:ֹ


//Ϊ��̷��㶨��ı���:ʼ
	CResult* m_pResult;
	int		m_nRow;//�۲���
	double	m_dChi;//X2ֵ
	double	m_sig;//���Ŷ�
	CTString	m_str_x;//�Ա���������(һ��)
	CTString	m_str_FunInfo;//����ķֲ�����Ϣ(���ּ�����)
	CTStringArray m_AllFuncInfo;//���еļ��鷽�̵����ֺ���Ϣ
	int		m_nlastcountto;//�ϴ�����������, (����Ҫ֪��ʲô��˼)
//Ϊ��̷��㶨��ı���:ֹ
};
#endif // !defined(AFX_CHISQUARE_H__9FE7D572_27BC_49E3_B3DE_83B0D524A4D8__INCLUDED_)
