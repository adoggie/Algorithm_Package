// SelfCorrelation.h: interface for the CAutoCorrelation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SELFCORRELATION_H__383F02D7_A54F_4CD2_B76C_1D86D1B297D1__INCLUDED_)
#define AFX_SELFCORRELATION_H__383F02D7_A54F_4CD2_B76C_1D86D1B297D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Correlation.h"

class CSelfCorrelation : public CCorrelation  
{
public:
	CSelfCorrelation();
	virtual ~CSelfCorrelation();	
public:
	virtual CResult *OnChart(CDataInterface *pDataInterface, string VO);
private:
	bool CalcCorrelation();
	void CalcCorrelation(CDoubleVector v, CDoubleVector &v_s, CDoubleVector &v_p);
};

#endif // !defined(AFX_SelfCORRELATION_H__383F02D7_A54F_4CD2_B76C_1D86D1B297D1__INCLUDED_)
