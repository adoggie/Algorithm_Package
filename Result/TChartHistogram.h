// TChartHistogram.h: interface for the CTChartHistogram class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCHARTHISTOGRAM_H__059EE641_A10C_424A_9534_510D4E8A889B__INCLUDED_)
#define AFX_TCHARTHISTOGRAM_H__059EE641_A10C_424A_9534_510D4E8A889B__INCLUDED_
#pragma  warning(disable:4251)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//#include "..\public\TString.h"
#include "TChart1.h"
//
class CDoubleVector;
class AFX_EXT_CLASS CTChartHistogram : public CTChart  
{
public:
	CTChartHistogram();
	virtual ~CTChartHistogram();

	void SetHistogram(void* pValue,int nCount,EDataType DataType,CTString strLengend="");
	void SetNoraml(bool bNormal){m_bNormal = bNormal;}
	void SetDiviCount(int nDV){m_nDV = nDV;}
	bool GetNormal(){return m_bNormal;}
	int	 GetDiviCount(){return m_nDV;}
	void GetParam(double &fMean, double &fStdv, double &fVlen){fMean = m_fMean;fStdv = m_fStdv;fVlen = m_fVlen;};
	//计算X轴及Y轴参数
	bool CalcNormalCurve(CDoubleVector &v,void **pData,int &nCount,bool bSorted = true);//正态分布曲线
	bool CalcXYAxis(CDoubleVector &v,void **pData, int &nDV, bool bSorted = true);//输入的数据是否已经排序（升序）
private:
	int FindStartIndex(CDoubleVector &vec_x,int nVxInd,double fVtData,int nStep_g,int nStep_b);//查找第一个符合要求的索引
	int FindEndIndex(CDoubleVector &vec_x,int nVxInd,double fVtData,int nStep_g,int nStep_b);  //查找最后一个不符合要求的索引(符合要求的下一个索引)
protected:
	bool   m_bNormal;
	int	   m_nDV;//分块数，外部指定
protected:
	double m_fMean;//均值
	double m_fVar; //方差
	double m_fStdv;//标准差
	double m_fVlen;//和
};
#pragma  warning(default:4251)

#endif // !defined(AFX_TCHARTHISTOGRAM_H__059EE641_A10C_424A_9534_510D4E8A889B__INCLUDED_)
