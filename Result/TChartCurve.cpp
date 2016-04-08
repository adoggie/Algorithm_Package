// TChartCurve.cpp: implementation of the CTChartCurve class.
//
//////////////////////////////////////////////////////////////////////

#include "TChartCurve.h"
#include <math.h>
#include "../Public/TString.h"
#include "TGraphData.h"

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
#define STEP	0.01
#define NUM		99

CTChartCurve::CTChartCurve()
{	
	SetChartType(CTChart::Scatter);

}

CTChartCurve::~CTChartCurve()
{
}

CTChartCurve::CTChartCurve(int nGroup,void* pData,CTString* strLegend,CTString strXLabel,CTString strYLabel,bool bFlag)
{
	m_ChartKind= Scatter;
}
