// TChartCrossCorrelations.h: interface for the CTChartCrossCorrelations class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCHARTCROSSCORRELATIONS_H__5A1EF150_B7A6_41E7_8098_F6FAA51EDA15__INCLUDED_)
#define AFX_TCHARTCROSSCORRELATIONS_H__5A1EF150_B7A6_41E7_8098_F6FAA51EDA15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Public/TString.h"

#include "TChartAutocorrelations.h"

class AFX_EXT_CLASS CTChartCrossCorrelations : public CTChartAutocorrelations  
{
public:
	CTChartCrossCorrelations();
	CTChartCrossCorrelations(int nGraphStyle, void* pDPFB,int nCount,int nIDGraph,EDataType DataType,CTString strTitle,CTString strXAxisLabel="",CTString strYAxisLabel="");
	
	virtual ~CTChartCrossCorrelations();
	void  SetCrossCorrelations(void *pDPFB,int nCount, EDataType DataType, CTString strLengend = "");
public:
};

#endif // !defined(AFX_TCHARTCROSSCORRELATIONS_H__5A1EF150_B7A6_41E7_8098_F6FAA51EDA15__INCLUDED_)
