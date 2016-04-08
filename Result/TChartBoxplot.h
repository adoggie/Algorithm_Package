// TChartBoxplot.h: interface for the CTChartBoxplot class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCHARTBOXPLOT_H__17391B40_61D5_49D4_9A75_025B21015537__INCLUDED_)
#define AFX_TCHARTBOXPLOT_H__17391B40_61D5_49D4_9A75_025B21015537__INCLUDED_
#pragma  warning(disable:4251)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../Public/TString.h"
#include "TChart1.h"

const int DISP_PERCENT = 7;//��ʾ�ٷ�λ��
const char SEP_RISER[] = _T("|");//�ָ��
       
class CDoubleVector;
class CIntVector;
class AFX_EXT_CLASS CTChartBoxplot : public CTChart  
{
public:
	CTChartBoxplot();
	virtual ~CTChartBoxplot();
	void	SetBoxplot(void *pValue,int nCount,EDataType DataType,CTString strLengend="");
public:
	void SetPercent(double fPercent[DISP_PERCENT][2]);
	void SetParam(BOOL bWeight, BOOL bSorted, BOOL bCalcPercent);
	void CalcBoxPlot(CDoubleVector &v, void **pData, int &nCount);
	void CalcPercent(CDoubleVector &v, CDoubleVector &w, CIntVector &vecInd);
private:
	BOOL m_bWeight;
	BOOL m_bSorted;     //�����Ƿ��Ѿ�����
	BOOL m_bCalcPercent;//�Ƿ��Ѽ���
	double m_fPercentile[DISP_PERCENT][2];//�ٷ�λ��
};
#pragma  warning(default:4251)

#endif // !defined(AFX_TCHARTBOXPLOT_H__17391B40_61D5_49D4_9A75_025B21015537__INCLUDED_)
