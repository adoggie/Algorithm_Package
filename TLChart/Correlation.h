// Correlation.h: interface for the CCorrelation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CORRELATION_H__B8EDDA60_E19B_4417_BA2B_E32EC6E74F83__INCLUDED_)
#define AFX_CORRELATION_H__B8EDDA60_E19B_4417_BA2B_E32EC6E74F83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Chart.h"

class CCorrelation : public CChart  
{
public:
	CCorrelation();
	virtual ~CCorrelation();
	
public:
	virtual CResult *OnChart(CDataInterface *pDataInterface, string VO) = 0;
protected:
	virtual bool Verify();
	virtual bool ParseParam(string VO);
	virtual bool ParseTypeMode(string VO);
	virtual bool GetValueFromVo(string VO);
protected:
	virtual bool IsValidType(int nColIndex);
	virtual bool IsValidData(int nRowCount,int nColIndex1, int nColIndex2=-1);
protected:
	int m_nDiff;    //��ֽ���
	int m_nKappa;   //����ͺ����
	bool m_bSelf;   //�����ͼ
	bool m_bPartial;//ƫ���ͼ
};

#endif // !defined(AFX_CORRELATION_H__B8EDDA60_E19B_4417_BA2B_E32EC6E74F83__INCLUDED_)
