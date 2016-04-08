// TChartControl.cpp: implementation of the CTChartControl class.
//
//////////////////////////////////////////////////////////////////////

#include "TChartControl.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTChartControl::CTChartControl()
{
	SetChartType(CTChart::Control);
}

CTChartControl::~CTChartControl()
{

}
CTChartControl::CTChartControl(void *pDPFB, int nCount, int nIDGraph, EDataType DataType, CTString strTitle, CTString strXAxisLabel, CTString strYAxisLabel)
{
    m_ChartKind = Control;
}

CTChartControl::CTChartControl(int nCount, int nIDGraph, EDataType DataType, CTString strTitle, CTString strXAxisLabel, CTString strYAxisLabel,bool flag)
{
	m_ChartKind = Control;
}
