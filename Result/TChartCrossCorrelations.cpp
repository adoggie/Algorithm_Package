// TChartCrossCorrelations.cpp: implementation of the CTChartCrossCorrelations class.
//
//////////////////////////////////////////////////////////////////////

#include "TChartCrossCorrelations.h"

/*
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTChartCrossCorrelations::CTChartCrossCorrelations()
{
		m_ChartKind=CTChart::Crosscorrelations;		
}

CTChartCrossCorrelations::~CTChartCrossCorrelations()
{

}

CTChartCrossCorrelations::CTChartCrossCorrelations(int nGraphStyle, void* pDPFB,int nCount,int nIDGraph,EDataType DataType,CTString strTitle,CTString strXAxisLabel,CTString strYAxisLabel)
{
	m_ChartKind = Crosscorrelations;	
	m_strTitle  = strTitle;
	m_strXAxisLabel = strXAxisLabel;
	m_strYAxisLabel = strYAxisLabel;
	SetData(pDPFB,nCount,EHistogram,DataType);
}

void  CTChartCrossCorrelations::SetCrossCorrelations(void *pDPFB,int nCount, EDataType DataType, CTString strLengend)
{
	SetData(pDPFB,nCount,EHistogram,DataType,strLengend);
}
