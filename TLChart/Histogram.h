// Histogram.h: interface for the CHistogram class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HISTOGRAM_H__2E752F0B_89B9_40FB_90E5_AF738E72F996__INCLUDED_)
#define AFX_HISTOGRAM_H__2E752F0B_89B9_40FB_90E5_AF738E72F996__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Chart.h"

class CHistogram : public CChart  
{
public:
	CHistogram();
	virtual ~CHistogram();
	
public:
	virtual CResult *OnChart(CDataInterface *pDataInterface, string VO);
private:
	virtual bool Verify();
	virtual bool ParseTypeMode(string VO);
	virtual bool ParseParam(string VO); //�������еĲ���
	virtual bool GetValueFromVo(string VO);
private:
	bool CalcHistogram();
	bool CalcHistogram(CDoubleVector &v, CTString szXAxisLabel);//����
private:
	int m_nDivNum; //�ֿ���
	bool m_bNormal;//�Ƿ���Ҫ��̬����
	int  m_nMaxGroup;//����������
};

#endif // !defined(AFX_HISTOGRAM_H__2E752F0B_89B9_40FB_90E5_AF738E72F996__INCLUDED_)
