// TChartPP.cpp: implementation of the CTChartPP class.
//
//////////////////////////////////////////////////////////////////////

#include "TChartPP.h"

#define NORMAL_MAP  1
#define DETREND_MAP 2
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTChartPP::CTChartPP()
{
	SetChartType(CTChart::PP);
}

CTChartPP::CTChartPP(int nChartType)
{
	SetChartType(CTChart::PP);
	m_ChartType = nChartType;
}

CTChartPP::~CTChartPP()
{
	
}

void  CTChartPP::SetChartPPType(int nChartType)
{
	m_ChartType = nChartType;
}


CTChartPP::CTChartPP(int nType, void* pDPFB,int nCount,int nIDGraph,CTString strTitle,CTString strXAxisLabel,CTString strYAxisLabel,int nDataType)
{
	m_ChartKind= PP;
}
