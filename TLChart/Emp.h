// Emp.h: interface for the CEmp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EMP_H__A4BAE88E_A26F_478C_9108_C7855B8CDFD6__INCLUDED_)
#define AFX_EMP_H__A4BAE88E_A26F_478C_9108_C7855B8CDFD6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Chart.h"

class CEmp : public CChart  
{
public:
	CEmp();
	virtual ~CEmp();
public:
	virtual CResult *OnChart(CDataInterface *pDataInterface, string VO);
private:
	virtual bool Verify();
	virtual bool ParseTypeMode(string VO);
	virtual bool GetValueFromVo(string VO);
	bool ParseParam(string VO);
private:
	bool Emp();
	void Exp(CDoubleVector &v, int nColIndex);
	void Den(CDoubleVector &v, int nColIndex, int nMethod);
	CDoubleVector Kernal(CDoubleVector &v,double fMin,double fMax, double fDelta);
	CDoubleVector NearestNeigh(CDoubleVector &v, double fMin, double fMax, int nStep);
	CDoubleVector Histogram(CDoubleVector &v, double fMin, double fMax, double fDelta);
private:
	int m_nDivNum; //分块数
	bool m_bExp;   //经验图
	bool m_bDen;   //密度图
private:
	int m_nDistType;//分布类型
};

#endif // !defined(AFX_EMP_H__A4BAE88E_A26F_478C_9108_C7855B8CDFD6__INCLUDED_)

