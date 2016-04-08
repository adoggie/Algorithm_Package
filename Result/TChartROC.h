// TChartROC.h: interface for the CTChartROC class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCHARTROC_H__25F4A3F5_7D7A_4399_B2EA_A312B1BD2BCA__INCLUDED_)
#define AFX_TCHARTROC_H__25F4A3F5_7D7A_4399_B2EA_A312B1BD2BCA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Public/TString.h"

#include "TChartLine.h"

class AFX_EXT_CLASS CTChartROC : public CTChartLine  
{
public:
	CTChartROC();
	CTChartROC(int nCount,int nIDGraph,EDataType DataType,CTString strTitle,CTString strXAxisLabel="",CTString strYAxisLabel="");
	
	virtual ~CTChartROC();

};

#endif // !defined(AFX_TCHARTROC_H__25F4A3F5_7D7A_4399_B2EA_A312B1BD2BCA__INCLUDED_)
