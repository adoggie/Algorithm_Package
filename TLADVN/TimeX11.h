// TimeX11.h: interface for the CTimeX11 class.
//                      X_11季节调整
//                    标注： 余卫军
//					  时间： 2005年10月20日
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TIMEX11_H__E37A74FE_64A8_4B00_86CF_75F3BFC08203__INCLUDED_)
#define AFX_TIMEX11_H__E37A74FE_64A8_4B00_86CF_75F3BFC08203__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AlgorithmBase.h"



class AFX_EXT_CLASS CTimeX11 : public CAlgorithmBase  
{public:
	CTimeX11();
	virtual ~CTimeX11();
	CResult * CTimeX11::OnTimeX11(CDataInterface *pDataInterface, string VO);
	int m_nMove;			//平滑的方法：12×2 或是3×3
	int m_nYear;			//起始时间，年
	int m_nMonth;			//起始 月
	int m_nMethod;			//加法还是乘法，0－乘法  1－加法
	TIntStruct m_tVarInfo;	//变量下标

private:
	CDataInterface * m_pDataInterface;
	CResult		   * m_pResult;
	CRsltElementText *m_pRsltElementText;
    CRsltVariable * m_pRsltVariable;
	BOOL	GetValueFromVo(CDataInterface * pDataInterface, string VO);
	void	WeekCalcu(int year, int month);//计算序列每个月有多少天，有多少个周一周二……，放入m_DaysinMonth_vec和m_WeekDescrib_mx
	BOOL	Non11(CDoubleVector& Org_vec, int nMove, int Method);//主要的计算函数
	void	DrawResult(BOOL TOut, int VarInfo);//画图
	int		DaysInMonth(int year, int month);//各个月有多少天
	CDoubleVector Mov_ave(CDoubleVector& SmoothVec, int nMove);
	void	AdjustI(CDoubleVector& Ivec);
//为编程方便设置的变量
	int m_nRow;				//观察数
	CDoubleVector m_DaysinMonth_vec;//个月有多少天，
	CDoubleMatrix m_WeekDescrib_mx;//个月有周一周二……
	CDoubleVector m_CT_SmoothedVec;//最终得到的CT，趋势
	CDoubleVector m_S_SmoothedVec;//最终得到的S，季节
	CDoubleVector m_Dr_FinalVec;//最终得到的Dr，星期
	CDoubleVector m_I_FinalVec;//最终得到的I，偏差
	CDoubleVector m_Justed_vec;//调整后的序列
	CDoubleVector m_Org_vec;//原始的序列
};

#endif // !defined(AFX_TIMEX11_H__E37A74FE_64A8_4B00_86CF_75F3BFC08203__INCLUDED_)
