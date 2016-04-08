// CrossCorrelation.h: interface for the CCrossCorrelation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CROSSCORRELATION_H__46DB89DC_927F_4101_913F_55DB2D1F6FC8__INCLUDED_)
#define AFX_CROSSCORRELATION_H__46DB89DC_927F_4101_913F_55DB2D1F6FC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Correlation.h"

class CCrossCorrelation : public CCorrelation  
{
public:
	CCrossCorrelation();
	virtual ~CCrossCorrelation();
	
public:
	virtual CResult *OnChart(CDataInterface *pDataInterface, string VO);
private:
	bool CalcCorrelation();
	void CalcCorrelation(CDoubleVector &v1, CDoubleVector &v2, TDataPointValVal *pDPVV);
};

#endif // !defined(AFX_CROSSCORRELATION_H__46DB89DC_927F_4101_913F_55DB2D1F6FC8__INCLUDED_)
