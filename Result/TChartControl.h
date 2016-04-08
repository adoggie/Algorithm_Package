// TChartControl.h: interface for the CTChartControl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCHARTCONTROL_H__7D80E671_C42C_4993_8F6E_79D46C8B1D1A__INCLUDED_)
#define AFX_TCHARTCONTROL_H__7D80E671_C42C_4993_8F6E_79D46C8B1D1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Public/TString.h"

#include "TChartLine.h"

class AFX_EXT_CLASS CTChartControl : public CTChartLine  
{
public:
	CTChartControl();
	virtual ~CTChartControl();
	CTChartControl(int nCount, int nIDGraph, EDataType DataType, CTString strTitle, CTString strXAxisLabel, CTString strYAxisLabel,bool flag=true);
	CTChartControl(void *pDPFB, int nCount, int nIDGraph, EDataType DataType, CTString strTitle, CTString strXAxisLabel, CTString strYAxisLabel);
};

#endif // !defined(AFX_TCHARTCONTROL_H__7D80E671_C42C_4993_8F6E_79D46C8B1D1A__INCLUDED_)
