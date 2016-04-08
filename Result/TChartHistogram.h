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
	//����X�ἰY�����
	bool CalcNormalCurve(CDoubleVector &v,void **pData,int &nCount,bool bSorted = true);//��̬�ֲ�����
	bool CalcXYAxis(CDoubleVector &v,void **pData, int &nDV, bool bSorted = true);//����������Ƿ��Ѿ���������
private:
	int FindStartIndex(CDoubleVector &vec_x,int nVxInd,double fVtData,int nStep_g,int nStep_b);//���ҵ�һ������Ҫ�������
	int FindEndIndex(CDoubleVector &vec_x,int nVxInd,double fVtData,int nStep_g,int nStep_b);  //�������һ��������Ҫ�������(����Ҫ�����һ������)
protected:
	bool   m_bNormal;
	int	   m_nDV;//�ֿ������ⲿָ��
protected:
	double m_fMean;//��ֵ
	double m_fVar; //����
	double m_fStdv;//��׼��
	double m_fVlen;//��
};
#pragma  warning(default:4251)

#endif // !defined(AFX_TCHARTHISTOGRAM_H__059EE641_A10C_424A_9534_510D4E8A889B__INCLUDED_)
