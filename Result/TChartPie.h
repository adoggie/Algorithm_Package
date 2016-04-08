// TChartPie.h: interface for the CTChartPie class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCHARTPIE_H__A60CF4C7_842B_45E5_A0A8_9D1A330CE2ED__INCLUDED_)
#define AFX_TCHARTPIE_H__A60CF4C7_842B_45E5_A0A8_9D1A330CE2ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Public/TString.h"

#include "TChart1.h"

class AFX_EXT_CLASS CTChartPie : public CTChart  
{
public:
	CTChartPie();
	virtual ~CTChartPie();
	CTChartPie(void* pDPFB,int nCount,int nIDGraph,EDataType DataType,CTString strTitle,CTString strXAxisLabel="",CTString strYAxisLabel="",CTString* pStrLegendAddons=NULL);

	void SetPie(void* pDPFB,int nCount,EDataType DataType,CTString strLengend="");
};

#endif // !defined(AFX_TCHARTPIE_H__A60CF4C7_842B_45E5_A0A8_9D1A330CE2ED__INCLUDED_)
