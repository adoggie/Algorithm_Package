// Line.h: interface for the CLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LINE_H__4F632C23_89DD_4AB4_8CA3_4433452597DF__INCLUDED_)
#define AFX_LINE_H__4F632C23_89DD_4AB4_8CA3_4433452597DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Chart.h"
class CLine : public CChart  
{
public:
	CLine();
	virtual ~CLine();
public:
	virtual CResult *OnChart(CDataInterface *pDataInterface, string VO);
public:
private:
	bool CalcSimpleLine1();//变量分类
	bool CalcSimpleLine2();//变量
	bool CalcSimpleLine3();//变量值
	bool CalcMultipleLine1();
	bool CalcMultipleLine2();
	bool CalcMultipleLine3();
private:
	virtual bool Verify();
	virtual bool ParseTypeMode(string VO);
	virtual bool GetValueFromVo(string VO);
private:
};

#endif // !defined(AFX_LINE_H__4F632C23_89DD_4AB4_8CA3_4433452597DF__INCLUDED_)
