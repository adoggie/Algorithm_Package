// TChartErrBar.cpp: implementation of the CTChartErrBar class.
//
//////////////////////////////////////////////////////////////////////

#include "TChartErrBar.h"
#include "float.h"
#include "TChartArea.h"

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


CTChartErrBar::CTChartErrBar()
{
	SetChartType(CTChart::Boxplot);
}

CTChartErrBar::~CTChartErrBar()
{
	
}

CTChartErrBar::CTChartErrBar(void *pDPFB, int nCount, int nIDGraph, EDataType DataType, CTString strTitle, CTString strXAxisLabel, CTString strYAxisLabel)
{
	m_ChartKind = ErrorBar;
	
}

