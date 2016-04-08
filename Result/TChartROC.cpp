// TChartROC.cpp: implementation of the CTChartROC class.
//
//////////////////////////////////////////////////////////////////////

#include "TChartROC.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTChartROC::CTChartROC()
{
	SetChartType(CTChart::ROC);
}

CTChartROC::~CTChartROC()
{

}

CTChartROC::CTChartROC(int nCount, int nIDGraph, EDataType DataType, CTString strTitle, CTString strXAxisLabel, CTString strYAxisLabel)
{
	m_ChartKind = ROC;
	

}

