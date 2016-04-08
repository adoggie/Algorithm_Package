// TimeX12.h: interface for the CTimeX12 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMEX12_H__3D80476B_675F_4757_A883_D1A27A40B741__INCLUDED_)
#define AFX_TIMEX12_H__3D80476B_675F_4757_A883_D1A27A40B741__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmBase.h"
#include "Arima.h"

class CTimeX12 : public CAlgorithmBase  
{
public:
	CTimeX12();
	virtual ~CTimeX12();
	CResult * OnTimeX12(CDataInterface *pDataInterface, string VO); //外部调用主算法

private:
	BOOL    GetValueFromVo(CDataInterface *pDataInterface, string VO);
	void	WeekCalcu(int nYear, int nMonth);//计算序列每个月有多少天，有多少个周一周二……，放入m_DaysinMonth_vec和m_WeekDescrib_mx
	int		DaysInMonth(int nYear, int nMonth);//每个月有多少天
	void    IndependentVariables(int nYear, int nMonth);//定义自变量
	void    ChineseNewYear(int nYear);//每年春节的位置
 	void    IdentifiyOutliers(CDoubleMatrix variables_mx);  //自动识别异常值
	void    PreAdjust(CDoubleVector OrgVec);
	void    Predict(CDoubleVector OrgVec, int mp, int mq, int md, int length);  //预测
	BOOL    Nonx11(CDoubleVector Org_vec);
	CDoubleVector   SeasonMove(CDoubleVector SI_vec, int method);   
	CDoubleVector TrendMove(CDoubleVector SmoothedCT, int nMethod);
	void    WeightCalcu(int nSpan);
	int     TrendMethod(CDoubleVector TCI_vec);
	void    DrawResult(); //输出结果
	void    Diagnoses();


private://用户设置参数
	int m_nWeekMethod;	    //交易日影响 0-无影响，1-星期影响，2-工作日与否的影响
	int m_nHoliday;			//节假日影响 0-无影响，1-有节假日  2-有春节影响
	int m_nYear;			//起始时间，年
	int m_nMonth;			//起始 月
	int m_nMethod;			//加法还是乘法，0－乘法  1－加法
	TIntStruct m_tVarInfo;	//变量下标
	int m_nAO;				//用户定义的AO的个数
	CIntMatrix m_AOMx;		//用户定义的AO的位置矩阵
	int m_nLS;				//用户定义的LS的个数
	CIntMatrix m_LSMx;		//用户定义的LS的位置矩阵
	double m_threshold;		//奇异值阈值
	int m_AONumber;
	int m_LSNumber;
	int m_ndifference;        //差分阶数
    BOOL m_bpredict;         //是否输出预测结果
	int m_prestep;           //预测步数
    

	//编程方便定义的变量
	CResult *m_pResult;
	CDataInterface *m_pDataInterface;
	CRsltElementText *m_pRsltElementText;
	CRsltElementText * m_pWarningTextRslt;//报错信息
    CRsltVariable * m_pRsltVariable;
	CDoubleVector	m_OrgVec; //X12数据
	CDoubleVector   m_extendOrgVec; //ARIMA两端延伸的序列
	CDoubleMatrix  m_WeekDescrib_mx;   //每月有周一周二……
	CDoubleVector  m_DaysinMonth_vec;  //每月有多少天
	CDoubleMatrix  m_ForecastWeekDescrib_mx;  //向前预测的星期结构
	CDoubleMatrix  m_BackcastWeekDescrib_mx;  //向后预测的星期结构
	CDoubleVector  m_CasetDaysinMonth_vec;    //预测数据每月有多少天
	CIntVector     m_PreOutliers_mx;   //指定异常值的相对位置
	CDoubleMatrix  m_constant_mx;         //常数项
	CDoubleMatrix  m_trend_mx;        //趋势变量
	CDoubleMatrix  m_leapYear_mx;     //闰年变量
	CDoubleMatrix  m_Season_mx;       //季节因素变量
	CDoubleMatrix  m_AO_mx;           //指定AO变量
	CDoubleMatrix  m_LS_mx;           //指定LS变量
	CDoubleMatrix  m_workday_mx;       //工作日结构变量(工作日|非工作日)
	CDoubleMatrix  m_NewYearID_mx;    //每年春节所在位置
	CDoubleMatrix  m_ExtendNewYear_mx;  //延伸新年位置
    CDoubleMatrix  m_NewYear_mx;      //春节变量
    CDoubleMatrix  m_Holiday_mx;      //节假日变量
	CDoubleMatrix  m_allvariables_mx;	// 定义自变量：常数|闰年（m_leapYear_vec）|季节因素（m_Season_mx）|指定异常值（）|交易日（无/星期结构（m_weekDescrib_mx）/工作日（m_workday_mx））|春节（无/m_NewYear_mx）|节假日（无/m_Holiday_mx(春节|元旦|五一|十一)）；
	CDoubleMatrix  m_Forecastvariables_mx;   //向前延伸的变量
    CDoubleMatrix  m_Backcastvariables_mx;   //向后延伸的变量
	CDoubleVector  m_allBeta_vec;	//自变量系数
	CDoubleVector  m_AdjustVec;     //预调整后的序列
	
	CDoubleVector m_CT_SmoothedVec;
	CDoubleVector m_I_FinalVec;
	CDoubleVector m_S_SmoothedVec;
	CDoubleVector m_Dr_FinalVec;
	CDoubleVector m_Beta_Vec;
	CDoubleMatrix m_Coff_mx;
	//
	CDoubleVector  m_TrendVec;
	CDoubleVector  m_SeasonVec;
	CDoubleVector  m_IrregularVec;
	CDoubleVector m_TCI_Vec;
	CDoubleVector  m_Weight_Vec;   
	CDoubleVector  m_RatioOrg_Vec;
	CDoubleVector  m_RatioTCI_Vec;

	//
	CDoubleVector m_AveOrg_Vec;
	CDoubleVector m_AveTCI_Vec;
	CDoubleVector m_AveTrend_Vec;
	CDoubleVector m_AveSeason_Vec;
	CDoubleVector m_AveIrregular_Vec;
    CDoubleVector m_predict_Vec;



};

#endif // !defined(AFX_TIMEX12_H__3D80476B_675F_4757_A883_D1A27A40B741__INCLUDED_)
