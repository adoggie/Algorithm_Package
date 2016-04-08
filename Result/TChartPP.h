// TChartPP.h: interface for the CTChartPP class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCHARTPP_H__C02375E2_6C23_4A99_947A_C3C938500FA2__INCLUDED_)
#define AFX_TCHARTPP_H__C02375E2_6C23_4A99_947A_C3C938500FA2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Public/TString.h"

#include "TChartScatterPlot.h"

class AFX_EXT_CLASS CTChartPP : public CTChartScatterPlot  
{
public:
	CTChartPP(int nType, void* pDPFB,int nCount,int nIDGraph,CTString strTitle,CTString strXAxisLabel="",CTString strYAxisLabel="",int nDataType = 0);
	CTChartPP();
	CTChartPP(int nChartType);//识别正态PP图和去势PP图
	virtual ~CTChartPP();
	void  SetChartPPType(int nChartType);
public:
	int m_ChartType;//识别正态PP图和去势PP图
};

#endif // !defined(AFX_TCHARTPP_H__C02375E2_6C23_4A99_947A_C3C938500FA2__INCLUDED_)
