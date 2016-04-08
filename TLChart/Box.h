// Box.h: interface for the CBox class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOX_H__1BC65ECF_55D9_4816_9E63_FC50089AFF12__INCLUDED_)
#define AFX_BOX_H__1BC65ECF_55D9_4816_9E63_FC50089AFF12__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Chart.h"

class CBox : public CChart  
{
public:
	CBox();
	virtual ~CBox();
public:
	virtual CResult *OnChart(CDataInterface *pDataInterface, string VO);
private:
	virtual bool Verify();
	virtual bool ParseTypeMode(string VO);
	virtual bool GetValueFromVo(string VO);
private:
	bool CalcBox1();//变量分类
	bool CalcBox2();//变量
};

#endif // !defined(AFX_BOX_H__1BC65ECF_55D9_4816_9E63_FC50089AFF12__INCLUDED_)
