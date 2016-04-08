// TChartPareto.h: interface for the CTChartPareto class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCHARTPARETO_H__01274BCA_EC41_4502_BA3F_F658A687D135__INCLUDED_)
#define AFX_TCHARTPARETO_H__01274BCA_EC41_4502_BA3F_F658A687D135__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Public/TString.h"

#include "TChart1.h"
class AFX_EXT_CLASS CTChartPareto : public CTChart
{
public:
	CTChartPareto();
	virtual ~CTChartPareto();
	CTChartPareto(int nGraphStyle, void* pDPFB,int nCount,int nIDGraph,EDataType DataType,CTString strTitle,CTString strXAxisLabel="",CTString strYAxisLabel="");
	CTChartPareto(int nGraphStyle,int nCount,int nIDGraph,EDataType DataType,CTString strTitle,CTString strXAxisLabel="",CTString strYAxisLabel="",int nGroupCount=0);
	
};

#endif // !defined(AFX_TCHARTPARETO_H__01274BCA_EC41_4502_BA3F_F658A687D135__INCLUDED_)
