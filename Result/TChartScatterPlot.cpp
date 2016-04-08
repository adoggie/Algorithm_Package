// TChartScatterPlot.cpp: implementation of the CTChartScatterPlot class.
//
//////////////////////////////////////////////////////////////////////

#include "TChartScatterPlot.h"
#include "float.h"
#include "TChartArea.h"
#include "TChartClustering.h"
#include  <math.h>

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

CTChartScatterPlot::CTChartScatterPlot()
{
	SetChartType(CTChart::Scatter);
}

CTChartScatterPlot::~CTChartScatterPlot()
{
	
}

void CTChartScatterPlot::SetScatter(void* pDPFB,int nCount, EDataType DataType, CTString strLengend)
{
	SetData(pDPFB,nCount,EScatter,DataType,strLengend);
}