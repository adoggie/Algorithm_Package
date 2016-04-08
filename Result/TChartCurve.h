// TChartCurve.h: interface for the CTChartCurve class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCHARTCURVE_H__0EDB8244_BAE7_45CE_A999_08628399E1AB__INCLUDED_)
#define AFX_TCHARTCURVE_H__0EDB8244_BAE7_45CE_A999_08628399E1AB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Public/TString.h"

#include "TChart1.h"

class AFX_EXT_CLASS CTChartCurve : public CTChart  
{
public:
	CTChartCurve();
	~CTChartCurve();
	CTChartCurve(int nGroup,void* pData,CTString* strLegend,CTString strXLabel="",CTString strYLabel="",bool bFlag = false);
};

#endif // !defined(AFX_TCHARTCURVE_H__0EDB8244_BAE7_45CE_A999_08628399E1AB__INCLUDED_)
