// TChartArea.h: interface for the CTChartArea class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCHARTAREA_H__1F256FA2_243F_4658_97F9_E8EF6FF87F89__INCLUDED_)
#define AFX_TCHARTAREA_H__1F256FA2_243F_4658_97F9_E8EF6FF87F89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TChart1.h"
// #include "TString.h"


class AFX_EXT_CLASS CTChartArea : public CTChart  
{
public:
	CTChartArea();
	virtual ~CTChartArea();
	CTChartArea(void* pDPFB,int nCount,EDataType DataType,CTString strTitle,CTString strXAxisLabel="",CTString strYAxisLabel="");
	CTChartArea(int nGraphStyle, void* pDPFB,int nCount,int nIDGraph,EDataType DataType,CTString strTitle,CTString strXAxisLabel="",CTString strYAxisLabel="");
	void SetArea(void* pDPFB,int nCount,EDataType DataType,CTString strLengend="");
};


#endif // !defined(AFX_TCHARTAREA_H__1F256FA2_243F_4658_97F9_E8EF6FF87F89__INCLUDED_)
