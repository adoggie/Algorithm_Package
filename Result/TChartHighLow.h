// TChartHighLow.h: interface for the CTChartHighLow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCHARTHIGHLOW_H__F74BBEE3_F0A2_4D14_9453_0D885C4B0D72__INCLUDED_)
#define AFX_TCHARTHIGHLOW_H__F74BBEE3_F0A2_4D14_9453_0D885C4B0D72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Public/TString.h"

#include "TChart1.h"

class AFX_EXT_CLASS CTChartHighLow : public CTChart  
{
public:
	CTChartHighLow();
	virtual ~CTChartHighLow();
	CTChartHighLow(void* pDPFB,int nCount,int nIDGraph,EDataType DataType,double nYMax,int nSize,CTString* pStrHLC,int *pGroupCount,int nLineRepresent,CTString strTitle,CTString strXAxisLabel="",CTString strYAxisLabel="",bool bWeight=false);
	CTChartHighLow(double nYMin, void* pDPFB,int nCount,int nIDGraph,EDataType DataType,double nYMax,CTString* pStrLegend,int nSize,
		CTString strTitle,CTString strXAxisLabel,CTString strYAxisLabel,bool bWeight=false);
};

#endif // !defined(AFX_TCHARTHIGHLOW_H__F74BBEE3_F0A2_4D14_9453_0D885C4B0D72__INCLUDED_)
