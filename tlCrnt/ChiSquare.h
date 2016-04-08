// ChiSquare.h: interface for the CChiSquare class.
//				卡方检验
//					马良庄    2006/03/15
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHISQUARE_H__9FE7D572_27BC_49E3_B3DE_83B0D524A4D8__INCLUDED_)
#define AFX_CHISQUARE_H__9FE7D572_27BC_49E3_B3DE_83B0D524A4D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class  AFX_EXT_CLASS CChiSquare// : public CStatiMethod  
{
public:
	CResult* OnChiSquare(CDataInterface * pDataInterface, string VO);//外部调用的主程序
	TIntStruct m_tVarInfo;//自变量下标集
	CChiSquare();
	virtual ~CChiSquare();

private:
	BOOL GetValueFromVo(CDataInterface * pDataInterface, string VO);
	BOOL GetData(CDataInterface *pDataInterface, CDoubleMatrix & Matrix_Data, int index);//读取数据
	BOOL CountFreq(CDoubleVector & vec_x, CDoubleVector & vec_weight, CIntVector & vec_index, CDoubleVector & vec_crit, CDoubleVector & vec_count);//计算连续型的分布区间, 以vec_crit为分段标准, 计算vec_x在各段区间里面出现了vec_count次观察
	double	 countfordis(CDoubleVector & vec_x, CDoubleVector & vec_weight, CIntVector & vec_index, int i);//计算vec_x中, 观察值等于"i"值的出现了多少次
	double	 countfordis(CDoubleVector & vec_x, CDoubleVector & vec_weight, CIntVector & vec_index, double i);//计算vec_x中, 观察值等于"i"值的出现了多少次
	int FindStartIndex(CDoubleVector &vec_x,int nVxInd,int nVtData,int nStep_g,int nStep_b);//查找第一个符合要求的索引
	int FindEndIndex(CDoubleVector &vec_x,int nVxInd,int nVtData,int nStep_g,int nStep_b);  //查找最后一个不符合要求的索引(符合要求的下一个索引)
	BOOL _CountFreq(CDoubleVector & vec_x, CDoubleVector & vec_weight, CIntVector & vec_index, CDoubleVector & vec_crit, CDoubleVector & vec_count);//计算离散数据的频度(其中剔除掉不符合要求的部分,针对的是整数)
	BOOL CalcFreq(CDoubleVector & vec_x, CDoubleVector & vec_weight, CIntVector & vec_index, CDoubleMatrix &frq_matrix);//统计向量的频数，向量值及频数放在fre_matrix.
	int  FindNextIndex(CDoubleVector &vec_x, int nVxInd, int nStep_g, int nStep_b);//找到符合要求（相同值）的向量索引
	void ReSize(CDoubleVector & vec_x, double fPercent);//缩减向量的长度（针对首尾各一半 fPercent = 1%)
	void DrawResult(CDoubleVector vec_crit, CDoubleVector vec_count, CDoubleVector vec_expect, CDoubleVector vec_res, CResult *pResult);
	void DrawResultInt(CDoubleVector vec_crit, CDoubleVector vec_count, CDoubleVector vec_expect, CDoubleVector vec_res, CResult *pResult);
	void DrawResultBin(CDoubleVector vec_crit, CDoubleVector vec_count, CDoubleVector vec_expect, CDoubleVector vec_res, CResult *pResult);
	void DrawResultFreq(CDoubleVector vec_crit, CDoubleVector vec_count, CDoubleVector vec_expect, CDoubleVector vec_res, CResult *pResult);
	void ReSize(CIntVector & vec_x, double fPercent);//缩减向量的长度（针对首尾各一半 fPercent = 1%)
	void DrawOutput(CDoubleVector vec_Chi2, CDoubleVector vec_df, CDoubleVector vec_sig, CResult *pResult);//输出最后的置信



//从界面获得的参数:始
	BOOL	m_bWithWeight;//是否有权重
	int		m_nWeight;//权重的id
	BOOL	m_bOutputFreqTable;//是否输出频率分析详细表格
	BOOL	m_bCheckDel;//对数据进行截尾处理
	double	m_dDelPer;//对数据进行截尾处理的比例

	BOOL	m_bCheckDiscrib;//由用户输入离散的给定概率的分布
	CDoubleVector vec_y_dis;//用户输入的离散变量观察值
	CDoubleVector vec_y_prob;//用户输入的离散变量出现的预期概率

	int		m_nType;//0-离散, 1- 连续
	BOOL	m_bCheckFreq;//是否出现频率相同
	BOOL	m_bCheckBin;//Binomial分布
	BOOL	m_bGetVoBin;//Bin的参数是否从界面读取
	int		m_Bin_n;//Binomial分布的n
	double	m_Bin_p;//Binomial分布的p
	BOOL	m_bCheckGeo;//几何分布
	BOOL	m_bGetVoGeo;
	double	m_Geo_p;
	BOOL	m_bCheckNegativBin;//负二项式分布//负二项式分布, 没有负二项式分布的逆函数, 暂时不算他.
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
//从界面获得的参数:止


//为编程方便定义的变量:始
	CResult* m_pResult;
	int		m_nRow;//观测数
	double	m_dChi;//X2值
	double	m_sig;//可信度
	CTString	m_str_x;//自变量的名字(一个)
	CTString	m_str_FunInfo;//检验的分布的信息(名字及参数)
	CTStringArray m_AllFuncInfo;//所有的检验方程的名字和信息
	int		m_nlastcountto;//上次数到多少了, (不需要知道什么意思)
//为编程方便定义的变量:止
};
#endif // !defined(AFX_CHISQUARE_H__9FE7D572_27BC_49E3_B3DE_83B0D524A4D8__INCLUDED_)
