// Evaluate.h: interface for the CEvaluate class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVALUATE_H__A47A8385_C8DF_4716_BBAB_596B74B92A3C__INCLUDED_)
#define AFX_EVALUATE_H__A47A8385_C8DF_4716_BBAB_596B74B92A3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DecTree.h"

class AFX_EXT_CLASS CEvaluate  
{
public:
	CEvaluate(CDecTree *pDecTree);
	virtual ~CEvaluate();

	void EV(CDoubleMatrix &m, CResult *pResult);
	//�Ƿ��ڸ����ķ�Χ��
	bool In(int nValue);
private:
	//��ǩ
	CTString GetLabel(int nValue);
	//����-����Ŀ�������Ƶ��
	void CalcFrequence(int nValue, CTArray<int,int>&arrFreq);
	CTArray<int,int> CalcFrequence(CDoubleVector &v); 
	//����-����Ŀ�������Ԥ�������Ĳ����
	void CalcEvalue(CDoubleMatrix &m, CTArray<int,int>arrFreq, CDoubleMatrix &me);
	//����GAIN,LIFTͼ��ֵ
	void CalcChart(CDoubleMatrix &m, CPtrArray *pArr);
	//����׼ȷ�ʣ������ʣ���ȷ��
	void CalcText(CDoubleMatrix &m, CTArray<int,int>arrFreq, CDoubleVector &v);
	//�������Ŷȷ�Χ
	void CalcConfidence(double fValue, double fConfidence, CPtrArray *pArr);
	//�ͷ��ڴ�
	void Release(CPtrArray *pArr);
private:
	double    m_fValCnt;  //ʵ���ڷ�Χ�ڵ�����
	CDecTree *m_pDecTree;
private:
	class CEv
	{
	public:
		CEv()
		{
			fX = 0;
			fY = 0;
			fConfidence = 0;
		};
	public:
		double fY;      //Y-ת��ΪGainֵ
		double fX;      //X-ת��Ϊ�ٷ�λ��
		double fConfidence;  //���Ŷ�
	};
};

#endif // !defined(AFX_EVALUATE_H__A47A8385_C8DF_4716_BBAB_596B74B92A3C__INCLUDED_)
