// Scatter.h: interface for the CScatter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCATTER_H__7A52C72B_7E5D_4E91_9769_825DBBA668F5__INCLUDED_)
#define AFX_SCATTER_H__7A52C72B_7E5D_4E91_9769_825DBBA668F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Chart.h"

class CScatter : public CChart  
{
public:
	CScatter();
	virtual ~CScatter();	
public:
	virtual CResult *OnChart(CDataInterface *pDataInterface, string VO);
private:
	virtual bool Verify();
	virtual bool ParseTypeMode(string VO);
	virtual bool GetValueFromVo(string VO);
private:
	bool CalcScatter1();
	bool CalcScatter2();
};

#endif // !defined(AFX_SCATTER_H__7A52C72B_7E5D_4E91_9769_825DBBA668F5__INCLUDED_)
