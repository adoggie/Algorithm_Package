// Area.h: interface for the CArea class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AREA_H__7F6C0C7C_0836_44CA_946A_0588930C1283__INCLUDED_)
#define AFX_AREA_H__7F6C0C7C_0836_44CA_946A_0588930C1283__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Chart.h"

class CArea : public CChart  
{
public:
	CArea();
	virtual ~CArea();
	
public:
	virtual CResult *OnChart(CDataInterface *pDataInterface, string VO);
private:
	virtual bool Verify();
	virtual bool ParseTypeMode(string VO);
	virtual bool GetValueFromVo(string VO);
private:
	bool CalcArea1();//变量分类
	bool CalcArea2();//变量
	bool CalcArea3();//变量值
};

#endif // !defined(AFX_AREA_H__7F6C0C7C_0836_44CA_946A_0588930C1283__INCLUDED_)
