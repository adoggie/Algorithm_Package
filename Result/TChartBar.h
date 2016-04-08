// TChartBar.h: interface for the CTChartBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCHARTBAR_H__3C9C7FB0_8CD4_4FA9_8BE4_F16BFE831B4D__INCLUDED_)
#define AFX_TCHARTBAR_H__3C9C7FB0_8CD4_4FA9_8BE4_F16BFE831B4D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "TChart1.h"


class AFX_EXT_CLASS CTChartBar : public CTChart  
{
public:
	CTChartBar();
	virtual ~CTChartBar();
	CTChartBar(int nGraphStyle, void* pDPFB,int nCount,int nIDGraph,EDataType DataType,CTString strTitle,CTString strXAxisLabel="",CTString strYAxisLabel="");
	void SetBar(void* pDPFB,int nCount,EDataType DataType,CTString strLengend="");
};

#endif // !defined(AFX_TCHARTBAR_H__3C9C7FB0_8CD4_4FA9_8BE4_F16BFE831B4D__INCLUDED_)
