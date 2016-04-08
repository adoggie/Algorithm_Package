// TChartClustering.h: interface for the CTChartClustering class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCHARTCLUSTERING_H__52AECEE6_9211_4652_B536_1EA5A7FC3D96__INCLUDED_)
#define AFX_TCHARTCLUSTERING_H__52AECEE6_9211_4652_B536_1EA5A7FC3D96__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Public/TString.h"

#include "TChartScatterPlot.h"

class AFX_EXT_CLASS CTChartClustering : public CTChartScatterPlot  
{
public:
	CTChartClustering();
	~CTChartClustering();
	CTChartClustering(void* pDPFB,int nCount,int nIDGraph,CTString strTitle,CTString strXAxisLabel,CTString strYAxisLabel,int nDataType=0);
};

#endif // !defined(AFX_TCHARTCLUSTERING_H__52AECEE6_9211_4652_B536_1EA5A7FC3D96__INCLUDED_)
