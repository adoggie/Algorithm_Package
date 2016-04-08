// TChartErrBar.h: interface for the CTChartErrBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCHARTERRBAR_H__989D1445_17B5_45A9_9136_75069CCE52C4__INCLUDED_)
#define AFX_TCHARTERRBAR_H__989D1445_17B5_45A9_9136_75069CCE52C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Public/TString.h"

#include "TChartBoxplot.h"

class AFX_EXT_CLASS CTChartErrBar : public CTChartBoxplot  
{
public:
	CTChartErrBar();
	virtual ~CTChartErrBar();
	CTChartErrBar(void* pDPFB,int nCount,int nIDGraph,EDataType DataType,CTString strTitle,CTString strXAxisLabel="",CTString strYAxisLabel="");
	
};

#endif // !defined(AFX_TCHARTERRBAR_H__989D1445_17B5_45A9_9136_75069CCE52C4__INCLUDED_)
