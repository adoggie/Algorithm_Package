// TChartBoxplot.h: interface for the CTChartBoxplot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCHARTBOXPLOT_H__17391B40_61D5_49D4_9A75_025B21015537__INCLUDED_)
#define AFX_TCHARTBOXPLOT_H__17391B40_61D5_49D4_9A75_025B21015537__INCLUDED_
#pragma  warning(disable:4251)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Public/TString.h"
#include "TChart1.h"

const int DISP_PERCENT = 7;//显示百分位数
const char SEP_RISER[] = _T("|");//分割符
       
class CDoubleVector;
class CIntVector;
class AFX_EXT_CLASS CTChartBoxplot : public CTChart  
{
public:
	CTChartBoxplot();
	virtual ~CTChartBoxplot();
	void	SetBoxplot(void *pValue,int nCount,EDataType DataType,CTString strLengend="");
public:
	void SetPercent(double fPercent[DISP_PERCENT][2]);
	void SetParam(BOOL bWeight, BOOL bSorted, BOOL bCalcPercent);
	void CalcBoxPlot(CDoubleVector &v, void **pData, int &nCount);
	void CalcPercent(CDoubleVector &v, CDoubleVector &w, CIntVector &vecInd);
private:
	BOOL m_bWeight;
	BOOL m_bSorted;     //数据是否已经排序
	BOOL m_bCalcPercent;//是否已计算
	double m_fPercentile[DISP_PERCENT][2];//百分位数
};
#pragma  warning(default:4251)

#endif // !defined(AFX_TCHARTBOXPLOT_H__17391B40_61D5_49D4_9A75_025B21015537__INCLUDED_)
