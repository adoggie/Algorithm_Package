// TChartHighLow.cpp: implementation of the CTChartHighLow class.
//
//////////////////////////////////////////////////////////////////////

#include "TChartHighLow.h"
#include "TChartArea.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTChartHighLow::CTChartHighLow()
{
	SetChartType(CTChart::HighLow);
}

CTChartHighLow::~CTChartHighLow()
{
}

CTChartHighLow::CTChartHighLow(void* pDPFB,int nCount,int nIDGraph,EDataType DataType,double nYMax,int nSize,CTString* pStrHLC,int* pGroupCount,int nLineRepresent,
							   CTString strTitle,CTString strXAxisLabel,CTString strYAxisLabel,bool bWeight)
{
}
CTChartHighLow::CTChartHighLow(double nYMin, void* pDPFB,int nCount,int nIDGraph,EDataType DataType,double nYMax,CTString* pStrLegend,int nSize,
							   CTString strTitle,CTString strXAxisLabel,CTString strYAxisLabel,bool bWeight)
{
}
