// TChartScatterPlot.h: interface for the CTChartScatterPlot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCHARTSCATTERPLOT_H__4056C35A_092C_4715_B33C_046A5992F7BA__INCLUDED_)
#define AFX_TCHARTSCATTERPLOT_H__4056C35A_092C_4715_B33C_046A5992F7BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Public/TString.h"

#include "TChart1.h"

class AFX_EXT_CLASS CTChartScatterPlot : public CTChart  
{
public:
	CTChartScatterPlot();
	virtual ~CTChartScatterPlot();

	void SetScatter(void* pDPFB,int nCount, EDataType DataType, CTString strLengend="");
};

#endif // !defined(AFX_TCHARTSCATTERPLOT_H__4056C35A_092C_4715_B33C_046A5992F7BA__INCLUDED_)
