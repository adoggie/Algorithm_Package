// Pie.h: interface for the CPie class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PIE_H__E149375A_C821_4990_82CE_D8E92F230A48__INCLUDED_)
#define AFX_PIE_H__E149375A_C821_4990_82CE_D8E92F230A48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Chart.h"

class CPie : public CChart  
{
public:
	CPie();
	virtual ~CPie();
public:
	virtual CResult *OnChart(CDataInterface *pDataInterface, string VO);
private:
	virtual bool Verify();
	virtual bool ParseTypeMode(string VO);
	virtual bool GetValueFromVo(string VO);
private:
	bool CalcPie1();//变量分类
	bool CalcPie2();//变量
	bool CalcPie3();//变量值
};

#endif // !defined(AFX_PIE_H__E149375A_C821_4990_82CE_D8E92F230A48__INCLUDED_)
