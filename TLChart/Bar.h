// Bar.h: interface for the CBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BAR_H__D769B58E_25D1_495C_8F87_E7E9C48638CE__INCLUDED_)
#define AFX_BAR_H__D769B58E_25D1_495C_8F87_E7E9C48638CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Chart.h"

class CBar : public CChart  
{
public:
	CBar();
	virtual ~CBar();
	
public:
	virtual CResult *OnChart(CDataInterface *pDataInterface, string VO);
private:
	bool CalcSimpleBar1();
	bool CalcSimpleBar2();
	bool CalcSimpleBar3();
	bool CalcGroupBar1();
	bool CalcGroupBar2();
	bool CalcGroupBar3();
	bool CalcSegmentBar1();
	bool CalcSegmentBar2();
	bool CalcSegmentBar3();
	bool CalcBarClustered1(bool bStacked);
	bool CalcBarClustered2(bool bStacked);
	bool CalcBarClustered3(bool bStacked);
private:
	virtual bool Verify();
	virtual bool ParseTypeMode(string VO);
	virtual bool GetValueFromVo(string VO);
private:
	bool GetValForFunc(int nGroupD, int nGroupX, EData eData, int &nDataCntY, TDataSet *pDataSetY, int &nRowCntY, TGroupInfor *pGroupInforD, TDataSet *pDataSetYGroup, TDataSet *pDataSetXNoSort, TGroupInfor* pGroupInforX, CStatistics& statistics, double &fValTempY, CTString& szYAxisLabel, bool bString);
private:
	bool m_bStacked;
};

#endif // !defined(AFX_BAR_H__D769B58E_25D1_495C_8F87_E7E9C48638CE__INCLUDED_)
