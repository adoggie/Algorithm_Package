// TChartAutocorrelations.h: interface for the CTChartAutocorrelations class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCHARTAUTOCORRELATIONS_H__43889AAF_BB6C_4199_81DA_8C1174B07364__INCLUDED_)
#define AFX_TCHARTAUTOCORRELATIONS_H__43889AAF_BB6C_4199_81DA_8C1174B07364__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Public/TString.h"

#include "TChart1.h"

class AFX_EXT_CLASS CTChartAutocorrelations : public CTChart  
{
public:
	CTChartAutocorrelations();
	CTChartAutocorrelations(int nGraphStyle, void* pDPFB,int nCount,int nIDGraph,EDataType DataType,CTString strTitle,CTString strXAxisLabel="",CTString strYAxisLabel="");
	virtual ~CTChartAutocorrelations();
	void SetAutoCorrelations(void *pDPFB,int nCount, EDataType DataType, CTString strLengend = "");
};

#endif // !defined(AFX_TCHARTAUTOCORRELATIONS_H__43889AAF_BB6C_4199_81DA_8C1174B07364__INCLUDED_)
